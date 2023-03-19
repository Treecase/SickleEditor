/**
 * AppWin_Lua.cpp - AppWin Lua binding.
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

#include "../classes/AppWin.hpp"
#include "AppWin_Lua.hpp"
#include "LuaGdkEvent.hpp"
#include "MapArea2D_Lua.hpp"
#include "MapArea3D_Lua.hpp"

#include <se-lua/lua-utils.hpp>


static Lua::ReferenceManager refman{};


////////////////////////////////////////////////////////////////////////////////
// Metamethods
static int dunder_newindex(lua_State *L)
{
    lappwin_check(L, 1);
    lua_getiuservalue(L, -3, 1);
    lua_rotate(L, -3, 1);
    lua_settable(L, -3);
    return 0;
}

static int dunder_index(lua_State *L)
{
    lappwin_check(L, 1);
    lua_getiuservalue(L, 1, 1);
    lua_rotate(L, -2, 1);
    lua_gettable(L, -2);
    return 1;
}

static luaL_Reg metamethods[] = {
    {"__newindex", dunder_newindex},
    {"__index", dunder_index},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// Methods
static int set_grid_size(lua_State *L)
{
    auto aw = lappwin_check(L, 1);
    aw->set_grid_size(luaL_checknumber(L, 2));
    return 0;
}

static int get_grid_size(lua_State *L)
{
    auto aw = lappwin_check(L, 1);
    lua_pushinteger(L, aw->get_grid_size());
    return 1;
}

static int do_nothing(lua_State *L)
{
    return 0;
}

static luaL_Reg methods[] = {
    {"set_grid_size", set_grid_size},
    {"get_grid_size", get_grid_size},

    {"on_grid_size_changed", do_nothing},
    {"on_key_press_event", do_nothing},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// C++ facing
template<>
void Lua::push(lua_State *L, Sickle::AppWin *appwin)
{
    refman.get(appwin);
    if (!lua_isnil(L, -1))
        return;
    else
        lua_pop(L, 1);

    auto ptr = static_cast<Sickle::AppWin const **>(
        lua_newuserdatauv(L, sizeof(Sickle::AppWin *), 1));
    *ptr = appwin;
    luaL_setmetatable(L, "Sickle.appwin");

    // Add methods/data table.
    // TODO: methods should be shared, data should be individual.
    lua_newtable(L);
    luaL_setfuncs(L, methods, 0);
    lua_setiuservalue(L, -2, 1);

    // Add fields.
    Lua::push(L, &appwin->m_maparea);
    lua_setfield(L, -2, "mapArea3D");

    Lua::push(L, &appwin->m_drawarea_top);
    lua_setfield(L, -2, "topMapArea");

    Lua::push(L, &appwin->m_drawarea_front);
    lua_setfield(L, -2, "frontMapArea");

    Lua::push(L, &appwin->m_drawarea_right);
    lua_setfield(L, -2, "rightMapArea");

    // Connect signals.
    appwin->property_grid_size().signal_changed().connect(
        [L, appwin](){
            refman.get(appwin);
            Lua::call_method(L, "on_grid_size_changed");
        });
    appwin->signal_key_press_event().connect(
        [L, appwin](GdkEventKey const *e){
            refman.get(appwin);
            Lua::call_method(L, "on_key_press_event", e);
            return lua_toboolean(L, -1);
        });

    refman.set(appwin, -1);
}

Sickle::AppWin *lappwin_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, "Sickle.appwin");
    luaL_argcheck(L, ud != NULL, arg, "`Sickle.appwin' expected");
    return *static_cast<Sickle::AppWin **>(ud);
}

int luaopen_appwin(lua_State *L)
{
    luaL_requiref(L, "maparea2d", luaopen_maparea2d, 1);
    luaL_requiref(L, "maparea3d", luaopen_maparea3d, 1);

    // TODO: References should be removed when the C++ objects are destroyed.
    refman.init(L);

    luaL_newmetatable(L, "Sickle.appwin");
    luaL_setfuncs(L, metamethods, 0);

    return 0;
}
