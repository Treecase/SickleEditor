/**
 * se-lua.hpp - Sickle Lua integration.
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

#ifndef SE_LUA_HPP
#define SE_LUA_HPP

#include "utils/TemplateUtils.hpp"

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <stdexcept>
#include <string>


namespace Lua
{
    struct Error : public std::runtime_error
    {
        Error(std::string const &what);
    };

    /** Push a value onto the stack. */
    struct Pusher
    {
        lua_State *L;

        /** Template to allow extension by adding specifications. */
        template<typename T>
        void operator()(T value)=delete;

        void operator()(lua_Integer value)
        {
            lua_pushinteger(L, value);
        }
        void operator()(void *value)
        {
            lua_pushlightuserdata(L, value);
        }
        void operator()(char const *value)
        {
            lua_pushstring(L, value);
        }
        void operator()(std::string const &value)
        {
            lua_pushlstring(L, value.c_str(), value.length());
        }
    };


    /** Check a Lua status error. */
    void checkerror(lua_State *L, int status);

    /**
     * Gets METHOD from object on top of stack and rotates to prepare for a
     * method call.
     */
    void get_method(lua_State *L, std::string const &method);

    /** Call METHOD on the value at the top of the stack. */
    template<typename... Args>
    void call_method(lua_State *L, std::string const &method, Args... args)
    {
        get_method(L, method);
        auto count = foreach(Pusher{L}, args...);
        checkerror(L, lua_pcall(L, 1 + count, 0, 0));
    }

    /**
     * Add the value at the top of the stack to the Lua registry, using KEY as
     * the index.
     */
    template<typename T>
    void add_to_registry(lua_State *L, T key)
    {
        lua_pushvalue(L, LUA_REGISTRYINDEX);
        Pusher{L}(key);
        lua_pushvalue(L, -3);
        lua_settable(L, -3);
        lua_pop(L, 1);
    }

    /** Get a value from the Lua registry, using KEY. */
    template<typename T>
    void get_from_registry(lua_State *L, T key)
    {
        lua_pushvalue(L, LUA_REGISTRYINDEX);
        Pusher{L}(key);
        lua_gettable(L, -2);
        lua_remove(L, -2);
    }
}

#endif
