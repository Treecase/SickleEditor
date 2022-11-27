/**
 * About.cpp - Sickle "About" dialog.
 * Copyright (C) 2022 Trevor Last
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

#include "About.hpp"

#include "appid.hpp"


Sickle::About::About(BaseObjectType *cobject, Glib::RefPtr<Gtk::Builder> const &refBuilder)
:   Gtk::AboutDialog{cobject}
,   m_refBuilder{refBuilder}
{
    set_logo(Gdk::Pixbuf::create_from_resource(SE_GRESOURCE_PREFIX "logo.svg"));
}

Sickle::About *Sickle::About::create(Gtk::Window &parent)
{
    auto refBuilder = Gtk::Builder::create_from_resource(SE_GRESOURCE_PREFIX "About.glade");
    About *dialog = nullptr;
    refBuilder->get_widget_derived("about", dialog);
    if (!dialog)
        throw std::runtime_error("No \"about\" object in AppPrefs.ui");
    dialog->set_transient_for(parent);
    return dialog;
}
