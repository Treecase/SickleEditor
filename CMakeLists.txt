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

cmake_minimum_required(VERSION 3.13)

file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

project("Sickle Editor"
    VERSION 0.1.0
    DESCRIPTION "Open-Source GoldSrc Editor"
    LANGUAGES CXX
)


# Dependencies
add_subdirectory(extern/glutils)

set(imgui_DIR cmake)
find_package(imgui)

if(MSVC)
    set(SDL2_DIR cmake)
else()
    find_package(GLEW REQUIRED)
endif()
find_package(SDL2 REQUIRED)

# GL library depends on platform, so define an alias for it
add_library(SickleGL INTERFACE)
if(WIN32)
    target_link_libraries(SickleGL INTERFACE OpenGL32)
else()
    target_link_libraries(SickleGL INTERFACE GL)
endif()


# Declare targets
add_executable(sickle)
target_compile_features(sickle PRIVATE cxx_std_17)
set_target_properties(sickle PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(sickle PRIVATE "${PROJECT_BINARY_DIR}/include")

# Project sources
add_subdirectory(src)


if(MINGW)
    target_link_libraries(sickle PRIVATE mingw32 SDL2::SDL2main)
endif()

target_link_libraries(sickle PRIVATE
    imgui
    SDL2::SDL2
    GLEW::GLEW
    glm::glm
    SickleGL
    glutils::glutils
    ${CMAKE_DL_LIBS}
)
