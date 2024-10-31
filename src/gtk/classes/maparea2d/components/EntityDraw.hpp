/**
 * BrushDraw.hpp - Cairo Editor::Brush view.
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

#ifndef SE_MAPAREA2D_ENTITYDRAW_HPP
#define SE_MAPAREA2D_ENTITYDRAW_HPP

#include "DrawComponent.hpp"

#include <editor/world/Entity.hpp>

namespace World2D
{
    /**
     * Renders a 2D view of an Entity using Cairo.
     *
     * Can only be attached to a single Entity at a time.
     */
    class EntityDraw : public DrawComponent
    {
    public:
        EntityDraw() = default;
        virtual ~EntityDraw() = default;

        virtual void draw(
            Cairo::RefPtr<Cairo::Context> const &cr,
            Sickle::MapArea2D const &maparea) const override;

    protected:
        // Component interface.
        virtual void on_attach(Sickle::Componentable &) override;
        // Component interface.
        virtual void on_detach(Sickle::Componentable &) override;

    private:
        Sickle::Editor::Entity const *_entity{nullptr};
    };
} // namespace World2D

#endif
