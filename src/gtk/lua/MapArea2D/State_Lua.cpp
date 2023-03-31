/**
 * State_Lua.cpp - MapArea2D State Lua binding.
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


////////////////////////////////////////////////////////////////////////////////
// Methods
static int get_pointer_prev(lua_State *L)
{
    auto s = lstate_check(L, 1);
    Lua::make_table(L,
        std::make_pair("x", (lua_Number)s->pointer_prev.x),
        std::make_pair("y", (lua_Number)s->pointer_prev.y));
    return 1;
}

static int set_pointer_prev(lua_State *L)
{
    auto s = lstate_check(L, 1);
    luaL_argcheck(L, lua_istable(L, 2), 2, "`table' expected");
    lua_getfield(L, -1, "x");
    lua_getfield(L, -2, "y");
    s->pointer_prev.x = lua_tonumber(L, -2);
    s->pointer_prev.y = lua_tonumber(L, -1);
    return 0;
}

static int get_multiselect(lua_State *L)
{
    auto s = lstate_check(L, 1);
    Lua::push(L, s->multiselect);
    return 1;
}

static int set_multiselect(lua_State *L)
{
    auto s = lstate_check(L, 1);
    luaL_argcheck(L, lua_isboolean(L, 2), 2, "`boolean' expected");
    s->multiselect = lua_toboolean(L, 2);
    return 0;
}

static luaL_Reg methods[] = {
    {"get_pointer_prev", get_pointer_prev},
    {"set_pointer_prev", set_pointer_prev},
    {"get_multiselect", get_multiselect},
    {"set_multiselect", set_multiselect},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// C++ facing
void Lua::push(lua_State *L, Sickle::MapArea2Dx::State state)
{
    auto ptr = static_cast<Sickle::MapArea2Dx::State *>(
        lua_newuserdatauv(L, sizeof(Sickle::MapArea2Dx::State), 0));
    *ptr = state;
    luaL_setmetatable(L, "Sickle.maparea2d.state");
}

Sickle::MapArea2Dx::State *lstate_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, "Sickle.maparea2d.state");
    luaL_argcheck(L, ud != NULL, arg, "`Sickle.maparea2d.state' expected");
    return static_cast<Sickle::MapArea2Dx::State *>(ud);
}

int luaopen_state(lua_State *L)
{
    luaL_newmetatable(L, "Sickle.maparea2d.state");
    luaL_setfuncs(L, methods, 0);
    lua_setfield(L, -1, "__index");
    return 0;
}
