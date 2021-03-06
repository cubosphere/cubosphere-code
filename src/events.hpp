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
#include <SDL.h>

#include "mouse.hpp"
#include "keyboard.hpp"
#include "joystick.hpp"

class EventManager {
	protected:
		int windowclose;
		Keyboard *keyb;
		Mouse *mouse;
		JoystickServer *joy;
	public:
		EventManager() : windowclose(0) {};
		void HandleEvents();
		void Close() {windowclose=1;}
		int Closed() {return windowclose;}
		void SetMouse(Mouse *m) {mouse=m;}
		void SetKeyboard(Keyboard *kb) {keyb=kb;}
		void SetJoystick(JoystickServer *ys) {joy=ys;}
	};
