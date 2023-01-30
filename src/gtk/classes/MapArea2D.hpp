/**
 * MapArea2D.hpp - Sickle editor main window DrawingArea.
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

#ifndef SE_MAPAREA2D_HPP
#define SE_MAPAREA2D_HPP

#include "editor/Editor.hpp"

#include <gdkmm/rgba.h>
#include <glibmm/property.h>
#include <glibmm/ustring.h>
#include <gtkmm/drawingarea.h>
#include <cairomm/cairomm.h>


namespace Sickle
{
    /** Displays .map files. */
    class MapArea2D : public Gtk::DrawingArea
    {
    public:
        using DrawSpacePoint = glm::vec2;

        enum DrawAngle {TOP, FRONT, RIGHT};
        MapArea2D(Editor &ed);

        void set_draw_angle(DrawAngle angle);

        auto property_clear_color() {return _prop_clear_color.get_proxy();}
        auto property_grid_size() {return _prop_grid_size.get_proxy();}
        auto property_name() {return _prop_name.get_proxy();}

        // Signal handlers
        bool on_draw(Cairo::RefPtr<Cairo::Context> const &cr) override;

        // Input Signals
        bool on_key_press_event(GdkEventKey *event) override;
        bool on_key_release_event(GdkEventKey *event) override;

    protected:
        // Input Signals
        bool on_button_press_event(GdkEventButton *event) override;
        bool on_button_release_event(GdkEventButton *event) override;
        bool on_enter_notify_event(GdkEventCrossing *event) override;
        bool on_motion_notify_event(GdkEventMotion *event) override;
        bool on_scroll_event(GdkEventScroll *event) override;

        void on_editor_map_changed();

    private:
        Editor &_editor;
        DrawAngle _angle{TOP};
        struct Transform2D
        {
            double x{0}, y{0};
            double zoom{1};
        } _transform{};
        struct State
        {
            glm::vec2 pointer_prev{0, 0};
            bool dragged{false};
            bool multiselect{false};
        } _state{};

        // Properties
        Glib::Property<Gdk::RGBA> _prop_clear_color;
        Glib::Property<int> _prop_grid_size;
        Glib::Property<Glib::ustring> _prop_name;

        /** Convert screen-space coordinates to draw-space coordinates. */
        DrawSpacePoint _screenspace_to_drawspace(double x, double y) const;
        /** Convert draw-space coordinates to world-space coordinates. */
        MAP::Vertex _drawspace_to_worldspace(DrawSpacePoint const &v) const;
        /** Convert world-space coordinates to draw-space coordinates. */
        DrawSpacePoint _worldspace_to_drawspace(MAP::Vertex const &v) const;

        void _draw_brush(
            Cairo::RefPtr<Cairo::Context> const &cr,
            EditorBrush const &brush) const;
        void _draw_map(Cairo::RefPtr<Cairo::Context> const &cr) const;
        EditorBrush *pick_brush(DrawSpacePoint point);
    };
}

#endif
