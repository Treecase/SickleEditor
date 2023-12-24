/**
 * function.hpp - Lua Function objects.
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

#ifndef SE_LUA_FUNCTION_HPP
#define SE_LUA_FUNCTION_HPP

#include "se-lua.hpp"

#include <functional>


namespace Lua
{
    using Function = std::function<int(lua_State *)>;

    /** Open the Lua.function library. */
    int luaopen_function(lua_State *L);

    /**
     * Check that the value at stack index `arg` is a Function.
     *
     * @param L The Lua state.
     * @param arg Stack index of the value to check.
     * @return Pointer to the Function object, or null if the value is not a
     * function.
     */
    Function *checkfunction(lua_State *L, int arg);


    /**
     * Set the default message handler function used by Lua::pcall. This
     * function is called whenever an error occurs, but before the stack
     * unwinds. This allows the user to collect more information about the
     * error. The function is called with 1 argument, the error object, and
     * returns 1 value, the (possibly new/modified) error object.
     *
     * @param L The Lua state.
     * @param msgh The new message handler.
     */
    void set_msgh(lua_State *L, Function msgh);
}

/** Push a function to the stack. */
template<>
void Lua::push(lua_State *L, Function fn);

#endif
