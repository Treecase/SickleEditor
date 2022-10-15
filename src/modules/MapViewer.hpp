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

#include "../common.hpp"
#include "../map/load_map.hpp"
#include "../wad/load_wad.hpp"
#include "../wad/lumps.hpp"
#include "Module.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glutils/glutils.hpp>
#include <SDL.h>

#include <filesystem>


/** Wraps GLUtil's Texture, to keep some additional information we need. */
struct MapTexture
{
    std::shared_ptr<GLUtil::Texture> texture;
    int w, h;

    MapTexture();
    MapTexture(WAD::TexLump const &texlump);
};

/**
 * Lazy-loading texture manager. Grabbing all the textures from the WAD is
 * really slow, and we usually only need a few textures, so instead we only
 * load from WAD when we need to.
 */
struct TextureManager
{
    std::unordered_map<std::string, WAD::Lump> lumps{};
    std::unordered_map<std::string, MapTexture> textures;

    TextureManager(WAD::WAD const &wad);
    /** Same as `textures.at(key)`, unless this would fail, in which case
     *  attempt to load the lump identified by `key` from the WAD. */
    MapTexture &at(std::string const &key);
};

/** Displays .map files. */
class MapViewer : public Module
{
private:
    // Shader.
    GLUtil::Program _shader;

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

    // First-person camera.
    struct Camera {
        glm::vec3 pos;      // Position
        glm::vec2 angle;    // x/y angle
        GLfloat fov;        // FOV
        GLfloat speed;      // Movement speed
    } _camera;

    // Wireframe display toggle.
    bool _wireframe;

    // Map translation.
    GLfloat _translation[3];
    // Map rotation.
    GLfloat _rotation[3];
    // Map scaling.
    GLfloat _scale;

    GLBrush *_brush2gl(MAP::Brush const &brush, TextureManager &textures);
    /** Called when _selected is updated. */
    void _loadSelectedMap();
    void _loadMap();

public:
    MapViewer(Config &cfg);

    /** Handle user input. */
    void input(SDL_Event const *event) override;

    /** Draw the module's UI. */
    void drawUI() override;

    /** Draw non-UI module visuals. */
    void drawGL(float deltaT) override;
};

#endif
