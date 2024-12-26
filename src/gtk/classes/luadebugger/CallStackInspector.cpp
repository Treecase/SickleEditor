/**
 * CallStackInspector.cpp - Lua function call inspection widget.
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

#include "CallStackInspector.hpp"

using namespace Sickle;

CallStackInspector::CallStackInspector()
: Glib::ObjectBase{typeid(CallStackInspector)}
, _prop_lua_state{*this, "lua-state", nullptr}
, _prop_level{*this, "level", 0}
, _store{Gtk::ListStore::create(_columns)}
, _view{_store}
{
    set_hexpand(true);
    set_vexpand(true);

    _view.append_column("Stack Frame", _columns.name);
    add(_view);

    _view.get_selection()->signal_changed().connect(
        sigc::mem_fun(*this, &CallStackInspector::on_selection_changed));
}

void CallStackInspector::update()
{
    auto const L = get_lua_state();
    if (!L)
    {
        return;
    }

    _store->clear();

    lua_Debug dbg;
    for (int level = 0; lua_getstack(L, level, &dbg); ++level)
    {
        lua_getinfo(L, "nS", &dbg);

        auto row = *_store->append();
        row[_columns.level] = level;
        row[_columns.name]
            = ("[" + std::string{dbg.what} + "] "
               + std::string{dbg.name ? dbg.name : "?"});
    }
}

void CallStackInspector::clear()
{
    _store->clear();
}

void CallStackInspector::on_selection_changed()
{
    auto it = _view.get_selection()->get_selected();
    if (it)
    {
        property_level().set_value(it->get_value(_columns.level));
    }
}
