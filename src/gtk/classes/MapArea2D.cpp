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

#include <cmath>
#include <algorithm>


#define ZOOM_MULTIPLIER  1.1
#define MIN_ZOOM    0.05
#define MAX_ZOOM    16.0


struct DrawAnchor {
    bool top;
    bool left;
};
static DrawAnchor const TopLeft{true, true};
static DrawAnchor const TopRight{true, false};
static DrawAnchor const BottomLeft{false, true};
static DrawAnchor const BottomRight{false, false};


/**
 * Convert POINTS from point to pixel units. Needed because Cairo's
 * set_font_size set the font size in pixels, not points.
 */
double points_to_pixels(double points, double dpi)
{
    static constexpr auto const point_in_inches = 1.0 / 72.0;
    auto const inches_per_pixel = 1.0 / dpi;
    auto const font_size_inches = point_in_inches * points;
    return font_size_inches / inches_per_pixel;
}

/** Draw the grid. */
void draw_grid(
    Cairo::RefPtr<Cairo::Context> const &cr, double width, double height,
    double grid_size, double _transform_x, double _transform_y)
{
    auto const dx = std::fmod(_transform_x, grid_size);
    auto const dy = std::fmod(_transform_y, grid_size);
    int const count_x = std::ceil((width / 2) / grid_size);
    int const count_y = std::ceil((height / 2) / grid_size);
    for (int i = 0; i <= count_x; ++i)
    {
        cr->move_to(i*grid_size + dx, -height/2);
        cr->rel_line_to(0, height);
        cr->move_to(-i*grid_size + dx, -height/2);
        cr->rel_line_to(0, height);
    }
    for (int i = 0; i <= count_y; ++i)
    {
        cr->move_to(-width/2, i*grid_size + dy);
        cr->rel_line_to(width, 0);
        cr->move_to(-width/2, -i*grid_size + dy);
        cr->rel_line_to(width, 0);
    }
}

/** Draw main axes. */
void draw_axes(
    Cairo::RefPtr<Cairo::Context> const &cr, double width, double height,
    double _transform_x, double _transform_y)
{
    cr->move_to(_transform_x, -height / 2.0);
    cr->rel_line_to(0, height);
    cr->move_to(-width / 2.0, _transform_y);
    cr->rel_line_to(width, 0);
}

/**
 * Sets CR's font_face and font_size based on FONT and DPI. Returns font size.
 */
double select_font_from_pango(
    Cairo::RefPtr<Cairo::Context> const &cr,
    Pango::FontDescription const &font,
    double dpi)
{
    double const font_size = (
        font.get_size_is_absolute()
        ? font.get_size() / Pango::SCALE
        : points_to_pixels(font.get_size() / Pango::SCALE, dpi));

    auto weight = Cairo::FontWeight::FONT_WEIGHT_NORMAL;
    if (font.get_weight() == Pango::Weight::WEIGHT_BOLD)
        weight = Cairo::FontWeight::FONT_WEIGHT_BOLD;

    auto slant = Cairo::FontSlant::FONT_SLANT_NORMAL;
    switch (font.get_style())
    {
    case Pango::Style::STYLE_ITALIC:
        slant = Cairo::FontSlant::FONT_SLANT_ITALIC;
        break;
    case Pango::Style::STYLE_OBLIQUE:
        slant = Cairo::FontSlant::FONT_SLANT_OBLIQUE;
        break;
    }

    cr->select_font_face(font.get_family(), slant, weight);
    cr->set_font_size(font_size);
    return font_size;
}

