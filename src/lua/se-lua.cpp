/**
 * se-lua.cpp - Sickle Lua integration.
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

#include "se-lua/se-lua.hpp"


Lua::Error::Error(std::string const &what)
:   std::runtime_error{what}
{
}


void Lua::push(lua_State *L, bool value)
{
    lua_pushboolean(L, value);
}

void Lua::push(lua_State *L, lua_Integer value)
{
    lua_pushinteger(L, value);
}

void Lua::push(lua_State *L, lua_Number value)
{
    lua_pushnumber(L, value);
}

void Lua::push(lua_State *L, char const *value)
{
    lua_pushstring(L, value);
}

void Lua::push(lua_State *L, std::string const &value)
{
    lua_pushlstring(L, value.c_str(), value.length());
}


void Lua::checkerror(lua_State *L, int status)
{
    if (status != LUA_OK)
    {
        auto err = lua_tostring(L, -1);
        lua_pop(L, 1);
        throw Error{err};
    }
}

void Lua::get_method(lua_State *L, std::string const &method)
{
    lua_pushlstring(L, method.c_str(), method.length());
    lua_gettable(L, -2);
    lua_rotate(L, -2, 1);
}
