/**
 * AppWin.hpp - Sickle ApplicationWindow.
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

#ifndef _SE_APPWIN_HPP
#define _SE_APPWIN_HPP

#include "MapArea.hpp"

#include <gtkmm.h>


namespace Sickle
{
    class AppWin : public Gtk::ApplicationWindow
    {
    public:
        AppWin(BaseObjectType *cobject, Glib::RefPtr<Gtk::Builder> const &refBuilder);

        static AppWin *create();

        /** Open a file. */
        void open(Gio::File const *file);

    protected:
        Glib::RefPtr<Gtk::Builder> m_refBuilder;
        MapArea *m_maparea;

        bool on_key_press_event(GdkEventKey *event) override;
        bool on_key_release_event(GdkEventKey *event) override;
    };
}

#endif
