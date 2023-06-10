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
#include <array>
#include <map>
#include <memory>
#include <optional>
#include <unordered_map>

#if 1
#include <iostream>
std::ostream &operator<<(std::ostream &os, glm::vec3 v)
{
    return os << v.x << ' ' << v.y << ' ' << v.z;
}
#endif


/* ===[ Facet Enumeration Utils ]=== */
// We need to manage a list of edges between facets
// - Edges need to keep track of direction to maintain clockwise ordering

enum class EdgeCompare
{
    EQUAL,
    NOTEQUAL,
    OPPOSITE,
};

struct Edge
{
    glm::vec3 first, second;

    Edge(glm::vec3 first, glm::vec3 second)
    :   first{first}
    ,   second{second}
    {
    }

    EdgeCompare compare(Edge const &other) const
    {
        if (first == other.first && second == other.second)
            return EdgeCompare::EQUAL;
        else if (first == other.second && second == other.first)
            return EdgeCompare::OPPOSITE;
        else
            return EdgeCompare::NOTEQUAL;
    }

    bool operator==(Edge const &o) const
    {
        auto const c = compare(o);
        return c == EdgeCompare::EQUAL || c == EdgeCompare::OPPOSITE;
    }
};

template<>
struct std::hash<Edge>
{
    size_t operator()(Edge const &e) const
    {
        std::hash<glm::vec3> const h{};
        return h(e.first) ^ h(e.second);
    }
};


struct Facet : public HalfPlane
{
    // in clockwise order
    std::vector<Edge> edges;
    std::unordered_map<Edge, std::shared_ptr<Facet>> neighbors;

    Facet(glm::vec3 a, glm::vec3 b, glm::vec3 c)
    :   HalfPlane{a, b, c}
    ,   edges{{a, b}, {b, c}, {c, a}}
    {
        for (auto const &edge : edges)
            neighbors[edge] = nullptr;
    }

    bool checkIntegrity() const
    {
        for (auto const &edge : edges)
        {
            // each edge must be in neighbors, and no neighbor slot can be null
            assert(neighbors.count(edge) != 0);
            assert(neighbors.at(edge) != nullptr);

            // neighbor's slot must exist and point to this facet
            auto const &neighbor = neighbors.at(edge);
            Edge const opposite{edge.second, edge.first};
            assert(neighbor->neighbors.count(opposite) != 0);
            assert(neighbor->neighbors.at(opposite).get() == this);
        }
        return true;
    }

    bool operator==(Facet const &o) const
    {
        return glm::all(
            glm::epsilonEqual(
                glm::vec4{a, b, c, d},
                glm::vec4{o.a, o.b, o.c, o.d},
                EPSILON));
    }

