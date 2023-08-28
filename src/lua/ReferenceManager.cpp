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


static char _KEY = 'k';
static void *const REFTABLE_KEY = &_KEY;


void ReferenceManager::set(lua_State *L, Referenceable *pointer, int idx)
{
    auto const index = lua_absindex(L, idx);
    pushRefTable(L);
    lua_pushlightuserdata(L, pointer->get_id());
    lua_pushvalue(L, index);
    lua_settable(L, -3);
    lua_pop(L, 1);
    pointer->signal_destroy().connect(
        [this, L, pointer](){this->erase(L, pointer);});
}


void ReferenceManager::get(lua_State *L, Referenceable *pointer)
{
    pushRefTable(L);
    lua_pushlightuserdata(L, pointer->get_id());
    lua_gettable(L, -2);
    lua_remove(L, -2);
}


void ReferenceManager::erase(lua_State *L, Referenceable *pointer)
{
    pushRefTable(L);
    lua_pushlightuserdata(L, pointer->get_id());
    lua_pushnil(L);
    lua_settable(L, -3);
    lua_pop(L, 1);
}



void ReferenceManager::pushRefTable(lua_State *L)
{
    lua_pushlightuserdata(L, REFTABLE_KEY);
    int const type = lua_gettable(L, LUA_REGISTRYINDEX);
    if (type != LUA_TTABLE)
    {
        lua_pop(L, 1);
        lua_newtable(L);
        lua_pushlightuserdata(L, REFTABLE_KEY);
        lua_pushvalue(L, -2);
        lua_settable(L, LUA_REGISTRYINDEX);
    }
}
