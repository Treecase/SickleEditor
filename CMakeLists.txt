# Copyright (C) 2022-2023 Trevor Last
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
    VERSION 0.11.1
    DESCRIPTION "Open-Source GoldSrc Editor"
    LANGUAGES CXX
)

if(NOT CMAKE_BUILD_TYPE)
    message("CMAKE_BUILD_TYPE is unset, defaulting to Release")
    set(CMAKE_BUILD_TYPE Release)
endif()


# Application ID
set(APPLICATION_ID com.github.treecase.sicklegtk)
# GResource prefix
set(GRESOURCE_PREFIX /com/github/treecase/sicklegtk/)
# GSchema path
set(SCHEMA_DIR "${CMAKE_CURRENT_BINARY_DIR}")


# Dependencies
# GLEW
set(GLEW_VERBOSE TRUE)
find_package(GLEW REQUIRED)

# glutils
add_subdirectory(extern/glutils)

# gtkmm3
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTKMM REQUIRED IMPORTED_TARGET gtkmm-3.0)
pkg_check_modules(sigc++ REQUIRED IMPORTED_TARGET sigc++-2.0)

# GL library depends on platform, so define an alias for it
add_library(SickleGL INTERFACE)
if(WIN32)
    target_link_libraries(SickleGL INTERFACE OpenGL32)
else()
    target_link_libraries(SickleGL INTERFACE GL)
endif()


# Configure files
# Desktop file
configure_file(sicklegtk.desktop.in ${APPLICATION_ID}.desktop)
# Settings schema
configure_file(sicklegtk.gschema.xml.in sicklegtk.gschema.xml)

include_directories(src)

# Declare targets
add_executable(sickle)
target_compile_features(sickle PRIVATE cxx_std_17)

# Compiles settings gschema
add_custom_target(
    gschemas.compiled
    COMMAND
        glib-compile-schemas
        --strict
        --targetdir=.
        --schema-file=sicklegtk.gschema.xml
)
add_dependencies(sickle gschemas.compiled)

# Add .exe icon on Windows
if(WIN32)
    add_subdirectory(windows)
endif()

# Project sources
add_subdirectory(src)
# GResource data
add_subdirectory(gresource)


# Linking
if(MINGW)
    target_link_libraries(sickle PRIVATE mingw32)
endif()
target_link_libraries(sickle PRIVATE ${CMAKE_DL_LIBS})


install(TARGETS sickle)
install(DIRECTORY sickle TYPE DATA)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${APPLICATION_ID}.desktop"
    DESTINATION share/applications
)
if(WIN32)
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/gschemas.compiled"
        DESTINATION share/glib-2.0/schemas
    )
else()
    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/sicklegtk.gschema.xml"
        DESTINATION share/glib-2.0/schemas
        RENAME ${APPLICATION_ID}.gschema.xml
    )
endif()
install(FILES gresource/logo.svg
    DESTINATION share/icons/hicolor/scalable/apps
    RENAME ${APPLICATION_ID}.svg
)
