/**
 * EntityProperties.hpp - Defines the entity property types.
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

#include "EntityProperties.hpp"


using namespace Sickle::Editor;


std::shared_ptr<EntityProperty> EntityPropertyFactory::construct(
    std::shared_ptr<FGD::Property> const &prop)
{
    std::string const &name = prop->name;
    std::string default_value = "";

    if (typeid(*prop.get()) == typeid(FGD::IntegerProperty))
    {
        auto const intprop =\
            std::dynamic_pointer_cast<FGD::IntegerProperty>(prop);
        if (intprop->defaultvalue.has_value())
            default_value = std::to_string(intprop->defaultvalue.value());
    }
    else if (typeid(*prop.get()) == typeid(FGD::StringProperty))
    {
        auto const strprop =\
            std::dynamic_pointer_cast<FGD::StringProperty>(prop);
        if (strprop->defaultvalue.has_value())
            default_value = strprop->defaultvalue.value();
    }
    else if (typeid(*prop.get()) == typeid(FGD::ChoiceProperty))
    {
        auto const choiceprop =\
            std::dynamic_pointer_cast<FGD::ChoiceProperty>(prop);
        if (choiceprop->defaultvalue.has_value())
            default_value = std::to_string(choiceprop->defaultvalue.value());
    }
    else if (typeid(*prop.get()) == typeid(FGD::FlagProperty))
    {
        auto const flagprop =\
            std::dynamic_pointer_cast<FGD::FlagProperty>(prop);
        uint32_t flags = 0;
        for (auto const &kv : flagprop->flags)
            flags |= kv.second.start_value? kv.first : 0;
        default_value = std::to_string(flags);
    }
    else if (typeid(*prop.get()) == typeid(FGD::TargetSourceProperty))
    {
        auto const targetsourceprop =\
            std::dynamic_pointer_cast<FGD::TargetSourceProperty>(prop);
    }
    else if (typeid(*prop.get()) == typeid(FGD::TargetDestinationProperty))
    {
        auto const targetdestinationprop =\
            std::dynamic_pointer_cast<FGD::TargetDestinationProperty>(prop);
    }
    else if (typeid(*prop.get()) == typeid(FGD::Color255Property))
    {
        auto const colorprop =\
            std::dynamic_pointer_cast<FGD::Color255Property>(prop);
        default_value = colorprop->value;
    }
    else if (typeid(*prop.get()) == typeid(FGD::StudioProperty))
    {
        auto const studioprop =\
            std::dynamic_pointer_cast<FGD::StudioProperty>(prop);
    }
    else if (typeid(*prop.get()) == typeid(FGD::SpriteProperty))
    {
        auto const spriteprop =\
            std::dynamic_pointer_cast<FGD::SpriteProperty>(prop);
    }
    else if (typeid(*prop.get()) == typeid(FGD::SoundProperty))
    {
        auto const soundprop =\
            std::dynamic_pointer_cast<FGD::SoundProperty>(prop);
    }
    else if (typeid(*prop.get()) == typeid(FGD::DecalProperty))
    {
        auto const decalprop =\
            std::dynamic_pointer_cast<FGD::DecalProperty>(prop);
    }

    return std::make_shared<EntityProperty>(name, default_value);
}
