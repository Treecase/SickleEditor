/**
 * ColliderFactory.cpp - Factory which constructs Colliders for world objects.
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

#include "ColliderFactory.hpp"
#include "BoxColliderBrush.hpp"
#include "BoxColliderPointEntity.hpp"

#include <editor/world/EditorWorld.hpp>

#include <sstream>

using namespace World3D;
using namespace Sickle::Editor;

std::shared_ptr<Collider> ColliderFactory::construct(
    EditorObjectRef const &object)
{
    std::shared_ptr<Collider> collider{nullptr};
    if (!object)
        ;
    else if (typeid(*object.get()) == typeid(Brush))
    {
        collider = std::make_shared<BoxColliderBrush>();
    }
    else if (typeid(*object.get()) == typeid(Entity))
    {
        auto const entity = EntityRef::cast_dynamic(object);
        auto const entity_class = entity->classinfo();
        if (entity_class.type() == "PointClass")
        {
            collider = std::make_shared<BoxColliderPointEntity>();
        }
    }

    return collider;
}
