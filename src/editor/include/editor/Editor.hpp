/**
 * Editor.hpp - Map Editor class.
 * Copyright (C) 2022 Trevor Last
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

#include "map/map.hpp"

#include <sigc++/signal.h>

#include <unordered_set>


namespace Sickle
{
    /**
     * The Editor class manages all the objects in the map, as well editor-only
     * data like visgroups.
     */
    class Editor
    {
    public:
        class BBox
        {
        public:
            void p1(MAP::Vertex v);
            void p2(MAP::Vertex v);
            auto p1() const {return _p1;}
            auto p2() const {return _p2;}

            auto &signal_updated() {return _signal_updated;}
        protected:
            sigc::signal<void()> _signal_updated;
        private:
            MAP::Vertex _p1, _p2;
        };

        class Selection
        {
        public:
            using Item = MAP::Brush const *;

            void clear();
            void add(Item const &item);
            void remove(Item const &item);
            bool contains(Item const &item) const;

            auto begin() const {return _selected.begin();}
            auto end() const {return _selected.end();}

            auto &signal_updated() {return _signal_updated;}
        protected:
            sigc::signal<void()> _signal_updated;
        private:
            std::unordered_set<Item> _selected;
        };

        /** Box used to create new brushes. */
        BBox brushbox;
        /** Selected brushes/entities. */
        Selection selected;
    };
}

#endif
