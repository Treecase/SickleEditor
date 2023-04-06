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

#include "../se-lua.hpp"
#include "ReferenceManager.hpp"


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
        luaL_getmetatable(L, lua_tostring(L, lua_upvalueindex(1)));
        lua_pushvalue(L, 2);
        lua_gettable(L, -2);
    }
    return 1;
}


namespace Lua
{
    template<class T>
    class RefBuilder
    {
    private:
        std::string const _library;
        lua_State *L{nullptr};
        T *_object{nullptr};
        Lua::ReferenceManager _refman{};

    public:
        RefBuilder(std::string const &library)
        :   _library{library}
        {
        }

        /**
         * Must be called after creating the library's metatable.
         * Overrides the metatable's __index and __newindex.
         */
        void setLua(lua_State *nL)
        {
            if (nL == L)
                return;
            L = nL;
            _refman.destroy();
            _refman.init(nL);

            luaL_getmetatable(L, _library.c_str());

            lua_pushcfunction(L, _refbuilder_dunder_newindex);
            lua_setfield(L, -2, "__newindex");

            lua_pushstring(L, _library.c_str());
            lua_pushcclosure(L, _refbuilder_dunder_index, 1);
            lua_setfield(L, -2, "__index");

            lua_pop(L, 1);
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
        bool pushnew(T *object)
        {
            _refman.get(object);
            if (!lua_isnil(L, -1))
                return true;
            else
                lua_pop(L, 1);

            _object = object;
            auto ptr = static_cast<T const **>(
                lua_newuserdatauv(L, sizeof(T *), 1));
            *ptr = _object;
            luaL_setmetatable(L, _library.c_str());

            lua_newtable(L);
            lua_setiuservalue(L, -2, 1);

            return false;
        }

        /** Finish building the object. */
        void finish()
        {
            _refman.set(_object, -1);
            _object = nullptr;
        }

    private:
        template<class Signal, typename R, typename... A>
        void _addSignalHandler_low(Signal sig, char const *fn_name)
        {
            auto cached_L = L;
            auto cached_object = _object;
            auto conn = sig.connect(
                [this, cached_L, cached_object, fn_name](A... args){
                    _refman.get(cached_object);
                    Lua::call_method_r(cached_L, 1, fn_name, args...);
                    return Lua::get_as<R>(L, -1);
                }
            );
        }

        template<class Signal, typename... A>
        void _addSignalHandler_low_noret(Signal sig, char const *fn_name)
        {
            auto cached_L = L;
            auto cached_object = _object;
            auto conn = sig.connect(
                [this, cached_L, cached_object, fn_name](A... args){
                    _refman.get(cached_object);
                    Lua::call_method(cached_L, fn_name, args...);
                }
            );
        }
    };
}
