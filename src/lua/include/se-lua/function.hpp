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
}

/** Push a function to the stack. */
template<>
void Lua::push(lua_State *L, Function fn);

namespace Lua
{
    /**
     * Call the function at the top of the stack in protected mode, with an
     * error handler.
     *
     * @param L The Lua context.
     * @param nresults Number of results pushed by the function.
     * @param msgh Error handler.
     * @param args Function arguments.
     */
    template<typename... Args>
    void pcall(lua_State *L, int nresults, Function msgh, Args... args)
    {
        Lua::push<Function>(L, msgh);
        lua_rotate(L, -2, 1);
        auto const msgh_idx = lua_absindex(L, -2);
        auto const count = foreach(Pusher{L}, args...);
        checkerror(L, lua_pcall(L, count, nresults, msgh_idx));
    }
}

#endif
