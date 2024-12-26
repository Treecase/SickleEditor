/**
 * TextureManager.hpp - Lazy-loading texture manager.
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

#ifndef SE_EDITOR_TEXTURES_TEXTUREMANAGER_HPP
#define SE_EDITOR_TEXTURES_TEXTUREMANAGER_HPP

#include "TextureInfo.hpp"

#include <sigc++/signal.h>

#include <filesystem>
#include <memory>
#include <unordered_map>
#include <unordered_set>

namespace Sickle::Editor::Textures
{
    /**
     * Singleton managing texture access.
     */
    class TextureManager
    {
    public:
        /**
         * Emitted when WADs are added or removed from the manager.
         */
        static auto &signal_wads_changed() { return _sig_wads_changed; }

        /**
         * Get a reference to the TextureManager singleton.
         *
         * @return A reference to the TextureManager singleton.
         */
        static TextureManager &get_reference();

        /**
         * Add a WAD to the manager.
         *
         * @param wad_path File path to the wad to be added.
         *
         * @todo What happens when a WAD overwrites an existing texture?
         */
        void add_wad(std::filesystem::path const &wad_path);

        /**
         * Remove a WAD and all its textures from the manager. Fails silently
         * if the path is not in the manager.
         *
         * @param wad_path Path of the WAD to remove.
         */
        void remove_wad(std::filesystem::path const &wad_path);

        /**
         * Remove a WAD and all its textures from the manager. Fails silently
         * if the path is not in the manager.
         *
         * @param wad_name Name of the WAD to remove.
         */
        void remove_wad(std::string const &wad_name);

        /**
         * Remove all WADs and textures from the manager.
         */
        void clear_wads();

        /**
         * Get a list of all the names of the wads in the manager.
         *
         * @return A collection of all WAD names in the manager.
         */
        std::unordered_set<std::string> get_wads() const;

        /**
         * Get a list of all the paths of the wads in the manager.
         *
         * @return A collection of all WAD paths in the manager.
         */
        std::unordered_set<std::filesystem::path> get_wad_paths() const;

        /**
         * Get the named texture.
         *
         * @param name Name of the texture to get.
         * @return Texture info for the named texture.
         * @throw std::out_of_range if the texture does not exist.
         */
        std::shared_ptr<TextureInfo> get_texture(std::string const &name) const;

        /**
         * Get all the textures.
         *
         * @return A collection of all the stored textures.
         */
        auto get_textures() const { return _textures; }

    private:
        static sigc::signal<void> _sig_wads_changed;

        std::unordered_set<std::shared_ptr<TextureInfo>> _textures{};

        std::unordered_map<std::filesystem::path, std::string> _wad_paths{};
        std::unordered_map<
            std::string,
            std::vector<std::shared_ptr<TextureInfo>>>
            _by_wad{};
        std::unordered_map<std::string, std::shared_ptr<TextureInfo>>
            _by_name{};

        TextureManager();
        TextureManager(TextureManager const &) = delete;
        TextureManager &operator=(TextureManager const &) = delete;
    };
} // namespace Sickle::Editor::Textures

#endif
