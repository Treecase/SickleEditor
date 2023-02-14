/**
 * se-lua.hpp - Sickle Lua integration.
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

#ifndef SE_LUA_HPP
#define SE_LUA_HPP

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include <stdexcept>
#include <string>


namespace Lua
{
    struct Error : public std::runtime_error
    {
        Error(std::string const &what);
    };


    /** Check a Lua status error. */
    void checkerror(lua_State *L, int status);

    /** Expects a table/full userdata to be at the top of the stack. */
    void call_method(lua_State *L, std::string const &method);
}

#endif
