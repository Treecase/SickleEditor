/**
 * CellRendererFlags.hpp - Custom CellRenderer for flags.
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

#ifndef SE_APPWIN_PROPERTYEDITOR_CELLRENDERERFLAGS_HPP
#define SE_APPWIN_PROPERTYEDITOR_CELLRENDERERFLAGS_HPP

#include <glibmm/property.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/cellrenderer.h>
#include <gtkmm/cellrenderercombo.h>
#include <gtkmm/cellrendererspin.h>
#include <gtkmm/cellrenderertext.h>
#include <gtkmm/liststore.h>

#include <memory>

namespace Sickle::AppWin
{
    /**
     * CellRenderer used to display 32-bit bitwise flags.
     */
    class CellRendererFlags : public Gtk::CellRenderer
    {
    public:
        CellRendererFlags();
        virtual ~CellRendererFlags() = default;

        /** Can the flags be edited? */
        auto property_activatable() { return _prop_activatable.get_proxy(); }

        auto property_activatable() const
        {
            return _prop_activatable.get_proxy();
        }

        /** Number of bits per display row. */
        auto property_bits_per_row() { return _prop_bits_per_row.get_proxy(); }

        auto property_bits_per_row() const
        {
            return _prop_bits_per_row.get_proxy();
        }

        /** Width of columns between the bit cells. */
        auto property_column_padding()
        {
            return _prop_column_padding.get_proxy();
        }

        auto property_column_padding() const
        {
            return _prop_column_padding.get_proxy();
        }

        /** The bitwise flag values being displayed. */
        auto property_flags() { return _prop_flags.get_proxy(); }

        auto property_flags() const { return _prop_flags.get_proxy(); }

        /**
         * The bit mask for displayed values. Masked-out values will display
         * faded and not react to user input.
         */
        auto property_mask() { return _prop_mask.get_proxy(); }

        auto property_mask() const { return _prop_mask.get_proxy(); }

        /** Height of rows between the bit cells. */
        auto property_row_padding() { return _prop_row_padding.get_proxy(); }

        auto property_row_padding() const
        {
            return _prop_row_padding.get_proxy();
        }

        /** Emitted when a flag bit is changed. */
        auto signal_flag_changed() { return _sig_flag_changed; }

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

        virtual Gtk::SizeRequestMode get_request_mode_vfunc() const override;

        virtual void get_preferred_width_vfunc(
            Gtk::Widget &widget,
            int &minimum_width,
            int &natural_width) const override;

        virtual void get_preferred_height_vfunc(
            Gtk::Widget &widget,
            int &minimum_height,
            int &natural_height) const override;

        virtual void get_preferred_width_for_height_vfunc(
            Gtk::Widget &widget,
            int height,
            int &minimum_width,
            int &natural_width) const override;

        virtual void get_preferred_height_for_width_vfunc(
            Gtk::Widget &widget,
            int width,
            int &minimum_height,
            int &natural_height) const override;

        virtual void render_bit(
            Glib::RefPtr<Gtk::StyleContext> const &context,
            Cairo::RefPtr<Cairo::Context> const &cr,
            double x,
            double y,
            double width,
            double height) const;

    private:
        static constexpr int BITS_IN_INT = 32;

        Glib::Property<bool> _prop_activatable;
        Glib::Property<int> _prop_bits_per_row;
        Glib::Property<int> _prop_column_padding;
        Glib::Property<uint32_t> _prop_flags;
        Glib::Property<uint32_t> _prop_mask;
        Glib::Property<int> _prop_row_padding;

        sigc::signal<void(Glib::ustring const &)> _sig_flag_changed{};

        Gdk::Rectangle _get_cell_rect_for_size(int bit, int width, int height)
            const;
        Gtk::StateFlags _get_cell_state(int bit) const;
        void _on_activatable_changed();
    };
} // namespace Sickle::AppWin

#endif
