/**
 * TextureViewer.hpp - MDL texture viewer app.
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

#ifndef _TEXTUREVIEWER_HPP
#define _TEXTUREVIEWER_HPP

#include "load_model.hpp"
#include "glUtils/glUtil.hpp"
#include "version.hpp"

#include <GL/glew.h>
#include <imgui.h>
#include <SDL.h>

#include <iostream>
#include <unordered_map>
#include <vector>


/** Convert a MDL texture into a GL Texture. */
GLUtil::Texture texture2GLTexture(MDL::Texture const &texture);


/** Displays Textures contained in a .MDL file. */
class TextureViewer
{
private:
    // Screenquad vertex data.
    static std::vector<GLfloat> const _sqv;
    // Screenquad shader.
    std::shared_ptr<GLUtil::Program> _shader;
    // Screenquad VAO.
    std::shared_ptr<GLUtil::VertexArray> _vao;

public:
    // App title.
    std::string title;
    // List of MDLs loaded.
    std::vector<MDL::Model> models{};
    // Map of MDLs loaded to a list of their associated GL textures.
    std::unordered_map<std::string, std::vector<GLUtil::Texture>> model_textures;
    // MDL filenames to load.
    struct AppConfig
    {
        std::vector<char const *> models;
    } config;
    // Index of currently displayed model.
    int current_model;
    // Index of currently displayed texture.
    int current_texture;


    TextureViewer()
    :   _shader{nullptr}
    ,   _vao{nullptr}
    ,   title{"Texture Viewer"}
    ,   models{}
    ,   model_textures{}
    ,   config{}
    ,   current_model{0}
    ,   current_texture{0}
    {
    }

    /** Configure app from command-line arguments. */
    void handleArgs(int argc, char *argv[])
    {
        if (argc == 1)
        {
            std::cerr << "No models passed\n";
            exit(0);
        }
        for (int i = 1; i < argc; ++i)
        {
            config.models.push_back(argv[i]);
            if (strcmp(argv[i], "--version") == 0)
            {
                std::cout << SE_CANON_NAME << " " << SE_VERSION << "\n" <<
                    "Copyright (C) 2022 Trevor Last\n"
                    "License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>\n"
                    "This is free software: you are free to change and redistribute it.\n"
                    "There is NO WARRANTY, to the extent permitted by law.\n";
                exit(0);
            }
            else if (strcmp(argv[i], "--help") == 0)
            {
                std::cout <<
                    "Usage: " << argv[0] << " [MODEL]...\n"
                    "\n"
                    "  --help\tdisplay this help and exit\n"
                    "  --version\toutput version information and exit\n"
                    "\n"
                    "Report bugs to: https://github.com/Treecase/Sickle/issues\n"
                    "pkg home page: https://github.com/Treecase/Sickle\n";
                exit(0);
            }
        }
    }

    /** Load non-GL-context-requiring app data. */
    void loadData()
    {
        for (auto path : config.models)
        {
            models.push_back(MDL::load_mdl(path));
        }
    }

    /** Load app data which requires a GL context. */
    void loadGL()
    {
        // Create screenquad shader program.
        _shader.reset(new GLUtil::Program{
            {   GLUtil::shader_from_file(
                    "shaders/vertex.vert", GL_VERTEX_SHADER),
                GLUtil::shader_from_file(
                    "shaders/fragment.frag", GL_FRAGMENT_SHADER)},
            "ScreenQuadShader"});
        // Create screenquad vao.
        _vao.reset(new GLUtil::VertexArray{"ScreenQuadVAO"});
        _vao->bind();
        // Screenquad vbo.
        GLUtil::Buffer vbo{GL_ARRAY_BUFFER, "ScreenQuadVBO"};
        vbo.bind();
        vbo.buffer(GL_STATIC_DRAW, _sqv);
        // Positions array.
        _vao->enableVertexAttribArray(0, 3, GL_FLOAT, 5 * sizeof(GLfloat));
        // UV array.
        _vao->enableVertexAttribArray(
            1, 2, GL_FLOAT, 5 * sizeof(GLfloat), 3 * sizeof(GLfloat));
        vbo.unbind();
        _vao->unbind();

        // Generate textures from the MDL.
        for (auto mdl : models)
        {
            model_textures[mdl.name] = std::vector<GLUtil::Texture>{};
            for (auto tex : mdl.textures)
            {
                model_textures[mdl.name].push_back(texture2GLTexture(tex));
            }
        }
    }

