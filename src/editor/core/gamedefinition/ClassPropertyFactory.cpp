/**
 * ClassPropertyFactory.cpp - Constructs ClassProperty objects.
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

#include "ClassProperties.hpp"

using namespace Sickle::Editor;

std::shared_ptr<ClassProperty> ClassPropertyFactory::construct(
    std::shared_ptr<FGD::Attribute> const &prop)
{
    if (!prop)
        ;
    else if (typeid(*prop.get()) == typeid(FGD::ColorAttribute))
    {
        auto const color = std::dynamic_pointer_cast<FGD::ColorAttribute>(prop);
        glm::vec3 const rgb{
            std::get<0>(color->rgb) / 255.0f,
            std::get<1>(color->rgb) / 255.0f,
            std::get<2>(color->rgb) / 255.0f,
        };
        return std::make_shared<ClassPropertyColor>(rgb);
    }
    else if (typeid(*prop.get()) == typeid(FGD::IconSpriteAttribute))
    {
        auto const iconsprite
            = std::dynamic_pointer_cast<FGD::IconSpriteAttribute>(prop);
        return std::make_shared<ClassPropertyIconsprite>(iconsprite->iconpath);
    }
    else if (typeid(*prop.get()) == typeid(FGD::SizeAttribute))
    {
        auto size = std::dynamic_pointer_cast<FGD::SizeAttribute>(prop);
        glm::vec3 p1{
            std::get<0>(size->bbox1),
            std::get<1>(size->bbox1),
            std::get<2>(size->bbox1)};
        glm::vec3 p2{};
        if (size->bbox2.has_value())
        {
            p2 = glm::vec3{
                std::get<0>(size->bbox2.value()),
                std::get<1>(size->bbox2.value()),
                std::get<2>(size->bbox2.value())};
        }
        else
        {
            p2 = p1 * glm::vec3{+0.5f, +0.5, +0.5};
            p1 = p1 * glm::vec3{-0.5f, -0.5, -0.5};
        }
        return std::make_shared<ClassPropertySize>(p1, p2);
    }
    else if (typeid(*prop.get()) == typeid(FGD::SpriteAttribute))
    {
        return std::make_shared<ClassPropertySprite>();
    }
    return nullptr;
}
