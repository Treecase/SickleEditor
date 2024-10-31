/**
 * BoundingBox.hpp - Bounding box.
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

#ifndef SE_BOUNDINGBOX_HPP
#define SE_BOUNDINGBOX_HPP

#include <glm/glm.hpp>

#include <initializer_list>

/** Templated bounding box using glm vectors. */
template<glm::length_t L, typename T>
struct BBox
{
    using Point = glm::vec<L, T>;
    Point min{INFINITY}, max{-INFINITY};
    BBox() = default;

    BBox(std::initializer_list<Point> const &points)
    {
        for (auto const &point : points)
        {
            add(point);
        }
    }

    T volume() const
    {
        auto const wh = glm::abs(max - min);
        return wh.x * wh.y;
    }

    bool contains(Point point) const
    {
        return (
            glm::all(glm::lessThanEqual(min, point))
            && glm::all(glm::lessThanEqual(point, max)));
    }

    void add(Point pt)
    {
        for (glm::length_t i = 0; i < Point::length(); ++i)
        {
            if (pt[i] < min[i])
            {
                min[i] = pt[i];
            }
            if (pt[i] > max[i])
            {
                max[i] = pt[i];
            }
        }
    }
};

using BBox3 = BBox<glm::vec3::length(), glm::vec3::value_type>;
using BBox2 = BBox<glm::vec2::length(), glm::vec2::value_type>;

#endif
