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

#pragma once

#include "glew.hpp"
#include <SDL2/SDL.h>

#include "vectors.hpp"


#define NUM_BUTTONS 6

// BUTTON, CLICK OR RELEASE, x,y
using MouseButtonFunc = void(*)(int,int,int,int);

struct MouseButton {
	int pressed;
	int lastx,lasty,relx,rely;
	};

class Mouse {
	protected:
		int snapping;
		int x,y,dx,dy,WarpMode;
		MouseButtonFunc buttonfunc;
		MouseButton buttons[NUM_BUTTONS];
	public:
		void Initialize();
		void BeginDispatch(); //Set the Relative motion to 0
		void HandleMotion(int cx, int cy, int cpassive);
		void CenterPointer();
		void SetCursor(int curs);
		void SetButtonHandler(MouseButtonFunc f);
		void HandleClick(int butt,int press,int x,int y);
		MouseButton GetButton(int i);
		Vector2d getRelativeMotion();
		void DispatchEvent(SDL_Event *ev);
		void Snap(int active);
		int IsSnapped() {return snapping;}
	};

extern void LUA_MOUSE_RegisterLib();