/** Draw text. */
void draw_text(
    Cairo::RefPtr<Cairo::Context> const &cr, std::string const &text,
    double x, double y, DrawAnchor anchor=TopLeft)
{
    Cairo::TextExtents extents;
    cr->get_text_extents(text, extents);

    auto tx = x - extents.x_bearing;
    auto ty = y - extents.y_bearing;

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
,   _map{}
,   _transform{0, 0, 1.0}
,   _prop_clear_color{*this, "clear-color", {}}
,   _prop_grid_size{*this, "grid-size", 32}
,   _prop_name{*this, "name", "<blank>"}
{
    set_hexpand(true);
    set_vexpand(true);
    set_size_request(320, 240);
    set_can_focus(true);

    property_grid_size().signal_changed().connect(
        sigc::mem_fun(*this, MapArea2D::queue_draw));

    add_events(
        Gdk::POINTER_MOTION_MASK
        | Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK
        | Gdk::BUTTON_MOTION_MASK | Gdk::BUTTON_PRESS_MASK
        | Gdk::SCROLL_MASK);
}

void Sickle::MapArea2D::set_map(MAP::Map const *map)
{
    if (map)
        _map = MAP::V::VertexMap::from_planes_map(*map);
    else
        _map = MAP::V::VertexMap{};
    queue_draw();
}

void Sickle::MapArea2D::set_draw_angle(DrawAngle angle)
{
    _angle = angle;
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
    cr->set_source_rgb(
        clear_color.get_red(), clear_color.get_green(), clear_color.get_blue());
    cr->paint();


    /* ===[ Grid ]=== */
    cr->save();
        cr->set_antialias(Cairo::ANTIALIAS_NONE);
        cr->translate(width / 2.0, height / 2.0);
        cr->scale(_transform.zoom, _transform.zoom);

        // Draw the grid
        cr->set_source_rgb(0.3, 0.3, 0.3);
        cr->set_line_width(1 / _transform.zoom);
        draw_grid(
            cr,
            width / _transform.zoom, height / _transform.zoom,
            grid_size,
            _transform.x / _transform.zoom, _transform.y / _transform.zoom);
        cr->stroke();

        // Draw the x/y axes
        cr->set_source_rgb(0.5, 0.5, 0.5);
        cr->set_line_width(2 / _transform.zoom);
        draw_axes(
            cr,
            width / _transform.zoom, height / _transform.zoom,
            _transform.x / _transform.zoom, _transform.y / _transform.zoom);
        cr->stroke();
    cr->restore();


    /* ===[ World-Space Drawing ]=== */
    cr->save();
        cr->set_antialias(Cairo::ANTIALIAS_NONE);
        cr->translate(width / 2.0, height / 2.0);
        cr->translate(_transform.x, _transform.y);
        cr->scale(_transform.zoom, _transform.zoom);
        // Draw the map
        cr->set_source_rgb(1, 1, 1);
        cr->set_line_width(1 / _transform.zoom);
        _draw_map(cr, _map);
        cr->stroke();
    cr->restore();


    /* ===[ Screen-Space Overlay Drawing ]=== */
    auto const &font = get_style_context()->get_font();
    auto const dpi = get_screen()->get_resolution();
    auto font_size = select_font_from_pango(cr, font, dpi);
    auto offset = font_size / 3.0;

    cr->set_source_rgb(1, 1, 1);
    // Show name in top-left corner
    draw_text(cr, name, offset, offset);
    // Show transform coords in top-right corner
    draw_text(
        cr, std::to_string(_transform.x) + "," + std::to_string(_transform.y),
        width - offset, offset, TopRight);
    // Show transform zoom in top-right corner
    draw_text(
        cr, std::to_string(_transform.zoom),
        width - offset, font_size + 2*offset, TopRight);

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
    switch (event->direction)
    {
    case GDK_SCROLL_DOWN:
        _transform.zoom /= ZOOM_MULTIPLIER;
        break;
    case GDK_SCROLL_UP:
        _transform.zoom *= ZOOM_MULTIPLIER;
        break;
    }
    _transform.zoom = std::clamp(_transform.zoom, MIN_ZOOM, MAX_ZOOM);
    queue_draw();
    return true;
}


float Sickle::MapArea2D::_axis_horizontal(std::array<float, 3> const &vertex)
const
{
    switch (_angle)
    {
    case DrawAngle::TOP: return vertex[0]; break;
    case DrawAngle::FRONT: return vertex[1]; break;
    case DrawAngle::RIGHT: return vertex[0]; break;
    }
    throw std::runtime_error{"Bad ANGLE"};
}

float Sickle::MapArea2D::_axis_vertical(std::array<float, 3> const &vertex)
const
{
    switch (_angle)
    {
    case DrawAngle::TOP: return -vertex[1]; break;
    case DrawAngle::FRONT: return -vertex[2]; break;
    case DrawAngle::RIGHT: return -vertex[2]; break;
    }
    throw std::runtime_error{"Bad ANGLE"};
}

void Sickle::MapArea2D::_draw_brush(
    Cairo::RefPtr<Cairo::Context> const &cr, MAP::V::Brush const &brush)
const
{
    for (auto const &face : brush.faces)
    {
        auto const &first = face.vertices[0];
        cr->move_to(_axis_horizontal(first), _axis_vertical(first));
        for (auto const &vertex : face.vertices)
            cr->line_to(_axis_horizontal(vertex), _axis_vertical(vertex));
        cr->close_path();
    }
}

void Sickle::MapArea2D::_draw_map(
    Cairo::RefPtr<Cairo::Context> const &cr, MAP::V::VertexMap const &map)
const
{
    for (auto const &e : map.entities)
        for (auto const &brush : e.brushes)
            _draw_brush(cr, brush);
}
