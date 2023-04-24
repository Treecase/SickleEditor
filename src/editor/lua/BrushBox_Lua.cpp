/**
 * BrushBox_Lua.cpp - Editor::BrushBox Lua binding.
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


static Lua::RefBuilder<Sickle::Editor::Editor::BrushBox> builder{
    "Sickle.editor.brushbox"};


////////////////////////////////////////////////////////////////////////////////
// Methods
static int get_start(lua_State *L)
{
    auto bb = lbrushbox_check(L, 1);
    Lua::push(L, bb->p1());
    return 1;
}

static int set_start(lua_State *L)
{
    auto bb = lbrushbox_check(L, 1);
    bb->p1(lgeo_checkvector(L, 2));
    return 0;
}

static int get_end(lua_State *L)
{
    auto bb = lbrushbox_check(L, 1);
    Lua::push(L, bb->p2());
    return 1;
}

static int set_end(lua_State *L)
{
    auto bb = lbrushbox_check(L, 1);
    bb->p2(lgeo_checkvector(L, 2));
    return 0;
}

static int do_nothing(lua_State *L)
{
    return 0;
}

static luaL_Reg methods[] = {
    {"get_start", get_start},
    {"set_start", set_start},
    {"get_end", get_end},
    {"set_end", set_end},

    {"on_updated", do_nothing},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// C++ facing
template<>
void Lua::push(lua_State *L, Sickle::Editor::Editor::BrushBox *bb)
{
    if (builder.pushnew(bb))
        return;
    builder.addSignalHandler(bb->signal_updated(), "on_updated");
    builder.finish();
}

Sickle::Editor::Editor::BrushBox *lbrushbox_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, "Sickle.editor.brushbox");
    luaL_argcheck(L, ud != NULL, arg, "`Sickle.editor.brushbox' expected");
    return *static_cast<Sickle::Editor::Editor::BrushBox **>(ud);
}

int luaopen_brushbox(lua_State *L)
{
    lua_newtable(L);
    luaL_newmetatable(L, "Sickle.editor.brushbox");
    luaL_setfuncs(L, methods, 0);
    lua_setfield(L, -2, "metatable");

    builder.setLua(L);
    return 0;
}
