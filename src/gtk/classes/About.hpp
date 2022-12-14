/**
 * About.hpp - Sickle "About" dialog.
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

#ifndef _SE_ABOUT_HPP
#define _SE_ABOUT_HPP

#include <gtkmm.h>


namespace Sickle
{
    class About : public Gtk::AboutDialog
    {
    public:
        About(BaseObjectType *cobject, Glib::RefPtr<Gtk::Builder> const &refBuilder);

        static About *create(Gtk::Window &parent);

    protected:
        Glib::RefPtr<Gtk::Builder> m_refBuilder;
    };
}

#endif
