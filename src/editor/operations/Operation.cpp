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

/* See OperationLoader.cpp for an explanation of the Lua interface. */

#include "Operation.hpp"
#include "OperationLoader.hpp"

#include <core/Editor.hpp>
#include <LuaGeo.hpp>

using namespace Sickle::Editor;


template<>
Operation Lua::get_as<Operation>(lua_State *L, int idx)
{
    int const I = lua_absindex(L, idx);

    int const module_type = lua_getfield(L, I, "module");
    int const name_type = lua_getfield(L, I, "name");
    int const mode_type = lua_getfield(L, I, "mode");
    int const args_type = lua_getfield(L, I, "args");
    int const defaults_type = lua_getfield(L, I, "defaults");
    bool const has_defaults = (defaults_type != LUA_TNIL);

    if (module_type != LUA_TSTRING) throw Lua::Error{"module is not a string"};
    if (name_type != LUA_TSTRING) throw Lua::Error{"name is not a string"};
    if (mode_type != LUA_TSTRING) throw Lua::Error{"mode is not a string"};
    if (args_type != LUA_TTABLE) throw Lua::Error{"args is not a table"};
    if (has_defaults && defaults_type != LUA_TTABLE)
        throw Lua::Error{"args is not a table"};

    auto const module = lua_tostring(L, I + 1);
    auto const name = lua_tostring(L, I + 2);
    auto const mode = lua_tostring(L, I + 3);

    std::vector<Operation::ArgDef> args{};
    for (lua_Integer i = 1; ; ++i)
    {
        int const arg_type = lua_geti(L, I + 4, i);
        if (arg_type == LUA_TNIL)
            break;
        else if (arg_type == LUA_TSTRING)
        {
            auto const type = lua_tostring(L, -1);
            Operation::Arg value{};
            if (has_defaults)
            {
                lua_geti(L, I + 5, i);
                value = Operation::arg_from_lua(type, L, -1);
                lua_pop(L, 1);
            }
            else
                value = Operation::arg_default_construct(type);
            args.emplace_back(type, value);

        }
        else
            throw Lua::Error{"arg is not a string"};
        lua_pop(L, 1);
    }

    lua_pop(L, 6);
    return Operation{L, module, name, mode, args};
}



std::unordered_set<std::string> const Operation::VALID_TYPES{
    "f",
    "string",
    "vec3",
    "mat4",
};


std::unordered_set<std::string> const Operation::VALID_MODES{
    "brush"
};


static std::unordered_map<
    std::string,
    std::function<Operation::Arg(lua_State *, int)>> const ARG_FROM_LUA_MAP
{
    {"f", Lua::get_as<lua_Number>},
    {"string", Lua::get_as<std::string>},
    {"vec3", Lua::get_as<glm::vec3>},
    {"mat4", Lua::get_as<glm::mat4>},
};


static std::unordered_map<
    std::string,
    Operation::Arg> const ARG_DEFAULT_CONSTRUCT
{
    {"f", static_cast<lua_Number>(0.0)},
    {"string", std::string{}},
    {"vec3", glm::vec3{}},
    {"mat4", glm::identity<glm::mat4>()},
};


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


Operation::Arg Operation::arg_from_lua(
    std::string const &type,
    lua_State *l,
    int idx)
{
    try {
        return ARG_FROM_LUA_MAP.at(type)(l, idx);
    }
    catch (std::out_of_range const &e) {
        throw std::logic_error{"bad argument type '" + type + "'"};
    }
}


Operation::Arg Operation::arg_default_construct(std::string const &type)
{
    try {
        return ARG_DEFAULT_CONSTRUCT.at(type);
    }
    catch (std::out_of_range const &e) {
        throw std::logic_error{"bad argument type '" + type + "'"};
    }
}


Operation::Operation(
    lua_State *L,
    std::string const &module_name,
    std::string const &operation_name,
    std::string const &mode,
    std::vector<ArgDef> const &args)
:   L{L}
,   module_name{module_name}
,   name{operation_name}
,   mode{mode}
,   args{args}
{
    for (auto const arg : args)
        if (VALID_TYPES.count(arg.type) == 0)
            throw std::runtime_error{"bad arg type '" + arg.type + "'"};
    if (VALID_MODES.count(mode) == 0)
        throw std::runtime_error{"bad mode '" + mode + "'"};
}


Operation::Arg Operation::make_arg(size_t argument) const
{
    return args.at(argument).default_value;
}


Operation::Arg Operation::make_arg_from_lua(
    size_t argument,
    lua_State *l,
    int idx) const
{
    return arg_from_lua(args.at(argument).type, l, idx);
}


bool Operation::check_type(size_t argument, Arg const &arg) const
{
    auto const &type = args.at(argument).type;
    if (type == "f")
        return std::holds_alternative<lua_Number>(arg);
    else if (type == "string")
        return std::holds_alternative<std::string>(arg);
    else if (type == "vec3")
        return std::holds_alternative<glm::vec3>(arg);
    else if (type == "mat4")
        return std::holds_alternative<glm::mat4>(arg);
    else
        throw std::logic_error{"bad argument type '" + type + "'"};
}


void Operation::execute(Glib::RefPtr<Editor> ed, ArgList const &args) const
{
    execute(ed.get(), args);
}


void Operation::execute(Editor *ed, ArgList const &passed_args) const
{
    if (args.size() != passed_args.size())
        throw std::invalid_argument{"incorrect number of arguments"};

    int const pre = lua_gettop(L);

    OperationLoader::_push_operation(L, module_name, name);
    // Push function.
    lua_getfield(L, -1, "function");

    Lua::push(L, ed);
    if (mode == "brush")
    {
        // Push selected objects list.
        lua_newtable(L);
        lua_Integer i = 1;
        for (auto const &brush : ed->selected)
        {
            int const top = lua_gettop(L);

            Lua::push(L, brush);
            int const top2 = lua_gettop(L);
            assert(top2 == top + 1);

            lua_seti(L, -2, i++);
            int const top3 = lua_gettop(L);
            assert(top3 == top);
        }
    }
    else
        throw std::logic_error{"invalid mode '" + mode + "'"};

    // Push arguments.
    for (size_t i = 0; i < args.size(); ++i)
    {
        auto const &arg = passed_args.at(i);
        if (!check_type(i, arg))
            throw std::runtime_error{"mismatched Arg type"};
        std::visit([this](auto v){Lua::push(L, v);}, arg);
    }
    Lua::checkerror(L, lua_pcall(L, 2+args.size(), 0, 0));

    lua_pop(L, 1);
    assert(lua_gettop(L) == pre);
}
