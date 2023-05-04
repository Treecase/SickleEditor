/**
 * Entity.cpp - Editor::Entity.
 * Copyright (C) 2023 Trevor Last
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

#include "editor/EditorWorld.hpp"



Sickle::Editor::Entity::Entity(MAP::Entity const &entity)
:   properties{entity.properties}
{
    for (auto const &brush : entity.brushes)
        brushes.emplace_back(std::make_shared<Brush>(brush));
}


Sickle::Editor::Entity::Entity(RMF::Entity const &entity)
:   properties{entity.kv_pairs}
{
    properties["classname"] = entity.classname;
    for (auto const &brush : entity.brushes)
        brushes.emplace_back(std::make_shared<Brush>(brush));
}


Sickle::Editor::Entity::operator MAP::Entity() const
{
    MAP::Entity out{};
    out.properties = properties;
    for (auto const &brush : brushes)
        out.brushes.push_back(*brush);
    return out;
}
