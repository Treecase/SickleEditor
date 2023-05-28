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

#include "editor/Editor.hpp"
#include "Editor_Lua.hpp"

#include <se-lua/utils/RefBuilder.hpp>
#include <LuaGeo.hpp>

using namespace Sickle::Editor;


static Lua::RefBuilder<Editor> builder{"Sickle.editor"};


////////////////////////////////////////////////////////////////////////////////
// Methods
static int add_brush(lua_State *L)
{
    auto ed = leditor_check(L, 1);
    luaL_argcheck(L, lua_istable(L, 2), 2, "`table' expected");
    lua_Integer n = 1;
    while (lua_geti(L, 2, n) != LUA_TNIL)
        n++;
    std::vector<glm::vec3> points{};
    for (int i = 0; i < n; ++i)
        points.emplace_back(lgeo_tovector(L, 2 + i));
    lua_pop(L, n);
    ed->do_command(std::make_shared<commands::AddBrush>(points));
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

    {"get_selection", get_selection},
    {"get_brushbox", get_brushbox},

    {"on_map_changed", do_nothing},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// C++ facing
template<>
void Lua::push(lua_State *L, Editor *editor)
{
    if (builder.pushnew(editor))
        return;
    builder.addSignalHandler(editor->signal_map_changed(), "on_map_changed");
    builder.finish();
}

Editor *leditor_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, "Sickle.editor");
    luaL_argcheck(L, ud != NULL, arg, "`Sickle.editor' expected");
    return *static_cast<Editor **>(ud);
}

int luaopen_editor(lua_State *L)
{
    lua_newtable(L);

    luaL_requiref(L, "selection", luaopen_selection, 0);
    luaL_requiref(L, "brushbox", luaopen_brushbox, 0);
    lua_setfield(L, -3, "brushbox");
    lua_setfield(L, -2, "selection");

    luaL_newmetatable(L, "Sickle.editor");
    luaL_setfuncs(L, methods, 0);
    lua_setfield(L, -2, "metatable");

    builder.setLua(L);
    return 1;
}
