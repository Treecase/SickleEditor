/**
 * Flags.cpp - CellRenderer for Flags properties.
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

#include <gtkmm/dialog.h>
#include <gtkmm/flowbox.h>
#include <gtkmm/checkbutton.h>

#include <array>
#include <sstream>


using namespace Sickle::AppWin;


CellRendererProperty::FlagsRenderer::FlagsRenderer()
{
    _renderer.property_activatable() = true;
    _renderer.signal_flag_changed().connect(
        sigc::mem_fun(*this, &FlagsRenderer::on_renderer_flag_changed));
}


void CellRendererProperty::FlagsRenderer::set_value(ValueType const &value)
{
    uint32_t flags = 0;
    std::stringstream ss{value.value};
    ss >> flags;

    auto const &type =\
        std::dynamic_pointer_cast<Editor::EntityPropertyDefinitionFlags>(
            value.type);

    uint32_t mask = 0;
    for (int i = 0; i < 32; ++i)
        mask |= (type->is_bit_defined(i)? (1 << i) : 0);

    _renderer.property_flags() = flags;
    _renderer.property_mask() = mask;
}


Gtk::CellRenderer *CellRendererProperty::FlagsRenderer::renderer()
{
    return &_renderer;
}


Gtk::CellRendererMode CellRendererProperty::FlagsRenderer::mode()
{
    return Gtk::CellRendererMode::CELL_RENDERER_MODE_ACTIVATABLE;
}



void CellRendererProperty::FlagsRenderer::on_renderer_flag_changed(
    Glib::ustring const &path)
{
    signal_changed.emit(path, std::to_string(_renderer.property_flags()));
}
