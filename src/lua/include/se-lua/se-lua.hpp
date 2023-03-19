/**
 * se-lua.hpp - Sickle Lua integration.
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

#ifndef SE_LUA_HPP
#define SE_LUA_HPP

#include <utils/TemplateUtils.hpp>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <stdexcept>
#include <string>
#include <utility>


namespace Lua
{
    struct Error : public std::runtime_error
    {
        Error(std::string const &what);
    };


    /** Push a value onto the stack. */
    template<typename T>
    void push(lua_State *L, T *value)=delete;
    void push(lua_State *L, bool value);
    void push(lua_State *L, lua_Integer value);
    void push(lua_State *L, lua_Number value);
    void push(lua_State *L, char const *value);
    void push(lua_State *L, std::string const &value);

    struct Pusher
    {
        lua_State *L;
        template<typename T> void operator()(T value) {push(L, value);}
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
     * Call METHOD on the value at the top of the stack. R is the number of
     * return values.
     */
    template<typename... Args>
    void call_method_r(lua_State *L, int r, std::string const &method, Args... args)
    {
        get_method(L, method);
        auto count = foreach(Pusher{L}, args...);
        checkerror(L, lua_pcall(L, 1 + count, r, 0));
    }


    /** Set table[KEY] = VALUE. Table is at the top of the stack. */
    template<typename K, typename V>
    void set_table(lua_State *L, K key, V value)
    {
        push(L, key);
        push(L, value);
        lua_settable(L, -3);
    }

    /** Push a new table with the given key/value pairs installed.*/
    template<typename... Keys, typename... Values>
    void make_table(lua_State *L, std::pair<Keys, Values>... c)
    {
        lua_newtable(L);
        (set_table(L, c.first, c.second), ...);
    }
}

#endif
