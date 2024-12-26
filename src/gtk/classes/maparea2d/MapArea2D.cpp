/**
 * MapArea2D.cpp - Sickle editor main window DrawingArea.
 * Copyright (C) 2022-2024 Trevor Last
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

////////////////////////////////////////////////////////////////////////////////
// TODO
//
// Drawing the world can get pretty slow.
// Maybe draw the world to a cache surface that updates only when the world
// changes? Problem is keeping track of the changes.
//
// Should take the same caching approach with the grid.
//
// Also have to consider widget resizes.
// And possibly style changes in future although that's static for now.

#include "MapArea2D.hpp"
#include "AppWin.hpp"
#include "components/BBoxComponentFactory.hpp"
#include "components/DrawComponentFactory.hpp"

#include <gtkmm/builder.h>

#include <cassert>
#include <cmath>

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>

static Glib::ustring const DRAW_ANGLE_NAMES[] = {
    [Sickle::MapArea2D::DrawAngle::TOP] = "top",
    [Sickle::MapArea2D::DrawAngle::FRONT] = "front",
    [Sickle::MapArea2D::DrawAngle::RIGHT] = "right",
};

static Glib::ustring const AXIS_NAMES[] = {
    [Sickle::MapArea2D::Axis::X] = "x",
    [Sickle::MapArea2D::Axis::Y] = "y",
    [Sickle::MapArea2D::Axis::Z] = "z",
};

Sickle::MapArea2D::MapArea2D(Editor::EditorRef ed)
:   Glib::ObjectBase{typeid(MapArea2D)}
,   _editor{ed}
,   _css{Gtk::CssProvider::create()}
,   _prop_draw_angle{*this, "draw-angle", DrawAngle::TOP}
,   _prop_grid_size{*this, "grid-size", 32}
,   _prop_transform{*this, "transform", {}}
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
{
    set_name("maparea2d");
    set_hexpand(true);
    set_vexpand(true);
    set_size_request(320, 240);
    set_can_focus(true);

    add_events(
        Gdk::POINTER_MOTION_MASK | Gdk::KEY_PRESS_MASK | Gdk::KEY_RELEASE_MASK
        | Gdk::BUTTON_MOTION_MASK | Gdk::BUTTON_PRESS_MASK
        | Gdk::BUTTON_RELEASE_MASK | Gdk::SCROLL_MASK | Gdk::ENTER_NOTIFY_MASK);

    _css->load_from_resource(SE_GRESOURCE_PREFIX "MapArea2D.css");
    get_style_context()->add_provider(
        _css,
        GTK_STYLE_PROVIDER_PRIORITY_FALLBACK);

    _editor->signal_maptools_changed().connect(
        sigc::mem_fun(*this, &MapArea2D::on_editor_maptools_changed));
    _editor->brushbox.signal_updated().connect(
        sigc::mem_fun(*this, &MapArea2D::on_editor_brushbox_changed));
    _editor->selected.signal_updated().connect(
        sigc::mem_fun(*this, &MapArea2D::on_editor_selection_changed));
    _editor->property_map().signal_changed().connect(
        sigc::mem_fun(*this, &MapArea2D::on_editor_map_changed));
    property_grid_size().signal_changed().connect(
        sigc::mem_fun(*this, &MapArea2D::queue_draw));
    property_draw_angle().signal_changed().connect(
        sigc::mem_fun(*this, &MapArea2D::on_draw_angle_changed));
    property_draw_angle().signal_changed().connect(
        sigc::mem_fun(*this, &MapArea2D::queue_draw));
    property_transform().signal_changed().connect(
        sigc::mem_fun(*this, &MapArea2D::queue_draw));

    on_editor_maptools_changed();
}

Sickle::MapArea2D::DrawSpacePoint Sickle::MapArea2D::screenspace_to_drawspace(
    double x,
    double y) const
{
    auto const &transform = property_transform().get_value();
    auto const width = get_allocated_width();
    auto const height = get_allocated_height();
    return {
        ((x - 0.5 * width) / transform.zoom) - transform.x,
        ((y - 0.5 * height) / transform.zoom) - transform.y};
}

Sickle::MapArea2D::ScreenSpacePoint Sickle::MapArea2D::drawspace_to_screenspace(
    DrawSpacePoint const &v) const
{
    auto const &transform = property_transform().get_value();
    auto const width = get_allocated_width();
    auto const height = get_allocated_height();
    return {
        (v.x + transform.x) * transform.zoom + 0.5 * width,
        (v.y + transform.y) * transform.zoom + 0.5 * height};
}

Sickle::MapArea2D::WorldSpacePoint Sickle::MapArea2D::drawspace_to_worldspace(
    DrawSpacePoint const &v) const
{
    switch (property_draw_angle().get_value())
    {
    case DrawAngle::TOP:
        return {v.x, -v.y, 0};
        break;
    case DrawAngle::FRONT:
        return {0, v.x, -v.y};
        break;
    case DrawAngle::RIGHT:
        return {v.x, 0, -v.y};
        break;
    }
    throw std::logic_error{"bad DrawAngle value"};
}

Sickle::MapArea2D::WorldSpacePoint Sickle::MapArea2D::drawspace3_to_worldspace(
    glm::vec3 const &v) const
{
    switch (property_draw_angle().get_value())
    {
    case DrawAngle::TOP:
        return {v.x, -v.y, v.z};
        break;
    case DrawAngle::FRONT:
        return {v.z, v.x, -v.y};
        break;
    case DrawAngle::RIGHT:
        return {v.x, v.z, -v.y};
        break;
    }
    throw std::logic_error{"bad DrawAngle value"};
}

Sickle::MapArea2D::DrawSpacePoint Sickle::MapArea2D::worldspace_to_drawspace(
    WorldSpacePoint const &v) const
{
    return glm::vec2{worldspace_to_drawspace3(v)};
}

glm::vec3 Sickle::MapArea2D::worldspace_to_drawspace3(
    WorldSpacePoint const &v) const
{
    switch (property_draw_angle().get_value())
    {
    case DrawAngle::TOP:
        return {v.x, -v.y, v.z};
        break;
    case DrawAngle::FRONT:
        return {v.y, -v.z, v.x};
        break;
    case DrawAngle::RIGHT:
        return {v.x, -v.z, v.y};
        break;
    }
    throw std::logic_error{"bad DrawAngle value"};
}

Sickle::Editor::EditorObjectRef Sickle::MapArea2D::pick_object(
    DrawSpacePoint point)
{
    Editor::EditorObjectRef picked{nullptr};
    BBox2 pbbox{};

    auto const objects = _editor->get_map()->children_recursive_breadth_first();
    for (auto const &obj : objects)
    {
        for (auto const &c : obj->get_components())
        {
            auto const bbox_c
                = std::dynamic_pointer_cast<World2D::BBoxComponent>(c);
            if (!bbox_c)
            {
                continue;
            }

            auto const bbox = bbox_c->bbox(*this);
            if (bbox.contains(point))
            {
                // If the point is inside multiple bboxes, we pick the one with
                // the smallest volume. Could use other metrics, but this seems
                // logical enough.
                if (bbox.volume() < pbbox.volume())
                {
                    picked = obj;
                    pbbox = bbox;
                }
            }
        }
    }
    return picked;
}

Sickle::MapArea2D::Axis Sickle::MapArea2D::get_horizontal_axis_name() const
{
    switch (property_draw_angle().get_value())
    {
    case DrawAngle::TOP:
        return Axis::X;
        break;
    case DrawAngle::FRONT:
        return Axis::Y;
        break;
    case DrawAngle::RIGHT:
        return Axis::X;
        break;
    }
    throw std::logic_error{"bad DrawAngle value"};
}

Sickle::MapArea2D::Axis Sickle::MapArea2D::get_vertical_axis_name() const
{
    switch (property_draw_angle().get_value())
    {
    case DrawAngle::TOP:
        return Axis::Y;
        break;
    case DrawAngle::FRONT:
        return Axis::Z;
        break;
    case DrawAngle::RIGHT:
        return Axis::Z;
        break;
    }
    throw std::logic_error{"bad DrawAngle value"};
}

bool Sickle::MapArea2D::on_draw(Cairo::RefPtr<Cairo::Context> const &cr)
{
    auto const style = get_style_context();
    auto const transform = property_transform().get_value();
    auto const width = get_allocated_width();
    auto const height = get_allocated_height();

    style->set_state(get_state_flags());

    /* ===[ Grid and Axes ]=== */
    _draw_background(cr);
    _draw_grid_lines(cr);
    _draw_axes(cr);

    /* ===[ World-Space Drawing ]=== */
    {
        cr->save();
        cr->set_antialias(Cairo::ANTIALIAS_NONE);
        cr->translate(0.5 * width, 0.5 * height);
        cr->translate(
            transform.x * transform.zoom,
            transform.y * transform.zoom);
        cr->scale(transform.zoom, transform.zoom);
        auto const pixel = 1.0 / transform.zoom;

        auto const execute_draw_components
            = [this, cr](Editor::EditorObjectRef const &obj) -> void
        {
            for (auto const &c : obj->get_components())
            {
                auto const dc
                    = std::dynamic_pointer_cast<World2D::DrawComponent>(c);
                if (dc)
                {
                    dc->draw(cr, *this);
                }
            }
        };

        // Draw the world.
        cr->set_line_width(pixel);
        _editor->get_map()->foreach(
            [&execute_draw_components](auto obj)
            {
                if (!obj->is_selected())
                {
                    execute_draw_components(obj);
                }
            });

        // Draw selected objects on top.
        _editor->get_map()->foreach(
            [&execute_draw_components](auto obj)
            {
                if (obj->is_selected())
                {
                    execute_draw_components(obj);
                }
            });

        // Selected brushes grab handles.
        _selected_box.unit = pixel;
        _selected_box_view.draw(cr, _selected_box);

        // Draw the brushbox.
        if (_editor->brushbox.p1() != _editor->brushbox.p2())
        {
            _brushbox.unit = pixel;
            _brushbox_view.draw(cr, _brushbox);
        }

        cr->restore();
    }

    /* ===[ Screen-Space Overlay Drawing ]=== */
    _draw_name_overlay(cr);
    _draw_transform_overlay(cr);

    return true;
}

