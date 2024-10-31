/**
 * CellRendererTexture.hpp - Custom CellRenderer for textures.
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

#include "CellRendererTexture.hpp"

#include <gtk/classes/textureselector/TextureSelector.hpp>

#include <glibmm/convert.h>
#include <gtkmm/icontheme.h>

using namespace Sickle::AppWin;

CellRendererTexture::CellRendererTexture()
: Glib::ObjectBase{typeid(CellRendererTexture)}
, _prop_texture_name{*this, "texture-name", ""}
, _prop_wad_name{*this, "wad-name", ""}
{
    property_mode() = Gtk::CellRendererMode::CELL_RENDERER_MODE_ACTIVATABLE;
}

void CellRendererTexture::render_vfunc(
    Cairo::RefPtr<Cairo::Context> const &cr,
    Gtk::Widget &widget,
    Gdk::Rectangle const &background_area,
    Gdk::Rectangle const &cell_area,
    Gtk::CellRendererState flags)
{
    auto const layout = _get_layout(widget);
    auto const context = widget.get_style_context();
    auto state = get_state(widget, flags);

    auto const icon = _get_icon(widget);
    auto const icon_area = _get_icon_area(widget, cell_area);

    context->context_save();
    context->set_state(state);

    context->render_background(
        cr,
        background_area.get_x(),
        background_area.get_y(),
        background_area.get_width(),
        background_area.get_height());

    auto const xpad = property_xpad().get_value();
    auto const ypad = property_ypad().get_value();

    layout->set_ellipsize(Pango::EllipsizeMode::ELLIPSIZE_END);
    layout->set_width(
        std::max(0U, cell_area.get_width() - 2 * xpad - icon_area.get_width())
        * PANGO_SCALE);

    int text_width, text_height;
    layout->get_pixel_size(text_width, text_height);

    cr->save();

    cr->rectangle(
        cell_area.get_x(),
        cell_area.get_y(),
        cell_area.get_width(),
        cell_area.get_height());
    cr->clip();

    context->render_icon(cr, icon, icon_area.get_x(), icon_area.get_y());

    context->render_layout(
        cr,
        (cell_area.get_x() + xpad + icon_area.get_width() + icon_padding),
        (cell_area.get_y() + ypad + cell_area.get_height() / 2
         - text_height / 2),
        layout);

    cr->restore();
    context->context_restore();
}

bool CellRendererTexture::activate_vfunc(
    GdkEvent *event,
    Gtk::Widget &widget,
    Glib::ustring const &path,
    Gdk::Rectangle const &background_area,
    Gdk::Rectangle const &cell_area,
    Gtk::CellRendererState flags)
{
    if (event && event->type == GdkEventType::GDK_BUTTON_PRESS)
    {
        auto const icon_area = _get_icon_area(widget, cell_area);

        Gdk::Rectangle const click_rect{
            static_cast<int>(event->button.x),
            static_cast<int>(event->button.y),
            1,
            1};

        if (!icon_area.intersects(click_rect))
        {
            return false;
        }
    }

    auto ts = TextureSelector::TextureSelector::create();
    ts->set_wad_filter(property_wad_name());
    int const response = ts->run();
    if (response != GTK_RESPONSE_ACCEPT)
    {
        return false;
    }

    auto const texture_name = ts->get_selected_texture();
    signal_texture_edited().emit(path, texture_name);

    return true;
}

void CellRendererTexture::get_preferred_width_vfunc(
    Gtk::Widget &widget,
    int &minimum_width,
    int &natural_width) const
{
    int text_width, _;
    auto const layout = _get_layout(widget);
    layout->get_pixel_size(text_width, _);

    auto const icon_area = _get_icon_area(widget, Gdk::Rectangle{});
    auto const icon_width = icon_area.get_width();

    natural_width = text_width + icon_padding + icon_width;
    minimum_width = natural_width;
}

void CellRendererTexture::get_preferred_height_vfunc(
    Gtk::Widget &widget,
    int &minimum_height,
    int &natural_height) const
{
    int _, text_height;
    auto const layout = _get_layout(widget);
    layout->get_pixel_size(_, text_height);

    auto const icon_area = _get_icon_area(widget, Gdk::Rectangle{});
    auto const icon_height = icon_area.get_height();

    natural_height = std::max(text_height, icon_height);
    minimum_height = natural_height;
}

Gtk::SizeRequestMode CellRendererTexture::get_request_mode_vfunc() const
{
    return Gtk::SizeRequestMode::SIZE_REQUEST_CONSTANT_SIZE;
}

Glib::RefPtr<Gdk::Pixbuf> CellRendererTexture::_get_icon(
    Gtk::Widget &widget) const
{
    auto const icontheme = Gtk::IconTheme::get_for_screen(widget.get_screen());
    return icontheme->load_icon("folder", GTK_ICON_SIZE_MENU);
}

Gdk::Rectangle CellRendererTexture::_get_icon_area(
    Gtk::Widget &widget,
    Gdk::Rectangle const &cell_area) const
{
    auto const icon = _get_icon(widget);
    return Gdk::Rectangle{
        static_cast<int>(cell_area.get_x() + property_xpad().get_value()),
        static_cast<int>(
            cell_area.get_y() + property_ypad().get_value()
            + cell_area.get_height() / 2 - icon->get_height() / 2),
        icon->get_width(),
        icon->get_height()};
}

Glib::RefPtr<Pango::Layout> CellRendererTexture::_get_layout(
    Gtk::Widget &widget) const
{
    auto const layout
        = widget.create_pango_layout(property_texture_name().get_value());
    return layout;
}
