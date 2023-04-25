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
#include <iomanip>
#include <iostream>
#include <sstream>


#if RMFENABLEDEBUG
static std::ofstream dbgstream;

std::string dbgbyte(uint8_t byte)
{
    std::stringstream ss{};
    ss << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    return ss.str();
}

template<typename T>
void dbg_lo(T arg)
{
    dbgstream << arg;
}

template<typename T, typename... Ts>
void dbg_lo(T arg, Ts... args)
{
    dbgstream << arg;
    dbg_lo(args...);
}

template<typename... Ts>
void dbg(std::istream &s, Ts... ts)
{
    std::stringstream ss{};
    ss << std::hex << std::setw(8) << std::setfill('0') << s.tellg();
    dbgstream << ss.str();
    dbg_lo(ts...);
}
#else
#define dbg(...) (0)
#define dbg_lo(...) (0)
#define dbgbyte(...) (0)
#endif


using namespace RMF;


std::string
LoadError::_make_what(std::streampos where, std::string const &what)
{
    std::stringstream ss{};
    ss << std::hex << std::setw(8) << std::setfill('0') << where << " " << what;
    return ss.str();
}


void rmfloadassert(std::istream &s, bool expr, std::string const &what)
{
    if (!expr)
        throw RMF::LoadError(s.tellg(), what);
}


int32_t readInt(std::istream &s)
{
    dbg(s, " INT(");
    uint8_t b[4];
    s.read(reinterpret_cast<char *>(b), 4);
    uint32_t r = b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24);
    dbg_lo(r, ")\n");
    return r;
}

float readFloat(std::istream &s)
{
    dbg(s, " FLOAT(");
    float f;
    s.read(reinterpret_cast<char *>(&f), 4);
    dbg_lo(f, ")\n");
    return f;
}

uint8_t readByte(std::istream &s)
{
    dbg(s, " BYTE(");
    uint8_t b;
    s.read(reinterpret_cast<char *>(&b), 1);
    dbg_lo(dbgbyte(b), ")\n");
    return b;
}

void readBytes(std::istream &s, std::streamsize n)
{
    dbg(s, " BYTES(", n, ")\n");
    char *b = new char[n];
    s.read(b, n);
    delete[] b;
}

std::string readString(std::istream &s, std::streamsize n)
{
    dbg(s, " STRING(", n, ", \"");
    char *buf = new char[n + 1];
    s.read(buf, n);
    std::string str{buf};
    dbg_lo(str, "\")\n");
    delete[] buf;
    return str;
}

std::string readNString(std::istream &s)
{
    dbg(s, " NSTRING(");
    uint8_t n = 0;
    s.read(reinterpret_cast<char *>(&n), 1);
    dbg_lo((int)n, ", \"");
    char *buf = new char[n + 1];
    s.read(buf, n);
    std::string str{buf};
    dbg_lo(str, "\")\n");
    delete[] buf;
    return str;
}


Color readColor(std::istream &s)
{
    dbg(s, " Color(\n");
    Color color{};
    color.r = readByte(s);
    color.g = readByte(s);
    color.b = readByte(s);
    dbg(s, " Color)\n");
    return color;
}

VisGroup readVisGroup(std::istream &s)
{
    dbg(s, " VisGroup(\n");
    VisGroup visgroup{};
    visgroup.name = readString(s, 128);
    visgroup.color = readColor(s);
    readByte(s);
    visgroup.index = readInt(s);
    visgroup.visible = (readByte(s) != 0);
    readBytes(s, 3);
    dbg(s, " VisGroup)\n");
    return visgroup;
}

Vector readVector(std::istream &s)
{
    dbg(s, " Vector(\n");
    Vector vector{};
    vector.x = readFloat(s);
    vector.y = readFloat(s);
    vector.z = readFloat(s);
    dbg(s, " Vector)\n");
    return vector;
}

Face readFace(std::istream &s)
{
    dbg(s, " Face(\n");
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
    dbg(s, " Face)\n");
    return face;
}

