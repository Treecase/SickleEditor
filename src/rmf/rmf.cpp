/**
 * rmf.cpp - Rich Map Format data.
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

#include "rmf/rmf.hpp"

#include <fstream>

using namespace RMF;


void rmfloadassert(bool expr, std::string const &what)
{
    if (!expr)
        throw RMF::LoadError(what);
}


int32_t readInt(std::istream &s)
{
    uint8_t b[4];
    s.read(reinterpret_cast<char *>(b), 4);
    uint32_t r = b[0] | (b[1] >> 8) | (b[2] >> 16) | (b[3] >> 24);
    return r;
}

float readFloat(std::istream &s)
{
    float f;
    s.read(reinterpret_cast<char *>(&f), 4);
    return f;
}

uint8_t readByte(std::istream &s)
{
    uint8_t b;
    s.read(reinterpret_cast<char *>(&b), 1);
    return b;
}

void readBytes(std::istream &s, std::streamsize n)
{
    char *b = new char[n];
    s.read(b, n);
    delete[] b;
}

std::string readString(std::istream &s, std::streamsize n)
{
    char *buf = new char[n + 1];
    s.read(buf, n);
    std::string str{buf};
    delete[] buf;
    return str;
}

std::string readNString(std::istream &s)
{
    uint8_t n = 0;
    s.read(reinterpret_cast<char *>(&n), 1);
    char *buf = new char[n + 1];
    s.read(buf, n);
    std::string str{buf};
    delete[] buf;
    return str;
}


Color readColor(std::istream &s)
{
    Color color{};
    color.r = readByte(s);
    color.g = readByte(s);
    color.b = readByte(s);
    return color;
}

VisGroup readVisGroup(std::istream &s)
{
    VisGroup visgroup{};
    visgroup.name = readString(s, 128);
    visgroup.color = readColor(s);
    readByte(s);
    visgroup.index = readInt(s);
    visgroup.visible = (readByte(s) != 0);
    readBytes(s, 3);
    return visgroup;
}

Vector readVector(std::istream &s)
{
    Vector vector{};
    vector.x = readFloat(s);
    vector.y = readFloat(s);
    vector.z = readFloat(s);
    return vector;
}

Face readFace(std::istream &s)
{
    Face face{};
    face.texture_name = readString(s, 256);
    readFloat(s);
    face.texture_u = readVector(s);
    face.texture_x_shift = readFloat(s);
    face.texture_v = readVector(s);
    face.texture_y_shift = readFloat(s);
    face.texture_rotation = readFloat(s);
    face.texture_x_scale = readFloat(s);
    face.texture_y_scale = readFloat(s);
    readBytes(s, 16);
    auto vertex_count = readInt(s);
    for (int i = 0; i < vertex_count; ++i)
        face.vertices.push_back(readVector(s));
    face.plane[0] = readVector(s);
    face.plane[1] = readVector(s);
    face.plane[2] = readVector(s);
    return face;
}

void readObject(std::istream &s, Group &group);

auto readSolid(std::istream &s, bool withheader=true)
{
    Solid solid{};
    if (withheader)
    {
        auto type = readNString(s);
        rmfloadassert(
            type == "CMapSolid",
            "Expected CMapSolid, got '" + type + "'");
    }
    solid.visgroup_index = readInt(s);
    solid.color = readColor(s);
    readBytes(s, 4);
    auto face_count = readInt(s);
    for (int32_t i = 0; i < face_count; ++i)
        solid.faces.push_back(readFace(s));
    return solid;
}

auto readEntity(std::istream &s, bool withheader=true)
{
    Entity entity{};
    if (withheader)
    {
        auto type = readNString(s);
        rmfloadassert(
            type == "CMapEntity",
            "Expected CMapEntity, got '" + type + "'");
    }
    entity.visgroup_index = readInt(s);
    entity.color = readColor(s);
    auto brush_count = readInt(s);
    for (int32_t i = 0; i < brush_count; ++i)
        entity.brushes.push_back(readSolid(s));
    entity.classname = readNString(s);
    readBytes(s, 4);
    entity.flags = readInt(s);
    auto kv_pairs_count = readInt(s);
    for (int32_t i = 0; i < kv_pairs_count; ++i)
    {
        auto key = readNString(s);
        auto value = readNString(s);
        entity.kv_pairs[key] = value;
    }
    readBytes(s, 14);
    entity.position = readVector(s);
    readBytes(s, 4);
    return entity;
}

auto readGroup(std::istream &s, bool withheader=true)
{
    Group group{};
    if (withheader)
    {
        auto type = readNString(s);
        rmfloadassert(
            type == "CMapGroup",
            "expected CMapGroup, got '" + type + "'");
    }
    group.visgroup_index = readInt(s);
    group.color = readColor(s);
    auto object_count = readInt(s);
    for (int32_t i = 0; i < object_count; ++i)
        readObject(s, group);
    return group;
}

void readObject(std::istream &s, Group &group)
{
    auto type = readNString(s);
    if (type == "CMapSolid")
        group.brushes.push_back(readSolid(s, false));
    else if (type == "CMapEntity")
        group.entities.push_back(readEntity(s, false));
    else if (type == "CMapGroup")
        group.groups.push_back(readGroup(s, false));
    else
        throw RMF::LoadError{"Invalid Object type '" + type + "'"};
}

auto readCorner(std::istream &s)
{
    Corner corner{};
    corner.position = readVector(s);
    corner.index = readInt(s);
    corner.name_override = readString(s, 128);
    auto kv_pairs_count = readInt(s);
    for (int32_t i = 0; i < kv_pairs_count; ++i)
    {
        auto key = readNString(s);
        auto value = readNString(s);
        corner.kv_pairs[key] = value;
    }
    return corner;
}

auto readPath(std::istream &s)
{
    Path path{};
    path.name = readString(s, 128);
    path.class_ = readString(s, 128);
    path.type = readInt(s);
    auto corner_count = readInt(s);
    for (int32_t i = 0; i < corner_count; ++i)
        path.corners.push_back(readCorner(s));
    return path;
}

auto readCamera(std::istream &s)
{
    Camera camera{};
    camera.eye = readVector(s);
    camera.look = readVector(s);
    return camera;
}


RichMap RMF::load(std::string const &path)
{
    std::ifstream f{path, std::ios::in | std::ios::binary};
    f.exceptions(std::ios_base::failbit | std::ios_base::eofbit);
    RichMap map{};

    map.version = readFloat(f);

    char rmf[3];
    f.read(rmf, 3);
    rmfloadassert(
        rmf[0] == 'R' && rmf[1] == 'M' && rmf[2] == 'F',
        "Missing RMF identifier");

    auto visgroup_count = readInt(f);
    for (int32_t i = 0; i < visgroup_count; ++i)
        map.visgroups.push_back(readVisGroup(f));

    auto cmapworld = readNString(f);
    rmfloadassert(
        cmapworld == "CMapWorld",
        "Expected CMapWorld, got '" + cmapworld + "'");
    readBytes(f, 7);

    auto object_count = readInt(f);
    for (int32_t i = 0; i < object_count; ++i)
        readObject(f, map.objects);

    map.worldspawn_name = readNString(f);
    readBytes(f, 4);

    auto worldspawn_flags = readInt(f); // unused?
    auto worldspawn_kv_count = readInt(f);
    for (int32_t i = 0; i < worldspawn_kv_count; ++i)
    {
        auto key = readNString(f);
        auto value = readNString(f);
        map.worldspawn_properties[key] = value;
    }
    readBytes(f, 12);
    auto path_count = readInt(f);
    for (int32_t i = 0; i < path_count; ++i)
        map.paths.push_back(readPath(f));
    auto docinfo = readString(f, 8);
    rmfloadassert(
        docinfo == "DOCINFO",
        "Expected DOCINFO, got '" + docinfo + "'");
    readFloat(f);
    map.active_camera = readInt(f);
    auto camera_count = readInt(f);
    for (int32_t i = 0; i < camera_count; ++i)
        map.cameras.push_back(readCamera(f));

    return map;
}
