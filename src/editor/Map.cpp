/**
 * Map.cpp - Editor::Map.
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

using namespace Sickle::Editor;


Map::Map()
{
    auto &worldspawn = entities.emplace_back();
    worldspawn.properties["classname"] = "worldspawn";
}

Map::Map(MAP::Map const &map)
{
    for (auto const &entity : map.entities)
        entities.push_back(entity);
}


Map::Map(RMF::RichMap const &map)
{
    Entity worldspawn{};
    worldspawn.properties = map.worldspawn_properties;
    worldspawn.properties["classname"] = map.worldspawn_name;

    std::stack<RMF::Group> groups{};
    groups.push(map.objects);
    while (!groups.empty())
    {
        auto group = groups.top();
        groups.pop();
        for (auto const &brush : group.brushes)
            worldspawn.brushes.emplace_back(std::make_shared<Brush>(brush));
        for (auto const &entity : group.entities)
            entities.push_back(entity);
        for (auto group2 : group.groups)
            groups.push(group2);
    }
    entities.emplace_back(worldspawn);
}


Map::operator MAP::Map() const
{
    MAP::Map out{};
    for (auto const &entity : entities)
        out.entities.push_back(entity);
    return out;
}
