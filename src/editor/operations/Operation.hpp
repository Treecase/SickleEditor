/**
 * Operation.hpp - Sickle map operations class.
 * Copyright (C) 2023-2024 Trevor Last
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

#include <editor/core/Editor.hpp>
#include <se-lua/se-lua.hpp>

#include <glibmm/refptr.h>
#include <glm/glm.hpp>

#include <string>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>


namespace Sickle::Editor
{
    class Operation
    {
    public:
        using Arg = std::variant<lua_Number, std::string, glm::vec3, glm::mat4>;
        using ArgList = std::vector<Arg>;

        /// Abstractly defines an argument to an operation.
        struct ArgDef
        {
            /// Typename of the argument.
            std::string const type;
            /// Name of the argument.
            std::string const name;
            /// Default value of the argument.
            Arg const default_value;

            ArgDef(
                std::string const &type,
                std::string const &name,
                Arg const &default_value)
            :   type{type}
            ,   name{name}
            ,   default_value{default_value}
            {
            }
        };

        /// Name of the module the operation is a part of.
        std::string const module_name;
        /// Name of the operation.
        std::string const name;
        /// Mode the operation acts on.
        std::string const mode;
        /// Abstract definition of arguments to the function.
        std::vector<ArgDef> const args;

        /**
         * Get the identifier for the given operation. Current ID format is
         * "<module>.<operation>".
         *
         * @param module Module name.
         * @param operation Operation name.
         * @return The ID for the operation.
         */
        static std::string id(
            std::string const &module,
            std::string const &operation);

        /**
         * Break an ID into the module and operation names (in that order).
         *
         * @param id The ID to seperate.
         * @return A `std::pair`, with module as the first element and
         * operation as the second.
         */
        static std::pair<std::string, std::string> unid(std::string const &id);

        /**
         * Default-construct an argument object.
         *
         * @param type Typename of object to construct.
         * @return The constructed object.
         */
        static Arg arg_default_construct(std::string const &type);

        /**
         * Convert a Lua object to a C++ one.
         *
         * @param type Typename of the Lua object.
         * @param L Lua state.
         * @param idx Stack index of the Lua object.
         * @return The converted object.
         */
        static Arg arg_from_lua(std::string const &type, lua_State *L, int idx);

        /**
         * Get all valid Operation modes.
         *
         * @return A collection of all valid modes.
         */
        static std::unordered_set<std::string> modes();

        /**
         * Get the identifier for this operation.
         *
         * @return The ID for the operation.
         */
        auto id() const {return id(module_name, name);};

        /**
         * Get the default value of the `argument`-th argument.
         *
         * @param argument Index of the argument to get.
         * @return Default value of the argument.
         */
        Arg make_arg(size_t argument) const;

        /**
         * Convert a Lua object to a C++ one, using the type from the
         * `argument`-th argument.
         *
         * @param argument Index of the argument to copy type from.
         * @param L Lua state.
         * @param idx Stack index of the Lua object.
         * @return The converted object.
         */
        Arg make_arg_from_lua(size_t argument, lua_State *L, int idx) const;

        /**
         * Check if `arg` matches the type of the `argument`-th parameter.
         *
         * @param argument Index of the parameter to check type against.
         * @param arg Value to check the type of.
         * @return True if the type of `arg` matches the indexed parameter.
         */
        bool check_type(size_t argument, Arg const &arg) const;

        /**
         * Execute the operation with the given arguments.
         *
         * @param ed Editor instance to use.
         * @param args Arguments to call the function with.
         */
        void execute(EditorRef ed, ArgList const &args) const;

    private:
        /// Contains mode-dependent data for use by operations.
        struct ModeData
        {
            /// Push an object onto the lua stack representing the contents of
            /// the selection.
            std::function<void(lua_State *L, Selection const &)> push_selection;
        };

        /// Type-dependent data for operations.
        struct TypeData
        {
            /// Convert a Lua value to a C++ value.
            std::function<Arg(lua_State *, int)> from_lua;
            /// Call std::holds_alternative with the correct type.
            std::function<bool(Arg const &)> holds_alternative;
            /// Default value for objects of type.
            Arg default_construct;
        };

        /// Argument typestrings and type-dependent data.
        static std::unordered_map<std::string, TypeData> const TYPES;

        /// Names valid modes and stores mode-dependant data for operations.
        static std::unordered_map<std::string, ModeData> const MODES;

        lua_State *const L;

        Operation(
            lua_State *L,
            std::string const &module_name,
            std::string const &operation_name,
            std::string const &mode,
            std::vector<ArgDef> const &args);

        // Operations can only be constructed through this function.
        friend Sickle::Editor::Operation Lua::get_as(lua_State *L, int idx);
    };
}

template<> Sickle::Editor::Operation Lua::get_as(lua_State *L, int idx);

#endif
