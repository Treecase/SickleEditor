/**
 * ModeSelector.cpp - Mode selector bar.
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

#include "ModeSelector.hpp"
#include <gtkmm/image.h>

using namespace Sickle::AppWin;


ModeSelector::ModeSelector(std::initializer_list<std::string> const &modes)
:   Glib::ObjectBase{typeid(ModeSelector)}
,   Gtk::Box{Gtk::Orientation::ORIENTATION_VERTICAL}
,   _prop_mode{*this, "mode", ""}
{
    for (auto mode : modes)
    {
        auto p = _buttons.emplace(mode, Gtk::RadioButton{_group, mode});
        auto &button = p.first->second;
        button.signal_clicked().connect(
            sigc::bind(
                sigc::mem_fun(*this, &ModeSelector::on_button_clicked), mode));
        add(button);
    }
    property_mode().set_value(*modes.begin());
}



void ModeSelector::on_button_clicked(std::string const &mode)
{
    if (!_buttons.at(mode).property_active().get_value())
        return;
    property_mode().set_value(mode);
}
