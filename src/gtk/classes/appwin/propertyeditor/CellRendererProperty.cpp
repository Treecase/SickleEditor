/**
 * CellRendererProperty.cpp - Custom CellRenderer for Entity properties.
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

#include <limits>
#include <sstream>


using namespace Sickle::AppWin;


CellRendererProperty::CellRendererProperty()
:   Glib::ObjectBase{typeid(CellRendererProperty)}
,   Gtk::CellRenderer{}
,   _choices_renderer{std::make_unique<ComboRenderer>()}
,   _integer_renderer{std::make_unique<IntegerRenderer>()}
,   _text_renderer{std::make_unique<StringRenderer>()}
,   renderer{_text_renderer.get()}
,   _prop_value{*this, "value"}
{
    property_value().signal_changed().connect(
        sigc::mem_fun(*this, &CellRendererProperty::on_value_changed));

    _choices_renderer->signal_changed.connect(
        sigc::mem_fun(*this, &CellRendererProperty::on_choices_edited));
    _integer_renderer->signal_changed.connect(
        sigc::mem_fun(*this, &CellRendererProperty::on_integer_edited));
    _text_renderer->signal_changed.connect(
        sigc::mem_fun(*this, &CellRendererProperty::on_text_edited));
}



void CellRendererProperty::render_vfunc(
    Cairo::RefPtr<Cairo::Context> const &cr,
    Gtk::Widget &widget,
    Gdk::Rectangle const &background_area,
    Gdk::Rectangle const &cell_area,
    Gtk::CellRendererState flags)
{
    renderer->renderer()->render(cr, widget, background_area, cell_area, flags);
}


bool CellRendererProperty::activate_vfunc(
    GdkEvent *event,
    Gtk::Widget &widget,
    Glib::ustring const &path,
    Gdk::Rectangle const &background_area,
    Gdk::Rectangle const &cell_area,
    Gtk::CellRendererState flags)
{
    return renderer->renderer()->activate(
        event, widget, path, background_area, cell_area, flags);
}


Gtk::CellEditable *CellRendererProperty::start_editing_vfunc(
    GdkEvent *event,
    Gtk::Widget &widget,
    Glib::ustring const &path,
    Gdk::Rectangle const &background_area,
    Gdk::Rectangle const &cell_area,
    Gtk::CellRendererState flags)
{
    return renderer->renderer()->start_editing(
        event, widget, path, background_area, cell_area, flags);
}


Gtk::SizeRequestMode CellRendererProperty::get_request_mode_vfunc() const
{
    return renderer->renderer()->get_request_mode();
}


void CellRendererProperty::get_preferred_width_vfunc(
    Gtk::Widget &widget,
    int &minimum_width,
    int &natural_width) const
{
    renderer->renderer()->get_preferred_width(
        widget, minimum_width, natural_width);
}


void CellRendererProperty::get_preferred_height_vfunc(
    Gtk::Widget &widget,
    int &minimum_height,
    int &natural_height) const
{
    renderer->renderer()->get_preferred_height(
        widget, minimum_height, natural_height);
}


void CellRendererProperty::get_preferred_width_for_height_vfunc(
    Gtk::Widget &widget,
    int height,
    int &minimum_width,
    int &natural_width) const
{
    renderer->renderer()->get_preferred_width_for_height(
        widget, height, minimum_width, natural_width);
}


void CellRendererProperty::get_preferred_height_for_width_vfunc(
    Gtk::Widget &widget,
    int width,
    int &minimum_height,
    int &natural_height) const
{
    renderer->renderer()->get_preferred_height_for_width(
        widget, width, minimum_height, natural_height);
}



void CellRendererProperty::on_value_changed()
{
    static std::unordered_map<Sickle::Editor::PropertyType, Renderer *> const
    RENDERERS
    {
        {Sickle::Editor::PropertyType::CHOICES, _choices_renderer.get()},
        {Sickle::Editor::PropertyType::INTEGER, _integer_renderer.get()},
        {Sickle::Editor::PropertyType::STRING, _text_renderer.get()},
    };
    static auto const DEFAULT_RENDERER = RENDERERS.at(
        Sickle::Editor::PropertyType::STRING);
    renderer = DEFAULT_RENDERER;

    auto const value = property_value().get_value();
    if (auto const type = value.type)
    {
        auto const idx = type->type();
        try {
            renderer = RENDERERS.at(idx);
        }
        catch (std::out_of_range const &e) {
        }
    }

    renderer->set_value(value);
    property_mode() = Gtk::CellRendererMode::CELL_RENDERER_MODE_EDITABLE;
}


void CellRendererProperty::on_choices_edited(
    Glib::ustring const &path,
    Glib::ustring const &value)
{
    signal_changed().emit(path, value);
}


void CellRendererProperty::on_float_edited(
    Glib::ustring const &path,
    Glib::ustring const &value)
{
    signal_changed().emit(path, value);
}


void CellRendererProperty::on_integer_edited(
    Glib::ustring const &path,
    Glib::ustring const &value)
{
    signal_changed().emit(path, value);
}


void CellRendererProperty::on_text_edited(
    Glib::ustring const &path,
    Glib::ustring const &value)
{
    signal_changed().emit(path, value);
}




/* === Renderers ============================================================ */
/* --- ComboRenderer -------------------------------------------------------- */
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


/* --- IntegerRenderer ------------------------------------------------------ */
CellRendererProperty::IntegerRenderer::IntegerRenderer()
{
    _renderer.property_adjustment() = Gtk::Adjustment::create(
        0.0,
        std::numeric_limits<int>::lowest(),
        std::numeric_limits<int>::max(),
        1.0);
    _renderer.property_digits() = 0;
    _renderer.property_editable() = true;
    _renderer.signal_edited().connect(signal_changed.make_slot());
}

void CellRendererProperty::IntegerRenderer::set_value(ValueType const &value)
{
    _renderer.property_text() = value.value;
}

Gtk::CellRenderer *CellRendererProperty::IntegerRenderer::renderer()
{
    return &_renderer;
}


/* --- StringRenderer ------------------------------------------------------- */
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
