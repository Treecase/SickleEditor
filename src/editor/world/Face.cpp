/**
 * Face.cpp - Editor::Face.
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

#include "world/Face.hpp"

#include <glm/gtc/epsilon.hpp>

#include <algorithm>

using namespace Sickle::Editor;


/* ===[ .map conversion utils ]=== */
/** Implements std::less interface to sort MAP::Vertex counterclockwise. */
class VectorLessCounterClockwise
{
    template<class Container>
    auto _find_polyhedron_center(Container const &points)
    {
        MAP::Vertex center{0.0f};
        for (auto const &point : points)
            center += point;
        return center / (MAP::Vertex::value_type)points.size();
    }

public:
    // Precalculated center of points to be compared.
    MAP::Vertex const center;
    // Plane's U and V axes.
    MAP::Vector3 const u_axisN, v_axisN;

    VectorLessCounterClockwise(
        HalfPlane const &plane, std::vector<glm::vec3> const &points)
    :   center{_find_polyhedron_center(points)}
    ,   u_axisN{glm::normalize(points.at(1) - points.at(0))}
    ,   v_axisN{glm::normalize(glm::cross(u_axisN, plane.normal()))}
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
            glm::dot(a_local, u_axisN), glm::dot(a_local, v_axisN)};
        MAP::Vector2 const b_proj{
            glm::dot(b_local, u_axisN), glm::dot(b_local, v_axisN)};

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



Face::Face(
    HalfPlane const &plane,
    std::vector<glm::vec3> const &brush_vertices)
:   Glib::ObjectBase{typeid(Face)}
,   vertices{}
,   texture{*this, "texture", "TODO"} // TODO
,   u{*this, "u", {1.0f, 0.0f, 0.0f}} // TODO
,   v{*this, "v", {0.0f, 1.0f, 0.0f}} // TODO
,   shift{*this, "shift", {0.0f, 0.0f}}
,   scale{*this, "scale", {1.0f, 1.0f}}
,   rotation{*this, "rotation", 0.0f}
{
    // Build Face by finding all the vertices that lie on each plane.
    std::copy_if(
        brush_vertices.cbegin(), brush_vertices.cend(),
        std::back_inserter(vertices),
        [&plane](auto v){return plane.isPointOnPlane(v);});
    assert(vertices.size() >= 3);
    std::sort(
        vertices.begin(), vertices.end(),
        VectorLessCounterClockwise{plane, vertices});
    if (vertices.size() < 3)
        throw std::runtime_error{"not enough points for a face"};
}


Face::Face(
    MAP::Plane const &plane,
    std::unordered_set<glm::vec3> const &brush_vertices)
:   Glib::ObjectBase{typeid(Face)}
,   vertices{}
,   texture{*this, "texture", plane.miptex}
,   u{*this, "u", plane.s}
,   v{*this, "v", plane.t}
,   shift{*this, "shift", plane.offsets}
,   scale{*this, "scale", plane.scale}
,   rotation{*this, "rotation", plane.rotation}
{
    // Build Face by finding all the vertices that lie on each plane.
    HalfPlane const mp{plane.a, plane.b, plane.c};
    assert(mp.isPointOnPlane(plane.a));
    assert(mp.isPointOnPlane(plane.b));
    assert(mp.isPointOnPlane(plane.c));
    std::copy_if(
        brush_vertices.cbegin(), brush_vertices.cend(),
        std::back_inserter(vertices),
        [&mp](auto v){return mp.isPointOnPlane(v);});
    assert(vertices.size() >= 3);
    std::sort(
        vertices.begin(), vertices.end(),
        VectorLessCounterClockwise{mp, vertices});

    if (vertices.size() < 3)
        throw std::runtime_error{"not enough points for a face"};
}


Face::Face(RMF::Face const &face)
:   Glib::ObjectBase{typeid(Face)}
,   vertices{}
,   texture{*this, "texture", face.texture_name}
,   u{*this, "u", {face.texture_u.x, face.texture_u.y, face.texture_u.z}}
,   v{*this, "v", {face.texture_v.x, face.texture_v.y, face.texture_v.z}}
,   shift{*this, "shift", {face.texture_x_shift, face.texture_y_shift}}
,   scale{*this, "scale", {face.texture_x_scale, face.texture_y_scale}}
,   rotation{*this, "rotation", face.texture_rotation}
{
    // RMF stores verts sorted clockwise. We need them counterclockwise.
    for (auto const &vert : face.vertices)
        vertices.emplace(vertices.begin(), vert.x, vert.y, vert.z);
    if (vertices.size() < 3)
        throw std::runtime_error{"not enough points for a face"};
}


Face::operator MAP::Plane() const
{
    auto abc = get_plane_points();
    return {
        abc[2], abc[1], abc[0],
        texture,
        u, v,
        shift,
        rotation,
        scale
    };
}


std::array<glm::vec3, 3> Face::get_plane_points() const
{
    return {vertices.at(0), vertices.at(1), vertices.at(2)};
}


void Face::set_vertex(size_t index, glm::vec3 vertex)
{
    vertices.at(index) = vertex;
    signal_vertices_changed().emit();
}
