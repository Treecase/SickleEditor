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

#include <se-lua/utils/Referenceable.hpp>
#include <utils/BoundingBox.hpp>

#include <cairomm/cairomm.h>
#include <glm/glm.hpp>

#include <array>
#include <functional>
#include <memory>


namespace Sickle
{
    /**
     * GrabbableBox interface.
     * A 2D box with 8 grabbable handles.
     */
    class GrabbableBox : public Lua::Referenceable
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
            // Anchor point of the handle.
            glm::vec2 position;
            // Expand direction of the handle.
            glm::vec2 offset;
            // Size of the handle.
            glm::vec2 size;

            BBox2 bounds(float unit) const
            {
                auto const &size_scaled = 0.5f * size * unit;
                return BBox2{
                    position - size_scaled + offset * size_scaled,
                    position + size_scaled + offset * size_scaled};
            }
        };

        // Size of handles
        glm::vec2 const grab_size{8};
        // Handle scale factor.
        float unit;


        /** Set main bounding-box. */
        void set_box(BBox2 const &other)
        {
            _center = other;
            auto box = other;
            auto const width = box.max.x - box.min.x;
            auto const height = box.max.y - box.min.y;

            _handles[Area::NW] = Handle{box.min, {-1.5, -1.5}, grab_size};
            _handles[Area::NE] = Handle{
                {box.max.x, box.min.y}, {1.5, -1.5}, grab_size};
            _handles[Area::SW] = Handle{
                {box.min.x, box.max.y}, {-1.5, 1.5}, grab_size};
            _handles[Area::SE] = Handle{box.max, {1.5, 1.5}, grab_size};

            _handles[Area::N] = Handle{
                box.min + glm::vec2{width / 2.0, 0}, {0, -1.5}, grab_size};
            _handles[Area::E] = Handle{
                box.min + glm::vec2{width, height / 2.0}, {1.5, 0}, grab_size};
            _handles[Area::S] = Handle{
                glm::vec2{box.min.x, box.max.y} + glm::vec2{width / 2.0, 0},
                {0, 1.5}, grab_size};
            _handles[Area::W] = Handle{
                box.min + glm::vec2{0, height / 2.0}, {-1.5, 0}, grab_size};
        }

        /** Get main bounding-box. */
        BBox2 get_box() const
        {
            return _center;
        }

        auto get_handle(Area area) const
        {
            return _handles.at(area);
        }

        static auto get_handle_areas()
        {
            return std::vector{
                Area::N,
                Area::NE,
                Area::E,
                Area::SE,
                Area::S,
                Area::SW,
                Area::W,
                Area::NW,
            };
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
        // Core box area.
        BBox2 _center{};
        // Grab Handles.
        std::array<Handle, Area::COUNT> _handles{};
    };


    /** BBox2 View. */
    class BBox2View
    {
    public:
        virtual ~BBox2View()=default;

        /** Draw the BBox2. */
        virtual void draw(
            Cairo::RefPtr<Cairo::Context> const &cr,
            BBox2 const &box,
            float unit)
        {
            auto const width = box.max.x - box.min.x;
            auto const height = box.max.y - box.min.y;
            cr->rectangle(box.min.x, box.min.y, width, height);
        }
    };

    /** BBox2 View which calls functions before and after drawing. */
    class BBox2ViewCustom : public BBox2View
    {
    public:
        using Func = std::function<
            void(Cairo::RefPtr<Cairo::Context> const &,
            BBox2 const &,
            float)>;

        BBox2ViewCustom(Func pre, Func post): pre{pre}, post{post} {}
        virtual ~BBox2ViewCustom()=default;

        virtual void draw(
            Cairo::RefPtr<Cairo::Context> const &cr,
            BBox2 const &box,
            float unit) override
        {
            pre(cr, box, unit);
            BBox2View::draw(cr, box, unit);
            post(cr, box, unit);
        }
    private:
        Func pre{}, post{};
    };

    /** GrabbableBox view. */
    class GrabbableBoxView
    {
        using Area = GrabbableBox::Area;
        std::array<std::shared_ptr<BBox2View>, Area::COUNT> _views{};
    public:
        GrabbableBoxView()=default;
        GrabbableBoxView(
            std::shared_ptr<BBox2View> const &box,
            std::shared_ptr<BBox2View> const &handles)
        {
            for (auto &area : _views)
                area = handles;
            _views[Area::BOX] = box;
        }

        virtual ~GrabbableBoxView()=default;

        /** Draw a GrabbableBox. */
        virtual void draw(
            Cairo::RefPtr<Cairo::Context> const &cr, GrabbableBox const &gb)
        {
            if (_views[Area::BOX])
                _views[Area::BOX]->draw(cr, gb.get_box(), gb.unit);
            for (auto area : gb.get_handle_areas())
            {
                if (_views[area])
                {
                    _views[area]->draw(
                        cr,
                        gb.get_handle(area).bounds(gb.unit),
                        gb.unit);
                }
            }
        }
    };
}

#endif
