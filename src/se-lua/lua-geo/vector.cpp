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

#include "vector.hpp"

template<>
void Lua::push(lua_State *L, glm::vec2 vector)
{
    _make_vector(L, vector);
}

template<>
void Lua::push(lua_State *L, glm::vec3 vector)
{
    _make_vector(L, vector);
}

template<>
void Lua::push(lua_State *L, glm::vec4 vector)
{
    _make_vector(L, vector);
}

template<>
glm::vec2 Lua::get_as(lua_State *L, int idx)
{
    return lgeo_tovector<glm::vec2>(L, idx);
}

template<>
glm::vec3 Lua::get_as(lua_State *L, int idx)
{
    return lgeo_tovector<glm::vec3>(L, idx);
}

template<>
glm::vec4 Lua::get_as(lua_State *L, int idx)
{
    return lgeo_tovector<glm::vec4>(L, idx);
}
