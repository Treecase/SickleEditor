/**
 * BSPViewer.hpp - Map viewer module.
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

#ifndef _BSPVIEWER_HPP
#define _BSPVIEWER_HPP

#include "utils/Transform.hpp"
#include "bsp/bsp2gl.hpp"
#include "bsp/load_bsp.hpp"
#include "Base3DViewer.hpp"


/** Displays BSP files. */
class BSPViewer : public Base3DViewer
{
private:
    // Loaded map.
    BSP::BSP _map;
    // GL map.
    BSP::GLBSP _glbsp;
    // Path to loaded map.
    std::filesystem::path _selected;

    // Map transform.
    Transform _transform;

    /** Called when _selected is updated. */
    void _loadSelectedMap();

public:
    BSPViewer(Config &cfg);

    /** Draw the module's UI. */
    void drawUI() override;

    /** Draw non-UI module visuals. */
    void drawGL(float deltaT) override;
};

#endif
