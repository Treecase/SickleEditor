/**
 * DebugDrawer3D.hpp - Draw simple 3D shapes for debugging.
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

#ifndef SE_DEBUGDRAWER3D_HPP
#define SE_DEBUGDRAWER3D_HPP

#include <glutils/glutils.hpp>

class DebugDrawer3D
{
public:
    char const *const rayShaderVertexSource{
        "#version 430 core\n"
        "layout(location=0) in vec3 vPos;"
        "uniform mat4 view;"
        "uniform mat4 projection;"
        "void main()"
        "{"
        "    gl_Position = projection * view * vec4(vPos, 1.0);"
        "}"};
    char const *const rayShaderFragmentSource{"#version 430 core\n"
                                              "out vec4 FragColor;"
                                              "uniform vec3 color;"
                                              "void main()"
                                              "{"
                                              "    FragColor = vec4(color, 1);"
                                              "}"};

    std::shared_ptr<GLUtil::Program> rayShader{nullptr};
    std::shared_ptr<GLUtil::VertexArray> rayVAO{nullptr};
    std::shared_ptr<GLUtil::Buffer> rayVBO{nullptr};

    void init()
    {
        rayShader.reset(new GLUtil::Program{
            {GLUtil::Shader{GL_VERTEX_SHADER, rayShaderVertexSource},
             GLUtil::Shader{GL_FRAGMENT_SHADER, rayShaderFragmentSource}}
        });
        rayVAO.reset(new GLUtil::VertexArray{"DebugRayVAO"});
        rayVBO.reset(new GLUtil::Buffer{GL_ARRAY_BUFFER, "DebugRayVAO"});
        rayVAO->bind();
        rayVBO->bind();
        rayVBO->buffer(GL_DYNAMIC_DRAW, std::vector<float>{0, 0, 0, 0, 0, 0});
        rayVAO->enableVertexAttribArray(0, 3, GL_FLOAT, 3 * sizeof(float));
        rayVBO->unbind();
        rayVAO->unbind();
    }

    void setRayPoints(glm::vec3 const &start, glm::vec3 const &end)
    {
        rayVBO->bind();
        rayVBO->update(
            std::vector<float>{start.x, start.y, start.z, end.x, end.y, end.z});
        rayVBO->unbind();
    }

    void drawRay(glm::mat4 const &view, glm::mat4 const &proj)
    {
        rayVAO->bind();
        rayShader->use();
        rayShader->setUniformS("view", view);
        rayShader->setUniformS("projection", proj);
        rayShader->setUniformS("color", glm::vec3{1, 0, 0});
        glDrawArrays(GL_LINES, 0, 2);
    }
};

#endif
