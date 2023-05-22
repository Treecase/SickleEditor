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

#include "BrushBox.hpp"
#include "MapTools.hpp"
#include "Selection.hpp"

#include <sigc++/signal.h>

#include <memory>
#include <string>
#include <vector>


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
        /** Box used to create new brushes. */
        BrushBox brushbox{};
        /** Selected brushes/entities. */
        Selection selected{};
        /** Map tools. */
        Property<std::shared_ptr<MapTool>> maptool{nullptr};
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
