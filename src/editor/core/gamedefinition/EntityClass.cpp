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

#include "EntityClass.hpp"

using namespace Sickle::Editor;


EntityClass::EntityClass(FGD::Class const &cls)
:   _type{cls.type()}
{
    for (auto const &class_property : cls.attributes)
    {
        add_class_property(
            ClassPropertyFactory::construct(class_property));
    }
    for (auto const &entity_property : cls.properties)
    {
        add_entity_property(
            EntityPropertyDefinitionFactory::construct(entity_property));
    }
}


std::string EntityClass::type() const
{
    return _type;
}


bool EntityClass::has_property(std::string const &name) const
{
    return _entity_properties.count(name) != 0;
}


std::shared_ptr<EntityPropertyDefinition> EntityClass::get_property(
    std::string const &name) const
{
    try {
        return _entity_properties.at(name);
    }
    catch (std::out_of_range const &e) {
        return nullptr;
    }
}


std::vector<std::shared_ptr<EntityPropertyDefinition>>
EntityClass::get_entity_properties() const
{
    std::vector<std::shared_ptr<EntityPropertyDefinition>> properties{};
    for (auto const &kv : _entity_properties)
        properties.push_back(kv.second);
    return properties;
}


void EntityClass::inherit_from(EntityClass const &other)
{
    for (auto const &kv : other._class_properties)
        add_class_property(kv.second);
    for (auto const &kv : other._entity_properties)
        add_entity_property(kv.second);
}



void EntityClass::add_class_property(
    std::shared_ptr<ClassProperty> const &property)
{
    if (!property)
        return;
    _class_properties.insert({
        std::type_index{typeid(*property.get())},
        property});
}


void EntityClass::add_entity_property(
    std::shared_ptr<EntityPropertyDefinition> const &property)
{
    if (!property)
        return;
    // Flag properties need special handling...
    if (auto const &newflagprop =\
        std::dynamic_pointer_cast<EntityPropertyDefinitionFlags>(property))
    {
        std::shared_ptr<EntityPropertyDefinitionFlags> flagprop{nullptr};
        try {
            flagprop = std::dynamic_pointer_cast<EntityPropertyDefinitionFlags>(
                get_property(property->name()));
        }
        catch (std::out_of_range const &) {
        }
        // ...But only if the property already exists.
        if (flagprop)
        {
            flagprop->merge(*newflagprop);
            return;
        }
    }
    _entity_properties.insert({property->name(), property});
}
