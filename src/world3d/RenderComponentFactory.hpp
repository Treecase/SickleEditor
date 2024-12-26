/**
 * RenderComponentFactory.hpp - Factory which constructs RenderComponents for
 *                              world objects.
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

#ifndef SE_WORLD3D_RENDERERFACTORY_HPP
#define SE_WORLD3D_RENDERERFACTORY_HPP

#include "RenderComponent.hpp"

#include <editor/interfaces/EditorObject.hpp>

#include <memory>

namespace World3D
{
    /**
     * Constructs the correct RenderComponents for different EditorObject types.
     */
    class RenderComponentFactory final
    {
    public:
        RenderComponentFactory() = default;

        /**
         * Construct an appropriate RenderComponent for an object. Note that
         * the constructed component is not attached to the object.
         *
         * @param object The object to construct a RenderComponent for.
         * @return The new component, or nullptr if none are appropriate.
         */
        std::shared_ptr<RenderComponent> construct(
            Sickle::Editor::EditorObjectRef const &object);
    };
} // namespace World3D

#endif
