/**
 * CellRendererColor.cpp - Custom CellRenderer for colors.
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

#include "CellRendererColor.hpp"


using namespace Sickle::AppWin;


CellRendererColor::CellRendererColor()
:   Glib::ObjectBase{typeid(CellRendererColor)}
,   _prop_rgba{*this, "color"}
{
    property_mode() = Gtk::CellRendererMode::CELL_RENDERER_MODE_ACTIVATABLE;
    _ccd.set_use_alpha(true);
}



void CellRendererColor::render_vfunc(
    Cairo::RefPtr<Cairo::Context> const &cr,
    Gtk::Widget &widget,
    Gdk::Rectangle const &background_area,
    Gdk::Rectangle const &cell_area,
    Gtk::CellRendererState flags)
{
    auto context = widget.get_style_context();

    auto const xpad = property_xpad().get_value();
    auto const ypad = property_ypad().get_value();

    auto const width = cell_area.get_width() - xpad * 2;
    auto const height = cell_area.get_height() - ypad * 2;

    if (width <= 0 || height <= 0)
        return;


    auto state = get_state(widget, flags);

    /* Draw the cell. */
    cr->save();
    // Clip drawing area.
    cr->rectangle(
        cell_area.get_x(),
        cell_area.get_y(),
        cell_area.get_width(),
        cell_area.get_height());
    cr->clip();

    // Render cell background.
    context->context_save();
    context->set_state(state);
    context->render_background(
        cr,
        cell_area.get_x() + xpad,
        cell_area.get_y() + ypad,
        width,
        height);

    // Draw the color rect.
    auto const border = context->get_border(context->get_state());
    auto const padding = context->get_padding(context->get_state());
    auto const color = property_rgba().get_value();
    cr->save();
    cr->set_source_rgba(
        color.get_red(),
        color.get_green(),
        color.get_blue(),
        color.get_alpha());
    cr->rectangle(
        cell_area.get_x() + border.get_left() + padding.get_left(),
        cell_area.get_y() + border.get_top() + padding.get_top(),
        (   width
            - border.get_left()
            - border.get_right()
            - padding.get_left()
            - padding.get_right()),
        (   height
            - border.get_top()
            - border.get_bottom()
            - padding.get_top()
            - padding.get_bottom()));
    cr->fill();
    cr->restore();

    context->render_frame(
        cr,
        cell_area.get_x() + xpad,
        cell_area.get_y() + ypad,
        width,
        height);

    context->context_restore();
    cr->restore();
}


bool CellRendererColor::activate_vfunc(
    GdkEvent *event,
    Gtk::Widget &widget,
    Glib::ustring const &path,
    Gdk::Rectangle const &background_area,
    Gdk::Rectangle const &cell_area,
    Gtk::CellRendererState flags)
{
    _ccd.property_rgba().set_value(property_rgba().get_value());
    auto const response = _ccd.run();
    if (response == Gtk::ResponseType::RESPONSE_OK)
        signal_rgba_edited().emit(path, _ccd.property_rgba().get_value());
    _ccd.hide();
    return true;
}


void CellRendererColor::get_preferred_width_vfunc(
    Gtk::Widget &widget,
    int &minimum_width,
    int &natural_width) const
{
    minimum_width = 1;
    natural_width = 1;
}


void CellRendererColor::get_preferred_height_vfunc(
    Gtk::Widget &widget,
    int &minimum_height,
    int &natural_height) const
{
    minimum_height = 1;
    natural_height = 1;
}


Gtk::SizeRequestMode CellRendererColor::get_request_mode_vfunc() const
{
    return Gtk::SizeRequestMode::SIZE_REQUEST_CONSTANT_SIZE;
}
