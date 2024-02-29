/**
 * TextureImage.cpp - Texture selection window.
 * Copyright (C) 2023 Trevor Last
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


decltype(TextureImage::_rgb_data)
TextureImage::convert_texlump_to_rgb_data(WAD::TexLump const &texlump)
{
    decltype(TextureImage::_rgb_data) buffer{};
    auto const &tex1 = texlump.tex1();
    auto const &palette = texlump.palette();
    for (size_t i = 0; i < texlump.width() * texlump.height(); ++i)
    {
        auto const pixel = tex1.at(i);
        auto const &rgb = palette.at(pixel);
        buffer.push_back(rgb.at(0));
        buffer.push_back(rgb.at(1));
        buffer.push_back(rgb.at(2));
    }
    return buffer;
}


TextureImage::TextureImage(std::string const &name, WAD::TexLump const &texlump)
:   Glib::ObjectBase{typeid(TextureImage)}
,   Gtk::Box{Gtk::Orientation::ORIENTATION_VERTICAL}
,   _name{name}
,   _label{_name}
{
    _rgb_data = convert_texlump_to_rgb_data(texlump);
    auto const pixbuf = Gdk::Pixbuf::create_from_data(
        _rgb_data.data(),
        Gdk::Colorspace::COLORSPACE_RGB,
        false,
        8,
        texlump.width(), texlump.height(),
        texlump.width() * 3);
    _image = Gtk::Image{pixbuf};

    add(_image);
    add(_label);
}


std::string TextureImage::get_name() const
{
    return _name;
}
