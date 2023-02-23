/**
 * MapArea2D_Lua.cpp - MapArea2D Lua binding.
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
#include "Editor_Lua.hpp"
#include "LuaGdkEvent.hpp"
#include "MapArea2D_Lua.hpp"


#define LIBRARY_NAME    "Sickle.maparea2d"
#define CLASSNAME       Sickle::MapArea2D


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
    lmaparea2d_check(L, 1);
    lua_getiuservalue(L, -3, 1);
    lua_rotate(L, -3, 1);
    lua_settable(L, -3);
    return 0;
}

static int dunder_index(lua_State *L)
{
    lmaparea2d_check(L, 1);
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
static int screenspace_to_drawspace(lua_State *L)
{
    auto ma = lmaparea2d_check(L, 1);
    auto x = luaL_checknumber(L, 2);
    auto y = luaL_checknumber(L, 3);
    auto xy = ma->screenspace_to_drawspace(x, y);
    lua_pushnumber(L, xy.x);
    lua_pushnumber(L, xy.y);
    return 2;
}

static int drawspace_to_worldspace(lua_State *L)
{
    auto ma = lmaparea2d_check(L, 1);
    auto x = luaL_checknumber(L, 2);
    auto y = luaL_checknumber(L, 3);
    auto xyz = ma->drawspace_to_worldspace({x, y});
    lua_pushnumber(L, xyz.x);
    lua_pushnumber(L, xyz.y);
    lua_pushnumber(L, xyz.z);
    return 3;
}

static int worldspace_to_drawspace(lua_State *L)
{
    auto ma = lmaparea2d_check(L, 1);
    auto x = luaL_checknumber(L, 2);
    auto y = luaL_checknumber(L, 3);
    auto z = luaL_checknumber(L, 4);
    auto xy = ma->worldspace_to_drawspace({x, y, z});
    lua_pushnumber(L, xy.x);
    lua_pushnumber(L, xy.y);
    return 2;
}

static int pick_brush(lua_State *L)
{
    auto ma = lmaparea2d_check(L, 1);
    auto x = luaL_checknumber(L, 2);
    auto y = luaL_checknumber(L, 3);
    auto picked = ma->pick_brush({x, y});
    if (picked)
        leditorbrush_new(L, picked);
    else
        lua_pushnil(L);
    return 1;
}

static int set_cursor(lua_State *L)
{
    auto ma = lmaparea2d_check(L, 1);
    auto cursor = luaL_checkstring(L, 2);
    ma->get_window()->set_cursor(
        Gdk::Cursor::create(ma->get_display(), cursor));
    return 0;
}

static int set_draw_angle(lua_State *L)
{
    auto ma = lmaparea2d_check(L, 1);
    ma->set_draw_angle((Sickle::MapArea2D::DrawAngle)luaL_checkinteger(L, 2));
    return 0;
}

static int get_draw_angle(lua_State *L)
{
    auto ma = lmaparea2d_check(L, 1);
    lua_pushinteger(L, ma->get_draw_angle());
    return 1;
}

static int get_editor(lua_State *L)
{
    auto ma = lmaparea2d_check(L, 1);
    auto &ed = ma->get_editor();
    return leditor_new(L, &ed);
}

static int get_transform(lua_State *L)
{
    auto ma = lmaparea2d_check(L, 1);
    return ltransform2d_new(L, ma->property_transform().get_value());
}

static int set_transform(lua_State *L)
{
    auto ma = lmaparea2d_check(L, 1);
    ma->property_transform().set_value(*ltransform2d_check(L, 2));
    return 0;
}

static int get_selection_box(lua_State *L)
{
    auto ma = lmaparea2d_check(L, 1);
    return lgrabbablebox_new(L, &ma->get_box());
}

static int get_state(lua_State *L)
{
    auto ma = lmaparea2d_check(L, 1);
    return lstate_new(L, ma->property_state().get_value());
}

static int set_state(lua_State *L)
{
    auto ma = lmaparea2d_check(L, 1);
    ma->property_state().set_value(*lstate_check(L, 2));
    return 0;
}

static int do_nothing(lua_State *L)
{
    return 0;
}

static luaL_Reg methods[] = {
    {"screenspace_to_drawspace", screenspace_to_drawspace},
    {"drawspace_to_worldspace", drawspace_to_worldspace},
    {"worldspace_to_drawspace", worldspace_to_drawspace},
    {"pick_brush", pick_brush},

    {"set_cursor", set_cursor},
    {"set_draw_angle", set_draw_angle},
    {"get_draw_angle", get_draw_angle},
    {"get_editor", get_editor},
    {"get_transform", get_transform},
    {"set_transform", set_transform},
    {"get_selection_box", get_selection_box},
    {"get_state", get_state},
    {"set_state", set_state},

    {"on_key_press_event", do_nothing},
    {"on_key_release_event", do_nothing},
    {"on_button_press_event", do_nothing},
    {"on_button_release_event", do_nothing},
    {"on_motion_notify_event", do_nothing},
    {"on_scroll_event", do_nothing},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// Functions
static luaL_Reg functions[] = {
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// C++ facing
int lmaparea2d_new(lua_State *L, CLASSNAME *maparea)
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

CLASSNAME *lmaparea2d_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, LIBRARY_NAME);
    luaL_argcheck(L, ud != NULL, arg, "`" LIBRARY_NAME "' expected");
    return *static_cast<CLASSNAME **>(ud);
}

int luaopen_maparea2d(lua_State *L)
{
    luaL_requiref(L, "editor", luaopen_editor, 1);
    luaL_requiref(L, "grabbablebox", luaopen_grabbablebox, 1);
    luaL_requiref(L, "state", luaopen_state, 1);
    luaL_requiref(L, "transform2d", luaopen_transform2d, 1);

    // Table used to map C++ pointers to Lua objects.
    // TODO: References should be removed when the C++ objects are destroyed.
    lua_newtable(L);
    Lua::add_to_registry(L, LIBRARY_NAME".objectTable");
    lua_pop(L, 1);

    luaL_newmetatable(L, LIBRARY_NAME);
    luaL_setfuncs(L, metamethods, 0);

    // Export DrawAngle enum values.
    luaL_newlib(L, functions);
    lua_pushliteral(L, "TOP");
    lua_pushinteger(L, Sickle::MapArea2D::DrawAngle::TOP);
    lua_settable(L, -3);
    lua_pushliteral(L, "FRONT");
    lua_pushinteger(L, Sickle::MapArea2D::DrawAngle::FRONT);
    lua_settable(L, -3);
    lua_pushliteral(L, "RIGHT");
    lua_pushinteger(L, Sickle::MapArea2D::DrawAngle::RIGHT);
    lua_settable(L, -3);

    return 1;
}
