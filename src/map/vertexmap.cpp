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

#include "map/vertexmap.hpp"
#include "../convexhull/convexhull.hpp"

#include <glm/glm.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/epsilon.hpp>

#include <set>


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
    :   MathPlane{
        {p.a[0], p.a[1], p.a[2]},
        {p.b[0], p.b[1], p.b[2]},
        {p.c[0], p.c[1], p.c[2]}}
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

/** Implements std::less interface to sort glm::vec3s counterclockwise. */
struct VectorLessCounterClockwise
{
    // Precalculated center of points to be compared.
    glm::vec3 const center;
    // Plane to compare in.
    MathPlane const plane;
    // Plane's S and T axes.
    glm::vec3 const s_axisN, t_axisN;

    VectorLessCounterClockwise(glm::vec3 const &center, MathPlane const &plane)
    :   center{center}
    ,   plane{plane}
    ,   s_axisN{glm::normalize(plane.points[1] - plane.points[0])}
    ,   t_axisN{glm::normalize(glm::cross(s_axisN, plane.normal))}
    {
    }
    /** Sorts vertices counterclockwise. */
    bool operator()(glm::vec3 const &a, glm::vec3 const &b) const
    {
        // Local space A and B vectors.
        auto const a_local = a - center;
        auto const b_local = b - center;

        // Vertices projected onto the plane.
        glm::vec2 const a_proj{
            glm::dot(a_local, s_axisN), glm::dot(a_local, t_axisN)};
        glm::vec2 const b_proj{
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


/** Sort vertices counterclockwise. */
auto _sort_vertices_counterclockwise(
    std::unordered_set<glm::vec3> const &vertices, MathPlane const &plane)
{
    glm::vec3 center{0.0f};
    for (auto const &vertex : vertices)
        center += vertex;
    center /= vertices.size();
    std::set<glm::vec3, VectorLessCounterClockwise> sorted{
        vertices.cbegin(), vertices.cend(),
        VectorLessCounterClockwise{center, plane}};
    return sorted;
}

/** Convert from half-plane Brush to vertex Brush. */
MAP::V::Brush brush_h_to_v(MAP::Brush const &hbrush)
{
    MAP::V::Brush vbrush{};

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
    for (auto const &plane : hbrush.planes)
    {
        decltype(MAP::V::Face::vertices) face_verts{};
        MathPlane const mp{plane};
        for (auto const &vertex : vertices)
            if (mp.containsPoint(vertex))
                face_verts.push_back({vertex.x, vertex.y, vertex.z});

        MAP::V::Face face{
            face_verts,
            plane.miptex,
            {plane.s_vector[0], plane.s_vector[1], plane.s_vector[2]},
            {plane.t_vector[0], plane.t_vector[1], plane.t_vector[2]},
            {plane.s_vector[3], plane.t_vector[3]},
            plane.rotation,
            {plane.scalex, plane.scaley}
        };

        // TODO: clean up
        std::unordered_set<glm::vec3> vertices_us{};
        for (auto const &v : face.vertices)
            vertices_us.emplace(v[0], v[1], v[2]);

        auto const vertices_sorted =\
            _sort_vertices_counterclockwise(vertices_us, mp);

        face.vertices.clear();
        for (auto const &v : vertices_sorted)
            face.vertices.push_back({v.x, v.y, v.z});

        vbrush.faces.push_back(face);
    }

    return vbrush;
}


MAP::V::VertexMap MAP::V::VertexMap::from_planes_map(
    MAP::Map const &map)
{
    VertexMap vm{};

    for (auto const &ent : map.entities)
    {
        auto &e = vm.entities.emplace_back();
        e.properties = ent.properties;
        for (auto const &brush : ent.brushes)
            e.brushes.emplace_back(brush_h_to_v(brush));
    }

    return vm;
}
