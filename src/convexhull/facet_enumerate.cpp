/**
 * facet_enumerate.cpp - Facet enumeration algorithm.
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

#include "convexhull/convexhull.hpp"

#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/epsilon.hpp>

#include <algorithm>
#include <unordered_map>

#if 1
#include <iostream>
std::ostream &operator<<(std::ostream &os, glm::vec3 v)
{
    return os << v.x << ' ' << v.y << ' ' << v.z;
}
#endif


/* ===[ Facet Enumeration Utils ]=== */
struct Facet;

struct Edge
{
    glm::vec3 first, second;
    Facet *forward{nullptr}, *reverse{nullptr};

    Edge(glm::vec3 first, glm::vec3 second)
    :   first{first}
    ,   second{second}
    {
    }
};

struct Facet : public HalfPlane
{
    std::vector<Edge> edges;

    Facet(glm::vec3 a, glm::vec3 b, glm::vec3 c)
    :   HalfPlane{a, b, c}
    ,   edges{{a, b}, {b, c}, {c, a}}
    {
        for (auto &edge : edges)
            edge.forward = this;
    }

    Facet(glm::vec3 p, Edge &edge)
    :   HalfPlane{p, edge.second, edge.first}
    ,   edges{{p, edge.second}, edge, {edge.first, p}}
    {
        edges[0].forward = this;
        edge.reverse = this;
        edges[2].forward = this;
    }

    auto extend(glm::vec3 p)
    {
        std::vector<Facet> added{};
        for (auto &edge : edges)
        {
            auto f = added.emplace_back(p, edge.second, edge.first);
            edge.reverse = &f;
        }
        return added;
    }

    bool operator==(Facet const &o) const
    {
        return glm::all(
            glm::epsilonEqual(
                glm::vec4{a, b, c, d},
                glm::vec4{o.a, o.b, o.c, o.d},
                EPSILON));
    }
};

std::ostream &operator<<(std::ostream &os, Facet const &facet)
{
    return os << "Facet(" << facet.normal() << ')';
}

template<>
struct std::hash<Facet>
{
    size_t operator()(Facet const &f) const
    {
        std::hash<float> const h{};
        return h(f.a) ^ h(f.b) ^ h(f.c) ^ h(f.d);
    }
};

/** Distance from X0 to the line between X1 and X2. */
auto distance_to_line(glm::vec3 x0, glm::vec3 x1, glm::vec3 x2)
{
    return glm::length(glm::cross(x0 - x1, x0 - x2)) / glm::length(x2 - x1);
}

/** Find the minimum and maximum points in a given dimension. */
std::pair<glm::vec3, glm::vec3>
minmax(size_t dimension, std::vector<glm::vec3> const &points)
{
    assert(!points.empty());
    auto min = points.at(0), max = points.at(0);
    for (auto const &p : points)
    {
        if (p[dimension] < min[dimension]) min = p;
        if (p[dimension] > max[dimension]) max = p;
    }
    return {min, max};
}

/** Create a tetrahedron from given points. */
auto create_tetrahedron(std::vector<glm::vec3> const &vertices)
{
    // Get distinct min/max points for a cardinal axis.
    auto m = minmax(0, vertices);
    for (size_t d = 1; m.first == m.second; ++d)
    {
        if (d > 3)
            throw std::runtime_error{
                "create_tetrahedron degenerate case 1D/2D"};
        m = minmax(d, vertices);
    }
    auto const min = m.first, max = m.second;
    std::cout << "min/max: " << min << '/' << max << '\n';

    // Find point furthest from the line between the min and max.
    auto const linedist =\
        [&min, &max](auto x){return distance_to_line(x, min, max);};
    auto const farL = *std::max_element(
        vertices.cbegin(), vertices.cend(),
        [&linedist](auto a, auto b){return linedist(a) < linedist(b);});
    if (glm::epsilonEqual(linedist(farL), 0.0f, HalfPlane::EPSILON))
        throw std::runtime_error{"create_tetrahedron degenerate case 1D"};

    // Find point furthest from the plane formed by prior 3 points.
    Facet const plane{min, max, farL};
    auto const farP = *std::max_element(
        vertices.cbegin(), vertices.cend(),
        [&plane](auto a, auto b){
            return abs(plane.distanceTo(a)) < abs(plane.distanceTo(b));
        });
    auto const farD = plane.classify(farP);
    if (farD == ON)
        throw std::runtime_error{"create_tetrahedron degenerate case 2D"};
    std::cout << "d: " << farD << '\n';

    // Create the tetrahedron. FARD's sign tells us clockwise vertex ordering.
    auto const A = min;
    auto const B = max;
    auto const C = farL;
    auto const D = farP;
    std::cout << "ABCD: " << A << '|' << B << '|' << C << '|' << D << '\n';
    if (farD == BELOW)
    {
        Facet base{A, B, C};
        base.extend(D);
        Facet f2{D, B, A};
        Facet f3{D, C, B};
        Facet f4{D, A, C};
        return std::unordered_set{base};
    }
    else
    {
        Facet base{C, B, A};
        Facet f2{A, B, D};
        Facet f3{B, C, D};
        Facet f4{A, D, C};
        return std::unordered_set{base};
    }
}

