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

#include "../classes/appwin/AppWin.hpp"
#include "AppWin_Lua.hpp"
#include "LuaGdkEvent.hpp"
#include "MapArea2D_Lua.hpp"
#include "MapArea3D_Lua.hpp"

#include <se-lua/utils/RefBuilder.hpp>
#include <core/MapTools.hpp>

#define METATABLE "Sickle.gtk.appwin"


using namespace Sickle::AppWin;


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

static int get_maptool(lua_State *L)
{
    auto aw = lappwin_check(L, 1);
    auto maptool = aw->editor->get_maptool();
    lua_pushstring(L, maptool.name().c_str());
    return 1;
}

static int add_maptool(lua_State *L)
{
    auto aw = lappwin_check(L, 1);
    auto const name = luaL_checkstring(L, 2);

    // Argument 3 is an array of opdefs.
    std::vector<Sickle::Editor::MapTool::OpDef> opdefs{};
    for (lua_Integer i = 1; ; ++i)
    {
        int const type = lua_geti(L, 3, i); // opdef
        if (type == LUA_TNIL)
            break;
        lua_geti(L, -1, 1); // operation label
        lua_geti(L, -2, 2); // operation id
        auto const label = lua_tostring(L, -2);
        auto const id = lua_tostring(L, -1);
        lua_pop(L, 3);
        opdefs.push_back(Sickle::Editor::MapTool::OpDef{label, id});
    }

    // Argument 4 is the "Should Popup?" function.
    auto const ref = luaL_ref(L, LUA_REGISTRYINDEX);
    auto const fn = [L, ref](Glib::RefPtr<Sickle::Editor::Editor> const &editor)
        {
            lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
            Lua::push(L, editor.get());
            Lua::checkerror(L, lua_pcall(L, 1, 1, 0));
            bool const result = lua_toboolean(L, -1);
            return result;
        };

    aw->add_maptool(Sickle::Editor::MapTool{name, opdefs, fn});
    return 0;
}

static int do_nothing(lua_State *L)
{
    return 0;
}

static luaL_Reg methods[] = {
    {"set_grid_size", set_grid_size},
    {"get_grid_size", get_grid_size},
    {"get_maptool", get_maptool},

    {"add_maptool", add_maptool},

    {"on_grid_size_changed", do_nothing},
    {"on_maptool_changed", do_nothing},
    {"on_key_press_event", do_nothing},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// C++ facing
template<>
void Lua::push(lua_State *L, AppWin *appwin)
{
    Lua::RefBuilder builder{L, METATABLE, appwin};
    if (builder.pushnew())
        return;

    builder.addField("mapArea3D", &appwin->_view3d);
    builder.addField("topMapArea", &appwin->_view2d_top);
    builder.addField("frontMapArea", &appwin->_view2d_front);
    builder.addField("rightMapArea", &appwin->_view2d_right);

    builder.addSignalHandler(
        appwin->property_grid_size().signal_changed(), "on_grid_size_changed");
    builder.addSignalHandler(
        appwin->editor->property_maptool().signal_changed(),
        "on_maptool_changed");
    builder.addSignalHandler(
        appwin->signal_key_press_event(), "on_key_press_event");

    builder.finish();
}

AppWin *lappwin_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, METATABLE);
    luaL_argcheck(L, ud != NULL, arg, "`" METATABLE "' expected");
    return *static_cast<AppWin **>(ud);
}

int luaopen_appwin(lua_State *L)
{
    luaL_requiref(L, "maparea2d", luaopen_maparea2d, 1);
    luaL_requiref(L, "maparea3d", luaopen_maparea3d, 1);
    lua_pop(L, 2);

    lua_newtable(L);
    luaL_newmetatable(L, METATABLE);
    luaL_setfuncs(L, methods, 0);
    lua_setfield(L, -2, "metatable");

    Lua::RefBuilder<AppWin>::setup_indexing(L, METATABLE);
    return 1;
}
