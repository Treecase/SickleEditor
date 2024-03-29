/**
 * MapToolConfig.cpp - Config box for MapTools.
 * Copyright (C) 2023-2024 Trevor Last
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

#include "MapToolConfig.hpp"
#include "TextureSelector.hpp"

#include <editor/core/Editor.hpp>

#include <glibmm/binding.h>
#include <gtkmm/grid.h>

#include <cstdlib>

using namespace Sickle::AppWin;
using namespace Sickle::Editor;
using namespace Sickle::TextureSelector;


// TODO: error hardening
struct NumberConfig : public Gtk::Entry, public Config
{
    NumberConfig(Operation::Arg const &arg=0.0)
    :   Gtk::Entry{}
    {
        auto const value = std::get<lua_Number>(arg);
        set_value(value);
    }

    void set_value(lua_Number value)
    {
        set_text(std::to_string(value));
    }

    Operation::Arg get_value() override
    {
        return Operation::Arg{std::stod(get_text())};
    }
};


struct StringConfig : public Gtk::Entry, public Config
{
    StringConfig(Operation::Arg const &arg="")
    :   Gtk::Entry{}
    {
        auto const value = std::get<std::string>(arg);
        set_value(value);
    }

    void set_value(std::string const &value)
    {
        set_text(value);
    }

    Operation::Arg get_value() override
    {
        return Operation::Arg{get_text()};
    }
};


struct TextureConfig : public Gtk::Box, public Config
{
    TextureConfig(
        EditorRef const &editor,
        Operation::Arg const &arg="")
    :   Gtk::Box{Gtk::Orientation::ORIENTATION_HORIZONTAL}
    ,   _texture_selector{TextureSelector::create(editor)}
    {
        _texture_selector_btn.signal_clicked().connect(
            sigc::mem_fun(
                *this,
                &TextureConfig::on_texture_selector_button_clicked));

        add(_texture);
        add(_texture_selector_btn);

        auto const value = std::get<std::string>(arg);
        set_value(value);
    }

    void set_value(std::string const &value)
    {
        _texture.set_text(value);
    }

    Operation::Arg get_value() override
    {
        return Operation::Arg{_texture.get_text()};
    }

protected:
    void on_texture_selector_button_clicked()
    {
        auto const result = _texture_selector->run();
        if (result == Gtk::RESPONSE_ACCEPT)
        {
            auto const tex = _texture_selector->get_selected_texture();
            set_value(tex);
        }
    }

private:
    Gtk::Entry _texture{};
    Gtk::Button _texture_selector_btn{"Select Texture"};
    Glib::RefPtr<TextureSelector> _texture_selector{nullptr};
};


class Vec3Config : public Gtk::Box, public Config
{
    std::array<NumberConfig, 3> _xyz{};
public:

    Vec3Config(Operation::Arg const &arg)
    :   Gtk::Box{Gtk::Orientation::ORIENTATION_VERTICAL}
    ,   Config{}
    {
        auto const &value = std::get<glm::vec3>(arg);
        auto &x = _xyz.at(0);
        auto &y = _xyz.at(1);
        auto &z = _xyz.at(2);
        x.set_value(value.x);
        y.set_value(value.y);
        z.set_value(value.z);
        add(x);
        add(y);
        add(z);
    }

    Operation::Arg get_value() override
    {
        return Operation::Arg{
            glm::vec3{
                std::get<lua_Number>(_xyz.at(0).get_value()),
                std::get<lua_Number>(_xyz.at(1).get_value()),
                std::get<lua_Number>(_xyz.at(2).get_value())}};
    }
};


class Mat4Config : public Gtk::Grid, public Config
{
    std::array<NumberConfig, 4*4> _elements{};

    NumberConfig &_config_for(size_t column, size_t row)
    {
        return _elements.at((4 * column) + row);
    }
public:

    Mat4Config(Operation::Arg const &arg)
    :   Gtk::Grid{}
    ,   Config{}
    {
        set_row_homogeneous(true);
        set_column_homogeneous(true);
        auto const &value = std::get<glm::mat4>(arg);
        for (size_t col = 0; col < 4; ++col)
        {
            for (size_t row = 0; row < 4; ++row)
            {
                auto &config = _config_for(col, row);
                config.set_value(value[col][row]);
                config.set_width_chars(3);
                attach(config, col, row);
            }
        }
    }

    Operation::Arg get_value() override
    {
        glm::mat4 out{};
        for (size_t col = 0; col < 4; ++col)
        {
            for (size_t row = 0; row < 4; ++row)
            {
                auto &config = _config_for(col, row);
                auto const value = std::get<lua_Number>(config.get_value());
                out[col][row] = value;
            }
        }
        return Operation::Arg{out};
    }
};


static Glib::RefPtr<Gtk::Widget> make_config_for(
    EditorRef const &editor,
    Operation const &op,
    size_t argument)
{
    auto const &def = op.args.at(argument);
    if (def.type == "f")
        return Glib::RefPtr{new NumberConfig{def.default_value}};
    else if (def.type == "string")
        return Glib::RefPtr{new StringConfig{def.default_value}};
    else if (def.type == "texture")
        return Glib::RefPtr{new TextureConfig{editor, def.default_value}};
    else if (def.type == "vec3")
        return Glib::RefPtr{new Vec3Config{def.default_value}};
    else if (def.type == "mat4")
        return Glib::RefPtr{new Mat4Config{def.default_value}};
    else
        throw std::logic_error{"bad arg type"};
}



MapToolConfig::MapToolConfig(Editor::EditorRef const &editor)
:   Glib::ObjectBase{typeid(MapToolConfig)}
,   Gtk::Frame{}
,   _editor{editor}
{
    set_label("Tool Options");

    _confirm.signal_clicked().connect(signal_confirmed().make_slot());

    _grid.set_row_spacing(8);
    _grid.set_column_spacing(8);
    _grid.set_margin_top(8);
    _grid.set_margin_left(8);
    _grid.set_margin_right(8);
    _grid.set_margin_bottom(8);

    _scrolled_window.add(_grid);

    add(_scrolled_window);
}


bool MapToolConfig::has_operation() const
{
    return (bool)_operation;
}


void MapToolConfig::set_operation(Editor::Operation const &op)
{
    _operation.reset(new Editor::Operation{op});

    _grid.foreach(sigc::mem_fun(_grid, &Gtk::Grid::remove));
    _arg_configs.clear();

    for (size_t i = 0; i < _operation->args.size(); ++i)
    {
        auto const &arg = _operation->args.at(i);
        auto label = Gtk::make_managed<Gtk::Label>(arg.name);
        auto widget = make_config_for(_editor, *_operation, i);

        _arg_configs.push_back(widget);
        _grid.attach(*label, 0, i);
        _grid.attach(*widget.get(), 1, i);
    }
    _grid.attach(_confirm, 0, _operation->args.size(), 2);
    show_all_children();
}


Operation MapToolConfig::get_operation() const
{
    if (_operation)
        return *_operation;
    throw std::logic_error{"operation unset"};
}


void MapToolConfig::clear_operation()
{
    _operation.release();
    _grid.foreach(sigc::mem_fun(_grid, &Gtk::Grid::remove));
    _arg_configs.clear();
}


Operation::ArgList MapToolConfig::get_arguments() const
{
    Editor::Operation::ArgList args{};
    for (auto &config : _arg_configs)
    {
        auto const c = dynamic_cast<Config *>(config.get());
        args.push_back(c->get_value());
    }
    return args;
}
