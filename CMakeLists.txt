# Copyright (C) 2022 Trevor Last
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.7...3.23)
if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()


project(Sickle
    VERSION 0.1.0
    DESCRIPTION "Open-Source GoldSrc Editor"
    LANGUAGES CXX
)

configure_file(
    "${PROJECT_SOURCE_DIR}/src/version.hpp.in"
    "${PROJECT_BINARY_DIR}/include/version.hpp"
)

add_executable(sickle)
target_sources(sickle PRIVATE
    src/bsp/bsp2gl.cpp
    src/bsp/entities_lump.cpp
    src/bsp/load_bsp.cpp
    src/mdl/load_model.cpp
    src/mdl/mdl2gl.cpp
    src/wad/load_wad.cpp
    src/wad/lumps.cpp
    src/App.cpp
    src/main.cpp
    src/MapViewer.cpp
    src/ModelViewer.cpp
    src/SoundPlayer.cpp
    src/TextureViewer.cpp
    src/ui_helpers.cpp
    src/WADTextureViewer.cpp
)
target_compile_features(sickle PRIVATE cxx_std_17)
set_target_properties(sickle PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(sickle PRIVATE "${PROJECT_BINARY_DIR}/include")


add_library(glUtil OBJECT)
target_sources(glUtil PRIVATE
    src/glUtils/Buffer.cpp
    src/glUtils/Program.cpp
    src/glUtils/Shader.cpp
    src/glUtils/Texture.cpp
    src/glUtils/VertexArray.cpp
)
target_compile_features(glUtil PRIVATE cxx_std_17)
set_target_properties(glUtil PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(glUtil PUBLIC src/glUtils)
target_link_libraries(sickle PRIVATE glUtil)

add_library(imgui OBJECT)
target_sources(imgui PRIVATE
    extern/imgui/backends/imgui_impl_sdl.cpp
    extern/imgui/backends/imgui_impl_opengl3.cpp
    extern/imgui/misc/cpp/imgui_stdlib.cpp
    extern/imgui/imgui.cpp
    extern/imgui/imgui_demo.cpp
    extern/imgui/imgui_draw.cpp
    extern/imgui/imgui_tables.cpp
    extern/imgui/imgui_widgets.cpp
)
target_compile_features(imgui PRIVATE cxx_std_17)
set_target_properties(imgui PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(imgui
    PUBLIC
    extern/imgui/
    extern/imgui/backends
)
target_link_libraries(sickle PRIVATE imgui)

if(MSVC)
    target_compile_options(sickle PRIVATE /W3)
    target_include_directories(sickle
        PRIVATE
        "externals/glew-2.2.0/include"
        "externals/SDL2-2.0.22/include"
        "externals/glm"
    )
    target_link_libraries(sickle
        PRIVATE
        "../externals/SDL2-2.0.22/lib/x64/SDL2main"
        "../externals/SDL2-2.0.22/lib/x64/SDL2"
        OpenGL32
        "../externals/glew-2.2.0/lib/Release/x64/glew32"
    )
    target_include_directories(glUtil
        PRIVATE
        "externals/glew-2.2.0/include"
        "externals/glm"
    )
    target_include_directories(imgui
        PRIVATE
        "externals/SDL2-2.0.22/include"
    )
    target_link_libraries(imgui PRIVATE
        "../externals/SDL2-2.0.22/lib/x64/SDL2main"
        "../externals/SDL2-2.0.22/lib/x64/SDL2"
    )
else()
    target_compile_options(sickle PRIVATE -Wall -Wextra -g)
    find_package(SDL2 REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(glm REQUIRED)
    target_link_libraries(sickle PRIVATE
        SDL2::SDL2
        GLEW::GLEW
        glm::glm
        GL
        ${CMAKE_DL_LIBS}
    )
    target_link_libraries(imgui PUBLIC SDL2::SDL2)
endif()
