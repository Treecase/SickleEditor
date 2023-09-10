/**
 * MapToolConfig.cpp - Config box for MapTools.
 * Copyright (C) 2023 Trevor Last
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

#include <cstdlib>

using namespace Sickle::AppWin;


// TODO: error hardening
struct NumberConfig : public Gtk::Entry, public Config
{
    NumberConfig()
    :   Gtk::Entry{}
    {
        set_text("0.0");
    }

    Sickle::Editor::Operation::Arg get_value() override
    {
        return Sickle::Editor::Operation::Arg{std::stod(get_text())};
    }
};


class Vec3Config : public Gtk::Box, public Config
{
    std::array<NumberConfig, 3> _xyz{};
public:

    Vec3Config()
    :   Gtk::Box{Gtk::Orientation::ORIENTATION_VERTICAL}
    ,   Config{}
    {
        add(_xyz.at(0));
        add(_xyz.at(1));
        add(_xyz.at(2));
    }

    Sickle::Editor::Operation::Arg get_value() override
    {
        return Sickle::Editor::Operation::Arg{
            glm::vec3{
                std::get<lua_Number>(_xyz.at(0).get_value()),
                std::get<lua_Number>(_xyz.at(1).get_value()),
                std::get<lua_Number>(_xyz.at(2).get_value())}};
    }
};


static Glib::RefPtr<Gtk::Widget> make_config_for(std::string const &type)
{
    if (type == "f")
        return Glib::RefPtr{new NumberConfig{}};
    else if (type == "vec3")
        return Glib::RefPtr{new Vec3Config{}};
    else
        throw std::logic_error{"bad arg type"};
}



MapToolConfig::MapToolConfig()
:   Glib::ObjectBase{typeid(MapToolConfig)}
,   Gtk::Frame{}
{
    set_label("Tool Options");

    _confirm.signal_clicked().connect(signal_confirmed().make_slot());
    _confirm.signal_show().connect(
        [this](){_confirm.set_visible(has_operation());});
    _confirm.signal_hide().connect(
        [this](){_confirm.set_visible(has_operation());});

    _box.set_orientation(Gtk::Orientation::ORIENTATION_VERTICAL);
    _box.set_spacing(8);
    _box.set_margin_top(8);
    _box.pack_end(_confirm, Gtk::PackOptions::PACK_SHRINK);

    _scrolled_window.property_hscrollbar_policy() =\
        Gtk::PolicyType::POLICY_NEVER;
    _scrolled_window.add(_box);

    add(_scrolled_window);
}


bool MapToolConfig::has_operation() const
{
    return (bool)_operation;
}


void MapToolConfig::set_operation(Editor::Operation const &op)
{
    _operation.reset(new Editor::Operation{op});

    for (auto &config : _arg_configs)
        _box.remove(*config.get());
    _arg_configs.clear();

    for (auto const &type : _operation->arg_types)
    {
        auto widget = make_config_for(type);
        _arg_configs.push_back(widget);
        _box.add(*widget.get());
    }
    _box.show_all_children();
    _confirm.show();
}


Sickle::Editor::Operation MapToolConfig::get_operation() const
{
    if (_operation)
        return *_operation;
    throw std::logic_error{"operation unset"};
}


void MapToolConfig::clear_operation()
{
    _operation.release();
    for (auto &config : _arg_configs)
        _box.remove(*config.get());
    _arg_configs.clear();
    _confirm.hide();
}


Sickle::Editor::Operation::ArgList MapToolConfig::get_arguments() const
{
    Editor::Operation::ArgList args{};
    for (auto &config : _arg_configs)
    {
        auto c = dynamic_cast<Config *>(config.get());
        args.push_back(c->get_value());
    }
    return args;
}
