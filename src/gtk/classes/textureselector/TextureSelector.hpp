/**
 * TextureSelector.hpp - Texture selection window.
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

#ifndef SE_APPWIN_TEXTURESELECTOR_HPP
#define SE_APPWIN_TEXTURESELECTOR_HPP

#include "TextureImage.hpp"

#include <core/Editor.hpp>
#include <wad/TextureManager.hpp>
#include <wad/wad.hpp>

#include <glibmm/property.h>
#include <glibmm/refptr.h>
#include <gtkmm/button.h>
#include <gtkmm/dialog.h>
#include <gtkmm/flowbox.h>
#include <gtkmm/searchentry.h>


namespace Sickle::TextureSelector
{
    /**
     * Allows the user to select a texture from available WAD files.
     */
    class TextureSelector : public Glib::Object
    {
    public:
        static Glib::RefPtr<TextureSelector> create(
            Glib::RefPtr<Editor::Editor> const &editor);

        /** Get the name of the texture currently selected by the user. */
        std::string get_selected_texture() const;

        /** See Gtk::Dialog::run. */
        int run();

    protected:
        void on_wads_changed();
        void on_search_changed();
        bool filter_func(Gtk::FlowBoxChild const *child);

    private:
        Gtk::Dialog *_dialog{nullptr};
        Gtk::SearchEntry *_search{nullptr};
        Gtk::FlowBox *_flow{nullptr};
        Gtk::Button *_cancel{nullptr};
        Gtk::Button *_confirm{nullptr};
        std::vector<TextureImage> _images{};

        Glib::RefPtr<Editor::Editor> _editor{nullptr};

        TextureSelector(Glib::RefPtr<Editor::Editor> const &editor);

        void _refresh_textures();
        void _clear_textures();
        void _add_textures();
    };
}

#endif
