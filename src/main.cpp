/**
 * main.cpp - Program entry point.
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

#include "SoundPlayer.hpp"
#include "TextureViewer.hpp"
#include "load_model.hpp"
#include "glUtils/glUtil.hpp"

#include <GL/glew.h>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <SDL.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

// Name of main function depends on platform.
#ifdef _WIN32
#define MAIN SDL_main
#else
#define MAIN main
#endif

#define STRINGIFY(x) #x
#define APPEND(pre, sepstr, suf) STRINGIFY(pre) sepstr STRINGIFY(suf)

// OpenGL context major version number.
#define SE_GL_VERSION_MAJOR 4
// OpenGL context minor version number.
#define SE_GL_VERSION_MINOR 3

#define INITIAL_WINDOW_WIDTH 640
#define INITIAL_WINDOW_HEIGHT 480


/** Callback to print OpenGL debug messages. */
void opengl_debug_message_callback(
    GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length, GLchar const *message,
    void const *userParam)
{
    std::cout << "OpenGL: ";
    if (type == GL_DEBUG_TYPE_ERROR)
    {
        std::cout << "** GL ERROR ** ";
    }
    std::cout << message << "\n";
}


/** SDL initialization. */
void init_SDL()
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    // Set OpenGL context version and profile.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, SE_GL_VERSION_MAJOR);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, SE_GL_VERSION_MINOR);
    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    // Enable VSync. First try adaptive, if that's not available, use regular.
    if (SDL_GL_SetSwapInterval(-1) == -1)
    {
        SDL_GL_SetSwapInterval(1);
    }
}


/** OpenGL initialization. */
void init_OpenGL()
{
    GLenum error = glewInit();
    if (error != GLEW_OK)
    {
        throw std::runtime_error{
            "glewInit - " + std::string{(char *)glewGetErrorString(error)}};
    }
    auto glversion =
        "GL_VERSION_" APPEND(SE_GL_VERSION_MAJOR, "_", SE_GL_VERSION_MINOR);
    if (glewIsSupported(glversion) == GL_FALSE)
    {
        throw std::runtime_error{
            "GLEW: OpenGL Version "
            APPEND(SE_GL_VERSION_MAJOR, ".", SE_GL_VERSION_MINOR)
            " not supported"
        };
    }
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(opengl_debug_message_callback, nullptr);
}


/** Main program body. */
int run(int argc, char *argv[])
{
    SoundPlayer snd{};
    TextureViewer app{};
    // Handle command-line args.
    snd.handleArgs(argc, argv);
    app.handleArgs(argc, argv);
    // Load non-GL app data.
    snd.loadData();
    app.loadData();


    /* ===[ Graphics Initialization ]=== */
    // Create SDL2 window and OpenGL context.
    int width = INITIAL_WINDOW_WIDTH,
        height = INITIAL_WINDOW_HEIGHT;
    SDL_Window *window = SDL_CreateWindow(
        app.title.c_str(),
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        width, height,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    SDL_GLContext context = SDL_GL_CreateContext(window);

    // OpenGL initialization.
    init_OpenGL();
    glViewport(0, 0, width, height);

    // Init Dear ImGui.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window, context);
    ImGui_ImplOpenGL3_Init(
        "#version "
        APPEND(SE_GL_VERSION_MAJOR, "", SE_GL_VERSION_MINOR)
        "0");


    // Load app data requiring a GL context.
    snd.loadGL();
    app.loadGL();


    /* ===[ Main Loop ]=== */
    for (bool running = true; running; )
    {
        /* ===[ Event Handling ]=== */
        for (SDL_Event event; SDL_PollEvent(&event); )
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            auto io = ImGui::GetIO();
            // Don't pass events to main app if ImGui wants it.
            if (io.WantCaptureMouse || io.WantCaptureKeyboard)
            {
                continue;
            }

            // Built-in events.
            switch (event.type)
            {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    width = event.window.data1;
                    height = event.window.data2;
                    glViewport(0, 0, width, height);
                }
                break;
            }

            // App event handling.
            snd.input(&event);
            app.input(&event);
        }

        /* ===[ Update UI ]=== */
        ImGui_ImplSDL2_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();
        ImGui::BeginMainMenuBar();
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit"))
                running = false;
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Windows"))
        {
            if (ImGui::MenuItem("Sound Player"))
                snd.ui_visible = true;
            if (ImGui::MenuItem("Texture Viewer"))
                app.ui_visible = true;
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
        ImGui::ShowMetricsWindow();
        // Update app UI.
        snd.drawUI();
        app.drawUI();
        ImGui::EndFrame();

        /* ===[ Update Screen ]=== */
        // Clear screen.
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Render non-UI app visuals.
        snd.drawGL();
        app.drawGL();
        // Render the UI.
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        // Update the screen.
        SDL_GL_SwapWindow(window);
    }


    /* ===[ Shutdown ]=== */
    // Dear ImGui.
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    // OpenGL context + SDL2 window.
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);

    return EXIT_SUCCESS;
}


/** Program entry point. */
int MAIN(int argc, char *argv[])
{
    int r = EXIT_FAILURE;
    // Global initialization.
    init_SDL();

    // Run the program.
    try
    {
        r = run(argc, argv);
    }
    catch (std::exception const &e)
    {
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR, "Error", e.what(), nullptr);
        std::cerr << "FATAL: " << e.what() << std::endl;
    }

    // Global cleanup.
    SDL_Quit();
    return r;
}
