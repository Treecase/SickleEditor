/**
 * BBox2View.cpp - Views for BBox2s.
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

#include "BBox2View.hpp"

using namespace Sickle;

/* ===[ BBox2View ]=== */
void BBox2View::draw(
    Cairo::RefPtr<Cairo::Context> const &cr,
    BBox2 const &box,
    float unit)
{
    auto const width = box.max.x - box.min.x;
    auto const height = box.max.y - box.min.y;
    cr->rectangle(box.min.x, box.min.y, width, height);
}

/* ===[ BBox2ViewCustom ]=== */
BBox2ViewCustom::BBox2ViewCustom(Func const &pre, Func const &post)
: _pre{pre}
, _post{post}
{
}

void BBox2ViewCustom::draw(
    Cairo::RefPtr<Cairo::Context> const &cr,
    BBox2 const &box,
    float unit)
{
    _pre(cr, box, unit);
    BBox2View::draw(cr, box, unit);
    _post(cr, box, unit);
}
