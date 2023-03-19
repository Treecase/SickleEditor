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
#include "Editor_Lua.hpp"

#include <LuaGeo.hpp>
#include <se-lua/lua-utils.hpp>


static Lua::ReferenceManager refman{};


////////////////////////////////////////////////////////////////////////////////
// Metamethods
static int dunder_newindex(lua_State *L)
{
    lmaparea3d_check(L, 1);
    lua_getiuservalue(L, -3, 1);
    lua_rotate(L, -3, 1);
    lua_settable(L, -3);
    return 0;
}

static int dunder_index(lua_State *L)
{
    lmaparea3d_check(L, 1);
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
static int pick_brush(lua_State *L)
{
    auto m3d = lmaparea3d_check(L, 1);
    auto xy = lgeo_checkvector(L, 2);
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
    auto const &xy = lgeo_checkvector(L, 2);
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
    Lua::push(L, &m3d->get_editor());
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
void Lua::push(lua_State *L, Sickle::MapArea3D *maparea)
{
    refman.get(maparea);
    if (!lua_isnil(L, -1))
        return;
    else
        lua_pop(L, 1);

    auto ptr = static_cast<Sickle::MapArea3D const **>(
        lua_newuserdatauv(L, sizeof(Sickle::MapArea3D *), 1));
    *ptr = maparea;
    luaL_setmetatable(L, "Sickle.maparea3d");

    // Add methods/data table.
    // TODO: individual data, shared methods
    lua_newtable(L);
    luaL_setfuncs(L, methods, 0);
    lua_setiuservalue(L, -2, 1);

    // Connect signals.
    maparea->signal_key_press_event().connect(
        [L, maparea](GdkEventKey const *e){
            refman.get(maparea);
            Lua::call_method_r(L, 1, "on_key_press_event", e);
            return lua_toboolean(L, -1);
        });
    maparea->signal_key_release_event().connect(
        [L, maparea](GdkEventKey const *e){
            refman.get(maparea);
            Lua::call_method_r(L, 1, "on_key_release_event", e);
            return lua_toboolean(L, -1);
        });
    maparea->signal_button_press_event().connect(
        [L, maparea](GdkEventButton const *e){
            refman.get(maparea);
            Lua::call_method_r(L, 1, "on_button_press_event", e);
            return lua_toboolean(L, -1);
        });
    maparea->signal_button_release_event().connect(
        [L, maparea](GdkEventButton const *e){
            refman.get(maparea);
            Lua::call_method_r(L, 1, "on_button_release_event", e);
            return lua_toboolean(L, -1);
        });
    maparea->signal_motion_notify_event().connect(
        [L, maparea](GdkEventMotion const *e){
            refman.get(maparea);
            Lua::call_method_r(L, 1, "on_motion_notify_event", e);
            return lua_toboolean(L, -1);
        });
    maparea->signal_scroll_event().connect(
        [L, maparea](GdkEventScroll const *e){
            refman.get(maparea);
            Lua::call_method_r(L, 1, "on_scroll_event", e);
            return lua_toboolean(L, -1);
        });

    refman.set(maparea, -1);
}

Sickle::MapArea3D *lmaparea3d_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, "Sickle.maparea3d");
    luaL_argcheck(L, ud != NULL, arg, "`Sickle.maparea3d' expected");
    return *static_cast<Sickle::MapArea3D **>(ud);
}

int luaopen_maparea3d(lua_State *L)
{
    luaL_requiref(L, "Sickle.maparea3d.state", luaopen_maparea3d_state, 0);
    luaL_requiref(L, "Sickle.freecam", luaopen_freecam, 0);

    refman.init(L);

    luaL_newmetatable(L, "Sickle.maparea3d");
    luaL_setfuncs(L, metamethods, 0);

    return 0;
}
