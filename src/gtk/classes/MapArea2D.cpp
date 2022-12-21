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


struct DrawAnchor {
    bool top;
    bool left;
};
static DrawAnchor const TopLeft{true, true};
static DrawAnchor const TopRight{true, false};
static DrawAnchor const BottomLeft{false, true};
static DrawAnchor const BottomRight{false, false};

/** Draw text. */
void drawtext(Cairo::RefPtr<Cairo::Context> const &cr, std::string const &text, int x, int y, DrawAnchor anchor=TopLeft)
{
    Cairo::TextExtents extents;
    cr->get_text_extents(text, extents);

    int tx = x - extents.x_bearing;
    int ty = y - extents.y_bearing;

    if (!anchor.left)
        tx -= extents.width;
    if (!anchor.top)
        ty -= extents.height;

    cr->move_to(tx, ty);
    cr->show_text(text);
}


/* ===[ MapArea2D ]=== */
Sickle::MapArea2D::MapArea2D()
:   Glib::ObjectBase{typeid(MapArea2D)}
,   Gtk::DrawingArea{}
,   _map{nullptr}
,   _transform{0, 0}
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
    _map = map;
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


    /* ===[ World-Space Drawing ]=== */
    cr->save();
    cr->set_antialias(Cairo::ANTIALIAS_NONE);
    cr->translate(width / 2.0, height / 2.0);

    /* Grid */
    cr->set_source_rgb(0.3, 0.3, 0.3);
    cr->set_line_width(1);
    int const grid_count_v = width / grid_size;
    int const grid_count_h = width / grid_size;
    int const left = -(grid_count_v / 2) * grid_size;
    int const top = -(grid_count_h / 2) * grid_size;

    // Vertical lines
    cr->move_to(left + _transform.x % grid_size, -height / 2.0);
    for (int i = 0; i <= grid_count_v; ++i)
    {
        cr->rel_line_to(0, height);
        cr->rel_move_to(grid_size, -height);
    }
    // Horizontal lines
    cr->move_to(-width / 2.0, top + _transform.y % grid_size);
    for (int i = 0; i <= grid_count_h; ++i)
    {
        cr->rel_line_to(width, 0);
        cr->rel_move_to(-width, grid_size);
    }
    cr->stroke();

    // x/y axes
    cr->set_source_rgb(0.5, 0.5, 0.5);
    cr->set_line_width(2);
    cr->move_to(_transform.x, -height / 2.0);
    cr->rel_line_to(0, height);
    cr->move_to(-width / 2.0, _transform.y);
    cr->rel_line_to(width, 0);
    cr->stroke();

    /* World */
    cr->translate(_transform.x, _transform.y);

    // Draw a "brush"
    cr->set_source_rgb(1, 0, 0);
    cr->set_line_width(1);
    cr->rectangle(0, 0, grid_size, grid_size);
    cr->stroke();

    cr->restore();


    /* ===[ Screen-Space Overlay Drawing ]=== */
    cr->set_source_rgb(1, 1, 1);
    cr->select_font_face("sans-serif", Cairo::FontSlant::FONT_SLANT_NORMAL, Cairo::FontWeight::FONT_WEIGHT_NORMAL);
    cr->set_font_size(12);
    // Show name in top-left corner
    drawtext(cr, name, 4, 4);
    // Show transform coords in top-right corner
    drawtext(cr, std::to_string(_transform.x) + "," + std::to_string(_transform.y), width - 4, 4, TopRight);

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
    if (event->button == 2)
    {
        _state.pointer_prev_x = event->x;
        _state.pointer_prev_y = event->y;
        return true;
    }
    return Gtk::DrawingArea::on_button_press_event(event);
}

bool Sickle::MapArea2D::on_motion_notify_event(GdkEventMotion *event)
{
    if (event->state & Gdk::BUTTON2_MASK)
    {
        auto dx = event->x - _state.pointer_prev_x;
        auto dy = event->y - _state.pointer_prev_y;
        _transform.x += dx;
        _transform.y += dy;
        _state.pointer_prev_x = event->x;
        _state.pointer_prev_y = event->y;
        queue_draw();
        return true;
    }
    return Gtk::DrawingArea::on_motion_notify_event(event);
}

bool Sickle::MapArea2D::on_scroll_event(GdkEventScroll *event)
{
    return Gtk::DrawingArea::on_scroll_event(event);
}
