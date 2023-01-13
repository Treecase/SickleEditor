/**
 * load_map.hpp - .map format data.
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

#ifndef SE_MAP_HPP
#define SE_MAP_HPP

#include <glm/glm.hpp>

#include <array>
#include <string>
#include <unordered_map>
#include <vector>


namespace MAP
{
    typedef glm::vec3 Vertex;
    typedef glm::vec3 Vector3;
    typedef glm::vec2 Vector2;

    /** Half-spaces used to define a brush. */
    struct Plane
    {
        // 3 points which define the plane
        Vertex a, b, c;
        // Vertices are sorted counterclockwise
        std::vector<Vertex> vertices;
        // Name of the texture to paste on the plane
        std::string miptex;
        // Texture application vars
        Vector3 s, t;
        Vector2 offsets;
        float rotation;
        Vector2 scale;
    };

    /** A convex polyhedron. */
    struct Brush
    {
        // Half-spaces comprising the brush. Minimum size of 4.
        std::vector<Plane> planes;
    };

    /** Map entity. BRUSHES can be empty if this is a PointClass entity. */
    struct Entity
    {
        std::unordered_map<std::string, std::string> properties;
        std::vector<Brush> brushes;
    };

    /** A map is just a collection of entities. */
    struct Map
    {
        std::vector<Entity> entities;
    };


    /** Parse a .map file. */
    Map load(std::string const &path);
}

#endif
