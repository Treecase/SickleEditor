/**
 * BoxCollider.hpp - 3D box collider for raycast operations.
 * Copyright (C) 2024 Trevor Last
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

#ifndef SE_WORLD3D_RAYCAST_BOXCOLLIDER_HPP
#define SE_WORLD3D_RAYCAST_BOXCOLLIDER_HPP

#include "Collider.hpp"

#include <editor/interfaces/Component.hpp>
#include <utils/BoundingBox.hpp>


namespace World3D
{
    /**
     * A component defining a 3D box, used for raycasting operations.
     */
    class BoxCollider : public Collider
    {
    public:
        virtual ~BoxCollider()=default;

        virtual void execute() override;

        /**
         * Get the 3D bounding box.
         *
         * @return The object's bounding box.
         */
        virtual BBox3 get_box() const;

    protected:
        BBox3 _bbox{};

        /**
         * Update the stored bounding box.
         *
         * @param bbox The new bounding box.
         */
        virtual void set_box(BBox3 const &bbox);
    };
}

#endif
