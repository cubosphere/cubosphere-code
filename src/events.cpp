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

#include <GL/glew.h>
#include <SDL.h>

#include "events.hpp"
#include <iostream>

void EventManager::HandleEvents() {
	SDL_Event event;
	if (mouse) { mouse->BeginDispatch(); }
	while(SDL_PollEvent(&event)) {
			//Und gehen diese durch
			switch(event.type) {
					case SDL_KEYDOWN:
					case SDL_KEYUP:
						//	case SDL_TEXTINPUT:
						//		case SDL_TEXTEDITING:
						//Send it to Keyboard
						break;
					case SDL_MOUSEMOTION:
					case SDL_MOUSEBUTTONDOWN:
					case SDL_MOUSEBUTTONUP:
						if (mouse) { mouse->DispatchEvent(&event); }
						break;
					case SDL_JOYAXISMOTION:
					case SDL_JOYBALLMOTION:
					case SDL_JOYHATMOTION:
					case SDL_JOYBUTTONDOWN:
					case SDL_JOYBUTTONUP:
						if (joy) { joy->DispatchEvent(&event); }
						break;
					case SDL_QUIT:
						//Sollte das Fenster geschlossen werden, soll er auch der Loop beendet werden
						windowclose=true;
						break;
					default:
						break;
					}
			}

	}
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
