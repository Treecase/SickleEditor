/**
 * BoxCollider.cpp - 3D box collider for raycast operations.
 * Copyright (C) 2024 Trevor Last
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

#include "BoxCollider.hpp"


using namespace World3D;


void BoxCollider::execute()
{
}


BBox3 BoxCollider::get_box() const
{
    return _bbox;
}



void BoxCollider::set_box(BBox3 const &bbox)
{
    _bbox = bbox;
}
