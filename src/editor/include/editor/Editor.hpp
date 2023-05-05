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
#include <sigc++/connection.h>

#include <unordered_set>


namespace Sickle
{
namespace Editor
{
    class BrushBox
    {
    public:
        void p1(glm::vec3 v);
        void p2(glm::vec3 v);
        auto p1() const {return _p1;}
        auto p2() const {return _p2;}

        auto &signal_updated() {return _signal_updated;}

    private:
        glm::vec3 _p1, _p2;
        sigc::signal<void()> _signal_updated;
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

        Selection()=default;

    private:
        std::unordered_set<Item *> _selected{};

        std::unordered_map<Item *, std::vector<sigc::connection>> _selected_signals{};
        sigc::signal<void()> _signal_updated{};

        Selection(Selection const &)=delete;
        Selection &operator=(Selection const &)=delete;
    };


    /**
     * The Editor class manages all the objects in the map, as well editor-only
     * data like visgroups.
     */
    class Editor
    {
    public:
        /** Box used to create new brushes. */
        BrushBox brushbox{};
        /** Selected brushes/entities. */
        Selection selected{};
        /** WAD paths. */
        Property<std::vector<std::string>> wads{};

        auto &signal_map_changed() {return _signal_map_changed;}

        void set_map(Map const &map);
        Map &get_map();

        Editor();

    private:
        Map _map{};

        sigc::signal<void()> _signal_map_changed{};

        void _on_map_changed();

        Editor(Editor const &)=delete;
        Editor &operator=(Editor const &)=delete;
    };
}
}

#endif
