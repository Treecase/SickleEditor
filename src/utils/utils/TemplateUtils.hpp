/**
 * TemplateUtils.hpp - Template utilities.
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

#ifndef SE_TEMPLATEUTILS_HPP
#define SE_TEMPLATEUTILS_HPP

#include <cstddef>


/** Apply a callable over all the arguments. */
template<class Callable>
size_t foreach(Callable c)
{
    return 0;
}
template<class Callable, typename Arg, typename... Rest>
size_t foreach(Callable c, Arg arg, Rest... rest)
{
    c(arg);
    return 1 + foreach(c, rest...);
}

#endif
