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
    public:
        using Arg = std::variant<lua_Number, std::string, glm::vec3, glm::mat4>;
        using ArgList = std::vector<Arg>;

        struct ArgDef
        {
            std::string const type;
            Arg const default_value;

            ArgDef(std::string const &type, Arg const &default_value)
            :   type{type}
            ,   default_value{default_value}
            {
            }
        };

        std::string const module_name;
        std::string const name;
        std::string const mode;
        std::vector<ArgDef> const args;

        /**
         * Return the identifier for the given operation.
         * Current ID format is "<module>.<operation>".
         */
        static std::string id(
            std::string const &module,
            std::string const &operation);

        /** Break an ID into the module and operation names (in that order). */
        static std::pair<std::string, std::string> unid(std::string const &id);

        static Arg arg_default_construct(std::string const &type);
        static Arg arg_from_lua(std::string const &type, lua_State *L, int idx);

        /**
         * Return the Operation's ID.
         * Current ID format is "<module>.<operation>".
         */
        auto id() const {return id(module_name, name);};

        Arg make_arg(size_t argument) const;
        Arg make_arg_from_lua(size_t argument, lua_State *L, int idx) const;

        bool check_type(size_t argument, Arg const &arg) const;

        void execute(Editor *ed, ArgList const &args) const;
        void execute(Glib::RefPtr<Editor> ed, ArgList const &args) const;

    private:
        lua_State *const L;

        static std::unordered_set<std::string> const VALID_TYPES;
        static std::unordered_set<std::string> const VALID_MODES;

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
