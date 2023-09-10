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


void Lua::push(lua_State *L, glm::vec4 vector)
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

void Lua::push(lua_State *L, glm::vec3 vector)
{
    push(L, glm::vec4{vector, 0.0});
}

void Lua::push(lua_State *L, glm::vec2 vector)
{
    push(L, glm::vec4{vector, 0.0, 0.0});
}


template<>
glm::vec4 Lua::get_as(lua_State *L, int idx)
{
    return lgeo_tovector(L, idx);
}

template<>
glm::vec3 Lua::get_as(lua_State *L, int idx)
{
    return lgeo_tovector(L, idx);
}

template<>
glm::vec2 Lua::get_as(lua_State *L, int idx)
{
    return lgeo_tovector(L, idx);
}


////////////////////////////////////////////////////////////////////////////////
// Metamethods
static int vector_add(lua_State *L)
{
    Lua::push(L, lgeo_checkvector(L, 1) + lgeo_checkvector(L, 2));
    return 1;
}

static int vector_sub(lua_State *L)
{
    Lua::push(L, lgeo_checkvector(L, 1) - lgeo_checkvector(L, 2));
    return 1;
}

static int vector_mul(lua_State *L)
{
    if (lua_isnumber(L, 1))
        Lua::push(L, (float)lua_tonumber(L, 1) * lgeo_checkvector(L, 2));
    else if (lua_isnumber(L, 2))
        Lua::push(L, lgeo_checkvector(L, 1) * (float)lua_tonumber(L, 2));
    else
        Lua::push(L, lgeo_checkvector(L, 1) * lgeo_checkvector(L, 2));
    return 1;
}

static int vector_div(lua_State *L)
{
    if (lua_isnumber(L, 1))
        Lua::push(L, (float)luaL_checknumber(L, 1) / lgeo_checkvector(L, 2));
    else if (lua_isnumber(L, 2))
        Lua::push(L, lgeo_checkvector(L, 1) / (float)luaL_checknumber(L, 2));
    else
        Lua::push(L, lgeo_checkvector(L, 1) / lgeo_checkvector(L, 2));
    return 1;
}

static int vector_negate(lua_State *L)
{
    Lua::push(L, -lgeo_checkvector(L, 1));
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
    {"__unm", vector_negate},
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
        Lua::push(L, glm::vec4{0, 0, 0, 0});
        break;
    case 1:
        try {
            Lua::push(L, lgeo_tovector(L, 1));
        }
        catch (Lua::Error const &e) {
            luaL_error(L, "%s", e.what());
        }
        break;
    case 2:
        Lua::push(L, glm::vec4{
            luaL_checknumber(L, 1), luaL_checknumber(L, 2), 0, 0});
        break;
    case 3:
        Lua::push(L, glm::vec4{
            luaL_checknumber(L, 1),
            luaL_checknumber(L, 2),
            luaL_checknumber(L, 3),
            0});
        break;
    case 4:
        Lua::push(L, glm::vec4{
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
    auto v = lgeo_checkvector(L, 2);
    for (size_t i = 0; i < 4; ++i)
    {
        // Duplicate the callable
        lua_pushvalue(L, 1);
        // Push the vector's i-th value
        lua_pushnumber(L, v[i]);
        // Call the function
        lua_call(L, 1, 1);
        // Set result's i-th value
        v[i] = lua_tonumber(L, -1);
        lua_pop(L, 1);
    }
    Lua::push(L, v);
    return 1;
}

static int fn_vector_length(lua_State *L)
{
    auto v = lgeo_checkvector(L, 1);
    lua_pushnumber(L, glm::length(v));
    return 1;
}

static luaL_Reg functions[] = {
    {"new", lgeo_vector_new},
    {"map", fn_vector_map},
    {"length", fn_vector_length},
    {NULL, NULL}
};


static void lgeo_checkvectorfast(lua_State *L, int arg)
{
    luaL_checkudata(L, arg, "geo.vector");
}

glm::vec4 lgeo_checkvector(lua_State *L, int arg)
{
    lgeo_checkvectorfast(L, arg);
    return lgeo_tovector(L, arg);
}

glm::vec4 lgeo_tovector(lua_State *L, int idx)
{
    if (!(lua_istable(L, idx) || lua_isuserdata(L, idx)))
        throw Lua::Error{"expected table or userdata"};

    auto i = idx;
    if (i < 0)
        i = lua_gettop(L) + i + 1;

    // If the value has an 'x' field, interpret it as a table-style vector.
    lua_getfield(L, i, "x");
    if (!lua_isnil(L, -1))
    {
        lua_getfield(L, i, "y");
        lua_getfield(L, i, "z");
        lua_getfield(L, i, "w");
    }
    // Otherwise, interpret as a list-style vector.
    else
    {
        lua_pop(L, 1);
        lua_geti(L, i, 1);
        lua_geti(L, i, 2);
        lua_geti(L, i, 3);
        lua_geti(L, i, 4);
    }

    std::array<int, 4> success{};
    auto x = lua_tonumberx(L, -4, &success[0]);
    auto y = lua_tonumberx(L, -3, &success[1]);
    auto z = lua_tonumberx(L, -2, &success[2]);
    auto w = lua_tonumberx(L, -1, &success[3]);
    lua_pop(L, 4);

    bool failed = std::all_of(
        success.cbegin(), success.cend(),
        [](auto v){return v == 0;});
    if (failed)
        throw Lua::Error{"Value is not a vector-like object"};

    return {x, y, z, w};
}

int luaopen_geo_vector(lua_State *L)
{
    luaL_newmetatable(L, "geo.vector");
    luaL_setfuncs(L, metamethods, 0);
    luaL_newlib(L, functions);
    return 1;
}
