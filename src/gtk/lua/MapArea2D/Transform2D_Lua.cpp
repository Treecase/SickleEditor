/**
 * Transform2D_Lua.cpp - Transform2D Lua binding.
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

#include "../classes/MapArea2D.hpp"
#include "MapArea2D_Lua.hpp"

#define METATABLE "Sickle.gtk.maparea2d.transform2d"


using namespace Sickle::MapArea2Dx;


////////////////////////////////////////////////////////////////////////////////
// Methods
static int get_x(lua_State *L)
{
    auto t = ltransform2d_check(L, 1);
    Lua::push(L, t->x);
    return 1;
}

static int set_x(lua_State *L)
{
    auto t = ltransform2d_check(L, 1);
    t->x = luaL_checknumber(L, 2);
    return 0;
}

static int get_y(lua_State *L)
{
    auto t = ltransform2d_check(L, 1);
    Lua::push(L, t->y);
    return 1;
}

static int set_y(lua_State *L)
{
    auto t = ltransform2d_check(L, 1);
    t->y = luaL_checknumber(L, 2);
    return 0;
}

static int get_zoom(lua_State *L)
{
    auto t = ltransform2d_check(L, 1);
    Lua::push(L, t->zoom);
    return 1;
}

static int set_zoom(lua_State *L)
{
    auto t = ltransform2d_check(L, 1);
    t->zoom = luaL_checknumber(L, 2);
    return 0;
}

static luaL_Reg methods[] = {
    {"get_x", get_x},
    {"set_x", set_x},
    {"get_y", get_y},
    {"set_y", set_y},
    {"get_zoom", get_zoom},
    {"set_zoom", set_zoom},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// C++ facing
void Lua::push(lua_State *L, Transform2D transform)
{
    auto ptr = static_cast<Transform2D *>(
        lua_newuserdatauv(L, sizeof(Transform2D), 0));
    *ptr = transform;
    luaL_setmetatable(L, METATABLE);
}

Transform2D *ltransform2d_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, METATABLE);
    luaL_argcheck(L, ud != NULL, arg, "`" METATABLE "' expected");
    return static_cast<Transform2D *>(ud);
}

int luaopen_transform2d(lua_State *L)
{
    luaL_newmetatable(L, METATABLE);
    luaL_setfuncs(L, methods, 0);
    lua_setfield(L, -1, "__index");
    return 1;
}
