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
using namespace Sickle::Editor;

ModeSelector::ModeSelector()
: Glib::ObjectBase{typeid(ModeSelector)}
, Gtk::Box{Gtk::Orientation::ORIENTATION_VERTICAL}
, _prop_mode{*this, "mode", "__invalid_mode__"}
{
    set_halign(Gtk::Align::ALIGN_START);
    set_valign(Gtk::Align::ALIGN_START);
}

void ModeSelector::add_mode(Mode const &mode, Glib::ustring const &label)
{
    auto const &p = _buttons.emplace(mode, ModeData{});
    auto &data = p.first->second;
    data.btn = Gtk::RadioButton{_group, label};
    data.conn = data.btn.signal_clicked().connect(sigc::bind(
        sigc::mem_fun(*this, &ModeSelector::on_button_clicked),
        mode));
    add(data.btn);
}

void ModeSelector::remove_mode(Mode const &mode)
{
    if (_buttons.count(mode) == 0)
    {
        return;
    }
    auto &data = _buttons.at(mode);
    remove(data.btn);
    data.conn.disconnect();
    _buttons.erase(mode);
}

void ModeSelector::on_button_clicked(Mode const &mode)
{
    if (!_buttons.at(mode).btn.property_active().get_value())
    {
        return;
    }
    property_mode().set_value(mode);
}
