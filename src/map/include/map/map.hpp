/**
 * load_map.hpp - .map format data.
 * Copyright (C) 2022-2023 Trevor Last
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

#ifndef SE_MAP_HPP
#define SE_MAP_HPP

#include <glm/glm.hpp>

#include <array>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>


namespace MAP
{
    struct LoadError : std::runtime_error
    {
        LoadError(std::string const &what): std::runtime_error{what} {}
    };

    struct TokenizeError : LoadError
    {
        TokenizeError(std::string const &what): LoadError{what} {}
    };

    struct ParseError : LoadError
    {
        ParseError(std::string const &what): LoadError{what} {}
    };

    struct FormatError : LoadError
    {
        FormatError(std::string const &what): LoadError{what} {}
    };


    typedef glm::vec3 Vertex;
    typedef glm::vec3 Vector3;
    typedef glm::vec2 Vector2;

    /** Half-spaces used to define a brush. */
    struct Plane
    {
        // 3 points which define the plane
        Vertex a, b, c;
        // Vertices are sorted counterclockwise
        std::vector<Vertex> vertices;
        // Name of the texture to paste on the plane
        std::string miptex;
        // Texture application vars
        Vector3 s, t;
        Vector2 offsets;
        float rotation;
        Vector2 scale;
    };

    /** A convex polyhedron. */
    struct Brush
    {
        // Half-spaces comprising the brush. Minimum size of 4.
        std::vector<Plane> planes;
    };


    template<class BrushT>
    struct TEntity
    {
        std::unordered_map<std::string, std::string> properties{};
        std::vector<BrushT> brushes{};

        template<class OtherBrushT>
        TEntity(TEntity<OtherBrushT> const &other)
        :   properties{other.properties}
        {
            for (auto const &otherbrush : other.brushes)
                brushes.emplace_back(otherbrush);
        }
        TEntity()=default;
        virtual ~TEntity()=default;
    };

    template<class BrushT>
    struct TMap
    {
        std::vector<TEntity<BrushT>> entities{};

        template<class OtherBrushT>
        TMap(TMap<OtherBrushT> const &other)
        {
            for (auto const &otherentity : other.entities)
                entities.emplace_back(otherentity);
        }
        TMap()=default;
        virtual ~TMap()=default;
    };

    using Entity = TEntity<Brush>;
    using Map = TMap<Brush>;


    /** Parse a .map file. */
    Map load(std::string const &path);
}

#endif
