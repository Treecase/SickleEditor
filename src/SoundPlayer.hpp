/**
 * SoundPlayer.hpp - WAV sound player app.
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

#include "load_model.hpp"
#include "glUtils/glUtil.hpp"
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
        throw std::runtime_error{"Failed to load WAV: " + std::string{SDL_GetError()}};
    }

    // Open new audio device.
    SDL_AudioSpec have;
    dev = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0);
    if (dev == 0)
    {
        throw std::runtime_error{"Failed to open audio: " + std::string{SDL_GetError()}};
    }
    // Play the audio.
    if (SDL_QueueAudio(dev, buf, len) == -1)
    {
        throw std::runtime_error{"Failed to queue audio: " + std::string{SDL_GetError()}};
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

public:
    // App title.
    std::string title;
    // MDL filenames to load.
    struct AppConfig
    {
        std::string game_dir;
    } config;
    // Currently selected sound.
    std::filesystem::path selected_sound;
    std::string error;
    bool ui_visible;


    SoundPlayer()
    :   _device{0}
    ,   title{"Sound Player"}
    ,   config{}
    ,   selected_sound{}
    ,   error{""}
    ,   ui_visible{false}
    {
    }

    ~SoundPlayer()
    {
        SDL_CloseAudioDevice(_device);
    }

    /** Configure app from command-line arguments. */
    void handleArgs(int argc, char *argv[])
    {
        for (int i = 1; i < argc; ++i)
        {
            if (strcmp(argv[i], "--version") == 0)
            {
                std::cout << SE_CANON_NAME << " " << SE_VERSION << "\n" <<
                    "Copyright (C) 2022 Trevor Last\n"
                    "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>\n"
                    "This is free software: you are free to change and redistribute it.\n"
                    "There is NO WARRANTY, to the extent permitted by law.\n";
                exit(0);
            }
            else if (strcmp(argv[i], "--help") == 0)
            {
                std::cout <<
                    "Usage: " << argv[0] << " [MODEL]...\n"
                    "\n"
                    "  --help\tdisplay this help and exit\n"
                    "  --version\toutput version information and exit\n"
                    "\n"
                    "Report bugs to: https://github.com/Treecase/Sickle/issues\n"
                    "pkg home page: https://github.com/Treecase/Sickle\n";
                exit(0);
            }
        }
        // FIXME -- do this properly
        config.game_dir = argv[1];
    }

    /** Load non-GL-context-requiring app data. */
    void loadData()
    {
    }

    /** Load app data which requires a GL context. */
    void loadGL()
    {
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
            ImGui::TextUnformatted(("Playing: " + (selected_sound.empty()? "<none>" : selected_sound.filename().string())).c_str());
            if (ImGui::Button("Play"))
            {
                try
                {
                    _device = _play_sound(_device, selected_sound.string().c_str());
                    error = "";
                }
                catch (std::runtime_error const &e)
                {
                    _device = 0;
                    error = e.what();
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Stop"))
            {
                SDL_PauseAudioDevice(_device, 1);
            }
            ImGui::TextUnformatted(error.c_str());
            ImGui::Separator();
            if (ImGui::BeginChild("SoundTree"))
            {
                if (ImGui::TreeNode("valve/sound"))
                {
                    ImGui::DirectoryTree(config.game_dir + "/valve/sound", &selected_sound, [](std::filesystem::path p){return p.extension() == ".wav";});
                    ImGui::TreePop();
                }
            }
            ImGui::EndChild();
        }
        ImGui::End();
    }

    /** Draw non-UI app visuals. */
    void drawGL()
    {
    }
};

#endif
