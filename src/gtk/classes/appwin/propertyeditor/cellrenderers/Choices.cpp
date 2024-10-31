/**
 * Choices.cpp - CellRenderer for Choices properties.
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

CellRendererProperty::ChoicesRenderer::ChoicesRenderer()
: filter_edit{[](auto p, auto v) { return v; }}
{
    _renderer.property_editable() = true;
    _renderer.property_text_column() = columns().desc.index();
    _renderer.signal_edited().connect(
        sigc::mem_fun(*this, &ChoicesRenderer::on_edited));
}

CellRendererProperty::ChoicesRenderer::ChoicesColumnDefs const &
CellRendererProperty::ChoicesRenderer::columns()
{
    static ChoicesColumnDefs the_columns{};
    return the_columns;
}

void CellRendererProperty::ChoicesRenderer::set_value(ValueType const &value)
{
    auto const type
        = std::dynamic_pointer_cast<Editor::EntityPropertyDefinitionChoices>(
            value.type);

    std::stringstream ss{value.value};
    int value_idx = 0;
    ss >> value_idx;

    Glib::ustring display = "";
    try
    {
        display = type->choices().at(value_idx);
    }
    catch (std::out_of_range const &)
    {
        display = value.value;
    }
    _renderer.property_text() = display;
}

Gtk::CellRenderer *CellRendererProperty::ChoicesRenderer::renderer()
{
    return &_renderer;
}

Gtk::CellRendererMode CellRendererProperty::ChoicesRenderer::mode()
{
    return Gtk::CellRendererMode::CELL_RENDERER_MODE_EDITABLE;
}

void CellRendererProperty::ChoicesRenderer::on_edited(
    Glib::ustring const &path,
    Glib::ustring const &displayed)
{
    auto const the_value = std::invoke(filter_edit, path, displayed);
    signal_changed.emit(path, the_value);
}
