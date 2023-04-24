/**
 * Editor.hpp - Map Editor class.
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

#ifndef SE_EDITOR_HPP
#define SE_EDITOR_HPP

#include "EditorWorld.hpp"

#include <map/map.hpp>

#include <glm/glm.hpp>
#include <sigc++/signal.h>

#include <unordered_set>


namespace Sickle
{
namespace Editor
{
    /**
     * The Editor class manages all the objects in the map, as well editor-only
     * data like visgroups.
     */
    class Editor
    {
    public:
        class BrushBox
        {
        public:
            void p1(glm::vec3 v);
            void p2(glm::vec3 v);
            auto p1() const {return _p1;}
            auto p2() const {return _p2;}

            auto &signal_updated() {return _signal_updated;}
        protected:
            sigc::signal<void()> _signal_updated;
        private:
            glm::vec3 _p1, _p2;
        };

        class Selection
        {
        public:
            using Item = Brush;

            void clear();
            void add(Item *item);
            void remove(Item *item);
            bool contains(Item *item) const;

            auto begin() const {return _selected.begin();}
            auto end() const {return _selected.end();}

            auto &signal_updated() {return _signal_updated;}
        protected:
            sigc::signal<void()> _signal_updated;
        private:
            std::unordered_set<Item *> _selected;
        };

        /** Box used to create new brushes. */
        BrushBox brushbox{};
        /** Selected brushes/entities. */
        Selection selected{};

        auto &signal_map_changed() {return _signal_map_changed;}

        void set_map(Map const &map)
        {
            _map = map;
            signal_map_changed().emit();
        }
        auto &get_map() const {return _map;}

        Editor()
        {
            signal_map_changed().connect(
                sigc::mem_fun(*this, &Editor::_on_map_changed));
        }

        protected:
            sigc::signal<void()> _signal_map_changed{};

        private:
            Map _map{};

            void _on_map_changed()
            {
                brushbox = BrushBox{};
                selected.clear();
            }
    };
}
}

#endif
