/**
 * EntityClass.cpp - Entity class information.
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

#include "GameDefinition.hpp"

using namespace Sickle::Editor;


EntityClass::EntityClass(std::string const &type)
:   _type{type}
{
}


std::string EntityClass::type() const
{
    return _type;
}


EntityProperty *EntityClass::get_property(std::string const &name)
{
    try {
        return _entity_properties.at(name).get();
    }
    catch (std::out_of_range const &e) {
        return nullptr;
    }
}


std::vector<EntityProperty *> EntityClass::get_entity_properties() const
{
    std::vector<EntityProperty *> properties{};
    for (auto const &kv : _entity_properties)
        properties.push_back(kv.second.get());
    return properties;
}



void EntityClass::add_class_property(
    std::shared_ptr<ClassProperty> const &property)
{
    if (!property)
        return;
    _class_properties.insert_or_assign(
        std::type_index{typeid(*property.get())},
        property);
}


void EntityClass::add_entity_property(
    std::shared_ptr<EntityProperty> const &property)
{
    if (!property)
        return;
    _entity_properties.insert_or_assign(property->name(), property);
}
