/**
 * MapViewer.hpp - Map viewer module.
 * Copyright (C) 2022 Trevor Last
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

#ifndef _MAPVIEWER_HPP
#define _MAPVIEWER_HPP

#include "../Transform.hpp"
#include "../map/load_map.hpp"
#include "../map/map2gl.hpp"
#include "../wad/load_wad.hpp"
#include "../wad/lumps.hpp"
#include "../wad/TextureManager.hpp"
#include "Base3DViewer.hpp"


/** Displays .map files. */
class MapViewer : public Base3DViewer
{
private:
    // Loaded map.
    MAP::Map _map;
    /** GL representation of a Brush Plane. */
    struct GLPlane
    {
        GLUtil::Texture texture;
        GLsizei count;
        void *indices;
    };
    /** GL representation of a Brush. */
    struct GLBrush
    {
        std::vector<GLPlane> planes;
        GLUtil::VertexArray vao;
        GLUtil::Buffer vbo, ebo;

        GLBrush(
            std::vector<GLPlane> const &planes,
            std::vector<GLfloat> const &vbodata,
            std::vector<GLuint> const &ebodata);
    };
    std::vector<std::shared_ptr<GLBrush>> _brushes;
    // Path to loaded map.
    std::filesystem::path _selected;

    // Map transform
    Transform _transform;

    GLBrush *_brush2gl(MAP::Brush const &brush, MAP::TextureManager &textures);
    /** Called when _selected is updated. */
    void _loadSelectedMap();
    void _loadMap();

public:
    MapViewer(Config &cfg);

    /** Draw the module's UI. */
    void drawUI() override;

    /** Draw non-UI module visuals. */
    void drawGL(float deltaT) override;
};

#endif
