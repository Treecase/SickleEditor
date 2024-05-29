/**
 * BrushBox.cpp - Editor::BrushBox.
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

#include "BrushBox.hpp"

using namespace Sickle::Editor;


void BrushBox::p1(glm::vec3 const &v)
{
    _p1 = v;
    signal_updated().emit();
}


void BrushBox::p2(glm::vec3 const &v)
{
    _p2 = v;
    signal_updated().emit();
}


glm::vec3 BrushBox::p1() const
{
    return _p1;
}


glm::vec3 BrushBox::p2() const
{
    return _p2;
}
