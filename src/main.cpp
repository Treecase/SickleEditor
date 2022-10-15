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

#include "App.hpp"
#include "common.hpp"
#include "version.hpp"

#include "modules/BSPViewer.hpp"
#include "modules/MapViewer.hpp"
#include "modules/ModelViewer.hpp"
#include "modules/SoundPlayer.hpp"
#include "modules/TextureViewer.hpp"
#include "modules/WADTextureViewer.hpp"

#include <GL/glew.h>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <SDL.h>

#include <iostream>
#include <stdexcept>
#include <string>

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

#define APP_TITLE "Sickle Editor"


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
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
}


/** Set app config from command-line arguments. */
Config handle_args(int argc, char *argv[])
{
    Config cfg{};
    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--version") == 0)
        {
            std::cout << SE_CANON_NAME << " " << SE_VERSION << "\n" <<
                "Copyright (C) 2022 Trevor Last\n"
                "License GPLv3+: GNU GPL version 3 or later "
                    "<https://gnu.org/licenses/gpl.html>\n"
                "This is free software: you are free to change and "
                    "redistribute it.\n"
                "There is NO WARRANTY, to the extent permitted by law.\n";
            exit(EXIT_SUCCESS);
        }
        else if (strcmp(argv[i], "--help") == 0)
        {
            std::cout <<
                "Usage: " << argv[0] << " GAMEDEF.fgd MAPSDIR [GAMEDIR]\n"
                "Open-source GoldSrc editor.\n"
                "\n"
                "  --help\tdisplay this help and exit\n"
                "  --version\toutput version information and exit\n"
                "\n"
                "Report bugs to: "
                    "https://github.com/Treecase/SickleEditor/issues\n"
                "pkg home page: https://github.com/Treecase/SickleEditor\n";
            exit(EXIT_SUCCESS);
        }
    }
    // TODO: these shouldn't error out
    if (argc < 2)
    {
        std::cerr << "No .fgd supplied!\n";
        exit(EXIT_FAILURE);
    }
    else
        cfg.game_def = argv[1];
    if (argc < 3)
    {
        std::cerr << "No maps directory supplied!\n";
        exit(EXIT_FAILURE);
    }
    else
        cfg.maps_dir = argv[2];

    if (argc < 4)
        cfg.game_dir = "/";
    else
        cfg.game_dir = argv[3];
    return cfg;
}


/** Main program body. */
int run(int argc, char *argv[])
{
    // Handle command-line args.
    auto config = handle_args(argc, argv);


    /* ===[ Graphics Initialization ]=== */
    // Create SDL2 window and OpenGL context.
    int width = INITIAL_WINDOW_WIDTH,
        height = INITIAL_WINDOW_HEIGHT;
    SDL_Window *window = SDL_CreateWindow(
        APP_TITLE,
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


    // Create the app.
    config.window_width = &width;
    config.window_height = &height;
    App<
        BSPViewer,
        MapViewer,
        ModelViewer,
        SoundPlayer,
        TextureViewer,
        WADTextureViewer
    > app{config};


    /* ===[ Main Loop ]=== */
    auto lastFrame = SDL_GetTicks64();
    while (app.running)
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
                app.running = false;
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
            app.input(&event);
        }

        /* ===[ Update UI ]=== */
        ImGui_ImplSDL2_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();
        // Update app UI.
        app.drawUI();

        /* ===[ Update Screen ]=== */
        // Clear screen.
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Render non-UI app visuals.
        auto now = SDL_GetTicks64();
        app.drawGL((now - lastFrame) / 1000.0f);
        lastFrame = now;
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
#ifdef NDEBUG
    try
    {
        r = run(argc, argv);
    }
    catch (std::exception const &e)
    {
        std::cerr << "FATAL: " << e.what() << std::endl;
        SDL_ShowSimpleMessageBox(
            SDL_MESSAGEBOX_ERROR, "Error", e.what(), nullptr);
    }
#else
    r = run(argc, argv);
#endif

    // Global cleanup.
    SDL_Quit();
    return r;
}
