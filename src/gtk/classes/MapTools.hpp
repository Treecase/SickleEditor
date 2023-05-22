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

#ifndef SE_MAPTOOLS_HPP
#define SE_MAPTOOLS_HPP

#include <editor/Editor.hpp>

#include <gtkmm/box.h>
#include <gtkmm/radiobutton.h>
#include <glibmm/property.h>

#include <array>
#include <memory>


namespace Sickle
{
    class MapTools : public Gtk::Box
    {
    public:
        enum Tool
        {
            SELECT,
            CREATE_BRUSH,
            _COUNT
        };

        auto property_tool() {return _prop_tool.get_proxy();}

        MapTools(Editor::Editor &editor);

    protected:
        void on_tool_button_toggled(Tool tool);
        void on_tool_changed();

    private:
        Glib::Property<Tool> _prop_tool;

        Editor::Editor &_editor;
        std::array<std::shared_ptr<Editor::MapTool>, Tool::_COUNT> const _tools{
            std::make_shared<Editor::MapToolSelect>(),
            std::make_shared<Editor::MapToolCreateBrush>(),
        };
        std::array<Gtk::RadioButton, Tool::_COUNT> _buttons{};
    };
}

#endif
