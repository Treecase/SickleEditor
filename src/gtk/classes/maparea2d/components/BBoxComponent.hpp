/**
 * BBoxComponent.hpp - Generic bounding-box component.
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

#ifndef SE_MAPAREA2D_BBOXCOMPONENT_HPP
#define SE_MAPAREA2D_BBOXCOMPONENT_HPP

#include <editor/interfaces/Component.hpp>
#include <gtk/classes/maparea2d/MapArea2D.hpp>
#include <utils/BoundingBox.hpp>

namespace World2D
{
    /**
     * Component to generate bounding boxes for world objects.
     */
    class BBoxComponent : public Sickle::Component
    {
    public:
        /**
         * Get the bounding box of the attached object in drawspace.
         *
         * @param maparea The MapArea2D who's drawspace we're using.
         * @return Bounding box of the object in drawspace.
         */
        virtual BBox2 bbox(Sickle::MapArea2D const &maparea) const = 0;

        virtual void execute() override {}
    };
} // namespace World2D

#endif
