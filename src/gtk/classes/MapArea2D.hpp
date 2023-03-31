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

#include "editor/Editor.hpp"
#include "utils/BoundingBox.hpp"

#include <gdkmm/rgba.h>
#include <glibmm/property.h>
#include <glibmm/ustring.h>
#include <gtkmm/drawingarea.h>
#include <cairomm/cairomm.h>


namespace Sickle
{
    namespace MapArea2Dx
    {
        struct Transform2D
        {
            double x{0}, y{0};
            double zoom{1};
        };

        struct State
        {
            glm::vec2 pointer_prev{0, 0};
            bool multiselect{false};
        };
    }

    class GrabbableBox
    {
        // in screenspace
        glm::vec2 const grab_size{8};
        // in drawspace
        BBox2 box{};
        // convert from screenspace to drawspace
        float _unit{1.0};
        void _draw_bbox(
            Cairo::RefPtr<Cairo::Context> const &cr, BBox2 const &box)
        {
            auto const width = box.max.x - box.min.x;
            auto const height = box.max.y - box.min.y;
            cr->rectangle(box.min.x, box.min.y, width, height);
        }
    public:
        /** Specifies a grabbable element of the GrabBox. */
        enum Area
        {
            NONE,
            BOX,
            N, NE, E, SE, S, SW, W, NW
        };

        /** Set main bounding-box. */
        void set_box(BBox2 const &other) {box = other;}
        /** Get main bounding-box. */
        auto &get_box() const {return box;}

        /** Set screenspace-to-drawspace multiplier. */
        void set_unit(double unit)
        {
            _unit = unit;
        }

        /**
         * Check if a point is inside any of the grabbable areas of the
         * GrabBox.
         */
        Area check_point(glm::vec2 const &point)
        {
            auto const width = box.max.x - box.min.x;
            auto const height = box.max.y - box.min.y;
            if (width == 0 && height == 0)
                return Area::NONE;

            // North-west
            BBox2 nw{box.min, box.min - grab_size * _unit};
            // North-east
            BBox2 ne{
                {box.max.x, box.min.y},
                glm::vec2{box.max.x, box.min.y}
                    + grab_size * glm::vec2{1, -1} * _unit
            };
            // South-west
            BBox2 sw{
                {box.min.x, box.max.y},
                glm::vec2{box.min.x, box.max.y}
                    + grab_size * glm::vec2{-1, 1} * _unit
            };
            // South-east
            BBox2 se{
                box.max,
                box.max + grab_size * _unit
            };

            // North
            BBox2 n{
                box.min
                    + glm::vec2{width / 2.0, 0}
                    + grab_size * glm::vec2{-0.5, -1} * _unit,
                box.min
                    + glm::vec2{width / 2.0, 0}
                    + grab_size * glm::vec2{0.5, 0} * _unit,
            };
            // East
            BBox2 e{
                box.min
                    + glm::vec2{width, height / 2.0}
                    + grab_size * glm::vec2{1, -0.5} * _unit,
                box.min
                    + glm::vec2{width, height / 2.0}
                    + grab_size * glm::vec2{0, 0.5} * _unit,
            };
            // South
            BBox2 s{
                glm::vec2{box.min.x, box.max.y}
                    + glm::vec2{width / 2.0, 0}
                    + grab_size * glm::vec2{-0.5, 1} * _unit,
                glm::vec2{box.min.x, box.max.y}
                    + glm::vec2{width / 2.0, 0}
                    + grab_size * glm::vec2{0.5, 0} * _unit,
            };
            // West
            BBox2 w{
                box.min
                    + glm::vec2{0, height / 2.0}
                    + grab_size * glm::vec2{-1, -0.5} * _unit,
                box.min
                    + glm::vec2{0, height / 2.0}
                    + grab_size * glm::vec2{0, 0.5} * _unit,
            };

            if (box.contains(point)) return Area::BOX;
            if (nw.contains(point)) return Area::NW;
            if (ne.contains(point)) return Area::NE;
            if (sw.contains(point)) return Area::SW;
            if (se.contains(point)) return Area::SE;
            if (n.contains(point)) return Area::N;
            if (e.contains(point)) return Area::E;
            if (s.contains(point)) return Area::S;
            if (w.contains(point)) return Area::W;
            return Area::NONE;
        }

