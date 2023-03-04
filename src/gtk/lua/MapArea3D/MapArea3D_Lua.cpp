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
#include "LuaGeo.hpp"
#include "Editor_Lua.hpp"


#define LIBRARY_NAME    "Sickle.maparea3d"
#define CLASSNAME       Sickle::MapArea3D


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
        leditorbrush_new(L, brush);
    else
        lua_pushnil(L);
    return 1;
}

static int screenspace_to_glspace(lua_State *L)
{
    auto m3d = lmaparea3d_check(L, 1);
    auto const &xy = lgeo_checkvector(L, 2);
    auto const &p = m3d->screenspace_to_glspace(xy);
    Lua::Pusher{L}(p);
    return 1;
}

static int get_camera(lua_State *L)
{
    auto m3d = lmaparea3d_check(L, 1);
    auto cam = m3d->property_camera().get_value();
    Lua::Pusher{L}(cam);
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
    return leditor_new(L, &m3d->get_editor());
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
    Lua::Pusher{L}(m3d->property_state().get_value());
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
int lmaparea3d_new(lua_State *L, Sickle::MapArea3D *maparea)
{
    // If we've already built an object for this pointer, just reuse it.
    get_from_objectTable(L, maparea);
    if (!lua_isnil(L, -1))
        return 1;
    else
        lua_pop(L, 1);

    // Create the Lua object.
    auto ptr = static_cast<CLASSNAME const **>(
        lua_newuserdatauv(L, sizeof(CLASSNAME *), 1));
    *ptr = maparea;

    // Add methods/data table.
    lua_newtable(L);
    luaL_setfuncs(L, methods, 0);
    lua_setiuservalue(L, -2, 1);

    // Set metatable.
    luaL_setmetatable(L, LIBRARY_NAME);

    // Add the object to the Lua registry, using the pointer as key. This is
    // needed for the C++ callbacks to know what object to call methods on.
    add_to_objectTable(L, maparea);

    // Connect signals.
    maparea->signal_key_press_event().connect(
        [L, maparea](GdkEventKey const *e){
            get_from_objectTable(L, maparea);
            Lua::call_method_r(L, 1, "on_key_press_event", e);
            return lua_toboolean(L, -1);
        });
    maparea->signal_key_release_event().connect(
        [L, maparea](GdkEventKey const *e){
            get_from_objectTable(L, maparea);
            Lua::call_method_r(L, 1, "on_key_release_event", e);
            return lua_toboolean(L, -1);
        });
    maparea->signal_button_press_event().connect(
        [L, maparea](GdkEventButton const *e){
            get_from_objectTable(L, maparea);
            Lua::call_method_r(L, 1, "on_button_press_event", e);
            return lua_toboolean(L, -1);
        });
    maparea->signal_button_release_event().connect(
        [L, maparea](GdkEventButton const *e){
            get_from_objectTable(L, maparea);
            Lua::call_method_r(L, 1, "on_button_release_event", e);
            return lua_toboolean(L, -1);
        });
    maparea->signal_motion_notify_event().connect(
        [L, maparea](GdkEventMotion const *e){
            get_from_objectTable(L, maparea);
            Lua::call_method_r(L, 1, "on_motion_notify_event", e);
            return lua_toboolean(L, -1);
        });
    maparea->signal_scroll_event().connect(
        [L, maparea](GdkEventScroll const *e){
            get_from_objectTable(L, maparea);
            Lua::call_method_r(L, 1, "on_scroll_event", e);
            return lua_toboolean(L, -1);
        });

    return 1;
}

CLASSNAME *lmaparea3d_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, LIBRARY_NAME);
    luaL_argcheck(L, ud != NULL, arg, "`" LIBRARY_NAME "' expected");
    return *static_cast<CLASSNAME **>(ud);
}

int luaopen_maparea3d(lua_State *L)
{
    luaL_requiref(L, "Sickle.maparea3d.state", luaopen_maparea3d_state, 0);
    luaL_requiref(L, "Sickle.freecam", luaopen_freecam, 0);

    // Table used to map C++ pointers to Lua objects.
    // TODO: References should be removed when the C++ objects are destroyed.
    lua_newtable(L);
    Lua::add_to_registry(L, LIBRARY_NAME".objectTable");
    lua_pop(L, 1);

    luaL_newmetatable(L, LIBRARY_NAME);
    luaL_setfuncs(L, metamethods, 0);

    return 0;
}
