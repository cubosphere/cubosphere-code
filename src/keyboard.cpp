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

#include"keyboard.hpp"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
//#include <GL/glut.h>
#endif

#include "glew.hpp"
#include <SDL2/SDL.h>

#include <vector>
#include"cuboutils.hpp"

#include "lua.hpp"

#include"luautils.hpp"
#include"game.hpp"

SDL_Scancode Keyboard::GetKeyConstFor(std::string keyname) {
	return SDL_GetScancodeFromName(keyname.c_str());
	}


void Keyboard::Init() {
//Create the keys
	rep_start_time=0;
	rep_rep_time=0;
	keydown = SDL_GetKeyboardState(&keynum);
	downbefore.resize(keynum,0);
	pressedtime.resize(keynum,0);
	}

void Keyboard::HandleKeys() {
	double el=g_Game()->GetElapsed();
	for (int i=0; i<keynum; i++) {
			if ((keydown[i]) || (downbefore[i])) {
					bool toggle=((keydown[i]) && !(downbefore[i]));

					if (rep_start_time>0) {
							if (pressedtime[i]>rep_start_time) {
									if (i!=SDLK_RSHIFT && i!=SDLK_LSHIFT && i!=SDLK_LCTRL && i!=SDLK_RSHIFT && i!=SDLK_RCTRL && i!=SDLK_LALT && i!=SDLK_RALT && i!=SDLK_CAPSLOCK && i!=SDLK_NUMLOCKCLEAR)
										while (pressedtime[i]>rep_start_time+rep_rep_time) { toggle=1; pressedtime[i]-=rep_rep_time;}

									}
							}

					if (keydown[i])  {pressedtime[i]+=el; }
					else { pressedtime[i]=0; }

					handler(i,(keydown[i]),toggle);
					}

			//Reset everything
			downbefore[i]=keydown[i];

			}
	}

void Keyboard::StartTextInput() {
	//SDL_EnableUNICODE(SDL_ENABLE);
	SDL_StartTextInput();
	}

void Keyboard::StopTextInput() {
	//SDL_EnableUNICODE(SDL_DISABLE);
	SDL_StopTextInput();
	}

void Keyboard::DispatchEvent(SDL_Event *ev) {
	if (ev->type==SDL_TEXTINPUT) {
			if (!textHandler) { return; }
			auto str = std::string(ev->text.text);
			textHandler(str);
			}
	}

std::string Keyboard::GetKeyName(SDL_Scancode key) {
	return SDL_GetScancodeName(key);
	}




////////////LUA-IMPLEMENT////////////


int KEYB_GetKeyConst(lua_State *state) {
	std::string k=LUA_GET_STRING(state);
	SDL_Keycode res = g_Game()->GetKeyboard()->GetKeyConstFor(k);
	LUA_SET_NUMBER(state, res);
	return 1;
	}

int KEYB_GetKeyName(lua_State *state) {
	int k=LUA_GET_INT(state);
	std::string s = g_Game()->GetKeyboard()->GetKeyName((SDL_Scancode)k);
	LUA_SET_STRING(state, s);
	return 1;
	}


void LUA_KEYB_RegisterLib() {
	g_CuboLib()->AddFunc("KEYB_GetKeyConst",KEYB_GetKeyConst);
	g_CuboLib()->AddFunc("KEYB_GetKeyName",KEYB_GetKeyName);
	}
