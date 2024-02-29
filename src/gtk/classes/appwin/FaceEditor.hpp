/**
 * FaceEditor.hpp - The Sickle face editor.
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

#ifndef SE_APPWIN_FACE_EDITOR_HPP
#define SE_APPWIN_FACE_EDITOR_HPP

#include "TextureSelector.hpp"

#include <editor/core/Editor.hpp>
#include <editor/world/Face.hpp>

#include <glibmm/binding.h>
#include <glm/glm.hpp>
#include <gtkmm/box.h>
#include <gtkmm/entry.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/spinbutton.h>

#include <array>


namespace Sickle::AppWin
{
    /**
     * Edit an N-dimensional vector with Gtk::SpinButtons.
     */
    template<size_t N>
    class VectorEdit : public Gtk::Box
    {
    public:
        VectorEdit(double increment=1.0)
        :   Glib::ObjectBase{typeid(VectorEdit)}
        ,   Gtk::Box{Gtk::Orientation::ORIENTATION_HORIZONTAL}
        ,   _prop_vector{*this, "vector", {}}
        {
            for (size_t i = 0; i < N; ++i)
            {
                auto &spin = _spins.at(i);
                spin.set_digits(2);
                spin.set_increments(increment, 1.0);
                spin.set_range(-1e16, 1e16);
                spin.property_value().signal_changed().connect(
                    [this, i](){
                        auto v = get_vector();
                        v[i] = _spins.at(i).get_value();
                        set_vector(v);
                    });
                add(spin);
            }
            property_vector().signal_changed().connect(
                sigc::mem_fun(*this, &VectorEdit::on_vector_changed));
        }

        /** The value of the vector. */
        auto property_vector() {return _prop_vector.get_proxy();}
        /** The value of the vector. */
        auto property_vector() const {return _prop_vector.get_proxy();}

        /**
         * Get the vector.
         *
         * @return The vector.
         */
        auto get_vector() const
        {
            return property_vector().get_value();
        }

        /**
         * Set the vector.
         *
         * @param v The new value of the vector.
         */
        void set_vector(glm::vec<N, float> const &v)
        {
            property_vector().set_value(v);
        }

    protected:
        void on_vector_changed()
        {
            auto const v = get_vector();
            for (size_t i = 0; i < N; ++i)
            {
                auto &spin = _spins.at(i);
                spin.set_value(v[i]);
            }
        }

    private:
        Glib::Property<glm::vec<N, float>> _prop_vector;

        std::array<Gtk::SpinButton, N> _spins{};
    };


    /**
     * Widget to edit editor face object data.
     */
    class FaceEditor : public Gtk::Grid
    {
    public:
        FaceEditor(Editor::EditorRef const &editor);

        /** The face to edit. */
        auto property_face() {return _prop_face.get_proxy();}
        /** The face to edit. */
        auto property_face() const {return _prop_face.get_proxy();}

        /**
         * Set the face to be edited.
         *
         * @param face New face to edit.
         */
        void set_face(Editor::FaceRef const &face);

        /**
         * Get the face being edited.
         *
         * @return The face being edited.
         */
        Editor::FaceRef get_face() const;

    protected:
        void on_face_changed();
        void on_texture_selector_button_clicked(
            Gtk::EntryIconPosition const &icon_pos,
            GdkEventButton const *button);
        void show_texture_select_window();

    private:
        Glib::Property<Editor::FaceRef> _prop_face;

        Glib::RefPtr<Glib::Binding> _bind_texture{};
        Glib::RefPtr<Glib::Binding> _bind_u{};
        Glib::RefPtr<Glib::Binding> _bind_v{};
        Glib::RefPtr<Glib::Binding> _bind_shift{};
        Glib::RefPtr<Glib::Binding> _bind_scale{};
        Glib::RefPtr<Glib::Binding> _bind_rotation{};

        Glib::RefPtr<TextureSelector::TextureSelector> _texture_selector{};

        Gtk::Label _texture_label{"Texture"};
        Gtk::Entry _texture_entry{};

        Gtk::Label _u_label{"U"};
        VectorEdit<3> _u_value{0.05};

        Gtk::Label _v_label{"V"};
        VectorEdit<3> _v_value{0.05};

        Gtk::Label _shift_label{"Shift"};
        VectorEdit<2> _shift_value{};

        Gtk::Label _scale_label{"Scale"};
        VectorEdit<2> _scale_value{0.05};

        Gtk::Label _rotation_label{"Rotation"};
        Gtk::SpinButton _rotation_value{};
    };
}

#endif
