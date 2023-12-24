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
#include <functional>

#include <lua.hpp>

#include <stdexcept>
#include <string>
#include <utility>


namespace Lua
{
    struct Error : public std::runtime_error
    {
        Error(std::string const &what);
    };

    struct StackOverflow : public Error
    {
        StackOverflow(std::string const &what);
    };


    /** Push a value onto the stack. */
    template<typename T> void push(lua_State *L, T value)=delete;
    void push(lua_State *L, bool value);
    void push(lua_State *L, lua_Integer value);
    void push(lua_State *L, lua_Number value);
    void push(lua_State *L, char const *value);
    void push(lua_State *L, std::string const &value);

    /** Get a value from the stack. */
    template<typename T>
    T get_as(lua_State *L, int idx)=delete;

    struct Pusher
    {
        lua_State *L;
        template<typename T> void operator()(T value) {push(L, value);}
    };


    /**
     * Set the error handler for this lua_State. The default handler just
     * throws a Lua::Error.
     */
    void set_error_handler(lua_State *L, std::function<void(lua_State *)> fn);

    /**
     * Restore default error handler for this lua_State. The default handler
     * just throws a Lua::Error.
     */
    void clear_error_handler(lua_State *L);

    /** Check a Lua status error. */
    void checkerror(lua_State *L, int status);


    /**
     * Call the function at the top of the stack in protected mode. Uses the
     * defined __msgh function for the state if it exists.
     *
     * @param L The Lua context.
     * @param nargs Number of arguments to the function (same as lua_pcall).
     * @param nresults Number of results pushed by the function (same as
     * lua_pcall).
     * @return Lua status code (same as lua_pcall).
     */
    int pcall(lua_State *L, int nargs, int nresults);

    /**
     * Call the function at the top of the stack in protected mode. Uses the
     * defined __msgh function for the state if it exists.
     *
     * @param L The Lua context.
     * @param nresults Number of results pushed by the function (same as
     * lua_pcall).
     * @param args Function arguments.
     * @return Lua status code (same as lua_pcall).
     */
    template<typename... Args>
    int pcallT(lua_State *L, int nresults, Args... args)
    {
        auto count = foreach(Pusher{L}, args...);
        return pcall(L, count, nresults);
    }


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
        checkerror(L, pcall(L, 1 + count, 0));
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
        checkerror(L, pcall(L, 1 + count, r));
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

template<> bool Lua::get_as(lua_State *L, int idx);
template<> lua_Integer Lua::get_as(lua_State *L, int idx);
template<> lua_Number Lua::get_as(lua_State *L, int idx);
template<> char const *Lua::get_as(lua_State *L, int idx);
template<> std::string Lua::get_as(lua_State *L, int idx);

#endif
