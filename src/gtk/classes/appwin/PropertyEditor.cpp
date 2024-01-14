/**
 * PropertyEditor.hpp - The Sickle object property editor.
 * Copyright (C) 2024 Trevor Last
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

#include "PropertyEditor.hpp"

using namespace Sickle::AppWin;


PropertyEditor::PropertyEditor()
:   Glib::ObjectBase{typeid(PropertyEditor)}
,   _prop_entity{*this, "entity", {}}
,   _store{Gtk::ListStore::create(_columns)}
,   _properties{_store}
{
    property_entity().signal_changed().connect(
        sigc::mem_fun(*this, &PropertyEditor::on_entity_changed));

    _store->set_sort_column(0, Gtk::SortType::SORT_ASCENDING);

    _properties.append_column_editable("Name", _columns.name);
    _properties.append_column_editable("Value", _columns.value);

    auto cr = dynamic_cast<Gtk::CellRendererText *>(
        _properties.get_column_cell_renderer(0));
    cr->signal_edited().connect(
        sigc::mem_fun(*this, &PropertyEditor::on_name_edited), false);
    cr = dynamic_cast<Gtk::CellRendererText *>(
        _properties.get_column_cell_renderer(1));
    cr->signal_edited().connect(
        sigc::mem_fun(*this, &PropertyEditor::on_value_edited));

    _scroll.add(_properties);

    _add_property_button.set_image_from_icon_name("list-add");
    _add_property_button.set_tooltip_text("Add a new property");
    _add_property_button.signal_clicked().connect(
        sigc::mem_fun(*this, &PropertyEditor::_add_property));

    _remove_property_button.set_image_from_icon_name("list-remove");
    _remove_property_button.set_tooltip_text("Remove the selected property");
    _remove_property_button.signal_clicked().connect(
        sigc::mem_fun(*this, &PropertyEditor::_remove_property));

    _buttons_box.add(_add_property_button);
    _buttons_box.add(_remove_property_button);

    _main_box.pack_start(_scroll, Gtk::PackOptions::PACK_EXPAND_WIDGET);
    _main_box.pack_end(_buttons_box, Gtk::PackOptions::PACK_SHRINK);

    _frame.add(_main_box);
    _frame.set_label("Properties");

    add(_frame);
}


void PropertyEditor::set_entity(Editor::EntityRef const &entity)
{
    property_entity().set_value(entity);
}


Sickle::Editor::EntityRef PropertyEditor::get_entity() const
{
    return property_entity().get_value();
}



void PropertyEditor::on_entity_changed()
{
    _store->clear();
    auto entity = get_entity();
    if (!entity)
        return;
    for (auto kv : entity->properties())
    {
        auto row = *_store->append();
        row[_columns.name] = kv.first;
        row[_columns.value] = kv.second;
    }
}


void PropertyEditor::on_name_edited(
    Glib::ustring const &path,
    Glib::ustring const &new_name)
{
    auto it = _store->get_iter(path);
    auto const old_name = it->get_value(_columns.name);
    auto const value = it->get_value(_columns.value);
    auto entity = get_entity();
    if (entity->remove_property(old_name))
        entity->set_property(new_name, value);
}


void PropertyEditor::on_value_edited(
    Glib::ustring const &path,
    Glib::ustring const &new_value)
{
    auto const it = _store->get_iter(path);
    auto const name = it->get_value(_columns.name);
    get_entity()->set_property(name, new_value);
}



void PropertyEditor::_add_property()
{
    if (!get_entity())
        return;

    auto row = *_store->append();
    row[_columns.name] = "<name>";
    row[_columns.value] = "<value>";

    auto entity = get_entity();
    entity->set_property(
        row.get_value(_columns.name),
        row.get_value(_columns.value));
}


void PropertyEditor::_remove_property()
{
    auto sel = _properties.get_selection();
    auto const it = sel->get_selected();
    if (it)
    {
        auto const name = it->get_value(_columns.name);
        auto entity = get_entity();
        if (entity->remove_property(name))
            _store->erase(it);
    }
}
