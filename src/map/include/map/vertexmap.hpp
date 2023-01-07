/**
 * vertexmap.hpp - Vertex representation of a .map.
 * Copyright (C) 2022 Trevor Last
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

#ifndef SE_VERTEXMAP_HPP
#define SE_VERTEXMAP_HPP

#include "map/map.hpp"

#include <array>
#include <string>
#include <unordered_map>
#include <vector>


namespace MAP
{
    namespace V
    {
        struct Face
        {
            // Vertices are sorted counterclockwise
            std::vector<std::array<float, 3>> vertices;
            std::string miptex;
            std::array<float, 3> s, t;
            std::array<float, 2> offsets;
            float rotation;
            std::array<float, 2> scale;
        };

        // TODO: Do this properly instead of copying everything just to change
        // the Brush list's contained type from Plane to Face. Will also let me
        // get rid of stupid "V" namespace
        struct Brush
        {
            std::vector<Face> faces;
        };

        struct Entity
        {
            std::unordered_map<std::string, std::string> properties;
            std::vector<Brush> brushes;
        };

        struct VertexMap
        {
            std::vector<Entity> entities;
            static VertexMap from_planes_map(MAP::Map const &map);
        };
    };
};

#endif