void Sickle::MapArea2D::on_editor_brushbox_changed()
{
    auto const p1 = worldspace_to_drawspace(_editor->brushbox.p1());
    auto const p2 = worldspace_to_drawspace(_editor->brushbox.p2());
    _brushbox.set_box(BBox2{p1, p2});
    queue_draw();
}

void Sickle::MapArea2D::on_editor_map_changed()
{
    static auto const on_brush_added
        = [](Editor::EditorObjectRef const &obj) -> void
    {
        obj->add_component(World2D::DrawComponentFactory{}.construct(obj));
        obj->add_component(World2D::BBoxComponentFactory{}.construct(obj));
    };
    static auto const on_entity_added
        = [](Editor::EditorObjectRef const &obj) -> void
    {
        obj->add_component(World2D::DrawComponentFactory{}.construct(obj));
        obj->add_component(World2D::BBoxComponentFactory{}.construct(obj));

        sigc::connection conn
            = obj->signal_child_added().connect(on_brush_added);
        obj->signal_removed().connect(
            [conn]() mutable -> void { conn.disconnect(); });
        obj->foreach_direct(on_brush_added);
    };

    _editor->brushbox.signal_updated().connect(
        sigc::mem_fun(*this, &MapArea2D::on_editor_brushbox_changed));
    _editor->selected.signal_updated().connect(
        sigc::mem_fun(*this, &MapArea2D::on_editor_selection_changed));

    auto const world = _editor->get_map();
    sigc::connection conn
        = world->signal_child_added().connect(on_entity_added);
    world->signal_removed().connect(
        [conn]() mutable -> void { conn.disconnect(); });
    world->foreach_direct(on_entity_added);

    property_transform().reset_value();
    queue_draw();
}

