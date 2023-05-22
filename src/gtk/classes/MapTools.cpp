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


Sickle::MapTools::MapTools(Editor::Editor &editor)
:   Glib::ObjectBase{typeid(MapTools)}
,   Gtk::Box{}
,   _prop_tool{*this, "tool", Tool::SELECT}
,   _editor{editor}
{
    set_orientation(Gtk::Orientation::ORIENTATION_VERTICAL);

    property_tool().signal_changed().connect(
        sigc::mem_fun(*this, &MapTools::on_tool_changed));

    // Add RadioButtons for the tools.
    auto group = _buttons.at(property_tool().get_value()).get_group();
    _buttons.at(property_tool().get_value()).set_active(true);
    for (int m = 0; m < Tool::_COUNT; ++m)
    {
        auto &button = _buttons.at(m);
        button.set_label(_tools.at(m)->name());
        button.set_group(group);
        button.signal_toggled().connect(
            sigc::bind(
                sigc::mem_fun(*this, &MapTools::on_tool_button_toggled),
                static_cast<Tool>(m)));
        add(button);
    }

    _editor.maptool = _tools.at(property_tool().get_value());
}



void Sickle::MapTools::on_tool_button_toggled(Tool tool)
{
    if (_buttons.at(tool).get_active())
        property_tool() = tool;
}


void Sickle::MapTools::on_tool_changed()
{
    _buttons.at(property_tool().get_value()).set_active(true);
    _editor.maptool = _tools.at(property_tool().get_value());
}
