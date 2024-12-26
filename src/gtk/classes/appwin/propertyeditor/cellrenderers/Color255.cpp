/**
 * Color255.cpp - Custom CellRenderer for color255 properties.
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

#include <sstream>

using namespace Sickle::AppWin;

static Gdk::RGBA extract_color(std::string const &value);

CellRendererProperty::Color255Renderer::Color255Renderer()
{
    _renderer.signal_rgba_edited().connect(
        sigc::mem_fun(*this, &Color255Renderer::on_rgba_edited));
}

void CellRendererProperty::Color255Renderer::set_value(ValueType const &value)
{
    _renderer.property_rgba() = extract_color(value.value);
}

Gtk::CellRenderer *CellRendererProperty::Color255Renderer::renderer()
{
    return &_renderer;
}

Gtk::CellRendererMode CellRendererProperty::Color255Renderer::mode()
{
    return Gtk::CellRendererMode::CELL_RENDERER_MODE_ACTIVATABLE;
}

void CellRendererProperty::Color255Renderer::on_rgba_edited(
    Glib::ustring const &path,
    Gdk::RGBA const &rgba)
{
    Glib::ustring const str{
        std::to_string(static_cast<int>(rgba.get_red() * 255)) + " "
        + std::to_string(static_cast<int>(rgba.get_green() * 255)) + " "
        + std::to_string(static_cast<int>(rgba.get_blue() * 255)) + " "
        + std::to_string(static_cast<int>(rgba.get_alpha() * 255))};
    signal_changed.emit(path, str);
}

static Gdk::RGBA extract_color(std::string const &value)
{
    int r = 255, g = 255, b = 255, a = 255;
    std::stringstream ss{value};
    ss >> r >> g >> b >> a;
    Gdk::RGBA rgba{};
    rgba.set_rgba(r / 255.0, g / 255.0, b / 255.0, a / 255.0);
    return rgba;
}
