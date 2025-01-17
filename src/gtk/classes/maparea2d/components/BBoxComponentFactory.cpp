/**
 * BBoxComponentFactory.cpp - Factory to construct appropriate BBoxComponents
 *                            for Sickle world objects.
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

#include "BBoxComponentFactory.hpp"
#include "BrushBBox.hpp"
#include "EntityBBox.hpp"

using namespace World2D;

std::shared_ptr<BBoxComponent> BBoxComponentFactory::construct(
    Sickle::Editor::EditorObjectRef const &obj)
{
    std::shared_ptr<BBoxComponent> dc{nullptr};

    if (!obj)
        ;
    else if (typeid(*obj.get()) == typeid(Sickle::Editor::Brush))
    {
        dc = std::make_shared<BrushBBox>();
    }
    else if (typeid(*obj.get()) == typeid(Sickle::Editor::Entity))
    {
        auto const entity = Sickle::Editor::EntityRef::cast_dynamic(obj);
        if (entity->classinfo().type() == "PointClass")
        {
            dc = std::make_shared<EntityBBox>();
        }
    }

    return dc;
}
