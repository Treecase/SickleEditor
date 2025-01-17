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

#ifndef SE_LUAGEO_HPP
#define SE_LUAGEO_HPP

#include "vector.hpp"

#include <se-lua/se-lua.hpp>

#include <glm/glm.hpp>

int luaopen_geo(lua_State *L);

int luaopen_geo_matrix(lua_State *L);
int lgeo_matrix_new(lua_State *L);
glm::mat4 lgeo_checkmatrix(lua_State *L, int arg);
glm::mat4 lgeo_tomatrix(lua_State *L, int i);

template<>
void Lua::push(lua_State *L, glm::mat4 mat);
template<>
glm::mat4 Lua::get_as(lua_State *L, int idx);

#endif
