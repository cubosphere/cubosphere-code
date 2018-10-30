/**
Copyright (C) 2010 Chriddo

This program is free software;
you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program;
if not, see <http://www.gnu.org/licenses/>.
**/


#ifndef THEMES_H_G
#define THEMES_H_G

#include <string>

using namespace std;

extern string ThemeFileName(string dir,string fname,string ext);
extern string GetThemeName();
extern void ThemeClear();
extern void ThemeAddDir(string subdir);
extern void ThemeLoad(string n);

extern void LUA_THEME_RegisterLib();

#endif
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
