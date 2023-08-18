/**
 * ReferenceManager.cpp - Manager for C++-to-Lua references.
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

#include "se-lua/utils/ReferenceManager.hpp"

using namespace Lua;


std::unique_ptr<lua_State, ReferenceManager::LuaDeleter> const
ReferenceManager::_L_actual{luaL_newstate()};
lua_State *const ReferenceManager::_L{ReferenceManager::_L_actual.get()};
std::unordered_map<void *, int> ReferenceManager::_references{};


void ReferenceManager::set(lua_State *L, void *pointer, int idx)
{
    if (_references.count(pointer) != 0)
        unref(L, pointer);

    lua_pushvalue(L, idx);
    lua_xmove(L, _L, 1);
    int const r = luaL_ref(_L, LUA_REGISTRYINDEX);
    _references.insert({pointer, r});
}


void ReferenceManager::get(lua_State *L, void *pointer)
{
    int const r = _references.at(pointer);
    lua_rawgeti(_L, LUA_REGISTRYINDEX, r);
    lua_xmove(_L, L, 1);
}


void ReferenceManager::unref(lua_State *L, void *pointer)
{
    try {
        int const r = _references.at(pointer);
        luaL_unref(_L, LUA_REGISTRYINDEX, r);
        _references.erase(pointer);
    }
    catch (std::out_of_range const &e) {
    }
}
