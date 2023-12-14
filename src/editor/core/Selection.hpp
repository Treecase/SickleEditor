/**
 * Selection.hpp - Editor Selection class.
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

#ifndef SE_EDITOR_SELECTION_HPP
#define SE_EDITOR_SELECTION_HPP

#include <interfaces/Selectable.hpp>
#include <se-lua/utils/Referenceable.hpp>

#include <sigc++/signal.h>

#include <unordered_set>


namespace Sickle::Editor
{
    class Selection : public Lua::Referenceable
    {
    public:
        using Item = Glib::RefPtr<Selectable>;

        Selection()=default;

        /** Emitted when an object is selected or deselected. */
        auto &signal_updated() {return _signal_updated;}

        /**
         * Deselect all selected objects. Sets the object's "selected" property
         * to false if it isn't already.
         */
        void clear();

        /**
         * Add an object to the selection. Sets the object's "selected"
         * property to true if it isn't already.
         *
         * @param item The item to select.
         */
        void add(Item item);

        /**
         * Remove an object from the selection. Sets the object's "selected"
         * property to false if it isn't already.
         *
         * @param item The item to deselect.
         */
        void remove(Item item);

        /**
         * Check if an object is in the selection.
         *
         * @param item Item to check status of.
         * @return True if object is selected, else false.
        */
        bool contains(Item item) const;

        /**
         * Check if the selection is empty.
         *
         * @return True if the selection is empty, else false.
         */
        bool empty() const;

        auto begin() const {return _selected.begin();}
        auto end() const {return _selected.end();}

    private:
        struct ItemHasher
        {
            std::size_t operator()(Selection::Item const &item) const
            {
                return std::hash<void *>{}(item.get());
            }
        };

        std::unordered_set<Item, ItemHasher> _selected{};

        sigc::signal<void()> _signal_updated{};

        Selection(Selection const &)=delete;
        Selection &operator=(Selection const &)=delete;
    };
}

#endif
