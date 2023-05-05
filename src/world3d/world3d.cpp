/**
 * world3d.cpp - OpenGL Editor::World view.
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

#include "world3d/world3d.hpp"

#include <wad/lumps.hpp>


World3D::World3D::World3D(
    Sickle::Editor::Map &src,
    std::vector<std::string> const &wads)
{
    Sickle::Editor::Entity const *worldspawn{nullptr};
    for (auto const &entity : src.entities)
        if (entity.properties.at("classname") == "worldspawn")
            worldspawn = &entity;
    if (!worldspawn)
        return;

    for (auto const &path : wads)
        texman.add_wad(WAD::load(path));

    for (auto &entity : src.entities)
        entities.emplace_back(*this, entity);
}


void World3D::World3D::render() const
{
    for (auto const &entity : entities)
        entity.render();
}
