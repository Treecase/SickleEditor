/**
 * EditorObject.hpp - Interface for any object visible in the Outliner view.
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

#ifndef SE_EDITOR_INTERFACE_EDITOROBJECT_HPP
#define SE_EDITOR_INTERFACE_EDITOROBJECT_HPP

#include "Selectable.hpp"

#include <gdkmm/pixbuf.h>
#include <glibmm/refptr.h>
#include <glibmm/ustring.h>

#include <functional>


namespace Sickle::Editor
{
    class EditorObject : public Selectable
    {
    public:
        using SlotForEach = std::function<void(Glib::RefPtr<EditorObject>)>;

        EditorObject();
        virtual ~EditorObject()=default;

        /**
         * Emitted when a child object is added. Implementing classes must
         * trigger this when appropriate.
         */
        auto &signal_child_added() {return _sig_child_added;}

        /**
         * Emitted when a child object is removed. Implementing classes must
         * trigger this when appropriate.
         */
        auto &signal_child_removed() {return _sig_child_removed;}

        /** Emitted when the object is added to the world. */
        auto &signal_added() {return _sig_added;}

        /** Emitted when the object is removed from the world. */
        auto &signal_removed() {return _sig_removed;}

        /**
         * Get this object's unique name.
         *
         * @return The object's name.
         */
        virtual Glib::ustring name() const=0;

        /**
         * Get an icon representing this object's type. Note that this method
         * may return the same object every time.
         *
         * @return The icon inside an Gdk::Pixbuf.
         */
        virtual Glib::RefPtr<Gdk::Pixbuf> icon() const=0;

        /**
         * Get the direct children of this object.
         *
         * @return A list of this object's direct children.
         */
        virtual std::vector<Glib::RefPtr<EditorObject>> children() const=0;

        /**
         * Get all child objects recursively.
         *
         * @return A list of all the object's children.
         */
        std::vector<Glib::RefPtr<EditorObject>> children_recursive() const;

        /**
         * Call `func` on each of the object's children recursively.
         *
         * @param func A callable object to apply to each child.
         */
        void foreach(SlotForEach func);

    private:
        sigc::signal<void(Glib::RefPtr<EditorObject>)> _sig_child_added{};
        sigc::signal<void(Glib::RefPtr<EditorObject>)> _sig_child_removed{};
        sigc::signal<void()> _sig_added{};
        sigc::signal<void()> _sig_removed{};
    };
}

#endif