/** Find the points outside a convex hull. */
auto get_outer_points(
    std::unordered_set<Facet> const &hull, std::vector<glm::vec3> const &points)
{
    std::vector<glm::vec3> outside{};
    std::copy_if(
        points.cbegin(), points.cend(),
        std::back_inserter(outside),
        [&hull](auto p){
            return std::all_of(
                hull.cbegin(), hull.cend(),
                [&p](auto plane){return plane.classify(p) == ABOVE;});
        }
    );
    return outside;
}

/**
 * Maps points to visible facets. Each point is assigned to a single facet,
 * even if multiple are visible. A facet is visible to a point if the point is
 * above the facet.
 */
auto get_conflict_lists(
    std::unordered_set<Facet> const &hull, std::vector<glm::vec3> const &points)
{
    std::unordered_map<Facet, std::vector<glm::vec3>> conflicts{};
    for (auto const &point : points)
        for (auto const &plane : hull)
            if (plane.classify(point) == ABOVE)
                conflicts[plane].push_back(point);
    return conflicts;
}

auto get_horizon(
    glm::vec3 const &eye, Facet const &start,
    std::unordered_set<Facet> const &facets)
{
    std::unordered_map<Facet const *, std::vector<Facet const *>> neighbors{};
    for (auto const &facet : facets)
        neighbors[&facet] = {};

    for (auto const &facet : facets)
    {
        for (auto const &other : facets)
        {
            if (other == facet) continue;

            for (auto const &edge1 : facet.edges)
                ;
            for (auto const &edge2 : other.edges)
                ;
        }
    }

    std::unordered_set<Facet> out{};
    return out;
}


std::vector<HalfPlane> facet_enumeration(std::vector<glm::vec3> const &vertices)
{
    // QuickHull (from http://algolist.ru/maths/geom/convhull/qhull3d.php)
    // https://ubm-twvideo01.s3.amazonaws.com/o1/vault/GDC2014/Presentations/Gregorius_Dirk_Physics_for_Game_01.pdf

    assert(vertices.size() >= 4);

    // Calculate starting tetrahedral hull.
    auto convex_hull = create_tetrahedron(vertices);
    assert(convex_hull.size() == 4);

    // Find points outside the hull.
    auto outer_points = get_outer_points(convex_hull, vertices);

    // Build conflict lists.
    auto conflict_lists = get_conflict_lists(convex_hull, outer_points);

    while (!conflict_lists.empty())
    {
        for (auto const &conflict : conflict_lists)
        {
            auto const &f = conflict.first;
            auto const &points = conflict.second;

            auto const furthest = *std::max_element(
                points.cbegin(), points.cend(),
                [&f](auto a, auto b){
                    return f.distanceTo(a) < f.distanceTo(b);});

            auto const horizon = get_horizon(furthest, f, convex_hull);
        }
        // convex_hull = {};
        // outer_points = get_outer_points(convex_hull, outer_points);
        // conflict_lists = get_conflict_lists(convex_hull, outer_points);
    }

    return {};
}

