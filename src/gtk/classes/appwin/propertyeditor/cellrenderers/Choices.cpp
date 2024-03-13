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


CellRendererProperty::ComboRenderer::ComboRenderer()
:   _store{Gtk::ListStore::create(_columns)}
{
    _renderer.property_editable() = true;
    _renderer.property_model() = _store;
    _renderer.property_text_column() = 1;
    _renderer.signal_edited().connect(
        sigc::mem_fun(*this, &ComboRenderer::on_edited));
}


void CellRendererProperty::ComboRenderer::set_value(ValueType const &value)
{
    auto const type =\
        std::dynamic_pointer_cast<Editor::EntityPropertyDefinitionChoices>(
            value.type);

    // TODO: Wrong descriptions sometimes show up when modifying. Not sure why?
    _store->clear();
    for (auto const &kv : type->choices())
    {
        auto it = _store->append();
        it->set_value(_columns.idx, kv.first);
        it->set_value(_columns.desc, Glib::ustring{kv.second});
    }

    std::stringstream ss{value.value};
    int value_idx = 0;
    ss >> value_idx;

    Glib::ustring display = "";
    try {
        display = type->choices().at(value_idx);
    }
    catch (std::out_of_range const &) {
        display = value.value;
    }
    _renderer.property_text() = display;
}


Gtk::CellRenderer *CellRendererProperty::ComboRenderer::renderer()
{
    return &_renderer;
}


Gtk::CellRendererMode CellRendererProperty::ComboRenderer::mode()
{
    return Gtk::CellRendererMode::CELL_RENDERER_MODE_EDITABLE;
}


void CellRendererProperty::ComboRenderer::on_edited(
    Glib::ustring const &path,
    Glib::ustring const &displayed)
{
    auto the_value = displayed;

    _store->foreach_iter(
        [this, displayed, &the_value](
            Gtk::TreeModel::iterator const &it) -> bool
        {
            auto const desc = it->get_value(_columns.desc);
            if (desc == displayed)
            {
                the_value = std::to_string(it->get_value(_columns.idx));
                return true;
            }
            else
                return false;
        });

    signal_changed.emit(path, the_value);
}
