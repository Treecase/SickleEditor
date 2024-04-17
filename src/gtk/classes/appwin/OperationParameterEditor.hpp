/**
 * OperationParameterEditor.hpp - Config box for MapTools.
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

#ifndef SE_APPWIN_MAPTOOLCONFIG_HPP
#define SE_APPWIN_MAPTOOLCONFIG_HPP

#include <editor/operations/Operation.hpp>

#include <gtkmm/button.h>
#include <gtkmm/entry.h>
#include <gtkmm/frame.h>
#include <gtkmm/grid.h>
#include <gtkmm/scrolledwindow.h>

#include <memory>
#include <vector>


namespace Sickle::AppWin
{
    /**
     * Edit operation parameters before execution.
     */
    class OperationParameterEditor : public Gtk::Frame
    {
    public:
        OperationParameterEditor();

        /** Emitted when the user is finished editing parameters. */
        auto &signal_confirmed() {return _sig_confirmed;}

        /**
         * Check if an operation's parameters are currently being edited.
         *
         * @return True if an operation is currently being edited, else false.
         */
        bool has_operation() const;

        /**
         * Edit an operation's parameters.
         *
         * @param op The operation to edit.
         */
        void set_operation(Editor::Operation const &op);

        /**
         * Get the edited operation.
         *
         * @return The edited operation.
         */
        Editor::Operation get_operation() const;

        /**
         * Clear the operation.
         */
        void clear_operation();

        /**
         * Get the argument values.
         *
         * @return An Arglist containing the argument values.
         */
        Editor::Operation::ArgList get_arguments() const;

    private:
        sigc::signal<void()> _sig_confirmed{};

        Gtk::Button _confirm{"Confirm"};
        Gtk::Grid _grid{};
        Gtk::ScrolledWindow _scrolled_window{};

        std::vector<Gtk::Widget *> _arg_configs{};
        std::unique_ptr<Editor::Operation> _operation{nullptr};
    };
}

#endif
