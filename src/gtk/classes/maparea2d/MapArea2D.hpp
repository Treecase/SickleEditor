/**
 * MapArea2D.hpp - Sickle editor main window DrawingArea.
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

#ifndef SE_MAPAREA2D_HPP
#define SE_MAPAREA2D_HPP

#include "gbox/GrabbableBox.hpp"
#include "gbox/GrabbableBoxView.hpp"
#include "popup-menus/ToolPopupMenu.hpp"

#include <editor/core/Editor.hpp>
#include <se-lua/utils/Referenceable.hpp>

#include <cairomm/cairomm.h>
#include <gdkmm/rgba.h>
#include <giomm/simpleactiongroup.h>
#include <glibmm/property.h>
#include <glibmm/refptr.h>
#include <glibmm/ustring.h>
#include <glm/glm.hpp>
#include <gtkmm/cssprovider.h>
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
        enum Axis {X, Y, Z};

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

        /** Get the name of this area's horizontal axis. */
        Axis get_horizontal_axis_name() const;
        /** Get the name of this area's vertical axis. */
        Axis get_vertical_axis_name() const;

        auto property_draw_angle() {return _prop_draw_angle.get_proxy();}
        auto property_draw_angle() const {return _prop_draw_angle.get_proxy();}
        auto property_grid_size() {return _prop_grid_size.get_proxy();}
        auto property_grid_size() const {return _prop_grid_size.get_proxy();}
        auto property_transform() {return _prop_transform.get_proxy();}
        auto property_transform() const {return _prop_transform.get_proxy();}

        void set_draw_angle(DrawAngle angle)
        {property_draw_angle().set_value(angle);}
        auto get_draw_angle() const {return property_draw_angle().get_value();}

        void set_grid_size(int grid_size)
        {property_grid_size().set_value(grid_size);}
        auto get_grid_size() const {return property_grid_size().get_value();}

        auto get_transform() const {return property_transform().get_value();}

        auto get_editor() const {return _editor;}
        auto &get_selected_box() {return _selected_box;}
        auto const &get_selected_box() const {return _selected_box;}
        auto &get_brushbox() {return _brushbox;}
        auto const &get_brushbox() const {return _brushbox;}

    protected:
        // Signal handlers
        virtual bool on_draw(Cairo::RefPtr<Cairo::Context> const &cr) override;
        void on_editor_brushbox_changed();
        void on_editor_map_changed();
        void on_editor_maptools_changed();
        void on_editor_selection_changed();
        void on_draw_angle_changed();

        // Input Signals
        virtual bool on_button_press_event(GdkEventButton *event) override;
        virtual bool on_enter_notify_event(GdkEventCrossing *event) override;

    private:
        Editor::EditorRef _editor;
        Glib::RefPtr<Gtk::CssProvider> _css{nullptr};

        Glib::Property<DrawAngle> _prop_draw_angle;
        Glib::Property<int> _prop_grid_size;
        Glib::Property<MapArea2Dx::Transform2D> _prop_transform;

        GrabbableBox _brushbox{};
        GrabbableBoxView _brushbox_view;
        GrabbableBox _selected_box{};
        GrabbableBoxView _selected_box_view;
        std::unordered_map<std::string, ToolPopupMenu> _popup_menus{};

        void _draw_background(Cairo::RefPtr<Cairo::Context> const &cr) const;
        void _draw_grid_lines(Cairo::RefPtr<Cairo::Context> const &cr) const;
        void _draw_axes(Cairo::RefPtr<Cairo::Context> const &cr) const;

        void _draw_name_overlay(Cairo::RefPtr<Cairo::Context> const &cr) const;
        void _draw_transform_overlay(
            Cairo::RefPtr<Cairo::Context> const &cr) const;
    };
}

#endif
