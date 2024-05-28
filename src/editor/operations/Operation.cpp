/**
 * Operation.cpp - Sickle map operations class.
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

/* See OperationLoader.cpp for an explanation of the Lua interface. */

#include "Operation.hpp"
#include "OperationLoader.hpp"

#include <editor/core/Editor.hpp>
#include <editor/lua/Editor_Lua.hpp>
#include <se-lua/lua-geo/LuaGeo.hpp>


using namespace Sickle::Editor;


std::unordered_map<std::string, Operation::ModeData> const Operation::MODES
{
    {   "brush",
        {
            [](lua_State *L, Selection const &sel) -> void {
                lua_newtable(L);
                lua_Integer i = 1;
                for (auto const &brush : sel.get_all_of_type<Brush>())
                {
                    Lua::push(L, brush);
                    lua_seti(L, -2, i++);
                }
            },
        }
    },
    {   "entity",
        {
            [](lua_State *L, Selection const &sel) -> void {
                lua_newtable(L);
                lua_Integer i = 1;
                for (auto const &entity : sel.get_all_of_type<Entity>())
                {
                    Lua::push(L, entity);
                    lua_seti(L, -2, i++);
                }
            },
        }
    },
    {   "face",
        {
            [](lua_State *L, Selection const &sel) -> void {
                lua_newtable(L);
                lua_Integer i = 1;
                for (auto const &face : sel.get_all_of_type<Face>())
                {
                    Lua::push(L, face);
                    lua_seti(L, -2, i++);
                }
            }
        }
    },
    {   "object",
        {
            [](lua_State *L, Selection const &sel) -> void {
                lua_newtable(L);
                lua_Integer i = 1;
                for (auto const &obj : sel)
                {
                    auto const edobj = EditorObjectRef::cast_dynamic(obj);
                    if (!edobj)
                        throw std::bad_cast{};
                    auto ptr = static_cast<EditorObjectRef *>(
                        lua_newuserdatauv(L, sizeof(EditorObjectRef), 0));
                    std::uninitialized_copy(&edobj, (&edobj) + 1, ptr);
                    lua_seti(L, -2, i++);
                }
            },
        }
    },
};


std::unordered_map<std::string, Operation::TypeData> const Operation::TYPES{
    {   "f",
        {
            Lua::get_as<lua_Number>,
            [](auto a){return std::holds_alternative<lua_Number>(a);},
            static_cast<lua_Number>(0.0),
        }
    },
    {   "string",
        {
            Lua::get_as<std::string>,
            [](auto a){return std::holds_alternative<std::string>(a);},
            "",
        },
    },
    {   "texture",
        {
            Lua::get_as<std::string>,
            [](auto a){return std::holds_alternative<std::string>(a);},
            "",
        },
    },
    {   "vec3",
        {
            Lua::get_as<glm::vec3>,
            [](auto a){return std::holds_alternative<glm::vec3>(a);},
            glm::vec3{},
        },
    },
    {   "mat4",
        {
            Lua::get_as<glm::mat4>,
            [](auto a){return std::holds_alternative<glm::mat4>(a);},
            glm::identity<glm::mat4>(),
        },
    },
};


std::unordered_set<std::string> Operation::modes()
{
    std::unordered_set<std::string> the_modes{};
    for (auto const &kv : MODES)
        the_modes.insert(kv.first);
    return the_modes;
}



template<>
Operation Lua::get_as<Operation>(lua_State *L, int idx)
{
    if (!lua_checkstack(L, 6))
        throw Lua::StackOverflow{};

    int const I = lua_absindex(L, idx);
    int const T = lua_gettop(L);

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

    auto const module = lua_tostring(L, T + 1);
    auto const name = lua_tostring(L, T + 2);
    auto const mode = lua_tostring(L, T + 3);

    // Get the argument names (first two are skipped since they are the editor
    // and the selection).
    lua_getfield(L, I, "function");
    std::vector<std::string> arg_names{};
    char const *arg_name = nullptr;
    for (int i = 3; (arg_name = lua_getlocal(L, nullptr, i)) != nullptr; ++i)
        arg_names.push_back(arg_name);
    lua_pop(L, 1);

    std::vector<Operation::ArgDef> args{};
    for (lua_Integer i = 1; ; ++i)
    {
        int const arg_type = lua_geti(L, T + 4, i);
        if (arg_type == LUA_TNIL)
        {
            lua_pop(L, 1);
            break;
        }
        else if (arg_type == LUA_TSTRING)
        {
            auto const type = lua_tostring(L, -1);
            Operation::Arg value{};
            if (has_defaults)
            {
                lua_geti(L, T + 5, i);
                value = Operation::arg_from_lua(type, L, -1);
                lua_pop(L, 1);
            }
            else
                value = Operation::arg_default_construct(type);

            args.emplace_back(
                type,
                static_cast<size_t>(i) <= arg_names.size()
                    ? arg_names.at(i - 1)
                    : "",
                value);
            lua_pop(L, 1);
        }
        else
        {
            // Clear the stack before throwing.
            lua_pop(L, 6);
            throw Lua::Error{"arg is not a string"};
        }
    }

    // Pop fields pushed at top of function.
    lua_pop(L, 5);
    auto const t2 = lua_gettop(L);
    assert(t2 == T);
    return Operation{L, module, name, mode, args};
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


Operation::Arg Operation::arg_default_construct(std::string const &type)
{
    try {
        return TYPES.at(type).default_construct;
    }
    catch (std::out_of_range const &e) {
        throw std::logic_error{"bad argument type '" + type + "'"};
    }
}


Operation::Arg Operation::arg_from_lua(
    std::string const &type,
    lua_State *l,
    int idx)
{
    try {
        return TYPES.at(type).from_lua(l, idx);
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
:   module_name{module_name}
,   name{operation_name}
,   mode{mode}
,   args{args}
,   L{L}
{
    if (MODES.count(mode) == 0)
        throw std::runtime_error{"bad mode '" + mode + "'"};
    for (auto const &arg : args)
        if (TYPES.count(arg.type) == 0)
            throw std::runtime_error{"bad arg type '" + arg.type + "'"};
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
    try {
        return TYPES.at(type).holds_alternative(arg);
    }
    catch (std::out_of_range const &e) {
        throw std::logic_error{"bad argument type '" + type + "'"};
    }
}


void Operation::execute(EditorRef ed, ArgList const &passed_args) const
{
    if (args.size() != passed_args.size())
        throw std::invalid_argument{"incorrect number of arguments"};

    int const pre = lua_gettop(L);

    OperationLoader::_push_operation(L, module_name, name);

    // Push function.
    lua_getfield(L, -1, "function");

    // Remove the operation object.
    lua_remove(L, -2);

    // Arg 1 is the editor.
    Lua::push(L, ed);

    // Arg 2 is selection for this mode.
    if (MODES.count(mode) != 0)
        std::invoke(MODES.at(mode).push_selection, L, ed->selected);
    else
        throw std::logic_error{"invalid mode '" + mode + "'"};

    // Push the rest of the arguments.
    for (size_t i = 0; i < args.size(); ++i)
    {
        auto const &arg = passed_args.at(i);
        if (!check_type(i, arg))
            throw std::runtime_error{"mismatched Arg type"};
        std::visit([this](auto v){Lua::push(L, v);}, arg);
    }

    // Call the function.
    Lua::checkerror(L, Lua::pcall(L, 2+args.size(), 0));

    assert(lua_gettop(L) == pre);
}
