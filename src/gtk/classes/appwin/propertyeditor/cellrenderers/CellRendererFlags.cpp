/**
 * CellRendererFlags.cpp - Custom CellRenderer for flags.
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

// TODO: Make this resize dynamically.
// TODO: Custom CSS integration.

#include "CellRendererFlags.hpp"


using namespace Sickle::AppWin;


static bool is_bit_set(uint32_t flags, int bit)
{
    return (flags >> bit) & 1;
}

static uint32_t set_bit(uint32_t flags, int bit)
{
    return flags | (1 << bit);
}

static uint32_t clear_bit(uint32_t flags, int bit)
{
    return flags & ~(1 << bit);
}


CellRendererFlags::CellRendererFlags()
:   Glib::ObjectBase{typeid(CellRendererFlags)}
,   _prop_activatable{*this, "activatable", false}
,   _prop_bit_size{*this, "bit-size", 16}
,   _prop_bits_per_row{*this, "bits-per-row", 16}
,   _prop_column_padding{*this, "column-padding", 1}
,   _prop_flags{*this, "flags", 0}
,   _prop_row_padding{*this, "row-padding", 1}
{
    property_activatable().signal_changed().connect(
        sigc::mem_fun(*this, &CellRendererFlags::_on_activatable_changed));
}


void CellRendererFlags::render_vfunc(
    Cairo::RefPtr<Cairo::Context> const &cr,
    Gtk::Widget &widget,
    Gdk::Rectangle const &background_area,
    Gdk::Rectangle const &cell_area,
    Gtk::CellRendererState flags)
{
    auto context = widget.get_style_context();

    /* Figuring out size. */
    Gtk::Requisition minimum_size{}, natural_size{};
    get_preferred_size(widget, minimum_size, natural_size);
    gint const x_offset = 0;
    gint const y_offset = 0;
    gint width = natural_size.width;
    gint height = natural_size.height;

    gint xpad, ypad;
    get_padding(xpad, ypad);
    width -= xpad * 2;
    height -= ypad * 2;

    if (width <= 0 || height <= 0)
        return;

    /* Setting up state flags. */
    auto state = get_state(widget, flags);
    if (!property_activatable())
        state |= Gtk::StateFlags::STATE_FLAG_INSENSITIVE;

    /* Draw the cell. */
    cr->save();
    cr->rectangle(
        cell_area.get_x(),
        cell_area.get_y(),
        cell_area.get_width(),
        cell_area.get_height());
    cr->clip();

    context->context_save();
    context->set_state(state);

    context->render_background(
        cr,
        cell_area.get_x() + x_offset + xpad,
        cell_area.get_y() + y_offset + ypad,
        width,
        height);
    context->render_frame(
        cr,
        cell_area.get_x() + x_offset + xpad,
        cell_area.get_y() + y_offset + ypad,
        width,
        height);

    auto const padding = context->get_padding(context->get_state());
    auto const border = context->get_border(context->get_state());

    context->context_save();
    context->add_class("cell");
    for (int bit = 0; bit < BITS_IN_INT; ++bit)
    {
        auto const rect = _get_cell_rect(bit);
        context->set_state(
            (   state
                & ( Gtk::StateFlags::STATE_FLAG_SELECTED
                    | Gtk::StateFlags::STATE_FLAG_PRELIGHT))
                ? Gtk::StateFlags::STATE_FLAG_NORMAL
                : Gtk::StateFlags::STATE_FLAG_PRELIGHT);
        context->render_background(
            cr,
            (   cell_area.get_x()
                + x_offset
                + xpad
                + padding.get_left()
                + border.get_left()
                + rect.get_x()),
            (   cell_area.get_y()
                + y_offset
                + ypad
                + padding.get_top()
                + border.get_top()
                + rect.get_y()
            ),
            rect.get_width(),
            rect.get_height());
        context->set_state(state | _get_cell_state(bit));
        context->render_option(
            cr,
            (   cell_area.get_x()
                + x_offset
                + xpad
                + padding.get_left()
                + border.get_left()
                + rect.get_x()),
            (   cell_area.get_y()
                + y_offset
                + ypad
                + padding.get_top()
                + border.get_top()
                + rect.get_y()
            ),
            rect.get_width(),
            rect.get_height());
    }
    context->context_restore();

    context->context_restore();
    cr->restore();
}


