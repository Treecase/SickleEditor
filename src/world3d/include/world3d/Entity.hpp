/**
 * Entity.hpp - OpenGL Editor::Entity view.
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

#ifndef SE_WORLD3D_ENTITY_HPP
#define SE_WORLD3D_ENTITY_HPP

#include "Brush.hpp"

#include <world/Entity.hpp>

#include <memory>
#include <vector>


namespace World3D
{
    /**
     * Entity view.
     *
     * Manages Brush object views, synchronized with an Editor::Entity object.
     */
    class Entity
    {
    public:
        Entity(Sickle::Editor::EntityRef src);

        auto const &brushes() const {return _brushes;}

        void render() const;

    private:
        std::vector<std::shared_ptr<Brush>> _brushes{};

        Entity(Entity const &)=delete;
        Entity &operator=(Entity const &)=delete;

        void _on_brush_deleted(std::shared_ptr<Brush> const &brush);
    };
}

#endif
