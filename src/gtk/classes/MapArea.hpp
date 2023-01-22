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

#ifndef SE_MAPAREA_HPP
#define SE_MAPAREA_HPP

#include "editor/Editor.hpp"
#include "map/glmap.hpp"
#include "utils/Transform.hpp"
#include "utils/FreeCam.hpp"

#include <gdkmm/frameclock.h>
#include <gtkmm/glarea.h>


namespace Sickle
{
    /** Displays .map files. */
    class MapArea : public Gtk::GLArea
    {
    public:
        MapArea(Editor &ed);

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
        bool tick_callback(Glib::RefPtr<Gdk::FrameClock> const &clock);

        // Input Signals
        bool on_button_press_event(GdkEventButton *event) override;
        bool on_enter_notify_event(GdkEventCrossing *event) override;
        bool on_motion_notify_event(GdkEventMotion *event) override;
        bool on_scroll_event(GdkEventScroll *event) override;

        void on_editor_map_changed();

    private:
        Editor &_editor;
        std::shared_ptr<GLUtil::Program> _shader;
        FreeCam _camera;
        Transform _transform;
        std::unique_ptr<MAP::GLMap> _mapview{nullptr};

        struct State
        {
            gdouble pointer_prev_x, pointer_prev_y;
            gint64 last_frame_time;
            glm::vec3 move_direction;
            glm::vec2 turn_rates;
            bool gofast;
        } _state;

        // Properties
        bool _prop_wireframe;
        float _prop_shift_multiplier;
        float _prop_mouse_sensitivity;

        void _synchronize_glmap();
    };
}

#endif
