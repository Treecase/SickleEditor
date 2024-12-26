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

#ifndef SE_MAPAREA2D_BRUSHDRAW_HPP
#define SE_MAPAREA2D_BRUSHDRAW_HPP

#include "DrawComponent.hpp"

#include <editor/world/Brush.hpp>

namespace World2D
{
    /**
     * Renders a 2D view of the Brush using Cairo.
     *
     * Can only be attached to a single Brush at a time.
     */
    class BrushDraw : public DrawComponent
    {
    public:
        BrushDraw() = default;
        virtual ~BrushDraw() = default;

        virtual void draw(
            Cairo::RefPtr<Cairo::Context> const &cr,
            Sickle::MapArea2D const &maparea) const override;

    protected:
        // Component interface.
        virtual void on_attach(Sickle::Componentable &) override;
        // Component interface.
        virtual void on_detach(Sickle::Componentable &) override;

    private:
        Sickle::Editor::Brush const *_brush{nullptr};
    };
} // namespace World2D

#endif
