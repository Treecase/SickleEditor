/**
 * AppWin_Lua.cpp - AppWin Lua binding.
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

#include "../classes/AppWin.hpp"
#include "AppWin_Lua.hpp"
#include "MapArea2D_Lua.hpp"


#define LIBRARY_NAME    "Sickle.appwin"


/** Add value at the top of the stack to the objectTable using KEY. */
void add_to_objectTable(lua_State *L, Sickle::AppWin *key)
{
    Lua::get_from_registry(L, LIBRARY_NAME".objectTable");
    lua_pushlightuserdata(L, key);
    lua_pushvalue(L, -3);
    lua_settable(L, -3);
    lua_pop(L, 1);
}

/** Get the Lua value associated with KEY from the objectTable. */
void get_from_objectTable(lua_State *L, Sickle::AppWin *key)
{
    Lua::get_from_registry(L, LIBRARY_NAME".objectTable");
    lua_pushlightuserdata(L, key);
    lua_gettable(L, -2);
    lua_remove(L, -2);
}


////////////////////////////////////////////////////////////////////////////////
// Internal
Sickle::AppWin *lappwin_checkappwin(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, LIBRARY_NAME);
    luaL_argcheck(L, ud != NULL, arg, "`" LIBRARY_NAME "' expected");
    return *static_cast<Sickle::AppWin **>(ud);
}


////////////////////////////////////////////////////////////////////////////////
// Metamethods
int lappwin_dunder_newindex(lua_State *L)
{
    lappwin_checkappwin(L, 1);
    lua_getiuservalue(L, -3, 1);
    lua_rotate(L, -3, 1);
    lua_settable(L, -3);
    return 0;
}

int lappwin_dunder_index(lua_State *L)
{
    lappwin_checkappwin(L, 1);
    lua_getiuservalue(L, 1, 1);
    lua_rotate(L, -2, 1);
    lua_gettable(L, -2);
    return 1;
}

luaL_Reg appwinlib_metamethods[] = {
    {"__newindex", lappwin_dunder_newindex},
    {"__index", lappwin_dunder_index},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// Methods
int lappwin_on_grid_size_changed(lua_State *L)
{
    auto aw = lappwin_checkappwin(L, 1);
    return 0;
}

int lappwin_set_grid_size(lua_State *L)
{
    auto aw = lappwin_checkappwin(L, 1);
    aw->set_grid_size(luaL_checkinteger(L, 2));
    return 0;
}

int lappwin_get_grid_size(lua_State *L)
{
    auto aw = lappwin_checkappwin(L, 1);
    lua_pushinteger(L, aw->get_grid_size());
    return 1;
}

int lappwin_on_key_press_event(lua_State *L)
{
    auto aw = lappwin_checkappwin(L, 1);
    auto keyval = luaL_checkinteger(L, 2);

    switch (keyval)
    {
    case GDK_KEY_bracketleft:{
        aw->set_grid_size(aw->get_grid_size() / 2);
        break;}
    case GDK_KEY_bracketright:{
        aw->set_grid_size(aw->get_grid_size() * 2);
        break;}
    default:
        lua_pushboolean(L, false);
        return 1;
        break;
    }
    lua_pushboolean(L, true);
    return 1;
}

luaL_Reg appwinlib_methods[] = {
    {"on_grid_size_changed", lappwin_on_grid_size_changed},
    {"set_grid_size", lappwin_set_grid_size},
    {"get_grid_size", lappwin_get_grid_size},
    {"on_key_press_event", lappwin_on_key_press_event},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// Functions
luaL_Reg appwinlib_functions[] = {
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// C++ facing
int Sickle::lappwin_new(lua_State *L, Sickle::AppWin *appwin)
{
    // Create the Lua object.
    auto ptr = static_cast<Sickle::AppWin const **>(
        lua_newuserdatauv(L, sizeof(Sickle::AppWin *), 1));
    *ptr = appwin;

    // Add methods/data table.
    lua_newtable(L);
    luaL_setfuncs(L, appwinlib_methods, 0);
    lua_setiuservalue(L, -2, 1);

    // Set metatable.
    luaL_setmetatable(L, LIBRARY_NAME);

    // Add fields.
    lua_pushliteral(L, "topMapArea");
    lmaparea2d_new(L, &appwin->m_drawarea_top);
    lua_settable(L, -3);

    lua_pushliteral(L, "frontMapArea");
    lmaparea2d_new(L, &appwin->m_drawarea_front);
    lua_settable(L, -3);

    lua_pushliteral(L, "rightMapArea");
    lmaparea2d_new(L, &appwin->m_drawarea_right);
    lua_settable(L, -3);


    // Add the object to the Lua registry, using the pointer as key. This is
    // needed for the C++ callbacks to know what object to call methods on.
    add_to_objectTable(L, appwin);

    // Connect signals.
    appwin->property_grid_size().signal_changed().connect(
        [L, appwin](){
            get_from_objectTable(L, appwin);
            Lua::call_method(L, "on_grid_size_changed");
        });
    appwin->signal_key_press_event().connect(
        [L, appwin](GdkEventKey *e){
            get_from_objectTable(L, appwin);
            Lua::call_method(L, "on_key_press_event", (lua_Integer)e->keyval);
            return lua_toboolean(L, -1);
        });

    return 1;
}

int luaopen_appwin(lua_State *L)
{
    // Table used to map C++ pointers to Lua objects.
    // TODO: References should be removed when the C++ objects are destroyed.
    lua_newtable(L);
    Lua::add_to_registry(L, LIBRARY_NAME".objectTable");
    lua_pop(L, 1);

    luaL_newmetatable(L, LIBRARY_NAME);
    luaL_setfuncs(L, appwinlib_metamethods, 0);
    luaL_newlib(L, appwinlib_functions);
    return 1;
}
