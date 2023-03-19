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

#include "../../classes/MapArea2D.hpp"
#include "MapArea2D_Lua.hpp"

#include <se-lua/lua-utils.hpp>
#include <LuaGeo.hpp>


static Lua::ReferenceManager refman{};


////////////////////////////////////////////////////////////////////////////////
// Methods
static int check_point(lua_State *L)
{
    auto box = lgrabbablebox_check(L, 1);
    auto xy = lgeo_tovector(L, 2);
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
void Lua::push(lua_State *L, Sickle::GrabbableBox *box)
{
    refman.get(box);
    if (!lua_isnil(L, -1))
        return;
    else
        lua_pop(L, 1);

    auto ptr = static_cast<Sickle::GrabbableBox const **>(
        lua_newuserdatauv(L, sizeof(Sickle::GrabbableBox *), 0));
    *ptr = box;
    luaL_setmetatable(L, "Sickle.maparea2d.grabbablebox");

    refman.set(box, -1);
}

Sickle::GrabbableBox *lgrabbablebox_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, "Sickle.maparea2d.grabbablebox");
    luaL_argcheck(L, ud != NULL, arg,
        "`Sickle.maparea2d.grabbablebox' expected");
    return *static_cast<Sickle::GrabbableBox **>(ud);
}

int luaopen_grabbablebox(lua_State *L)
{
    refman.init(L);

    luaL_newmetatable(L, "Sickle.maparea2d.grabbablebox");
    luaL_setfuncs(L, methods, 0);
    lua_setfield(L, -1, "__index");

    // Export enum values.
    lua_newtable(L);
    Lua::set_table(L, "NONE", (lua_Integer)Sickle::GrabbableBox::Area::NONE);
    Lua::set_table(L, "BOX", (lua_Integer)Sickle::GrabbableBox::Area::BOX);
    Lua::set_table(L, "N", (lua_Integer)Sickle::GrabbableBox::Area::N);
    Lua::set_table(L, "NE", (lua_Integer)Sickle::GrabbableBox::Area::NE);
    Lua::set_table(L, "E", (lua_Integer)Sickle::GrabbableBox::Area::E);
    Lua::set_table(L, "SE", (lua_Integer)Sickle::GrabbableBox::Area::SE);
    Lua::set_table(L, "S", (lua_Integer)Sickle::GrabbableBox::Area::S);
    Lua::set_table(L, "SW", (lua_Integer)Sickle::GrabbableBox::Area::SW);
    Lua::set_table(L, "W", (lua_Integer)Sickle::GrabbableBox::Area::W);
    Lua::set_table(L, "NW", (lua_Integer)Sickle::GrabbableBox::Area::NW);

    return 1;
}
