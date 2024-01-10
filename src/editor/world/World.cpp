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


WorldRef World::create()
{
    return Glib::RefPtr{new World{}};
}


WorldRef World::create(MAP::Map const &map)
{
    auto world = World::create();
    for (auto const &entity : map.entities)
    {
        if (entity.properties.at("classname") == "worldspawn")
        {
            auto worldspawn = world->worldspawn();
            for (auto const &kv : entity.properties)
                worldspawn->set_property(kv.first, kv.second);
            for (auto const &b : entity.brushes)
                worldspawn->add_brush(Brush::create(b));
        }
        else
            world->add_entity(Entity::create(entity));
    }
    return world;
}


WorldRef World::create(RMF::RichMap const &map)
{
    auto world = World::create();

    auto worldspawn = world->worldspawn();
    for (auto const &kv : map.worldspawn_properties)
        worldspawn->set_property(kv.first, kv.second);
    worldspawn->set_property("classname", map.worldspawn_name);

    std::stack<RMF::Group> groups{};
    groups.push(map.objects);
    while (!groups.empty())
    {
        auto group = groups.top();
        groups.pop();
        for (auto const &brush : group.brushes)
            worldspawn->add_brush(Brush::create(brush));
        for (auto const &entity : group.entities)
            world->add_entity(Entity::create(entity));
        for (auto group2 : group.groups)
            groups.push(group2);
    }

    return world;
}


World::World()
:   Glib::ObjectBase{typeid(World)}
{
    _add_worldspawn();
}


World::operator MAP::Map() const
{
    MAP::Map out{};
    for (auto const &entity : _entities)
        out.entities.push_back(*entity.get());
    return out;
}


void World::add_entity(EntityRef const &entity)
{
    _entities.push_back(entity);
    signal_child_added().emit(entity);
}


void World::remove_entity(EntityRef const &entity)
{
    auto const it = std::find(_entities.cbegin(), _entities.cend(), entity);
    if (it == _entities.cend())
        return;
    _entities.erase(it);
    signal_child_removed().emit(entity);
}


void World::remove_brush(BrushRef const &brush)
{
    for (auto &entity : _entities)
        entity->remove_brush(brush);
}


EntityRef World::worldspawn()
{
    for (auto &entity : _entities)
        if (entity->get_property("classname") == "worldspawn")
            return entity;
    throw std::logic_error{"missing worldspawn"};
}


/* ---[ EditorObject interface ]--- */
Glib::ustring World::name() const
{
    std::stringstream ss{};
    ss << "World " << this;
    return Glib::ustring{ss.str()};
}


Glib::RefPtr<Gdk::Pixbuf> World::icon() const
{
    return Gdk::Pixbuf::create_from_data(
        NULL, Gdk::Colorspace::COLORSPACE_RGB, false, 8, 0, 0, 0);
}


std::vector<EditorObjectRef> World::children() const
{
    std::vector<EditorObjectRef> out{};
    for (auto const &entity : entities())
        out.push_back(entity);
    return out;
}



void World::_add_worldspawn()
{
    auto worldspawn = Entity::create();
    worldspawn->set_property("classname", "worldspawn");
    worldspawn->signal_removed().connect(
        sigc::mem_fun(*this, &World::_add_worldspawn));
    add_entity(worldspawn);
}
