/**
 * Editor_Lua.cpp - Editor Lua binding.
 * Copyright (C) 2022 Trevor Last
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


#define LIBRARY_NAME    "Sickle.editor"
#define CLASSNAME       Sickle::Editor


/** Add value at the top of the stack to the objectTable using KEY. */
static void add_to_objectTable(lua_State *L, CLASSNAME *key)
{
    Lua::get_from_registry(L, LIBRARY_NAME".objectTable");
    lua_pushlightuserdata(L, key);
    lua_pushvalue(L, -3);
    lua_settable(L, -3);
    lua_pop(L, 1);
}

/** Get the Lua value associated with KEY from the objectTable. */
static void get_from_objectTable(lua_State *L, CLASSNAME *key)
{
    Lua::get_from_registry(L, LIBRARY_NAME".objectTable");
    lua_pushlightuserdata(L, key);
    lua_gettable(L, -2);
    lua_remove(L, -2);
}


////////////////////////////////////////////////////////////////////////////////
// Metamethods
static int dunder_newindex(lua_State *L)
{
    leditor_check(L, 1);
    lua_getiuservalue(L, -3, 1);
    lua_rotate(L, -3, 1);
    lua_settable(L, -3);
    return 0;
}

static int dunder_index(lua_State *L)
{
    leditor_check(L, 1);
    lua_getiuservalue(L, 1, 1);
    lua_rotate(L, -2, 1);
    lua_gettable(L, -2);
    return 1;
}

static luaL_Reg metamethods[] = {
    {"__newindex", dunder_newindex},
    {"__index", dunder_index},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// Methods
static int get_selection(lua_State *L)
{
    auto ed = leditor_check(L, 1);
    lselection_new(L, &ed->selected);
    return 1;
}

static int get_brushbox(lua_State *L)
{
    auto ed = leditor_check(L, 1);
    lbrushbox_new(L, &ed->brushbox);
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
int leditor_new(lua_State *L, CLASSNAME *editor)
{
    // If we've already built an object for this pointer, just reuse it.
    get_from_objectTable(L, editor);
    if (!lua_isnil(L, -1))
        return 1;
    else
        lua_pop(L, 1);

    // Create the Lua object.
    auto ptr = static_cast<CLASSNAME const **>(
        lua_newuserdatauv(L, sizeof(CLASSNAME *), 1));
    *ptr = editor;

    // Add methods/data table.
    lua_newtable(L);
    luaL_setfuncs(L, methods, 0);
    lua_setiuservalue(L, -2, 1);

    // Set metatable.
    luaL_setmetatable(L, LIBRARY_NAME);

    // Add the object to the Lua registry, using the pointer as key. This is
    // needed for the C++ callbacks to know what object to call methods on.
    add_to_objectTable(L, editor);

    // Connect signals.
    editor->signal_map_changed().connect(
        [L, editor](){
            get_from_objectTable(L, editor);
            Lua::call_method(L, "on_map_changed");
        });

    return 1;
}

CLASSNAME *leditor_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, LIBRARY_NAME);
    luaL_argcheck(L, ud != NULL, arg, "`" LIBRARY_NAME "' expected");
    return *static_cast<CLASSNAME **>(ud);
}

int luaopen_editor(lua_State *L)
{
    luaL_requiref(L, "selection", luaopen_selection, 1);
    luaL_requiref(L, "brushbox", luaopen_brushbox, 1);

    // Table used to map C++ pointers to Lua objects.
    // TODO: References should be removed when the C++ objects are destroyed.
    lua_newtable(L);
    Lua::add_to_registry(L, LIBRARY_NAME".objectTable");
    lua_pop(L, 1);

    luaL_newmetatable(L, LIBRARY_NAME);
    luaL_setfuncs(L, metamethods, 0);

    return 0;
}
