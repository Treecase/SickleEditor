/**
 * Face.hpp - OpenGL Editor::Face view.
 * Copyright (C) 2023-2024 Trevor Last
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

#ifndef SE_WORLD3D_FACE_HPP
#define SE_WORLD3D_FACE_HPP

#include "DeferredExec.hpp"
#include "Texture.hpp"

#include <glutils/glutils.hpp>
#include <sigc++/signal.h>
#include <wad/lumps.hpp>
#include <world/Face.hpp>

#include <array>
#include <memory>
#include <vector>


namespace World3D
{
    /**
     * Describes a vertex of a 3D face. Contains a 3D position and UV
     * information.
     */
    class Vertex
    {
    public:
        /** Number of float elements per vertex. */
        static constexpr size_t ELEMENTS = 5;

        /**
         * Convert the vertex to the proper format to be sent to the shader.
         * Format is `XYZST`. XYZ are the position, ST are the texture
         * coordinates.
         */
        std::array<GLfloat, ELEMENTS> as_vbo() const;

        Vertex(glm::vec3 pos, glm::vec2 uv);

    private:
        glm::vec3 _position;
        glm::vec2 _uv;
    };


    /**
     * A 3D view of an editor face object. Acts as a proxy for an editor face,
     * translating that data into a format usable for OpenGL rendering.
     * Basically it consists of some vertex data and a texture. These are
     * updated automatically to track the referenced editor face.
     *
     * See Sickle::Editor::Face for more context.
     */
    class Face : public sigc::trackable, public DeferredExec
    {
    public:
        using SlotPreDraw = sigc::slot<void(
            GLUtil::Program &,
            Sickle::Editor::Face const *)>;

        /// Called just before a face is rendered.
        static SlotPreDraw predraw;

        /**
         * Emitted when a face's texture cannot be loaded. The signal parameter
         * is the missing texture's name.
         */
        static auto &signal_missing_texture() {return _signal_missing_texture;}

        Face(
            Sickle::Editor::FaceRef const &face,
            GLint offset);

        /**
         * Emitted when the vertices change.
         */
        auto &signal_vertices_changed() {return _signal_vertices_changed;}

        /**
         * Length of span in parent brush's VBO. (ie. number of vertices in the
         * face).
         *
         * TODO: Probably shouldn't be here, tight coupling.
         */
        GLsizei count() const {return _vertices.size();}

        /**
         * Offset into parent brush's VBO marking the start of this face's
         * vertex data.
         *
         * TODO: Probably shouldn't be here, tight coupling.
         */
        auto offset() const {return _offset;}

        /**
         * Get the face's vertices.
         *
         * @return A `Container` containing the face's vertices.
         */
        auto const &vertices() const {return _vertices;}

        /**
         * Get the face's texture.
         *
         * @return The texture to be pasted onto the face.
         */
        auto const &texture() const {return _texture;}

        /**
         * Prepare face data for rendering.
         *
         * NOTE: The parent brush is the one actually making the render call,
         * this just prepares for the render by eg. binding textures.
         *
         * @warning Requires an active OpenGL context.
         */
        void render() const;

    protected:
        static GLUtil::Program &shader();

        void on_src_texture_changed();
        void on_src_uv_changed();
        void on_src_shift_changed();
        void on_src_scale_changed();
        void on_src_rotation_changed();

        void on_src_verts_changed();

    private:
        static sigc::signal<void(std::string)> _signal_missing_texture;

        sigc::signal<void()> _signal_vertices_changed{};

        Sickle::Editor::FaceRef _src{};
        GLint _offset;
        float const _starting_rotation;
        Texture _texture{};
        std::vector<Vertex> _vertices{};

        Face(Face const &)=delete;
        Face &operator=(Face const &)=delete;

        void _sync_vertices();
        /** @warning Requires an active OpenGL context. */
        void _sync_texture();
    };
}

#endif
