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

#ifndef SE_MAPAREA2D_GRABBABLEBOX_HPP
#define SE_MAPAREA2D_GRABBABLEBOX_HPP

#include <se-lua/utils/Referenceable.hpp>
#include <utils/BoundingBox.hpp>

#include <glm/glm.hpp>

#include <array>
#include <vector>


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
            CENTER,
            N, NE, E, SE, S, SW, W, NW,
            COUNT
        };

        struct Handle
        {
            glm::vec2 anchor;
            glm::vec2 direction; // Only the sign is used.
            glm::vec2 size;

            BBox2 bounds(float unit) const;
        };

        // Size of handles
        glm::vec2 const grab_size{8};
        // Handle scale factor.
        float unit{1.0};

        static std::vector<Area> get_handle_areas();

        /** Set main bounding-box. */
        void set_box(BBox2 const &other);
        /** Get main bounding-box. */
        BBox2 get_box() const;

        Handle get_handle(Area area) const;

        /**
         * Check if a point is inside any of the grabbable areas of the GrabBox.
         */
        Area check_point(glm::vec2 const &point);

    private:
        // Core box area.
        BBox2 _center{};
        // Grab Handles.
        std::array<Handle, Area::COUNT> _handles{};
    };
}

#endif
