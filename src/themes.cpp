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

#include "themes.hpp"
#include <vector>
#include <string>
#include <iostream>
#include <cstdio>
#include "globals.hpp"
#include "luautils.hpp"
#include "game.hpp"

std::string Theme::activeTheme;
std::vector<std::string> Theme::themedirs;

std::string Theme::GetName()
	{
	return activeTheme;
	}


std::string Theme::FileName(std::string dir,std::string fname,std::string ext)
	{
//Parsing all sub dirs... If no file is found return the normal dirname

	for (unsigned int i=0; i<themedirs.size(); i++)
			{
			std::string testname=dir+"/"+themedirs[i]+"/"+fname+ext;
			CuboFile *finfo=GetCuboFileFromRelativeName(testname);
			if (finfo) {
					delete finfo;
					return testname;
					}
			}
	return dir+"/"+fname+ext;
	}

void Theme::Clear()
	{
	themedirs.clear();
	}
void Theme::AddDir(std::string subdir)
	{
	themedirs.push_back(subdir);
	}

void Theme::Load(std::string n)
	{
	if (n!=activeTheme)
			{
			//Make sure to clear everything
			g_Game()->FreeMedia();
			}
	activeTheme=n;

	LuaAccess lua;
	CuboFile *finfo=GetFileName(n,FILE_THEMEDEF,".themedef");
	if (!finfo) {coutlog("ERROR: Cannot load theme "+n,2); return;}
	lua.Include(g_CuboLib());
	lua.LoadFile(finfo,FILE_THEMEDEF,-1);
	delete finfo;
	}




/////////////////LUA IMPLEMENT/////////////////7


int THEME_Clear([[maybe_unused]] lua_State *state)
	{
	Theme::Clear();
	return 0;
	}
int THEME_AddDir([[maybe_unused]] lua_State *state)
	{
	std::string s=LUA_GET_STRING(state);
	Theme::AddDir(s);
	return 0;
	}
int THEME_Load([[maybe_unused]] lua_State *state)
	{
	std::string s=LUA_GET_STRING(state);
	Theme::Load(s);
	return 0;
	}

int THEME_GetName([[maybe_unused]] lua_State *state)
	{
	std::string s=Theme::GetName();
	LUA_SET_STRING(state, s);
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
