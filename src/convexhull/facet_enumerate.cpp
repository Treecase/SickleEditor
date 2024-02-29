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

#include "convexhull.hpp"

#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/epsilon.hpp>

#include <algorithm>
#include <array>
#include <map>
#include <memory>
#include <optional>
#include <stdexcept>
#include <unordered_map>


/* ===[ Facet Enumeration Utils ]=== */
enum class EdgeCompare
{
    EQUAL,
    NOTEQUAL,
    OPPOSITE,
};

struct Edge
{
    glm::vec3 first, second;

    Edge(glm::vec3 const &first, glm::vec3 const &second)
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


class ConvexHull
{
public:
    std::unordered_set<HalfPlane> facets{};
    std::unordered_map<HalfPlane, std::vector<Edge>> edges{};
    std::unordered_map<
        HalfPlane,
        std::unordered_map<Edge, HalfPlane>> neighbors{};

    // Unless this is the first facet being added, B and C must make up an edge
    // already in the hull.
    void addFacet(glm::vec3 const &a, glm::vec3 const &b, glm::vec3 const &c)
    {
        auto const p = facets.emplace(a, b, c);
        auto const newf = *p.first;
        // Plane already exists in the hull.
        if (!p.second)
        {
            _merge_existing_facet(newf, a, b, c);
        }
        // Plane does not already exist in the hull.
        else
        {
            edges[newf] = {};
            edges.at(newf).emplace_back(a, b);
            edges.at(newf).emplace_back(b, c);
            edges.at(newf).emplace_back(c, a);
        }
        recalculateNeighbors();
    }

    void removeFacet(HalfPlane const &facet)
    {
        facets.erase(facet);
        edges.erase(facet);
        neighbors.erase(facet);
        // Erase references in the neighbors map.
        for (auto &kv : neighbors)
        {
            for (auto it = kv.second.begin(); it != kv.second.end(); )
            {
                if (it->second == facet)
                    it = kv.second.erase(it);
                else
                    it++;
            }
        }
    }

    auto as_planes() const
    {
        return facets;
    }

    auto as_points() const
    {
        std::unordered_set<glm::vec3> points{};
        for (auto const kv : edges)
        {
            for (auto const edge : kv.second)
            {
                points.insert(edge.first);
                points.insert(edge.second);
            }
        }
        return points;
    }

#if !NDEBUG
    // for debugging
    bool checkIntegrity() const
    {
        for (auto const &facet : facets)
        {
            // Every facet must have a neighbors map
            assert(neighbors.count(facet) != 0);
            // Every facet must have edges
            assert(edges.count(facet) != 0);
        }

        // Edges must be ordered clockwise
        for (auto const &kv : edges)
        {
            for (size_t i = 0; i + 1 < kv.second.size(); ++i)
                assert(kv.second.at(i).second == kv.second.at(i+1).first);
            assert(kv.second.back().second == kv.second.front().first);
        }

        for (auto const &kv : edges)
        {
            for (auto const &edge : kv.second)
            {
                // Every edge must have a neighbor
                assert(neighbors.at(kv.first).count(edge) != 0);
                // All the neighbors must be in the hull
                assert(neighbors.count(neighbors.at(kv.first).at(edge)) != 0);
            }
        }
        return true;
    }
#endif // !NDEBUG

private:
    // Splice new edges into an existing face
    void _merge_existing_facet(
        HalfPlane const &facet,
        glm::vec3 const &a,
        glm::vec3 const &b,
        glm::vec3 const &c)
    {
        assert(facets.count(facet) != 0);

        auto &e = edges.at(facet);
        Edge const existing{c, b};

        auto const replace = std::find(e.begin(), e.end(), existing);
        assert(replace != e.end());

        Edge const e1{existing.first, a};
        Edge const e2{a, existing.second};

        e.insert(e.erase(replace), {e1, e2});
    }

    // Find the edge between two planes.
    std::optional<std::pair<Edge, Edge>>
    neighboring_edge(HalfPlane const &a, HalfPlane const &b) const
    {
        for (auto const &edge1 : edges.at(a))
            for (auto const &edge2 : edges.at(b))
                if (edge1.compare(edge2) == EdgeCompare::OPPOSITE)
                    return std::make_pair(edge1, edge2);
        return std::nullopt;
    }

