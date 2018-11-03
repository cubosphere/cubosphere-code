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

#ifdef WIN32

#include <windows.h>
#ifdef MINGW_CROSS_COMPILE
#include <GL/glew.h>
#else
#include <GL\glew.h>
#endif
// #include <GL\gl.h>
#include <SDL.h>



#else

#include <GL/glew.h>
#include <SDL/SDL.h>

#endif


#include "mouse.hpp"
#include "game.hpp"
#include "console.hpp"
#include <iostream>

TMouse *g_Mouse;

void TMouse::BeginDispatch()
	{
	dx=0;
	dy=0;
	}

void TMouse::SetButtonHandler(TMouseButtonFunc f)
	{
	buttonfunc=f;
	}

void TMouse::HandleClick(int butt,int press,int x,int y)
	{
	buttons[butt].pressed=!press;
	if (buttons[butt].pressed) { buttons[butt].lastx=x; buttons[butt].lasty=y; }
	else  { buttons[butt].relx=x; buttons[butt].rely=y; }
	if (buttonfunc) buttonfunc(butt,press,x,y);
	}

TMouseButton TMouse::GetButton(int i)
	{

	if ((i==3) || (i==4)) buttons[i].pressed=0;

	return buttons[i];
	}

void TMouse::DispatchEvent(SDL_Event *ev)
	{
	if (CuboConsole::getInstance()->IsActive()) return ;

	if (ev->type==SDL_MOUSEMOTION)
			{
			dx+=ev->motion.xrel;
			dy+=ev->motion.yrel;
///TODO: allow buttons etc.
			}
	else if (ev->type==SDL_MOUSEBUTTONDOWN)
			{

			//int bt;
			//if (ev->button.button== SDL_BUTTON_LEFT) bt=0;
			//else bt=1;

			buttons[ev->button.button-1].pressed=1;
			}
	else if (ev->type==SDL_MOUSEBUTTONUP)
			{
			int bt=ev->button.button-1;
			//if (ev->button.button == SDL_BUTTON_LEFT) bt=0;
			//else bt=1;
			if ((bt!=3) && (bt!=4))
				buttons[bt].pressed=0;
			}
	}

void TMouse::CenterPointer()
	{

	}

void TMouse::Initialize()
	{
	snapping=0;
	g_Mouse=this;
	WarpMode=1;
	for (int i=0; i<NUM_BUTTONS; i++) {
			buttons[i].lastx=-1;   buttons[i].lasty=-1;
			buttons[i].relx=-1; buttons[i].rely=-1;
			buttons[i].pressed=0;

			}
	SDL_ShowCursor(SDL_DISABLE);
	}

void TMouse::HandleMotion(int cx,int cy,int cpassive)
	{
	if (CuboConsole::getInstance()->IsActive()) return ;

	dx+=cx-x;
	dy+=cy-y;
	x=cx;
	y=cy;

	}

void TMouse::SetCursor(int curs)
	{

	}

T2dVector TMouse::getRelativeMotion()
	{
	T2dVector res(dx,dy);
	return res;
	}

void TMouse::Snap(int active)
	{
	if (active)
			{
			SDL_WM_GrabInput(SDL_GRAB_ON);
			snapping=1;
			getRelativeMotion();
			}
	else
			{
			snapping=0;
			SDL_WM_GrabInput(SDL_GRAB_OFF);
			}
	}



////////////////LUA-IMPLEMENT///////////////


int MOUSE_GetState(lua_State *state)
	{
	T2dVector dxy=g_Game()->GetMouse()->getRelativeMotion();
	lua_newtable(state);
	lua_pushstring(state, "dx");
	lua_pushnumber(state, dxy.u);
	lua_rawset(state, -3);
	lua_pushstring(state, "dy");
	lua_pushnumber(state, dxy.v);
	lua_rawset(state, -3);

	int dn=g_Game()->GetMouse()->GetButton(0).pressed;
	lua_pushstring(state, "button1");
	lua_pushnumber(state, dn);
	lua_rawset(state, -3);
	dn=g_Game()->GetMouse()->GetButton(1).pressed;
	lua_pushstring(state, "button2");
	lua_pushnumber(state, dn);
	lua_rawset(state, -3);
	dn=g_Game()->GetMouse()->GetButton(2).pressed;
	lua_pushstring(state, "button3");
	lua_pushnumber(state, dn);
	lua_rawset(state, -3);
	dn=g_Game()->GetMouse()->GetButton(3).pressed;
	lua_pushstring(state, "button4");
	lua_pushnumber(state, dn);
	lua_rawset(state, -3);
	dn=g_Game()->GetMouse()->GetButton(4).pressed;
	lua_pushstring(state, "button5");
	lua_pushnumber(state, dn);
	lua_rawset(state, -3);
	return 1;
	}

int MOUSE_Snap(lua_State *state)
	{
	int s=LUA_GET_INT;
	g_Game()->GetMouse()->Snap(s);
	return 0;
	}

void LUA_MOUSE_RegisterLib()
	{
	g_CuboLib()->AddFunc("MOUSE_GetState",MOUSE_GetState);
	g_CuboLib()->AddFunc("MOUSE_Snap",MOUSE_Snap);
	}
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
