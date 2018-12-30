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

#include "filesystem.hpp"
#include "definitions.hpp"

#include <iostream>
#include <vector>
#include "globals.hpp"
#include "cuboutils.hpp"
#include "game.hpp"

#include <GL/glew.h>
#include <SDL.h>

void BaseLuaDef::LoadDef() {
	isloaded=true;
	int typ=GetType();
	std::string ext;
	FileTypeFromString(g_SubDirs[typ],&ext);
	auto fileinfo=GetFileName(name,typ,"."+ext);

	if (!fileinfo) {
			coutlog("Lua script "+name+"."+ext+" not found!",1);
			return;
			}

	fname=fileinfo->GetName();


	lua.LoadUserLibs();
	lua.LoadFile(fileinfo,typ,myid);
// if (lua.FuncExists("Precache")) lua.CallVA("Precache","");
	if (SendIDWhenPrecache()==0) {
			lua.CallVAIfPresent("Precache");
			}
	else if (SendIDWhenPrecache()==1) {
			lua.CallVAIfPresent("Precache", {{myid}});
			}
	//Otherwise no Precache-Call
	}

BaseLuaDef::~BaseLuaDef() {
	lua.CallVAIfPresent("CleanUp");
	}

void BaseLuaDef::Reload() {
	lua.Reset();
	BaseLuaDef::LoadDef();
	}

/////////////////////////////////////

/*void TMenu::Reload()
{
    lua.Reset();
    TBaseLuaDef::LoadDef();
}*/

void  Menu::LoadDef(std::string cname) {
	if (isloaded) {
			nextname=cname;
			change=1;
			}
	else {
			SetName(cname);
			BaseLuaDef::LoadDef();
			}
	}

void Menu::PostThink() {

	if (!change) { return; }

	name=nextname;
	change=0;
	lua.Reset();
	auto finfo=GetFileName(name,FILE_MENUDEF,".mdef");
	if (!finfo) { coutlog("Menudef "+name+" not found!",1); isloaded=false; return;}
	lua.LoadUserLibs();
	lua.LoadFile(finfo,FILE_MENUDEF,-1);
	lua.CallVAIfPresent("Precache");


	isloaded=true;
	}


void Menu::Render() {
	if (!isloaded) { return; }
		lua.CallVAIfPresent("Render");
	}


void Menu::Think() {
	if (!isloaded) { return; }
			lua.CallVAIfPresent("Think");
	}

void Menu::JoyAxisChange(int joys,int axis,double val,double pval) {
	if (!isloaded) { return; }
			lua.CallVAIfPresent("OnJoyAxisChange", {{joys,axis,val,pval}});
	}


void Menu::SendKey(int key,int down, int toggle) {
	if (!isloaded) { return; }
	lua.CallVAIfPresent("OnKeyPressed", {{key,down,toggle}});
	}

void Menu::SendTextInput(std::string& str) {
	if (!isloaded) { return; }
	lua.CallVAIfPresent("OnTextInput", {{str}});
}


void Menu::SendJoyButton(int joy, int button,int dir,int down, int toggle) {
	if (!isloaded) { return; }
			lua.CallVAIfPresent("OnJoyButton", {{joy,button,dir,down,toggle}});
	}



//////////////LUA-IMPLEMENT///////////////////////7


int MENU_Load(lua_State *state) {
	std::string s=LUA_GET_STRING(state);
	g_Game()->GetMenu()->LoadDef(s);
	return 0;
	}
int MENU_Activate(lua_State *state) {
	g_Game()->SetMenuActive(1);
	return 0;
	}
int MENU_Deactivate(lua_State *state) {
	g_Game()->SetMenuActive(0);
	return 0;
	}


void LUA_MENU_RegisterLib() {
	g_CuboLib()->AddFunc("MENU_Load",MENU_Load);
	g_CuboLib()->AddFunc("MENU_Activate",MENU_Activate);
	g_CuboLib()->AddFunc("MENU_Deactivate",MENU_Deactivate);
	}


// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
