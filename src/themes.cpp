/**
Copyright (C) 2010 Chriddo

This program is free software;
you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program;
if not, see <http://www.gnu.org/licenses/>.
**/

#include "themes.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <cstdio>
#include "globals.hpp"
#include "luautils.hpp"
#include "game.hpp"

static std::vector<std::string> themedirs;
static std::string momThemeName="";

std::string GetThemeName()
	{
	return momThemeName;
	}


std::string ThemeFileName(std::string dir,std::string fname,std::string ext)
	{
//Parsing all sub dirs... If no file is found return the normal dirname

	for (unsigned int i=0; i<themedirs.size(); i++)
			{
			std::string testname=dir+"/"+themedirs[i]+"/"+fname+ext;
			TCuboFile *finfo=GetCuboFileFromRelativeName(testname);
			if (finfo) {
					delete finfo;
					return testname;
					}
			}
	return dir+"/"+fname+ext;
	}

void ThemeClear()
	{
	themedirs.clear();
	}
void ThemeAddDir(std::string subdir)
	{
	themedirs.push_back(subdir);
	}

void ThemeLoad(std::string n)
	{
	if (n!=momThemeName)
			{
			//Make sure to clear everythink
			g_Game()->FreeMedia();
			}
	momThemeName=n;

	TLuaAccess lua;
	TCuboFile *finfo=GetFileName(n,FILE_THEMEDEF,".themedef");
	if (!finfo) {coutlog("ERROR: Cannot load theme "+n,2); return;}
	lua.Include(g_CuboLib());
	lua.LoadFile(finfo,FILE_THEMEDEF,-1);
	delete finfo;
	}




/////////////////LUA IMPLEMENT/////////////////7


int THEME_Clear(lua_State *state)
	{
	ThemeClear();
	return 0;
	}
int THEME_AddDir(lua_State *state)
	{
	std::string s=LUA_GET_STRING;
	ThemeAddDir(s);
	return 0;
	}
int THEME_Load(lua_State *state)
	{
	std::string s=LUA_GET_STRING;
	ThemeLoad(s);
	return 0;
	}

int THEME_GetName(lua_State *state)
	{
	std::string s=GetThemeName();
	LUA_SET_STRING(s);
	return 1;
	}

void LUA_THEME_RegisterLib()
	{
	g_CuboLib()->AddFunc("THEME_Clear",THEME_Clear);
	g_CuboLib()->AddFunc("THEME_AddDir",THEME_AddDir);
	g_CuboLib()->AddFunc("THEME_Load",THEME_Load);
	g_CuboLib()->AddFunc("THEME_GetName",THEME_GetName);
	}
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
