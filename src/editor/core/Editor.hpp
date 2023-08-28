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

#include <commands/Commands.hpp>
#include <se-lua/utils/Referenceable.hpp>

#include <glibmm.h>

#include <memory>
#include <string>
#include <vector>


namespace Sickle::Editor
{
    /**
     * The Editor class manages all the objects in the map, as well editor-only
     * data like visgroups.
     */
    class Editor : public Glib::Object, public Lua::Referenceable
    {
    public:
        /** Box used to create new brushes. */
        BrushBox brushbox{};
        /** Selected brushes/entities. */
        Selection selected{};

        Editor();

        auto property_map() {return _prop_map.get_proxy();}
        auto property_map() const {return _prop_map.get_proxy();}
        auto property_maptool() {return _prop_maptool.get_proxy();}
        auto property_maptool() const {return _prop_maptool.get_proxy();}
        auto property_wads() {return _prop_wads.get_proxy();}
        auto property_wads() const {return _prop_wads.get_proxy();}

        auto get_map() {return property_map().get_value();}
        auto get_maptool() {return property_maptool().get_value();}
        auto get_wads() {return property_wads().get_value();}

        void set_map(Glib::RefPtr<World> const &value) {
            property_map() = value;}
        void set_maptool(std::shared_ptr<MapTool> const &value) {
            property_maptool() = value;}
        void set_wads(std::vector<std::string> const &value) {
            property_wads() = value;}

        void do_command(std::shared_ptr<Command> command);

    private:
        Glib::Property<Glib::RefPtr<World>> _prop_map;
        Glib::Property<std::shared_ptr<MapTool>> _prop_maptool;
        Glib::Property<std::vector<std::string>> _prop_wads;

        void _on_map_changed();

        Editor(Editor const &)=delete;
        Editor &operator=(Editor const &)=delete;
    };
}

#endif
