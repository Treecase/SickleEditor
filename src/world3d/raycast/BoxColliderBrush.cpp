/**
 * BoxColliderBrush.cpp - 3D box collider for raycast operations, Brush
 *                        tracking version.
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

#include "BoxColliderBrush.hpp"

#include <stdexcept>

using namespace World3D;

void BoxColliderBrush::on_attach(Sickle::Componentable &obj)
{
    if (_src)
    {
        throw std::logic_error{"already attached"};
    }
    auto &brush = dynamic_cast<Sickle::Editor::Brush &>(obj);
    _src = &brush;
    _signals = std::make_unique<Signals>();

    for (auto const &face : _src->faces())
    {
        _signals->conns.push_back(face->signal_vertices_changed().connect(
            sigc::mem_fun(*this, &BoxColliderBrush::update_bbox)));
    }
    update_bbox();
}

void BoxColliderBrush::on_detach(Sickle::Componentable &obj)
{
    _src = nullptr;
    _signals.release();
    set_box(BBox3{});
}

void BoxColliderBrush::update_bbox()
{
    BBox3 bbox{};
    for (auto const &face : _src->faces())
    {
        for (auto const vertex : face->get_vertices())
        {
            bbox.add(vertex);
        }
    }
    set_box(bbox);
}
