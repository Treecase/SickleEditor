/**
 * convexhull.cpp - Vertex and Facet enumeration algorithms.
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

#include "convexhull.hpp"

#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/epsilon.hpp>



/** Cramer's rule. Solve `Ax = d` for x. */
template<glm::length_t C, typename T>
bool _cramer(
    glm::mat<C, C, T> const &A, glm::vec<C, T> const &d, glm::vec<C, T> *x)
{
    static constexpr float EPSILON = glm::epsilon<float>();
    auto const &a = A[0];
    auto const &b = A[1];
    auto const &c = A[2];
    auto const &D = glm::determinant(A);
    if (glm::epsilonEqual(glm::length(d), 0.0f, EPSILON))
    {
        // Only solution is 0,0,0
        if (glm::epsilonEqual(D, 0.0f, EPSILON))
        {
            *x = {0.0f, 0.0f, 0.0f};
            return true;
        }
        // Infinite solutions
        else
            ;
    }
    else
    {
        // Single solution
        if (glm::epsilonNotEqual(D, 0.0f, EPSILON))
        {
            *x = {
                glm::determinant(glm::mat3{d, b, c}) / D,
                glm::determinant(glm::mat3{a, d, c}) / D,
                glm::determinant(glm::mat3{a, b, d}) / D};
            return true;
        }
        // No unique solution
        else
            ;
    }
    return false;
}

/**
 * True if x is a valid solution to `b + Ax >= 0`.
 * Rows of A come from plane.a,b,c; rows of b come from plane.d.
 */
bool _is_point_in_polygon(
    std::vector<HalfPlane> const &facets, glm::vec3 const &x)
{
    static constexpr double EPSILON = 0.0001;
    // 'Epsilon >=' only needs to compare against lower bound.
    for (auto const &f : facets)
        if (f.solveForPoint(x) < -EPSILON)
            return false;
    return true;
}


/* ===[ HalfPlane ]=== */
double HalfPlane::solveForPoint(glm::vec3 const &p) const
{
    return a * p.x + b * p.y + c * p.z + d;
}


std::unordered_set<glm::vec3> vertex_enumeration(
    std::vector<HalfPlane> const &facets)
{
    // Algorithm from
    // http://www.lab2.kuis.kyoto-u.ac.jp/~avis/courses/pc/2010/notes/lec2.pdf

    // A is an NxD matrix.
    // Each row of A contains a plane's coefficients.
    //     [a1 b1 c1]
    // A = [a2 b2 c2]
    //     [   ..   ]

    // b is an N-dimensional column vector.
    //     [d1]
    // b = [d2]
    //     [..]
    // Each row of b contains a plane's d coefficient.

    // b + Ax >= 0
    // Where x is a D-dimensional row vector.

    // b    + A          x                             >= 0
    // [d1]   [a1 b1 c1]              [a1x1 b1x2 c1x3]
    // [d2] + [a2 b2 c2] [x1 x2 x3] = [a2x1 b2x2 c2x3] >= 0
    // [..]   [   ..   ]              [      ..      ]


    // This simplified method can produce duplicates, unordered_set is used to
    // filter these out.
    std::unordered_set<glm::vec3> vertices{};

    // Pick 3 facets.
    for (auto const &p0 : facets)
    {
        for (auto const &p1 : facets)
        {
            for (auto const &p2 : facets)
            {
                // Let B be the 3x3 submatrix of A corresponding to the chosen
                // facets.
                // [a_i b_i c_i]
                // [a_j b_j c_j]
                // [a_k b_k c_k]
                glm::mat3 const B{
                    {-p0.a, -p1.a, -p2.a},
                    {-p0.b, -p1.b, -p2.b},
                    {-p0.c, -p1.c, -p2.c},
                };
                // Let b_bar be the corresponding subvector of b.
                // [d_i]
                // [d_j]
                // [d_k]
                glm::vec3 const b_bar{-p0.d, -p1.d, -p2.d};
                glm::vec3 x_bar{};
                // If `b_bar + B*x_bar = 0` has a unique solution, and that
                // solution satisfies `b + A*x_bar >= 0`, output it.
                if (_cramer(B, -b_bar, &x_bar))
                    if (_is_point_in_polygon(facets, x_bar))
                        vertices.emplace(glm::round(x_bar)); // WARNING: value gets rounded to avoid issues with the unordered_set. Fix this in future!
            }
        }
    }
    return vertices;
}

std::vector<HalfPlane> facet_enumeration(std::vector<glm::vec3> const &vertices)
{
    return {};
}
