/**
 * RenderComponentFactory.cpp - Factory which constructs RenderComponents for
 *                              world objects.
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

#include "RenderComponentFactory.hpp"
#include "Brush.hpp"
#include "Entity.hpp"

#include <editor/world/Brush.hpp>
#include <editor/world/Entity.hpp>

#include <iostream>

using namespace World3D;

std::shared_ptr<RenderComponent> RenderComponentFactory::construct(
    Sickle::Editor::EditorObjectRef const &object)
{
    std::shared_ptr<RenderComponent> renderer{nullptr};

    if (!object)
        ;
    else if (typeid(*object.get()) == typeid(Sickle::Editor::Brush))
    {
        renderer = std::make_shared<Brush>();
    }
    else if (typeid(*object.get()) == typeid(Sickle::Editor::Entity))
    {
        auto entity = Sickle::Editor::EntityRef::cast_dynamic(object);
        auto const entity_class = entity->classinfo();
        if (entity_class.type() == "PointClass")
        {
            if (entity_class.has_class_property<
                    Sickle::Editor::ClassPropertyIconsprite>())
            {
                renderer = std::make_shared<PointEntitySprite>();
            }
            else if (entity_class.has_class_property<
                         Sickle::Editor::ClassPropertySprite>())
            {
                renderer = std::make_shared<PointEntitySprite>();
            }
            else
            {
                renderer = std::make_shared<PointEntityBox>();
            }
        }
        else if (entity_class.type() == "SolidClass")
        {
            renderer = std::make_shared<SolidEntity>();
        }
        else
        {
            std::cout << "WARNING: entity has unknown class type '"
                      << entity_class.type() << "'\n";
        }
    }

    return renderer;
}
