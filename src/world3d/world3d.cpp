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


/**
 * Split a string on ';' characters.
 * TODO: temp
 */
auto split_wadpaths(std::string const &str)
{
    std::vector<std::string> wad_paths{};
    for (size_t i = 0, j = 0; j != std::string::npos; i = j + 1)
    {
        j = str.find(';', i);
        auto const n = (j == std::string::npos? j : j - i);
        wad_paths.emplace_back(str.substr(i, n));
    }
    return wad_paths;
}



World3D::World3D::World3D(Sickle::Editor::Map &src)
{
    Sickle::Editor::Entity const *worldspawn{nullptr};
    for (auto const &entity : src.entities)
        if (entity.properties.at("classname") == "worldspawn")
            worldspawn = &entity;
    if (!worldspawn)
        return;

    // TODO: WAD paths will be configured in-editor, map WAD info should be
    // ignored
    auto paths = split_wadpaths(worldspawn->properties.at("wad"));
    for (auto const &path : paths)
        texman.add_wad(WAD::load(path));

    for (auto &entity : src.entities)
        entities.emplace_back(*this, entity);
}


void World3D::World3D::render() const
{
    for (auto const &entity : entities)
        entity.render();
}
