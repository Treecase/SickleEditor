/**
 * ClassProperties.hpp - Defines the class property types.
 * Copyright (C) 2024 Trevor Last
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

#ifndef SE_EDITOR_CORE_GAMEDEFINITION_CLASSPROPERTIES_HPP
#define SE_EDITOR_CORE_GAMEDEFINITION_CLASSPROPERTIES_HPP

#include <files/fgd/fgd.hpp>

#include <glm/glm.hpp>

#include <initializer_list>
#include <memory>
#include <string>
#include <vector>


namespace Sickle::Editor
{
    struct ClassProperty
    {
        virtual ~ClassProperty()=default;
    };


    // base() is not included since class hierarchies are collapsed when
    // creating ClassInfo objects.


    struct ClassPropertyColor : public ClassProperty
    {
        ClassPropertyColor(glm::vec3 const &color)
        :   _color{color}
        {
        }
        virtual ~ClassPropertyColor()=default;

        glm::vec3 get_color() const {return _color;}

    private:
        glm::vec3 _color;
    };


    // TODO: decal


    struct ClassPropertyIconsprite : public ClassProperty
    {
        ClassPropertyIconsprite(std::string const &path)
        :   _path{path}
        {
        }
        virtual ~ClassPropertyIconsprite()=default;

        std::string get_path() const {return _path;}

    private:
        std::string _path;
    };


    struct ClassPropertySize : public ClassProperty
    {
        ClassPropertySize(glm::vec3 const &p1, glm::vec3 const &p2)
        :   _points{p1, p2}
        {
        }
        virtual ~ClassPropertySize()=default;

        std::pair<glm::vec3, glm::vec3> get_points() const {return _points;}

    private:
        std::pair<glm::vec3, glm::vec3> _points;
    };


    // TODO: sprite()


    // TODO: studio()



    struct ClassPropertyFactory
    {
        static std::shared_ptr<ClassProperty> construct(
            std::shared_ptr<FGD::Attribute> const &prop);
    };
}

#endif
