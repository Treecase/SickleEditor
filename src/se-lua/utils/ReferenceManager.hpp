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

#include "Referenceable.hpp"

#include <se-lua/se-lua.hpp>

#include <glibmm/refptr.h>

#include <memory>
#include <unordered_map>

namespace Lua
{
    /**
     * Singleton mapping pointers to Lua values.
     */
    class ReferenceManager
    {
        // pushes the reftable (and creates it if it doesn't exists)
        static void pushRefTable(lua_State *L);

    public:
        /**
         * Create a reference from POINTER to the Lua value at stack IDX.
         *
         * An existing mapping will be silently overwritten.
         */
        void set(lua_State *L, Referenceable *pointer, int idx);

        template<typename T>
        void set(lua_State *L, Glib::RefPtr<T> const &pointer, int idx)
        {
            set(L, pointer.get(), idx);
        }

        /** Push the Lua value referenced by POINTER to the stack. */
        void get(lua_State *L, Referenceable *pointer);

        template<typename T>
        void get(lua_State *L, Glib::RefPtr<T> const &pointer)
        {
            get(L, pointer.get());
        }

        /** Delete a reference. */
        void erase(lua_State *L, Referenceable *pointer);

        template<typename T>
        void erase(lua_State *L, Glib::RefPtr<T> const &pointer)
        {
            erase(L, pointer.get());
        }
    };
} // namespace Lua

#endif
