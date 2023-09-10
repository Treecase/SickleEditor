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
    class Editor;

    class Operation
    {
        lua_State *const L;
    public:
        using Arg = std::variant<lua_Number, glm::vec3>;
        using ArgList = std::vector<Arg>;

        static std::unordered_set<std::string> const VALID_TYPES;

        std::string const module_name;
        std::string const name;
        std::string const mode;
        std::vector<std::string> const arg_types;

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

        Arg make_arg(size_t argument) const;
        Arg make_arg_from_string(
            size_t argument,
            std::string const &value) const;
        Arg make_arg_from_lua(size_t argument, lua_State *L, int idx) const;

        bool check_type(size_t argument, Arg const &arg) const;

        Operation(
            lua_State *L,
            std::string const &module_name,
            std::string const &operation_name,
            std::string const &mode,
            std::vector<std::string> const &args);

        void execute(Editor *ed, ArgList const &args) const;
        void execute(Glib::RefPtr<Editor> ed, ArgList const &args) const;
    };
}

#endif
