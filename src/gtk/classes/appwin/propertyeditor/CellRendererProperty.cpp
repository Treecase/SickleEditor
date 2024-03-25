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
,   renderer{&_text_renderer}
,   _prop_choices_model{
        *this,
        "choices-model",
        Glib::RefPtr<Gtk::TreeModel>{nullptr}}
,   _prop_value{*this, "value"}
,   _bind_choices_model{
        Glib::Binding::bind_property(
            property_choices_model(),
            dynamic_cast<Gtk::CellRendererCombo *>(
                _choices_renderer.renderer())->property_model())}
{
    property_value().signal_changed().connect(
        sigc::mem_fun(*this, &CellRendererProperty::on_value_changed));

    _choices_renderer.signal_changed.connect(signal_changed().make_slot());
    _color_renderer.signal_changed.connect(signal_changed().make_slot());
    _file_renderer.signal_changed.connect(signal_changed().make_slot());
    _flags_renderer.signal_changed.connect(signal_changed().make_slot());
    _integer_renderer.signal_changed.connect(signal_changed().make_slot());
    _text_renderer.signal_changed.connect(signal_changed().make_slot());
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
        {Sickle::Editor::PropertyType::CHOICES, &_choices_renderer},
        {Sickle::Editor::PropertyType::COLOR255, &_color_renderer},
        {Sickle::Editor::PropertyType::DECAL, &_file_renderer},
        {Sickle::Editor::PropertyType::FLAGS, &_flags_renderer},
        {Sickle::Editor::PropertyType::INTEGER, &_integer_renderer},
        {Sickle::Editor::PropertyType::SOUND, &_file_renderer},
        {Sickle::Editor::PropertyType::SPRITE, &_file_renderer},
        {Sickle::Editor::PropertyType::STRING, &_text_renderer},
        // TODO: maybe a proper visual model browser?
        {Sickle::Editor::PropertyType::STUDIO, &_file_renderer},
    };
    renderer = &_text_renderer;

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
    property_mode() = renderer->mode();
}
