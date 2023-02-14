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


Sickle::AppWin *lappwin_checkappwin(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, "Sickle.appwin");
    luaL_argcheck(L, ud != NULL, arg, "`appwin' expected");
    return *static_cast<Sickle::AppWin **>(ud);
}

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

luaL_Reg appwinlib_metamethods[] = {
    {"__newindex", lappwin_dunder_newindex},
    {"__index", lappwin_dunder_index},
    {NULL, NULL}
};

luaL_Reg appwinlib_methods[] = {
    {"on_grid_size_changed", lappwin_on_grid_size_changed},
    {"set_grid_size", lappwin_set_grid_size},
    {"get_grid_size", lappwin_get_grid_size},
    {NULL, NULL}
};

luaL_Reg appwinlib_functions[] = {
    {NULL, NULL}
};


int Sickle::lappwin_new_no_signals(lua_State *L, Sickle::AppWin const *appwin)
{
    auto ptr = static_cast<Sickle::AppWin const **>(
        lua_newuserdatauv(L, sizeof(Sickle::AppWin *), 1));
    *ptr = appwin;

    lua_newtable(L);
    luaL_setfuncs(L, appwinlib_methods, 0);
    lua_setiuservalue(L, -2, 1);

    luaL_setmetatable(L, "Sickle.appwin");

    lua_pushliteral(L, "topMapArea");
    lmaparea2d_new(L, &appwin->m_drawarea_top);
    lua_settable(L, -3);

    lua_pushliteral(L, "frontMapArea");
    lmaparea2d_new(L, &appwin->m_drawarea_front);
    lua_settable(L, -3);

    lua_pushliteral(L, "rightMapArea");
    lmaparea2d_new(L, &appwin->m_drawarea_right);
    lua_settable(L, -3);

    return 1;
}

int Sickle::lappwin_new(lua_State *L, Sickle::AppWin *appwin)
{
    lappwin_new_no_signals(L, appwin);
    appwin->property_grid_size().signal_changed().connect(
        [L, appwin](){
            lappwin_new_no_signals(L, appwin);
            Lua::call_method(L, "on_grid_size_changed");
        });
    return 1;
}

int luaopen_appwin(lua_State *L)
{
    luaL_newmetatable(L, "Sickle.appwin");
    luaL_setfuncs(L, appwinlib_metamethods, 0);
    luaL_newlib(L, appwinlib_functions);
    return 1;
}
