/**
 * OperationLoader.cpp - Load operations from Lua scripts.
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

/**
 * Lua Interface Explanation
 * =========================
 *
 * There is a toplevel Module Table in the Lua Registry. The Module Table is at
 * lightuserdata index `REGISTRY_KEY`.
 *
 * Each Module is contained in the Module Table, using the Module's name as the
 * index. A Module is just a table containing Operations.
 *
 * Each Operation is stored in the corresponding Module. The Operation's name
 * is the index.
 *
 * Operation
 * {
 *   mode: String ; names the mode this operation is active for.
 *   args: String ; argument types string.
 *   function: Function ; points to the corresponding Function value.
 * }
 */

#include "OperationLoader.hpp"

#include <cassert>

using namespace Sickle::Editor;

static char _REGISTRY_KEY_BASE = 'k';
static void *const REGISTRY_KEY = static_cast<void *>(&_REGISTRY_KEY_BASE);

void OperationLoader::_push_module_table(lua_State *L)
{
    lua_pushlightuserdata(L, REGISTRY_KEY);
    auto const type = lua_gettable(L, LUA_REGISTRYINDEX);
    if (type != LUA_TTABLE)
    {
        throw Lua::Error{"ModuleTable is not a table"};
    }
}

void OperationLoader::_push_module(lua_State *L, std::string const &module)
{
    _push_module_table(L);
    int const type = lua_getfield(L, -1, module.c_str());
    if (type != LUA_TTABLE)
    {
        throw Lua::Error{"Module '" + module + "' is not a table"};
    }
    lua_remove(L, -2);
}

void OperationLoader::_push_operation(
    lua_State *L,
    std::string const &module,
    std::string const &operation)
{
    _push_module(L, module);
    int const type = lua_getfield(L, -1, operation.c_str());
    if (type != LUA_TTABLE)
    {
        throw Lua::Error{
            "Operation '" + Operation::id(module, operation)
            + "' is not a table"};
    }
    lua_remove(L, -2);
}

/**
 * add_operation(module: String, operation: String, mode: String,
 *               args: Array[String], fn: Callable, [defaults: Array])
 *
 * Adds an operation with ID of 'MODULE.OPERATION'. MODE specifies which editor
 * mode the operation will be active in. ARGS is a list of strings naming the
 * types of any extra arguments to be passed to the function. FN is a callable
 * to be called when the operation is invoked. DEFAULTS is an optional
 * parameter, which is an array of values matching the types listed in ARGS.
 * These values are the defaults for the corresponding operation argument.
 *
 * If an operation with the same ID already exists, it will be overwritten.
 */
static int fn_add_operation(lua_State *L)
{
    auto const modname = luaL_checkstring(L, 1);
    auto const opname = luaL_checkstring(L, 2);
    auto const mode = luaL_checkstring(L, 3);
    luaL_argexpected(L, lua_istable(L, 4), 4, "table");
    bool const has_defaults = (lua_gettop(L) >= 6);
    if (has_defaults)
    {
        luaL_argexpected(L, lua_istable(L, 6), 6, "table");
    }

    auto const ptr = static_cast<OperationLoader *>(
        lua_touserdata(L, lua_upvalueindex(1)));
    if (!ptr)
    {
        return luaL_error(L, "bad upvalue");
    }

    OperationLoader::_push_module_table(L);

    // Try to get the module from the registry table.
    int const modtype = lua_getfield(L, -1, modname);

    // If module doesn't exist, create it.
    if (modtype == LUA_TNIL)
    {
        lua_pop(L, 1);
        lua_newtable(L);
        lua_setfield(L, -2, modname);
        lua_getfield(L, -1, modname);
    }
    else if (modtype != LUA_TTABLE)
    {
        throw Lua::Error{
            "Module '" + std::string{modname} + "' is not a table"};
    }

    // Add the operation to the module.
    lua_newtable(L);
    lua_pushvalue(L, 1);
    lua_setfield(L, -2, "module");
    lua_pushvalue(L, 2);
    lua_setfield(L, -2, "name");
    lua_pushvalue(L, 3);
    lua_setfield(L, -2, "mode");
    lua_pushvalue(L, 4);
    lua_setfield(L, -2, "args");
    lua_pushvalue(L, 5);
    lua_setfield(L, -2, "function");
    if (has_defaults)
    {
        lua_pushvalue(L, 6);
        lua_setfield(L, -2, "defaults");
    }

    lua_setfield(L, -2, opname);

    ptr->signal_operation_added().emit(Operation::id(modname, opname));
    return 0;
}

OperationLoader::OperationLoader(lua_State *L)
: L{L}
{
    if (!L)
    {
        throw std::invalid_argument{"null Lua state"};
    }
    luaL_checkversion(L);
    luaL_openlibs(L);

    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, fn_add_operation, 1);
    lua_setglobal(L, "add_operation");

    lua_pushlightuserdata(L, REGISTRY_KEY);
    lua_newtable(L);
    lua_settable(L, LUA_REGISTRYINDEX);
}

void OperationLoader::add_source(std::string const &source)
{
    Lua::checkerror(L, luaL_dostring(L, source.c_str()));
}

void OperationLoader::add_source_from_file(std::string const &path)
{
    Lua::checkerror(L, luaL_dofile(L, path.c_str()));
}

std::vector<Operation> OperationLoader::get_operations() const
{
    auto const pre = lua_gettop(L);
    std::vector<Operation> ops{};

    _push_module_table(L);

    // Iterate through modules.
    lua_pushnil(L);
    while (lua_next(L, -2) != 0)
    {
        auto const module_name = lua_tostring(L, -2);

        auto const mod_ops = L_get_module_operations(module_name);
        for (auto const &op : mod_ops)
        {
            ops.emplace_back(op);
        }

        lua_pop(L, 1);
    }
    lua_pop(L, 1);
    assert(lua_gettop(L) == pre);
    return ops;
}

Operation OperationLoader::get_operation(
    std::string const &module,
    std::string const &operation) const
{
    _push_operation(L, module, operation);
    auto const op = Lua::get_as<Operation>(L, -1);
    return op;
}

Operation OperationLoader::get_operation(std::string const &id)
{
    auto const module_and_operation = Operation::unid(id);
    return get_operation(
        module_and_operation.first,
        module_and_operation.second);
}

std::vector<Operation> OperationLoader::get_module(
    std::string const &module_name) const
{
    auto const pre = lua_gettop(L);
    _push_module(L, module_name);
    auto const ops = L_get_module_operations(module_name);
    lua_pop(L, 1);
    assert(lua_gettop(L) == pre);
    return ops;
}

std::vector<Operation> OperationLoader::L_get_module_operations(
    std::string const &module_name) const
{
    std::vector<Operation> ops{};
    lua_pushnil(L);
    while (lua_next(L, -2) != 0)
    {
        ops.push_back(Lua::get_as<Operation>(L, -1));
        lua_pop(L, 1);
    }
    return ops;
}
