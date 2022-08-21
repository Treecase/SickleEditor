/**
 * SoundPlayer.hpp - WAV sound player module.
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

#ifndef _SOUNDPLAYER_HPP
#define _SOUNDPLAYER_HPP

#include "common.hpp"
#include "glUtil.hpp"
#include "Module.hpp"

#include <GL/glew.h>
#include <SDL.h>

#include <filesystem>


/** Plays WAV files. */
class SoundPlayer : public Module
{
private:
    // Currently playing sound device.
    SDL_AudioDeviceID _device;
    // Currently selected sound.
    std::filesystem::path _selected_sound;
    // Error string for player failures.
    std::string _error;

public:
    SoundPlayer(Config &cfg);
    ~SoundPlayer();

    /** Handle user input. */
    void input(SDL_Event const *event) override;

    /** Draw the player's UI. */
    void drawUI() override;

    /** Does nothing. */
    void drawGL() override {};
};

#endif
