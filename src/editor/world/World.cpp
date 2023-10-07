/**
 * World.cpp - Editor::World.
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

#include "world/World.hpp"

using namespace Sickle::Editor;


Glib::RefPtr<World> World::create()
{
    return Glib::RefPtr{new World{}};
}


Glib::RefPtr<World> World::create(MAP::Map const &map)
{
    auto world = World::create();
    for (auto const &entity : map.entities)
        world->add_entity(entity);
    return world;
}


Glib::RefPtr<World> World::create(RMF::RichMap const &map)
{
    auto world = World::create();

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
            worldspawn.add_brush(Brush::create(brush));
        for (auto const &entity : group.entities)
            world->add_entity(entity);
        for (auto group2 : group.groups)
            groups.push(group2);
    }
    world->add_entity(worldspawn);

    return world;
}


World &World::operator=(World const &other)
{
    if (this != &other)
    {
        _entities = other._entities;
    }
    return *this;
}


World::operator MAP::Map() const
{
    MAP::Map out{};
    for (auto const &entity : _entities)
        out.entities.push_back(entity);
    return out;
}


Entity &World::add_entity(Entity const &entity)
{
    auto &e = _entities.emplace_back(entity);
    return e;
}


void World::remove_brush(BrushRef const &brush)
{
    for (auto &entity : _entities)
        entity.remove_brush(brush);
}


Entity &World::worldspawn()
{
    for (auto &entity : _entities)
        if (entity.properties.at("classname") == "worldspawn")
            return entity;
    throw std::logic_error{"missing worldspawn"};
}



World::World()
:   Glib::ObjectBase{typeid(World)}
{
    auto &worldspawn = add_entity({});
    worldspawn.properties["classname"] = "worldspawn";
}
