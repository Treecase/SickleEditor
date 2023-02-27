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

// TODO

#include "LuaGeo.hpp"


////////////////////////////////////////////////////////////////////////////////
// Metamethods
static luaL_Reg metamethods[] = {
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// Functions
int lgeo_matrix_new(lua_State *L)
{
    lua_newuserdatauv(L, 0, 1);
    luaL_setmetatable(L, "geo.matrix");
    lua_newtable(L);
    Lua::set_table(L, (lua_Integer)0, (lua_Number)0.0);
    Lua::set_table(L, (lua_Integer)1, (lua_Number)0.0);
    Lua::set_table(L, (lua_Integer)2, (lua_Number)0.0);
    Lua::set_table(L, (lua_Integer)3, (lua_Number)0.0);
    lua_setiuservalue(L, -2, 1);
    return 1;
}


static luaL_Reg functions[] = {
    {"new", lgeo_matrix_new},
    {NULL, NULL}
};


glm::mat4 lgeo_checkmatrix(lua_State *L, int arg)
{
    luaL_checkudata(L, arg, "geo.matrix");
    return glm::mat4{1.0};
}

int luaopen_geo_matrix(lua_State *L)
{
    luaL_newmetatable(L, "geo.matrix");
    luaL_setfuncs(L, metamethods, 0);
    luaL_newlib(L, functions);
    return 1;
}
