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

#include <se-lua/lua-utils.hpp>
#include <LuaGeo.hpp>


static Lua::ReferenceManager refman{};


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
void Lua::push(lua_State *L, Sickle::Editor::BrushBox *bb)
{
    refman.get(bb);
    if (!lua_isnil(L, -1))
        return;
    else
        lua_pop(L, 1);

    auto ptr = static_cast<Sickle::Editor::BrushBox const **>(
        lua_newuserdatauv(L, sizeof(Sickle::Editor::BrushBox *), 0));
    *ptr = bb;
    luaL_setmetatable(L, "Sickle.editor.brushbox");

    bb->signal_updated().connect(
        [L, bb](){
            refman.get(bb);
            Lua::call_method(L, "on_updated");
        });

    refman.set(bb, -1);
}

Sickle::Editor::BrushBox *lbrushbox_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, "Sickle.editor.brushbox");
    luaL_argcheck(L, ud != NULL, arg, "`Sickle.editor.brushbox' expected");
    return *static_cast<Sickle::Editor::BrushBox **>(ud);
}

int luaopen_brushbox(lua_State *L)
{
    // TODO: References should be removed when the C++ objects are destroyed.
    refman.init(L);

    luaL_newmetatable(L, "Sickle.editor.brushbox");
    lua_newtable(L);
    luaL_setfuncs(L, methods, 0);
    lua_setfield(L, -2, "__index");

    return 0;
}
