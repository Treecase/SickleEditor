/**
 * MapArea2D.hpp - Sickle editor main window DrawingArea.
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

#ifndef SE_MAPAREA2D_HPP
#define SE_MAPAREA2D_HPP

#include "gbox/GrabbableBox.hpp"
#include "gbox/GrabbableBoxView.hpp"
#include "popup-menus/ToolPopupMenu.hpp"

#include <core/Editor.hpp>
#include <se-lua/utils/Referenceable.hpp>

#include <cairomm/cairomm.h>
#include <gdkmm/rgba.h>
#include <giomm/simpleactiongroup.h>
#include <glibmm/property.h>
#include <glibmm/refptr.h>
#include <glibmm/ustring.h>
#include <glm/glm.hpp>
#include <gtkmm/drawingarea.h>
#include <gtkmm/menu.h>

#include <memory>


namespace Sickle
{
    namespace MapArea2Dx
    {
        struct Transform2D
        {
            double x{0}, y{0};
            double zoom{1};
        };
    }

    /** Displays .map files. */
    class MapArea2D : public Gtk::DrawingArea, public Lua::Referenceable
    {
    public:
        using ScreenSpacePoint = glm::vec2;
        using DrawSpacePoint = glm::vec2;
        using WorldSpacePoint = glm::vec3;

        enum DrawAngle {TOP, FRONT, RIGHT};

        MapArea2D(Editor::EditorRef ed);

        /** Convert screen-space coordinates to draw-space coordinates. */
        DrawSpacePoint screenspace_to_drawspace(double x, double y) const;
        /** Convert draw-space coordinates to screen-space coordinates. */
        glm::vec2 drawspace_to_screenspace(DrawSpacePoint const &v) const;

        /** Convert draw-space coordinates to world-space coordinates. */
        WorldSpacePoint drawspace_to_worldspace(DrawSpacePoint const &v) const;
        WorldSpacePoint drawspace3_to_worldspace(glm::vec3 const &v) const;

        /** Convert world-space coordinates to draw-space coordinates. */
        DrawSpacePoint worldspace_to_drawspace(WorldSpacePoint const &v) const;
        glm::vec3 worldspace_to_drawspace3(WorldSpacePoint const &v) const;

        /** Pick an EditorBrush based on the given point. */
        Editor::BrushRef pick_brush(DrawSpacePoint point);

        auto property_clear_color() {return _prop_clear_color.get_proxy();}
        auto property_grid_size() {return _prop_grid_size.get_proxy();}
        auto property_draw_angle() {return _prop_draw_angle.get_proxy();}
        auto property_draw_angle() const {return _prop_draw_angle.get_proxy();}
        auto property_transform() {return _prop_transform.get_proxy();}
        auto property_transform() const {return _prop_transform.get_proxy();}

        void set_draw_angle(DrawAngle angle){property_draw_angle().set_value(angle);}
        auto get_draw_angle() {return property_draw_angle().get_value();};
        auto get_editor() {return _editor;}
        auto &get_selected_box() {return _selected_box;}
        auto &get_brushbox() {return _brushbox;}

    protected:
        // Signal handlers
        bool on_draw(Cairo::RefPtr<Cairo::Context> const &cr) override;
        void on_editor_brushbox_changed();
        void on_editor_map_changed();
        void on_editor_maptools_changed();
        void on_editor_selection_changed();
        void on_draw_angle_changed();

        // Input Signals
        bool on_button_press_event(GdkEventButton *event) override;
        bool on_enter_notify_event(GdkEventCrossing *event) override;

    private:
        Editor::EditorRef _editor;

        GrabbableBox _selected_box{};
        GrabbableBoxView _selected_box_view;
        GrabbableBox _brushbox{};
        GrabbableBoxView _brushbox_view;

        // Properties
        Glib::Property<Gdk::RGBA> _prop_clear_color;
        Glib::Property<int> _prop_grid_size;
        Glib::Property<DrawAngle> _prop_draw_angle;
        Glib::Property<MapArea2Dx::Transform2D> _prop_transform;

        std::unordered_map<std::string, ToolPopupMenu> _popup_menus{};

        void _draw_brush(
            Cairo::RefPtr<Cairo::Context> const &cr,
            Editor::BrushRef const &brush) const;
        void _draw_map(Cairo::RefPtr<Cairo::Context> const &cr) const;
    };
}

#endif