        /** Draw the main bounding-box. */
        void draw_box(Cairo::RefPtr<Cairo::Context> const &cr)
        {
            _draw_bbox(cr, box);
        }
        /** Draw the box control handles. */
        void draw_handles(Cairo::RefPtr<Cairo::Context> const &cr)
        {
            auto const width = box.max.x - box.min.x;
            auto const height = box.max.y - box.min.y;
            if (width == 0 && height == 0)
                return;

            // North-west
            _draw_bbox(cr, BBox2{box.min, box.min - grab_size * _unit});
            // North-east
            _draw_bbox(cr, BBox2{
                {box.max.x, box.min.y},
                glm::vec2{box.max.x, box.min.y}
                    + grab_size * glm::vec2{1, -1} * _unit
            });
            // South-west
            _draw_bbox(cr, BBox2{
                {box.min.x, box.max.y},
                glm::vec2{box.min.x, box.max.y}
                    + grab_size * glm::vec2{-1, 1} * _unit
            });
            // South-east
            _draw_bbox(cr, BBox2{
                box.max,
                box.max + grab_size * _unit
            });

            // North
            _draw_bbox(cr, BBox2{
                box.min
                    + glm::vec2{width / 2.0, 0}
                    + grab_size * glm::vec2{-0.5, -1} * _unit,
                box.min
                    + glm::vec2{width / 2.0, 0}
                    + grab_size * glm::vec2{0.5, 0} * _unit,
            });
            // East
            _draw_bbox(cr, BBox2{
                box.min
                    + glm::vec2{width, height / 2.0}
                    + grab_size * glm::vec2{1, -0.5} * _unit,
                box.min
                    + glm::vec2{width, height / 2.0}
                    + grab_size * glm::vec2{0, 0.5} * _unit,
            });
            // South
            _draw_bbox(cr, BBox2{
                glm::vec2{box.min.x, box.max.y}
                    + glm::vec2{width / 2.0, 0}
                    + grab_size * glm::vec2{-0.5, 1} * _unit,
                glm::vec2{box.min.x, box.max.y}
                    + glm::vec2{width / 2.0, 0}
                    + grab_size * glm::vec2{0.5, 0} * _unit,
            });
            // West
            _draw_bbox(cr, BBox2{
                box.min
                    + glm::vec2{0, height / 2.0}
                    + grab_size * glm::vec2{-1, -0.5} * _unit,
                box.min
                    + glm::vec2{0, height / 2.0}
                    + grab_size * glm::vec2{0, 0.5} * _unit,
            });
        }
    };

    /** Displays .map files. */
    class MapArea2D : public Gtk::DrawingArea
    {
    public:
        using DrawSpacePoint = glm::vec2;

        enum DrawAngle {TOP, FRONT, RIGHT};

        MapArea2D(Editor &ed);

        /** Convert screen-space coordinates to draw-space coordinates. */
        DrawSpacePoint screenspace_to_drawspace(double x, double y) const;
        /** Convert draw-space coordinates to screen-space coordinates. */
        glm::vec2 drawspace_to_screenspace(DrawSpacePoint const &v) const;
        /** Convert draw-space coordinates to world-space coordinates. */
        MAP::Vertex drawspace_to_worldspace(DrawSpacePoint const &v) const;
        /** Convert world-space coordinates to draw-space coordinates. */
        DrawSpacePoint worldspace_to_drawspace(MAP::Vertex const &v) const;
        /** Pick an EditorBrush based on the given point. */
        EditorBrush *pick_brush(DrawSpacePoint point);

        auto property_clear_color() {return _prop_clear_color.get_proxy();}
        auto property_grid_size() {return _prop_grid_size.get_proxy();}
        auto property_draw_angle() {return _prop_draw_angle.get_proxy();}
        auto property_draw_angle() const {return _prop_draw_angle.get_proxy();}
        auto property_transform() {return _prop_transform.get_proxy();}
        auto property_transform() const {return _prop_transform.get_proxy();}
        auto property_state() {return _prop_state.get_proxy();}

        void set_draw_angle(DrawAngle angle){property_draw_angle().set_value(angle);}
        auto get_draw_angle() {return property_draw_angle().get_value();};
        auto &get_editor() {return _editor;}
        auto &get_box() {return _box;}

    protected:
        // Signal handlers
        bool on_draw(Cairo::RefPtr<Cairo::Context> const &cr) override;
        void on_editor_map_changed();
        void on_editor_selection_changed();
        void on_draw_angle_changed();

        // Input Signals
        bool on_enter_notify_event(GdkEventCrossing *event) override;

    private:
        Editor &_editor;

        GrabbableBox _box;

        // Properties
        Glib::Property<Gdk::RGBA> _prop_clear_color;
        Glib::Property<int> _prop_grid_size;
        Glib::Property<DrawAngle> _prop_draw_angle;
        Glib::Property<MapArea2Dx::Transform2D> _prop_transform;
        Glib::Property<MapArea2Dx::State> _prop_state;

        void _draw_brush(
            Cairo::RefPtr<Cairo::Context> const &cr,
            EditorBrush const &brush) const;
        void _draw_map(Cairo::RefPtr<Cairo::Context> const &cr) const;
    };
}

#endif
