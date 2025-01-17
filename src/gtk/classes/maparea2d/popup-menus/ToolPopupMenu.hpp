/**
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

#ifndef SE_MAPAREA2D_TOOLPOPUPMENU_HPP
#define SE_MAPAREA2D_TOOLPOPUPMENU_HPP

#include <editor/core/Editor.hpp>

#include <giomm/simpleactiongroup.h>
#include <glibmm/refptr.h>
#include <gtkmm/menu.h>

namespace Sickle
{
    /**
     * Dynamically created popup menu for Editor MapTools.
     */
    class ToolPopupMenu : public Gtk::Menu
    {
        Glib::RefPtr<Gio::SimpleActionGroup> _actions{};
        Editor::EditorRef _editor{nullptr};
        Editor::MapTool _tool; // FIXME: temp?

    public:
        ToolPopupMenu(Editor::MapTool const &tool);

        void set_editor(decltype(_editor) editor);
        bool should_popup() const;

        void action_triggered(std::string const &id);
    };
} // namespace Sickle

#endif
