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

#ifndef SE_APPWIN_PROPERTYEDITOR_CELLRENDERERTEXTURE_HPP
#define SE_APPWIN_PROPERTYEDITOR_CELLRENDERERTEXTURE_HPP

#include <glibmm/property.h>
#include <gtkmm/cellrenderer.h>
#include <gtkmm/filechoosernative.h>

namespace Sickle::AppWin
{
    /**
     * CellRenderer used to display textures.
     */
    class CellRendererTexture : public Gtk::CellRenderer
    {
    public:
        CellRendererTexture();
        virtual ~CellRendererTexture() = default;

        /** Name of the texture being displayed. */
        auto property_texture_name() { return _prop_texture_name.get_proxy(); }

        auto property_texture_name() const
        {
            return _prop_texture_name.get_proxy();
        }

        /** WAD to start in when selecting a texture. */
        auto property_wad_name() { return _prop_wad_name.get_proxy(); }

        auto property_wad_name() const { return _prop_wad_name.get_proxy(); }

        /** Emitted when the texture is changed. */
        auto signal_texture_edited() { return _sig_texture_edited; }

    protected:
        virtual void render_vfunc(
            Cairo::RefPtr<Cairo::Context> const &cr,
            Gtk::Widget &widget,
            Gdk::Rectangle const &background_area,
            Gdk::Rectangle const &cell_area,
            Gtk::CellRendererState flags) override;

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

    private:
        static constexpr int icon_padding = 3;

        Glib::Property<std::string> _prop_texture_name;
        Glib::Property<std::string> _prop_wad_name;

        sigc::signal<void(Glib::ustring const &, std::string const &)>
            _sig_texture_edited{};

        Glib::RefPtr<Gdk::Pixbuf> _get_icon(Gtk::Widget &widget) const;
        Gdk::Rectangle _get_icon_area(
            Gtk::Widget &widget,
            Gdk::Rectangle const &cell_area) const;

        Glib::RefPtr<Pango::Layout> _get_layout(Gtk::Widget &widget) const;
    };
} // namespace Sickle::AppWin

#endif
