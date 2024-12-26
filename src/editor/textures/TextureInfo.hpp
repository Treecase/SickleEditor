/**
 * TextureInfo.hpp - Holds texture information.
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

#ifndef SE_EDITOR_TEXTURES_TEXTUREINFO_HPP
#define SE_EDITOR_TEXTURES_TEXTUREINFO_HPP

#include <files/wad/LumpTexture.hpp>

#include <memory>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <unordered_map>

namespace Sickle::Editor::Textures
{
    /**
     * Identifies texture mipmap levels.
     */
    enum MipmapLevel
    {
        /// Full size texture.
        MIPMAP_FULL = 0,
        /// 1/2 size texture.
        MIPMAP_HALF = 1,
        /// 1/4 size texture.
        MIPMAP_QUARTER = 2,
        /// 1/8 size texture.
        MIPMAP_EIGHTH = 3
    };

    /**
     * Holds information about a texture.
     *
     * Also has a caching functionality. The cache contains one object for each
     * type. The user can avoid slow reads from disk by using the cache to
     * store already constructed objects.
     */
    class TextureInfo
    {
    public:
        /**
         * Get the name of the WAD this texture came from.
         *
         * @return Name of this texture's source WAD.
         */
        std::string get_source_wad() const;

        /**
         * Get this texture's name.
         *
         * @return The name of the texture.
         */
        std::string get_name() const;

        /**
         * Get the width of the texture.
         *
         * @param mipmap Mipmap level of the texture to get the width of.
         * @return The width of the texture.
         */
        unsigned int get_width(
            MipmapLevel mipmap = MipmapLevel::MIPMAP_FULL) const;

        /**
         * Get the height of the texture.
         *
         * @param mipmap Mipmap level of the texture to get the height of.
         * @return The height of the texture.
         */
        unsigned int get_height(
            MipmapLevel mipmap = MipmapLevel::MIPMAP_FULL) const;

        /**
         * Load the texture into a buffer. Caller takes ownership of the
         * buffer. This does blocking I/O.
         *
         * @param mipmap Mipmap level of the texture to load data from.
         * @return A pointer to the loaded buffer.
         */
        std::shared_ptr<uint8_t[]> load_rgba(
            MipmapLevel mipmap = MipmapLevel::MIPMAP_FULL) const;

        /**
         * Load the texture into a buffer. Caller takes ownership of the
         * buffer. This does blocking I/O.
         *
         * @param mipmap Mipmap level of the texture to load data from.
         * @return A pointer to the loaded buffer.
         */
        std::shared_ptr<uint8_t[]> load_rgb(
            MipmapLevel mipmap = MipmapLevel::MIPMAP_FULL) const;

        /**
         * Cache an object of type T. If an object is already cached, it will
         * be overwritten.
         *
         * @tparam T Type of the object to be cached.
         * @param obj The object to store.
         */
        template<class T>
        void cache_object(std::shared_ptr<T> const &obj)
        {
            std::type_index const idx{typeid(T)};
            auto ptr = std::reinterpret_pointer_cast<void, T>(obj);
            _cache.insert_or_assign(idx, ptr);
        }

        /**
         * Clear the cache for type T.
         *
         * @tparam T Type of cache to clear.
         */
        template<class T>
        void clear_cache()
        {
            std::type_index const idx{typeid(T)};
            _cache.erase(idx);
        }

        /**
         * Retrieve an object of type T from the cache.
         *
         * @tparam T Type of the object to retrieve.
         * @return A pointer to the cached object.
         * @throw std::out_of_range if there is no cached object of type T.
         */
        template<class T>
        std::shared_ptr<T> get_cached() const
        {
            std::type_index const idx{typeid(T)};
            auto ptr = _cache.at(idx);
            return std::reinterpret_pointer_cast<T>(ptr);
        }

    protected:
        // TextureInfos can only be created by TextureManagers.
        friend class TextureManager;

        TextureInfo(std::string source_wad, WAD::LumpTexture const &texlump);

    private:
        std::string _source_wad;
        WAD::LumpTexture _texlump;

        std::unordered_map<std::type_index, std::shared_ptr<void>> _cache{};
    };
} // namespace Sickle::Editor::Textures

#endif
