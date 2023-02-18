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


/** Add value at the top of the stack to the objectTable using KEY. */
void add_to_objectTable(lua_State *L, Sickle::Editor *key)
{
    Lua::get_from_registry(L, LIBRARY_NAME".objectTable");
    lua_pushlightuserdata(L, key);
    lua_pushvalue(L, -3);
    lua_settable(L, -3);
    lua_pop(L, 1);
}

/** Get the Lua value associated with KEY from the objectTable. */
void get_from_objectTable(lua_State *L, Sickle::Editor *key)
{
    Lua::get_from_registry(L, LIBRARY_NAME".objectTable");
    lua_pushlightuserdata(L, key);
    lua_gettable(L, -2);
    lua_remove(L, -2);
}


////////////////////////////////////////////////////////////////////////////////
// Internal
Sickle::Editor *leditor_checkeditor(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, LIBRARY_NAME);
    luaL_argcheck(L, ud != NULL, arg, "`" LIBRARY_NAME "' expected");
    return *static_cast<Sickle::Editor **>(ud);
}


////////////////////////////////////////////////////////////////////////////////
// Metamethods
int leditor_dunder_newindex(lua_State *L)
{
    leditor_checkeditor(L, 1);
    lua_getiuservalue(L, -3, 1);
    lua_rotate(L, -3, 1);
    lua_settable(L, -3);
    return 0;
}

int leditor_dunder_index(lua_State *L)
{
    leditor_checkeditor(L, 1);
    lua_getiuservalue(L, 1, 1);
    lua_rotate(L, -2, 1);
    lua_gettable(L, -2);
    return 1;
}

luaL_Reg editorlib_metamethods[] = {
    {"__newindex", leditor_dunder_newindex},
    {"__index", leditor_dunder_index},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// Methods
int _leditor_do_nothing(lua_State *L)
{
    return 0;
}

luaL_Reg editorlib_methods[] = {
    {"on_map_changed", _leditor_do_nothing},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// Functions
static luaL_Reg editorlib_functions[] = {
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// C++ facing
int leditor_new(lua_State *L, Sickle::Editor *editor)
{
    // Create the Lua object.
    auto ptr = static_cast<Sickle::Editor const **>(
        lua_newuserdatauv(L, sizeof(Sickle::Editor *), 1));
    *ptr = editor;

    // Add methods/data table.
    lua_newtable(L);
    luaL_setfuncs(L, editorlib_methods, 0);
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

int luaopen_editor(lua_State *L)
{
    // Table used to map C++ pointers to Lua objects.
    // TODO: References should be removed when the C++ objects are destroyed.
    lua_newtable(L);
    Lua::add_to_registry(L, LIBRARY_NAME".objectTable");
    lua_pop(L, 1);

    luaL_newmetatable(L, LIBRARY_NAME);
    luaL_setfuncs(L, editorlib_metamethods, 0);
    luaL_newlib(L, editorlib_functions);

    return 1;
}
