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

#include <core/Editor.hpp>
#include <se-lua/se-lua.hpp>

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>


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

        /**
         * Return the identifier for the given operation.
         * Current ID format is "<module>.<operation>".
         */
        static std::string id(
            std::string const &module,
            std::string const &operation);

        /** Break an ID into the module and operation names (in that order). */
        static std::pair<std::string, std::string> unid(std::string const &id);

        /**
         * Return the Operation's ID.
         * Current ID format is "<module>.<operation>".
         */
        auto id() const {return id(module_name, name);};

        Operation(
            lua_State *L,
            std::string const &module_name,
            std::string const &operation_name,
            std::string const &mode,
            std::string const &args);

        void execute(Glib::RefPtr<Editor> ed) const;
        void execute(Editor *ed) const;
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
        sigc::signal<void(std::string const &)> _sig_operation_added{};

        lua_State *const L; // alias for _L_actual

        std::vector<Operation>
        L_get_module_operations(std::string const &module_name) const;

        // Copying not allowed
        OperationLoader(OperationLoader const &)=delete;
        OperationLoader &operator=(OperationLoader const &)=delete;

    public:
        OperationLoader(lua_State *L);

        /**
         * Emitted when a new operation is added. The operation's ID is passed
         * as the parameter.
         */
        auto &signal_operation_added() {return _sig_operation_added;};

        /** Execute Lua code from the string. */
        void add_source(std::string const &source);

        /** Get a list of all the operations. */
        std::vector<Operation> get_operations() const;

        /** Get a single operation. */
        Operation get_operation(
            std::string const &module,
            std::string const &operation) const;
        /** Get a single operation. */
        Operation get_operation(std::string const &id);

        /** Get a list of operations in the module. */
        std::vector<Operation> get_module(std::string const &module_name) const;
    };
}

#endif
