/**
 * fgd.cpp - Load .fgd files.
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

#include "fgd.hpp"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <iostream>


enum TokenType
{
    NONE,
    // ClassDef
    CLASSTYPE,
    CLASSPROPERTY,
    CLASSPROPERTYARG,
    CLASSNAME,
    DESCRIPTION,
    // Field
    FIELDNAME,
    FIELDTYPE,
    DEFAULTVALUE,
    // Choices
    CHOICES,
    CHOICEVALUE,
    // Flags
    FLAGS,
    FLAGVALUE,
    FLAGINIT,
};

/** An Entities lump token. */
struct Token
{
    TokenType type;
    std::string text;
};


/** .fgd tokenizer. */
class FGDTokenizer
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
        ,   _column{0}
        {
        }

        auto line() const { return _line; }
        auto column() const { return _column; }

        auto peek() const { return _stream.peek(); }
        auto get()
        {
            auto ch = _stream.get();
            if (ch == EOF)
                throw std::ios::failure("eof");
            if (ch == '\n')
            {
                _line++;
                _column = 0;
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
        msg << _io.line() << ":" << _io.column()-1 << " ";
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
        char ch = _io.peek();
        while (ch == '/' || isspace(ch))
        {
            // Comment
            if (ch == '/')
            {
                _io.get();
                if (_io.peek() == '/')
                    while (_io.peek() != '\n')
                        ch = _io.get();
            }
            // Whitespace
            else
                while (isspace(_io.peek()))
                    ch = _io.get();
            ch = _io.peek();
        }
    }

    /** Skip over whitespace, throwing an error if there isn't any. */
    void s_RequiredWhitespace()
    {
        if (!isspace(_io.peek()))
            throw TokenizeError{"Expected whitespace"};
        s_Whitespace();
    }


    /* ===[ Terminals ]=== */
    /** . */
    void t_Char(char expect)
    {
        int ch = _io.get();
        if (ch != expect)
            e_Unexpected(expect, ch);
    }

    /** " .* " */
    void t_String(TokenType type)
    {
        t_Char('"');
        std::string token{};
        while (_io.peek() != '"')
            token += _io.get();
        t_Char('"');
        _tokens.push_back({type, token});
    }

    /** [A-Za-z0-9_]+ */
    void t_Identifier(TokenType type)
    {
        std::string token{};
        char ch = _io.peek();
        while (ch == '_' || (isalnum(ch) && !isspace(ch)))
        {
            token += _io.get();
            ch = _io.peek();
        }
        if (token.size() == 0)
            TokenizeError("Expected IDENTIFIER");
        _tokens.push_back({type, token});
    }

    /** Integer */
    void t_Integer(TokenType type)
    {
        int number;
        std::string buf{};
        _io >> number;
        _tokens.push_back({type, std::to_string(number)});
    }


    /* ===[ Symbols ]=== */
    /** '@' ClassType [ClassProperty]* '=' ClassName [':' Description] '[' Field* ']' */
    void s_ClassDef()
    {
        s_Whitespace(); t_Char('@'); s_ClassType();
        bool whitespace = true;
        try
        {
            s_RequiredWhitespace();
        }
        catch (TokenizeError const &)
        {
            whitespace = false;
        }
        if (whitespace)
        {
            while (_io.peek() != '=')
            {
                s_ClassProperty();
                s_Whitespace();
            }
        }
        s_Whitespace(); t_Char('=');
        s_Whitespace(); t_Identifier(CLASSNAME);
        s_Whitespace();
        if (_io.peek() == ':')
        {
            t_Char(':');
            s_Whitespace(); t_String(DESCRIPTION);
        }
        s_Whitespace(); t_Char('[');
        s_Whitespace();
        while (_io.peek() != ']')
        {
            s_Field();
            s_Whitespace();
        }
        t_Char(']');
    }

    void s_ClassType()
    {
        t_Identifier(CLASSTYPE);
        auto const &last = _tokens.back();
        if (last.text != "SolidClass" && last.text != "BaseClass" && last.text != "PointClass")
            throw TokenizeError("Expected CLASSTYPE");
    }

    void s_ClassProperty()
    {
        t_Identifier(CLASSPROPERTY);
        s_Whitespace(); t_Char('(');
        auto const &last = _tokens.back();
        if (last.text == "base")
        {
            s_Whitespace(); t_Identifier(CLASSPROPERTYARG);
            s_Whitespace();
            while (_io.peek() == ',')
            {
                t_Char(',');
                s_Whitespace(); t_Identifier(CLASSPROPERTYARG);
                s_Whitespace();
            }
        }
        else if (last.text == "color")
        {
            s_Whitespace(); t_Integer(CLASSPROPERTYARG);
            s_Whitespace(); t_Integer(CLASSPROPERTYARG);
            s_Whitespace(); t_Integer(CLASSPROPERTYARG);
        }
        else if (last.text == "decal")
        {
        }
        else if (last.text == "iconsprite")
        {
            s_Whitespace(); t_String(CLASSPROPERTYARG);
        }
        else if (last.text == "size")
        {
            s_Whitespace(); t_Integer(CLASSPROPERTYARG);
            s_Whitespace(); t_Integer(CLASSPROPERTYARG);
            s_Whitespace(); t_Integer(CLASSPROPERTYARG);
            s_Whitespace();
            if (_io.peek() == ',')
            {
                t_Char(',');
                s_Whitespace(); t_Integer(CLASSPROPERTYARG);
                s_Whitespace(); t_Integer(CLASSPROPERTYARG);
                s_Whitespace(); t_Integer(CLASSPROPERTYARG);
            }
        }
        else if (last.text == "sprite")
        {
        }
        else if (last.text == "studio")
        {
            s_Whitespace(); t_String(CLASSPROPERTYARG);
        }
        else
            throw TokenizeError("Expected 'base', 'color', 'decal', 'iconsprite', 'size', 'sprite', or 'studio'.");
        s_Whitespace(); t_Char(')');
    }

    void s_Field()
    {
        t_Identifier(FIELDNAME);
        s_Whitespace(); t_Char('(');
        s_Whitespace(); t_Identifier(FIELDTYPE);
        auto &type = _tokens.back();
        std::transform(type.text.begin(), type.text.end(), type.text.begin(), [](unsigned char c){ return std::tolower(c); });
        auto const t = _tokens.back();
        s_Whitespace(); t_Char(')');
        // Optional description
        s_Whitespace();
        if (_io.peek() == ':')
        {
            t_Char(':');
            s_Whitespace();
            if (_io.peek() != '"')
                s_DefaultValue();
            else
                t_String(DESCRIPTION);
        }
        // Optional default value
        s_Whitespace();
        if (_io.peek() == ':')
        {
            t_Char(':');
            s_Whitespace();
            s_DefaultValue();
        }
        // Choices def
        if (t.text == "choices")
        {
            s_Whitespace(); t_Char('=');
            s_Whitespace(); s_Choices();
        }
        // Flags def
        else if (t.text == "flags")
        {
            s_Whitespace(); t_Char('=');
            s_Whitespace(); s_Flags();
        }
    }

    void s_DefaultValue()
    {
        if (_io.peek() == '"')
            t_String(DEFAULTVALUE);
        else
            t_Integer(DEFAULTVALUE);
    }

    void s_Choices()
    {
        t_Char('[');
        s_Whitespace();
        while (_io.peek() != ']')
        {
            t_Integer(CHOICEVALUE);
            s_Whitespace(); t_Char(':');
            s_Whitespace(); t_String(DESCRIPTION);
            s_Whitespace();
        }
        t_Char(']');
    }

    void s_Flags()
    {
        t_Char('[');
        s_Whitespace();
        while (_io.peek() != ']')
        {
            t_Integer(FLAGVALUE);
            s_Whitespace(); t_Char(':');
            s_Whitespace(); t_String(DESCRIPTION);
            s_Whitespace(); t_Char(':');
            s_Whitespace(); t_Integer(FLAGINIT);
            s_Whitespace();
        }
        t_Char(']');
    }

public:
    FGDTokenizer(std::istream &f)
    :   _tokens{}
    ,   _io{f}
    {
        while (!f.eof())
        {
            try
            {
                s_ClassDef();
            }
            catch (std::ios::failure const &)
            {
                // eof
                break;
            }
        }
    }

    /** Get the resulting tokens. */
    auto tokens() const { return _tokens; }
};

