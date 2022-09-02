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


add_library(glutil OBJECT
    Buffer.cpp
    Program.cpp
    Shader.cpp
    Texture.cpp
    VertexArray.cpp
)
target_include_directories(glutil PUBLIC .)

if(MSVC)
    target_include_directories(glutil
        PRIVATE
        "${PROJECT_SOURCE_DIR}/externals/glew-2.2.0/include"
        "${PROJECT_SOURCE_DIR}/externals/glm"
    )
endif()
