/**
 * Outliner.cpp - The Sickle object outliner.
 * Copyright (C) 2023-2024 Trevor Last
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "Outliner.hpp"
#include <appid.hpp>

#include <giomm/menu.h>
#include <giomm/simpleactiongroup.h>

#include <sstream>

using namespace Sickle::AppWin;


/** Defines items to be installed in the Outliner popup menu. */
struct PopupItemDef {
    /// Label to be displayed on the UI. Also used as the name in the action
    /// group (but converted to lowercase).
    Glib::ustring label;
    /// Name of the operation to execute when clicked.
    std::string operation_name;
};


// List of items to install into the Outliner's popup menu.
static std::vector<struct PopupItemDef> popup_items{
    {"Delete", "brush.delete"},
};


// Create a MenuModel for the Outliner's popup menu.
static Glib::RefPtr<Gio::Menu> make_model()
{
    auto model = Gio::Menu::create();
    for (auto const &item : popup_items)
        model->append(item.label, "outliner." + item.label.lowercase());
    return model;
}


Outliner::Outliner()
:   Glib::ObjectBase{typeid(Outliner)}
,   Gtk::Bin{}
,   _prop_editor{*this, "editor", Editor::EditorRef{}}
,   _prop_world{*this, "world", Editor::WorldRef{}}
,   _popup{make_model()}
{
    // Create the action group for the popup menu.
    auto action_group = Gio::SimpleActionGroup::create();
    for (auto const &item : popup_items)
    {
        action_group->add_action(
            item.label.lowercase(),
            sigc::bind(
                sigc::mem_fun(*this, &Outliner::_run_operation),
                item.operation_name));
    }
    insert_action_group("outliner", action_group);

    _tree_view.set_model(_tree_store);
    _tree_view.append_column("Icon", _tree_columns.icon);
    _tree_view.append_column("Label", _tree_columns.text);
    _tree_view.set_headers_visible(false);

    auto sel = _tree_view.get_selection();
    sel->signal_changed().connect(
        sigc::mem_fun(*this, &Outliner::on_selection_changed));

    _popup.attach_to_widget(_tree_view);

    _scrolled.add(_tree_view);

    add(_scrolled);

    property_world().signal_changed().connect(
        sigc::mem_fun(*this, &Outliner::on_world_changed));
    _tree_view.signal_button_press_event().connect_notify(
        sigc::mem_fun(*this, &Outliner::on_button_press_event_notify));
}



void Outliner::on_button_press_event_notify(GdkEventButton *event)
{
    if (event->button == 3)
        _popup.popup(event->button, event->time);
}


void Outliner::on_object_is_selected_changed(
    Gtk::TreeModel::iterator const &iter)
{
    auto sel = _tree_view.get_selection();
    auto obj = iter->get_value(_tree_columns.ptr);
    if (obj->is_selected())
        sel->select(iter);
    else
        sel->unselect(iter);
}


void Outliner::on_selection_changed()
{
    auto sel = _tree_view.get_selection();
    _tree_store->foreach_iter(
        [this, &sel](Gtk::TreeModel::iterator const &iter) -> bool
        {
            iter->get_value(_tree_columns.ptr)->select(sel->is_selected(iter));
            return false;
        });
}


void Outliner::on_world_changed()
{
    _tree_store->clear();

    auto const &w = property_world().get_value();
    for (auto const &e : w->children())
    {
        auto iter = _tree_store->append();
        _add_object(e, iter);
    }
}



void Outliner::_add_object(
    Glib::RefPtr<Sickle::Editor::EditorObject> obj,
    Gtk::TreeModel::iterator const &iter)
{
    obj->property_selected().signal_changed().connect(
        sigc::bind(
            sigc::mem_fun(*this, &Outliner::on_object_is_selected_changed),
            iter));

    obj->signal_child_added().connect(
        [this, iter](auto child){
            auto child_iter = _tree_store->append(iter->children());
            _add_object(child, child_iter);
        });

    obj->signal_removed().connect([this, iter](){_remove_object(iter);});

    auto &row = *iter;
    row[_tree_columns.text] = obj->name();
    row[_tree_columns.icon] = obj->icon();
    row[_tree_columns.ptr] = obj;

    for (auto const &child : obj->children())
    {
        auto child_iter = _tree_store->append(iter->children());
        _add_object(child, child_iter);
    }
}


void Outliner::_remove_object(Gtk::TreeModel::iterator const &iter)
{
    _tree_store->erase(iter);
}


void Outliner::_run_operation(std::string const &operation_name)
{
    auto editor = property_editor().get_value();
    auto op = editor->oploader->get_operation(operation_name);
    op.execute(editor, {});
}
