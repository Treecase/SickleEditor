/**
 * Selection_Lua.cpp - Editor::Selection Lua binding.
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


#define LIBRARY_NAME    "Sickle.editor.selection"


/** Add value at the top of the stack to the objectTable using KEY. */
void add_to_objectTable(lua_State *L, Sickle::Editor::Selection *key)
{
    Lua::get_from_registry(L, LIBRARY_NAME".objectTable");
    lua_pushlightuserdata(L, key);
    lua_pushvalue(L, -3);
    lua_settable(L, -3);
    lua_pop(L, 1);
}

/** Get the Lua value associated with KEY from the objectTable. */
void get_from_objectTable(lua_State *L, Sickle::Editor::Selection *key)
{
    Lua::get_from_registry(L, LIBRARY_NAME".objectTable");
    lua_pushlightuserdata(L, key);
    lua_gettable(L, -2);
    lua_remove(L, -2);
}


////////////////////////////////////////////////////////////////////////////////
// Internal
Sickle::Editor::Selection *checkselection(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, LIBRARY_NAME);
    luaL_argcheck(L, ud != NULL, arg, "`" LIBRARY_NAME "' expected");
    return *static_cast<Sickle::Editor::Selection **>(ud);
}


////////////////////////////////////////////////////////////////////////////////
// Metamethods
int dunder_newindex(lua_State *L)
{
    checkselection(L, 1);
    lua_getiuservalue(L, -3, 1);
    lua_rotate(L, -3, 1);
    lua_settable(L, -3);
    return 0;
}

int dunder_index(lua_State *L)
{
    checkselection(L, 1);
    lua_getiuservalue(L, 1, 1);
    lua_rotate(L, -2, 1);
    lua_gettable(L, -2);
    return 1;
}

luaL_Reg metamethods[] = {
    {"__newindex", dunder_newindex},
    {"__index", dunder_index},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// Methods
int do_nothing(lua_State *L)
{
    return 0;
}

luaL_Reg methods[] = {
    {"on_updated", do_nothing},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// Functions
static luaL_Reg functions[] = {
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// C++ facing
int leditor_new(lua_State *L, Sickle::Editor::Selection *selection)
{
    // Create the Lua object.
    auto ptr = static_cast<Sickle::Editor::Selection const **>(
        lua_newuserdatauv(L, sizeof(Sickle::Editor::Selection *), 1));
    *ptr = selection;

    // Add methods/data table.
    lua_newtable(L);
    luaL_setfuncs(L, methods, 0);
    lua_setiuservalue(L, -2, 1);

    // Set metatable.
    luaL_setmetatable(L, LIBRARY_NAME);

    // Add the object to the Lua registry, using the pointer as key. This is
    // needed for the C++ callbacks to know what object to call methods on.
    add_to_objectTable(L, selection);

    // Connect signals.
    selection->signal_updated().connect(
        [L, selection](){
            get_from_objectTable(L, selection);
            Lua::call_method(L, "on_updated");
        });

    return 1;
}

int luaopen_selection(lua_State *L)
{
    // Table used to map C++ pointers to Lua objects.
    // TODO: References should be removed when the C++ objects are destroyed.
    lua_newtable(L);
    Lua::add_to_registry(L, LIBRARY_NAME".objectTable");
    lua_pop(L, 1);

    luaL_newmetatable(L, LIBRARY_NAME);
    luaL_setfuncs(L, metamethods, 0);
    luaL_newlib(L, functions);

    return 1;
}
