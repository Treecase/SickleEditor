/**
 * Editor.cpp - Editor::Editor.
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

#include "core/Editor.hpp"

using namespace Sickle::Editor;


Glib::RefPtr<Editor> Editor::create(lua_State *L)
{
    return Glib::RefPtr<Editor>{new Editor{L}};
}


Editor::Editor(lua_State *L)
:   Glib::ObjectBase{typeid(Editor)}
,   Lua::Referenceable{}
,   oploader{std::make_shared<OperationLoader>(L)}
,   _prop_map{*this, "map", World::create()}
,   _prop_maptool{*this, "maptool", ""}
,   _prop_wads{*this, "wads", {}}
{
    property_map().signal_changed().connect(
        sigc::mem_fun(*this, &Editor::_on_map_changed));
}


MapTool Editor::get_maptool() const
{
    auto const name = property_maptool().get_value();
    if (name.empty())
        throw std::logic_error{"no maptool set"};
    return _maptools.at(name);
}


void Editor::add_maptool(MapTool const &maptool)
{
    _maptools.insert({maptool.name(), maptool});
    signal_maptools_changed().emit();
    if (property_maptool().get_value().empty())
        set_maptool(maptool.name());
}


void Editor::do_operation(std::string const &id)
{
    auto const &op = oploader->get_operation(id);
    op.execute(this);
}



void Editor::_on_map_changed()
{
    brushbox.p1(glm::vec3{});
    brushbox.p2(glm::vec3{});
    selected.clear();
}
