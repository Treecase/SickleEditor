/**
 * Entity_Lua.cpp - Editor Entity Lua binding.
 * Copyright (C) 2024 Trevor Last
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

#include <editor/world/Entity.hpp>
#include <se-lua/lua-geo/LuaGeo.hpp>
#include <se-lua/utils/RefBuilder.hpp>

#define METATABLE "Sickle.entity"

using namespace Sickle::Editor;


////////////////////////////////////////////////////////////////////////////////
// Methods
static int is_selected(lua_State *L)
{
    auto entity = lentity_check(L, 1);
    lua_pushboolean(L, entity->is_selected());
    return 1;
}


static int classname(lua_State *L)
{
    auto entity = lentity_check(L, 1);
    Lua::push(L, entity->classname());
    return 1;
}


static int get_property(lua_State *L)
{
    auto entity = lentity_check(L, 1);
    auto const key = luaL_checkstring(L, 2);
    Lua::push(L, entity->get_property(key));
    return 1;
}


static int set_property(lua_State *L)
{
    auto entity = lentity_check(L, 1);
    auto const key = luaL_checkstring(L, 2);
    auto const value = luaL_checkstring(L, 3);
    entity->set_property(key, value);
    return 0;
}


static int remove_property(lua_State *L)
{
    auto entity = lentity_check(L, 1);
    auto const key = luaL_checkstring(L, 2);
    Lua::push(L, entity->remove_property(key));
    return 1;
}


static int get_brushes(lua_State *L)
{
    auto const entity = lentity_check(L, 1);
    lua_newtable(L);
    lua_Integer idx = 1;
    for (auto const &brush : entity->brushes())
    {
        Lua::push(L, brush);
        lua_seti(L, -2, idx++);
    }
    return 1;
}


static int add_brush(lua_State *L)
{
    auto entity = lentity_check(L, 1);
    auto const brush = leditorbrush_check(L, 2);
    entity->add_brush(brush);
    return 0;
}


static int remove_brush(lua_State *L)
{
    auto entity = lentity_check(L, 1);
    auto const brush = leditorbrush_check(L, 2);
    entity->remove_brush(brush);
    return 0;
}


static int do_nothing(lua_State *L)
{
    return 0;
}


static luaL_Reg methods[] = {
    {"is_selected", is_selected},
    {"classname", classname},
    {"get_property", get_property},
    {"set_property", set_property},
    {"remove_property", remove_property},
    {"get_brushes", get_brushes},
    {"add_brush", add_brush},
    {"remove_brush", remove_brush},
    {NULL, NULL}
};



////////////////////////////////////////////////////////////////////////////////
// C++ facing
template<>
void Lua::push(lua_State *L, EntityRef entity)
{
    Lua::RefBuilder builder{L, METATABLE, entity};
    if (builder.pushnew())
        return;
    entity->signal_removed().connect(
        [L, entity](){
            ReferenceManager refman{};
            refman.erase(L, entity);
        });
    builder.finish();
}


EntityRef lentity_check(lua_State *L, int arg)
{
    void *ud = luaL_checkudata(L, arg, METATABLE);
    luaL_argcheck(L, ud != NULL, arg, "`" METATABLE "' expected");
    return *static_cast<EntityRef *>(ud);
}


int luaopen_entity(lua_State *L)
{
    lua_newtable(L);
    luaL_newmetatable(L, METATABLE);
    luaL_setfuncs(L, methods, 0);
    lua_setfield(L, -2, "metatable");
    Lua::RefBuilder<Entity>::setup_indexing(L, METATABLE);
    return 1;
}
