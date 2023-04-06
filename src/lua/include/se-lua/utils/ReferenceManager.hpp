/**
 * ReferenceManager.hpp - Manager for C++-to-Lua references.
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

#ifndef SE_LUA_REFERENCEMANAGER_HPP
#define SE_LUA_REFERENCEMANAGER_HPP

#include "../se-lua.hpp"


namespace Lua
{
    class ReferenceManager
    {
        lua_State *L{nullptr};
    public:
        virtual ~ReferenceManager()
        {
            destroy();
        }

        void init(lua_State *_L)
        {
            if (L != nullptr)
                throw std::runtime_error{"attempted to re-init a Lua::ReferenceManager"};
            L = _L;
            // Add reference table to Lua registry.
            lua_pushlightuserdata(L, this);
            lua_newtable(L);
            lua_settable(L, LUA_REGISTRYINDEX);
        }

        void destroy()
        {
            if (L == nullptr)
                return;
            // Remove reference table from Lua registry.
            lua_pushlightuserdata(L, this);
            lua_pushnil(L);
            lua_settable(L, LUA_REGISTRYINDEX);
            L = nullptr;
        }

        /** Make POINTER a reference to VALUE. */
        void set(void *pointer, int value)
        {
            if (value < 0)
                value = lua_gettop(L) + value + 1;
            // Get reference table.
            lua_pushlightuserdata(L, this);
            lua_gettable(L, LUA_REGISTRYINDEX);
            // Add the reference to the table.
            lua_pushlightuserdata(L, pointer);
            lua_pushvalue(L, value);
            lua_settable(L, -3);
            // Clean up.
            lua_pop(L, 1);
        }

        /** Get the Lua value referenced by POINTER. */
        void get(void *pointer)
        {
            // Get reference table.
            lua_pushlightuserdata(L, this);
            lua_gettable(L, LUA_REGISTRYINDEX);
            // Get the reference from the table.
            lua_pushlightuserdata(L, pointer);
            lua_gettable(L, -2);
            // Clean up.
            lua_remove(L, -2);
        }
    };
}

#endif
