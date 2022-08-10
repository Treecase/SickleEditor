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
#include "glUtils/glUtil.hpp"
#include "load_model.hpp"
#include "ui_helpers.hpp"
#include "version.hpp"

#include <GL/glew.h>
#include <imgui.h>
#include <SDL.h>

#include <filesystem>
#include <iostream>


/** Play a WAV file. */
SDL_AudioDeviceID _play_sound(SDL_AudioDeviceID dev, char const *path)
{
    // Close previously playing audio device.
    SDL_CloseAudioDevice(dev);

    // Load WAV data.
    SDL_AudioSpec want;
    SDL_zero(want);
    Uint32 len;
    Uint8 *buf;
    if (SDL_LoadWAV(path, &want, &buf, &len) == nullptr)
    {
        throw std::runtime_error{
            "Failed to load WAV: " + std::string{SDL_GetError()}};
    }

    // Open new audio device.
    SDL_AudioSpec have;
    dev = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
    if (dev == 0)
    {
        throw std::runtime_error{
            "Failed to open audio: " + std::string{SDL_GetError()}};
    }
    // Play the audio.
    if (SDL_QueueAudio(dev, buf, len) == -1)
    {
        throw std::runtime_error{
            "Failed to queue audio: " + std::string{SDL_GetError()}};
    }
    SDL_FreeWAV(buf);
    SDL_PauseAudioDevice(dev, 0);
    return dev;
}


/** Plays WAV files. */
class SoundPlayer
{
private:
    SDL_AudioDeviceID _device;
    // Currently selected sound.
    std::filesystem::path _selected_sound;
    std::string _error;
    Config &_cfg;

public:
    // App title.
    std::string title;
    bool ui_visible;


    SoundPlayer(Config &cfg)
    :   _device{0}
    ,   _selected_sound{}
    ,   _error{""}
    ,   _cfg{cfg}
    ,   title{"Sound Player"}
    ,   ui_visible{false}
    {
    }

    ~SoundPlayer()
    {
        SDL_CloseAudioDevice(_device);
    }

    /** Handle user input. */
    void input(SDL_Event const *event)
    {
    }

    /** Draw the app's UI. */
    void drawUI()
    {
        if (!ui_visible)
            return;

        if (ImGui::Begin(title.c_str(), &ui_visible))
        {
            ImGui::TextUnformatted(
                ("Playing: " + (
                    _selected_sound.empty()
                        ? "<none>"
                        : _selected_sound.filename().string())).c_str());
            if (ImGui::Button("Play"))
            {
                try
                {
                    _device = _play_sound(
                        _device, _selected_sound.string().c_str());
                    _error = "";
                }
                catch (std::runtime_error const &e)
                {
                    _device = 0;
                    _error = e.what();
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Stop"))
            {
                SDL_PauseAudioDevice(_device, 1);
            }
            ImGui::TextUnformatted(_error.c_str());
            ImGui::Separator();
            if (ImGui::BeginChild("SoundTree"))
            {
                if (ImGui::TreeNode("valve/sound"))
                {
                    ImGui::DirectoryTree(
                        _cfg.game_dir.string() + "/valve/sound",
                        &_selected_sound,
                        [](std::filesystem::path const &p){
                            return p.extension() == ".wav";
                        });
                    ImGui::TreePop();
                }
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }
};

#endif
