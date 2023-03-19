/**
 * State_Lua.cpp - MapArea3D::State Lua binding.
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
static int get_pointer_prev(lua_State *L)
{
    auto s = lmaparea3d_state_check(L, 1);
    Lua::push(L, s->pointer_prev);
    return 1;
}

static int set_pointer_prev(lua_State *L)
{
    auto s = lmaparea3d_state_check(L, 1);
    auto const &v = lgeo_checkvector(L, 2);
    s->pointer_prev = v;
    return 0;
}

static int get_last_frame_time(lua_State *L)
{
    auto s = lmaparea3d_state_check(L, 1);
    lua_pushnumber(L, s->last_frame_time);
    return 1;
}

static int get_move_direction(lua_State *L)
{
    auto s = lmaparea3d_state_check(L, 1);
    Lua::push(L, s->move_direction);
    return 1;
}

static int set_move_direction(lua_State *L)
{
    auto s = lmaparea3d_state_check(L, 1);
    auto const &v = lgeo_checkvector(L, 2);
    s->move_direction = v;
    return 0;
}

static int get_turn_rates(lua_State *L)
{
    auto s = lmaparea3d_state_check(L, 1);
    Lua::push(L, s->turn_rates);
    return 1;
}

static int set_turn_rates(lua_State *L)
{
    auto s = lmaparea3d_state_check(L, 1);
    auto const &v = lgeo_checkvector(L, 2);
    s->turn_rates = v;
    return 0;
}

static int get_gofast(lua_State *L)
{
    auto s = lmaparea3d_state_check(L, 1);
    lua_pushboolean(L, s->gofast);
    return 1;
}

static int set_gofast(lua_State *L)
{
    auto s = lmaparea3d_state_check(L, 1);
    s->gofast = lua_toboolean(L, 2);
    return 0;
}

static int get_multiselect(lua_State *L)
{
    auto s = lmaparea3d_state_check(L, 1);
    lua_pushboolean(L, s->multiselect);
    return 1;
}

static int set_multiselect(lua_State *L)
{
    auto s = lmaparea3d_state_check(L, 1);
    s->multiselect = lua_toboolean(L, 2);
    return 0;
}

static luaL_Reg methods[] = {
    {"get_pointer_prev", get_pointer_prev},
    {"get_last_frame_time", get_last_frame_time},
    {"get_move_direction", get_move_direction},
    {"get_turn_rates", get_turn_rates},
    {"get_gofast", get_gofast},
    {"get_multiselect", get_multiselect},

    {"set_pointer_prev", set_pointer_prev},
    {"set_move_direction", set_move_direction},
    {"set_turn_rates", set_turn_rates},
    {"set_gofast", set_gofast},
    {"set_multiselect", set_multiselect},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// C++ facing
void Lua::push(lua_State *L, Sickle::MapArea3D::State state)
{
    auto ptr = static_cast<Sickle::MapArea3D::State *>(
        lua_newuserdatauv(L, sizeof(Sickle::MapArea3D::State), 0));
    *ptr = state;
    luaL_setmetatable(L, "Sickle.maparea3d.state");
}

Sickle::MapArea3D::State *lmaparea3d_state_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, "Sickle.maparea3d.state");
    luaL_argcheck(L, ud != NULL, arg, "`Sickle.maparea3d.state' expected");
    return static_cast<Sickle::MapArea3D::State *>(ud);
}

int luaopen_maparea3d_state(lua_State *L)
{
    luaL_newmetatable(L, "Sickle.maparea3d.state");
    luaL_setfuncs(L, methods, 0);
    lua_setfield(L, -1, "__index");
    return 0;
}
