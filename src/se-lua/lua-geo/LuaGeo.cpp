/**
 * LuaGeo.hpp - Lua vectors and matrices.
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

int luaopen_geo(lua_State *L)
{
    lua_newtable(L);
    luaL_requiref(L, "geo.vec2", luaopen_geo_vector<glm::vec2>, 0);
    luaL_requiref(L, "geo.vec3", luaopen_geo_vector<glm::vec3>, 0);
    luaL_requiref(L, "geo.vec4", luaopen_geo_vector<glm::vec4>, 0);
    luaL_requiref(L, "geo.matrix", luaopen_geo_matrix, 0);
    lua_setfield(L, -5, "matrix");
    lua_setfield(L, -4, "vec4");
    lua_setfield(L, -3, "vec3");
    lua_setfield(L, -2, "vec2");
    return 1;
}