    std::optional<std::pair<Edge, Edge>> checkNeighbor(Facet const &other)
    {
        for (auto const &edge1 : edges)
            for (auto const &edge2 : other.edges)
                if (edge1.compare(edge2) == EdgeCompare::OPPOSITE)
                    return std::make_pair(edge1, edge2);
        return std::nullopt;
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


struct ConvexHull
{
    std::unordered_set<std::shared_ptr<Facet>> facets{};

    void addFacet(Facet const &f)
    {
        auto p = facets.insert(std::make_shared<Facet>(f));
        if (!p.second)
            return;

        auto newf = *p.first;
        for (auto &facet : facets)
        {
            auto const edges = facet->checkNeighbor(*newf);
            if (edges.has_value())
            {
                facet->neighbors.at(edges.value().first) = newf;
                newf->neighbors.at(edges.value().second) = facet;
            }
        }
    }

    bool checkIntegrity() const
    {
        for (auto const &facet : facets)
        {
            assert(facet->checkIntegrity());
            // neighbors must be in the hull
            for (auto const &kv : facet->neighbors)
                assert(facets.count(kv.second) != 0);
        }
        return true;
    }
};


/** Distance from X0 to the line between X1 and X2. */
auto distance_to_line(glm::vec3 x0, glm::vec3 x1, glm::vec3 x2)
{
    return glm::length(glm::cross(x0 - x1, x0 - x2)) / glm::length(x2 - x1);
}

// Helper for create_tetrahedron
auto choose_minmax(std::vector<glm::vec3> const &vertices)
{
    using VerticesIter = std::vector<glm::vec3>::const_iterator;
    using MinMaxT = std::pair<VerticesIter, VerticesIter>;

    assert(!vertices.empty());

    std::array<size_t, 3> const axes{0, 1, 2};
    std::array<MinMaxT, 3> minmaxes;

    // Get min/max points for cardinal axes.
    auto const compareInAxis = [](auto d){
        return [d](auto a, auto b){return a[d] < b[d];};
    };
    auto const getAxisMinMax = [&vertices, &compareInAxis](auto d){
        return std::minmax_element(
            vertices.cbegin(), vertices.cend(), compareInAxis(d));
    };
    std::transform(axes.cbegin(), axes.cend(), minmaxes.begin(), getAxisMinMax);

    // Ensure we have distinct points in at least one dimension.
    auto const areEqual = [](auto e){return *e.first == *e.second;};
    if (std::all_of(minmaxes.cbegin(), minmaxes.cend(), areEqual))
        throw std::runtime_error{"create_tetrahedron degenerate case 1D/2D"};

    // Pick furthest apart min/max points.
    auto const minmax = std::max_element(
        minmaxes.cbegin(), minmaxes.cend(),
        [distance=[](auto e){return glm::length(*e.second - *e.first);}]
        (auto a, auto b){return distance(a) < distance(b);});

    return std::make_pair(*minmax->first, *minmax->second);
}

/** Create a tetrahedron from given points. */
auto create_tetrahedron(std::vector<glm::vec3> const &vertices)
{
    auto const minmax = choose_minmax(vertices);
    auto const min = minmax.first, max = minmax.second;
    std::cout << "min/max: " << min << '/' << max << '\n';

    // Find point furthest from the line between the min and max.
    auto const linedist =\
        [&min, &max](auto x){return distance_to_line(x, min, max);};
    auto const farL = *std::max_element(
        vertices.cbegin(), vertices.cend(),
        [&linedist](auto a, auto b){return linedist(a) < linedist(b);});
    // Degenerate case if this point is on the line.
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
    // Degenerate case if this point is on the plane.
    if (farD == ON)
        throw std::runtime_error{"create_tetrahedron degenerate case 2D"};
    std::cout << "d: " << farD << '\n';

    // Create the tetrahedron. FARD's sign tells us clockwise vertex ordering.
    auto const A = min;
    auto const B = max;
    auto const C = farL;
    auto const D = farP;
    std::cout << "ABCD: " << A << '|' << B << '|' << C << '|' << D << '\n';
    ConvexHull out{};
    if (farD == BELOW)
    {
        std::cout << "Below\n";
        out.addFacet(Facet{A, B, C});
        out.addFacet(Facet{D, B, A});
        out.addFacet(Facet{D, C, B});
        out.addFacet(Facet{D, A, C});
    }
    else
    {
        std::cout << "Above\n";
        out.addFacet(Facet{A, C, B});
        out.addFacet(Facet{D, A, B});
        out.addFacet(Facet{D, B, C});
        out.addFacet(Facet{D, C, A});
    }
    return out;
}

/** Find the points outside a convex hull. */
auto get_outer_points(
    ConvexHull const &hull, std::vector<glm::vec3> const &points)
{
    std::vector<glm::vec3> outside{};
    std::copy_if(
        points.cbegin(), points.cend(),
        std::back_inserter(outside),
        [&hull](auto p){
            return std::any_of(
                hull.facets.cbegin(), hull.facets.cend(),
                [&p](auto plane){return plane->classify(p) == ABOVE;});
        });
    return outside;
}

/**
 * Maps points to visible facets. Each point is assigned to a single facet,
 * even if multiple are visible. A facet is visible to a point if the point is
 * above the facet.
 */
auto get_conflict_lists(
    ConvexHull const &hull, std::vector<glm::vec3> const &points)
{
    std::unordered_map<
        std::shared_ptr<Facet>,
        std::vector<glm::vec3>> conflicts{};
    for (auto const &point : points)
    {
        for (auto const &facet : hull.facets)
        {
            if (facet->classify(point) == ABOVE)
            {
                conflicts[facet].push_back(point);
                break;
            }
        }
    }
    return conflicts;
}


void dfs2(
    glm::vec3 const &eye,
    std::shared_ptr<Facet> const &f,
    std::unordered_set<std::shared_ptr<Facet>> &visited,
    std::vector<Edge> &horizon)
{
    std::cout << "Visiting " << *f << '\n';
    visited.insert(f);

    for (auto const &edge : f->edges)
    {
        auto const &next = f->neighbors.at(edge);
        // Crossing this edge leads to a non-visible face, so this edge is on
        // the horizon.
        if (next->classify(eye) != ABOVE)
            horizon.push_back(edge);
        // If we haven't already visited the neighboring face, visit it.
        else if (visited.count(next) == 0)
            dfs2(eye, next, visited, horizon);
    }
}

auto dfs(glm::vec3 const &eye, std::shared_ptr<Facet> const &start)
{
    assert(start->classify(eye) == ABOVE);

    std::cout << ">>> start dfs\n";
    std::unordered_set<std::shared_ptr<Facet>> visited{};
    std::vector<Edge> horizon{};
    dfs2(eye, start, visited, horizon);
    std::cout << "<<< end dfs\n";
    return std::make_pair(horizon, visited);
}

/**
 * Find the list of edges on EYE's horizon.
 * A horizon edge is an edge between a visible facet and a non-visible facet.
 */
auto get_horizon(
    glm::vec3 const &eye,
    std::shared_ptr<Facet> const &start,
    ConvexHull const &facets)
{
    // run a depth-first search through the convexhull facets, using START as
    // the root
    auto const horizon_and_visible = dfs(eye, start);
    auto const horizon = horizon_and_visible.first;

    std::cout << "HORIZON(" << eye << ")\n";
    for (auto const &edge : horizon)
        std::cout << "  " << edge.first << " -> " << edge.second << '\n';

    // Check horizon is ordered clockwise
    for (size_t i = 0; i < horizon.size() - 1; ++i)
        assert(horizon.at(i).second == horizon.at(i + 1).first);
    assert(horizon.back().second == horizon.front().first);

    return horizon_and_visible;
}


/**
 * Returns a list of HalfPlanes making up the convex hull, and the vertices
 * that contribute to that hull.
 */
std::pair<std::vector<HalfPlane>, std::vector<glm::vec3>>
facet_enumeration(std::vector<glm::vec3> const &vertices)
{
    // QuickHull (from http://algolist.ru/maths/geom/convhull/qhull3d.php)
    // https://ubm-twvideo01.s3.amazonaws.com/o1/vault/GDC2014/Presentations/Gregorius_Dirk_Physics_for_Game_01.pdf

    assert(vertices.size() >= 4);

    // Calculate starting tetrahedral hull.
    auto convex_hull = create_tetrahedron(vertices);
    assert(convex_hull.facets.size() == 4);
    assert(convex_hull.checkIntegrity());

    // Find points outside the hull.
    auto outer_points = get_outer_points(convex_hull, vertices);

    // Build conflict lists.
    auto conflict_lists = get_conflict_lists(convex_hull, outer_points);

{//TEMP
    std::cout << "HULL\n";
    for (auto const &f : convex_hull.facets)
        std::cout << "  " << *f << '\n';

    std::cout << "OUTSIDE\n";
    for (auto const &p : outer_points)
        std::cout << "  " << p << '\n';

    std::cout << "CONFLICTS\n";
    for (auto const &conflict : conflict_lists)
    {
        std::cout << "  " << conflict.first->normal() << " |";
        for (auto const &p : conflict.second)
            std::cout << " [" << p << ']';
        std::cout << '\n';
    }
}//TEMP

    while (!conflict_lists.empty())
    {
        // Find furthest conflicting point.
        auto far = std::make_pair(
            conflict_lists.cbegin()->first,
            conflict_lists.cbegin()->second.at(0));
        for (auto const &conflict : conflict_lists)
        {
            auto const &f = conflict.first;
            auto const &points = conflict.second;

            auto const furthest = *std::max_element(
                points.cbegin(), points.cend(),
                [&f](auto a, auto b){
                    return f->distanceTo(a) < f->distanceTo(b);});

            if (far.first->distanceTo(far.second) < f->distanceTo(furthest))
                far = std::make_pair(f, furthest);
        }

        auto const eye = far.second;
        auto const facet = far.first;

        // Find horizon for furthest point.
        auto const horizon_and_visible = get_horizon(eye, facet, convex_hull);
        auto const horizon = horizon_and_visible.first;
        auto const visible = horizon_and_visible.second;

        // Delete facets visible to the eye point.
        for (auto const &visible_face : visible)
            convex_hull.facets.erase(visible_face);

        // Add faces between each horizon edge and the eye point.
        for (auto const &edge : horizon)
            convex_hull.addFacet(Facet{far.second, edge.first, edge.second});
        assert(convex_hull.checkIntegrity());

        outer_points = get_outer_points(convex_hull, outer_points);
        conflict_lists = get_conflict_lists(convex_hull, outer_points);
        std::cout << "===\n";
    }


    // return results
    std::vector<HalfPlane> halfplane_hull{};
    for (auto const &f : convex_hull.facets)
        halfplane_hull.push_back(*f);

    std::unordered_set<glm::vec3> contrib{};
    for (auto const &f : convex_hull.facets)
    {
        for (auto const &e : f->edges)
        {
            contrib.insert(e.first);
            contrib.insert(e.second);
        }
    }
    std::vector<glm::vec3> contribv{contrib.cbegin(), contrib.cend()};

{//TEMP
    std::cout << "FINAL\n";
    for (auto const &p : halfplane_hull)
        std::cout << "  " << p.normal() << '\n';
}//TEMP
    return std::make_pair(halfplane_hull, contribv);
}
