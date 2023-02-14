/**
 * MapArea2D_Lua.cpp - MapArea2D Lua binding.
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

#include "../classes/MapArea2D.hpp"
#include "MapArea2D_Lua.hpp"


Sickle::MapArea2D *lmaparea2d_checkmaparea2d(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, "Sickle.maparea2d");
    luaL_argcheck(L, ud != NULL, arg, "`maparea2d' expected");
    return *static_cast<Sickle::MapArea2D **>(ud);
}

int lmaparea2d_dunder_newindex(lua_State *L)
{
    lmaparea2d_checkmaparea2d(L, 1);
    lua_getiuservalue(L, -3, 1);
    lua_rotate(L, -3, 1);
    lua_settable(L, -3);
    return 0;
}

int lmaparea2d_dunder_index(lua_State *L)
{
    lmaparea2d_checkmaparea2d(L, 1);
    lua_getiuservalue(L, 1, 1);
    lua_rotate(L, -2, 1);
    lua_gettable(L, -2);
    return 1;
}

int lmaparea2d_set_draw_angle(lua_State *L)
{
    auto ma = lmaparea2d_checkmaparea2d(L, 1);
    ma->set_draw_angle((Sickle::MapArea2D::DrawAngle)luaL_checkinteger(L, 2));
    return 0;
}

int lmaparea2d_get_draw_angle(lua_State *L)
{
    auto ma = lmaparea2d_checkmaparea2d(L, 1);
    lua_pushinteger(L, ma->get_draw_angle());
    return 1;
}

luaL_Reg maparea2dlib_metamethods[] = {
    {"__newindex", lmaparea2d_dunder_newindex},
    {"__index", lmaparea2d_dunder_index},
    {NULL, NULL}
};

luaL_Reg maparea2dlib_methods[] = {
    {"set_draw_angle", lmaparea2d_set_draw_angle},
    {"get_draw_angle", lmaparea2d_get_draw_angle},
    {NULL, NULL}
};

static luaL_Reg maparea2dlib_functions[] = {
    {NULL, NULL}
};


int Sickle::lmaparea2d_new_no_signals(lua_State *L, Sickle::MapArea2D const *maparea)
{
    auto ptr = static_cast<Sickle::MapArea2D const **>(
        lua_newuserdatauv(L, sizeof(Sickle::MapArea2D *), 1));
    *ptr = maparea;

    lua_newtable(L);
    luaL_setfuncs(L, maparea2dlib_methods, 0);
    lua_setiuservalue(L, -2, 1);

    luaL_setmetatable(L, "Sickle.maparea2d");

    return 1;
}

int Sickle::lmaparea2d_new(lua_State *L, Sickle::MapArea2D const *maparea)
{
    lmaparea2d_new_no_signals(L, maparea);
    return 1;
}

int luaopen_maparea2d(lua_State *L)
{
    luaL_newmetatable(L, "Sickle.maparea2d");
    luaL_setfuncs(L, maparea2dlib_metamethods, 0);
    luaL_newlib(L, maparea2dlib_functions);
    // export MapArea2D::DrawAngle enum values
    lua_pushliteral(L, "TOP");
    lua_pushinteger(L, Sickle::MapArea2D::DrawAngle::TOP);
    lua_settable(L, -3);
    lua_pushliteral(L, "FRONT");
    lua_pushinteger(L, Sickle::MapArea2D::DrawAngle::FRONT);
    lua_settable(L, -3);
    lua_pushliteral(L, "RIGHT");
    lua_pushinteger(L, Sickle::MapArea2D::DrawAngle::RIGHT);
    lua_settable(L, -3);
    return 1;
}