bool CellRendererFlags::activate_vfunc(
    GdkEvent *event,
    Gtk::Widget &widget,
    Glib::ustring const &path,
    Gdk::Rectangle const &background_area,
    Gdk::Rectangle const &cell_area,
    Gtk::CellRendererState flags)
{
    if (property_activatable())
    {
        if (event->type != GdkEventType::GDK_BUTTON_PRESS)
            return false;

        Gdk::Rectangle const click_rect{
            static_cast<int>(event->button.x) - cell_area.get_x(),
            static_cast<int>(event->button.y) - cell_area.get_y(),
            1,
            1};

        for (int bit = 0; bit < BITS_IN_INT; ++bit)
        {
            auto const rect = _get_cell_rect(bit);
            if (rect.intersects(click_rect))
            {
                auto flags = property_flags().get_value();
                if (is_bit_set(flags, bit))
                    flags = clear_bit(flags, bit);
                else
                    flags = set_bit(flags, bit);
                property_flags().set_value(flags);
                signal_flag_changed().emit(path);
                return true;
            }
        }
    }
    return false;
}


Gtk::SizeRequestMode CellRendererFlags::get_request_mode_vfunc() const
{
    return Gtk::SizeRequestMode::SIZE_REQUEST_CONSTANT_SIZE;
    // return Gtk::SizeRequestMode::SIZE_REQUEST_HEIGHT_FOR_WIDTH;
}


void CellRendererFlags::get_preferred_width_vfunc(
    Gtk::Widget &widget,
    int &minimum_width,
    int &natural_width) const
{
    int const column_count = property_bits_per_row();
    int const padding_count = std::max(0, column_count - 1);

    minimum_width = (
        column_count * property_bit_size()
        + padding_count * property_column_padding()
    );
    natural_width = minimum_width;
}


void CellRendererFlags::get_preferred_height_vfunc(
    Gtk::Widget &widget,
    int &minimum_height,
    int &natural_height) const
{
    int const row_count = std::ceil(
        (float)BITS_IN_INT / (float)property_bits_per_row());
    int const padding_count = std::max(0, row_count - 1);

    minimum_height = (
        row_count * property_bit_size()
        + padding_count * property_row_padding()
    );
    natural_height = minimum_height;
}


// void CellRendererFlags::get_preferred_width_for_height_vfunc(
//     Gtk::Widget &widget,
//     int height,
//     int &minimum_width,
//     int &natural_width) const
// {
// }


// void CellRendererFlags::get_preferred_height_for_width_vfunc(
//     Gtk::Widget &widget,
//     int width,
//     int &minimum_height,
//     int &natural_height) const
// {
// }



Gdk::Rectangle CellRendererFlags::_get_cell_rect(int bit) const
{
    int const col = bit % property_bits_per_row();
    int const row = bit / property_bits_per_row();
    return {
        col * property_column_padding() + col * property_bit_size(),
        row * property_row_padding() + row * property_bit_size(),
        property_bit_size(),
        property_bit_size()
    };
}


Gtk::StateFlags CellRendererFlags::_get_cell_state(int bit) const
{
    if (is_bit_set(property_flags(), bit))
        return Gtk::StateFlags::STATE_FLAG_CHECKED;
    else
        return Gtk::StateFlags::STATE_FLAG_NORMAL;
}


void CellRendererFlags::_on_activatable_changed()
{
    property_mode() = property_activatable().get_value()
        ? Gtk::CellRendererMode::CELL_RENDERER_MODE_ACTIVATABLE
        : Gtk::CellRendererMode::CELL_RENDERER_MODE_INERT;
}
