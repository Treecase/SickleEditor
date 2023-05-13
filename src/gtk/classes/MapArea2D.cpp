/**
 * MapArea2D.cpp - Sickle editor main window DrawingArea.
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

#include "MapArea2D.hpp"
#include "MapArea2D_Lua.hpp"
#include "AppWin.hpp"

#include <cmath>
#include <algorithm>

// Test function for the Xspace_to_Yspace methods.
void _test_space_conversions(Sickle::MapArea2D const &maparea);


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
    auto const half_w = 0.5 * width;
    auto const half_h = 0.5 * height;
    auto const dx = std::fmod(_transform_x, grid_size);
    auto const dy = std::fmod(_transform_y, grid_size);
    int const count_x = std::ceil((0.5 * width) / grid_size);
    int const count_y = std::ceil((0.5 * height) / grid_size);
    for (int i = 0; i <= count_x; ++i)
    {
        cr->move_to(half_w + i*grid_size + dx, 0);
        cr->rel_line_to(0, height);
        cr->move_to(half_w - i*grid_size + dx, 0);
        cr->rel_line_to(0, height);
    }
    for (int i = 0; i <= count_y; ++i)
    {
        cr->move_to(0, half_h + i*grid_size + dy);
        cr->rel_line_to(width, 0);
        cr->move_to(0, half_h - i*grid_size + dy);
        cr->rel_line_to(width, 0);
    }
}

/** Draw main axes. */
void draw_axes(
    Cairo::RefPtr<Cairo::Context> const &cr, double width, double height,
    double _transform_x, double _transform_y)
{
    cr->move_to(0.5 * width + _transform_x, 0);
    cr->rel_line_to(0, height);
    cr->move_to(0, 0.5 * height + _transform_y);
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
Sickle::MapArea2D::MapArea2D(Editor::Editor &ed)
:   Glib::ObjectBase{typeid(MapArea2D)}
,   Gtk::DrawingArea{}
,   _editor{ed}
,   _prop_clear_color{*this, "clear-color", {}}
,   _prop_grid_size{*this, "grid-size", 32}
,   _prop_draw_angle{*this, "draw-angle", DrawAngle::TOP}
,   _prop_transform{*this, "transform", {}}
,   _selected_box_view{
        std::make_shared<BBox2ViewCustom>(
            [](auto cr, auto box, auto unit){
                cr->set_source_rgb(1, 0, 0);
                cr->set_line_width(unit);
                cr->set_dash(std::vector<double>{4*unit, 4*unit}, 0);},
            [](auto cr, auto box, auto unit){cr->stroke();}
        ),
        std::make_shared<BBox2ViewCustom>(
            [](auto cr, auto box, auto unit){
                cr->set_source_rgb(1, 1, 1);
                cr->set_line_width(unit);},
            [](auto cr, auto box, auto unit){cr->fill();}
        )
    }
,   _brushbox_view{
        std::make_shared<BBox2ViewCustom>(
            [](auto cr, auto box, auto unit){
                cr->set_source_rgb(1, 1, 1);
                cr->set_line_width(unit);
                cr->set_dash(std::vector<double>{4*unit, 4*unit}, 0);},
            [](auto cr, auto box, auto unit){cr->stroke();}
        ),
        std::make_shared<BBox2ViewCustom>(
            [](auto cr, auto box, auto unit){
                cr->set_source_rgb(1, 1, 1);
                cr->set_line_width(unit);},
            [](auto cr, auto box, auto unit){cr->fill();}
        )
    }
{
    set_hexpand(true);
    set_vexpand(true);
    set_size_request(320, 240);
    set_can_focus(true);

    _editor.brushbox.signal_updated().connect(
        sigc::mem_fun(*this, &MapArea2D::on_editor_brushbox_changed));
    _editor.selected.signal_updated().connect(
        sigc::mem_fun(*this, &MapArea2D::on_editor_selection_changed));
    _editor.signal_map_changed().connect(
        sigc::mem_fun(*this, &MapArea2D::on_editor_map_changed));
    property_grid_size().signal_changed().connect(
        sigc::mem_fun(*this, &MapArea2D::queue_draw));
    property_draw_angle().signal_changed().connect(
        sigc::mem_fun(*this, &MapArea2D::on_draw_angle_changed));
    property_draw_angle().signal_changed().connect(
        sigc::mem_fun(*this, &MapArea2D::queue_draw));
    property_transform().signal_changed().connect(
        sigc::mem_fun(*this, &MapArea2D::queue_draw));

    add_events(
        Gdk::POINTER_MOTION_MASK
        | Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK
        | Gdk::BUTTON_MOTION_MASK
        | Gdk::BUTTON_PRESS_MASK | Gdk::BUTTON_RELEASE_MASK
        | Gdk::SCROLL_MASK
        | Gdk::ENTER_NOTIFY_MASK);
}

Sickle::MapArea2D::DrawSpacePoint
Sickle::MapArea2D::screenspace_to_drawspace(double x, double y) const
{
    auto const &transform = property_transform().get_value();
    auto const width = get_allocated_width();
    auto const height = get_allocated_height();
    return {
        ((x - 0.5 * width ) / transform.zoom) - transform.x,
        ((y - 0.5 * height) / transform.zoom) - transform.y
    };
}

Sickle::MapArea2D::ScreenSpacePoint
Sickle::MapArea2D::drawspace_to_screenspace(DrawSpacePoint const &v) const
{
    auto const &transform = property_transform().get_value();
    auto const width = get_allocated_width();
    auto const height = get_allocated_height();
    return {
        (v.x + transform.x) * transform.zoom + 0.5 * width,
        (v.y + transform.y) * transform.zoom + 0.5 * height
    };
}

Sickle::MapArea2D::WorldSpacePoint
Sickle::MapArea2D::drawspace_to_worldspace(DrawSpacePoint const &v) const
{
    switch (property_draw_angle().get_value())
    {
    case DrawAngle::TOP  : return {v.x, -v.y, 0}; break;
    case DrawAngle::FRONT: return {0, v.x, -v.y}; break;
    case DrawAngle::RIGHT: return {v.x, 0, -v.y}; break;
    }
    throw std::logic_error{"bad DrawAngle value"};
}

Sickle::MapArea2D::WorldSpacePoint
Sickle::MapArea2D::drawspace3_to_worldspace(glm::vec3 const &v) const
{
    switch (property_draw_angle().get_value())
    {
    case DrawAngle::TOP  : return {v.x, -v.y, v.z}; break;
    case DrawAngle::FRONT: return {v.z, v.x, -v.y}; break;
    case DrawAngle::RIGHT: return {v.x, v.z, -v.y}; break;
    }
    throw std::logic_error{"bad DrawAngle value"};
}

Sickle::MapArea2D::DrawSpacePoint
Sickle::MapArea2D::worldspace_to_drawspace(WorldSpacePoint const &v) const
{
    return glm::vec2{worldspace_to_drawspace3(v)};
}

glm::vec3
Sickle::MapArea2D::worldspace_to_drawspace3(WorldSpacePoint const &v) const
{
    switch (property_draw_angle().get_value())
    {
    case DrawAngle::TOP  : return {v.x, -v.y, v.z}; break;
    case DrawAngle::FRONT: return {v.y, -v.z, v.x}; break;
    case DrawAngle::RIGHT: return {v.x, -v.z, v.y}; break;
    }
    throw std::logic_error{"bad DrawAngle value"};
}

std::shared_ptr<Sickle::Editor::Brush>
Sickle::MapArea2D::pick_brush(DrawSpacePoint point)
{
    std::shared_ptr<Editor::Brush> picked{nullptr};
    BBox2 pbbox{};

    for (auto const &entity : _editor.get_map().entities)
    {
        for (auto const &brush : entity.brushes)
        {
            BBox2 bbox{};
            for (auto const &face : brush->faces)
                for (auto const &vertex : face->vertices)
                    bbox.add(worldspace_to_drawspace(vertex));

            if (bbox.contains(point))
            {
                // If the point is inside multiple bboxes, we pick the one with
                // the smallest volume. Could use other metrics, but this seems
                // logical enough.
                if (bbox.volume() < pbbox.volume())
                {
                    picked = brush;
                    pbbox = bbox;
                }
            }
        }
    }
    return picked;
}

bool Sickle::MapArea2D::on_draw(Cairo::RefPtr<Cairo::Context> const &cr)
{
#if !NDEBUG
    _test_space_conversions(*this);
#endif

    auto const grid_size = property_grid_size().get_value();
    auto const name = property_name().get_value();
    auto const &clear_color = property_clear_color().get_value();
    auto const &_transform = property_transform().get_value();

    auto const width = get_allocated_width();
    auto const height = get_allocated_height();

    // Clear background
    cr->set_source_rgb(
        clear_color.get_red(), clear_color.get_green(), clear_color.get_blue());
    cr->paint();


    /* ===[ Grid ]=== */
    // Grid squares
    cr->set_source_rgb(0.3, 0.3, 0.3);
    draw_grid(cr,
        width, height,
        grid_size * _transform.zoom,
        _transform.x * _transform.zoom, _transform.y * _transform.zoom);
    cr->stroke();

    // x/y axes
    cr->set_source_rgb(0.5, 0.5, 0.5);
    draw_axes(cr,
        width, height,
        _transform.x * _transform.zoom, _transform.y * _transform.zoom);
    cr->stroke();


    /* ===[ World-Space Drawing ]=== */
    {
    cr->save();
        cr->set_antialias(Cairo::ANTIALIAS_NONE);
        cr->translate(0.5 * width, 0.5 * height);
        cr->translate(
            _transform.x * _transform.zoom,
            _transform.y * _transform.zoom);
        cr->scale(_transform.zoom, _transform.zoom);
        auto pixel = 1.0 / _transform.zoom;

        // Draw all brushes.
        cr->set_source_rgb(1, 1, 1);
        cr->set_line_width(pixel);
        _draw_map(cr);
        cr->stroke();

        // Draw selected brushes.
        for (auto const &e : _editor.get_map().entities)
        {
            for (auto const &b : e.brushes)
            {
                if (b->is_selected)
                {
                    cr->set_source_rgb(1, 0, 0);
                    cr->set_line_width(pixel);
                    _draw_brush(cr, b);
                    cr->stroke();
                }
            }
        }

        // Selected brushes grab handles.
        _selected_box.unit = pixel;
        _selected_box_view.draw(cr, _selected_box);

        // Draw the brushbox.
        if (_editor.brushbox.p1() != _editor.brushbox.p2())
        {
            _brushbox.unit = pixel;
            _brushbox_view.draw(cr, _brushbox);
        }

    cr->restore();
    }


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

void Sickle::MapArea2D::on_editor_brushbox_changed()
{
    auto const p1 = worldspace_to_drawspace(_editor.brushbox.p1());
    auto const p2 = worldspace_to_drawspace(_editor.brushbox.p2());
    _brushbox.set_box(BBox2{p1, p2});
    queue_draw();
}

void Sickle::MapArea2D::on_editor_map_changed()
{
    _editor.brushbox.signal_updated().connect(
        sigc::mem_fun(*this, &MapArea2D::on_editor_brushbox_changed));
    _editor.selected.signal_updated().connect(
        sigc::mem_fun(*this, &MapArea2D::on_editor_selection_changed));
    property_transform().reset_value();
    queue_draw();
}

void Sickle::MapArea2D::on_editor_selection_changed()
{
    BBox2 selection_bounds{};
    for (auto const &brush : _editor.selected)
        for (auto const &face : brush->faces)
            for (auto const &vertex : face->vertices)
                selection_bounds.add(worldspace_to_drawspace(vertex));
    _selected_box.set_box(selection_bounds);
    queue_draw();
}

void Sickle::MapArea2D::on_draw_angle_changed()
{
    switch (property_draw_angle().get_value())
    {
    case DrawAngle::TOP: property_name() = "top (x/y)"; break;
    case DrawAngle::FRONT: property_name() = "front (y/z)"; break;
    case DrawAngle::RIGHT: property_name() = "right (x/z)"; break;
    }
    queue_draw();
}

bool Sickle::MapArea2D::on_enter_notify_event(GdkEventCrossing *event)
{
    grab_focus();
    return true;
}


void Sickle::MapArea2D::_draw_brush(
    Cairo::RefPtr<Cairo::Context> const &cr,
    std::shared_ptr<Editor::Brush> const &brush)
const
{
    for (auto const &face : brush->faces)
    {
        if (face->vertices.empty())
            continue;
        auto const p0 = worldspace_to_drawspace(face->vertices[0]);
        cr->move_to(p0.x, p0.y);
        for (auto const &vertex : face->vertices)
        {
            auto const p = worldspace_to_drawspace(vertex);
            cr->line_to(p.x, p.y);
        }
        cr->close_path();
    }
}

void Sickle::MapArea2D::_draw_map(Cairo::RefPtr<Cairo::Context> const &cr) const
{
    for (auto const &e : _editor.get_map().entities)
        for (auto const &brush : e.brushes)
            _draw_brush(cr, brush);
}



void _test_space_conversions(Sickle::MapArea2D const &maparea)
{
    static constexpr float EPSILON = 0.001f;

    glm::vec2 ss{
        (float)rand() / (float)RAND_MAX,
        (float)rand() / (float)RAND_MAX};
    auto ss2ds = maparea.screenspace_to_drawspace(ss.x, ss.y);
    auto ss2ds2ss = maparea.drawspace_to_screenspace(ss2ds);
    assert(glm::all(glm::epsilonEqual(ss2ds2ss, ss, EPSILON)));

    auto ds = ss2ds;
    auto ds2ws = maparea.drawspace_to_worldspace(ds);
    auto ds2ws2ds = maparea.worldspace_to_drawspace(ds2ws);
    assert(glm::all(glm::epsilonEqual(ds2ws2ds, ds, EPSILON)));
}
