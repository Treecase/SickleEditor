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

#include <config/appid.hpp>
#include <config/version.hpp>
#include <OperationSearch.hpp>

#include <memory>

static void initialize_buildable_types()
{
    auto const dummy_OperationSearch
        = std::make_unique<Sickle::AppWin::OperationSearch>();
}

int main(int argc, char *argv[])
{
    auto app = Sickle::App::create();
    // The Gtkmm Builder API requires us to register the GType of each derived
    // widget we want to construct. To do this, we simply create a dummy
    // instance of each derived class here, which then allows us to use the API
    // as expected.
    initialize_buildable_types();
    return app->run(argc, argv);
}
