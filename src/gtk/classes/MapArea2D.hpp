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

#include "map/map.hpp"

#include <gdkmm/rgba.h>
#include <glibmm/property.h>
#include <glibmm/ustring.h>
#include <gtkmm/drawingarea.h>
#include <cairomm/cairomm.h>

#include <tuple>


namespace Sickle
{
    /** Displays .map files. */
    class MapArea2D : public Gtk::DrawingArea
    {
    public:
        enum DrawAngle {TOP, FRONT, RIGHT};
        MapArea2D();

        void set_map(MAP::Map const *map);
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
        bool on_enter_notify_event(GdkEventCrossing *event) override;
        bool on_motion_notify_event(GdkEventMotion *event) override;
        bool on_scroll_event(GdkEventScroll *event) override;

    private:
        MAP::Map _map;
        DrawAngle _angle{TOP};
        struct Transform2D
        {
            double x, y;
            double zoom;
        } _transform;
        struct
        {
            struct BBox {int x1, y1, z1; int x2, y2, z2;};
            int pointer_prev_x, pointer_prev_y;
            // FIXME: SELECTION and SELECTED are easily confused. Should be renamed to something better.
            BBox selection; // TODO: This is shared b/t all MapAreas
            MAP::Brush *selected; // TODO: This is shared b/t all MapAreas
        } _state;

        // Properties
        Glib::Property<Gdk::RGBA> _prop_clear_color;
        Glib::Property<int> _prop_grid_size;
        Glib::Property<Glib::ustring> _prop_name;

        /** Convert screen-space coordinates to world-space coordinates. */
        template<typename T>
        std::tuple<T, T> _screenspace_to_worldspace(T x, T y) const
        {
            auto const width = get_allocated_width();
            auto const height = get_allocated_height();
            return {
                ((x - 0.5 * width ) - _transform.x) / _transform.zoom,
                ((y - 0.5 * height) - _transform.y) / _transform.zoom};
        }

        /** Convert world-space coordinates to draw-space coordinates. */
        std::tuple<double, double> _worldspace_to_drawspace(
            MAP::Vertex const &v) const;

        void _draw_brush(
            Cairo::RefPtr<Cairo::Context> const &cr,
            MAP::Brush const &brush) const;
        void _draw_map(
            Cairo::RefPtr<Cairo::Context> const &cr,
            MAP::Map const &map) const;
    };
}

#endif
