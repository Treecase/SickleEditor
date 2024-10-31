/**
 * BrushBox.hpp - Editor BrushBox class.
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

#ifndef SE_EDITOR_BRUSHBOX_HPP
#define SE_EDITOR_BRUSHBOX_HPP

#include <se-lua/utils/Referenceable.hpp>

#include <glm/glm.hpp>
#include <sigc++/signal.h>

namespace Sickle::Editor
{
    class BrushBox : public Lua::Referenceable
    {
    public:
        void p1(glm::vec3 const &v);
        void p2(glm::vec3 const &v);
        glm::vec3 p1() const;
        glm::vec3 p2() const;

        auto &signal_updated() { return _signal_updated; }

    private:
        glm::vec3 _p1, _p2;
        sigc::signal<void()> _signal_updated{};
    };
} // namespace Sickle::Editor

#endif
