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

#ifndef SE_APPWIN_PROPERTYEDITOR_CELLRENDERERFILE_HPP
#define SE_APPWIN_PROPERTYEDITOR_CELLRENDERERFILE_HPP

#include <glibmm/property.h>
#include <gtkmm/cellrenderer.h>
#include <gtkmm/filechoosernative.h>

namespace Sickle::AppWin
{
    /**
     * CellRenderer used to display file paths.
     */
    class CellRendererFile : public Gtk::CellRenderer
    {
    public:
        CellRendererFile();
        virtual ~CellRendererFile() = default;

        /** Path will be relative to here. */
        auto property_base_path() { return _prop_base_path.get_proxy(); }

        auto property_base_path() const { return _prop_base_path.get_proxy(); }

        /** File chooser filter. */
        auto property_filter() { return _prop_filter.get_proxy(); }

        auto property_filter() const { return _prop_filter.get_proxy(); }

        /** The path being displayed. */
        auto property_path() { return _prop_path.get_proxy(); }

        auto property_path() const { return _prop_path.get_proxy(); }

        /** Start path when choosing a new path. */
        auto property_start_path() { return _prop_start_path.get_proxy(); }

        auto property_start_path() const
        {
            return _prop_start_path.get_proxy();
        }

        /** File chooser dialog title. */
        auto property_title() { return _prop_title.get_proxy(); }

        auto property_title() const { return _prop_title.get_proxy(); }

        /** Emitted when a flag bit is changed. */
        auto signal_path_edited() { return _sig_path_edited; }

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

        Glib::Property<std::string> _prop_base_path;
        Glib::Property<Glib::RefPtr<Gtk::FileFilter>> _prop_filter;
        Glib::Property<std::string> _prop_path;
        Glib::Property<std::string> _prop_start_path;
        Glib::Property<Glib::ustring> _prop_title;

        sigc::signal<void(Glib::ustring const &, std::string const &)>
            _sig_path_edited{};

        Glib::RefPtr<Gtk::FileChooserNative> _filechooser{nullptr};

        Glib::RefPtr<Gdk::Pixbuf> _get_icon(Gtk::Widget &widget) const;
        Gdk::Rectangle _get_icon_area(
            Gtk::Widget &widget,
            Gdk::Rectangle const &cell_area) const;

        Glib::RefPtr<Pango::Layout> _get_layout(Gtk::Widget &widget) const;
    };
} // namespace Sickle::AppWin

#endif
