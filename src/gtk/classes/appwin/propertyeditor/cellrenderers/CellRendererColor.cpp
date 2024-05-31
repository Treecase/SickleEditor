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

    auto const swatch_rect = _get_swatch_rect(cell_area);


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
        background_area.get_x(),
        background_area.get_y(),
        background_area.get_width(),
        background_area.get_height());

    // Draw the color swatch.
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
        swatch_rect.get_x() + border.get_left() + padding.get_left(),
        swatch_rect.get_y() + border.get_top() + padding.get_top(),
        (   swatch_rect.get_width()
            - border.get_left()
            - border.get_right()
            - padding.get_left()
            - padding.get_right()),
        (   swatch_rect.get_height()
            - border.get_top()
            - border.get_bottom()
            - padding.get_top()
            - padding.get_bottom()));
    cr->fill();
    cr->restore();

    context->render_frame(
        cr,
        swatch_rect.get_x(),
        swatch_rect.get_y(),
        swatch_rect.get_width(),
        swatch_rect.get_height());

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
    if (event && event->type == GdkEventType::GDK_BUTTON_PRESS)
    {
        auto const swatch_rect = _get_swatch_rect(cell_area);

        Gdk::Rectangle const click_rect{
            static_cast<int>(event->button.x),
            static_cast<int>(event->button.y),
            1,
            1};

        if (!swatch_rect.intersects(click_rect))
            return false;
    }

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



Gdk::Rectangle CellRendererColor::_get_swatch_rect(
    Gdk::Rectangle const &cell_area) const
{
    int const xpad = property_xpad().get_value();
    int const ypad = property_ypad().get_value();

    int const height = cell_area.get_height() - ypad * 2;
    int const width = std::min(
        cell_area.get_width() - xpad * 2,
        height * SWATCH_ASPECT);

    return Gdk::Rectangle{
        cell_area.get_x() + xpad,
        cell_area.get_y() + ypad,
        width < 0? 0 : width,
        height < 0? 0 : height};
}
