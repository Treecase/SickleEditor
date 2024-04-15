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

#include "gtk/classes/App.hpp"

#include <OperationSearch.hpp>
#include <config/appid.hpp>
#include <config/version.hpp>

#include <memory>

#ifndef NDEBUG
#include <glibmm/miscutils.h>
#endif


void initialize_buildable_types()
{
    auto const dummy_OperationSearch =\
        std::make_unique<Sickle::AppWin::OperationSearch>();
}


int main(int argc, char *argv[])
{
    /* Debug builds won't have a schema installed, so we need to manually point
     * to it. */
#ifndef NDEBUG
    Glib::setenv("GSETTINGS_SCHEMA_DIR", SE_BINARY_DIR, false);
#endif
    auto app = Sickle::App::create();
    initialize_buildable_types();
    return app->run(argc, argv);
}
