/**
 * MapTools.cpp - Sickle Map Tools toolbar.
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

#include "MapTools.hpp"

#include <gtkmm/adjustment.h>

using namespace Sickle::AppWin;

MapTools::MapTools(Editor::EditorRef editor)
: Glib::ObjectBase{typeid(MapTools)}
, Gtk::Frame{}
, _editor{editor}
{
    set_label("Tools");

    _box.set_orientation(Gtk::Orientation::ORIENTATION_VERTICAL);

    _scrolled.property_hscrollbar_policy() = Gtk::PolicyType::POLICY_NEVER;
    _scrolled.add(_box);
    add(_scrolled);

    _editor->signal_maptools_changed().connect(
        sigc::mem_fun(*this, &MapTools::on_maptools_changed));
    _editor->property_maptool().signal_changed().connect(
        sigc::mem_fun(*this, &MapTools::on_tool_changed));
}

void MapTools::on_tool_button_toggled(std::string const &tool)
{
    if (_buttons.at(tool).get_active())
    {
        _editor->set_maptool(tool);
    }
}

void MapTools::on_tool_changed()
{
    auto const tool = _editor->get_maptool().name();
    _buttons.at(tool).set_active(true);
}

void MapTools::on_maptools_changed()
{
    for (auto const &p : _editor->get_maptools())
    {
        _add_tool(p.second);
    }
}

void MapTools::_add_tool(Editor::MapTool const &tool)
{
    if (_buttons.count(tool.name()) != 0)
    {
        return;
    }
    _buttons[tool.name()] = {};
    auto &button = _buttons.at(tool.name());
    button.set_label(tool.name());
    button.set_group(_button_group);
    button.signal_toggled().connect(sigc::bind(
        sigc::mem_fun(*this, &MapTools::on_tool_button_toggled),
        tool.name()));
    _box.add(button);
}
