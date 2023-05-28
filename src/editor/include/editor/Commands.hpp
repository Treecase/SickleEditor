/**
 * Commands.hpp - Editor Command classes.
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

#ifndef SE_EDITOR_COMMANDS_HPP
#define SE_EDITOR_COMMANDS_HPP

#include <glm/glm.hpp>

#include <vector>


namespace Sickle
{
namespace Editor
{
    class Editor;

    class Command
    {
    public:
        virtual void execute(Editor &)=0;
        virtual ~Command()=default;
    };

namespace commands
{
    class AddBrush : public Command
    {
        std::vector<glm::vec3> const _points;
    public:
        AddBrush(std::vector<glm::vec3> const &points);
        virtual void execute(Editor &editor) override;
        virtual ~AddBrush()=default;
    };
}
}
}

#endif
