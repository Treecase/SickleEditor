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

#include <memory>
#include <unordered_map>


namespace Lua
{
    /**
     * Singleton mapping pointers to Lua values.
     */
    class ReferenceManager
    {
        struct LuaDeleter {void operator()(lua_State *L) const {lua_close(L);}};

        static std::unique_ptr<lua_State, LuaDeleter> const _L_actual;
        static lua_State *const _L;
        static std::unordered_map<void *, int> _references;

    public:
        /**
         * Create a reference from POINTER to the Lua value at stack IDX.
         *
         * An existing mapping will be silently overwritten.
         */
        void set(lua_State *L, void *pointer, int idx);

        /** Push the Lua value referenced by POINTER to the stack. */
        void get(lua_State *L, void *pointer);

        /** Delete the Lua reference. */
        void unref(lua_State *L, void *pointer);
    };
}

#endif
