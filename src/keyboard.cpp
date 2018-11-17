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

#ifdef WIN32

#ifdef MINGW_CROSS_COMPILE
#include <GL/glew.h>
#else
#include <GL\glew.h>
#endif
// #include <GL\gl.h>
#include <windows.h>
#include <SDL.h>


#else

#include <GL/glew.h>
#include <SDL/SDL.h>

#endif

#include <vector>
#include"cuboutils.hpp"

#include "lua.hpp"

#include"luautils.hpp"
#include"game.hpp"

SDLKey Keyboard::GetKeyConstFor(std::string keyname) {
	if (keyname=="return") { return SDLK_RETURN; }
	else if (keyname=="space") { return SDLK_SPACE; }
	if (keyname=="uparrow") { return SDLK_UP; }
	if (keyname=="downarrow") { return SDLK_DOWN; }
	if (keyname=="leftarrow") { return SDLK_LEFT; }
	if (keyname=="rightarrow") { return SDLK_RIGHT; }
	if (keyname=="esc") { return SDLK_ESCAPE; }

	else if (keyname=="backspace") { return SDLK_BACKSPACE; }
	else if (keyname=="tab") { return SDLK_TAB; }
	else if (keyname=="clear") { return SDLK_CLEAR; }
	else if (keyname=="pause") { return SDLK_PAUSE; }
	else if (keyname=="exclamation mark") { return SDLK_EXCLAIM; }
	else if (keyname=="double quote") { return SDLK_QUOTEDBL; }
	else if (keyname=="hash") { return SDLK_HASH; }
	else if (keyname=="dollar") { return SDLK_DOLLAR; }
	else if (keyname=="ampersand") { return SDLK_AMPERSAND; }
	else if (keyname=="single quote") { return SDLK_QUOTE; }
	else if (keyname=="left parenthesis") { return SDLK_LEFTPAREN; }
	else if (keyname=="right parenthesis") { return SDLK_RIGHTPAREN; }
	else if (keyname=="asterisk") { return SDLK_ASTERISK; }
	else if (keyname=="plus sign") { return SDLK_PLUS; }
	else if (keyname=="comma") { return SDLK_COMMA; }
	else if (keyname=="minus sign") { return SDLK_MINUS; }
	else if (keyname=="period / full stop") { return SDLK_PERIOD; }
	else if (keyname=="forward slash") { return SDLK_SLASH; }
	else if (keyname=="0") { return SDLK_0; }
	else if (keyname=="1") { return SDLK_1; }
	else if (keyname=="2") { return SDLK_2; }
	else if (keyname=="3") { return SDLK_3; }
	else if (keyname=="4") { return SDLK_4; }
	else if (keyname=="5") { return SDLK_5; }
	else if (keyname=="6") { return SDLK_6; }
	else if (keyname=="7") { return SDLK_7; }
	else if (keyname=="8") { return SDLK_8; }
	else if (keyname=="9") { return SDLK_9; }
	else if (keyname=="colon") { return SDLK_COLON; }
	else if (keyname=="semicolon") { return SDLK_SEMICOLON; }
	else if (keyname=="less-than sign") { return SDLK_LESS; }
	else if (keyname=="equals sign") { return SDLK_EQUALS; }
	else if (keyname=="greater-than sign") { return SDLK_GREATER; }
	else if (keyname=="question mark") { return SDLK_QUESTION; }
	else if (keyname=="at") { return SDLK_AT; }
	else if (keyname=="left bracket") { return SDLK_LEFTBRACKET; }
	else if (keyname=="backslash") { return SDLK_BACKSLASH; }
	else if (keyname=="right bracket") { return SDLK_RIGHTBRACKET; }
	else if (keyname=="caret") { return SDLK_CARET; }
	else if (keyname=="underscore") { return SDLK_UNDERSCORE; }
	else if (keyname=="grave") { return SDLK_BACKQUOTE; }
	else if (keyname=="a") { return SDLK_a; }
	else if (keyname=="b") { return SDLK_b; }
	else if (keyname=="c") { return SDLK_c; }
	else if (keyname=="d") { return SDLK_d; }
	else if (keyname=="e") { return SDLK_e; }
	else if (keyname=="f") { return SDLK_f; }
	else if (keyname=="g") { return SDLK_g; }
	else if (keyname=="h") { return SDLK_h; }
	else if (keyname=="i") { return SDLK_i; }
	else if (keyname=="j") { return SDLK_j; }
	else if (keyname=="k") { return SDLK_k; }
	else if (keyname=="l") { return SDLK_l; }
	else if (keyname=="m") { return SDLK_m; }
	else if (keyname=="n") { return SDLK_n; }
	else if (keyname=="o") { return SDLK_o; }
	else if (keyname=="p") { return SDLK_p; }
	else if (keyname=="q") { return SDLK_q; }
	else if (keyname=="r") { return SDLK_r; }
	else if (keyname=="s") { return SDLK_s; }
	else if (keyname=="t") { return SDLK_t; }
	else if (keyname=="u") { return SDLK_u; }
	else if (keyname=="v") { return SDLK_v; }
	else if (keyname=="w") { return SDLK_w; }
	else if (keyname=="x") { return SDLK_x; }
	else if (keyname=="y") { return SDLK_y; }
	else if (keyname=="z") { return SDLK_z; }
	else if (keyname=="delete") { return SDLK_DELETE; }
	else if (keyname=="keypad 0") { return SDLK_KP0; }
	else if (keyname=="keypad 1") { return SDLK_KP1; }
	else if (keyname=="keypad 2") { return SDLK_KP2; }
	else if (keyname=="keypad 3") { return SDLK_KP3; }
	else if (keyname=="keypad 4") { return SDLK_KP4; }
	else if (keyname=="keypad 5") { return SDLK_KP5; }
	else if (keyname=="keypad 6") { return SDLK_KP6; }
	else if (keyname=="keypad 7") { return SDLK_KP7; }
	else if (keyname=="keypad 8") { return SDLK_KP8; }
	else if (keyname=="keypad 9") { return SDLK_KP9; }
	else if (keyname=="keypad period") { return SDLK_KP_PERIOD; }
	else if (keyname=="keypad divide") { return SDLK_KP_DIVIDE; }
	else if (keyname=="keypad multiply") { return SDLK_KP_MULTIPLY; }
	else if (keyname=="keypad minus") { return SDLK_KP_MINUS; }
	else if (keyname=="keypad plus") { return SDLK_KP_PLUS; }
	else if (keyname=="keypad enter") { return SDLK_KP_ENTER; }
	else if (keyname=="keypad equals") { return SDLK_KP_EQUALS; }
	else if (keyname=="up arrow") { return SDLK_UP; }
	else if (keyname=="down arrow") { return SDLK_DOWN; }
	else if (keyname=="right arrow") { return SDLK_RIGHT; }
	else if (keyname=="left arrow") { return SDLK_LEFT; }
	else if (keyname=="insert") { return SDLK_INSERT; }
	else if (keyname=="home") { return SDLK_HOME; }
	else if (keyname=="end") { return SDLK_END; }
	else if (keyname=="page up") { return SDLK_PAGEUP; }
	else if (keyname=="page down") { return SDLK_PAGEDOWN; }
	else if (keyname=="F1") { return SDLK_F1; }
	else if (keyname=="F2") { return SDLK_F2; }
	else if (keyname=="F3") { return SDLK_F3; }
	else if (keyname=="F4") { return SDLK_F4; }
	else if (keyname=="F5") { return SDLK_F5; }
	else if (keyname=="F6") { return SDLK_F6; }
	else if (keyname=="F7") { return SDLK_F7; }
	else if (keyname=="F8") { return SDLK_F8; }
	else if (keyname=="F9") { return SDLK_F9; }
	else if (keyname=="F10") { return SDLK_F10; }
	else if (keyname=="F11") { return SDLK_F11; }
	else if (keyname=="F12") { return SDLK_F12; }
	else if (keyname=="F13") { return SDLK_F13; }
	else if (keyname=="F14") { return SDLK_F14; }
	else if (keyname=="F15") { return SDLK_F15; }
	else if (keyname=="numlock") { return SDLK_NUMLOCK; }
	else if (keyname=="capslock") { return SDLK_CAPSLOCK; }
	else if (keyname=="scrollock") { return SDLK_SCROLLOCK; }
	else if (keyname=="right shift") { return SDLK_RSHIFT; }
	else if (keyname=="left shift") { return SDLK_LSHIFT; }
	else if (keyname=="right ctrl") { return SDLK_RCTRL; }
	else if (keyname=="left ctrl") { return SDLK_LCTRL; }
	else if (keyname=="right alt / alt gr") { return SDLK_RALT; }
	else if (keyname=="left alt") { return SDLK_LALT; }
	else if (keyname=="right meta") { return SDLK_RMETA; }
	else if (keyname=="left meta") { return SDLK_LMETA; }
	else if (keyname=="left windows key") { return SDLK_LSUPER; }
	else if (keyname=="right windows key") { return SDLK_RSUPER; }
	else if (keyname=="mode shift") { return SDLK_MODE; }
	else if (keyname=="compose") { return SDLK_COMPOSE; }
	else if (keyname=="help") { return SDLK_HELP; }
	else if (keyname=="print-screen") { return SDLK_PRINT; }
	else if (keyname=="SysRq") { return SDLK_SYSREQ; }
	else if (keyname=="break") { return SDLK_BREAK; }
	else if (keyname=="menu") { return SDLK_MENU; }
	else if (keyname=="power") { return SDLK_POWER; }
	else if (keyname=="euro") { return SDLK_EURO; }
	else if (keyname=="undo") { return SDLK_UNDO; }

	return (SDLKey)0;
	}


