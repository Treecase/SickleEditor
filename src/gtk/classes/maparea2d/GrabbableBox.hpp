/**
 * GrabbableBox.hpp - 2D box with grabbable handles.
 * Copyright (C) 2023 Trevor Last
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

#ifndef SE_GRABBABLEBOX_HPP
#define SE_GRABBABLEBOX_HPP

#include <utils/BoundingBox.hpp>

#include <cairomm/cairomm.h>
#include <glm/glm.hpp>

#include <array>


namespace Sickle
{
    /**
     * GrabbableBox interface.
     * A 2D box with 8 grabbable handles.
     */
    class GrabbableBox
    {
    public:
        /** Areas of the grabbable box. */
        enum Area
        {
            NONE,
            BOX,
            N, NE, E, SE, S, SW, W, NW,
            COUNT
        };

        struct Handle
        {
            /// Anchor point of the handle.
            glm::vec2 anchor;
            /// Expand direction of the handle.
            glm::vec2 direction;
            BBox2 bounds(float unit) const
            {
                return BBox2{anchor, anchor + direction * unit};
            }
        };

        /// Size of handles
        glm::vec2 const grab_size{8};
        /// Handle scale factor.
        float unit;


        /** Set main bounding-box. */
        void set_box(BBox2 const &other)
        {
            _center = other;
            auto box = other;
            auto const width = box.max.x - box.min.x;
            auto const height = box.max.y - box.min.y;

            _handles[Area::NW] = Handle{box.min, -grab_size};
            _handles[Area::NE] = Handle{
                {box.max.x, box.min.y},
                grab_size * glm::vec2{1, -1}
            };
            _handles[Area::SW] = Handle{
                {box.min.x, box.max.y},
                grab_size * glm::vec2{-1, 1}
            };
            _handles[Area::SE] = Handle{box.max, grab_size};

            _handles[Area::N] = Handle{
                box.min
                    + glm::vec2{width / 2.0, 0}
                    + grab_size * glm::vec2{-0.5, -1},
                grab_size * glm::vec2{0.5, 0}
            };
            _handles[Area::E] = Handle{
                box.min
                    + glm::vec2{width, height / 2.0}
                    + grab_size * glm::vec2{1, -0.5},
                grab_size * glm::vec2{0, 0.5}
            };
            _handles[Area::S] = Handle{
                glm::vec2{box.min.x, box.max.y}
                    + glm::vec2{width / 2.0, 0}
                    + grab_size * glm::vec2{-0.5, 1},
                grab_size * glm::vec2{0.5, 0}
            };
            _handles[Area::W] = Handle{
                box.min
                    + glm::vec2{0, height / 2.0}
                    + grab_size * glm::vec2{-1, -0.5},
                grab_size * glm::vec2{0, 0.5}
            };
        }

        /** Get main bounding-box. */
        BBox2 get_box() const
        {
            return _center;
        }

        auto get_handles() const
        {
            return _handles;
        }

        /**
         * Check if a point is inside any of the grabbable areas of the GrabBox.
         */
        Area check_point(glm::vec2 const &point)
        {
            if (_center.contains(point)) return Area::BOX;
            for (size_t i = 0; i < _handles.size(); ++i)
                if (_handles[i].bounds(unit).contains(point))
                    return static_cast<Area>(i);
            return Area::NONE;
        }

    private:
        // Handles & box areas
        BBox2 _center{};
        // Handle definitions
        std::array<Handle, Area::COUNT> _handles{};
    };

    /** GrabbableBox view. */
    class GrabbableBoxView
    {
    public:
        GrabbableBoxView()=default;
        virtual ~GrabbableBoxView()=default;

        void draw(
            Cairo::RefPtr<Cairo::Context> const &cr, GrabbableBox const &gb)
        {
            cr->set_source_rgb(1, 0, 0);
            cr->set_line_width(gb.unit);
            cr->set_dash(std::vector<double>{4*gb.unit, 4*gb.unit}, 0);
            drawRect(cr, gb.get_box());
            cr->stroke();

            cr->set_source_rgb(1, 1, 1);
            cr->set_line_width(gb.unit);
            for (auto const &handle : gb.get_handles())
                drawRect(cr, handle.bounds(gb.unit));
            cr->fill();
        }

        /** Draw a BBox2. */
        void drawRect(Cairo::RefPtr<Cairo::Context> const &cr, BBox2 const &box)
        {
            auto const width = box.max.x - box.min.x;
            auto const height = box.max.y - box.min.y;
            cr->rectangle(box.min.x, box.min.y, width, height);
        }
    };
}

#endif
