/**
 * DrawComponent.hpp - Abstract base for MapArea2D draw components.
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

#ifndef SE_MAPAREA2D_DRAWCOMPONENT_HPP
#define SE_MAPAREA2D_DRAWCOMPONENT_HPP

#include <editor/interfaces/Component.hpp>
#include <gtk/classes/maparea2d/MapArea2D.hpp>

#include <cairomm/cairomm.h>


namespace World2D
{
    /**
     * Draw a world object to a Cairo surface.
     */
    class DrawComponent : public Sickle::Component
    {
    public:
        /**
         * Draw the attached object to the Cairo surface.
         *
         * @param cr The context to draw to.
         * @param maparea The maparea that owns the context.
         */
        virtual void draw(
            Cairo::RefPtr<Cairo::Context> const &cr,
            Sickle::MapArea2D const &maparea) const=0;

        virtual void execute() final override {};
    };
}

#endif
