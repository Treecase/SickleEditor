/**
 * RefBuilder.hpp - Build reference-style Lua objects.
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

#ifndef SE_LUA_REFBUILDER_HPP
#define SE_LUA_REFBUILDER_HPP

#include "../se-lua.hpp"
#include "ReferenceManager.hpp"

#include <cstring>


static int _refbuilder_dunder_newindex(lua_State *L)
{
    lua_getiuservalue(L, -3, 1);
    lua_rotate(L, -3, 1);
    lua_settable(L, -3);
    return 0;
}

static int _refbuilder_dunder_index(lua_State *L)
{
    // Try data table first.
    lua_getiuservalue(L, 1, 1);
    lua_pushvalue(L, 2);
    lua_gettable(L, -2);
    // Not in data table, try the metatable.
    if (lua_isnil(L, -1))
    {
        lua_pop(L, 2);
        if (lua_getmetatable(L, 1) == 0)
            return 0;
        lua_pushvalue(L, 2);
        lua_gettable(L, -2);
    }
    return 1;
}


namespace Lua
{
    template<class PointerType>
    class RefBuilder
    {
    private:
        std::string const _library;
        lua_State *const L;
        PointerType _pointer;
        Lua::ReferenceManager _refman{};

    public:
        /** Set up __index and __newindex methods. */
        static void setup_indexing(lua_State *L, std::string const &metatable)
        {
            luaL_getmetatable(L, metatable.c_str());
            if (lua_isnil(L, -1))
                throw std::runtime_error{"no metatable '" + metatable + "'"};

            lua_pushcfunction(L, _refbuilder_dunder_newindex);
            lua_setfield(L, -2, "__newindex");

            lua_pushcfunction(L, _refbuilder_dunder_index);
            lua_setfield(L, -2, "__index");

            lua_pop(L, 1);
        }

        RefBuilder(
            lua_State *L,
            std::string const &library,
            PointerType pointer)
        :   _library{library}
        ,   _pointer{pointer}
        ,   L{L}
        {
        }

        /** Add a data field to the object. */
        template<typename K, typename V>
        void addField(K key, V value)
        {
            Lua::set_table(L, key, value);
        }

        /** Add a signal handler. */
        template<template<typename> class Signal, typename R, typename... A>
        void addSignalHandler(Signal<R(A...)> sig, char const *fn_name)
        {
            _addSignalHandler_low<Signal<R(A...)>, R, A...>(sig, fn_name);
        }
        template<
            template<typename, typename...> class Signal,
            typename R, typename... A>
        void addSignalHandler(Signal<R, A...> sig, char const *fn_name)
        {
            _addSignalHandler_low<Signal<R, A...>, R, A...>(sig, fn_name);
        }

        /** Add a signal handler without a return value. */
        template<template<typename> class Signal, typename... A>
        void addSignalHandler(Signal<void(A...)> sig, char const *fn_name)
        {
            _addSignalHandler_low_noret<Signal<void(A...)>, A...>(sig, fn_name);
        }
        template<template<typename, typename...> class Signal, typename... A>
        void addSignalHandler(Signal<void, A...> sig, char const *fn_name)
        {
            _addSignalHandler_low_noret<Signal<void, A...>, A...>(sig, fn_name);
        }

        /** Add a signal handler with no return value and no arguments. */
        template<class Signal>
        void addSignalHandler(Signal sig, char const *fn_name)
        {
            _addSignalHandler_low_noret(sig, fn_name);
        }

        /** Push a new reference-style object onto the stack. */
        bool pushnew()
        {
            _refman.get(L, _pointer);
            int const type = lua_type(L, -1);
            switch (type)
            {
            case LUA_TUSERDATA:
                return true;
            case LUA_TNIL:
                lua_pop(L, 1);
                break;
            default:
                lua_pop(L, 1);
                throw Lua::Error{
                    "refman.get() pushed the wrong type: "
                    + std::string{lua_typename(L, type)}};
                return false;
            }

            auto ptr = static_cast<PointerType *>(
                lua_newuserdatauv(L, sizeof(PointerType), 1));
            std::memset(ptr, 0, sizeof(*ptr));
            *ptr = _pointer;
            luaL_setmetatable(L, _library.c_str());

            lua_newtable(L);
            lua_setiuservalue(L, -2, 1);

            return false;
        }

        /** Finish building the object. */
        void finish()
        {
            _refman.set(L, _pointer, -1);
        }

    private:
        template<class Signal, typename R, typename... A>
        void _addSignalHandler_low(Signal sig, char const *fn_name)
        {
            auto cL = L;
            auto cP = _pointer;
            auto conn = sig.connect(
                [cL, cP, fn_name](A... args){
                    ReferenceManager refman{};
                    refman.get(cL, cP);
                    Lua::call_method_r(cL, 1, fn_name, args...);
                    return Lua::get_as<R>(cL, -1);
                }
            );
        }

        template<class Signal, typename... A>
        void _addSignalHandler_low_noret(Signal sig, char const *fn_name)
        {
            auto cL = L;
            auto cP = _pointer;
            auto conn = sig.connect(
                [cL, cP, fn_name](A... args){
                    ReferenceManager refman{};
                    refman.get(cL, cP);
                    Lua::call_method(cL, fn_name, args...);
                }
            );
        }
    };
}

#endif
