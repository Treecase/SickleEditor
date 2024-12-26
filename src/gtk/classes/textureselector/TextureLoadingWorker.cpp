/**
 * TextureLoadingWorker.cpp - Worker thread to load TextureImages for a
 *                            TextureSelector.
 * Copyright (C) 2024 Trevor Last
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

#include "TextureLoadingWorker.hpp"

#include <editor/textures/TextureManager.hpp>

using namespace Sickle::TextureSelector;

void TextureLoadingWorker::do_work(Glib::Dispatcher *dispatcher)
{
    auto &texman = Editor::Textures::TextureManager::get_reference();
    for (auto const &texinfo : texman.get_textures())
    {
        auto const pixels = texinfo->load_rgb();
        {
            std::lock_guard lock{_mutex};
            _results.push_back(std::make_pair(texinfo, pixels));
            if (_cancelled)
            {
                break;
            }
        }
        dispatcher->emit();
    }

    {
        std::lock_guard lock{_mutex};
        _is_done = true;
    }
    dispatcher->emit();
}

void TextureLoadingWorker::cancel()
{
    std::lock_guard lock{_mutex};
    _cancelled = true;
}

bool TextureLoadingWorker::is_done() const
{
    std::lock_guard lock{_mutex};
    return _is_done;
}

std::vector<TextureLoadingWorker::Result> TextureLoadingWorker::get_results()
{
    std::lock_guard lock{_mutex};
    std::vector<TextureLoadingWorker::Result> const results{
        _results.cbegin(),
        _results.cend()};
    _results.clear();
    return results;
}
