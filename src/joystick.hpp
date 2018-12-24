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

#include <SDL.h>

#include <vector>
#include <memory>
#include <unordered_map>
#include "cuboutils.hpp"

class JoystickServer;

class Joystick {
	protected:
		JoystickServer *server;
		int index;
		SDL_Joystick *stick;
		std::vector<float> axisvals,axisovals;
		std::vector<int> buttons,obuttons;
		float SDLAxisToFloat(int sa);
	public:
		Joystick(int mindex,JoystickServer *serv);
		~Joystick();
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

// Stick-Index, Axis-Index, value, oldvalue
using JoystickAxisFunc = void(*)(int,int,float,float);

// Stick-Index, Button-Index, dir(for axis), value, toggle
using JoystickButtonFunc = void(*)(int,int,int,int,int);

class JoystickServer {
	protected:
		std::unordered_map<int, std::unique_ptr<Joystick>> sticks;
		JoystickAxisFunc axishandler;
		JoystickButtonFunc buttonhandler;
		friend class Joystick;
	public:
		void AddJoystick(int mindex);
		void RemoveJoystick(int mindex);
		void SetAxisHandler(JoystickAxisFunc h) {axishandler=h;}
		void SetDiscreteHandler(JoystickButtonFunc b) {buttonhandler=b;}
		int NumJoysticks();
		void DispatchEvent(SDL_Event *ev);
		void Initialize();
		void Free();
		void HandleKeys();
		void ResetButtons();
	};

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