void Sickle::MapArea2D::on_editor_maptools_changed()
{
    for (auto const &p : _editor->get_maptools())
    {
        auto const p2 = _popup_menus.emplace(p.first, p.second);
        if (p2.second)
        {
            auto &menu = p2.first->second;
            menu.set_editor(_editor);
            menu.attach_to_widget(*this);
        }
    }
}

void Sickle::MapArea2D::on_editor_selection_changed()
{
    BBox2 selection_bounds{};
    auto const brushes = _editor->selected.get_all_of_type<Editor::Brush>();
    for (auto const &brush : brushes)
    {
        for (auto const &face : brush->faces())
        {
            for (auto const &vertex : face->get_vertices())
            {
                selection_bounds.add(worldspace_to_drawspace(vertex));
            }
        }
    }
    _selected_box.set_box(selection_bounds);
    queue_draw();
}

void Sickle::MapArea2D::on_draw_angle_changed()
{
    queue_draw();
}

bool Sickle::MapArea2D::on_button_press_event(GdkEventButton *event)
{
    if (event->button == GDK_BUTTON_SECONDARY)
    {
        auto const &tool = _editor->get_maptool();
        auto const name = tool.name();
        if (_popup_menus.count(name) != 0)
        {
            auto &menu = _popup_menus.at(name);
            if (menu.should_popup())
            {
                menu.popup_at_pointer(nullptr);
                return true;
            }
        }
    }
    return false;
}

bool Sickle::MapArea2D::on_enter_notify_event(GdkEventCrossing *event)
{
    grab_focus();
    return true;
}

void Sickle::MapArea2D::_draw_background(
    Cairo::RefPtr<Cairo::Context> const &cr) const
{
    auto const style = get_style_context();
    auto const clear_color = style->get_background_color(style->get_state());
    cr->set_source_rgb(
        clear_color.get_red(),
        clear_color.get_green(),
        clear_color.get_blue());
    cr->paint();
}