void Keyboard::Init() {
//Create the keys
	rep_start_time=0;
	rep_rep_time=0;
	keydown= SDL_GetKeyState(&keynum);
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
									if (i!=SDLK_RSHIFT && i!=SDLK_LSHIFT && i!=SDLK_LCTRL && i!=SDLK_RSHIFT && i!=SDLK_RCTRL && i!=SDLK_LALT && i!=SDLK_RALT && i!=SDLK_CAPSLOCK && i!=SDLK_NUMLOCK)
										while (pressedtime[i]>rep_start_time+rep_rep_time) { toggle=1; pressedtime[i]-=rep_rep_time;}

									}
							}

					if (keydown[i])  {pressedtime[i]+=el; }
					else { pressedtime[i]=0; }

					handler(i,(keydown[i]),toggle);
					}

			//Reset everythink
			downbefore[i]=keydown[i];

			}
	}

void Keyboard::StartTextInput() {
	SDL_EnableUNICODE(SDL_ENABLE);
	textinputmode=1;
	}

void Keyboard::StopTextInput() {
	SDL_EnableUNICODE(SDL_DISABLE);
	textinputmode=0;
	}

void Keyboard::DispatchEvent(SDL_Event *ev) {
	if (!textinputmode) { return; }
	if (ev->type==SDL_KEYDOWN) {
			lastkeysim=ev->key.keysym;
			}
	}

