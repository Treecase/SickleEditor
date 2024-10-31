/**
 * GrabbableBox.cpp - 2D box with grabbable handles.
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

#include "GrabbableBox.hpp"

using namespace Sickle;

/* ===[ GrabbableBox ]=== */
std::vector<GrabbableBox::Area> GrabbableBox::get_handle_areas()
{
    return {
        Area::N,
        Area::NE,
        Area::E,
        Area::SE,
        Area::S,
        Area::SW,
        Area::W,
        Area::NW,
    };
}

void GrabbableBox::set_box(BBox2 const &box)
{
    _center = box;
    auto const width = box.max.x - box.min.x;
    auto const height = box.max.y - box.min.y;

    _handles[Area::NW] = Handle{
        box.min,
        {-1.5, -1.5},
        grab_size
    };
    _handles[Area::NE] = Handle{
        {box.max.x, box.min.y},
        {      1.5,      -1.5},
        grab_size
    };
    _handles[Area::SW] = Handle{
        {box.min.x, box.max.y},
        {     -1.5,       1.5},
        grab_size
    };
    _handles[Area::SE] = Handle{
        box.max,
        {1.5, 1.5},
        grab_size
    };

    _handles[Area::N] = Handle{
        box.min + glm::vec2{width / 2.0,    0},
        {          0, -1.5},
        grab_size
    };
    _handles[Area::E] = Handle{
        box.min + glm::vec2{width, height / 2.0},
        {  1.5,            0},
        grab_size
    };
    _handles[Area::S] = Handle{
        glm::vec2{  box.min.x, box.max.y}
        + glm::vec2{width / 2.0,         0},
        {          0,       1.5},
        grab_size
    };
    _handles[Area::W] = Handle{
        box.min + glm::vec2{   0, height / 2.0},
        {-1.5,            0},
        grab_size
    };
}

BBox2 GrabbableBox::get_box() const
{
    return _center;
}

GrabbableBox::Handle GrabbableBox::get_handle(Area area) const
{
    return _handles.at(area);
}

GrabbableBox::Area GrabbableBox::check_point(glm::vec2 const &point)
{
    if (_center.contains(point))
    {
        return Area::CENTER;
    }

    for (size_t i = 0; i < _handles.size(); ++i)
    {
        if (_handles[i].bounds(unit).contains(point))
        {
            return static_cast<Area>(i);
        }
    }

    return Area::NONE;
}

/* ===[ GrabbableBox::Handle ]=== */
BBox2 GrabbableBox::Handle::bounds(float unit) const
{
    auto const &size_scaled = 0.5f * size * unit;
    return BBox2{
        anchor - size_scaled + glm::sign(direction) * size_scaled,
        anchor + size_scaled + glm::sign(direction) * size_scaled};
}
