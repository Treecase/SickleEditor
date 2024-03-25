/**
 * EntityPropertyDefinitionFactory.hpp - Factory to construct
 *                                       EntityPropertyDefinitions.
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

#include <cmath>


using namespace Sickle::Editor;


std::shared_ptr<EntityPropertyDefinition>
EntityPropertyDefinitionFactory::construct(
    std::shared_ptr<FGD::Property> const &prop)
{
    std::string const &name = prop->name;
    std::string default_value = "";
    std::string description = "";
    PropertyType type = PropertyType::STRING;

    if (auto const descprop =\
        std::dynamic_pointer_cast<FGD::DescriptionProperty>(prop))
    {
        description = descprop->description.value_or("");
    }

    if (typeid(*prop.get()) == typeid(FGD::IntegerProperty))
    {
        type = PropertyType::INTEGER;
        auto const intprop =\
            std::dynamic_pointer_cast<FGD::IntegerProperty>(prop);
        if (intprop->defaultvalue.has_value())
            default_value = std::to_string(intprop->defaultvalue.value());
    }
    else if (typeid(*prop.get()) == typeid(FGD::StringProperty))
    {
        type = PropertyType::STRING;
        auto const strprop =\
            std::dynamic_pointer_cast<FGD::StringProperty>(prop);
        if (strprop->defaultvalue.has_value())
            default_value = strprop->defaultvalue.value();
    }
    else if (typeid(*prop.get()) == typeid(FGD::ChoiceProperty))
    {
        auto const choiceprop =\
            std::dynamic_pointer_cast<FGD::ChoiceProperty>(prop);
        std::string const default_value =\
            std::to_string(choiceprop->defaultvalue.value_or(0));
        return std::make_shared<EntityPropertyDefinitionChoices>(
            prop->name,
            default_value,
            choiceprop->description.value_or(""),
            choiceprop->choices);
    }
    else if (typeid(*prop.get()) == typeid(FGD::FlagProperty))
    {
        auto const flagprop =\
            std::dynamic_pointer_cast<FGD::FlagProperty>(prop);

        std::map<int, std::pair<std::string, bool>> flagdefs{};
        for (auto const &kv : flagprop->flags)
        {
            flagdefs.insert({
                static_cast<int>(std::log2(kv.first)),
                {kv.second.description, kv.second.start_value}});
        }

        return std::make_shared<EntityPropertyDefinitionFlags>(
            flagprop->name,
            flagdefs);
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
        return std::make_shared<EntityPropertyDefinitionColor255>(
            name,
            colorprop->value,
            description);
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
        default_value = spriteprop->defaultvalue.value_or(default_value);
        type = PropertyType::SPRITE;
    }
    else if (typeid(*prop.get()) == typeid(FGD::SoundProperty))
    {
        auto const soundprop =\
            std::dynamic_pointer_cast<FGD::SoundProperty>(prop);
        default_value = soundprop->defaultvalue.value_or(default_value);
        type = PropertyType::SOUND;
    }
    else if (typeid(*prop.get()) == typeid(FGD::DecalProperty))
    {
        auto const decalprop =\
            std::dynamic_pointer_cast<FGD::DecalProperty>(prop);
        type = PropertyType::DECAL;
    }

    return std::make_shared<EntityPropertyDefinition>(
        name,
        default_value,
        description,
        type);
}