    // Regenerare the `neighbors' map.
    void recalculateNeighbors()
    {
        neighbors.clear();
        for (auto const &f1 : facets)
        {
            for (auto const &f2 : facets)
            {
                if (f1 == f2) continue;

                auto edges_pair = neighboring_edge(f1, f2);
                if (edges_pair.has_value())
                {
                    assert(
                        neighbors[f1].count(edges_pair->first) == 0
                        || neighbors[f1][edges_pair->first] == f2);
                    assert(
                        neighbors[f2].count(edges_pair->second) == 0
                        || neighbors[f2][edges_pair->second] == f1);
                    neighbors[f1][edges_pair->first] = f2;
                    neighbors[f2][edges_pair->second] = f1;
                }
            }
        }
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
    HalfPlane const plane{min, max, farL};
    auto const farP = *std::max_element(
        vertices.cbegin(), vertices.cend(),
        [&plane](auto a, auto b){
            return abs(plane.distanceTo(a)) < abs(plane.distanceTo(b));
        });
    auto const farD = plane.classify(farP);
    // Degenerate case if this point is on the plane.
    if (farD == ON)
        throw std::runtime_error{"create_tetrahedron degenerate case 2D"};

    // Create the tetrahedron. FARD's sign tells us clockwise vertex ordering.
    auto const A = min;
    auto const B = max;
    auto const C = farL;
    auto const D = farP;
    ConvexHull out{};
    if (farD == BELOW)
    {
        out.addFacet(A, B, C);
        out.addFacet(D, B, A);
        out.addFacet(D, C, B);
        out.addFacet(D, A, C);
    }
    else
    {
        out.addFacet(A, C, B);
        out.addFacet(D, A, B);
        out.addFacet(D, B, C);
        out.addFacet(D, C, A);
    }
    return out;
}

/** Find the points outside a convex hull. */
auto
get_outer_points(ConvexHull const &hull, std::vector<glm::vec3> const &points)
{
    std::vector<glm::vec3> outside{};
    std::copy_if(
        points.cbegin(), points.cend(),
        std::back_inserter(outside),
        [&hull](auto p){
            return std::any_of(
                hull.facets.cbegin(), hull.facets.cend(),
                [&p](auto plane){return plane.classify(p) == ABOVE;});
        });
    return outside;
}

/**
 * Maps points to visible facets. Each point is assigned to a single facet,
 * even if multiple are visible. A facet is visible to a point if the point is
 * above the facet.
 */
auto get_conflict_lists(
    ConvexHull const &hull,
    std::vector<glm::vec3> const &points)
{
    std::unordered_map<HalfPlane, std::vector<glm::vec3>> conflicts{};
    for (auto const &point : points)
    {
        for (auto const &facet : hull.facets)
        {
            if (facet.classify(point) == ABOVE)
            {
                conflicts[facet].push_back(point);
                break;
            }
        }
    }
    return conflicts;
}


void dfs2(
    ConvexHull const &hull,
    glm::vec3 const &eye,
    HalfPlane const &f,
    std::unordered_set<HalfPlane> &visited,
    std::vector<Edge> &horizon)
{
    visited.insert(f);
    for (auto const &edge : hull.edges.at(f))
    {
        auto const &next = hull.neighbors.at(f).at(edge);
        // Crossing this edge leads to a non-visible face, so this edge is on
        // the horizon.
        if (next.classify(eye) != ABOVE)
        {
            horizon.push_back(edge);
        }
        // If we haven't already visited the neighboring face, visit it.
        else if (visited.count(next) == 0)
            dfs2(hull, eye, next, visited, horizon);
    }
}

auto dfs(ConvexHull const &hull, glm::vec3 const &eye, HalfPlane const &start)
{
    assert(start.classify(eye) == ABOVE);
    std::unordered_set<HalfPlane> visited{};
    std::vector<Edge> horizon{};
    dfs2(hull, eye, start, visited, horizon);
    return std::make_pair(horizon, visited);
}

/**
 * Find the list of edges on EYE's horizon.
 * A horizon edge is an edge between a visible facet and a non-visible facet.
 */
auto get_horizon(
    glm::vec3 const &eye,
    HalfPlane const &start,
    ConvexHull const &facets)
{
    // run a depth-first search through the convexhull facets, using START as
    // the root
    auto const horizon_and_visible = dfs(facets, eye, start);
    auto const horizon = horizon_and_visible.first;

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

    auto outside_points = get_outer_points(convex_hull, vertices);
    auto conflict_lists = get_conflict_lists(convex_hull, outside_points);

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
                    return f.distanceTo(a) < f.distanceTo(b);});

            if (far.first.distanceTo(far.second) < f.distanceTo(furthest))
                far = std::make_pair(f, furthest);
        }

        auto const eye = far.second;
        auto const facet = far.first;

        auto const horizon_and_visible = get_horizon(eye, facet, convex_hull);
        auto const horizon_edges = horizon_and_visible.first;
        auto const visible_faces = horizon_and_visible.second;

        for (auto const &face : visible_faces)
            convex_hull.removeFacet(face);

        for (auto const &edge : horizon_edges)
            convex_hull.addFacet(eye, edge.first, edge.second);

        assert(convex_hull.checkIntegrity());
        outside_points = get_outer_points(convex_hull, outside_points);
        conflict_lists = get_conflict_lists(convex_hull, outside_points);
    }


    auto const planes1 = convex_hull.as_planes();
    std::vector<HalfPlane> const planes{planes1.cbegin(), planes1.cend()};
    auto const points1 = convex_hull.as_points();
    std::vector<glm::vec3> const points{points1.cbegin(), points1.cend()};
    return std::make_pair(planes, points);
}
