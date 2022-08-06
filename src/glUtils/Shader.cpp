/**
 * Shader.cpp - OpenGL Shader object.
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

#include "glUtil.hpp"

#include <GL/glew.h>

#include <fstream>
#include <stdexcept>
#include <string>
#include <sstream>


/** Deleter for Shader objects. (For use with shared_ptr and co.) */
void _shader_delete(GLuint *shader)
{
    glDeleteShader(*shader);
    delete shader;
}

GLUtil::Shader GLUtil::shader_from_file(std::string path, GLenum type)
{
    std::ifstream shaderfile{path.c_str()};
    if (!shaderfile.is_open())
    {
        throw std::runtime_error{"Failed to open '" + path + "'"};
    }
    std::stringstream srcstream{};
    srcstream << shaderfile.rdbuf();
    shaderfile.close();
    return GLUtil::Shader{type, srcstream.str(), path};
}


GLUtil::Shader::Shader(GLenum type, std::string source, std::string label)
:   _id{new GLuint{glCreateShader(type)}, _shader_delete}
{
    GLchar const *const src = source.c_str();
    glShaderSource(*_id, 1, &src, nullptr);
    glCompileShader(*_id);
    GLint success = 0;
    glGetShaderiv(*_id, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLint infolog_size = 0;
        glGetShaderiv(*_id, GL_INFO_LOG_LENGTH, &infolog_size);
        std::string infolog{};
        infolog.reserve(infolog_size);
        glGetShaderInfoLog(*_id, infolog_size, nullptr, &infolog[0]);
        throw std::runtime_error{"Shader compile failed:\n" + infolog};
    }
    if (!label.empty())
    {
        glObjectLabel(GL_SHADER, *_id, (GLsizei)label.size(), label.c_str());
    }
}

GLuint GLUtil::Shader::id() const
{
    return *_id;
}
