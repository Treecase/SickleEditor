/**
 * glUtil.hpp - OpenGL utilities.
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

#ifndef _GLUTIL_HPP
#define _GLUTIL_HPP

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>


namespace GLUtil
{
/**
 * OpenGL shader object.
 */
class Shader
{
private:
    std::shared_ptr<GLuint> const _id;
public:
    Shader(GLenum type, std::string source, std::string label="");

    /** Get the shader's id. */
    GLuint id() const;
};

/**
 * OpenGL program object.
 */
class Program
{
private:
    std::shared_ptr<GLuint> const _id;
    GLint _getUniformLocation(std::string uniform) const;
public:
    Program(std::vector<Shader> shaders, std::string label="");

    /** Use the program. */
    void use() const;
    /** Get the program's id. */
    GLuint id() const;
    /** Set a boolean uniform. */
    void setUniform(std::string uniform, bool value) const;
    /** Set a floating-point uniform. */
    void setUniform(std::string uniform, GLfloat value) const;
    /** Set an integer uniform. */
    void setUniform(std::string uniform, GLint value) const;
    /** Set an unsigned integer uniform. */
    void setUniform(std::string uniform, GLuint value) const;
    /** Set a vec2 uniform. */
    void setUniform(std::string uniform, glm::vec2 value) const;
    /** Set a vec3 uniform. */
    void setUniform(std::string uniform, glm::vec3 value) const;
    /** Set a vec4 uniform. */
    void setUniform(std::string uniform, glm::vec4 value) const;
    /** Set a mat4 uniform. */
    void setUniform(std::string uniform, glm::mat4 value) const;
    /**
     * Set a uniform, without throwing exceptions. Return an error message on
     * failure, otherwise an empty string.
     */
    template<typename T>
    std::string setUniformS(std::string const &uniform, T const &value) const
    {
        try
        {
            setUniform(uniform, value);
        }
        catch (std::runtime_error const &e)
        {
            return "setUniformS(" + uniform + ") - " + e.what();
        }
        return "";
    }
};

/**
 * OpenGL buffer object.
 */
class Buffer
{
private:
    std::shared_ptr<GLuint> const _id;
public:
    GLenum target;

    Buffer(GLenum target, std::string label="");

    /** Get the buffer's id. */
    GLuint id() const;

    /** Bind the buffer to the previously used target. */
    void bind() const;
    /** Bind the buffer to a new target. */
    void bind(GLenum target);
    /** Unbind the buffer. */
    void unbind() const;

    /** Add data to the buffer. NOTE: The Buffer must be bound first! */
    template<typename T>
    void buffer(GLenum usage, std::vector<T> const &data)
    {
        glBufferData(target, data.size() * sizeof(T), data.data(), usage);
    }
    /** Update data in the buffer. NOTE: The Buffer must be bound first! */
    template<typename T>
    void update(std::vector<T> const &data, size_t offset=0, size_t count=0)
    {
        if (count == 0)
            count = data.size() - offset;
        glBufferSubData(
            target,
            offset * sizeof(T),
            count * sizeof(T),
            data.data() + offset);
    }
};

/**
 * OpenGL vertex array object.
 */
class VertexArray
{
private:
    std::shared_ptr<GLuint> const _id;
public:
    VertexArray(std::string label="");

    /** Get the vertex array's id. */
    GLuint id() const;

    /** Bind the vertex array. */
    void bind() const;
    /** Unbind the vertex array. */
    void unbind() const;

    /** Enable and associate a vertex attrib array. (By index) */
    void enableVertexAttribArray(
        GLuint index, GLint components, GLenum dataType, GLsizei stride=0,
        size_t offset=0, bool normalized=false) const;
};

/**
 * OpenGL texture object.
 */
class Texture
{
private:
    std::shared_ptr<GLuint> const _id;
    GLenum const _type;
public:
    /**
     * WARNING: Creating a Texture will also cause it to be bound to the passed
     * target!
     */
    Texture(GLenum type, std::string label="");

    /** Get the texture's id. */
    GLuint id() const;

    /** Get the texture's type. */
    GLuint type() const;

    /** Bind the texture. */
    void bind() const;
    /** Unbind the texture. */
    void unbind() const;

    /** Set a Texture parameter. NOTE: The Texture must be bound first! */
    void setParameter(GLenum pname, GLint param);
};


/** Load a GLSL shader from a file. */
Shader shader_from_file(std::string path, GLenum type);
}

#endif
