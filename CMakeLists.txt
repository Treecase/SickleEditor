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

# Declare targets
add_executable(sickle)
target_compile_features(sickle PRIVATE cxx_std_17)
set_target_properties(sickle PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(sickle PRIVATE "${PROJECT_BINARY_DIR}/include")

# Dependencies
add_subdirectory(extern/glutils)
target_link_libraries(sickle PRIVATE glutils::glutils)

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
target_include_directories(imgui
    PUBLIC
    extern/imgui/
    extern/imgui/backends
)

if(MSVC)
    if(NOT TARGET SDL2::SDL2)
        add_library(SDL2 INTERFACE)
        target_include_directories(SDL2 INTERFACE externals/SDL2-2.0.22/include)
        target_link_libraries(SDL2 INTERFACE
            "${PROJECT_SOURCE_DIR}/externals/SDL2-2.0.22/lib/x64/SDL2main.lib"
            "${PROJECT_SOURCE_DIR}/externals/SDL2-2.0.22/lib/x64/SDL2.lib"
        )
        add_library(SDL2::SDL2 ALIAS SDL2)
    endif()

    if(NOT TARGET GLEW::GLEW)
        add_library(glew INTERFACE)
        target_include_directories(glew INTERFACE externals/glew-2.2.0/include)
        target_link_libraries(glew INTERFACE
            externals/glew-2.2.0/lib/Release/x64/glew32
        )
        add_library(GLEW::GLEW ALIAS glew)
    endif()

    # TODO: use submodule
    if(NOT TARGET glm::glm)
        add_library(glm INTERFACE)
        target_include_directories(glm INTERFACE externals/glm)
        add_library(glm::glm ALIAS glm)
    endif()
else()
    find_package(SDL2 REQUIRED)
    find_package(GLEW REQUIRED)
    find_package(glm REQUIRED)
endif()


# Project sources
add_subdirectory(src)


# Linkage
if(MSVC)
    target_link_libraries(sickle PRIVATE OpenGL32)
else()
    target_link_libraries(sickle PRIVATE GL)
endif()

target_link_libraries(sickle PRIVATE
    imgui
    SDL2::SDL2
    GLEW::GLEW
    glm::glm
    ${CMAKE_DL_LIBS}
)
target_link_libraries(imgui PUBLIC SDL2::SDL2)
