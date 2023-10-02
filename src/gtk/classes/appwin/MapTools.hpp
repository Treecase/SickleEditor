/**
 * AppWin.hpp - Sickle Map Tools toolbar.
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

#ifndef SE_APPWIN_MAPTOOLS_HPP
#define SE_APPWIN_MAPTOOLS_HPP

#include <core/Editor.hpp>

#include <glibmm/property.h>
#include <gtkmm/box.h>
#include <gtkmm/frame.h>
#include <gtkmm/menu.h>
#include <gtkmm/radiobutton.h>
#include <gtkmm/scrolledwindow.h>

#include <memory>
#include <unordered_map>


namespace Sickle::AppWin
{
    /**
     * Side toolbar.
     *
     * Creates a RadioButton for each MapTool installed in the editor. Clicking
     * the buttons will set that tool as active. If the editor's active tool
     * changes, the button states will also update to match.
     */
    class MapTools : public Gtk::Frame
    {
    public:
        MapTools(Editor::EditorRef editor);

    protected:
        void on_tool_button_toggled(std::string const &tool);
        void on_tool_changed();
        void on_maptools_changed();

    private:
        Editor::EditorRef _editor;
        std::unordered_map<std::string, Gtk::RadioButton> _buttons{};

        Gtk::ScrolledWindow _scrolled{};
        Gtk::Box _box{};
        Gtk::RadioButtonGroup _button_group{};

        void _add_tool(Editor::MapTool const &tool);
    };
}

#endif
