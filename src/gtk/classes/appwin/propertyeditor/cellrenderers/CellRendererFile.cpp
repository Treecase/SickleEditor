/**
 * CellRendererFile.hpp - Custom CellRenderer for files.
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

#include "CellRendererFile.hpp"

#include <glibmm/convert.h>


using namespace Sickle::AppWin;


CellRendererFile::CellRendererFile()
:   Glib::ObjectBase{typeid(CellRendererFile)}
,   _prop_base_path{*this, "base-path", ""}
,   _prop_filter{*this, "filter"}
,   _prop_path{*this, "path", "."}
,   _prop_start_path{*this, "start-path", "."}
,   _prop_title{*this, "title", "Choose File"}
,   _filechooser{
        Gtk::FileChooserNative::create(
            property_title().get_value(),
            Gtk::FileChooserAction::FILE_CHOOSER_ACTION_OPEN)}
{
    property_mode() = Gtk::CellRendererMode::CELL_RENDERER_MODE_ACTIVATABLE;
}


void CellRendererFile::render_vfunc(
    Cairo::RefPtr<Cairo::Context> const &cr,
    Gtk::Widget &widget,
    Gdk::Rectangle const &background_area,
    Gdk::Rectangle const &cell_area,
    Gtk::CellRendererState flags)
{
    auto const layout = get_layout(widget);
    auto const context = widget.get_style_context();
    auto state = get_state(widget, flags);

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
    layout->set_width((cell_area.get_width() - 2 * xpad) * PANGO_SCALE);

    cr->save();

    cr->rectangle(
        cell_area.get_x(),
        cell_area.get_y(),
        cell_area.get_width(),
        cell_area.get_height());
    cr->clip();

    context->render_layout(
        cr,
        cell_area.get_x() + xpad,
        cell_area.get_y() + ypad,
        layout);

    cr->restore();
    context->context_restore();
}


bool CellRendererFile::activate_vfunc(
    GdkEvent *event,
    Gtk::Widget &widget,
    Glib::ustring const &path,
    Gdk::Rectangle const &background_area,
    Gdk::Rectangle const &cell_area,
    Gtk::CellRendererState flags)
{
    _filechooser->set_current_folder(property_start_path());
    _filechooser->set_filter(property_filter());
    _filechooser->set_title(property_title());
    int const response = _filechooser->run();
    _filechooser->hide();

    if (response != GTK_RESPONSE_ACCEPT)
        return false;

    if (auto const file = _filechooser->get_file())
    {
        auto const basepath = Gio::File::create_for_path(
            property_base_path().get_value());
        if (file->has_prefix(basepath))
        {
            auto const filepath = file->get_path().substr(
                property_base_path().get_value().size() + 1);
            signal_path_edited().emit(path, filepath);
            return true;
        }
    }

    return false;
}


void CellRendererFile::get_preferred_width_vfunc(
    Gtk::Widget &widget,
    int &minimum_width,
    int &natural_width) const
{
    int _;
    auto const layout = get_layout(widget);
    layout->get_pixel_size(natural_width, _);
    minimum_width = natural_width;
}


void CellRendererFile::get_preferred_height_vfunc(
    Gtk::Widget &widget,
    int &minimum_height,
    int &natural_height) const
{
    int _;
    auto const layout = get_layout(widget);
    layout->get_pixel_size(_, natural_height);
    minimum_height = natural_height;
}


Gtk::SizeRequestMode CellRendererFile::get_request_mode_vfunc() const
{
    return Gtk::SizeRequestMode::SIZE_REQUEST_CONSTANT_SIZE;
}



Glib::RefPtr<Pango::Layout> CellRendererFile::get_layout(
    Gtk::Widget &widget) const
{
    auto const layout = widget.create_pango_layout(
        Glib::filename_to_utf8(property_path()));
    return layout;
}
