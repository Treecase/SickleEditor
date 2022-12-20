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
#include "MapArea2D.hpp"

#include <gtkmm/applicationwindow.h>
#include <gtkmm/grid.h>


namespace Sickle
{
    class AppWin : public Gtk::ApplicationWindow
    {
    public:
        AppWin();

        /** Open a file. */
        void open(Gio::File const *file);

    protected:
        Gtk::Grid m_grid;
        MapArea m_maparea;
        MapArea2D m_drawarea_top, m_drawarea_front, m_drawarea_right;
    };
}

#endif
