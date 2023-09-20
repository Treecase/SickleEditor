/**
 * TextureImage.hpp - Texture selection window.
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

#ifndef SE_TEXTURESELECTOR_TEXTUREIMAGE_HPP
#define SE_TEXTURESELECTOR_TEXTUREIMAGE_HPP

#include <wad/wad.hpp>
#include <wad/lumps.hpp>

#include <glibmm/refptr.h>
#include <gtkmm/box.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>


namespace Sickle::TextureSelector
{
    /**
     * Displays a WAD texture and its name.
     */
    class TextureImage : public Gtk::Box
    {
    public:
        TextureImage(WAD::Lump const &lump);

        /** Get the Texture's name in the WAD. */
        std::string get_name() const;

    private:
        std::string _name;
        // Image's Pixbuf needs the data buffer to stay alive.
        std::vector<guint8> _rgb_data{};

        Gtk::Image _image;
        Gtk::Label _label;

        static decltype(_rgb_data) convert_texlump_to_rgb_data(
            WAD::TexLump const &texlump);
    };
}

#endif
