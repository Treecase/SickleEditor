/**
 * FaceEditor.cpp - The Sickle face editor.
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

#include "FaceEditor.hpp"

using namespace Sickle::AppWin;


FaceEditor::FaceEditor(Editor::EditorRef const &editor)
:   Glib::ObjectBase{typeid(FaceEditor)}
,   _prop_face{*this, "face", {}}
,   _texture_selector{TextureSelector::TextureSelector::create(editor)}
{
    set_column_spacing(16);
    set_sensitive(false);

    _texture_entry.property_secondary_icon_name() = "folder";
    _texture_entry.property_secondary_icon_activatable() = true;

    _rotation_value.set_digits(2);
    _rotation_value.set_increments(1.0, 15.0);
    _rotation_value.set_range(-360.0, 360.0);
    _rotation_value.set_wrap(true);
    // Have to set this to get the grid to expand.
    _rotation_value.set_hexpand(true);

    attach(_texture_label, 0, 0);
    attach(_texture_entry, 1, 0);

    attach(_u_label, 0, 1);
    attach(_u_value, 1, 1);

    attach(_v_label, 0, 2);
    attach(_v_value, 1, 2);

    attach(_shift_label, 0, 3);
    attach(_shift_value, 1, 3);

    attach(_scale_label, 0, 4);
    attach(_scale_value, 1, 4);

    attach(_rotation_label, 0, 5);
    attach(_rotation_value, 1, 5);

    property_face().signal_changed().connect(
        sigc::mem_fun(*this, &FaceEditor::on_face_changed));

    _texture_entry.signal_icon_press().connect(
        sigc::mem_fun(*this, &FaceEditor::on_texture_selector_button_clicked));
}


void FaceEditor::set_face(Editor::FaceRef const &face)
{
    property_face().set_value(face);
}


Sickle::Editor::FaceRef FaceEditor::get_face() const
{
    return property_face().get_value();
}



void FaceEditor::on_face_changed()
{
    _bind_texture.reset();
    _bind_u.reset();
    _bind_v.reset();
    _bind_shift.reset();
    _bind_scale.reset();
    _bind_rotation.reset();

    _texture_entry.set_text("");
    _u_value.set_vector(glm::vec3{});
    _v_value.set_vector(glm::vec3{});
    _shift_value.set_vector(glm::vec2{});
    _scale_value.set_vector(glm::vec2{});
    _rotation_value.set_value(0.0);

    auto const &face = get_face();
    set_sensitive((bool)face);
    if (!face)
        return;

    _bind_texture = Glib::Binding::bind_property(
        face->property_texture(),
        _texture_entry.property_text(),
        Glib::BindingFlags::BINDING_SYNC_CREATE
        | Glib::BindingFlags::BINDING_BIDIRECTIONAL);

    _bind_u = Glib::Binding::bind_property(
        face->property_u(),
        _u_value.property_vector(),
        Glib::BindingFlags::BINDING_SYNC_CREATE
        | Glib::BindingFlags::BINDING_BIDIRECTIONAL);

    _bind_v = Glib::Binding::bind_property(
        face->property_v(),
        _v_value.property_vector(),
        Glib::BindingFlags::BINDING_SYNC_CREATE
        | Glib::BindingFlags::BINDING_BIDIRECTIONAL);

    _bind_shift = Glib::Binding::bind_property(
        face->property_shift(),
        _shift_value.property_vector(),
        Glib::BindingFlags::BINDING_SYNC_CREATE
        | Glib::BindingFlags::BINDING_BIDIRECTIONAL);

    _bind_scale = Glib::Binding::bind_property(
        face->property_scale(),
        _scale_value.property_vector(),
        Glib::BindingFlags::BINDING_SYNC_CREATE
        | Glib::BindingFlags::BINDING_BIDIRECTIONAL);

    _bind_rotation = Glib::Binding::bind_property(
        face->property_rotation(),
        _rotation_value.property_value(),
        Glib::BindingFlags::BINDING_SYNC_CREATE
        | Glib::BindingFlags::BINDING_BIDIRECTIONAL);
}


void FaceEditor::on_texture_selector_button_clicked(
    Gtk::EntryIconPosition const &icon_pos,
    GdkEventButton const *button)
{
    if (icon_pos == Gtk::EntryIconPosition::ENTRY_ICON_SECONDARY
            && button->button == 1)
    {
        show_texture_select_window();
    }
}


void FaceEditor::show_texture_select_window()
{
    auto const result = _texture_selector->run();
    if (result == Gtk::RESPONSE_ACCEPT)
    {
        auto const tex = _texture_selector->get_selected_texture();
        get_face()->set_texture(tex);
    }
}
