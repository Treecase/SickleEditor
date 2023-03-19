/**
 * EditorBrush_Lua.cpp - EditorBrush Lua binding.
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

#include <se-lua/lua-utils.hpp>


static Lua::ReferenceManager refman{};


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
    for (auto const &plane : brush->planes)
    {
        for (auto const &vertex : plane.vertices)
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
void Lua::push(lua_State *L, Sickle::EditorBrush *brush)
{
    refman.get(brush);
    if (!lua_isnil(L, -1))
        return;
    else
        lua_pop(L, 1);

    auto ptr = static_cast<Sickle::EditorBrush const **>(
        lua_newuserdatauv(L, sizeof(Sickle::EditorBrush *), 0));
    *ptr = brush;
    luaL_setmetatable(L, "Sickle.editorbrush");

    brush->is_selected.signal_changed().connect(
        [L, brush](){
            refman.get(brush);
            Lua::call_method(L, "on_selected");
        });

    refman.set(brush, -1);
}

Sickle::EditorBrush *leditorbrush_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, "Sickle.editorbrush");
    luaL_argcheck(L, ud != NULL, arg, "`Sickle.editorbrush' expected");
    return *static_cast<Sickle::EditorBrush **>(ud);
}

int luaopen_editorbrush(lua_State *L)
{
    // TODO: References should be removed when the C++ objects are destroyed.
    refman.init(L);

    luaL_newmetatable(L, "Sickle.editorbrush");
    lua_newtable(L);
    luaL_setfuncs(L, methods, 0);
    lua_setfield(L, -2, "__index");

    return 0;
}
