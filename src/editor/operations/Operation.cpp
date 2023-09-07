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

using namespace Sickle::Editor;


std::string const Operation::VALID_TYPES = "f";


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
,   arg_types{args}
{
    for (auto const ch : arg_types)
        if (VALID_TYPES.find(ch) == std::string::npos)
            throw std::runtime_error{"bad arg type '" + std::string{ch} + "'"};
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


void Operation::execute(Glib::RefPtr<Editor> ed, ArgList const &args) const
{
    execute(ed.get(), args);
}


void Operation::execute(Editor *ed, ArgList const &args) const
{
    if (arg_types.size() != args.size())
        throw std::invalid_argument{"incorrect number of arguments"};

    int const pre = lua_gettop(L);

    OperationLoader::_push_operation(L, module_name, name);
    // Push function.
    lua_getfield(L, -1, "function");

    // FIXME: TEMP
    assert(mode == "brush");

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
        throw std::runtime_error{"invalid mode '" + mode + "'"};

    // Push arguments.
    for (size_t i = 0; i < arg_types.size(); ++i)
    {
        auto const type = arg_types.at(i);
        switch (type)
        {
        case 'f':
            Lua::push(L, std::any_cast<lua_Number>(args.at(i)));
            break;
        }
    }
    Lua::checkerror(L, lua_pcall(L, 2+args.size(), 0, 0));

    lua_pop(L, 1);
    assert(lua_gettop(L) == pre);
}
