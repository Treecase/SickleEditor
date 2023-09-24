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

#include <gtkmm/builder.h>

using namespace Sickle::TextureSelector;


Glib::RefPtr<TextureSelector> TextureSelector::create(
    Glib::RefPtr<Sickle::Editor::Editor> const &editor)
{
    return Glib::RefPtr{new TextureSelector{editor}};
}


TextureSelector::TextureSelector(Glib::RefPtr<Sickle::Editor::Editor> const &editor)
:   Glib::ObjectBase{typeid(TextureSelector)}
,   _editor{editor}
{
    _editor->property_wads().signal_changed().connect(
        sigc::mem_fun(*this, &TextureSelector::on_wads_changed));

    auto const builder = Gtk::Builder::create_from_resource(
        SE_GRESOURCE_PREFIX "gtk/TextureSelector.glade");
    builder->get_widget("textureselector", _dialog);
    builder->get_widget("search", _search);
    builder->get_widget("flow", _flow);
    builder->get_widget("cancel", _cancel);
    builder->get_widget("confirm", _confirm);

    _dialog->signal_response().connect([this](int){_dialog->hide();});

    _search->signal_search_changed().connect(
        sigc::mem_fun(*this, &TextureSelector::on_search_changed));

    _flow->set_filter_func(
        sigc::mem_fun(*this, &TextureSelector::filter_func));

    _cancel->signal_clicked().connect(
        [this](){_dialog->response(Gtk::ResponseType::RESPONSE_CANCEL);});
    _confirm->signal_clicked().connect(
        [this](){_dialog->response(Gtk::ResponseType::RESPONSE_ACCEPT);});

    _refresh_textures();
}


std::string TextureSelector::get_selected_texture() const
{
    auto const selected = _flow->get_selected_children();
    auto const child = dynamic_cast<TextureImage *>(
        selected.at(0)->get_child());
    return child->get_name();
}


int TextureSelector::run()
{
    _dialog->show_all();
    return _dialog->run();
}



void TextureSelector::on_wads_changed()
{
    _refresh_textures();
}


void TextureSelector::on_search_changed()
{
    _flow->invalidate_filter();
}


bool TextureSelector::filter_func(Gtk::FlowBoxChild const *child)
{
    auto const image = dynamic_cast<TextureImage const *>(child->get_child());

    auto const &search = _search->get_text();
    Glib::ustring const name{image->get_name()};

    // Search behaviour: Filter in any textures whose name includes SEARCH as a
    // substring, ignoring case.
    auto const search_cmp = search.casefold();
    auto const name_cmp = name.casefold();

    auto const it = name_cmp.find(search_cmp);
    return it != Glib::ustring::npos;
}



void TextureSelector::_refresh_textures()
{
    _clear_textures();
    _add_textures();
}


void TextureSelector::_clear_textures()
{
    for (auto &img : _images)
        _flow->remove(img);
    _images.clear();
}


void TextureSelector::_add_textures()
{
    auto texman = _editor->get_texman();
    for (auto const &kv : texman->lumps)
    {
        auto const &texlump = texman->at(kv.first);
        auto &image = _images.emplace_back(kv.first, texlump);
        _flow->add(image);
    }
}
