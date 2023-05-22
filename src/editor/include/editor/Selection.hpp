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

#include "EditorWorld.hpp"

#include <sigc++/signal.h>
#include <sigc++/connection.h>

#include <memory>
#include <unordered_set>


namespace Sickle
{
namespace Editor
{
    class Selection
    {
    public:
        using Item = std::shared_ptr<Brush>;

        void clear();
        void add(Item item);
        void remove(Item item);
        bool contains(Item item) const;

        auto begin() const {return _selected.begin();}
        auto end() const {return _selected.end();}

        auto &signal_updated() {return _signal_updated;}

        Selection()=default;

    private:
        std::unordered_set<Item> _selected{};

        std::unordered_map<Item, std::vector<sigc::connection>>
        _selected_signals{};
        sigc::signal<void()> _signal_updated{};

        Selection(Selection const &)=delete;
        Selection &operator=(Selection const &)=delete;
    };
}
}

#endif
