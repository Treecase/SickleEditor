/**
 * Operation.cpp - Sickle map operations class.
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

#include "operations/Operation.hpp"

#include <iostream> // temp


using namespace Sickle::Editor;


static char _REGISTRY_KEY_BASE = 'k';
static void *const REGISTRY_KEY = static_cast<void *>(&_REGISTRY_KEY_BASE);


static void _push_module_table(lua_State *L)
{
    lua_pushlightuserdata(L, REGISTRY_KEY);
    auto const type = lua_gettable(L, LUA_REGISTRYINDEX);
    if (type != LUA_TTABLE)
        throw Lua::Error{"ModuleTable is not a table"};
}


static void _push_module(lua_State *L, std::string const &module)
{
    _push_module_table(L);
    int const type = lua_getfield(L, -1, module.c_str());
    if (type != LUA_TTABLE)
        throw Lua::Error{"Module '" + module + "' is not a table"};
    lua_remove(L, -2);
}


static void _push_operation(
    lua_State *L,
    std::string const &module,
    std::string const &operation)
{
    _push_module(L, module);
    int const type = lua_getfield(L, -1, operation.c_str());
    if (type != LUA_TTABLE)
    {
        throw Lua::Error{
            "Operation '"
            + Operation::id(module, operation)
            + "' is not a table"};
    }
    lua_remove(L, -2);
}


/**
 * add_operation(module: String, operation: String, mode: String, args: String,
 *               fn: Callable)
 *
 * If an operation with the same name already exists in the table, it will be
 * overwritten.
 */
static int fn_add_operation(lua_State *L)
{
    auto const modname = luaL_checkstring(L, 1);
    auto const opname = luaL_checkstring(L, 2);
    auto const mode = luaL_checkstring(L, 3);
    auto const args = luaL_checkstring(L, 4);
    auto const ptr = static_cast<OperationLoader *>(
        lua_touserdata(L, lua_upvalueindex(1)));
    if (!ptr)
        return luaL_error(L, "bad upvalue");

    _push_module_table(L);

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
    // Operation{mode:String, args:String, function:Callable}
    lua_newtable(L);
    lua_pushvalue(L, 3);
    lua_setfield(L, -2, "mode");
    lua_pushvalue(L, 4);
    lua_setfield(L, -2, "args");
    lua_pushvalue(L, 5);
    lua_setfield(L, -2, "function");

    lua_setfield(L, -2, opname);

    ptr->signal_operation_added().emit(Operation::id(modname, opname));
    return 0;
}


/* ===[ Operation ]=== */
Operation::Operation(
    lua_State *L,
    std::string const &module_name,
    std::string const &operation_name,
    std::string const &mode,
    std::string const &args)
:   L{L}
,   module_name{module_name}
,   name{operation_name}
,   mode{mode}
,   args{args}
{
}


std::string Operation::id(
    std::string const &module,
    std::string const &operation)
{
    return module + "." + operation;
}


std::pair<std::string, std::string> Operation::unid(std::string const &id)
{
    auto const pos = id.find('.');
    if (pos == std::string::npos)
        throw std::runtime_error{"Not an ID"};
    auto const module = id.substr(0, pos);
    auto const operation = id.substr(pos + 1);
    return std::make_pair(module, operation);
}


void Operation::execute(Glib::RefPtr<Editor> ed) const
{
    execute(ed.get());
}


void Operation::execute(Editor *ed) const
{
    int const pre = lua_gettop(L);

    _push_operation(L, module_name, name);
    // Push function.
    lua_getfield(L, -1, "function");

    // FIXME: TEMP
    assert(mode == "brush" || mode == "editor");

    if (mode == "brush")
    {
        // Push selected objects list.
        lua_newtable(L);
        lua_Integer i = 1;
        for (auto const &brush : ed->selected)
        {
            int const top = lua_gettop(L);
            std::cout << top << std::endl;

            Lua::push(L, brush);
            int const top2 = lua_gettop(L);
            std::cout << top2 << std::endl;
            assert(top2 == top + 1);

            lua_seti(L, -2, i++);
            int const top3 = lua_gettop(L);
            std::cout << top3 << std::endl;
            assert(top3 == top);
        }
    }
    else if (mode == "editor")
    {
        Lua::push(L, ed);
    }
    else
        throw std::runtime_error{"invalid mode '" + mode + "'"};

    for (auto const ch : args)
        lua_pushnil(L); // TEMP: push args
    Lua::checkerror(L, lua_pcall(L, 1+args.size(), 0, 0));

    lua_pop(L, 1);
    assert(lua_gettop(L) == pre);
}


/* ===[ OperationLoader ]=== */
OperationLoader::OperationLoader(lua_State *L)
:   L{L}
{
    if (!L)
        throw std::invalid_argument{"null Lua state"};
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
            ops.emplace_back(op);

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
    auto const pre = lua_gettop(L);

    _push_operation(L, module, operation);
    int const modetype = lua_getfield(L, -1, "mode");
    int const argstype = lua_getfield(L, -2, "args");

    if (modetype != LUA_TSTRING) throw Lua::Error{"mode is not a string"};
    if (argstype != LUA_TSTRING) throw Lua::Error{"args is not a string"};

    auto const mode = lua_tostring(L, -2);
    auto const args = lua_tostring(L, -1);
    lua_pop(L, 3);

    Operation op{L, module, operation, mode, args};

    assert(lua_gettop(L) == pre);
    return op;
}


Operation OperationLoader::get_operation(std::string const &id)
{
    auto const module_and_operation = Operation::unid(id);
    return get_operation(
        module_and_operation.first,
        module_and_operation.second);
}


std::vector<Operation>
OperationLoader::get_module(std::string const &module_name) const
{
    auto const pre = lua_gettop(L);
    _push_module(L, module_name);
    auto const ops = L_get_module_operations(module_name);
    lua_pop(L, 1);
    assert(lua_gettop(L) == pre);
    return ops;
}



std::vector<Operation>
OperationLoader::L_get_module_operations(std::string const &module_name) const
{
    std::vector<Operation> ops{};

    // Iterate through operations in the module.
    lua_pushnil(L);
    while (lua_next(L, -2) != 0)
    {
        auto const operation_name = lua_tostring(L, -2);

        // Get the mode and args from the operation.
        lua_getfield(L, -1, "mode");
        lua_getfield(L, -2, "args");
        auto const mode = lua_tostring(L, -2);
        auto const args = lua_tostring(L, -1);
        lua_pop(L, 2);

        ops.emplace_back(L, module_name, operation_name, mode, args);

        lua_pop(L, 1);
    }

    return ops;
}
