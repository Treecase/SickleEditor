/**
 * main.cpp - Program entry point.
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

#include "appid.hpp"
#include "version.hpp"
#include "gtk/classes/App.hpp"

#ifndef _NDEBUG
#include <glibmm/miscutils.h>
#endif


int main(int argc, char *argv[])
{
    /* Debug builds won't have a schema installed, so we need to manually point
     * to it. */
#ifndef _NDEBUG
    Glib::setenv("GSETTINGS_SCHEMA_DIR", SE_BINARY_DIR, false);
#endif
    auto app = Sickle::App::create();
    return app->run(argc, argv);
}
