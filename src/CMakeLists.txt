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

configure_file(version.hpp.in "${PROJECT_BINARY_DIR}/include/version.hpp")

add_subdirectory(bsp)
add_subdirectory(fgd)
add_subdirectory(gtk)
add_subdirectory(map)
add_subdirectory(mdl)
add_subdirectory(modules)
add_subdirectory(util-apps)
add_subdirectory(utils)
add_subdirectory(wad)

target_sources(sickle PRIVATE main.cpp)

target_include_directories(sickle PRIVATE .)
target_link_libraries(sickle PRIVATE bsp bspgl fgd map mapgl mdl mdlgl wad)