void Sickle::MapArea2D::_draw_grid_lines(
    Cairo::RefPtr<Cairo::Context> const &cr) const
{
    auto const width = get_allocated_width();
    auto const height = get_allocated_height();
    auto const transform = property_transform().get_value();
    auto const grid_size = property_grid_size().get_value() * transform.zoom;

    auto const style = get_style_context();
    style->context_save();
    style->add_class("grid");

    auto const grid_color = style->get_color(style->get_state());
    cr->set_source_rgb(
        grid_color.get_red(),
        grid_color.get_green(),
        grid_color.get_blue());

    auto const half_w = 0.5 * width;
    auto const half_h = 0.5 * height;
    auto const dx = std::fmod(transform.x * transform.zoom, grid_size);
    auto const dy = std::fmod(transform.y * transform.zoom, grid_size);
    int const count_x = std::ceil((0.5 * width) / grid_size);
    int const count_y = std::ceil((0.5 * height) / grid_size);
    for (int i = 0; i <= count_x; ++i)
    {
        cr->move_to(half_w + i * grid_size + dx, 0);
        cr->rel_line_to(0, height);
        cr->move_to(half_w - i * grid_size + dx, 0);
        cr->rel_line_to(0, height);
    }
    for (int i = 0; i <= count_y; ++i)
    {
        cr->move_to(0, half_h + i * grid_size + dy);
        cr->rel_line_to(width, 0);
        cr->move_to(0, half_h - i * grid_size + dy);
        cr->rel_line_to(width, 0);
    }

    cr->stroke();
    style->context_restore();
}

void Sickle::MapArea2D::_draw_axes(
    Cairo::RefPtr<Cairo::Context> const &cr) const
{
    auto const h_axis = AXIS_NAMES[get_horizontal_axis_name()];
    auto const v_axis = AXIS_NAMES[get_vertical_axis_name()];

    auto const width = get_allocated_width();
    auto const height = get_allocated_height();
    auto const transform = property_transform().get_value();
    auto const style = get_style_context();

    // Draw vertical axis.
    style->context_save();
    style->add_class("grid");
    style->add_class(v_axis);
    auto const v_axis_color = style->get_color(style->get_state());
    cr->set_source_rgb(
        v_axis_color.get_red(),
        v_axis_color.get_green(),
        v_axis_color.get_blue());
    cr->move_to(0.5 * width + transform.x * transform.zoom, 0);
    cr->rel_line_to(0, height);
    cr->stroke();
    style->context_restore();

    // Draw horizontal axis.
    style->context_save();
    style->add_class("grid");
    style->add_class(h_axis);
    auto const h_axis_color = style->get_color(style->get_state());
    cr->set_source_rgb(
        h_axis_color.get_red(),
        h_axis_color.get_green(),
        h_axis_color.get_blue());
    cr->move_to(0, 0.5 * height + transform.y * transform.zoom);
    cr->rel_line_to(width, 0);
    cr->stroke();
    style->context_restore();
}

void Sickle::MapArea2D::_draw_name_overlay(
    Cairo::RefPtr<Cairo::Context> const &cr) const
{
    auto const style = get_style_context();
    auto const font = style->get_font();
    auto const fg = style->get_color(style->get_state());
    auto const margin = style->get_margin(style->get_state());

    cr->save();
    cr->set_source_rgb(fg.get_red(), fg.get_green(), fg.get_blue());
    cr->move_to(margin.get_left(), margin.get_top());

    auto const layout = Pango::Layout::create(cr);
    layout->set_text(
        DRAW_ANGLE_NAMES[get_draw_angle()] + " ("
        + AXIS_NAMES[get_horizontal_axis_name()] + "/"
        + AXIS_NAMES[get_vertical_axis_name()] + ")");
    layout->set_font_description(font);
    layout->show_in_cairo_context(cr);

    cr->restore();
}

void Sickle::MapArea2D::_draw_transform_overlay(
    Cairo::RefPtr<Cairo::Context> const &cr) const
{
    auto const style = get_style_context();
    auto const font = style->get_font();
    auto const fg = style->get_color(style->get_state());
    auto const margin = style->get_margin(style->get_state());

    auto const transform = get_transform();
    auto const width = get_allocated_width();

    std::stringstream text{};
    text << std::fixed << std::setprecision(1);
    text << transform.x << ", " << transform.y << '\n';
    text << std::setprecision(4);
    text << transform.zoom;

    cr->save();
    cr->set_source_rgb(fg.get_red(), fg.get_green(), fg.get_blue());

    auto const layout = Pango::Layout::create(cr);
    layout->set_text(text.str());
    layout->set_font_description(font);
    layout->set_alignment(Pango::Alignment::ALIGN_RIGHT);

    Pango::Rectangle extents_logical{}, extents_ink{};
    layout->get_pixel_extents(extents_ink, extents_logical);

    cr->move_to(
        width - extents_logical.get_rbearing() - margin.get_right(),
        margin.get_top());
    layout->show_in_cairo_context(cr);

    cr->restore();
}
