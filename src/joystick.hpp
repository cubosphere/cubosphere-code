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

#ifndef JOYSTICK_H_G
#define JOYSTICK_H_G


#ifdef WIN32

#include <SDL.h>

#else

#include <SDL/SDL.h>

#endif

#include <vector>
#include "cuboutils.hpp"

class TJoystickServer;

class TJoystick
	{
	protected:
		TJoystickServer *server;
		int index;
		SDL_Joystick *stick;
		std::vector<float> axisvals,axisovals;
		std::vector<int> buttons,obuttons;
		float SDLAxisToFloat(int sa);
	public:
		TJoystick(int mindex,TJoystickServer *serv);
		~TJoystick();
		SDL_Joystick * GetSDLJoystick() { return stick;}
		std::string Name();
		int NumAxes();
		int NumButtons();

		void AxisMotionEvent(int axis,int value);
		void HandleKeys();
		float GetAxis(int i);
		int GetButton(int i);
		void ResetButtons();
	};

// (Stick-Index, Axis-Index, value, oldvalue
typedef void(*TJoystickAxisFunc)(int,int,float,float) ;

// (Stick-Index, Button-Index, dir(for axis), value, toggle
typedef void(*TJoystickButtonFunc)(int,int,int,int,int) ;

class TJoystickServer
	{
	protected:
		std::vector<TJoystick*> sticks;
		TJoystickAxisFunc axishandler;
		TJoystickButtonFunc buttonhandler;
		friend class TJoystick;
	public:
		void SetAxisHandler(TJoystickAxisFunc h) {axishandler=h;}
		void SetDiscreteHandler(TJoystickButtonFunc b) {buttonhandler=b;}
		int NumJoysticks();
		void DispatchEvent(SDL_Event *ev);
		void Initialize();
		void Free();
		void HandleKeys();
		void ResetButtons();
	};



#endif
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
