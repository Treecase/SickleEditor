/**
 * BBox2View.hpp - Cairo BBox2 drawer.
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

#ifndef SE_MAPAREA2D_BBOX2VIEW_HPP
#define SE_MAPAREA2D_BBOX2VIEW_HPP

#include <utils/BoundingBox.hpp>

#include <cairomm/cairomm.h>

#include <functional>


namespace Sickle
{
    /** BBox2 View. */
    class BBox2View
    {
    public:
        virtual ~BBox2View()=default;

        /** Draw the BBox2. */
        virtual void draw(
            Cairo::RefPtr<Cairo::Context> const &cr,
            BBox2 const &box,
            float unit);
    };

    /** BBox2 View which calls functions before and after drawing. */
    class BBox2ViewCustom : public BBox2View
    {
    public:
        using Func = std::function<
            void(Cairo::RefPtr<Cairo::Context> const &,
            BBox2 const &,
            float)>;

        BBox2ViewCustom(Func const &pre, Func const &post);
        virtual ~BBox2ViewCustom()=default;

        virtual void draw(
            Cairo::RefPtr<Cairo::Context> const &cr,
            BBox2 const &box,
            float unit) override;
    private:
        Func _pre{}, _post{};
    };
}

#endif
