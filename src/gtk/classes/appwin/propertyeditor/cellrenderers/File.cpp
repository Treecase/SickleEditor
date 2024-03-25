/**
 * File.cpp - CellRenderer for file properties (decal, sprite, and sound).
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

#include "CellRendererProperty.hpp"

#include <giomm/settings.h>

#include <config/appid.hpp>


using namespace Sickle::AppWin;


struct TypeInfo
{
    Glib::ustring filter_pattern{"*"};
    std::string start_path{""};
    Glib::ustring title{"Choose File"};
};


CellRendererProperty::FileRenderer::FileRenderer()
{
    auto const _settings = Gio::Settings::create(SE_APPLICATION_ID);
    _renderer.property_base_path() = _settings->get_string("game-root-path");
    _renderer.signal_path_edited().connect(
        sigc::mem_fun(*this, &FileRenderer::on_renderer_path_edited));
}


void CellRendererProperty::FileRenderer::set_value(ValueType const &value)
{
    static std::unordered_map<
        Sickle::Editor::PropertyType,
        TypeInfo> const BASE_PATHS
    {
        {   Sickle::Editor::PropertyType::DECAL,
            {"*", "decals",  "Choose Decal"}},
        {   Sickle::Editor::PropertyType::SOUND,
            {"*.wav", "sound",   "Choose Sound"}},
        {   Sickle::Editor::PropertyType::SPRITE,
            {"*", "sprites", "Choose Sprite"}},
    };

    TypeInfo info{};
    try {
        info = BASE_PATHS.at(value.type->type());
    }
    catch (std::out_of_range const &) {
    }

    auto const start_path = Gio::File::create_for_path(
        _renderer.property_base_path())->get_child(info.start_path);
    auto filter = Gtk::FileFilter::create();
    filter->add_pattern(info.filter_pattern);

    _renderer.property_filter() = filter;
    _renderer.property_path() = Glib::filename_from_utf8(value.value);
    _renderer.property_start_path() = start_path->get_path();
    _renderer.property_title() = info.title;
}


Gtk::CellRenderer *CellRendererProperty::FileRenderer::renderer()
{
    return &_renderer;
}


Gtk::CellRendererMode CellRendererProperty::FileRenderer::mode()
{
    return Gtk::CellRendererMode::CELL_RENDERER_MODE_ACTIVATABLE;
}



void CellRendererProperty::FileRenderer::on_renderer_path_edited(
    Glib::ustring const &path,
    Glib::ustring const &filepath)
{
    signal_changed.emit(path, Glib::filename_to_utf8(filepath));
}
