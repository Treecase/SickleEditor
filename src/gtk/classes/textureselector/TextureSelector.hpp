/**
 * TextureSelector.hpp - Texture selection window.
 * Copyright (C) 2023-2024 Trevor Last
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

#include <glibmm/property.h>
#include <glibmm/refptr.h>
#include <gtkmm/button.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/dialog.h>
#include <gtkmm/flowbox.h>
#include <gtkmm/searchentry.h>

#include <memory>
#include <string>
#include <vector>


namespace Sickle::TextureSelector
{
    /**
     * Allows the user to select a texture from available WAD files.
     */
    class TextureSelector : public Glib::Object
    {
    public:
        /**
         * Create a new texture selector.
         *
         * @return A newly constructed texture selector.
         */
        static Glib::RefPtr<TextureSelector> create();

        /**
         * Get the name of the texture currently selected by the user.
         *
         * @return Name of the selected texture.
         */
        std::string get_selected_texture() const;

        /**
         * Set the WAD filter. This whitelists only textures from this WAD.
         *
         * @param filter Name of the WAD to include.
         */
        void set_wad_filter(std::string const &filter);

        /** See Gtk::Dialog::run. */
        int run();

    protected:
        void on_wads_changed();
        void on_search_changed();
        void on_wad_filter_changed();

        void on_TextureManager_wads_changed();

        bool filter_func(Gtk::FlowBoxChild const *child) const;
        int sort_func(
            Gtk::FlowBoxChild const *a,
            Gtk::FlowBoxChild const *b) const;

    private:
        Gtk::Dialog *_dialog{nullptr};
        Gtk::SearchEntry *_search{nullptr};
        Gtk::ComboBoxText *_wad_filter{nullptr};
        Gtk::FlowBox *_flow{nullptr};
        Gtk::Button *_cancel{nullptr};
        Gtk::Button *_confirm{nullptr};

        std::vector<std::shared_ptr<TextureImage>> _images{};

        TextureSelector();

        void _refresh_textures();
        void _clear_textures();
        void _add_textures();
    };
}

#endif
