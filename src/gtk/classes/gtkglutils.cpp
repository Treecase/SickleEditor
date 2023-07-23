/**
 * gtkglutils.cpp - GLUtils extensions for use with GTK.
 * Copyright (C) 2023 Trevor Last
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

#include "gtkglutils.hpp"
#include "appid.hpp"

#include <gtkmm/glarea.h>
#include <giomm/resource.h>


GLUtil::Shader
GLUtil::shader_from_resource(std::string const &path, GLenum type)
{
    auto const &b = Gio::Resource::lookup_data_global(
        SE_GRESOURCE_PREFIX + path);
    gsize size = 0;
    return {type, static_cast<char const *>(b->get_data(size)), path};
}
