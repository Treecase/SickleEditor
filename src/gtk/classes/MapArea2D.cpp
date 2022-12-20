/**
 * MapArea2D.cpp - Sickle editor main window DrawingArea.
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

#include "MapArea2D.hpp"


Sickle::MapArea2D::MapArea2D()
:   Glib::ObjectBase{typeid(MapArea2D)}
,   Gtk::DrawingArea{}
,   _map{nullptr}
,   _prop_clear_color{*this, "clear-color", {}}
,   _prop_grid_size{*this, "grid-size", 32}
,   _prop_name{*this, "name", "<blank>"}
{
    set_hexpand(true);
    set_vexpand(true);
    set_size_request(320, 240);
    set_can_focus(true);

    add_events(
        Gdk::POINTER_MOTION_MASK
        | Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK
        | Gdk::BUTTON_MOTION_MASK | Gdk::BUTTON_PRESS_MASK
        | Gdk::SCROLL_MASK);
}

void Sickle::MapArea2D::set_map(MAP::Map const *map)
{
    _map = const_cast<MAP::Map *>(map);
}

bool Sickle::MapArea2D::on_draw(Cairo::RefPtr<Cairo::Context> const &cr)
{
    auto const grid_size = property_grid_size().get_value();
    auto const name = property_name().get_value();

    auto context = get_style_context();

    auto const width = get_allocated_width();
    auto const height = get_allocated_height();

    // Clear to black
    auto const &clear_color = property_clear_color().get_value();
    cr->set_source_rgb(clear_color.get_red(), clear_color.get_green(), clear_color.get_blue());
    cr->paint();

    // Draw grid
    cr->set_source_rgb(0.3, 0.3, 0.3);
    cr->set_line_width(1);
    // Vertical lines
    cr->move_to(0, 0);
    for (int x = 0; x <= width; x += grid_size)
    {
        cr->rel_line_to(0, height);
        cr->rel_move_to(grid_size, -height);
    }
    // Horizontal lines
    cr->move_to(0, 0);
    for (int y = 0; y <= width; y += grid_size)
    {
        cr->rel_line_to(width, 0);
        cr->rel_move_to(-width, grid_size);
    }
    cr->stroke();

    // Show name in top-left corner
    cr->set_source_rgb(1, 1, 1);
    cr->select_font_face("sans-serif", Cairo::FontSlant::FONT_SLANT_NORMAL, Cairo::FontWeight::FONT_WEIGHT_NORMAL);
    cr->set_font_size(12);
    cr->move_to(4, 4);
    Cairo::TextExtents extents;
    cr->get_text_extents(name, extents);
    cr->rel_move_to(-extents.x_bearing, -extents.y_bearing);
    cr->show_text(name);

    return true;
}

bool Sickle::MapArea2D::on_key_press_event(GdkEventKey *event)
{
    return Gtk::DrawingArea::on_key_press_event(event);
}

bool Sickle::MapArea2D::on_key_release_event(GdkEventKey *event)
{
    return Gtk::DrawingArea::on_key_press_event(event);
}


bool Sickle::MapArea2D::on_button_press_event(GdkEventButton *event)
{
    return Gtk::DrawingArea::on_button_press_event(event);
}

bool Sickle::MapArea2D::on_motion_notify_event(GdkEventMotion *event)
{
    return Gtk::DrawingArea::on_motion_notify_event(event);
}

bool Sickle::MapArea2D::on_scroll_event(GdkEventScroll *event)
{
    return Gtk::DrawingArea::on_scroll_event(event);
}
