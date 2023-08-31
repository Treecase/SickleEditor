/**
 * GrabbableBoxView.hpp - Cairo drawer for GrabbableBox.
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

#ifndef SE_MAPAREA2D_GRABBABLEBOXVIEW_HPP
#define SE_MAPAREA2D_GRABBABLEBOXVIEW_HPP

#include "../BBox2View.hpp"
#include "GrabbableBox.hpp"

#include <cairomm/cairomm.h>

#include <array>
#include <memory>


namespace Sickle
{
    class GrabbableBoxView
    {
        using Area = GrabbableBox::Area;
        std::array<std::shared_ptr<BBox2View>, Area::COUNT> _views{};
    public:
        GrabbableBoxView()=default;
        GrabbableBoxView(
            std::shared_ptr<BBox2View> const &box,
            std::shared_ptr<BBox2View> const &handles);

        virtual ~GrabbableBoxView()=default;

        /** Draw a GrabbableBox. */
        virtual void draw(
            Cairo::RefPtr<Cairo::Context> const &cr,
            GrabbableBox const &gb);
    };
}

#endif
