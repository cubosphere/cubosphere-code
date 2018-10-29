/**
Copyright (C) 2010 Chriddo

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

using namespace std;




std::string IntToString(const int& t)
{
std::stringstream ss;
ss << t;
return ss.str();
}


inline std::string TrimStr(const std::string& Src, const std::string& c = " \r\n\t")
{
int p2 = Src.find_last_not_of(c);
if (p2 == (int)std::string::npos) return std::string();
int p1 = Src.find_first_not_of(c);
if (p1 == (int)std::string::npos) p1 = 0;
return Src.substr(p1, (p2-p1)+1);
}

void TrimSpaces( string& str)
{
  str=TrimStr(str);
}


void Tokenize(const string& str,
                      vector<string>& tokens,
                      const string& delimiters)
{
    // Skip delimiters at beginning.
    string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    string::size_type pos     = str.find_first_of(delimiters, lastPos);

    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

void TokenizeFull(const string& str,
                      vector<string>& tokens,
                      const string& delimiters)
{
    // Skip delimiters at beginning.
    string::size_type lastPos = 0;
    // Find first "non-delimiter".
     string::size_type pos     = str.find_first_of(delimiters, lastPos);
    while (string::npos != pos || string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}



bool BeginsWith(const string& str,const string& with)
{
 string t(str,0,with.length());
 return (t==with);
}

