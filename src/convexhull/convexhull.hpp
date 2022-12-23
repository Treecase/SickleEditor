/**
 * convexhull.hpp - Vertex and Facet enumeration algorithms.
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

#ifndef _CONVEXHULL_HPP
#define _CONVEXHULL_HPP

#include <glm/glm.hpp>

#include <unordered_set>
#include <vector>


// FIXME: TEMP
/** std::hash template so glm::vec3 can be used in std::unordered_set. */
template<>
struct std::hash<glm::vec3>
{
    std::size_t operator()(glm::vec3 const &vec) const noexcept
    {
        std::hash<float> const hashf{};
        return hashf(vec.x) ^ hashf(vec.y) ^ hashf(vec.z);
    }
};


/** 3D half-space. */
class HalfPlane
{
public:
    /** Coefficients for general form plane equation (ax + by + cz + d = 0). */
    double a, b, c, d;

    /**
     * Solve for point P on plane. Result will be ~0 if point lies on the
     * plane.
     */
    double solveForPoint(glm::vec3 const &p) const;
};


/**
 * Vertex enumeration. Given a list of half-planes comprising a convex
 * polyhedron, return the vertices of said polyhedron.
 */
std::unordered_set<glm::vec3> vertex_enumeration(
    std::vector<HalfPlane> const &facets);

/**
 * Facet enumeration. Given a list of vertices comprising a convex polyhedron,
 * return a list of half-planes making up said polyhedron.
 */
std::vector<HalfPlane> facet_enumeration(
    std::vector<glm::vec3> const &vertices);

#endif
