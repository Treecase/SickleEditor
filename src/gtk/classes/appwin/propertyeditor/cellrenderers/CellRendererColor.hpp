/**
 * CellRendererColor.hpp - Custom CellRenderer for colors.
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

#ifndef SE_APPWIN_PROPERTYEDITOR_CELLRENDERERCOLOR_HPP
#define SE_APPWIN_PROPERTYEDITOR_CELLRENDERERCOLOR_HPP

#include <glibmm/binding.h>
#include <glibmm/property.h>
#include <gtkmm/cellrenderer.h>
#include <gtkmm/colorchooserdialog.h>

#include <memory>

namespace Sickle::AppWin
{
    /**
     * CellRenderer used to display RGBA colors.
     */
    class CellRendererColor : public Gtk::CellRenderer
    {
    public:
        CellRendererColor();
        virtual ~CellRendererColor() = default;

        /** The color value being displayed. */
        auto property_rgba() { return _prop_rgba.get_proxy(); }

        auto property_rgba() const { return _prop_rgba.get_proxy(); }

        /** Emitted when the color is edited. */
        auto &signal_rgba_edited() { return _sig_rgba_edited; }

    protected:
        virtual bool activate_vfunc(
            GdkEvent *event,
            Gtk::Widget &widget,
            Glib::ustring const &path,
            Gdk::Rectangle const &background_area,
            Gdk::Rectangle const &cell_area,
            Gtk::CellRendererState flags) override;

        virtual void get_preferred_width_vfunc(
            Gtk::Widget &widget,
            int &minimum_width,
            int &natural_width) const override;

        virtual void get_preferred_height_vfunc(
            Gtk::Widget &widget,
            int &minimum_height,
            int &natural_height) const override;

        virtual Gtk::SizeRequestMode get_request_mode_vfunc() const override;

        virtual void render_vfunc(
            Cairo::RefPtr<Cairo::Context> const &cr,
            Gtk::Widget &widget,
            Gdk::Rectangle const &background_area,
            Gdk::Rectangle const &cell_area,
            Gtk::CellRendererState flags) override;

    private:
        /* Width of the swatch area is SWATCH_ASPECT * swatch_height */
        static constexpr int SWATCH_ASPECT = 3;

        Glib::Property<Gdk::RGBA> _prop_rgba;
        sigc::signal<void(Glib::ustring const &, Gdk::RGBA const &)>
            _sig_rgba_edited{};

        Gtk::ColorChooserDialog _ccd{};

        Gdk::Rectangle _get_swatch_rect(Gdk::Rectangle const &cell_area) const;
    };
} // namespace Sickle::AppWin

#endif
