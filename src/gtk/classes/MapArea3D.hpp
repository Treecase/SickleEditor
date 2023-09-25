/**
 * MapArea3D.hpp - Sickle editor main window GLArea.
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

#ifndef SE_MAPAREA3D_HPP
#define SE_MAPAREA3D_HPP

#include "gtkglutils.hpp"

#include <core/Editor.hpp>
#include <se-lua/utils/Referenceable.hpp>
#include <utils/Transform.hpp>
#include <utils/FreeCam.hpp>
#include <utils/DebugDrawer3D.hpp>
#include <world3d/world3d.hpp>

#include <gdkmm/frameclock.h>
#include <glibmm/property.h>
#include <gtkmm/glarea.h>


namespace Sickle
{
    /** Displays .map files. */
    class MapArea3D : public Gtk::GLArea, public Lua::Referenceable
    {
    public:
        using ScreenSpacePoint = glm::vec2;
        using GLSpacePoint = glm::vec3;

        struct State
        {
            glm::vec2 pointer_prev{0, 0};
            gint64 last_frame_time{0};
            glm::vec3 move_direction{0, 0, 0};
            glm::vec2 turn_rates{0, 0};
            bool gofast{false};
            bool multiselect{false};
        };

        DebugDrawer3D debug{};

        MapArea3D(Glib::RefPtr<Editor::Editor> ed);

        Editor::BrushRef pick_brush(glm::vec2 const &P);
        GLSpacePoint screenspace_to_glspace(ScreenSpacePoint const &) const;

        auto get_editor() {return _editor;}

        auto property_camera() {return _prop_camera.get_proxy();}
        auto property_mouse_sensitivity()
        {return _prop_mouse_sensitivity.get_proxy();}
        auto property_shift_multiplier()
        {return _prop_shift_multiplier.get_proxy();}
        auto property_state() {return _prop_state.get_proxy();}
        auto property_transform() {return _prop_transform.get_proxy();}
        auto property_wireframe() {return _prop_wireframe.get_proxy();}

        // Signal handlers
        /** Where GL initialization should be done. */
        void on_realize() override;
        /** Where GL cleanup should be done. */
        void on_unrealize() override;
        /** Draw everything we need. */
        bool on_render(Glib::RefPtr<Gdk::GLContext> const &context) override;

    protected:
        bool tick_callback(Glib::RefPtr<Gdk::FrameClock> const &clock);

        bool on_enter_notify_event(GdkEventCrossing *event);

        void on_editor_map_changed();
        void on_wireframe_changed();
        void on_world3d_face_missing_texture(std::string const &what);

    private:
        struct ErrorTracker
        {
            std::unordered_set<std::string> missing_textures{};

            bool error_occurred() const
            {
                return !missing_textures.empty();
            }
        };

        Glib::RefPtr<Editor::Editor> _editor;
        std::shared_ptr<GLUtil::Program> _shader{nullptr};
        std::unique_ptr<World3D::World3D> _mapview{nullptr};
        ErrorTracker _error_tracker{};

        // Properties
        Glib::Property<FreeCam> _prop_camera;
        Glib::Property<float> _prop_mouse_sensitivity;
        Glib::Property<float> _prop_shift_multiplier;
        Glib::Property<State> _prop_state;
        Glib::Property<Transform> _prop_transform;
        Glib::Property<bool> _prop_wireframe;

        void _check_errors();
        void _synchronize_glmap();
    };
}

#endif
