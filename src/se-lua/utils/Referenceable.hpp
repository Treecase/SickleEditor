/**
 * Referencable.hpp - Build reference-style Lua objects.
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

#ifndef SE_LUA_REFERENCABLE_HPP
#define SE_LUA_REFERENCABLE_HPP

#include <se-lua/se-lua.hpp>

#include <sigc++/signal.h>

#include <memory>


namespace Lua
{
    class Referenceable
    {
        std::shared_ptr<char> const _unique_id{new char{}};
        sigc::signal<void()> _signal_destroy{};

    public:
        auto get_id() const {return _unique_id.get();}
        auto signal_destroy() {return _signal_destroy;}

        virtual ~Referenceable() {signal_destroy().emit();}
    };
}

#endif
