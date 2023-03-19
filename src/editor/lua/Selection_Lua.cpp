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

#include "editor/Editor.hpp"
#include "Editor_Lua.hpp"

#include <se-lua/lua-utils.hpp>


static Lua::ReferenceManager refman{};


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
    auto i = leditorbrush_check(L, 2);
    s->add(i);
    return 0;
}

static int selection_remove(lua_State *L)
{
    auto s = lselection_check(L, 1);
    auto i = leditorbrush_check(L, 2);
    s->remove(i);
    return 0;
}

static int selection_contains(lua_State *L)
{
    auto s = lselection_check(L, 1);
    auto i = leditorbrush_check(L, 2);
    lua_pushboolean(L, s->contains(i));
    return 1;
}

static int selection_iterate_iterator(lua_State *L)
{
    auto s = lselection_check(L, lua_upvalueindex(1));
    auto I = luaL_checkinteger(L, lua_upvalueindex(2));

    auto it = s->begin();
    for (auto i = 0; i < I; i++)
        it++;
    if (it == s->end())
        return 0;

    lua_pushinteger(L, I + 1);
    lua_replace(L, lua_upvalueindex(2));
    Lua::push(L, *it);

    return 1;
}
static int selection_iterate(lua_State *L)
{
    auto s = lselection_check(L, 1);
    lua_pushinteger(L, 0);
    lua_pushcclosure(L, &selection_iterate_iterator, 2);
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
    {"on_updated", do_nothing},
    {NULL, NULL}
};


////////////////////////////////////////////////////////////////////////////////
// C++ facing
template<>
void Lua::push(lua_State *L, Sickle::Editor::Selection *selection)
{
    refman.get(selection);
    if (!lua_isnil(L, -1))
        return;
    else
        lua_pop(L, 1);

    auto ptr = static_cast<Sickle::Editor::Selection const **>(
        lua_newuserdatauv(L, sizeof(Sickle::Editor::Selection *), 0));
    *ptr = selection;
    luaL_setmetatable(L, "Sickle.editor.selection");

    // Connect signals.
    selection->signal_updated().connect(
        [L, selection](){
            refman.get(selection);
            Lua::call_method(L, "on_updated");
        });

    refman.set(selection, -1);
}

Sickle::Editor::Selection *lselection_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, "Sickle.editor.selection");
    luaL_argcheck(L, ud != NULL, arg, "`Sickle.editor.selection' expected");
    return *static_cast<Sickle::Editor::Selection **>(ud);
}

int luaopen_selection(lua_State *L)
{
    luaL_requiref(L, "editorbrush", luaopen_editorbrush, 1);

    // TODO: References should be removed when the C++ objects are destroyed.
    refman.init(L);

    luaL_newmetatable(L, "Sickle.editor.selection");
    lua_newtable(L);
    luaL_setfuncs(L, methods, 0);
    lua_setfield(L, -2, "__index");

    return 0;
}
