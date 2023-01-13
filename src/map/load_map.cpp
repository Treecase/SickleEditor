/**
 * load_map.cpp - Load .map files.
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

#include "map/map.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>


/** Add counterclockwise sorted vertices to BRUSH. */
void brush_add_vertices(MAP::Brush &hbrush); // defined in vertexmap.cpp


enum TokenType
{
    NONE,
    KEY,
    VALUE,
    MIPTEX,
    NUMBER,
    LPAREN = '(',
    RPAREN = ')',
    LBRACKET = '[',
    RBRACKET = ']',
    LBRACE = '{',
    RBRACE = '}',
};

/** An Entities lump token. */
struct Token
{
    TokenType type;
    std::string text;
};


/** .MAP tokenizer. */
class MAPTokenizer
{
private:
    struct TokenizeError : std::runtime_error
    {
        TokenizeError(std::string const &what)
        :   std::runtime_error{what}
        {
        }
    };

    struct IOWrapper
    {
    private:
        std::istream &_stream;
        size_t _line;
        size_t _column;

    public:
        IOWrapper(std::istream &stream)
        :   _stream{stream}
        ,   _line{1}
        ,   _column{1}
        {
        }

        auto line() { return _line; }
        auto column() { return _column; }

        auto peek() { return _stream.peek(); }
        auto get()
        {
            auto ch = _stream.get();
            if (ch == '\n')
            {
                _line++;
                _column = 1;
            }
            else
                _column++;
            return ch;
        }
        template<typename T>
        void operator>>(T &other)
        {
            auto a = _stream.tellg();
            _stream >> other;
            auto n = _stream.tellg() - a;
            _column += n;
        }
    };

    // Output buffer.
    std::vector<Token> _tokens;
    // Input buffer.
    IOWrapper _io;

    /* ===[ Tokenizer Errors ]=== */
    /** Unexpected char (One possibility). */
    [[noreturn]] void e_Unexpected(char expected, char ch)
    {
        e_Unexpected(std::string{expected}, ch);
    }

    /** Unexpected char (multiple possibilities). */
    [[noreturn]] void e_Unexpected(std::string const &expected, char ch)
    {
        std::stringstream msg{};
        msg << _io.line() << ":" << _io.column() << " ";
        msg << "Expected '" << expected[0] << "'";
        for (size_t i = 1; i < expected.size(); ++i)
            msg << " or '" << expected[i] << "'";
        msg << ", got '" << ch << "'";
        throw TokenizeError{msg.str()};
    }


    /* ===[ Whitespace ]=== */
    /** Skip over whitespace. */
    void s_Whitespace()
    {
        while (isspace(_io.peek()))
            _io.get();
    }

    /** Skip over whitespace, throwing an error if there isn't any. */
    void s_RequiredWhitespace()
    {
        if (!isspace(_io.peek()))
            throw TokenizeError{"Expected whitespace"};
        while (isspace(_io.peek()))
            _io.get();
    }


    /* ===[ Terminals ]=== */
    /** Single-character tokens (brackets, etc.). */
    void t_Char(char expect)
    {
        int ch = _io.get();
        if (ch != expect)
            e_Unexpected(expect, ch);
        _tokens.push_back({(TokenType)expect, std::string{(char)ch}});
    }

    /** String */
    void t_String(TokenType type)
    {
        s_Whitespace();
        // Start with "
        int ch;
        if ((ch = _io.get()) != '"')
            e_Unexpected('"', ch);
        // Read until "
        std::string token{};
        while ((ch = _io.get()) != EOF)
        {
            if (ch == '"')
                break;
            token += ch;
        }
        // End with "
        if (ch != '"')
            e_Unexpected('"', ch);
        _tokens.push_back({type, token});
    }

    /** MipTex: .* */
    void t_MipTex()
    {
        s_Whitespace();
        std::string token{};
        while (!isspace(_io.peek()))
            token += _io.get();
        _tokens.push_back({MIPTEX, token});
    }

    /** Number */
    void t_Number()
    {
        s_Whitespace();
        float number;
        std::string buf{};
        _io >> number;
        _tokens.push_back({NUMBER, std::to_string(number)});
    }


    /* ===[ Symbols ]=== */
    /** '{' (PROPERTY | BRUSH)* '}' */
    void s_Entity()
    {
        s_Whitespace();
        t_Char('{');
        // Some number of properties or brushes, ending with }
        for(;;)
        {
            s_Whitespace();
            int ch = _io.peek();
            switch (ch)
            {
            case '"':
                s_Property();
                break;
            case '{':
                s_Brush();
                break;
            case '}':
                goto end_of_entity;
            default:
                e_Unexpected("\"{}", ch);
            }
        }
    end_of_entity:
        t_Char('}');
        s_Whitespace();
    }

    /** KEY VALUE */
    void s_Property()
    {
        s_Whitespace();
        t_String(KEY);
        s_Whitespace();
        t_String(VALUE);
    }

    /** '{' PLANE PLANE PLANE PLANE+ '}' */
    void s_Brush()
    {
        s_Whitespace();
        t_Char('{');
        s_Plane();
        s_Plane();
        s_Plane();
        s_Plane();
        // 0 or more planes, bookended by }.
        s_Whitespace();
        for(; _io.peek() != '}';)
        {
            s_Plane();
            s_Whitespace();
        }
        t_Char('}');
    }

