/**
 * Face_Lua.cpp - Editor world Face Lua binding.
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

#include "Editor_Lua.hpp"

#include <editor/world/Face.hpp>
#include <se-lua/lua-geo/LuaGeo.hpp>
#include <se-lua/utils/RefBuilder.hpp>

#define METATABLE "Sickle.face"

using namespace Sickle::Editor;

////////////////////////////////////////////////////////////////////////////////
// Methods
static int is_selected(lua_State *L)
{
    auto face = lface_check(L, 1);
    lua_pushboolean(L, face->is_selected());
    return 1;
}

static int get_texture(lua_State *L)
{
    auto const f = lface_check(L, 1);
    Lua::push(L, f->get_texture());
    return 1;
}

static int get_u(lua_State *L)
{
    auto const f = lface_check(L, 1);
    Lua::push(L, f->get_u());
    return 1;
}

static int get_v(lua_State *L)
{
    auto const f = lface_check(L, 1);
    Lua::push(L, f->get_v());
    return 1;
}

static int get_shift(lua_State *L)
{
    auto const f = lface_check(L, 1);
    Lua::push(L, f->get_shift());
    return 1;
}

static int get_scale(lua_State *L)
{
    auto const f = lface_check(L, 1);
    Lua::push(L, f->get_scale());
    return 1;
}

static int get_rotation(lua_State *L)
{
    auto const f = lface_check(L, 1);
    Lua::push(L, static_cast<lua_Number>(f->get_rotation()));
    return 1;
}

static int get_vertices(lua_State *L)
{
    auto const f = lface_check(L, 1);
    auto const vertices = f->get_vertices();
    lua_newtable(L);
    lua_Integer i = 1;
    for (auto const &vertex : vertices)
    {
        Lua::push(L, vertex);
        lua_seti(L, -2, i++);
    }
    return 1;
}

static int set_texture(lua_State *L)
{
    auto const f = lface_check(L, 1);
    auto const t = luaL_checkstring(L, 2);
    f->set_texture(t);
    return 0;
}

static int set_u(lua_State *L)
{
    auto const f = lface_check(L, 1);
    auto const u = lgeo_checkvector<glm::vec3>(L, 2);
    f->set_u(u);
    return 0;
}

static int set_v(lua_State *L)
{
    auto const f = lface_check(L, 1);
    auto const v = lgeo_checkvector<glm::vec3>(L, 2);
    f->set_v(v);
    return 0;
}

static int set_shift(lua_State *L)
{
    auto const f = lface_check(L, 1);
    auto const shift = lgeo_checkvector<glm::vec2>(L, 2);
    f->set_shift(shift);
    return 0;
}

static int set_scale(lua_State *L)
{
    auto const f = lface_check(L, 1);
    auto const scale = lgeo_checkvector<glm::vec2>(L, 2);
    f->set_scale(scale);
    return 0;
}

static int set_rotation(lua_State *L)
{
    auto const f = lface_check(L, 1);
    auto const rotation = luaL_checknumber(L, 2);
    f->set_rotation(rotation);
    return 0;
}

static int do_nothing(lua_State *L)
{
    return 0;
}

static luaL_Reg methods[] = {
    {        "is_selected",  is_selected},
    {        "get_texture",  get_texture},
    {              "get_u",        get_u},
    {              "get_v",        get_v},
    {          "get_shift",    get_shift},
    {          "get_scale",    get_scale},
    {       "get_rotation", get_rotation},
    {       "get_vertices", get_vertices},
    {        "set_texture",  set_texture},
    {              "set_u",        set_u},
    {              "set_v",        set_v},
    {          "set_shift",    set_shift},
    {          "set_scale",    set_scale},
    {       "set_rotation", set_rotation},

    {"on_vertices_changed",   do_nothing},
    {                 NULL,         NULL}
};

////////////////////////////////////////////////////////////////////////////////
// C++ facing
template<>
void Lua::push(lua_State *L, FaceRef face)
{
    Lua::RefBuilder builder{L, METATABLE, face};
    int const t1 = lua_gettop(L);
    if (builder.pushnew())
    {
        int const t2 = lua_gettop(L);
        assert(t2 == t1 + 1);
        return;
    }
    builder.addSignalHandler(
        face->signal_vertices_changed(),
        "on_vertices_changed");
    builder.finish();
    int const t3 = lua_gettop(L);
    assert(t3 == t1 + 1);
}

FaceRef lface_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, METATABLE);
    luaL_argcheck(L, ud != NULL, arg, "`" METATABLE "' expected");
    return *static_cast<FaceRef *>(ud);
}

int luaopen_face(lua_State *L)
{
    lua_newtable(L);
    luaL_newmetatable(L, METATABLE);
    luaL_setfuncs(L, methods, 0);
    lua_setfield(L, -2, "metatable");
    Lua::RefBuilder<FaceRef>::setup_indexing(L, METATABLE);
    return 1;
}
