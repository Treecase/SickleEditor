/**
 * AppWin.cpp - Sickle ApplicationWindow.
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

#include "AppWin.hpp"

#include <iostream>

#include "appid.hpp"
#include "version.hpp"


Sickle::AppWin::AppWin(BaseObjectType *cobject, Glib::RefPtr<Gtk::Builder> const &refBuilder)
:   Gtk::ApplicationWindow{cobject}
,   m_refBuilder{refBuilder}
,   m_maparea{nullptr}
{
    m_refBuilder->get_widget_derived("gl-area", m_maparea);
    if (!m_maparea)
        throw std::runtime_error("No \"gl-area\" object in AppWin.glade");

    set_show_menubar(true);
    set_icon(Gdk::Pixbuf::create_from_resource(SE_GRESOURCE_PREFIX "logo.png"));
    set_title(SE_CANON_NAME);
}

Sickle::AppWin *Sickle::AppWin::create()
{
    auto refBuilder = Gtk::Builder::create_from_resource(SE_GRESOURCE_PREFIX "AppWin.glade");
    AppWin *window = nullptr;
    refBuilder->get_widget_derived("app_window", window);
    if (!window)
        throw std::runtime_error("No \"app_window\" object in AppWin.glade");
    return window;
}

void Sickle::AppWin::open(Glib::RefPtr<Gio::File> const &file)
{
    m_maparea->set_map(MAP::load(file->get_path()));
}
