/**
 * World.cpp - OpenGL Editor::World view.
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

#include "world3d/World.hpp"


World3D::World::World(Sickle::Editor::WorldRef src)
{
    src->signal_child_added().connect(
        [this](auto child){
            auto entity = Sickle::Editor::EntityRef::cast_dynamic(child);
            if (entity)
                add_entity(entity);
        });

    for (auto &entity : src->entities())
        add_entity(entity);
}


void World3D::World::render() const
{
    for (auto const &entity : entities())
        entity->render();
}



void World3D::World::add_entity(Sickle::Editor::EntityRef const &entity)
{
    auto e = std::make_shared<Entity>(entity);
    _entities.push_back(e);
    entity->signal_removed().connect([this, e](){remove_entity(e);});
}


void World3D::World::remove_entity(std::shared_ptr<Entity> const &entity)
{
    auto it = std::find(_entities.cbegin(), _entities.cend(), entity);
    _entities.erase(it);
}
