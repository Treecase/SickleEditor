/**
 * BrushBBox.hpp - Editor::Brush bounding-box component.
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

#include "BrushBBox.hpp"

using namespace World2D;

BBox2 BrushBBox::bbox(Sickle::MapArea2D const &maparea) const
{
    if (!_brush)
    {
        return BBox2{
            glm::vec2{0.0f, 0.0f}
        };
    }

    BBox2 the_bbox{};
    for (auto const &face : _brush->faces())
    {
        for (auto const &vertex : face->get_vertices())
        {
            the_bbox.add(maparea.worldspace_to_drawspace(vertex));
        }
    }
    return the_bbox;
}

void BrushBBox::on_attach(Sickle::Componentable &obj)
{
    if (_brush)
    {
        throw std::logic_error{"already attached"};
    }
    _brush = &dynamic_cast<Sickle::Editor::Brush const &>(obj);
}

void BrushBBox::on_detach(Sickle::Componentable &obj)
{
    _brush = nullptr;
}
