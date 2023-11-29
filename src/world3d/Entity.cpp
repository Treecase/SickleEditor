/**
 * Entity.cpp - World3D::Entity class.
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

#include "world3d/Entity.hpp"


World3D::Entity::Entity(Sickle::Editor::Entity &src)
{
    for (auto &brush : src.brushes())
    {
        auto b = std::make_shared<Brush>(brush);
        b->signal_deleted().connect(
            sigc::bind(sigc::mem_fun(*this, &Entity::_on_brush_deleted), b));
        _brushes.push_back(b);
    }
}


void World3D::Entity::render() const
{
    for (auto const &brush : brushes())
        brush->render();
}



void World3D::Entity::_on_brush_deleted(std::shared_ptr<Brush> const &brush)
{
    _brushes.erase(std::find(_brushes.cbegin(), _brushes.cend(), brush));
}
