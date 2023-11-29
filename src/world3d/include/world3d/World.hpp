/**
 * World.hpp - OpenGL Editor::World view.
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

#ifndef SE_WORLD3D_WORLD_HPP
#define SE_WORLD3D_WORLD_HPP

#include "Entity.hpp"

#include <glibmm/refptr.h>
#include <world/World.hpp>

#include <memory>
#include <vector>


namespace World3D
{
    /**
     * Top-level World view object.
     *
     * Manages Entity object views, sychronized with an Editor::World object.
     */
    class World
    {
    public:
        std::vector<std::shared_ptr<Entity>> entities{};

        World(Glib::RefPtr<Sickle::Editor::World> src);

        void render() const;

    private:
        World(World const &)=delete;
        World &operator=(World const &)=delete;
    };
}

#endif
