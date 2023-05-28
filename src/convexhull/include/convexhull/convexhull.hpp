/**
 * convexhull.hpp - Vertex and Facet enumeration algorithms.
 * Copyright (C) 2022-2023 Trevor Last
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

#ifndef SE_CONVEXHULL_HPP
#define SE_CONVEXHULL_HPP

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


enum Classification
{
    ON, ABOVE, BELOW,
};

/** 3D half-space. */
class HalfPlane
{
public:
    static constexpr float EPSILON = 0.001f;

    /** Coefficients for general form plane equation (ax + by + cz + d = 0). */
    float a, b, c, d;

    HalfPlane(float a, float b, float c, float d);
    /// Points are clockwise ordered.
    HalfPlane(glm::vec3 A, glm::vec3 B, glm::vec3 C);

    /** Signed distance from P to the plane. */
    float distanceTo(glm::vec3 const &p) const;

    /** Classify a point as either above, below, or on the plane. */
    Classification classify(glm::vec3 const &point) const;

    /** Check if a point lies on the plane. */
    bool isPointOnPlane(glm::vec3 const &point) const;

    /** Get the plane's normal vector. */
    glm::vec3 normal() const;
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
