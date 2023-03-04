/**
 * MapArea3D_Lua.hpp - MapArea3D Lua binding.
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

#ifndef SE_MAPAREA3D_LUA_HPP
#define SE_MAPAREA3D_LUA_HPP

#include "se-lua/se-lua.hpp"
#include "../../classes/MapArea3D.hpp"
#include "utils/FreeCam.hpp"


int luaopen_maparea3d(lua_State *L);
int lmaparea3d_new(lua_State *L, Sickle::MapArea3D *maparea);
Sickle::MapArea3D *lmaparea3d_check(lua_State *L, int arg);

int luaopen_freecam(lua_State *L);
template<> void Lua::Pusher::operator()(FreeCam camera);
FreeCam *lfreecam_check(lua_State *L, int arg);

int luaopen_maparea3d_state(lua_State *L);
template<> void Lua::Pusher::operator()(Sickle::MapArea3D::State state);
Sickle::MapArea3D::State *lmaparea3d_state_check(lua_State *L, int arg);

#endif
