/**
 * TextureSelector.cpp - Texture selection window.
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

#include "TextureSelector.hpp"

#include <appid.hpp>

using namespace Sickle::AppWin;


TextureSelector::Image::Image(WAD::Lump const &lump)
{
    auto const texlump = WAD::readTexLump(lump);
    for (size_t i = 0; i < texlump.width * texlump.height; ++i)
    {
        auto const pixel = texlump.tex1.at(i);
        auto const &rgb = texlump.palette.at(pixel);
        rgb_data.push_back(rgb.at(0));
        rgb_data.push_back(rgb.at(1));
        rgb_data.push_back(rgb.at(2));
    }
    pixbuf = Gdk::Pixbuf::create_from_data(
        rgb_data.data(),
        Gdk::Colorspace::COLORSPACE_RGB,
        false,
        8,
        texlump.width, texlump.height,
        texlump.width * 3);
    img = Gtk::Image{pixbuf};
    name = lump.name;
}


Glib::RefPtr<TextureSelector> TextureSelector::create()
{
    return Glib::RefPtr{new TextureSelector{}};
}


TextureSelector::TextureSelector()
:   Glib::ObjectBase{typeid(TextureSelector)}
,   _prop_wad_paths{*this, "wad-paths", {}}
{
    property_wad_paths().signal_changed().connect(
        sigc::mem_fun(*this, &TextureSelector::on_wad_paths_changed));


    auto const builder = Gtk::Builder::create_from_resource(
        SE_GRESOURCE_PREFIX "gtk/TextureSelector.glade");
    builder->get_widget<Gtk::Dialog>("textureselector", _dialog);
    builder->get_widget<Gtk::FlowBox>("flow", _flow);
    builder->get_widget<Gtk::Button>("cancel", _cancel);
    builder->get_widget<Gtk::Button>("confirm", _confirm);

    _cancel->signal_clicked().connect(
        [this](){_dialog->response(Gtk::ResponseType::RESPONSE_CANCEL);});
    _confirm->signal_clicked().connect(
        [this](){_dialog->response(Gtk::ResponseType::RESPONSE_ACCEPT);});

    _dialog->signal_response().connect([this](int){_dialog->hide();});
}


std::string TextureSelector::get_selected_texture() const
{
    auto const selected = _flow->get_selected_children();
    auto const child = selected.at(0)->get_child();
    for (auto const &img : _images)
        if (child == &img.img)
            return img.name;
    throw std::runtime_error{"selected child could not be found"};
}


int TextureSelector::run()
{
    _dialog->show_all();
    return _dialog->run();
}



void TextureSelector::on_wad_paths_changed()
{
    _refresh_textures();
}



void TextureSelector::_refresh_textures()
{
    _clear_textures();
    _add_textures();
}


void TextureSelector::_clear_textures()
{
    for (auto &img : _images)
        _flow->remove(img.img);
    _images.clear();
}


void TextureSelector::_add_textures()
{
    for (auto const &path : property_wad_paths().get_value())
    {
        auto const wad = WAD::load(path);
        for (auto const &lump : wad.directory)
            _add_texture(lump);
    }
}


void TextureSelector::_add_texture(WAD::Lump const &lump)
{
    auto &image = _images.emplace_back(lump);
    _flow->add(image.img);
}
