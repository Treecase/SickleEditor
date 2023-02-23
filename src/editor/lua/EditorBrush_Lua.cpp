/**
 * EditorBrush_Lua.cpp - EditorBrush Lua binding.
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


#define LIBRARY_NAME    "Sickle.editorbrush"
#define CLASSNAME       Sickle::EditorBrush


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
    leditorbrush_check(L, 1);
    lua_getiuservalue(L, -3, 1);
    lua_rotate(L, -3, 1);
    lua_settable(L, -3);
    return 0;
}

static int dunder_index(lua_State *L)
{
    leditorbrush_check(L, 1);
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
static int is_selected(lua_State *L)
{
    auto brush = leditorbrush_check(L, 1);
    lua_pushboolean(L, brush->is_selected.get());
    return 1;
}

static int translate(lua_State *L)
{
    auto brush = leditorbrush_check(L, 1);
    auto x = luaL_checknumber(L, 2);
    auto y = luaL_checknumber(L, 3);
    auto z = luaL_checknumber(L, 4);
    brush->translate({x, y, z});
    return 0;
}

static int do_nothing(lua_State *L)
{
    return 0;
}

static luaL_Reg methods[] = {
    {"is_selected", is_selected},
    {"translate", translate},

    {"on_selected", do_nothing},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// C++ facing
int leditorbrush_new(lua_State *L, CLASSNAME *brush)
{
    // If we've already built an object for this pointer, just reuse it.
    get_from_objectTable(L, brush);
    if (!lua_isnil(L, -1))
        return 1;
    else
        lua_pop(L, 1);

    // Create the Lua object.
    auto ptr = static_cast<CLASSNAME const **>(
        lua_newuserdatauv(L, sizeof(CLASSNAME *), 1));
    *ptr = brush;

    // Add methods/data table.
    lua_newtable(L);
    luaL_setfuncs(L, methods, 0);
    lua_setiuservalue(L, -2, 1);

    // Set metatable.
    luaL_setmetatable(L, LIBRARY_NAME);

    // Add the object to the Lua registry, using the pointer as key. This is
    // needed for the C++ callbacks to know what object to call methods on.
    add_to_objectTable(L, brush);

    // Connect signals.
    brush->is_selected.signal_changed().connect(
        [L, brush](){
            get_from_objectTable(L, brush);
            Lua::call_method(L, "on_selected");
        });

    return 1;
}

CLASSNAME *leditorbrush_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, LIBRARY_NAME);
    luaL_argcheck(L, ud != NULL, arg, "`" LIBRARY_NAME "' expected");
    return *static_cast<CLASSNAME **>(ud);
}

int luaopen_editorbrush(lua_State *L)
{
    // Table used to map C++ pointers to Lua objects.
    // TODO: References should be removed when the C++ objects are destroyed.
    lua_newtable(L);
    Lua::add_to_registry(L, LIBRARY_NAME".objectTable");
    lua_pop(L, 1);

    luaL_newmetatable(L, LIBRARY_NAME);
    luaL_setfuncs(L, metamethods, 0);

    return 0;
}
