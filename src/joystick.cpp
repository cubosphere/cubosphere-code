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

#include "joystick.hpp"



#ifdef WIN32

#include <SDL.h>

#else

#include <SDL/SDL.h>

#endif

#include <vector>
#include "cuboutils.hpp"
#include <sstream>
#include "luautils.hpp"

void Joystick::AxisMotionEvent(int axis,int value) {
	if (axis>=(int)(axisvals.size())) { return ; }
	float fval=SDLAxisToFloat(value);
	float oval=axisvals[axis];
	float diff=fval-oval;

	if (diff*diff>0.000000000000001)
//Call the handler //
///TODO: Careful -> Handler not installed in TGame currently
		if (server->axishandler) {
				server->axishandler(index,axis,fval,oval);
				}

	axisovals[axis]=oval;
	axisvals[axis]=fval;
	}


std::string Joystick::Name() {
	return SDL_JoystickName(index);
	}

float Joystick::SDLAxisToFloat(int sa) {
	return (sa < 0 ? (sa/32768.0) : (sa/32767.0));
	}

float Joystick::GetAxis(int i) {
	return SDLAxisToFloat(SDL_JoystickGetAxis(stick,i));
	}

int Joystick::GetButton(int i) {
	return SDL_JoystickGetButton(stick,i);
	}

int Joystick::NumAxes() {
	return SDL_JoystickNumAxes(stick);
	}

int Joystick::NumButtons() {
	return SDL_JoystickNumButtons(stick);
	}

Joystick::Joystick(int mindex,JoystickServer * serv) {
	server=serv;
	index=mindex;
	stick=SDL_JoystickOpen(index);
	if (!stick) { return; }
	for (int a=0; a<NumAxes(); a++) {
			axisvals.push_back(GetAxis(a));
			axisovals.push_back(axisvals[a]);
			}
	for (int b=0; b<NumButtons(); b++) {
			buttons.push_back(GetButton(b));
			obuttons.push_back(buttons[b]);
			}
	}

Joystick::~Joystick() {
	if (stick) {SDL_JoystickClose(stick);}
	}

void Joystick::HandleKeys() {
	float plustresh=0.5;
	float minustresh=-0.5;
//Iterate through the axis
	for (int a=0; a<NumAxes(); a++) {
			int down,downbefore;
			axisvals[a]=GetAxis(a);

			for (int sign=-1; sign<=1; sign+=2) {

					if (sign>0) {
							down=axisvals[a]>plustresh;
							downbefore=axisovals[a]>plustresh;
							}
					else {
							down=axisvals[a]<minustresh;
							downbefore=axisovals[a]<minustresh;

							}

					if ((down) || (downbefore)) {
							bool toggle=((down) && !(downbefore));
							server->buttonhandler(index,a,sign,down,toggle);
							}

					}
			axisovals[a]=axisvals[a];
			}

	for (int b=0; b<NumButtons(); b++) {
			//  coutlog("B");
			buttons[b]=GetButton(b);
			if ((buttons[b]) || (obuttons[b])) {
					bool toggle=((buttons[b]) && !(obuttons[b]));
					server->buttonhandler(index,b,0,buttons[b],toggle);
					}

			//Reset everythink
			obuttons[b]=buttons[b];
			}
	}

void Joystick::ResetButtons() {
	for (unsigned int i=0; i<obuttons.size(); i++) { obuttons[i]=0; }
	for (unsigned int i=0; i<axisovals.size(); i++) { axisovals[i]=0; }

	}

void JoystickServer::ResetButtons() {
	for (unsigned int i=0; i<sticks.size(); i++) { sticks[i]->ResetButtons(); }
	}


void JoystickServer::Initialize() {
	std::ostringstream oss;
	int ns=NumJoysticks();
	if (!ns) { oss << "no joystick found"; }
	else if (ns==1) { oss << "detected a joystick"; }
	else { oss << "detected " << ns << " joysticks"; }
	coutlog(oss.str());
	if (!ns) { return; }


	for (int i=0; i<ns; i++) {
			Joystick *njs=new Joystick(i,this);
			oss.str("");
			if (!(njs->GetSDLJoystick())) {
					oss.clear();
					oss << "  ("<<(i+1)<< ")  failed to initialize " << njs->Name() ;
					coutlog(oss.str(),2);
					sticks.push_back(NULL);
					}
			else {
					sticks.push_back(njs);
					oss.clear();
					oss << "  ("<<(i+1) << ")  initialized " << njs->Name() ;
					coutlog(oss.str());
					}
			}
	}

void JoystickServer::Free() {
	for (unsigned int i=0; i<sticks.size(); i++) {
			if (sticks[i]) { delete sticks[i]; }
			}
	}


void JoystickServer::DispatchEvent(SDL_Event *ev) {
	return;
	}


void JoystickServer::HandleKeys() {
	for (unsigned int i=0; i<sticks.size(); i++) {
			if (sticks[i]) { sticks[i]->HandleKeys(); }

			}
	}

int JoystickServer::NumJoysticks() {
	return SDL_NumJoysticks();
	}
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
