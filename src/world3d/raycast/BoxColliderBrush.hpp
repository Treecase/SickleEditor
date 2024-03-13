/**
 * BoxColliderBrush.hpp - 3D box collider for raycast operations, Brush
 *                        tracking version.
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

#ifndef SE_WORLD3D_RAYCAST_BOXCOLLIDERBRUSH_HPP
#define SE_WORLD3D_RAYCAST_BOXCOLLIDERBRUSH_HPP

#include "BoxCollider.hpp"

#include <editor/world/Brush.hpp>

#include <sigc++/connection.h>

#include <vector>
#include <memory>


namespace World3D
{
    /**
     * Implements the BoxCollider interface for use with Brushes.
     *
     * Attempting to attach this component to anything other than a Brush will
     * throw an error.
     */
    class BoxColliderBrush : public BoxCollider
    {
    public:
        BoxColliderBrush()=default;
        virtual ~BoxColliderBrush()=default;

    protected:
        virtual void on_attach(Sickle::Componentable &obj) override;
        virtual void on_detach(Sickle::Componentable &obj) override;

    private:
        struct Signals
        {
            std::vector<sigc::connection> conns{};
            ~Signals()
            {
                for (auto conn : conns)
                    conn.disconnect();
            }
        };

        Sickle::Editor::Brush *_src{nullptr};
        std::unique_ptr<Signals> _signals{nullptr};

        void update_bbox();
    };
}

#endif
