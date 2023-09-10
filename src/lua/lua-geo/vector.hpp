/**
 * vector.hpp - LuaGeo vectors.
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

#ifndef SE_LUAGEO_VECTOR_HPP
#define SE_LUAGEO_VECTOR_HPP

#include <se-lua/se-lua.hpp>

#include <glm/glm.hpp>

#include <array>


// Metatable names.
template<class V> static constexpr char const *vector_module;
template<> constexpr char const *vector_module<glm::vec2> = "geo.vec2";
template<> constexpr char const *vector_module<glm::vec3> = "geo.vec3";
template<> constexpr char const *vector_module<glm::vec4> = "geo.vec4";

// Names for vector fields.
static constexpr std::array<char const *, 4> XYZW{"x", "y", "z", "w"};


template<class V>
V lgeo_tovector(lua_State *L, int idx)
{
    constexpr auto S = V::length();
    static_assert(1 <= S && S <= XYZW.size());

    int const I = lua_absindex(L, idx);
    if (!(lua_istable(L, I) || lua_isuserdata(L, I)))
        throw Lua::Error{"expected table or userdata"};

    // If the value has an 'x' field, interpret it as a table-style vector.
    int const xT = lua_getfield(L, I, XYZW.at(0));
    if (xT != LUA_TNIL)
    {
        for (typename V::length_type d = 1; d < S; ++d)
            lua_getfield(L, I, XYZW.at(d));
    }
    // Otherwise, interpret as a list-style vector.
    else
    {
        lua_pop(L, 1);
        for (typename V::length_type d = 0; d < S; ++d)
            lua_geti(L, I, static_cast<lua_Integer>(d + 1));
    }

    V vector{};
    for (typename V::length_type d = 0; d < S; ++d)
    {
        int success = 0;
        vector[d] = lua_tonumberx(L, -(S - d), &success);
        if (success == 0)
            throw Lua::Error{"value is not a vector-like object"};
    }
    lua_pop(L, S);

    return vector;
}


template<class V>
static void lgeo_checkvectorfast(lua_State *L, int arg)
{
    luaL_checkudata(L, arg, vector_module<V>);
}


template<class V>
V lgeo_checkvector(lua_State *L, int arg)
{
    lgeo_checkvectorfast<V>(L, arg);
    return lgeo_tovector<V>(L, arg);
}


template<class V>
static void _make_vector(lua_State *L, V const &vector)
{
    constexpr auto S = V::length();
    static_assert(1 <= S && S <= XYZW.size());
    lua_newuserdatauv(L, 0, 1);
    luaL_setmetatable(L, vector_module<V>);
    lua_newtable(L);
    for (typename V::length_type i = 0; i < S; ++i)
        Lua::set_table(L, XYZW.at(i), static_cast<lua_Number>(vector[i]));
    lua_setiuservalue(L, -2, 1);
}


template<> void Lua::push(lua_State *L, glm::vec2 vector);
template<> void Lua::push(lua_State *L, glm::vec3 vector);
template<> void Lua::push(lua_State *L, glm::vec4 vector);


template<> glm::vec2 Lua::get_as(lua_State *L, int idx);
template<> glm::vec3 Lua::get_as(lua_State *L, int idx);
template<> glm::vec4 Lua::get_as(lua_State *L, int idx);


////////////////////////////////////////////////////////////////////////////////
// Metamethods
template<class V>
static int vector_add(lua_State *L)
{
    Lua::push(L, lgeo_checkvector<V>(L, 1) + lgeo_checkvector<V>(L, 2));
    return 1;
}


template<class V>
static int vector_sub(lua_State *L)
{
    Lua::push(L, lgeo_checkvector<V>(L, 1) - lgeo_checkvector<V>(L, 2));
    return 1;
}


template<class V>
static int vector_mul(lua_State *L)
{
    if (lua_isnumber(L, 1))
    {
        auto const n = static_cast<typename V::value_type>(lua_tonumber(L, 1));
        auto const v = lgeo_checkvector<V>(L, 2);
        Lua::push(L, n * v);
    }
    else if (lua_isnumber(L, 2))
    {
        auto const v = lgeo_checkvector<V>(L, 1);
        auto const n = static_cast<typename V::value_type>(lua_tonumber(L, 2));
        Lua::push(L, v * n);
    }
    else
        Lua::push(L, lgeo_checkvector<V>(L, 1) * lgeo_checkvector<V>(L, 2));
    return 1;
}


template<class V>
static int vector_div(lua_State *L)
{
    if (lua_isnumber(L, 1))
    {
        auto const n = static_cast<typename V::value_type>(lua_tonumber(L, 1));
        auto const v = lgeo_checkvector<V>(L, 2);
        Lua::push(L, n / v);
    }
    else if (lua_isnumber(L, 2))
    {
        auto const v = lgeo_checkvector<V>(L, 1);
        auto const n = static_cast<typename V::value_type>(lua_tonumber(L, 2));
        Lua::push(L, v / n);
    }
    else
        Lua::push(L, lgeo_checkvector<V>(L, 1) / lgeo_checkvector<V>(L, 2));
    return 1;
}


template<class V>
static int vector_negate(lua_State *L)
{
    Lua::push(L, -lgeo_checkvector<V>(L, 1));
    return 1;
}


template<class V>
static int vector_index(lua_State *L)
{
    lgeo_checkvectorfast<V>(L, 1);
    lua_getiuservalue(L, 1, 1);
    lua_rotate(L, -2, 1);
    lua_rawget(L, -2);
    return 1;
}


template<class V>
static int vector_newindex(lua_State *L)
{
    lgeo_checkvectorfast<V>(L, 1);
    lua_getiuservalue(L, 1, 1);
    lua_rotate(L, -3, 1);
    lua_rawset(L, -3);
    return 0;
}


template<class V> static int vector_tostring(lua_State *L);

template<>
int vector_tostring<glm::vec2>(lua_State *L)
{
    auto v = lgeo_checkvector<glm::vec2>(L, 1);
    lua_pushfstring(L, "<vector %f %f>", v.x, v.y);
    return 1;
}

template<>
int vector_tostring<glm::vec3>(lua_State *L)
{
    auto v = lgeo_checkvector<glm::vec3>(L, 1);
    lua_pushfstring(L, "<vector %f %f %f>", v.x, v.y, v.z);
    return 1;
}

template<>
int vector_tostring<glm::vec4>(lua_State *L)
{
    auto v = lgeo_checkvector<glm::vec4>(L, 1);
    lua_pushfstring(L, "<vector %f %f %f %f>", v.x, v.y, v.z, v.w);
    return 1;
}


template<class V>
static constexpr luaL_Reg _vector_methods[] = {
    {"__add", vector_add<V>},
    {"__sub", vector_sub<V>},
    {"__mul", vector_mul<V>},
    {"__div", vector_div<V>},
    {"__unm", vector_negate<V>},
    {"__index", vector_index<V>},
    {"__newindex", vector_newindex<V>},

    {"__tostring", vector_tostring<V>},
    {NULL, NULL}
};



////////////////////////////////////////////////////////////////////////////////
// Functions
template<class V>
int lgeo_vector_new(lua_State *L)
{
    switch (lua_gettop(L))
    {
    case 0:
        Lua::push(L, V{});
        break;
    case 1:
        try {
            Lua::push(L, lgeo_tovector<V>(L, 1));
        }
        catch (Lua::Error const &e) {
            luaL_error(L, "%s", e.what());
        }
        break;
    case V::length():{
        V vector{};
        for (auto i = 0; i < V::length(); ++i)
            vector[i] = luaL_checknumber(L, 1 + i);
        Lua::push(L, vector);
        break;}
    default:
        luaL_error(L, "incorrect number of arguments");
        return 0;
    }
    return 1;
}


template<class V>
static int fn_vector_map(lua_State *L)
{
    // 1 is a Callable.
    auto v = lgeo_checkvector<V>(L, 2);
    for (typename V::length_type i = 0; i < V::length(); ++i)
    {
        lua_pushvalue(L, 1);
        Lua::push(L, static_cast<lua_Number>(v[i]));
        lua_call(L, 1, 1);
        v[i] = static_cast<typename V::value_type>(lua_tonumber(L, -1));
        lua_pop(L, 1);
    }
    Lua::push(L, v);
    return 1;
}


template<class V>
static int fn_vector_length(lua_State *L)
{
    auto const v = lgeo_checkvector<V>(L, 1);
    Lua::push(L, static_cast<lua_Number>(glm::length(v)));
    return 1;
}


template<class V>
static constexpr luaL_Reg _vector_functions[] = {
    {"new", lgeo_vector_new<V>},
    {"map", fn_vector_map<V>},
    {"length", fn_vector_length<V>},
    {NULL, NULL}
};



template<class V>
int luaopen_geo_vector(lua_State *L)
{
    luaL_newmetatable(L, vector_module<V>);
    luaL_setfuncs(L, _vector_methods<V>, 0);
    luaL_newlib(L, _vector_functions<V>);
    return 1;
}

#endif
