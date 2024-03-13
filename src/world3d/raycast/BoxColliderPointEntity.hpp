/**
 * BoxColliderPointEntity.hpp - 3D box collider for raycast operations,
 *                              PointEntity tracking version.
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

#ifndef SE_WORLD3D_RAYCAST_BOXCOLLIDERPOINTENTITY_HPP
#define SE_WORLD3D_RAYCAST_BOXCOLLIDERPOINTENTITY_HPP

#include "BoxCollider.hpp"

#include <editor/world/Entity.hpp>

#include <sigc++/connection.h>


namespace World3D
{
    /**
     * Implements the BoxCollider interface for use with PointClass Entities.
     *
     * Attempting to attach this component to anything other than a Entity with
     * a PointClass class will throw an error.
     */
    class BoxColliderPointEntity : public BoxCollider
    {
    public:
        static constexpr float DEFAULT_SIZE = 32.0f;

        BoxColliderPointEntity()=default;
        virtual ~BoxColliderPointEntity()=default;

    protected:
        virtual void on_attach(Sickle::Componentable &obj) override;
        virtual void on_detach(Sickle::Componentable &obj) override;

    private:
        sigc::connection _conn_src_properties_changed;
        Sickle::Editor::Entity *_src{nullptr};

        void update_bbox();
    };
}

#endif
