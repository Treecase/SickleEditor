/**
 * TextureManager.cpp - Lazy-loading texture manager.
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

#include "TextureManager.hpp"

#include <files/wad/WADReader.hpp>

#include <giomm/file.h>
#include <giomm/datainputstream.h>

#include <iostream>


using namespace Sickle::Editor::Textures;


struct WADInputStreamGIO : WAD::WADInputStream
{
    WADInputStreamGIO(std::string const &wad_path)
    {
        auto const file = Gio::File::create_for_path(wad_path);
        _stream = file->read();
    }
    virtual ~WADInputStreamGIO()=default;

    virtual void seek(size_t offset)
    {
        _stream->seek(offset, Glib::SeekType::SEEK_TYPE_SET);
    }

    virtual void read_bytes(void *buf, size_t count)
    {
        gsize bytes_read = 0;
        _stream->read_all(buf, count, bytes_read);
    }

    virtual uint8_t read_uint8()
    {
        uint8_t byte = 0;
        read_bytes(&byte, 1);
        return byte;
    }

    virtual uint32_t read_uint32()
    {
        uint8_t raw[4];
        read_bytes(raw, 4);
        uint32_t num = (
            raw[0]
            | (raw[1] << 8)
            | (raw[2] << 16)
            | (raw[3] << 24));
        return num;
    }

private:
    Glib::RefPtr<Gio::FileInputStream> _stream{nullptr};
};


/**
 * Generate a uniquely identifying name for a WAD path given a set of already
 * used names.
 */
static std::string generate_unique_name(
    std::filesystem::path const &wad_path,
    std::unordered_set<std::string> const &wads)
{
    auto path = wad_path;
    auto name = wad_path.stem().generic_string();
    while (wads.count(name) && path.has_parent_path())
    {
        path = path.parent_path();
        name = path.stem().generic_string() + "/" + name;
    }
    return name;
}


sigc::signal<void> TextureManager::_sig_wads_changed{};


TextureManager &TextureManager::get_reference()
{
    static TextureManager the_instance{};
    return the_instance;
}



TextureManager::TextureManager()
{
}


void TextureManager::add_wad(std::filesystem::path const &wad_path)
{
    // Do nothing if the wad is already in the manager.
    if (_wad_paths.count(wad_path))
        return;

    auto const wad_name = generate_unique_name(wad_path, get_wads());

    WADInputStreamGIO inputstream{wad_path};
    WAD::WADReader reader{inputstream};
    reader.load();

    std::vector<std::shared_ptr<TextureInfo>> wad_textures{};
    for (auto const &entry : reader.get_directory())
    {
        WAD::LumpTexture texlump{};
        try {
            texlump = reader.load_lump_texture(entry);
        }
        catch (WAD::LumpTexture::BadTypeException const &e) {
            std::cerr << "Texture Load Error: " << e.what() << std::endl;
            continue;
        }
        std::shared_ptr<TextureInfo> texture_info{
            new TextureInfo{wad_name, texlump}};
        wad_textures.push_back(texture_info);
        _textures.insert(texture_info);
        _by_name.insert({texture_info->get_name(), texture_info});
    }
    _by_wad.insert({wad_name, wad_textures});
    signal_wads_changed().emit();
}


void TextureManager::remove_wad(std::string const &wad_name)
{
    if (!_by_wad.count(wad_name))
        return;
    for (auto const &texture : _by_wad.at(wad_name))
    {
        _textures.erase(texture);
        _by_name.erase(texture->get_name());
    }
    _by_wad.erase(wad_name);
    signal_wads_changed().emit();
}


void TextureManager::remove_wad(std::filesystem::path const &wad_path)
{
    if (_wad_paths.count(wad_path))
        remove_wad(_wad_paths.at(wad_path));
}


void TextureManager::clear_wads()
{
    _textures.clear();
    _by_wad.clear();
    _by_name.clear();
    signal_wads_changed().emit();
}


std::unordered_set<std::string> TextureManager::get_wads() const
{
    std::unordered_set<std::string> wads{};
    for (auto const &kv : _by_wad)
        wads.insert(kv.first);
    return wads;
}


std::unordered_set<std::filesystem::path> TextureManager::get_wad_paths() const
{
    std::unordered_set<std::filesystem::path> wads{};
    for (auto const &kv : _wad_paths)
        wads.insert(kv.first);
    return wads;
}


std::shared_ptr<TextureInfo> TextureManager::get_texture(
    std::string const &name) const
{
    return _by_name.at(name);
}
