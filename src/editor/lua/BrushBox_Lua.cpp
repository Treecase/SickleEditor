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

#include "Editor_Lua.hpp"

#include <se-lua/utils/RefBuilder.hpp>
#include <core/Editor.hpp>
#include <LuaGeo.hpp>

#define METATABLE "Sickle.editor.brushbox"


using namespace Sickle::Editor;


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
void Lua::push(lua_State *L, BrushBox *bb)
{
    Lua::RefBuilder<BrushBox> builder{L, METATABLE, bb};
    if (builder.pushnew())
        return;
    builder.addSignalHandler(bb->signal_updated(), "on_updated");
    builder.finish();
}

BrushBox *lbrushbox_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, METATABLE);
    luaL_argcheck(L, ud != NULL, arg, "`" METATABLE "' expected");
    return *static_cast<BrushBox **>(ud);
}

int luaopen_brushbox(lua_State *L)
{
    lua_newtable(L);
    luaL_newmetatable(L, METATABLE);
    luaL_setfuncs(L, methods, 0);
    lua_setfield(L, -2, "metatable");
    Lua::RefBuilder<BrushBox>::setup_indexing(L, METATABLE);
    return 0;
}
