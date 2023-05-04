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

#include "editor/Editor.hpp"
#include "Editor_Lua.hpp"
#include "LuaGeo.hpp"

#include <se-lua/utils/RefBuilder.hpp>


static Lua::RefBuilder<Sickle::Editor::Brush> builder{"Sickle.editorbrush"};


////////////////////////////////////////////////////////////////////////////////
// Methods
static int is_selected(lua_State *L)
{
    auto brush = leditorbrush_check(L, 1);
    lua_pushboolean(L, brush->is_selected.get());
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
    auto vec = lgeo_checkvector(L, 2);
    brush->translate(vec);
    return 0;
}

static int get_vertices(lua_State *L)
{
    auto brush = leditorbrush_check(L, 1);
    lua_newtable(L);
    lua_Integer idx = 1;
    for (auto const &face : brush->faces)
    {
        for (auto const &vertex : face->vertices)
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
    {"get_vertices", get_vertices},

    {"on_selected", do_nothing},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// C++ facing
template<>
void Lua::push(lua_State *L, Sickle::Editor::Brush *brush)
{
    if (builder.pushnew(brush))
        return;
    builder.addSignalHandler(
        brush->is_selected.signal_changed(), "on_selected");
    builder.finish();
}

Sickle::Editor::Brush *leditorbrush_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, "Sickle.editorbrush");
    luaL_argcheck(L, ud != NULL, arg, "`Sickle.editorbrush' expected");
    return *static_cast<Sickle::Editor::Brush **>(ud);
}

int luaopen_editorbrush(lua_State *L)
{
    lua_newtable(L);
    luaL_newmetatable(L, "Sickle.editorbrush");
    luaL_setfuncs(L, methods, 0);
    lua_setfield(L, -2, "metatable");

    builder.setLua(L);
    return 1;
}