    /** Handle user input. */
    void input(SDL_Event const *event)
    {
    }

    /** Draw the app's UI. */
    void drawUI()
    {
        ImGui::Begin("Texture Picker");
        if (ImGui::SliderInt(
            "Model",
            &current_model,
            0, models.size()-1,
            "%d", ImGuiSliderFlags_AlwaysClamp))
        {
            current_texture = 0;
        }
        ImGui::TextUnformatted(models[current_model].name.c_str());
        ImGui::SliderInt(
            "Texture",
            &current_texture,
            0, model_textures[models[current_model].name].size()-1,
            "%d", ImGuiSliderFlags_AlwaysClamp);
        ImGui::TextUnformatted(
            models[current_model].textures[current_texture].name.c_str());
        ImGui::TextUnformatted((
            "Originally "
            + std::to_string(
                models[current_model].textures[current_texture].w
            )
            + "x"
            + std::to_string(
                models[current_model].textures[current_texture].h)).c_str());
        int tex_w, tex_h;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tex_w);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tex_h);
        ImGui::TextUnformatted((
            "Sampled to " + std::to_string(tex_w) + "x"
            + std::to_string(tex_h)).c_str());
        ImGui::End();
    }

    /** Draw non-UI app visuals. */
    void drawGL()
    {
        // Draw screen.
        _shader->use();
        _vao->bind();
        glActiveTexture(GL_TEXTURE0);
        auto tex = model_textures[models[current_model].name][current_texture];
        tex.bind();
        _shader->setUniformS("tex", 0);
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)(_sqv.size() / 5));
    }
};


/** Convert from MDL texture format to an OpenGL texture object. */
GLUtil::Texture texture2GLTexture(MDL::Texture const &texture)
{
    // Calculate resized power-of-two dimensions.
    int resized_w = texture.w > 256? 256 : 1;
    int resized_h = texture.h > 256? 256 : 1;
    for (; resized_w < texture.w && resized_w < 256; resized_w *= 2)
        ;
    for (; resized_h < texture.h && resized_h < 256; resized_h *= 2)
        ;

    // Generate the resized texture.
    auto unpaletted = new GLubyte[resized_w * resized_h * 4];
    for (int y = 0; y < resized_h; ++y)
    {
        for (int x = 0; x < resized_w; ++x)
        {
            // The x,y coordinates of the samples.
            int x1 = ((x + 0.25) / (float)resized_w) * texture.w;
            int x2 = ((x + 0.75) / (float)resized_w) * texture.w;
            int y1 = ((y + 0.25) / (float)resized_h) * texture.h;
            int y2 = ((y + 0.75) / (float)resized_h) * texture.h;
            // Point to the palette values of the samples.
            uint8_t const *samples[4] = {
                texture.palette + texture.data[y1*texture.w + x1] * 3,
                texture.palette + texture.data[y1*texture.w + x2] * 3,
                texture.palette + texture.data[y2*texture.w + x1] * 3,
                texture.palette + texture.data[y2*texture.w + x2] * 3
            };
            // RGB value of the averaged samples.
            int averages[3] = {
                (samples[0][0]+samples[1][0]+samples[2][0]+samples[3][0])/4,
                (samples[0][1]+samples[1][1]+samples[2][1]+samples[3][1])/4,
                (samples[0][2]+samples[1][2]+samples[2][2]+samples[3][2])/4,
            };
            int offset = (y * resized_w + x) * 4;
            unpaletted[offset+0] = averages[0];
            unpaletted[offset+1] = averages[1];
            unpaletted[offset+2] = averages[2];
            unpaletted[offset+3] = 0xFF;
        }
    }

    // Create the GL texture.
    GLUtil::Texture t{GL_TEXTURE_2D, texture.name};
    t.setParameter(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    t.setParameter(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(
        t.type(), 0, GL_RGBA, resized_w, resized_h, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, unpaletted);
    delete[] unpaletted;
    return t;
}


std::vector<GLfloat> const TextureViewer::_sqv{
    // Positions        Texcoords
    // Top right tri
    -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, // tl
    1.0f,  1.0f, 0.0f,  1.0f, 0.0f, // tr
    1.0f, -1.0f, 0.0f,  1.0f, 1.0f, // br
    // Bottom left tri
    1.0f, -1.0f, 0.0f,  1.0f, 1.0f, // br
    -1.0f, -1.0f, 0.0f,  0.0f, 1.0f, // bl
    -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, // tl
};

#endif
