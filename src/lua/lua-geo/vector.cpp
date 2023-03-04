/**
 * vector.cpp - LuaGeo vectors.
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

#include "LuaGeo.hpp"


static void lgeo_checkvectorfast(lua_State *L, int arg);


template<> void Lua::Pusher::operator()(glm::vec4 vector)
{
    lua_newuserdatauv(L, 0, 1);
    luaL_setmetatable(L, "geo.vector");
    lua_newtable(L);
    Lua::set_table(L, "x", (lua_Number)vector.x);
    Lua::set_table(L, "y", (lua_Number)vector.y);
    Lua::set_table(L, "z", (lua_Number)vector.z);
    Lua::set_table(L, "w", (lua_Number)vector.w);
    lua_setiuservalue(L, -2, 1);
}

template<> void Lua::Pusher::operator()(glm::vec3 vector)
{
    (*this)(glm::vec4{vector, 1.0});
}

template<> void Lua::Pusher::operator()(glm::vec2 vector)
{
    (*this)(glm::vec4{vector, 0.0, 1.0});
}


////////////////////////////////////////////////////////////////////////////////
// Metamethods
static int vector_add(lua_State *L)
{
    Lua::Pusher{L}(lgeo_checkvector(L, 1) + lgeo_checkvector(L, 2));
    return 1;
}

static int vector_sub(lua_State *L)
{
    Lua::Pusher{L}(lgeo_checkvector(L, 1) - lgeo_checkvector(L, 2));
    return 1;
}

static int vector_mul(lua_State *L)
{
    if (lua_isnumber(L, 1))
        Lua::Pusher{L}((float)luaL_checknumber(L, 1) * lgeo_checkvector(L, 2));
    else
        Lua::Pusher{L}(lgeo_checkvector(L, 1) * (float)luaL_checknumber(L, 2));
    return 1;
}

static int vector_div(lua_State *L)
{
    Lua::Pusher{L}(lgeo_checkvector(L, 1) / (float)luaL_checknumber(L, 2));
    return 1;
}

static int vector_index(lua_State *L)
{
    lgeo_checkvectorfast(L, 1);
    lua_getiuservalue(L, 1, 1);
    lua_rotate(L, -2, 1);
    lua_rawget(L, -2);
    return 1;
}

static int vector_newindex(lua_State *L)
{
    lgeo_checkvectorfast(L, 1);
    lua_getiuservalue(L, 1, 1);
    lua_rotate(L, -3, 1);
    lua_rawset(L, -3);
    return 0;
}

static int vector_tostring(lua_State *L)
{
    auto v = lgeo_checkvector(L, 1);
    lua_pushfstring(L, "<vector %f %f %f %f>", v.x, v.y, v.z, v.w);
    return 1;
}

static luaL_Reg metamethods[] = {
    {"__add", vector_add},
    {"__sub", vector_sub},
    {"__mul", vector_mul},
    {"__div", vector_div},
    {"__index", vector_index},
    {"__newindex", vector_newindex},

    {"__tostring", vector_tostring},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// Functions
int lgeo_vector_new(lua_State *L)
{
    switch (lua_gettop(L))
    {
    case 0:
        Lua::Pusher{L}(glm::vec4{0, 0, 0, 1});
        break;
    case 2:
        Lua::Pusher{L}(glm::vec4{
            luaL_checknumber(L, 1),
            luaL_checknumber(L, 2),
            0, 1});
        break;
    case 3:
        Lua::Pusher{L}(glm::vec4{
            luaL_checknumber(L, 1),
            luaL_checknumber(L, 2),
            luaL_checknumber(L, 3),
            1});
        break;
    case 4:
        Lua::Pusher{L}(glm::vec4{
            luaL_checknumber(L, 1),
            luaL_checknumber(L, 2),
            luaL_checknumber(L, 3),
            luaL_checknumber(L, 4)});
        break;
    default:
        luaL_argcheck(L, false, 1, "invalid args");
        return 0;
    }
    return 1;
}

static int fn_vector_map(lua_State *L)
{
    static char const *const strings[4] = {"x", "y", "z", "w"};
    auto v = lgeo_checkvector(L, 2);
    for (size_t i = 0; i < 4; ++i)
    {
        // Duplicate the callable
        lua_pushvalue(L, 1);
        // Push the vector's i-th value
        lua_pushstring(L, strings[i]);
        lua_gettable(L, 2);
        // Call the function
        lua_call(L, 1, 1);
        // Set result's i-th value
        v[i] = lua_tonumber(L, -1);
        lua_pop(L, 1);
    }
    Lua::Pusher{L}(v);
    return 1;
}

static luaL_Reg functions[] = {
    {"new", lgeo_vector_new},
    {"map", fn_vector_map},
    {NULL, NULL}
};


static void lgeo_checkvectorfast(lua_State *L, int arg)
{
    luaL_checkudata(L, arg, "geo.vector");
}

glm::vec4 lgeo_checkvector(lua_State *L, int arg)
{
    lgeo_checkvectorfast(L, arg);

    lua_pushliteral(L, "x"); lua_gettable(L, arg);
    lua_pushliteral(L, "y"); lua_gettable(L, arg);
    lua_pushliteral(L, "z"); lua_gettable(L, arg);
    lua_pushliteral(L, "w"); lua_gettable(L, arg);

    auto x = lua_tonumber(L, -4);
    auto y = lua_tonumber(L, -3);
    auto z = lua_tonumber(L, -2);
    auto w = lua_tonumber(L, -1);

    lua_pop(L, 4);

    return {x, y, z, w};
}

int luaopen_geo_vector(lua_State *L)
{
    luaL_newmetatable(L, "geo.vector");
    luaL_setfuncs(L, metamethods, 0);
    luaL_newlib(L, functions);
    return 1;
}
