/**
Copyright (C) 2010 Chriddo

This program is free software;
you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program;
if not, see <http://www.gnu.org/licenses/>.
**/

#ifndef CUBO_UTILS_H_G
#define CUBO_UTILS_H_G

#include <vector>
#include <iostream>
#include <string>
#include "vectors.hpp"

using namespace std;

extern std::string IntToString(const int& t);
extern bool BeginsWith(const string &str, const string &with);
extern void Tokenize(const string& str,
                      vector<string>& tokens,
                      const string& delimiters = " ");

extern void TokenizeFull(const string& str,
                      vector<string>& tokens,
                      const string& delimiters = " ");
extern void TrimSpaces( string& str);



#endif

