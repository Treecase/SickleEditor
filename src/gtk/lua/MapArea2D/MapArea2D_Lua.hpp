/**
 * MapArea2D_Lua.hpp - MapArea2D Lua binding.
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

#ifndef SE_MAPAREA2D_LUA_HPP
#define SE_MAPAREA2D_LUA_HPP

#include "../../classes/MapArea2D.hpp"

#include <se-lua/se-lua.hpp>


int luaopen_maparea2d(lua_State *L);
Sickle::MapArea2D *lmaparea2d_check(lua_State *L, int arg);

int luaopen_grabbablebox(lua_State *L);
Sickle::GrabbableBox *lgrabbablebox_check(lua_State *L, int arg);

int luaopen_transform2d(lua_State *L);
Sickle::MapArea2Dx::Transform2D *ltransform2d_check(lua_State *L, int arg);

template<> void Lua::push(lua_State *L, Sickle::MapArea2D *maparea);
template<> void Lua::push(lua_State *L, Sickle::GrabbableBox *box);
namespace Lua
{
    void push(lua_State *L, Sickle::MapArea2Dx::Transform2D transform);
}

#endif
