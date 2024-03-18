/**
 * EntityPropertyDefinition.cpp - Defines the entity property types.
 * Copyright (C) 2024 Trevor Last
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

#include "EntityPropertyDefinition.hpp"


using namespace Sickle::Editor;


EntityPropertyDefinition::EntityPropertyDefinition(
    std::string const &name,
    std::string const &default_value,
    std::string const &description,
    PropertyType type)
:   _name{name}
,   _default_value{default_value}
,   _description{description}
,   _type{type}
{
}


std::string EntityPropertyDefinition::name() const
{
    return _name;
}


std::string EntityPropertyDefinition::default_value() const
{
    return _default_value;
}


std::string EntityPropertyDefinition::description() const
{
    return _description;
}


PropertyType EntityPropertyDefinition::type() const
{
    return _type;
}



/* --- EntityPropertyChoices ------------------------------------------------ */
EntityPropertyDefinitionChoices::EntityPropertyDefinitionChoices(
    std::string const &name,
    std::string const &default_value,
    std::string const &description,
    std::map<int, std::string> const &choices)
:   EntityPropertyDefinition{
        name,
        default_value,
        description,
        PropertyType::CHOICES}
,   _choices{choices}
{
}


/* --- EntityPropertyDefinitionFlags ---------------------------------------- */
static std::string generate_default(
    std::map<int, std::pair<std::string, bool>> const &flagdef)
{
    unsigned int value = 0;
    for (auto const &kv : flagdef)
        value |= (kv.second.second? kv.first : 0);
    return std::to_string(value);
}

EntityPropertyDefinitionFlags::EntityPropertyDefinitionFlags(
    std::string const &name,
    std::map<int, std::pair<std::string, bool>> const &flags)
:   EntityPropertyDefinition{
        name,
        generate_default(flags),
        "",
        PropertyType::FLAGS}
{
    for (auto const &kv : flags)
        _flags.at(kv.first).emplace(kv.second);
}


bool EntityPropertyDefinitionFlags::is_bit_defined(int bit) const
{
    try {
        return _flags.at(bit).has_value();
    }
    catch (std::out_of_range const &) {
        return false;
    }
}


std::string EntityPropertyDefinitionFlags::get_description(int bit) const
{
    try {
        return _flags.at(bit).value_or(FlagDef{}).description;
    }
    catch (std::out_of_range const &) {
        return "";
    }
}


void EntityPropertyDefinitionFlags::merge(
    EntityPropertyDefinitionFlags const &other)
{
    for (size_t i = 0; i < other._flags.size(); ++i)
    {
        auto &a = _flags.at(i);
        auto const &b = other._flags.at(i);
        if (!a.has_value() && b.has_value())
            a = b.value();
    }
}


EntityPropertyDefinitionFlags::FlagDef::FlagDef(
    std::pair<std::string, bool> const &flagdef)
:   description{flagdef.first}
,   enabled_by_default{flagdef.second}
{
}
