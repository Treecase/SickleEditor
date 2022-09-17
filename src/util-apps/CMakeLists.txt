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

add_executable(ListEntities
    ListEntities.cpp
    ../MapViewer.cpp
    ../ui_helpers.cpp
)
target_compile_features(ListEntities PRIVATE cxx_std_17)
set_target_properties(ListEntities PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(ListEntities PRIVATE "${PROJECT_BINARY_DIR}/include")

# Linkage
if(MSVC)
    target_link_libraries(ListEntities PRIVATE OpenGL32)
else()
    target_link_libraries(ListEntities PRIVATE GL)
endif()

target_link_libraries(ListEntities PRIVATE
    imgui
    bsp
    wad
    SDL2::SDL2
    GLEW::GLEW
    glm::glm
    ${CMAKE_DL_LIBS}
)


add_executable(PrintMap PrintMap.cpp)
target_include_directories(PrintMap PRIVATE "${PROJECT_BINARY_DIR}/include")
target_link_libraries(PrintMap PRIVATE map)

add_executable(LoadFGD LoadFGD.cpp)
target_include_directories(LoadFGD PRIVATE "${PROJECT_BINARY_DIR}/include")
target_link_libraries(LoadFGD PRIVATE fgd)
