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

#include "se-lua.hpp"

#include <unordered_map>

Lua::Error::Error(std::string const &what)
: std::runtime_error{what}
{
}

Lua::StackOverflow::StackOverflow()
: StackOverflow{"Not enough stack space"}
{
}

Lua::StackOverflow::StackOverflow(std::string const &what)
: Error{what}
{
}

/* ===[ Push values to Lua ]=== */
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

/* ===[ Get a Lua value ]=== */
template<>
bool Lua::get_as(lua_State *L, int idx)
{
    return lua_toboolean(L, idx);
}

template<>
lua_Integer Lua::get_as(lua_State *L, int idx)
{
    return lua_tointeger(L, idx);
}

template<>
lua_Number Lua::get_as(lua_State *L, int idx)
{
    return lua_tonumber(L, idx);
}

template<>
char const *Lua::get_as(lua_State *L, int idx)
{
    return lua_tostring(L, idx);
}

template<>
std::string Lua::get_as(lua_State *L, int idx)
{
    return std::string{lua_tostring(L, idx)};
}

void Lua::get_method(lua_State *L, std::string const &method)
{
    lua_pushlstring(L, method.c_str(), method.length());
    lua_gettable(L, -2);
    lua_rotate(L, -2, 1);
}

/* ===[ Error handling ]=== */
// TODO: Better way to do this?
static std::unordered_map<lua_State *, std::function<void(lua_State *)>>
    error_handlers{};

void Lua::set_error_handler(lua_State *L, std::function<void(lua_State *)> fn)
{
    error_handlers[L] = fn;
}

void Lua::clear_error_handler(lua_State *L)
{
    error_handlers.erase(L);
}

static void default_error_handler(lua_State *L)
{
    auto err = lua_tostring(L, -1);
    lua_pop(L, 1);
    throw Lua::Error{
        err ? err : "An error ocurred, but the error message was null"};
}

void Lua::checkerror(lua_State *L, int status)
{
    if (status != LUA_OK)
    {
        if (error_handlers.count(L) != 0)
        {
            error_handlers.at(L)(L);
        }
        else
        {
            default_error_handler(L);
        }
    }
}

int Lua::pcall(lua_State *L, int nargs, int nresults)
{
    if (!lua_checkstack(L, 1))
    {
        throw Lua::StackOverflow{};
    }

    int const msgh_type = lua_getfield(L, LUA_REGISTRYINDEX, "__msgh");

    // Rotate the message handler to be below the function and its arguments.
    lua_rotate(L, -nargs - 2, 1);

    // msgh value to be passed to lua_pcall. Uses 0 (ie. no handler) if the
    // state's __msg value is nil.
    int const msgh = (msgh_type == LUA_TNIL ? 0 : -nargs - 2);

    int const r = lua_pcall(L, nargs, nresults, msgh);

    // Pop the message handler if no error occurred.
    if (r == LUA_OK)
    {
        lua_pop(L, 1);
    }

    return r;
}
