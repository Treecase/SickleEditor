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

#include "../classes/MapArea2D.hpp"
#include "MapArea2D_Lua.hpp"


#define LIBRARY_NAME    "Sickle.grabbablebox"
#define CLASSNAME       Sickle::GrabbableBox


/** Add value at the top of the stack to the objectTable using KEY. */
static void add_to_objectTable(lua_State *L, CLASSNAME *key)
{
    Lua::get_from_registry(L, LIBRARY_NAME".objectTable");
    lua_pushlightuserdata(L, key);
    lua_pushvalue(L, -3);
    lua_settable(L, -3);
    lua_pop(L, 1);
}

/** Get the Lua value associated with KEY from the objectTable. */
static void get_from_objectTable(lua_State *L, CLASSNAME *key)
{
    Lua::get_from_registry(L, LIBRARY_NAME".objectTable");
    lua_pushlightuserdata(L, key);
    lua_gettable(L, -2);
    lua_remove(L, -2);
}


////////////////////////////////////////////////////////////////////////////////
// Methods
static int check_point(lua_State *L)
{
    auto box = lgrabbablebox_check(L, 1);
    auto x = luaL_checknumber(L, 2);
    auto y = luaL_checknumber(L, 3);
    lua_pushinteger(L, box->check_point({x, y}));
    return 1;
}

static luaL_Reg methods[] = {
    {"check_point", check_point},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// C++ facing
int lgrabbablebox_new(lua_State *L, CLASSNAME *box)
{
    // If we've already built an object for this pointer, just reuse it.
    get_from_objectTable(L, box);
    if (!lua_isnil(L, -1))
        return 1;
    else
        lua_pop(L, 1);

    // Create the Lua object.
    auto ptr = static_cast<CLASSNAME const **>(
        lua_newuserdatauv(L, sizeof(CLASSNAME *), 0));
    *ptr = box;

    // Set metatable.
    luaL_setmetatable(L, LIBRARY_NAME);

    // Add the object to the Lua registry, using the pointer as key. This is
    // needed for the C++ callbacks to know what object to call methods on.
    add_to_objectTable(L, box);

    return 1;
}

CLASSNAME *lgrabbablebox_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, LIBRARY_NAME);
    luaL_argcheck(L, ud != NULL, arg, "`" LIBRARY_NAME "' expected");
    return *static_cast<CLASSNAME **>(ud);
}

int luaopen_grabbablebox(lua_State *L)
{
    // Table used to map C++ pointers to Lua objects.
    // TODO: References should be removed when the C++ objects are destroyed.
    lua_newtable(L);
    Lua::add_to_registry(L, LIBRARY_NAME".objectTable");
    lua_pop(L, 1);

    luaL_newmetatable(L, LIBRARY_NAME);
    luaL_setfuncs(L, methods, 0);
    lua_pushliteral(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);

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
