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

#include <string>


namespace Sickle
{
namespace Editor
{
    class MapTool
    {
    public:
        virtual void operator()()=0;
        virtual std::string name() const=0;

        virtual ~MapTool()=default;
    };

    class MapToolSelect : public MapTool
    {
    public:
        virtual void operator()() override;
        virtual std::string name() const override;

        virtual ~MapToolSelect()=default;
    };

    class MapToolCreateBrush : public MapTool
    {
    public:
        virtual void operator()() override;
        virtual std::string name() const override;

        virtual ~MapToolCreateBrush()=default;
    };
}
}

#endif
