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
#include "version.hpp"


Sickle::About::About()
:   Gtk::AboutDialog{}
{
    set_program_name(SE_CANON_NAME);
    set_version(SE_VERSION);
    set_copyright("Copyright © 2022\nTrevor Last");
    set_website("https://github.com/Treecase/SickleEditor");
    set_website_label("Github");
    set_logo(Gdk::Pixbuf::create_from_resource(SE_GRESOURCE_PREFIX "logo.svg"));
    set_license_type(Gtk::License::LICENSE_GPL_3_0);
}
