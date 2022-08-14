/**
 * App.hpp - Main app class.
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

#ifndef _APP_HPP
#define _APP_HPP

#include "common.hpp"
#include "ModelViewer.hpp"
#include "SoundPlayer.hpp"
#include "TextureViewer.hpp"

#include <SDL.h>

#include <set>


/** Main app class. */
class App
{
private:
    struct ModuleComparer
    {   bool operator()(
            std::shared_ptr<Module> const &a,
            std::shared_ptr<Module> const &b) const
        {return a->title < b->title;}
    };
    std::set<std::shared_ptr<Module>, ModuleComparer> _modules;
    Config &_cfg;

public:
    bool running;


    App(Config &cfg);

    /** Handle user input. */
    void input(SDL_Event const *event);

    /** Draw the app's UI. */
    void drawUI();

    /** Draw non-UI app visuals. */
    void drawGL();
};

#endif