    /** POINT POINT POINT MIPTEX OFFX OFFY ROTATION SCALEX SCALEY */
    void s_Plane()
    {
        s_Whitespace();
        s_Point();
        s_Point();
        s_Point();
        s_RequiredWhitespace();
        t_MipTex();
        s_RequiredWhitespace();
        s_Off();
        s_Off();
        t_Number(); // Rotation
        s_RequiredWhitespace();
        t_Number(); // ScaleX
        s_RequiredWhitespace();
        t_Number(); // ScaleY
    }

    /** '(' NUMBER NUMBER NUMBER ')' */
    void s_Point()
    {
        s_Whitespace();
        t_Char('(');
        t_Number();
        s_RequiredWhitespace();
        t_Number();
        s_RequiredWhitespace();
        t_Number();
        s_Whitespace();
        t_Char(')');
    }

    /** OFFX and OFFY */
    void s_Off()
    {
        s_Whitespace();
        t_Char('[');
        t_Number();
        s_RequiredWhitespace();
        t_Number();
        s_RequiredWhitespace();
        t_Number();
        s_RequiredWhitespace();
        t_Number();
        s_Whitespace();
        t_Char(']');
    }

public:
    MAPTokenizer(std::istream &f)
    :   _tokens{}
    ,   _io{f}
    {
        while (!f.eof())
            s_Entity();
    }

    /** Get the resulting tokens. */
    auto tokens() const { return _tokens; }
};

/** .MAP parser. */
class MAPParser
{
private:
    struct ParseError : std::runtime_error
    {
        ParseError(std::string const &what)
        :   std::runtime_error{what}
        {
        }
    };

    size_t _i;
    std::vector<Token> const &_tokens;
    MAP::Map _result;

    Token const &_next()
    {
        return _tokens.at(_i++);
    }
    void _unget()
    {
        _i--;
    }
    Token const &_peek() const
    {
        return _tokens.at(_i);
    }


    MAP::Entity _Entity()
    {
        MAP::Entity entity{};
        auto t = _next();
        if (t.type != '{')
            throw ParseError{"Expected LBRACE"};
        while ((t = _peek()).type != '}')
        {
            if (t.type == '{')
                entity.brushes.push_back(_Brush());
            else if (t.type == KEY)
                entity.properties.insert(_Property());
            else
                throw ParseError{"Expected BRUSH or PROPERTY"};
        }
        if (_next().type != '}')
            throw ParseError{"Expected RBRACE"};
        return entity;
    }

    std::pair<std::string, std::string> _Property()
    {
        return {_Key(), _Value()};
    }

    std::string _Key()
    {
        auto key = _next();
        if (key.type != KEY)
            throw ParseError{"Expected KEY"};
        return key.text;
    }

    std::string _Value()
    {
        auto value = _next();
        if (value.type != VALUE)
            throw ParseError{"Expected VALUE"};
        return value.text;
    }

    MAP::Brush _Brush()
    {
        if (_next().type != '{')
            throw ParseError{"Expected LBRACE"};
        MAP::Brush brush{};
        while (_peek().type == '(')
            brush.planes.push_back(_Plane());
        brush_add_vertices(brush);
        auto t = _next();
        if (t.type != '}')
            throw ParseError{"Expected RBRACE"};
        return brush;
    }

    MAP::Plane _Plane()
    {
        MAP::Plane plane{};
        plane.a = _Point();
        plane.b = _Point();
        plane.c = _Point();
        plane.miptex = _MipTex();
        auto const offx = _Off();
        auto const offy = _Off();
        plane.s = {offx[0], offx[1], offx[2]};
        plane.t = {offy[0], offy[1], offy[2]};
        plane.offsets = {offx[3], offy[3]};
        plane.rotation = _Number();
        plane.scale = {_Number(), _Number()};
        return plane;
    }

    MAP::Vertex _Point()
    {
        if (_next().type != '(')
            throw ParseError{"Expected LPAREN"};
        MAP::Vertex pt{_Number(), _Number(), _Number()};
        if (_next().type != ')')
            throw ParseError{"Expected RPAREN"};
        return pt;
    }

    float _Number()
    {
        auto const &n = _next();
        if (n.type != NUMBER)
            throw ParseError{"Expected NUMBER"};
        return std::stof(n.text);
    }

    std::string _MipTex()
    {
        auto &t = _next();
        if (t.type != MIPTEX)
            throw ParseError{"Expected MIPTEX"};
        return t.text;
    }

    std::array<float, 4> _Off()
    {
        if (_next().type != '[')
            throw ParseError{"Expected LBRACKET"};
        std::array<float, 4> off{_Number(), _Number(), _Number(), _Number()};
        if (_next().type != ']')
            throw ParseError{"Expected RBRACKET"};
        return off;
    }

public:
    MAPParser(std::vector<Token> const &tokens)
    :   _i{0}
    ,   _tokens{tokens}
    ,   _result{}
    {
        while (_i < _tokens.size())
            _result.entities.push_back(_Entity());
    }

    auto map() const { return _result; }
};


MAP::Map MAP::load(std::string const &path)
{
    std::ifstream f{path, std::ios::in | std::ios::binary};
    if (!f.is_open())
        throw std::runtime_error{"Failed to open '" + path + "'"};

    // Reset tokenizer state.
    MAPTokenizer tokenizer{f};
    MAPParser parser{tokenizer.tokens()};
    return parser.map();
}
