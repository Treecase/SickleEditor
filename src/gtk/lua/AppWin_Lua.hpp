/**
 * AppWin_Lua.hpp - AppWin Lua binding.
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

#ifndef SE_APPWIN_LUA_HPP
#define SE_APPWIN_LUA_HPP

#include <se-lua/se-lua.hpp>

namespace Sickle::AppWin
{
    class AppWin;
}

int luaopen_appwin(lua_State *L);
Sickle::AppWin::AppWin *lappwin_check(lua_State *L, int arg);

template<>
void Lua::push(lua_State *L, Sickle::AppWin::AppWin *appwin);

#endif
