/**
 * Selectable.hpp - Interface for selectable object.
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

#ifndef SE_EDITOR_INTERFACE_SELECTABLE_HPP
#define SE_EDITOR_INTERFACE_SELECTABLE_HPP

#include <glibmm/object.h>
#include <glibmm/property.h>

namespace Sickle::Editor
{
    class Selectable : public Glib::Object
    {
    public:
        Selectable();
        virtual ~Selectable() = default;

        auto property_selected() { return _prop_selected.get_proxy(); }

        auto property_selected() const { return _prop_selected.get_proxy(); }

        auto is_selected() const { return property_selected().get_value(); }

        void select(bool value = true) { property_selected().set_value(value); }

    private:
        Glib::Property<bool> _prop_selected;
    };
} // namespace Sickle::Editor

#endif
