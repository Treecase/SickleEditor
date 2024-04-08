/**
 * TextureSelector.cpp - Texture selection window.
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

#include "TextureSelector.hpp"

#include <config/appid.hpp>
#include <editor/textures/TextureManager.hpp>

#include <gtkmm/builder.h>


using namespace Sickle::TextureSelector;


Glib::RefPtr<TextureSelector> TextureSelector::create()
{
    return Glib::RefPtr{new TextureSelector{}};
}


TextureSelector::TextureSelector()
:   Glib::ObjectBase{typeid(TextureSelector)}
{
    auto const builder = Gtk::Builder::create_from_resource(
        SE_GRESOURCE_PREFIX "gtk/TextureSelector.glade");
    builder->get_widget("textureselector", _dialog);
    builder->get_widget("search", _search);
    builder->get_widget("wad_filter", _wad_filter);
    builder->get_widget("flow", _flow);
    builder->get_widget("cancel", _cancel);
    builder->get_widget("confirm", _confirm);

    _dialog->signal_response().connect([this](int){_dialog->hide();});

    _search->signal_search_changed().connect(
        sigc::mem_fun(*this, &TextureSelector::on_search_changed));

    _wad_filter->property_active().signal_changed().connect(
        sigc::mem_fun(*this, &TextureSelector::on_wad_filter_changed));

    _flow->set_filter_func(
        sigc::mem_fun(*this, &TextureSelector::filter_func));
    _flow->set_sort_func(
        sigc::mem_fun(*this, &TextureSelector::sort_func));

    _cancel->signal_clicked().connect(
        [this](){_dialog->response(Gtk::ResponseType::RESPONSE_CANCEL);});
    _confirm->signal_clicked().connect(
        [this](){_dialog->response(Gtk::ResponseType::RESPONSE_ACCEPT);});

    auto &texman = Editor::Textures::TextureManager::get_reference();
    texman.signal_wads_changed().connect(
        sigc::mem_fun(*this, &TextureSelector::on_TextureManager_wads_changed));

    _refresh_textures();
}


std::string TextureSelector::get_selected_texture() const
{
    auto const selected = _flow->get_selected_children();
    auto const child = dynamic_cast<TextureImage *>(
        selected.at(0)->get_child());
    auto const &info = child->get_info();
    return info->get_name();
}


void TextureSelector::set_wad_filter(std::string const &filter)
{
    _wad_filter->set_active_text(filter);
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


void TextureSelector::on_wad_filter_changed()
{
    _flow->invalidate_filter();
}


void TextureSelector::on_TextureManager_wads_changed()
{
    _refresh_textures();
}


bool TextureSelector::filter_func(Gtk::FlowBoxChild const *child) const
{
    auto const image = dynamic_cast<TextureImage const *>(child->get_child());
    auto const &texinfo = image->get_info();

    // Exclude textures not in filtered WAD. Special value "*" skips this check.
    auto const filter_wad = _wad_filter->get_active_text();
    Glib::ustring const source_wad{texinfo->get_source_wad()};
    if (filter_wad != "*" && filter_wad != source_wad)
        return false;

    auto const search = _search->get_text();
    Glib::ustring const name{texinfo->get_name()};

    // Search behaviour: Filter in any textures whose name includes SEARCH as a
    // substring, ignoring case.
    auto const search_cmp = search.casefold();
    auto const name_cmp = name.casefold();

    auto const it = name_cmp.find(search_cmp);
    return it != Glib::ustring::npos;
}


int TextureSelector::sort_func(
    Gtk::FlowBoxChild const *a,
    Gtk::FlowBoxChild const *b) const
{
    auto const a_image = dynamic_cast<TextureImage const *>(a->get_child());
    auto const b_image = dynamic_cast<TextureImage const *>(b->get_child());

    auto const &a_texinfo = a_image->get_info();
    auto const &b_texinfo = b_image->get_info();

    auto const a_texture_name = a_texinfo->get_name();
    auto const b_texture_name = b_texinfo->get_name();

    return a_texture_name.compare(b_texture_name);
}



void TextureSelector::_refresh_textures()
{
    _clear_textures();
    _wad_filter->remove_all();
    _wad_filter->append("*");
    auto &texman = Editor::Textures::TextureManager::get_reference();
    for (auto const &wad_name : texman.get_wads())
        _wad_filter->append(wad_name);
    _add_textures();
}


void TextureSelector::_clear_textures()
{
    for (auto &img : _images)
        _flow->remove(*img);
    _images.clear();
}


void TextureSelector::_add_textures()
{
    auto &texman = Editor::Textures::TextureManager::get_reference();
    for (auto const &texinfo : texman.get_textures())
    {
        std::shared_ptr<TextureImage> image{nullptr};
        try {
            image = texinfo->get_cached<TextureImage>();
        }
        catch (std::out_of_range const &) {
            image = std::make_shared<TextureImage>(texinfo);
            texinfo->cache_object(image);
        }
        _images.push_back(image);
        _flow->add(*image);
    }
}
