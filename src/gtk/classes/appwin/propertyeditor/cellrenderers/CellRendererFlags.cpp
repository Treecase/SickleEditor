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
,   _prop_bits_per_row{*this, "bits-per-row", 16}
,   _prop_column_padding{*this, "column-padding", 1}
,   _prop_flags{*this, "flags", 0}
,   _prop_mask{*this, "mask", 0xffffffff}
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
        auto const rect = _get_cell_rect_for_size(
            bit,
            (   cell_area.get_width()
                - x_offset
                - 2 * xpad
                - padding.get_left() - padding.get_right()
                - border.get_left() - border.get_right()),
            (   cell_area.get_height()
                - y_offset
                - 2 * ypad
                - padding.get_top() - padding.get_bottom()
                - border.get_top() - border.get_bottom()));
        context->set_state(state | _get_cell_state(bit));
        render_bit(
            context,
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
            auto const rect = _get_cell_rect_for_size(
                bit,
                cell_area.get_width(),
                cell_area.get_height());
            if (rect.intersects(click_rect))
            {
                auto flags = property_flags().get_value();
                if (is_bit_set(flags, bit))
                    flags = clear_bit(flags, bit);
                else
                    flags = set_bit(flags, bit);
                property_flags().set_value(flags & property_mask());
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
}


void CellRendererFlags::get_preferred_width_vfunc(
    Gtk::Widget &widget,
    int &minimum_width,
    int &natural_width) const
{
    int const column_count = property_bits_per_row();
    int const padding_count = std::max(0, column_count - 1);

    minimum_width = column_count + padding_count;
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

    minimum_height = row_count + padding_count;
    natural_height = minimum_height;
}


void CellRendererFlags::get_preferred_width_for_height_vfunc(
    Gtk::Widget &widget,
    int height,
    int &minimum_width,
    int &natural_width) const
{
    get_preferred_width(widget, minimum_width, natural_width);
}


void CellRendererFlags::get_preferred_height_for_width_vfunc(
    Gtk::Widget &widget,
    int width,
    int &minimum_height,
    int &natural_height) const
{
    get_preferred_height(widget, minimum_height, natural_height);
}


void CellRendererFlags::render_bit(
    Glib::RefPtr<Gtk::StyleContext> const &context,
    Cairo::RefPtr<Cairo::Context> const &cr,
    double x, double y,
    double width, double height) const
{
    context->context_save();
    cr->save();
    auto const fg = context->get_color(context->get_state());
    auto const bg = context->get_color(
        context->get_state() | Gtk::StateFlags::STATE_FLAG_INSENSITIVE);

    if (context->get_state() & Gtk::StateFlags::STATE_FLAG_CHECKED)
    {
        cr->set_source_rgba(
            fg.get_red(),
            fg.get_green(),
            fg.get_blue(),
            fg.get_alpha());
    }
    else
    {
        cr->set_source_rgba(
            bg.get_red(),
            bg.get_green(),
            bg.get_blue(),
            bg.get_alpha());
    }
    cr->rectangle(x, y, width, height);
    cr->fill();

    cr->set_antialias(Cairo::Antialias::ANTIALIAS_NONE);
    cr->set_line_width(1.0);
    cr->set_source_rgba(
        fg.get_red(),
        fg.get_green(),
        fg.get_blue(),
        fg.get_alpha());
    cr->rectangle(x+1, y+1, width-1, height-1);
    cr->stroke();

    cr->restore();
    context->context_restore();
}



Gdk::Rectangle CellRendererFlags::_get_cell_rect_for_size(
    int bit,
    int width,
    int height) const
{
    int const num_columns = property_bits_per_row();
    int const num_rows = static_cast<int>(
        std::ceil(
            static_cast<float>(BITS_IN_INT)
            / static_cast<float>(property_bits_per_row())));

    int const num_column_spacers = std::max(0, num_columns - 1);
    int const num_row_spacers = std::max(0, num_rows - 1);

    // Calculate horizontal size.
    int const used_by_column_spacers = (
        property_column_padding()
        * num_column_spacers);
    int const usable_for_columns = width - used_by_column_spacers;
    int const cell_width = usable_for_columns / num_columns;

    // Calculate vertical size.
    int const used_by_row_spacers = property_row_padding() * num_row_spacers;
    int const usable_for_rows = height - used_by_row_spacers;
    int const cell_height = usable_for_rows / num_rows;

    // Cells are square, so cell size will be the minimum dimension.
    int const cell_size = std::min(cell_width, cell_height);

    // Calculate horizontal padding.
    int const unused_horizontal = (
        width
        - num_column_spacers
        - num_columns * cell_size);
    int const padding_horizontal = unused_horizontal / 2;

    // Calculate vertical padding.
    int const unused_vertical = (
        height
        - num_row_spacers
        - num_rows * cell_size);
    int const padding_vertical = unused_vertical / 2;

    int const bit_column_idx = bit % property_bits_per_row();
    int const bit_row_idx = bit / property_bits_per_row();
    return {
        (   padding_horizontal
            + bit_column_idx * property_column_padding()
            + bit_column_idx * cell_size),
        (   padding_vertical
            + bit_row_idx * property_row_padding()
            + bit_row_idx * cell_size),
        cell_size,
        cell_size};
}


Gtk::StateFlags CellRendererFlags::_get_cell_state(int bit) const
{
    Gtk::StateFlags state = Gtk::StateFlags::STATE_FLAG_NORMAL;
    if (is_bit_set(property_flags(), bit))
        state |= Gtk::StateFlags::STATE_FLAG_CHECKED;
    if (!is_bit_set(property_mask(), bit))
        state |= Gtk::StateFlags::STATE_FLAG_INSENSITIVE;

    return state;
}


void CellRendererFlags::_on_activatable_changed()
{
    property_mode() = property_activatable().get_value()
        ? Gtk::CellRendererMode::CELL_RENDERER_MODE_ACTIVATABLE
        : Gtk::CellRendererMode::CELL_RENDERER_MODE_INERT;
}
