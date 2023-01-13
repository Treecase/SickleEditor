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

#include "map/map.hpp"
#include "../convexhull/convexhull.hpp"

#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/epsilon.hpp>

#include <algorithm>
#include <array>
#include <set>
#include <vector>


/** An abstracted plane. */
struct MathPlane
{
    // 3 points used to define the plane.
    std::array<glm::vec3, 3> points;
    // Plane normal.
    glm::vec3 normal;
    // Coefficients for the general form plane equation (ax + by + cz + d = 0).
    float a;
    float b;
    float c;
    float d;

    MathPlane(glm::vec3 const &a, glm::vec3 const &b, glm::vec3 const &c)
    :   points{a, b, c}
    ,   normal{glm::normalize(glm::cross(c - a, b - a))}
    ,   a{normal.x}
    ,   b{normal.y}
    ,   c{normal.z}
    ,   d{-normal.x*a.x - normal.y*a.y - normal.z*a.z}
    {
    }
    MathPlane(MAP::Plane const &p)
    :   MathPlane{p.a, p.b, p.c}
    {
    }

    /** Check if `point` lies on the plane. */
    bool containsPoint(glm::vec3 const &point) const
    {
        static constexpr float EPSILON = 0.001f;
        auto v = a*point.x + b*point.y + c*point.z + d;
        return glm::epsilonEqual(v, 0.0f, EPSILON);
    }
};

/** Implements std::less interface to sort MAP::Vertex counterclockwise. */
struct VectorLessCounterClockwise
{
    // Precalculated center of points to be compared.
    MAP::Vertex const center;
    // Plane to compare in.
    MathPlane const plane;
    // Plane's S and T axes.
    MAP::Vector3 const s_axisN, t_axisN;

    VectorLessCounterClockwise(
        MAP::Vertex const &center, MathPlane const &plane)
    :   center{center}
    ,   plane{plane}
    ,   s_axisN{glm::normalize(plane.points[1] - plane.points[0])}
    ,   t_axisN{glm::normalize(glm::cross(s_axisN, plane.normal))}
    {
    }
    /** Sorts vertices counterclockwise. */
    bool operator()(MAP::Vertex const &a, MAP::Vertex const &b) const
    {
        // Local space A and B vectors.
        auto const a_local = a - center;
        auto const b_local = b - center;

        // Vertices projected onto the plane.
        MAP::Vector2 const a_proj{
            glm::dot(a_local, s_axisN), glm::dot(a_local, t_axisN)};
        MAP::Vector2 const b_proj{
            glm::dot(b_local, s_axisN), glm::dot(b_local, t_axisN)};

        // Angle between the 0 vector and point
        auto a_theta = glm::atan(a_proj.y, a_proj.x);
        auto b_theta = glm::atan(b_proj.y, b_proj.x);

        // Don't want negative angles. (Doesn't really matter, but whatever.)
        if (a_theta < 0.0f) a_theta += glm::radians(360.0);
        if (b_theta < 0.0f) b_theta += glm::radians(360.0);

        // If the angles aren't equal, compare them.
        // If the angles are the same, use distance from center as tiebreaker.
        if (glm::epsilonNotEqual(a_theta, b_theta, glm::epsilon<float>()))
            return -a_theta < -b_theta;
        else
            return glm::length(a_proj) < glm::length(b_proj);
    }
};

/** Must be an iterator over glm::vec3s. */
template<class RandomIt>
auto _find_polyhedron_center(RandomIt first, RandomIt last)
{
    MAP::Vertex center{0.0f};
    MAP::Vertex::value_type i = 0;
    for (; first != last; ++first, ++i)
        center += *first;
    return center / i;
}


void brush_add_vertices(MAP::Brush &hbrush)
{
    // Get brush vertices from planes.
    std::vector<HalfPlane> halfplanes{};
    for (auto const &plane : hbrush.planes)
    {
        MathPlane const mp{plane};
        // TODO: figure out why these need to be negated
        halfplanes.push_back({-mp.a, -mp.b, -mp.c, -mp.d});
    }
    auto const vertices = vertex_enumeration(halfplanes);

    // Build faces by finding all the vertices that lie on each plane.
    for (auto &plane : hbrush.planes)
    {
        MathPlane const mp{plane};
        std::copy_if(
            vertices.cbegin(), vertices.cend(),
            std::back_inserter(plane.vertices),
            [&mp](auto v){return mp.containsPoint(v);});
        // Sort vertices counterclockwise
        std::sort(
            plane.vertices.begin(), plane.vertices.end(),
            VectorLessCounterClockwise{
                _find_polyhedron_center(
                    plane.vertices.cbegin(), plane.vertices.cend()),
                plane
            });
    }
}
