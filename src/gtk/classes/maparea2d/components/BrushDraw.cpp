/**
 * BrushDraw.cpp - Cairo Editor::Brush view.
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

#include "BrushDraw.hpp"

using namespace World2D;

void BrushDraw::draw(
    Cairo::RefPtr<Cairo::Context> const &cr,
    Sickle::MapArea2D const &maparea) const
{
    if (!cr || !_brush)
    {
        return;
    }

    auto const style = maparea.get_style_context();
    style->context_save();
    style->add_class("brush");

    if (_brush->is_selected())
    {
        style->set_state(
            style->get_state() | Gtk::StateFlags::STATE_FLAG_SELECTED);
    }

    auto const color = style->get_color(style->get_state());
    cr->set_source_rgb(color.get_red(), color.get_green(), color.get_blue());

    for (auto const &face : _brush->faces())
    {
        if (face->get_vertices().empty())
        {
            continue;
        }
        auto const p0 = maparea.worldspace_to_drawspace(face->get_vertex(0));
        cr->move_to(p0.x, p0.y);
        for (auto const &vertex : face->get_vertices())
        {
            auto const p = maparea.worldspace_to_drawspace(vertex);
            cr->line_to(p.x, p.y);
        }
        cr->close_path();
    }
    cr->stroke();
    style->context_restore();
}

void BrushDraw::on_attach(Sickle::Componentable &obj)
{
    if (_brush)
    {
        throw std::logic_error{"already attached"};
    }
    _brush = &dynamic_cast<Sickle::Editor::Brush const &>(obj);
}

void BrushDraw::on_detach(Sickle::Componentable &obj)
{
    _brush = nullptr;
}
