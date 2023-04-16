/**
 * rmf2map.cpp - Convert RMF data to MAP data.
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

#include "rmf/rmf2map.hpp"

#include <cassert>


MAP::Map mergeMaps(MAP::Map const &a, MAP::Map const &b);

/** Convert an RMF Face to a MAP Plane. */
MAP::Plane convertPlane(RMF::Face const &face)
{
    MAP::Plane p{
        .a={face.plane[0].x, face.plane[0].y, face.plane[0].z},
        .b={face.plane[1].x, face.plane[1].y, face.plane[1].z},
        .c={face.plane[2].x, face.plane[2].y, face.plane[2].z},
        .vertices={},
        .miptex=face.texture_name,
        .s={face.texture_u.x, face.texture_u.y, face.texture_u.z},
        .t={face.texture_v.x, face.texture_v.y, face.texture_v.z},
        .offsets={face.texture_x_shift, face.texture_y_shift},
        .rotation=face.texture_rotation,
        .scale={face.texture_x_scale, face.texture_y_scale}
    };
    for (auto const &vertex : face.vertices)
        p.vertices.emplace(p.vertices.begin(), vertex.x, vertex.y, vertex.z);
    return p;
}

/** Convert an RMF Solid to a MAP Brush. */
MAP::Brush convertBrush(RMF::Solid const &brush)
{
    MAP::Brush b{};
    for (auto const &face : brush.faces)
        b.planes.push_back(convertPlane(face));
    return b;
}

/** Convert an RMF Entity to a MAP Entity. */
MAP::Entity convertEntity(RMF::Entity const &entity)
{
    MAP::Entity e{};
    for (auto const &brush : entity.brushes)
        e.brushes.push_back(convertBrush(brush));
    e.properties["classname"] = entity.classname;
    for (auto const &kv : entity.kv_pairs)
        e.properties[kv.first] = kv.second;
    return e;
}

void convertGroup(RMF::Group const &group, MAP::Map &map)
{
    auto &worldspawn = map.entities.front();

    for (auto const &brush : group.brushes)
        worldspawn.brushes.push_back(convertBrush(brush));

    for (auto const &entity : group.entities)
        map.entities.push_back(convertEntity(entity));

    for (auto const &g : group.groups)
        convertGroup(g, map);
}

MAP::Map convertRMF(RMF::RichMap const &rmf)
{
    MAP::Map map{};
    MAP::Entity &worldspawn = map.entities.emplace_back();
    worldspawn.properties["classname"] = "worldspawn";

    for (auto const &kv : rmf.worldspawn_properties)
        worldspawn.properties[kv.first] = kv.second;

    worldspawn.properties["classname"] = rmf.worldspawn_name;
    worldspawn.properties["wad"] = (
        "C:/Program Files (x86)/Steam/steamapps/common/Half-Life SDK/Texture Wad Files/decals.wad;"
        "C:/Program Files (x86)/Steam/steamapps/common/Half-Life SDK/Texture Wad Files/halflife.wad;"
        "C:/Program Files (x86)/Steam/steamapps/common/Half-Life SDK/Texture Wad Files/liquids.wad;"
        "C:/Program Files (x86)/Steam/steamapps/common/Half-Life SDK/Texture Wad Files/spraypaint.wad;"
        "C:/Program Files (x86)/Steam/steamapps/common/Half-Life SDK/Texture Wad Files/xeno.wad;"
        "C:/Users/trevo/AppData/Local/vhlt/tools/zhlt.wad"
    );

    convertGroup(rmf.objects, map);

    return map;
}
