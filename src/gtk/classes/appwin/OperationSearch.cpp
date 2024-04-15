/**
 * OperationSearch.cpp - Operation Search popup.
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

#include "OperationSearch.hpp"

#include <config/appid.hpp>


using namespace Sickle::AppWin;


OperationSearch *OperationSearch::create(Editor::EditorRef const &editor)
{
    static auto builder = Gtk::Builder::create_from_resource(
        SE_GRESOURCE_PREFIX "gtk/OperationSearch.glade");
    OperationSearch *opsearch{nullptr};
    builder->get_widget_derived("operationSearch", opsearch, editor);
    return opsearch;
}



OperationSearch::OperationSearch()
:   Glib::ObjectBase{"OperationSearch"}
{
}


OperationSearch::OperationSearch(
    BaseObjectType *cobject,
    Glib::RefPtr<Gtk::Builder> const &builder,
    Editor::EditorRef const &editor)
:   Glib::ObjectBase{"OperationSearch"}
,   Gtk::Window{cobject}
,   _editor{editor}
,   _operations{Gtk::ListStore::create(_columns())}
,   _filtered{Gtk::TreeModelFilter::create(_operations)}
,   _sorted{Gtk::TreeModelSort::create(_filtered)}
{
    builder->get_widget("searchbar", _searchbar);
    builder->get_widget("operationview", _treeview);

    _treeview->set_model(_sorted);
    _treeview->append_column("Operation", _columns().name);
    _treeview->set_headers_visible(false);

    _filtered->set_visible_func(
        sigc::mem_fun(*this, &OperationSearch::_filter_visible_func));

    _sorted->set_default_sort_func(
        sigc::mem_fun(*this, &OperationSearch::_operations_sort_func));

    auto const &ops = _editor->oploader->get_operations();
    for (auto const &operation : ops)
        _add_row(operation);


    _editor->oploader->signal_operation_added().connect(
        sigc::mem_fun(*this, &OperationSearch::_on_editor_operation_added));
    _editor->property_mode().signal_changed().connect(
        sigc::mem_fun(*_filtered.get(), &Gtk::TreeModelFilter::refilter));

    _searchbar->signal_search_changed().connect(
        sigc::mem_fun(*this, &OperationSearch::_on_search_changed));

    _treeview->signal_row_activated().connect(
        sigc::mem_fun(*this, &OperationSearch::_on_row_activated));
}


OperationSearch::~OperationSearch()
{
}



bool OperationSearch::on_focus_out_event(GdkEventFocus *gdk_event)
{
    hide();
    return Gtk::Window::on_focus_out_event(gdk_event);
}


void OperationSearch::on_hide()
{
    Gtk::Window::on_hide();
    _searchbar->set_text("");
}


bool OperationSearch::on_key_press_event(GdkEventKey *event)
{
    if (event->keyval == GDK_KEY_Escape)
    {
        hide();
        return true;
    }
    return Gtk::Window::on_key_press_event(event);
}



OperationSearch::Columns &OperationSearch::_columns()
{
    static Columns columns{};
    return columns;
}


size_t OperationSearch::_similarity(
    Glib::ustring const &a, Glib::ustring const &b)
{
    auto const ac = a.casefold();
    auto const bc = b.casefold();
    for (size_t i = 0; i < ac.size() && i < bc.size(); ++i)
        if (ac.at(i) != bc.at(i))
            return i;
    return bc.size();
}


void OperationSearch::_add_row(Editor::Operation const &op)
{
    auto const iter = _operations->append();
    iter->set_value(
        _columns().operation,
        std::make_shared<Editor::Operation>(op));
    iter->set_value<Glib::ustring>(_columns().name, op.id());
}


bool OperationSearch::_filter_visible_func(
    Gtk::TreeModelFilter::iterator const &iter) const
{
    if (!iter)
        return false;

    auto const op = iter->get_value(_columns().operation);
    if (!op)
        return false;

    auto const query = _searchbar->get_text();
    Glib::ustring const name{op->name};
    bool const name_matches = _similarity(name, query) >= query.size();

    auto const mode = _editor->property_mode().get_value();
    bool const correct_mode = (op->mode == mode);

    return name_matches && correct_mode;
}


int OperationSearch::_operations_sort_func(
    Gtk::TreeModel::iterator const &a,
    Gtk::TreeModel::iterator const &b) const
{
    if (!a) return -1;
    if (!b) return 1;

    auto const aptr = a->get_value(_columns().operation);
    auto const bptr = b->get_value(_columns().operation);

    Glib::ustring const au{aptr->name};
    Glib::ustring const bu{bptr->name};

    auto const query = _searchbar->get_text();
    auto const sim_a = _similarity(au, query);
    auto const sim_b = _similarity(bu, query);

    if (sim_a < sim_b)
        return 1;
    else if (sim_a == sim_b)
        return au.casefold().compare(bu.casefold());
    else
        return -1;
}


void OperationSearch::_on_editor_operation_added(std::string const &id)
{
    _add_row(_editor->oploader->get_operation(id));
}


void OperationSearch::_on_row_activated(
    Gtk::TreeModel::Path const &path,
    Gtk::TreeView::Column *column)
{
    auto const path2 = _sorted->convert_path_to_child_path(path);
    auto const path3 = _filtered->convert_path_to_child_path(path2);
    auto const iter = _operations->get_iter(path3);

    auto const operation = iter->get_value(_columns().operation);
    hide();

    signal_operation_chosen().emit(*operation);
}


void OperationSearch::_on_search_changed()
{
    _filtered->refilter();
}
