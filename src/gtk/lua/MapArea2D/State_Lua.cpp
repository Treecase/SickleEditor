/**
 * State_Lua.cpp - State Lua binding.
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


#define LIBRARY_NAME    "Sickle.maparea2d.state"
#define CLASSNAME       Sickle::MapArea2Dx::State


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
    lua_pushliteral(L, "x"); lua_gettable(L, -2);
    lua_pushliteral(L, "y"); lua_gettable(L, -3);
    s->pointer_prev.x = lua_tonumber(L, -2);
    s->pointer_prev.y = lua_tonumber(L, -1);
    return 0;
}

static int get_dragged(lua_State *L)
{
    auto s = lstate_check(L, 1);
    Lua::Pusher{L}(s->dragged);
    return 1;
}

static int set_dragged(lua_State *L)
{
    auto s = lstate_check(L, 1);
    luaL_argcheck(L, lua_isboolean(L, 2), 2, "`boolean' expected");
    s->dragged = lua_toboolean(L, 2);
    return 0;
}

static int get_multiselect(lua_State *L)
{
    auto s = lstate_check(L, 1);
    Lua::Pusher{L}(s->multiselect);
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
    {"get_dragged", get_dragged},
    {"set_dragged", set_dragged},
    {"get_multiselect", get_multiselect},
    {"set_multiselect", set_multiselect},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// C++ facing
int lstate_new(lua_State *L, CLASSNAME const &state)
{
    // Create the Lua object.
    auto ptr = static_cast<CLASSNAME *>(
        lua_newuserdatauv(L, sizeof(CLASSNAME), 0));
    *ptr = state;

    // Set metatable.
    luaL_setmetatable(L, LIBRARY_NAME);

    return 1;
}

CLASSNAME *lstate_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, LIBRARY_NAME);
    luaL_argcheck(L, ud != NULL, arg, "`" LIBRARY_NAME "' expected");
    return static_cast<CLASSNAME *>(ud);
}

int luaopen_state(lua_State *L)
{
    luaL_newmetatable(L, LIBRARY_NAME);
    luaL_setfuncs(L, methods, 0);
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);
    return 0;
}
