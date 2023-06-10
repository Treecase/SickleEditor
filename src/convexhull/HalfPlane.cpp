/**
 * HalfPlane.cpp - HalfPlane class.
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

#include "convexhull/convexhull.hpp"

#include <glm/gtc/epsilon.hpp>


HalfPlane::HalfPlane(float a, float b, float c, float d)
:   a{a}, b{b}, c{c}, d{d}
{
}


HalfPlane::HalfPlane(glm::vec3 A, glm::vec3 B, glm::vec3 C)
{
    auto const normal = glm::normalize(glm::cross(C - A, B - A));
    a = normal.x;
    b = normal.y;
    c = normal.z;
    d = -normal.x*A.x - normal.y*A.y - normal.z*A.z;
}


bool HalfPlane::operator==(HalfPlane const &other) const
{
    return glm::all(
        glm::epsilonEqual(
            glm::vec4{a, b, c, d},
            glm::vec4{other.a, other.b, other.c, other.d},
            EPSILON));
}


float HalfPlane::distanceTo(glm::vec3 const &p) const
{
    return a * p.x + b * p.y + c * p.z + d;
}


Classification HalfPlane::classify(glm::vec3 const &point) const
{
    auto const d = distanceTo(point);
    if (d > EPSILON)
        return ABOVE;
    else if (d < -EPSILON)
        return BELOW;
    else
        return ON;
}


bool HalfPlane::isPointOnPlane(glm::vec3 const &point) const
{
    return classify(point) == ON;
}


glm::vec3 HalfPlane::normal() const
{
    return {a, b, c};
}
