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

#include <operations/OperationLoader.hpp>
#include <se-lua/utils/Referenceable.hpp>

#include <glibmm.h>

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>


namespace Sickle::Editor
{
    class OperationLoader;

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

        std::shared_ptr<OperationLoader> oploader{nullptr};

        static Glib::RefPtr<Editor> create(lua_State *L);

        auto property_map() {return _prop_map.get_proxy();}
        auto property_map() const {return _prop_map.get_proxy();}
        auto property_maptool() {return _prop_maptool.get_proxy();}
        auto property_maptool() const {return _prop_maptool.get_proxy();}
        auto property_wads() {return _prop_wads.get_proxy();}
        auto property_wads() const {return _prop_wads.get_proxy();}
        auto &signal_maptools_changed() {return _sig_maptools_changed;}

        auto get_map() {return property_map().get_value();}
        MapTool get_maptool() const;
        auto &get_maptools() const {return _maptools;}
        auto get_wads() {return property_wads().get_value();}

        void set_map(Glib::RefPtr<World> const &value) {
            property_map() = value;}
        void set_maptool(std::string const &value) {
            property_maptool() = value;}
        void set_wads(std::vector<std::string> const &value) {
            property_wads() = value;}

        void add_maptool(MapTool const &maptool);

    private:
        Glib::Property<Glib::RefPtr<World>> _prop_map;
        Glib::Property<std::string> _prop_maptool;
        Glib::Property<std::vector<std::string>> _prop_wads;
        sigc::signal<void()> _sig_maptools_changed{};

        std::unordered_map<std::string, MapTool> _maptools{};

        void _on_map_changed();

        Editor(lua_State *L);
        Editor(Editor const &)=delete;
        Editor &operator=(Editor const &)=delete;
    };
}

#endif
