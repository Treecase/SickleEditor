/**
 * FunctionInspector.cpp - Lua function call inspection widget.
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

#include "FunctionInspector.hpp"

using namespace Sickle;

FunctionInspector::FunctionInspector()
: Glib::ObjectBase{typeid(FunctionInspector)}
, Gtk::Box{Gtk::Orientation::ORIENTATION_VERTICAL}
, _prop_lua_state{*this, "lua-state", nullptr}
, _prop_level{*this, "level", 0}
, _store{Gtk::TreeStore::create(_columns)}
, _filtered{Gtk::TreeModelFilter::create(_store)}
, _view{_filtered}
{
    set_hexpand(true);
    set_vexpand(true);

    _filtered->set_visible_func(
        [this](Gtk::TreeModel::iterator const it) -> bool
        {
            auto const level = it->get_value(_columns.level);
            return level == property_level().get_value();
        });

    _view.append_column("Variables", _columns.display);
    add(_view);

    property_level().signal_changed().connect(
        sigc::mem_fun(*this, &FunctionInspector::on_level_changed));
}

void FunctionInspector::update()
{
    auto const L = get_lua_state();
    if (!L)
    {
        return;
    }

    _store->clear();

    lua_Debug dbg;
    for (int level = 1; lua_getstack(L, level, &dbg); ++level)
    {
        auto row = *_store->append();
        row[_columns.level] = level;
        row[_columns.display] = "Locals";

        char const *name = nullptr;
        for (int i = 1; (name = lua_getlocal(L, &dbg, i)) != nullptr; ++i)
        {
            auto row2 = *_store->append(row.children());
            row2[_columns.level] = level;
            row2[_columns.display]
                = (std::string{name} + ": "
                   + std::string{luaL_tolstring(L, -1, nullptr)});
            lua_pop(L, 2);
        }
    }
}

void FunctionInspector::clear()
{
    _store->clear();
}

void FunctionInspector::on_level_changed()
{
    _filtered->refilter();
}