auto readSolid(std::istream &s, bool withheader=true)
{
    dbg(s, " Solid(\n");
    Solid solid{};
    if (withheader)
    {
        auto type = readNString(s);
        rmfloadassert(
            s,
            type == "CMapSolid",
            "Expected CMapSolid, got '" + type + "'");
    }
    solid.visgroup_index = readInt(s);
    solid.color = readColor(s);
    readBytes(s, 4);
    auto face_count = readInt(s);
    for (int32_t i = 0; i < face_count; ++i)
        solid.faces.push_back(readFace(s));
    dbg(s, " Solid)\n");
    return solid;
}

auto readEntity(std::istream &s, bool withheader=true)
{
    dbg(s, " Entity(\n");
    Entity entity{};
    if (withheader)
    {
        auto type = readNString(s);
        rmfloadassert(
            s,
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
    dbg(s, " Entity)\n");
    return entity;
}

void readObject(std::istream &s, Group &group);

auto readGroup(std::istream &s, bool withheader=true)
{
    dbg(s, " Group(\n");
    Group group{};
    if (withheader)
    {
        auto type = readNString(s);
        rmfloadassert(
            s,
            type == "CMapGroup",
            "expected CMapGroup, got '" + type + "'");
    }
    group.visgroup_index = readInt(s);
    group.color = readColor(s);
    auto object_count = readInt(s);
    for (int32_t i = 0; i < object_count; ++i)
        readObject(s, group);
    dbg(s, " Group)\n");
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
        throw RMF::LoadError{s.tellg(), "Invalid Object type '" + type + "'"};
}

auto readCorner(std::istream &s)
{
    dbg(s, " Corner(\n");
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
    dbg(s, " Corner)\n");
    return corner;
}

auto readPath(std::istream &s)
{
    dbg(s, " Path(\n");
    Path path{};
    path.name = readString(s, 128);
    path.class_ = readString(s, 128);
    path.type = readInt(s);
    auto corner_count = readInt(s);
    for (int32_t i = 0; i < corner_count; ++i)
        path.corners.push_back(readCorner(s));
    dbg(s, " Path)\n");
    return path;
}

auto readCamera(std::istream &s)
{
    dbg(s, " Camera(\n");
    Camera camera{};
    camera.eye = readVector(s);
    camera.look = readVector(s);
    dbg(s, " Camera)\n");
    return camera;
}


RichMap RMF::load(std::string const &path)
{
#if RMFENABLEDEBUG
    dbgstream.open("out.txt");
#endif

    std::ifstream s{path, std::ios::in | std::ios::binary};
    s.exceptions(std::ios_base::failbit | std::ios_base::eofbit);
    RichMap map{};

    map.version = readFloat(s);
    if (map.version != 2.2f)
    {
        std::cerr << "WARNING: Possibly unsupported RMF version "
            << map.version << "!\n";
    }

    char rmf[3];
    s.read(rmf, 3);
    rmfloadassert(
        s,
        rmf[0] == 'R' && rmf[1] == 'M' && rmf[2] == 'F',
        "Missing RMF identifier");

    auto visgroup_count = readInt(s);
    for (int32_t i = 0; i < visgroup_count; ++i)
        map.visgroups.push_back(readVisGroup(s));

    auto cmapworld = readNString(s);
    rmfloadassert(
        s,
        cmapworld == "CMapWorld",
        "Expected CMapWorld, got '" + cmapworld + "'");
    readBytes(s, 7);

    auto object_count = readInt(s);
    for (int32_t i = 0; i < object_count; ++i)
        readObject(s, map.objects);

    map.worldspawn_name = readNString(s);
    readBytes(s, 4);

    auto worldspawn_flags = readInt(s); // unused?
    auto worldspawn_kv_count = readInt(s);
    for (int32_t i = 0; i < worldspawn_kv_count; ++i)
    {
        auto key = readNString(s);
        auto value = readNString(s);
        map.worldspawn_properties[key] = value;
    }
    readBytes(s, 12);
    auto path_count = readInt(s);
    for (int32_t i = 0; i < path_count; ++i)
        map.paths.push_back(readPath(s));
    auto docinfo = readString(s, 8);
    rmfloadassert(
        s,
        docinfo == "DOCINFO",
        "Expected DOCINFO, got '" + docinfo + "'");
    readFloat(s);
    map.active_camera = readInt(s);
    auto camera_count = readInt(s);
    for (int32_t i = 0; i < camera_count; ++i)
        map.cameras.push_back(readCamera(s));

    return map;
}