std::string Keyboard::GetKeyName(int key) {
	std::string result="";
	switch (key) {
			case SDLK_BACKSPACE : result="backspace"; break;
			case SDLK_TAB : result="tab"; break;
			case SDLK_CLEAR : result="clear"; break;
			case SDLK_RETURN : result="return"; break;
			case SDLK_PAUSE : result="pause"; break;
			case SDLK_ESCAPE : result="escape"; break;
			case SDLK_SPACE : result="space"; break;
			case SDLK_EXCLAIM : result="exclamation mark"; break;
			case SDLK_QUOTEDBL : result="double quote"; break;
			case SDLK_HASH : result="hash"; break;
			case SDLK_DOLLAR : result="dollar"; break;
			case SDLK_AMPERSAND : result="ampersand"; break;
			case SDLK_QUOTE : result="single quote"; break;
			case SDLK_LEFTPAREN : result="left parenthesis"; break;
			case SDLK_RIGHTPAREN : result="right parenthesis"; break;
			case SDLK_ASTERISK : result="asterisk"; break;
			case SDLK_PLUS : result="plus sign"; break;
			case SDLK_COMMA : result="comma"; break;
			case SDLK_MINUS : result="minus sign"; break;
			case SDLK_PERIOD : result="period / full stop"; break;
			case SDLK_SLASH : result="forward slash"; break;
			case SDLK_0 : result="0"; break;
			case SDLK_1 : result="1"; break;
			case SDLK_2 : result="2"; break;
			case SDLK_3 : result="3"; break;
			case SDLK_4 : result="4"; break;
			case SDLK_5 : result="5"; break;
			case SDLK_6 : result="6"; break;
			case SDLK_7 : result="7"; break;
			case SDLK_8 : result="8"; break;
			case SDLK_9 : result="9"; break;
			case SDLK_COLON : result="colon"; break;
			case SDLK_SEMICOLON : result="semicolon"; break;
			case SDLK_LESS : result="less-than sign"; break;
			case SDLK_EQUALS : result="equals sign"; break;
			case SDLK_GREATER : result="greater-than sign"; break;
			case SDLK_QUESTION : result="question mark"; break;
			case SDLK_AT : result="at"; break;
			case SDLK_LEFTBRACKET : result="left bracket"; break;
			case SDLK_BACKSLASH : result="backslash"; break;
			case SDLK_RIGHTBRACKET : result="right bracket"; break;
			case SDLK_CARET : result="caret"; break;
			case SDLK_UNDERSCORE : result="underscore"; break;
			case SDLK_BACKQUOTE : result="grave"; break;
			case SDLK_a : result="a"; break;
			case SDLK_b : result="b"; break;
			case SDLK_c : result="c"; break;
			case SDLK_d : result="d"; break;
			case SDLK_e : result="e"; break;
			case SDLK_f : result="f"; break;
			case SDLK_g : result="g"; break;
			case SDLK_h : result="h"; break;
			case SDLK_i : result="i"; break;
			case SDLK_j : result="j"; break;
			case SDLK_k : result="k"; break;
			case SDLK_l : result="l"; break;
			case SDLK_m : result="m"; break;
			case SDLK_n : result="n"; break;
			case SDLK_o : result="o"; break;
			case SDLK_p : result="p"; break;
			case SDLK_q : result="q"; break;
			case SDLK_r : result="r"; break;
			case SDLK_s : result="s"; break;
			case SDLK_t : result="t"; break;
			case SDLK_u : result="u"; break;
			case SDLK_v : result="v"; break;
			case SDLK_w : result="w"; break;
			case SDLK_x : result="x"; break;
			case SDLK_y : result="y"; break;
			case SDLK_z : result="z"; break;
			case SDLK_DELETE : result="delete"; break;
			case SDLK_KP0 : result="keypad 0"; break;
			case SDLK_KP1 : result="keypad 1"; break;
			case SDLK_KP2 : result="keypad 2"; break;
			case SDLK_KP3 : result="keypad 3"; break;
			case SDLK_KP4 : result="keypad 4"; break;
			case SDLK_KP5 : result="keypad 5"; break;
			case SDLK_KP6 : result="keypad 6"; break;
			case SDLK_KP7 : result="keypad 7"; break;
			case SDLK_KP8 : result="keypad 8"; break;
			case SDLK_KP9 : result="keypad 9"; break;
			case SDLK_KP_PERIOD : result="keypad period"; break;
			case SDLK_KP_DIVIDE : result="keypad divide"; break;
			case SDLK_KP_MULTIPLY : result="keypad multiply"; break;
			case SDLK_KP_MINUS : result="keypad minus"; break;
			case SDLK_KP_PLUS : result="keypad plus"; break;
			case SDLK_KP_ENTER : result="keypad enter"; break;
			case SDLK_KP_EQUALS : result="keypad equals"; break;
			case SDLK_UP : result="up arrow"; break;
			case SDLK_DOWN : result="down arrow"; break;
			case SDLK_RIGHT : result="right arrow"; break;
			case SDLK_LEFT : result="left arrow"; break;
			case SDLK_INSERT : result="insert"; break;
			case SDLK_HOME : result="home"; break;
			case SDLK_END : result="end"; break;
			case SDLK_PAGEUP : result="page up"; break;
			case SDLK_PAGEDOWN : result="page down"; break;
			case SDLK_F1 : result="F1"; break;
			case SDLK_F2 : result="F2"; break;
			case SDLK_F3 : result="F3"; break;
			case SDLK_F4 : result="F4"; break;
			case SDLK_F5 : result="F5"; break;
			case SDLK_F6 : result="F6"; break;
			case SDLK_F7 : result="F7"; break;
			case SDLK_F8 : result="F8"; break;
			case SDLK_F9 : result="F9"; break;
			case SDLK_F10 : result="F10"; break;
			case SDLK_F11 : result="F11"; break;
			case SDLK_F12 : result="F12"; break;
			case SDLK_F13 : result="F13"; break;
			case SDLK_F14 : result="F14"; break;
			case SDLK_F15 : result="F15"; break;
			case SDLK_NUMLOCK : result="numlock"; break;
			case SDLK_CAPSLOCK : result="capslock"; break;
			case SDLK_SCROLLOCK : result="scrollock"; break;
			case SDLK_RSHIFT : result="right shift"; break;
			case SDLK_LSHIFT : result="left shift"; break;
			case SDLK_RCTRL : result="right ctrl"; break;
			case SDLK_LCTRL : result="left ctrl"; break;
			case SDLK_RALT : result="right alt / alt gr"; break;
			case SDLK_LALT : result="left alt"; break;
			case SDLK_RMETA : result="right meta"; break;
			case SDLK_LMETA : result="left meta"; break;
			case SDLK_LSUPER : result="left windows key"; break;
			case SDLK_RSUPER : result="right windows key"; break;
			case SDLK_MODE : result="mode shift"; break;
			case SDLK_COMPOSE : result="compose"; break;
			case SDLK_HELP : result="help"; break;
			case SDLK_PRINT : result="print-screen"; break;
			case SDLK_SYSREQ : result="SysRq"; break;
			case SDLK_BREAK : result="break"; break;
			case SDLK_MENU : result="menu"; break;
			case SDLK_POWER : result="power"; break;
			case SDLK_EURO : result="euro"; break;
			case SDLK_UNDO : result="undo"; break;
			}
//TrimSpaces(result);
	return result;
	}




////////////LUA-IMPLEMENT////////////7


int KEYB_GetKeyConst(lua_State *state) {
	std::string k=LUA_GET_STRING(state);
	SDLKey res=  g_Game()->GetKeyboard()->GetKeyConstFor(k);
	LUA_SET_NUMBER(state, res);
	return 1;
	}

int KEYB_GetKeyName(lua_State *state) {
	int k=LUA_GET_INT(state);
	std::string s=  g_Game()->GetKeyboard()->GetKeyName(k);
	LUA_SET_STRING(state, s);
	return 1;
	}


void LUA_KEYB_RegisterLib() {
	g_CuboLib()->AddFunc("KEYB_GetKeyConst",KEYB_GetKeyConst);
	g_CuboLib()->AddFunc("KEYB_GetKeyName",KEYB_GetKeyName);
	}
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
