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

#ifndef _SE_MAPAREA2D_HPP
#define _SE_MAPAREA2D_HPP

#include "map/map.hpp"

#include <gdkmm/rgba.h>
#include <glibmm/property.h>
#include <glibmm/ustring.h>
#include <gtkmm/drawingarea.h>


namespace Sickle
{
    /** Displays .map files. */
    class MapArea2D : public Gtk::DrawingArea
    {
    public:
        MapArea2D();

        void set_map(MAP::Map const *map);

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
        bool on_motion_notify_event(GdkEventMotion *event) override;
        bool on_scroll_event(GdkEventScroll *event) override;

    private:
        MAP::Map const *_map;
        struct Transform2D
        {
            int x, y;
        } _transform;
        struct
        {
            int pointer_prev_x, pointer_prev_y;
        } _state;

        // Properties
        Glib::Property<Gdk::RGBA> _prop_clear_color;
        Glib::Property<int> _prop_grid_size;
        Glib::Property<Glib::ustring> _prop_name;
    };
}

#endif
