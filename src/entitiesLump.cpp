/**
 * entitiesLump.cpp - Parse the .bsp Entities lump.
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

#include "entitiesLump.hpp"

#include <stdexcept>


/** Parse an Entities lump. */
enum TokenType
{
    NONE,
    LPAREN,
    RPAREN,
    KEY,
    VALUE
};

/** An Entities lump token. */
struct Token
{
    TokenType type;
    std::string text;
};

// Tokenizer's current state.
static void (*_state)(char);
// Tokenizer's output.
static std::vector<Token> _tokens;
// Buffer for KEY and VALUE token construction.
static std::string _tokbuf;
// Error string. Empty means no error.
static std::string _error;


void s_Begin(char ch);
void s_ExpectKey(char ch);
void s_Key(char ch);
void s_ExpectValue(char ch);
void s_Value(char ch);
void s_ExpectKeyOrRParen(char ch);


/** Start of a stream/between entity defs. */
void s_Begin(char ch)
{
    if (ch == '{')
    {
        _tokens.push_back({LPAREN, "{"});
        _state = s_ExpectKey;
    }
    else if (isspace(ch))
        ;// ignore whitespace
    else
        _error = "Expected '{', got '" + std::string{ch} + "'";
}

/** Inside an entity def, but before entering a KEY token. */
void s_ExpectKey(char ch)
{
    if (ch == '"')
        _state = s_Key;
    else if (isspace(ch))
        ;// ignore whitespace
    else
        _error = "Expected '\"' (KEY), got '" + std::string{ch} + "'";
}

/** Inside a KEY token. */
void s_Key(char ch)
{
    if (ch == '"')
    {
        _tokens.push_back({KEY, _tokbuf});
        _tokbuf = "";
        _state = s_ExpectValue;
    }
    else
        _tokbuf += ch;
}

/** Between a KEY token and a VALUE token. */
void s_ExpectValue(char ch)
{
    if (ch == '"')
        _state = s_Value;
    else if (isspace(ch))
        ;// ignore whitespace
    else
        _error = "Expected '\"' (VALUE), got '" + std::string{ch} + "'";
}

/** Inside a VALUE token. */
void s_Value(char ch)
{
    if (ch == '"')
    {
        _tokens.push_back({VALUE, _tokbuf});
        _tokbuf = "";
        _state = s_ExpectKeyOrRParen;
    }
    else
        _tokbuf += ch;
}

/** After a VALUE token. */
void s_ExpectKeyOrRParen(char ch)
{
    if (ch == '"')
        _state = s_Key;
    else if (ch == '}')
    {
        _tokens.push_back({RPAREN, "}"});
        _state = s_Begin;
    }
    else if (isspace(ch))
        ;// ignore whitespace
    else
        _error = "Expected '\"' (KEY) or '}', got '" + std::string{ch} + "'";
}

/** Tokenize a .bsp's Entities lump. */
std::vector<Token> tokenize_entities(std::string const &buf)
{
    // Reset state variables.
    _state = s_Begin;
    _tokens = std::vector<Token>{};
    _tokbuf = "";
    _error = "";

    size_t _line_number = 1;
    size_t _column_number = 0;

    for (auto const &ch : buf)
    {
        // Update line/column counters.
        if (ch == '\n')
        {
            _line_number++;
            _column_number = 0;
        }
        else
            _column_number++;

        if (ch == '\0')
        {
            // Throw an error if there's an EOF in the middle of the data.
            if (_state != s_Begin)
                throw std::runtime_error{
                    std::to_string(_line_number)
                    + ":"
                    + std::to_string(_column_number)
                    + " -- Unexpected end-of-file"};
            // Otherwise just stop reading.
            else
                break;
        }

        // Update state machine.
        _state(ch);

        // Throw errors if one occurred.
        if (!_error.empty())
            throw std::runtime_error{
                std::to_string(_line_number)
                + ":"
                + std::to_string(_column_number)
                + " -- "
                + _error};
    }

    return _tokens;
}

/** Parse a .bsp's Entities lump from a Tokens list. */
std::vector<Entity> parse_entities_low(std::vector<Token> const &tokens)
{
    std::vector<Entity> out{};

    size_t i = 0;
    while (i < tokens.size())
    {
        // Search for end of entity def
        size_t e_end = i;
        for (; e_end < tokens.size(); ++e_end)
            if (tokens[e_end].type == RPAREN)
                break;

        // Extract key/value pairs from entity def
        Entity ent{};
        i++;
        for (; i < e_end; i += 2)
            ent[tokens[i].text] = tokens[i+1].text;
        out.push_back(ent);
        i++;
    }
    return out;
}


std::vector<Entity> parse_entities(std::string const &buf)
{
    return parse_entities_low(tokenize_entities(buf));
}
