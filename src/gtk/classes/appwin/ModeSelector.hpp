/**
 * ModeSelector.hpp - Mode selector bar.
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

#ifndef SE_APPWIN_MODESELECTOR_HPP
#define SE_APPWIN_MODESELECTOR_HPP

#include <glibmm/property.h>
#include <gtkmm/box.h>
#include <gtkmm/radiobutton.h>

#include <initializer_list>
#include <unordered_map>


namespace Sickle::AppWin
{
    class ModeSelector : public Gtk::Box
    {
    public:
        ModeSelector(std::initializer_list<std::string> const &modes);

        auto property_mode() {return _prop_mode.get_proxy();}

    protected:
        void on_button_clicked(std::string const &mode);

    private:
        Glib::Property<std::string> _prop_mode;

        Gtk::RadioButtonGroup _group{};

        std::unordered_map<std::string, Gtk::RadioButton> _buttons{};
    };
}

#endif
