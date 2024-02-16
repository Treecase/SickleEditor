/**
 * RenderComponent.hpp - Abstract base class for World3D render components.
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

#ifndef SE_WORLD3D_RENDERCOMPONENT_HPP
#define SE_WORLD3D_RENDERCOMPONENT_HPP

#include <interfaces/Component.hpp>


namespace World3D
{
    /**
     * Render components draw a 3D representation of their attached object using
     * OpenGL.
     */
    class RenderComponent : public Sickle::Component {};
}

#endif
