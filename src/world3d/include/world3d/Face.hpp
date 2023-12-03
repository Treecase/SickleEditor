/**
 * Face.hpp - OpenGL Editor::Face view.
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
    class Vertex
    {
    public:
        static size_t const ELEMENTS = 5;
        std::array<GLfloat, ELEMENTS> as_vbo() const;

        Vertex(glm::vec3 pos, glm::vec2 uv);

    private:
        glm::vec3 _position;
        glm::vec2 _uv;
    };


    class Face : public sigc::trackable, public DeferredExec
    {
    public:
        static auto &signal_missing_texture() {return _signal_missing_texture;}

        Face(
            Sickle::Editor::FaceRef const &face,
            GLint offset);

        /** Emitted when the vertices change. */
        auto &signal_vertices_changed() {return _signal_vertices_changed;}

        /** Length of span in _PARENT's VBO. */
        GLsizei count() const {return _vertices.size();}

        auto const &vertices() const {return _vertices;}
        auto const &texture() const {return _texture;}
        auto offset() const {return _offset;}

    private:
        static sigc::signal<void(std::string)> _signal_missing_texture;

        sigc::signal<void()> _signal_vertices_changed{};

        Sickle::Editor::FaceRef const _src{nullptr};
        Texture _texture{};
        std::vector<Vertex> _vertices{};
        GLint _offset; // offset into _PARENT's VBO.

        Face(Face const &)=delete;
        Face &operator=(Face const &)=delete;

        void _sync_vertices();
        void _sync_texture();

        void _on_src_verts_changed();
        void _on_src_texture_changed();
    };
}

#endif
