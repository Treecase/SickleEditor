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

#include "se-lua/function.hpp"

#include <memory>
#include <cstring>


static int __call(lua_State *L)
{
    auto const ptr = Lua::checkfunction(L, 1);
    if (!ptr)
        return lua_error(L);
    auto const fn = *ptr;
    lua_remove(L, 1);
    return std::invoke(fn, L);
}


static luaL_Reg methods[] = {
    {"__call", __call},
    {NULL, NULL}
};


int Lua::luaopen_function(lua_State *L)
{
    luaL_newmetatable(L, "Lua.function");
    luaL_setfuncs(L, methods, 0);
    lua_pop(L, 1);
    return 0;
}


Lua::Function *Lua::checkfunction(lua_State *L, int arg)
{
    auto fn = luaL_checkudata(L, arg, "Lua.function");
    luaL_argcheck(L, fn != nullptr, arg, "`function' expected");
    return static_cast<std::unique_ptr<Lua::Function> *>(fn)->get();
}


template<>
void Lua::push(lua_State *L, Function msgh)
{
    auto fn = static_cast<std::unique_ptr<Function> *>(
        lua_newuserdatauv(L, sizeof(std::unique_ptr<Function>), 0));
    memset(fn, 0, sizeof(*fn));
    fn->reset(new Function{msgh});
    luaL_getmetatable(L, "Lua.function");
    lua_setmetatable(L, -2);
}
