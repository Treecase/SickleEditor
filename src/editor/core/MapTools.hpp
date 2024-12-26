/**
 * MapTools.hpp - Editor MapTools class.
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

#ifndef SE_EDITOR_MAPTOOLS_HPP
#define SE_EDITOR_MAPTOOLS_HPP

#include <glibmm/refptr.h>

#include <functional> // temp
#include <string>
#include <vector>

namespace Sickle::Editor
{
    class Editor;

    class MapTool
    {
    public:
        // temp
        using FunctionType = std::function<bool(Glib::RefPtr<Editor> const &)>;

        struct OpDef
        {
            std::string const label;
            std::string const operation_id;
        };

        FunctionType x;

        MapTool(
            std::string const &name,
            std::vector<OpDef> const &opdefs,
            FunctionType const &fn);
        virtual ~MapTool() = default;

        std::string name() const;
        std::vector<OpDef> operations() const;

    private:
        std::string _name;
        std::vector<OpDef> _opdefs;
    };
} // namespace Sickle::Editor

#endif
