/**
 * String.cpp - Custom CellRenderer for String properties.
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


using namespace Sickle::AppWin;


CellRendererProperty::StringRenderer::StringRenderer()
{
    _renderer.property_editable() = true;
    _renderer.signal_edited().connect(signal_changed.make_slot());
}


void CellRendererProperty::StringRenderer::set_value(ValueType const &value)
{
    _renderer.property_text() = value.value;
}


Gtk::CellRenderer *CellRendererProperty::StringRenderer::renderer()
{
    return &_renderer;
}


Gtk::CellRendererMode CellRendererProperty::StringRenderer::mode()
{
    return Gtk::CellRendererMode::CELL_RENDERER_MODE_EDITABLE;
}
