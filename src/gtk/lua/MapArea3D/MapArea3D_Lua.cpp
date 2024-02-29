/**
 * MapArea3D_Lua.cpp - MapArea3D Lua binding.
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

#include "MapArea3D_Lua.hpp"
#include "LuaGdkEvent.hpp"

#include <editor/lua/Editor_Lua.hpp>
#include <se-lua/lua-geo/LuaGeo.hpp>
#include <se-lua/utils/RefBuilder.hpp>

#define METATABLE "Sickle.gtk.maparea3d"


using namespace Sickle;


////////////////////////////////////////////////////////////////////////////////
// Methods
static int pick_brush(lua_State *L)
{
    auto m3d = lmaparea3d_check(L, 1);
    auto const xy = lgeo_checkvector<glm::vec2>(L, 2);
    auto brush = m3d->pick_brush(xy);
    if (brush)
        Lua::push(L, brush);
    else
        lua_pushnil(L);
    return 1;
}

static int screenspace_to_glspace(lua_State *L)
{
    auto m3d = lmaparea3d_check(L, 1);
    auto const xy = lgeo_checkvector<glm::vec2>(L, 2);
    auto const &p = m3d->screenspace_to_glspace(xy);
    Lua::push(L, p);
    return 1;
}

static int get_camera(lua_State *L)
{
    auto m3d = lmaparea3d_check(L, 1);
    auto cam = m3d->property_camera().get_value();
    Lua::push(L, cam);
    return 1;
}

static int set_camera(lua_State *L)
{
    auto m3d = lmaparea3d_check(L, 1);
    auto cam = lfreecam_check(L, 2);
    m3d->property_camera().set_value(*cam);
    return 0;
}

static int get_editor(lua_State *L)
{
    auto m3d = lmaparea3d_check(L, 1);
    Lua::push(L, m3d->get_editor());
    return 1;
}

static int get_mouse_sensitivity(lua_State *L)
{
    auto m3d = lmaparea3d_check(L, 1);
    lua_pushnumber(L, m3d->property_mouse_sensitivity().get_value());
    return 1;
}

static int set_mouse_sensitivity(lua_State *L)
{
    auto m3d = lmaparea3d_check(L, 1);
    m3d->property_mouse_sensitivity().set_value(luaL_checknumber(L, 2));
    return 0;
}

static int get_shift_multiplier(lua_State *L)
{
    auto m3d = lmaparea3d_check(L, 1);
    lua_pushnumber(L, m3d->property_shift_multiplier().get_value());
    return 1;
}

static int set_shift_multiplier(lua_State *L)
{
    auto m3d = lmaparea3d_check(L, 1);
    m3d->property_shift_multiplier().set_value(luaL_checknumber(L, 2));
    return 0;
}

static int get_state(lua_State *L)
{
    auto m3d = lmaparea3d_check(L, 1);
    Lua::push(L, m3d->property_state().get_value());
    return 1;
}

static int set_state(lua_State *L)
{
    auto m3d = lmaparea3d_check(L, 1);
    auto s = lmaparea3d_state_check(L, 2);
    m3d->property_state().set_value(*s);
    return 0;
}

static int get_wireframe(lua_State *L)
{
    auto m3d = lmaparea3d_check(L, 1);
    lua_pushboolean(L, m3d->property_wireframe().get_value());
    return 1;
}

static int set_wireframe(lua_State *L)
{
    auto m3d = lmaparea3d_check(L, 1);
    auto b = lua_toboolean(L, 2);
    m3d->property_wireframe().set_value(b);
    return 0;
}

static int do_nothing(lua_State *L)
{
    return 0;
}

static luaL_Reg methods[] = {
    {"pick_brush", pick_brush},
    {"screenspace_to_glspace", screenspace_to_glspace},

    {"get_camera", get_camera},
    {"get_editor", get_editor},
    {"get_mouse_sensitivity", get_mouse_sensitivity},
    {"get_shift_multiplier", get_shift_multiplier},
    {"get_state", get_state},
    // {"get_transform", get_transform},
    {"get_wireframe", get_wireframe},

    {"set_camera", set_camera},
    {"set_mouse_sensitivity", set_mouse_sensitivity},
    {"set_shift_multiplier", set_shift_multiplier},
    {"set_state", set_state},
    // {"set_transform", set_transform},
    {"set_wireframe", set_wireframe},

    {"on_key_press_event", do_nothing},
    {"on_key_release_event", do_nothing},
    {"on_button_press_event", do_nothing},
    {"on_button_release_event", do_nothing},
    {"on_motion_notify_event", do_nothing},
    {"on_scroll_event", do_nothing},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// C++ facing
template<>
void Lua::push(lua_State *L, MapArea3D *maparea)
{
    Lua::RefBuilder builder{L, METATABLE, maparea};
    if (builder.pushnew())
        return;

    builder.addSignalHandler(
        maparea->signal_key_press_event(), "on_key_press_event");
    builder.addSignalHandler(
        maparea->signal_key_release_event(), "on_key_release_event");
    builder.addSignalHandler(
        maparea->signal_button_press_event(), "on_button_press_event");
    builder.addSignalHandler(
        maparea->signal_button_release_event(), "on_button_release_event");
    builder.addSignalHandler(
        maparea->signal_motion_notify_event(), "on_motion_notify_event");
    builder.addSignalHandler(maparea->signal_scroll_event(), "on_scroll_event");

    builder.finish();
}

MapArea3D *lmaparea3d_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, METATABLE);
    luaL_argcheck(L, ud != NULL, arg, "`" METATABLE "' expected");
    return *static_cast<MapArea3D **>(ud);
}

int luaopen_maparea3d(lua_State *L)
{
    luaL_requiref(L, "Sickle.gtk.maparea3d.state", luaopen_maparea3d_state, 0);
    luaL_requiref(L, "Sickle.gtk.freecam", luaopen_freecam, 0);
    lua_pop(L, 2);

    lua_newtable(L);
    luaL_newmetatable(L, METATABLE);
    luaL_setfuncs(L, methods, 0);
    lua_setfield(L, -2, "metatable");

    Lua::RefBuilder<MapArea3D>::setup_indexing(L, METATABLE);
    return 1;
}
