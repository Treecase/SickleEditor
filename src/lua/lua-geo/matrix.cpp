/**
 * matrix.cpp - LuaGeo matrices.
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

#include <glm/gtc/matrix_transform.hpp>


static void lgeo_checkmatrixfast(lua_State *L, int arg);


void Lua::push(lua_State *L, glm::mat4 matrix)
{
    lua_newuserdatauv(L, 0, 1);
    luaL_setmetatable(L, "geo.matrix");
    lua_newtable(L);
    Lua::push(L, matrix[0]);
    Lua::push(L, matrix[1]);
    Lua::push(L, matrix[2]);
    Lua::push(L, matrix[3]);
    lua_rawseti(L, -5, 3);
    lua_rawseti(L, -4, 2);
    lua_rawseti(L, -3, 1);
    lua_rawseti(L, -2, 0);
    lua_setiuservalue(L, -2, 1);
}


////////////////////////////////////////////////////////////////////////////////
// Metamethods
static int matrix_mul(lua_State *L)
{
    Lua::push(L, lgeo_checkmatrix(L, 1) * lgeo_checkmatrix(L, 2));
    return 1;
}

static int matrix_index(lua_State *L)
{
    lgeo_checkmatrixfast(L, 1);
    lua_getiuservalue(L, 1, 1);
    lua_rotate(L, -2, 1);
    lua_rawget(L, -2);
    return 1;
}

static int matrix_newindex(lua_State *L)
{
    lgeo_checkmatrixfast(L, 1);
    lua_getiuservalue(L, 1, 1);
    lua_rotate(L, -3, 1);
    lua_rawset(L, -3);
    return 0;
}

static int matrix_tostring(lua_State *L)
{
    auto m = lgeo_checkmatrix(L, 1);
    lua_pushfstring(L,
        "<matrix %f %f %f %f\n"
        "        %f %f %f %f\n"
        "        %f %f %f %f\n"
        "        %f %f %f %f>",
        m[0].x, m[1].x, m[2].x, m[3].x,
        m[0].y, m[1].y, m[2].y, m[3].y,
        m[0].z, m[1].z, m[2].z, m[3].z,
        m[0].w, m[1].w, m[2].w, m[3].w);
    return 1;
}

static luaL_Reg metamethods[] = {
    {"__mul", matrix_mul},
    {"__index", matrix_index},
    {"__newindex", matrix_newindex},

    {"__tostring", matrix_tostring},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// Functions
int lgeo_matrix_new(lua_State *L)
{
    Lua::push(L, glm::mat4{1.0});
    return 1;
}

static int matrix_translate(lua_State *L)
{
    auto const m = lgeo_checkmatrix(L, 1);
    auto const v = lgeo_checkvector<glm::vec3>(L, 2);
    Lua::push(L, glm::translate(m, v));
    return 1;
}

static int matrix_rotate(lua_State *L)
{
    auto const m = lgeo_checkmatrix(L, 1);
    float const angle = glm::radians(luaL_checknumber(L, 2));
    auto const v = lgeo_checkvector<glm::vec3>(L, 3);
    Lua::push(L, glm::rotate(m, angle, v));
    return 1;
}

static int matrix_scale(lua_State *L)
{
    auto const m = lgeo_checkmatrix(L, 1);
    auto const v = lgeo_checkvector<glm::vec3>(L, 2);
    Lua::push(L, glm::scale(m, v));
    return 1;
}


static luaL_Reg functions[] = {
    {"new", lgeo_matrix_new},
    {"translate", matrix_translate},
    {"rotate", matrix_rotate},
    {"scale", matrix_scale},
    {NULL, NULL}
};


void lgeo_checkmatrixfast(lua_State *L, int arg)
{
    luaL_checkudata(L, arg, "geo.matrix");
}

glm::mat4 lgeo_checkmatrix(lua_State *L, int arg)
{
    lgeo_checkmatrixfast(L, arg);
    return lgeo_tomatrix(L, arg);
}

glm::mat4 lgeo_tomatrix(lua_State *L, int i)
{
    if (i < 0)
        i = lua_gettop(L) + i + 1;
    lua_geti(L, i, 0);
    lua_geti(L, i, 1);
    lua_geti(L, i, 2);
    lua_geti(L, i, 3);
    glm::mat4 m{
        lgeo_tovector<glm::vec4>(L, -4),
        lgeo_tovector<glm::vec4>(L, -3),
        lgeo_tovector<glm::vec4>(L, -2),
        lgeo_tovector<glm::vec4>(L, -1)};
    lua_pop(L, 4);
    return m;
}

int luaopen_geo_matrix(lua_State *L)
{
    luaL_newmetatable(L, "geo.matrix");
    luaL_setfuncs(L, metamethods, 0);
    luaL_newlib(L, functions);
    return 1;
}
