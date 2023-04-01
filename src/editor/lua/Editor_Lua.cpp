/**
 * Editor_Lua.cpp - Editor Lua binding.
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

#include "editor/Editor.hpp"
#include "Editor_Lua.hpp"

#include <se-lua/lua-utils.hpp>


static Lua::ReferenceManager refman{};


////////////////////////////////////////////////////////////////////////////////
// Methods
static int get_selection(lua_State *L)
{
    auto ed = leditor_check(L, 1);
    Lua::push(L, &ed->selected);
    return 1;
}

static int get_brushbox(lua_State *L)
{
    auto ed = leditor_check(L, 1);
    Lua::push(L, &ed->brushbox);
    return 1;
}

static int do_nothing(lua_State *L)
{
    return 0;
}

static luaL_Reg methods[] = {
    {"get_selection", get_selection},
    {"get_brushbox", get_brushbox},

    {"on_map_changed", do_nothing},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// C++ facing
template<>
void Lua::push(lua_State *L, Sickle::Editor *editor)
{
    refman.get(editor);
    if (!lua_isnil(L, -1))
        return;
    else
        lua_pop(L, 1);

    auto ptr = static_cast<Sickle::Editor const **>(
        lua_newuserdatauv(L, sizeof(Sickle::Editor *), 0));
    *ptr = editor;
    luaL_setmetatable(L, "Sickle.editor");

    editor->signal_map_changed().connect(
        [L, editor](){
            refman.get(editor);
            Lua::call_method(L, "on_map_changed");
        });

    refman.set(editor, -1);
}

Sickle::Editor *leditor_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, "Sickle.editor");
    luaL_argcheck(L, ud != NULL, arg, "`Sickle.editor' expected");
    return *static_cast<Sickle::Editor **>(ud);
}

int luaopen_editor(lua_State *L)
{
    // TODO: References should be removed when the C++ objects are destroyed.
    refman.init(L);

    luaL_newmetatable(L, "Sickle.editor");

    lua_newtable(L);
    luaL_setfuncs(L, methods, 0);
    luaL_requiref(L, "selection", luaopen_selection, 0);
    luaL_requiref(L, "brushbox", luaopen_brushbox, 0);

    lua_setfield(L, -4, "brushbox");
    lua_setfield(L, -3, "selection");
    lua_setfield(L, -2, "__index");

    return 0;
}
