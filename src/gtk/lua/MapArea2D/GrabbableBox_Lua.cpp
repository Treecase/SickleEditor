/**
 * GrabbableBox_Lua.cpp - GrabbableBox Lua binding.
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

#include "MapArea2D_Lua.hpp"

#include <gbox/GrabbableBox.hpp>
#include <se-lua/lua-geo/LuaGeo.hpp>
#include <se-lua/utils/RefBuilder.hpp>

#define METATABLE "Sickle.gtk.maparea2d.grabbablebox"


using namespace Sickle;


////////////////////////////////////////////////////////////////////////////////
// Methods
static int check_point(lua_State *L)
{
    auto box = lgrabbablebox_check(L, 1);
    auto const xy = lgeo_checkvector<glm::vec2>(L, 2);
    lua_pushinteger(L, box->check_point(xy));
    return 1;
}

static luaL_Reg methods[] = {
    {"check_point", check_point},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// C++ facing
template<>
void Lua::push(lua_State *L, GrabbableBox *box)
{
    Lua::RefBuilder builder{L, METATABLE, box};
    if (builder.pushnew())
        return;
    builder.finish();
}

GrabbableBox *lgrabbablebox_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, METATABLE);
    luaL_argcheck(L, ud != NULL, arg, "`" METATABLE "' expected");
    return *static_cast<GrabbableBox **>(ud);
}

int luaopen_grabbablebox(lua_State *L)
{
    // Export enum values.
    Lua::make_table(L,
        std::make_pair("NONE", (lua_Integer)GrabbableBox::Area::NONE),
        std::make_pair("CENTER", (lua_Integer)GrabbableBox::Area::CENTER),
        std::make_pair("N", (lua_Integer)GrabbableBox::Area::N),
        std::make_pair("NE", (lua_Integer)GrabbableBox::Area::NE),
        std::make_pair("E", (lua_Integer)GrabbableBox::Area::E),
        std::make_pair("SE", (lua_Integer)GrabbableBox::Area::SE),
        std::make_pair("S", (lua_Integer)GrabbableBox::Area::S),
        std::make_pair("SW", (lua_Integer)GrabbableBox::Area::SW),
        std::make_pair("W", (lua_Integer)GrabbableBox::Area::W),
        std::make_pair("NW", (lua_Integer)GrabbableBox::Area::NW)
    );

    luaL_newmetatable(L, METATABLE);
    luaL_setfuncs(L, methods, 0);
    lua_pushvalue(L, -1);
    lua_setfield(L, -1, "__index");
    lua_setfield(L, -2, "metatable");

    Lua::RefBuilder<GrabbableBox>::setup_indexing(L, METATABLE);
    return 1;
}
