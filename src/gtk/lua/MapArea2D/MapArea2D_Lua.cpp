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

#include "../../classes/MapArea2D.hpp"
#include "Editor_Lua.hpp"
#include "LuaGdkEvent.hpp"
#include "MapArea2D_Lua.hpp"

#include <se-lua/utils/RefBuilder.hpp>
#include <LuaGeo.hpp>


static Lua::RefBuilder<Sickle::MapArea2D> builder{"Sickle.maparea2d"};


////////////////////////////////////////////////////////////////////////////////
// Methods
static int screenspace_to_drawspace(lua_State *L)
{
    auto ma = lmaparea2d_check(L, 1);
    auto ss = lgeo_tovector(L, 2);
    auto ds = ma->screenspace_to_drawspace(ss.x, ss.y);
    Lua::push(L, ds);
    return 1;
}

static int drawspace_to_worldspace(lua_State *L)
{
    auto ma = lmaparea2d_check(L, 1);
    auto ds = lgeo_tovector(L, 2);
    auto ws = ma->drawspace_to_worldspace(ds);
    Lua::push(L, ws);
    return 1;
}

static int drawspace3_to_worldspace(lua_State *L)
{
    auto ma = lmaparea2d_check(L, 1);
    auto ds = lgeo_tovector(L, 2);
    auto ws = ma->drawspace3_to_worldspace(ds);
    Lua::push(L, ws);
    return 1;
}

static int worldspace_to_drawspace(lua_State *L)
{
    auto ma = lmaparea2d_check(L, 1);
    auto ws = lgeo_tovector(L, 2);
    auto ds = ma->worldspace_to_drawspace(ws);
    Lua::push(L, ds);
    return 1;
}

static int worldspace_to_drawspace3(lua_State *L)
{
    auto ma = lmaparea2d_check(L, 1);
    auto ws = lgeo_tovector(L, 2);
    auto ds = ma->worldspace_to_drawspace3(ws);
    Lua::push(L, ds);
    return 1;
}

static int pick_brush(lua_State *L)
{
    auto ma = lmaparea2d_check(L, 1);
    auto xy = lgeo_tovector(L, 2);
    auto picked = ma->pick_brush(xy);
    if (picked)
        Lua::push(L, picked.get());
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
    Lua::push(L, &ma->get_editor());
    return 1;
}

static int get_transform(lua_State *L)
{
    auto ma = lmaparea2d_check(L, 1);
    Lua::push(L, ma->property_transform().get_value());
    return 1;
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
    Lua::push(L, &ma->get_selected_box());
    return 1;
}

static int get_brushbox(lua_State *L)
{
    auto ma = lmaparea2d_check(L, 1);
    Lua::push(L, &ma->get_brushbox());
    return 1;
}

static int do_nothing(lua_State *L)
{
    return 0;
}

static luaL_Reg methods[] = {
    {"screenspace_to_drawspace", screenspace_to_drawspace},
    {"drawspace_to_worldspace", drawspace_to_worldspace},
    {"drawspace3_to_worldspace", drawspace3_to_worldspace},
    {"worldspace_to_drawspace", worldspace_to_drawspace},
    {"worldspace_to_drawspace3", worldspace_to_drawspace3},
    {"pick_brush", pick_brush},

    {"set_cursor", set_cursor},
    {"set_draw_angle", set_draw_angle},
    {"get_draw_angle", get_draw_angle},
    {"get_editor", get_editor},
    {"get_transform", get_transform},
    {"set_transform", set_transform},
    {"get_selection_box", get_selection_box},
    {"get_brushbox", get_brushbox},

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
void Lua::push(lua_State *L, Sickle::MapArea2D *maparea)
{
    if (builder.pushnew(maparea))
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

Sickle::MapArea2D *lmaparea2d_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, "Sickle.maparea2d");
    luaL_argcheck(L, ud != NULL, arg, "`Sickle.maparea2d' expected");
    return *static_cast<Sickle::MapArea2D **>(ud);
}

int luaopen_maparea2d(lua_State *L)
{
    luaL_requiref(L, "editor", luaopen_editor, 1);
    lua_pop(L, 1);

    lua_newtable(L);

    luaL_requiref(L, "grabbablebox", luaopen_grabbablebox, 0);
    luaL_requiref(L, "transform2d", luaopen_transform2d, 0);
    lua_setfield(L, -3, "transfrom2d");
    lua_setfield(L, -2, "grabbablebox");

    luaL_newmetatable(L, "Sickle.maparea2d");
    luaL_setfuncs(L, methods, 0);
    lua_setfield(L, -2, "metatable");

    lua_pushinteger(L, Sickle::MapArea2D::DrawAngle::TOP);
    lua_pushinteger(L, Sickle::MapArea2D::DrawAngle::FRONT);
    lua_pushinteger(L, Sickle::MapArea2D::DrawAngle::RIGHT);
    lua_setfield(L, -4, "RIGHT");
    lua_setfield(L, -3, "FRONT");
    lua_setfield(L, -2, "TOP");

    builder.setLua(L);
    return 1;
}
