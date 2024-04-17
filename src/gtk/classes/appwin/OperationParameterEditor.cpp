/**
 * OperationParameterEditor.cpp - Config box for MapTools.
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

#include "OperationParameterEditor.hpp"

#include <editor/core/gamedefinition/GameDefinition.hpp>
#include <editor/operations/Operation.hpp>
#include <gtk/classes/textureselector/TextureSelector.hpp>

#include <gtkmm/comboboxtext.h>
#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>

#include <array>
#include <set>

using namespace Sickle::AppWin;
using namespace Sickle::Editor;
using namespace Sickle::TextureSelector;


struct Config
{
    virtual Operation::Arg get_value() const=0;
};


class ClassnameConfig : public Gtk::ComboBoxText, public Config
{
public:
    ClassnameConfig(Operation::Arg const &arg="")
    {
        auto &gamedef = GameDefinition::instance();
        auto const classnames = gamedef.get_all_classnames();
        std::set<std::string> classnames_sorted{
            classnames.cbegin(),
            classnames.cend()};
        for (auto const &classname : classnames_sorted)
            append(classname);
        auto const value = std::get<std::string>(arg);
        set_active_text(value);
    }
    virtual ~ClassnameConfig()=default;

    virtual Operation::Arg get_value() const override
    {
        return Operation::Arg{get_active_text()};
    }
};


// TODO: error hardening
class NumberConfig : public Gtk::Entry, public Config
{
public:
    NumberConfig(Operation::Arg const &arg=0.0)
    :   Gtk::Entry{}
    {
        auto const value = std::get<lua_Number>(arg);
        set_value(value);
    }
    virtual ~NumberConfig()=default;

    void set_value(lua_Number value)
    {
        set_text(std::to_string(value));
    }

    virtual Operation::Arg get_value() const override
    {
        return Operation::Arg{std::stod(get_text())};
    }
};


class StringConfig : public Gtk::Entry, public Config
{
public:
    StringConfig(Operation::Arg const &arg="")
    :   Gtk::Entry{}
    {
        auto const value = std::get<std::string>(arg);
        set_value(value);
    }
    virtual ~StringConfig()=default;

    void set_value(std::string const &value)
    {
        set_text(value);
    }

    virtual Operation::Arg get_value() const override
    {
        return Operation::Arg{get_text()};
    }
};


class TextureConfig : public Gtk::Box, public Config
{
public:
    TextureConfig(Operation::Arg const &arg="")
    :   Gtk::Box{Gtk::Orientation::ORIENTATION_HORIZONTAL}
    ,   _texture_selector{TextureSelector::create()}
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
    virtual ~TextureConfig()=default;

    void set_value(std::string const &value)
    {
        _texture.set_text(value);
    }

    virtual Operation::Arg get_value() const override
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
    virtual ~Vec3Config()=default;

    virtual Operation::Arg get_value() const override
    {
        return Operation::Arg{
            glm::vec3{
                std::get<lua_Number>(_xyz.at(0).get_value()),
                std::get<lua_Number>(_xyz.at(1).get_value()),
                std::get<lua_Number>(_xyz.at(2).get_value())}};
    }

private:
    std::array<NumberConfig, 3> _xyz{};
};


class Mat4Config : public Gtk::Grid, public Config
{
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
    virtual ~Mat4Config()=default;

    virtual Operation::Arg get_value() const override
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

private:
    std::array<NumberConfig, 4*4> _elements{};

    NumberConfig &_config_for(size_t column, size_t row)
    {
        return _elements.at((4 * column) + row);
    }

    NumberConfig const &_config_for(size_t column, size_t row) const
    {
        return _elements.at((4 * column) + row);
    }
};


/**
 * Construct an appropriate widget to edit an argument.
 *
 * @param def Operation argument definition.
 * @return An argument editing widget or nullptr if the argument type is
 *         unsupported.
 */
static Gtk::Widget *make_config_for(Operation::ArgDef const &def)
{
    if (def.type == "classname")
        return Gtk::make_managed<ClassnameConfig>(def.default_value);
    else if (def.type == "f")
        return Gtk::make_managed<NumberConfig>(def.default_value);
    else if (def.type == "string")
        return Gtk::make_managed<StringConfig>(def.default_value);
    else if (def.type == "texture")
        return Gtk::make_managed<TextureConfig>(def.default_value);
    else if (def.type == "vec3")
        return Gtk::make_managed<Vec3Config>(def.default_value);
    else if (def.type == "mat4")
        return Gtk::make_managed<Mat4Config>(def.default_value);
    else
        return nullptr;
}



OperationParameterEditor::OperationParameterEditor()
:   Glib::ObjectBase{typeid(OperationParameterEditor)}
{
    set_label("Tool Options");

    _grid.set_row_spacing(8);
    _grid.set_column_spacing(8);
    _grid.set_margin_top(8);
    _grid.set_margin_left(8);
    _grid.set_margin_right(8);
    _grid.set_margin_bottom(8);

    _scrolled_window.add(_grid);
    add(_scrolled_window);

    _confirm.signal_clicked().connect(signal_confirmed().make_slot());
}


bool OperationParameterEditor::has_operation() const
{
    return _operation != nullptr;
}


void OperationParameterEditor::set_operation(Editor::Operation const &op)
{
    clear_operation();
    _operation = std::make_unique<Editor::Operation>(op);

    _grid.attach(_confirm, 0, 0, 2);
    for (auto const &arg : _operation->args)
    {
        auto label = Gtk::make_managed<Gtk::Label>(arg.name);
        auto widget = make_config_for(arg);

        _grid.insert_row(0);
        _grid.attach(*label, 0, 0);
        if (widget)
        {
            _grid.attach(*widget, 1, 0);
            _arg_configs.push_back(widget);
        }
    }
    show_all_children();
}


Operation OperationParameterEditor::get_operation() const
{
    if (_operation)
        return *_operation;
    throw std::logic_error{"operation unset"};
}


void OperationParameterEditor::clear_operation()
{
    _operation.release();
    _grid.foreach(sigc::mem_fun(_grid, &Gtk::Grid::remove));
    _arg_configs.clear();
}


Operation::ArgList OperationParameterEditor::get_arguments() const
{
    Editor::Operation::ArgList args{};
    for (auto const &config : _arg_configs)
    {
        auto const c = dynamic_cast<Config const *>(config);
        args.push_back(c->get_value());
    }
    return args;
}
