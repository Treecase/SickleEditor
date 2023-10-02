/**
 * MapToolConfig.hpp - Config box for MapTools.
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

#ifndef SE_APPWIN_MAPTOOLCONFIG_HPP
#define SE_APPWIN_MAPTOOLCONFIG_HPP

#include <operations/Operation.hpp>

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/frame.h>
#include <gtkmm/scrolledwindow.h>

#include <vector>


namespace Sickle::AppWin
{
    struct Config
    {
        virtual Editor::Operation::Arg get_value()=0;
    };

    class MapToolConfig : public Gtk::Frame
    {
    public:
        MapToolConfig(Editor::EditorRef const &editor);

        auto &signal_confirmed() {return _sig_confirmed;}

        bool has_operation() const;
        void set_operation(Editor::Operation const &op);
        Editor::Operation get_operation() const;
        void clear_operation();
        Editor::Operation::ArgList get_arguments() const;

    private:
        Editor::EditorRef _editor{nullptr};
        std::unique_ptr<Editor::Operation> _operation{nullptr};

        sigc::signal<void()> _sig_confirmed{};

        Gtk::Box _box{};
        Gtk::Button _confirm{"Confirm"};
        std::vector<Glib::RefPtr<Gtk::Widget>> _arg_configs{};
        Gtk::ScrolledWindow _scrolled_window{};
    };
}

#endif
