/**
 * MapArea2D_Lua.hpp - MapArea2D Lua binding.
 * Copyright (C) 2022 Trevor Last
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

#include "se-lua/se-lua.hpp"


namespace Sickle
{
    class MapArea2D;
    int lmaparea2d_new_no_signals(
        lua_State *L, Sickle::MapArea2D const *maparea);
    int lmaparea2d_new(lua_State *L, Sickle::MapArea2D const *maparea);
}

int luaopen_maparea2d(lua_State *L);

#endif
