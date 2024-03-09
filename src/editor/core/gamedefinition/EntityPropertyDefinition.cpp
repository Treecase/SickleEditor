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
    PropertyType type)
:   _name{name}
,   _default_value{default_value}
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


PropertyType EntityPropertyDefinition::type() const
{
    return _type;
}



/* EntityPropertyChoices ---------------------------------------------------- */
EntityPropertyDefinitionChoices::EntityPropertyDefinitionChoices(
    std::string const &name,
    std::string const &default_value,
    std::map<int, std::string> const &choices)
:   EntityPropertyDefinition{name, default_value, PropertyType::CHOICES}
,   _choices{choices}
{
}
