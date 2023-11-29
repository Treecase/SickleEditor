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

#include <gdkmm/pixbuf.h>
#include <glibmm/refptr.h>
#include <glibmm/ustring.h>


namespace Sickle::Editor
{
    class EditorObject
    {
    public:
        virtual ~EditorObject()=default;

        virtual Glib::ustring name() const=0;
        virtual Glib::RefPtr<Gdk::Pixbuf> icon() const=0;
        virtual std::vector<EditorObject *> children() const=0;
    };
}

#endif
