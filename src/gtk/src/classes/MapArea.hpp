/**
 * MapArea.hpp - Sickle editor main window GLArea.
 * Copyright (C) 2022 Trevor Last
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

#ifndef _SE_MAPAREA_HPP
#define _SE_MAPAREA_HPP

#include "../../../Transform.hpp"
#include "../../../FreeCam.hpp"
#include "../../../map/load_map.hpp"
#include "../../../map/map2gl.hpp"
#include "../../../wad/load_wad.hpp"
#include "../../../wad/lumps.hpp"
#include "../../../wad/TextureManager.hpp"

#include <gtkmm.h>

#include <filesystem>


namespace Sickle
{
    /** Displays .map files. */
    class MapArea : public Gtk::GLArea
    {
    public:
        MapArea(BaseObjectType *cobject, Glib::RefPtr<Gtk::Builder> const &refBuilder);

        void set_map(MAP::Map const &map);

        // Signal handlers
        /** Where GL initialization should be done. */
        void on_realize() override;
        /** Where GL cleanup should be done. */
        void on_unrealize() override;
        /** Draw everything we need. */
        bool on_render(Glib::RefPtr<Gdk::GLContext> const &context) override;

    protected:
        Glib::RefPtr<Gtk::Builder> m_refBuilder;

        // Loaded map.
        MAP::GLMap _glmap;

    private:
        // Shader.
        std::shared_ptr<GLUtil::Program> _shader;
        // Camera.
        FreeCam _camera;

        // Properties
        // Wireframe display toggle.
        bool _wireframe;
        /** Speed multiplier when shift key is held down. */
        float _shift_multiplier;

        // Map transform
        Transform _transform;
    };
}

#endif
