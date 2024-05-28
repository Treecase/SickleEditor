/**
 * Selection_Lua.cpp - Editor::Selection Lua binding.
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

#include <editor/core/Selection.hpp>
#include <se-lua/utils/RefBuilder.hpp>

#include <memory>

#define METATABLE "Sickle.editor.selection"


using namespace Sickle::Editor;


////////////////////////////////////////////////////////////////////////////////
// Methods
static int selection_clear(lua_State *L)
{
    auto s = lselection_check(L, 1);
    s->clear();
    return 0;
}

static int selection_add(lua_State *L)
{
    auto s = lselection_check(L, 1);
    auto item = static_cast<EditorObjectRef *>(lua_touserdata(L, 2));
    s->add(Selection::Item::cast_dynamic(*item));
    return 0;
}

static int selection_remove(lua_State *L)
{
    auto s = lselection_check(L, 1);
    auto item = static_cast<EditorObjectRef *>(lua_touserdata(L, 2));
    s->remove(Selection::Item::cast_dynamic(*item));
    return 0;
}

static int selection_contains(lua_State *L)
{
    auto s = lselection_check(L, 1);
    auto brush = leditorbrush_check(L, 2);
    lua_pushboolean(L, s->contains(brush));
    return 1;
}

static int selection_iterate_iterator(lua_State *L)
{
    auto const I = luaL_checkinteger(L, lua_upvalueindex(1));

    lua_pushinteger(L, I + 1);
    lua_replace(L, lua_upvalueindex(1));

    int const type = lua_geti(L, lua_upvalueindex(2), I);
    if (type == LUA_TNIL)
        return 0;
    else if (type != LUA_TUSERDATA)
        return luaL_error(L, "selection:iterate error -- bad 2nd upvalue");

    return 1;
}
static int selection_iterate(lua_State *L)
{
    auto const s = lselection_check(L, 1);

    // Upvalue 1: Iterator index
    lua_pushinteger(L, 1);

    // Upvalue 2: List of selected objects
    lua_newtable(L);
    lua_Integer i = 1;
    for (auto brush : s->get_all_of_type<Brush>())
    {
        Lua::push(L, brush);
        lua_seti(L, -2, i++);
    }

    lua_pushcclosure(L, &selection_iterate_iterator, 2);
    return 1;
}

static int selection_is_empty(lua_State *L)
{
    auto s = lselection_check(L, 1);
    lua_pushboolean(L, s->empty());
    return 1;
}

static int do_nothing(lua_State *L)
{
    return 0;
}

static luaL_Reg methods[] = {
    {"clear", selection_clear},
    {"add", selection_add},
    {"remove", selection_remove},
    {"contains", selection_contains},
    {"iterate", selection_iterate},
    {"is_empty", selection_is_empty},
    {"on_updated", do_nothing},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// C++ facing
template<>
void Lua::push(lua_State *L, Selection *selection)
{
    Lua::RefBuilder builder{L, METATABLE, selection};
    if (builder.pushnew())
        return;
    builder.addSignalHandler(selection->signal_updated(), "on_updated");
    builder.finish();
}

Selection *lselection_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, METATABLE);
    luaL_argcheck(L, ud != NULL, arg, "`" METATABLE "' expected");
    return *static_cast<Selection **>(ud);
}

int luaopen_selection(lua_State *L)
{
    lua_newtable(L);
    luaL_requiref(L, "editorbrush", luaopen_editorbrush, 0);
    lua_setfield(L, -2, "editorbrush");
    luaL_newmetatable(L, METATABLE);
    luaL_setfuncs(L, methods, 0);
    lua_setfield(L, -2, "metatable");
    Lua::RefBuilder<Selection>::setup_indexing(L, METATABLE);
    return 1;
}
