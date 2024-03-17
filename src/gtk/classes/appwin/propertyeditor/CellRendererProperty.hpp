/**
 * CellRendererProperty.hpp - Custom CellRenderer for Entity properties.
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

#ifndef SE_APPWIN_PROPERTYEDITOR_CELLRENDERERPROPERTY_HPP
#define SE_APPWIN_PROPERTYEDITOR_CELLRENDERERPROPERTY_HPP

#include "cellrenderers/CellRendererFlags.hpp"

#include <editor/world/Entity.hpp>

#include <glibmm/property.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/cellrenderer.h>
#include <gtkmm/cellrenderercombo.h>
#include <gtkmm/cellrendererspin.h>
#include <gtkmm/cellrenderertext.h>
#include <gtkmm/liststore.h>

#include <functional>
#include <memory>


namespace Sickle::AppWin
{
    /**
     * CellRenderer used to display Entity properties.
     *
     * Uses different CellRenderers for different types of property, eg.
     * CellRendererText for strings or a CellRendererSpin for integers.
     */
    class CellRendererProperty : public Gtk::CellRenderer
    {
    public:
        struct ValueType
        {
            std::string value;
            std::shared_ptr<Editor::EntityPropertyDefinition> type;
            ValueType()
            :   value{"<value>"}
            ,   type{nullptr}
            {
            }
            ValueType(
                std::string const &value,
                std::shared_ptr<Editor::EntityPropertyDefinition> const &type)
            :   value{value}
            ,   type{type}
            {
            }
        };

        struct Renderer
        {
            virtual ~Renderer()=default;
            virtual void set_value(ValueType const &value)=0;
            virtual Gtk::CellRenderer *renderer()=0;
            virtual Gtk::CellRendererMode mode()=0;
            sigc::signal<void(
                Glib::ustring const &,
                Glib::ustring const &)> signal_changed{};
        };

        struct ComboRenderer : public Renderer
        {
            struct ChoicesColumnDefs : public Gtk::TreeModelColumnRecord
            {
                ChoicesColumnDefs()
                {
                    add(idx);
                    add(desc);
                }
                Gtk::TreeModelColumn<int> idx{};
                Gtk::TreeModelColumn<Glib::ustring> desc{};
            };
            /// Column definition for the choices menu.
            static ChoicesColumnDefs const columns;
            /// Remaps the editing result from the value's description to the
            // value's integer value.
            std::function<Glib::ustring(
                Glib::ustring const &,
                Glib::ustring const &)> filter_edit{};
            ComboRenderer();
            virtual ~ComboRenderer()=default;
            virtual void set_value(ValueType const &value) override;
            virtual Gtk::CellRenderer *renderer() override;
            virtual Gtk::CellRendererMode mode();
        private:
            Gtk::CellRendererCombo _renderer{};
            void on_edited(Glib::ustring const &, Glib::ustring const &);
        };

        struct FlagsRenderer : public Renderer
        {
            FlagsRenderer();
            virtual ~FlagsRenderer()=default;
            virtual void set_value(ValueType const &value);
            virtual Gtk::CellRenderer *renderer();
            virtual Gtk::CellRendererMode mode();
        private:
            CellRendererFlags _renderer{};
            void on_renderer_flag_changed(Glib::ustring const &);
        };

        struct IntegerRenderer : public Renderer
        {
            IntegerRenderer();
            virtual ~IntegerRenderer()=default;
            virtual void set_value(ValueType const &value) override;
            virtual Gtk::CellRenderer *renderer() override;
            virtual Gtk::CellRendererMode mode();
        private:
            Gtk::CellRendererSpin _renderer{};
        };

        struct StringRenderer : public Renderer
        {
            StringRenderer();
            virtual ~StringRenderer()=default;
            virtual void set_value(ValueType const &value) override;
            virtual Gtk::CellRenderer *renderer() override;
            virtual Gtk::CellRendererMode mode();
        private:
            Gtk::CellRendererText _renderer{};
        };


        CellRendererProperty();
        virtual ~CellRendererProperty()=default;

        /** TreeModel for the Choices renderer to use. */
        auto property_choices_model() {return _prop_choices_model.get_proxy();}

        /** The actual value of the displayed property. */
        auto property_value() {return _prop_value.get_proxy();}
        auto property_value() const {return _prop_value.get_proxy();}

        /** Emitted when the value of the property changes. */
        auto &signal_changed() {return _sig_changed;}


        // FIXME: Temporary cruft to let PropertyEditor access the choices
        // renderer to set up the 'filter_edit' function. Probably the filter
        // should be insinde the combo renderer, with the tree store being a
        // property on the renderer, but that also seems kinda janky?
        //
        // Perhaps change how setting entity properties works to let choice
        // properties be set to the choice text rather than the integer value.
        // Seems like it would be the cleanest option.
        ComboRenderer &choices_renderer() {return _choices_renderer;}


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

        virtual Gtk::CellEditable *start_editing_vfunc(
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


    private:
        ComboRenderer _choices_renderer{};
        FlagsRenderer _flags_renderer{};
        IntegerRenderer _integer_renderer{};
        StringRenderer _text_renderer{};
        Renderer *renderer{nullptr};

        Glib::Property<Glib::RefPtr<Gtk::TreeModel>> _prop_choices_model;
        Glib::Property<ValueType> _prop_value;

        Glib::RefPtr<Glib::Binding> _bind_choices_model{nullptr};

        sigc::signal<void(
            Glib::ustring const &,
            Glib::ustring const &)> _sig_changed{};

        void on_value_changed();

        void on_choices_edited(
            Glib::ustring const &path,
            Glib::ustring const &value);
        void on_flags_edited(
            Glib::ustring const &path,
            Glib::ustring const &value);
        void on_integer_edited(
            Glib::ustring const &path,
            Glib::ustring const &value);
        void on_text_edited(
            Glib::ustring const &path,
            Glib::ustring const &value);
    };
}

#endif
