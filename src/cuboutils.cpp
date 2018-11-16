/**
Copyright (C) 2010 Chriddo
Copyright (C) 2018 Valeri Ochinski <v19930312@gmail.com>

This program is free software;
you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program;
if not, see <http://www.gnu.org/licenses/>.
**/

#include <stdio.h>
#include <stdlib.h>

#include <cmath>
#include <vector>
#include <iostream>
#include <string>
#include <stdio.h>
#include "cuboutils.hpp"
#include <sstream>
#include "vectors.hpp"

std::string TrimStr(const std::string& Src, const std::string& c = " \r\n\t")
	{
	auto p2 = Src.find_last_not_of(c);
	if (p2 == std::string::npos) return std::string();
	auto p1 = Src.find_first_not_of(c);
	if (p1 == std::string::npos) p1 = 0;
	return Src.substr(p1, (p2-p1)+1);
	}

void TrimSpaces( std::string& str)
	{
	str=TrimStr(str);
	}


void Tokenize(const std::string& str,
		std::vector<std::string>& tokens,
		const std::string& delimiters)
	{
	TokenizeFull(str, tokens, delimiters, str.find_first_not_of(delimiters, 0));
	}

void TokenizeFull(const std::string& str,
		std::vector<std::string>& tokens,
		const std::string& delimiters,
		const int& first)
	{
	// Skip delimiters at beginning.
	auto lastPos = first;
	// Find first "non-delimiter".
	auto pos     = str.find_first_of(delimiters, lastPos);
	while (std::string::npos != pos || std::string::npos != lastPos)
			{
			// Found a token, add it to the vector.
			tokens.push_back(str.substr(lastPos, pos - lastPos));
			// Skip delimiters.  Note the "not_of"
			lastPos = str.find_first_not_of(delimiters, pos);
			// Find next "non-delimiter"
			pos = str.find_first_of(delimiters, lastPos);
			}
	}



bool BeginsWith(const std::string& str,const std::string& with)
	{
	return str.substr(0, with.length()) == with;
	}

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