/** .MAP parser. */
class FGDParser
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
    FGD::FGD _result;

    Token const &_next()
    {
        return _tokens.at(_i++);
    }
    void _unget()
    {
        _i--;
    }
    Token _peek() const
    {
        if (_i >= _tokens.size())
            return {NONE, "EOF"};
        else
            return _tokens.at(_i);
    }


    FGD::Class _ClassDef()
    {
        FGD::Class cls{};
        cls.type = _token(CLASSTYPE);
        while (_peek().type == CLASSPROPERTY)
            cls.properties.push_back(_ClassProperty());
        cls.name = _token(CLASSNAME);
        if (_peek().type == DESCRIPTION)
            cls.description = _token(DESCRIPTION);
        while (_peek().type == FIELDNAME)
            cls.fields.push_back(_Field());
        return cls;
    }

    FGD::Property _ClassProperty()
    {
        FGD::Property property{};
        property.name = _token(CLASSPROPERTY);
        while (_peek().type == CLASSPROPERTYARG)
            property.arguments.push_back(_token(CLASSPROPERTYARG));
        return property;
    }

    FGD::Field _Field()
    {
        FGD::Field field{};
        field.name = _token(FIELDNAME);
        field.type = _token(FIELDTYPE);
        if (_peek().type == DESCRIPTION)
            field.description = _token(DESCRIPTION);
        if (_peek().type == DEFAULTVALUE)
            field.defaultvalue = _token(DEFAULTVALUE);
        if (field.type == "choices")
            while (_peek().type == CHOICEVALUE)
                field.choices.push_back(_Choice());
        else if (field.type == "flags")
            while (_peek().type == FLAGVALUE)
                field.flags.push_back(_Flag());
        return field;
    }

    FGD::Choice _Choice()
    {
        return {stoul(_token(CHOICEVALUE)), _token(DESCRIPTION)};
    }

    FGD::Flag _Flag()
    {
        return {
            stoul(_token(FLAGVALUE)),
            _token(DESCRIPTION),
            (bool)stoul(_token(FLAGINIT))};
    }

    std::string _token(TokenType expect)
    {
        auto const &t = _next();
        if (t.type != expect)
            throw ParseError("Unexpected token");
        return t.text;
    }

public:
    FGDParser(std::vector<Token> const &tokens)
    :   _i{0}
    ,   _tokens{tokens}
    ,   _result{}
    {
        while (_i < _tokens.size())
            _result.push_back(_ClassDef());
    }

    auto fgd() const { return _result; }
};


FGD::FGD FGD::load(std::string const &path)
{
    std::ifstream f{path, std::ios::in | std::ios::binary};
    if (!f.is_open())
        throw std::runtime_error{"Failed to open '" + path + "'"};
    FGDTokenizer tokenizer{f};
    FGDParser parser{tokenizer.tokens()};
    return parser.fgd();
}
