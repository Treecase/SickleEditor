/**
 * Program.cpp - OpenGL program object.
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
#include <glm/gtc/type_ptr.hpp>

#include <stdexcept>
#include <string>
#include <vector>


/** Deleter for Program objects. (For use with shared_ptr and co.) */
void _program_delete(GLuint *program)
{
    glDeleteProgram(*program);
    delete program;
}


GLUtil::Program::Program(std::vector<Shader> shaders, std::string label)
:   _id{new GLuint{glCreateProgram()}, _program_delete}
{
    for (auto shader : shaders)
    {
        glAttachShader(*_id, shader.id());
    }
    glLinkProgram(*_id);
    for (auto shader : shaders)
    {
        glDetachShader(*_id, shader.id());
    }
    GLint success = 0;
    glGetProgramiv(*_id, GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLint infolog_size = 0;
        glGetProgramiv(*_id, GL_INFO_LOG_LENGTH, &infolog_size);
        std::string infolog{};
        infolog.reserve(infolog_size);
        glGetProgramInfoLog(*_id, infolog_size, nullptr, &infolog[0]);
        throw std::runtime_error{"Program link failed:\n" + infolog + "\n"};
    }
    if (!label.empty())
    {
        glObjectLabel(GL_PROGRAM, *_id, (GLsizei)label.size(), label.c_str());
    }
}

void GLUtil::Program::use() const
{
    glUseProgram(*_id);
}

GLuint GLUtil::Program::id() const
{
    return *_id;
}

void GLUtil::Program::setUniform(std::string uniform, bool value) const
{
    glUniform1i(_getUniformLocation(uniform), value);
}

void GLUtil::Program::setUniform(std::string uniform, GLfloat value) const
{
    glUniform1f(_getUniformLocation(uniform), value);
}

void GLUtil::Program::setUniform(std::string uniform, GLint value) const
{
    glUniform1i(_getUniformLocation(uniform), value);
}

void GLUtil::Program::setUniform(std::string uniform, GLuint value) const
{
    glUniform1ui(_getUniformLocation(uniform), value);
}

void GLUtil::Program::setUniform(std::string uniform, glm::vec2 value) const
{
    glUniform2fv(_getUniformLocation(uniform), 1, glm::value_ptr(value));
}

void GLUtil::Program::setUniform(std::string uniform, glm::vec3 value) const
{
    glUniform3fv(_getUniformLocation(uniform), 1, glm::value_ptr(value));
}

void GLUtil::Program::setUniform(std::string uniform, glm::vec4 value) const
{
    glUniform4fv(_getUniformLocation(uniform), 1, glm::value_ptr(value));
}


GLint GLUtil::Program::_getUniformLocation(std::string uniform) const
{
    auto location = glGetUniformLocation(*_id, uniform.c_str());
    if (location == -1)
    {
        throw std::runtime_error{
            "glGetUniformLocation - uniform '" + uniform + "' does not exist"};
    }
    return location;
}
