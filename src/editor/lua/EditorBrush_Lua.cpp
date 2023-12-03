/**
 * EditorBrush_Lua.cpp - Editor  Brush Lua binding.
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

#include <se-lua/utils/RefBuilder.hpp>
#include <core/Editor.hpp>
#include <LuaGeo.hpp>

#define METATABLE "Sickle.editorbrush"

using namespace Sickle::Editor;


////////////////////////////////////////////////////////////////////////////////
// Methods
static int is_selected(lua_State *L)
{
    auto brush = leditorbrush_check(L, 1);
    lua_pushboolean(L, brush->is_selected());
    return 1;
}


static int transform(lua_State *L)
{
    auto brush = leditorbrush_check(L, 1);
    auto mat = lgeo_checkmatrix(L, 2);
    brush->transform(mat);
    return 0;
}


static int translate(lua_State *L)
{
    auto brush = leditorbrush_check(L, 1);
    auto const vec = lgeo_checkvector<glm::vec3>(L, 2);
    brush->translate(vec);
    return 0;
}


/**
 * Brush:rotate(angle: number, axis: geo.vec3)
 *
 * ANGLE is in degrees.
 */
static int rotate(lua_State *L)
{
    auto brush = leditorbrush_check(L, 1);
    auto const angle = static_cast<float>(luaL_checknumber(L, 2));
    auto const vec = lgeo_checkvector<glm::vec3>(L, 3);
    brush->transform(glm::rotate(glm::mat4{1.0}, glm::degrees(angle), vec));
    return 0;
}


static int scale(lua_State *L)
{
    auto brush = leditorbrush_check(L, 1);
    auto const vec = lgeo_checkvector<glm::vec3>(L, 2);
    brush->transform(glm::scale(glm::mat4{1.0}, vec));
    return 0;
}


static int get_faces(lua_State *L)
{
    auto const brush = leditorbrush_check(L, 1);
    lua_newtable(L);
    lua_Integer idx = 1;
    for (auto const &face : brush->faces())
    {
        Lua::push(L, face);
        lua_seti(L, -2, idx++);
    }
    return 1;
}


static int get_vertices(lua_State *L)
{
    auto brush = leditorbrush_check(L, 1);
    lua_newtable(L);
    lua_Integer idx = 1;
    for (auto const &face : brush->faces())
    {
        for (auto const &vertex : face->get_vertices())
        {
            Lua::push(L, vertex);
            lua_seti(L, 2, idx++);
        }
    }
    return 1;
}


static int do_nothing(lua_State *L)
{
    return 0;
}


static luaL_Reg methods[] = {
    {"is_selected", is_selected},
    {"transform", transform},
    {"translate", translate},
    {"rotate", rotate},
    {"scale", scale},
    {"get_faces", get_faces},
    {"get_vertices", get_vertices},

    {"on_selected", do_nothing},
    {NULL, NULL}
};



////////////////////////////////////////////////////////////////////////////////
// C++ facing
template<>
void Lua::push(lua_State *L, BrushRef brush)
{
    Lua::RefBuilder builder{L, METATABLE, brush};
    int const t1 = lua_gettop(L);
    if (builder.pushnew())
    {
        int const t2 = lua_gettop(L);
        assert(t2 == t1 + 1);
        return;
    }
    builder.addSignalHandler(
        brush->property_selected().signal_changed(), "on_selected");
    builder.finish();
    int const t3 = lua_gettop(L);
    assert(t3 == t1 + 1);
}


BrushRef leditorbrush_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, METATABLE);
    luaL_argcheck(L, ud != NULL, arg, "`" METATABLE "' expected");
    return *static_cast<BrushRef *>(ud);
}


int luaopen_editorbrush(lua_State *L)
{
    lua_newtable(L);
    luaL_newmetatable(L, METATABLE);
    luaL_setfuncs(L, methods, 0);
    lua_setfield(L, -2, "metatable");
    Lua::RefBuilder<Brush>::setup_indexing(L, METATABLE);
    return 1;
}
