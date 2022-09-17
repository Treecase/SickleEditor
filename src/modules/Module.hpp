/**
 * Module.hpp - Abstract module class.
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

#ifndef _MODULE_HPP
#define _MODULE_HPP

#include "../common.hpp"

#include <SDL.h>


/** Abstract Module class. */
class Module
{
protected:
    // Reference to app config.
    Config &_cfg;

public:
    // Module title.
    std::string title;
    // Is the UI visible?
    bool ui_visible;
    // Is the GL being rendered?
    bool gl_visible;


    Module(Config &cfg, std::string const &title, bool visible, bool glrender)
    :   _cfg{cfg}
    ,   title{title}
    ,   ui_visible{visible}
    ,   gl_visible{glrender}
    {}


    /** Handle user input. */
    virtual void input(SDL_Event const *event)=0;

    /** Draw the app's UI. */
    virtual void drawUI()=0;

    /** Draw non-UI app visuals. */
    virtual void drawGL(float deltaT)=0;
};

#endif
