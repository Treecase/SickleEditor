/**
 * FreeCam_Lua.cpp - FreeCam Lua binding.
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

#include "MapArea3D_Lua.hpp"
#include "LuaGeo.hpp"


////////////////////////////////////////////////////////////////////////////////
// Methods
static int rotate(lua_State *L)
{
    auto c = lfreecam_check(L, 1);
    c->rotate(lgeo_checkvector(L, 2));
    return 0;
}

static int translate(lua_State *L)
{
    auto c = lfreecam_check(L, 1);
    c->translate(lgeo_checkvector(L, 2));
    return 0;
}

static int get_angle(lua_State *L)
{
    auto c = lfreecam_check(L, 1);
    Lua::Pusher{L}(c->angle);
    return 1;
}

static int set_angle(lua_State *L)
{
    auto c = lfreecam_check(L, 1);
    c->setAngle(lgeo_checkvector(L, 2));
    return 0;
}

static int get_fov(lua_State *L)
{
    auto c = lfreecam_check(L, 1);
    lua_pushnumber(L, c->fov);
    return 1;
}

static int set_fov(lua_State *L)
{
    auto c = lfreecam_check(L, 1);
    c->setFOV(luaL_checknumber(L, 2));
    return 0;
}

static luaL_Reg methods[] = {
    {"rotate", rotate},
    {"translate", translate},

    {"get_angle", get_angle},
    {"get_fov", get_fov},

    {"set_angle", set_angle},
    {"set_fov", set_fov},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// C++ facing
template<> void Lua::Pusher::operator()(FreeCam camera)
{
    // Create the Lua object.
    auto ptr = static_cast<FreeCam *>(
        lua_newuserdatauv(L, sizeof(FreeCam), 0));
    *ptr = camera;

    // Set metatable.
    luaL_setmetatable(L, "Sickle.freecam");
}

FreeCam *lfreecam_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, "Sickle.freecam");
    luaL_argcheck(L, ud != NULL, arg, "`Sickle.freecam' expected");
    return static_cast<FreeCam *>(ud);
}

int luaopen_freecam(lua_State *L)
{
    luaL_newmetatable(L, "Sickle.freecam");
    luaL_setfuncs(L, methods, 0);
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    return 0;
}