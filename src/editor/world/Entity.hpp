/**
 * Entity.hpp - Editor::Entity.
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

#ifndef SE_EDITOR_WORLD_ENTITY_HPP
#define SE_EDITOR_WORLD_ENTITY_HPP

#include "Brush.hpp"

#include <map/map.hpp>
#include <rmf/rmf.hpp>

#include <glibmm.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


namespace Sickle::Editor
{
    class Entity : public Glib::Object
    {
    public:
        using BrushRef = Glib::RefPtr<Brush>;

        std::unordered_map<std::string, std::string> properties{};

        Entity();
        Entity(Entity const &other);
        Entity(MAP::Entity const &entity);
        Entity(RMF::Entity const &entity);

        operator MAP::Entity() const;
        Entity &operator=(Entity const &other);

        auto &signal_changed() {return _signal_changed;}

        std::vector<BrushRef> brushes() const;

        void add_brush(Brush const &brush);
        void remove_brush(BrushRef const &brush);

    private:
        sigc::signal<void()> _signal_changed{};
        std::vector<BrushRef> _brushes{};
        // TODO:
        // - visgroup id
        // - color
    };
}

#endif
