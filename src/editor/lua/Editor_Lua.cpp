/**
 * Editor_Lua.cpp - Editor Lua binding.
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

#include <core/Editor.hpp>
#include <operations/OperationLoader.hpp>
#include <se-lua/utils/RefBuilder.hpp>
#include <LuaGeo.hpp>

#define METATABLE "Sickle.editor"

using namespace Sickle::Editor;


////////////////////////////////////////////////////////////////////////////////
// Methods
static int add_brush(lua_State *L)
{
    auto ed = leditor_check(L, 1);

    auto const n = lua_gettop(L);

    std::vector<glm::vec3> points{};
    for (int i = 0; i < n - 1; ++i)
    {
        auto const v = lgeo_tovector<glm::vec3>(L, i + 2);
        points.push_back(v);
    }

    try {
        ed->get_map()->worldspawn().add_brush(Brush::create(points));
    }
    catch (std::runtime_error const &e) {
        // Brush construction degenerate case.
    }
    return 0;
}

static int remove_brush(lua_State *L)
{
    auto ed = leditor_check(L, 1);
    auto brush = leditorbrush_check(L, 2);
    ed->get_map()->remove_brush(brush);
    return 0;
}

static int do_operation(lua_State *L)
{
    auto ed = leditor_check(L, 1);
    auto const id = luaL_checkstring(L, 2);

    auto const op = ed->oploader->get_operation(id);

    Operation::ArgList args{};
    for (size_t i = 0; i < op.args.size(); ++i)
        args.push_back(op.make_arg_from_lua(i, L, 3 + i));

    try {
        op.execute(ed, args);
    }
    catch (Lua::Error const &e) {
        return luaL_error(L, "%s", e.what());
    }
    return 0;
}

static int get_selection(lua_State *L)
{
    auto ed = leditor_check(L, 1);
    Lua::push(L, &ed->selected);
    return 1;
}

static int get_brushbox(lua_State *L)
{
    auto ed = leditor_check(L, 1);
    Lua::push(L, &ed->brushbox);
    return 1;
}

static int do_nothing(lua_State *L)
{
    return 0;
}

static luaL_Reg methods[] = {
    {"add_brush", add_brush},
    {"remove_brush", remove_brush},
    {"do_operation", do_operation},

    {"get_selection", get_selection},
    {"get_brushbox", get_brushbox},

    {"on_map_changed", do_nothing},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// C++ facing
template<>
void Lua::push(lua_State *L, EditorRef editor)
{
    Lua::RefBuilder builder{L, METATABLE, editor};
    if (builder.pushnew())
        return;
    builder.addSignalHandler(
        editor->property_map().signal_changed(),
        "on_map_changed");
    builder.finish();
}

EditorRef leditor_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, METATABLE);
    luaL_argcheck(L, ud != NULL, arg, "`" METATABLE "' expected");
    return *static_cast<EditorRef *>(ud);
}

int luaopen_editor(lua_State *L)
{
    lua_newtable(L);

    luaL_requiref(L, "selection", luaopen_selection, 0);
    luaL_requiref(L, "brushbox", luaopen_brushbox, 0);
    luaL_requiref(L, "face", luaopen_face, 0);
    lua_setfield(L, -4, "face");
    lua_setfield(L, -3, "brushbox");
    lua_setfield(L, -2, "selection");

    luaL_newmetatable(L, METATABLE);
    luaL_setfuncs(L, methods, 0);
    lua_setfield(L, -2, "metatable");

    Lua::RefBuilder<Editor>::setup_indexing(L, METATABLE);
    return 1;
}
