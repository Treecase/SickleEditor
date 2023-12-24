/**
 * StackInspector.cpp - Lua stack inspector widget.
 * Copyright (C) 2023 Trevor Last
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

#include "StackInspector.hpp"

#include <gtkmm/cellrenderercombo.h>

#include <array>

using namespace Sickle;


StackInspector::StackInspector()
:   Glib::ObjectBase{typeid(StackInspector)}
,   Gtk::Bin{}
,   _prop_lua_state{*this, "lua-state", nullptr}
,   _store{Gtk::ListStore::create(_columns)}
,   _view{_store}
{
    set_hexpand(true);
    set_vexpand(true);


    // Create ListStore for 'Type' column CellRendererCombo.
    _combo_store = Gtk::ListStore::create(_combo_columns);

    // Populate the combobox store with all of the Lua typenames.
    static constexpr std::array LUA_TYPES{
        LUA_TBOOLEAN,
        LUA_TFUNCTION,
        LUA_TLIGHTUSERDATA,
        LUA_TNIL,
        LUA_TNUMBER,
        LUA_TSTRING,
        LUA_TTABLE,
        LUA_TTHREAD,
        LUA_TUSERDATA,
    };
    for (auto const type : LUA_TYPES)
    {
        auto row = *_combo_store->append();
        // NOTE: Not sure if calling lua_typename with a null lua_State is
        // really "safe" or "good practice", but it works on my machine...
        row[_combo_columns.types] = lua_typename(nullptr, type);
    }

    // Create the 'Type' column using CellRendererCombo.
    auto type_column = Gtk::make_managed<Gtk::TreeViewColumn>("Type");
    auto renderer = Gtk::make_managed<Gtk::CellRendererCombo>();
    type_column->pack_start(*renderer);
    type_column->add_attribute(renderer->property_text(), _columns.type);
    type_column->add_attribute(renderer->property_model(), _columns.types);
    renderer->property_text_column() = _combo_columns.types.index();
    renderer->property_editable() = true;
    renderer->signal_edited().connect(
        sigc::mem_fun(*this, &StackInspector::_on_type_cell_edited));

    _view.append_column("Index", _columns.index);
    _view.append_column(*type_column);
    _view.append_column("Value", _columns.value);
    add(_view);


    property_lua_state().signal_changed().connect(
        sigc::mem_fun(*this, &StackInspector::on_lua_state_changed));
}


void StackInspector::update()
{
    _update_store();
}


void StackInspector::clear()
{
    _store->clear();
}



void StackInspector::on_lua_state_changed()
{
}



void StackInspector::_on_type_cell_edited(
    Glib::ustring const &path,
    Glib::ustring const &value)
{
    auto it = _store->get_iter(path);
    if (it)
        (*it)[_columns.type] = value;
}


void StackInspector::_update_store()
{
    auto const L = get_lua_state();
    if (!L)
        return;

    _store->clear();

    int const top = lua_gettop(L);
    for (int i = 1; i <= top; ++i)
    {
        auto const value = luaL_tolstring(L, i, nullptr);
        auto row = *_store->append();
        row[_columns.index] = i;
        row[_columns.type] = luaL_typename(L, i);
        row[_columns.types] = _combo_store;
        row[_columns.value] = value;
        lua_pop(L, 1);
    }
}
