/**
 * Operation.hpp - Sickle map operations class.
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

#ifndef SE_EDITOR_OPERATION_HPP
#define SE_EDITOR_OPERATION_HPP

#include "editor/Editor.hpp"

#include <se-lua/se-lua.hpp>

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>


namespace Sickle::Editor
{
    class Operation
    {
        lua_State *const L;
    public:
        std::string const module_name;
        std::string const name;
        std::string const mode;
        std::string const args;

        Operation(
            lua_State *L,
            std::string const &module_name,
            std::string const &operation_name,
            std::string const &mode,
            std::string const &args);

        void execute(Editor &ed) const;
    };

    /**
     * Manages Operations.
     *
     * Operations are created by adding a Lua script through the 'add_source'
     * method. These script(s) call the Lua function 'add_operation':
     *
     *  add_operation(module: String, operation_title: String, mode: String,
     *                args: String, fn: Callable)
     */
    class OperationLoader
    {
        struct LuaCloseWrapper{void operator()(lua_State *L){lua_close(L);}};

        std::unique_ptr<lua_State, LuaCloseWrapper> const _L_actual;
        lua_State *const L; // alias for _L_actual

        std::vector<Operation>
        L_get_module_operations(std::string const &module_name) const;

    public:
        OperationLoader();

        /** Execute Lua code from the string. */
        void add_source(std::string const &source);

        /** Get a list of all the operations. */
        std::vector<Operation> get_operations() const;

        /** Get a list of operations in the module. */
        std::vector<Operation> get_module(std::string const &module_name) const;
    };
}

#endif
