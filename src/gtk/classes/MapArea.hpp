/**
 * MapArea.hpp - Sickle editor main window GLArea.
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

#ifndef _SE_MAPAREA_HPP
#define _SE_MAPAREA_HPP

#include "utils/Transform.hpp"
#include "utils/FreeCam.hpp"
#include "map/load_map.hpp"
#include "map/map2gl.hpp"
#include "wad/load_wad.hpp"
#include "wad/lumps.hpp"
#include "wad/TextureManager.hpp"

#include <gtkmm.h>

#include <filesystem>


namespace Sickle
{
    /** Displays .map files. */
    class MapArea : public Gtk::GLArea
    {
    public:
        MapArea(BaseObjectType *cobject, Glib::RefPtr<Gtk::Builder> const &refBuilder);

        void set_map(MAP::Map const *map);

        // Signal handlers
        /** Where GL initialization should be done. */
        void on_realize() override;
        /** Where GL cleanup should be done. */
        void on_unrealize() override;
        /** Draw everything we need. */
        bool on_render(Glib::RefPtr<Gdk::GLContext> const &context) override;

        // Input Signals
        bool on_key_press_event(GdkEventKey *event) override;
        bool on_key_release_event(GdkEventKey *event) override;

    protected:
        Glib::RefPtr<Gtk::Builder> m_refBuilder;

        bool tick_callback(Glib::RefPtr<Gdk::FrameClock> const &clock);

        // Input Signals
        bool on_button_press_event(GdkEventButton *event) override;
        bool on_motion_notify_event(GdkEventMotion *event) override;
        bool on_scroll_event(GdkEventScroll *event) override;

    private:
        std::shared_ptr<GLUtil::Program> _shader;
        FreeCam _camera;
        MAP::GLMap _glmap;
        Transform _transform;

        struct State
        {
            gdouble pointer_prev_x, pointer_prev_y;
            gint64 last_frame_time;
            glm::vec3 move_direction;
            bool gofast;
        } _state;

        // Properties
        bool _prop_wireframe;
        float _prop_shift_multiplier;
        float _prop_mouse_sensitivity;
    };
}

#endif
