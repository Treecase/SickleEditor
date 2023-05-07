/**
 * MAPScanner.hpp - Flex .map scanner.
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

#ifndef MAP_SCANNER_HPP
#define MAP_SCANNER_HPP

#include "location.hh"
#include "MAPParser.hpp"

#ifndef yyFlexLexerOnce
#undef yyFlexLexer
#define yyFlexLexer mapFlexLexer
#include <FlexLexer.h>
#endif

#undef YY_DECL
#define YY_DECL int MAP::MAPScanner::next_token(MAPParser::semantic_type *yylval, MAPParser::location_type *yylloc)


namespace MAP
{
    class MAPScanner : public yyFlexLexer
    {
    public:
        MAPScanner(std::istream *in): yyFlexLexer{in} {}
        int next_token(
            MAPParser::semantic_type *yylval,
            MAPParser::location_type *yylloc);
    };
}

#endif
