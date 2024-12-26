/**
 * TextureImage.cpp - Texture selection window.
 * Copyright (C) 2023-2024 Trevor Last
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

#include "TextureImage.hpp"

#include <config/appid.hpp>

using namespace Sickle::TextureSelector;

TextureImage::TextureImage(
    std::shared_ptr<Editor::Textures::TextureInfo> const &texinfo,
    std::shared_ptr<uint8_t[]> const &pixels)
: Glib::ObjectBase{typeid(TextureImage)}
, Gtk::Box{Gtk::Orientation::ORIENTATION_VERTICAL}
, _texinfo{texinfo}
, _pixels{pixels}
, _label{_texinfo->get_name()}
{
    auto const pixbuf = Gdk::Pixbuf::create_from_data(
        _pixels.get(),
        Gdk::Colorspace::COLORSPACE_RGB,
        false,
        8,
        _texinfo->get_width(),
        _texinfo->get_height(),
        _texinfo->get_width() * 3);
    _image = Gtk::Image{pixbuf};

    add(_image);
    add(_label);
}

std::shared_ptr<Sickle::Editor::Textures::TextureInfo> const &TextureImage::
    get_info() const
{
    return _texinfo;
}
