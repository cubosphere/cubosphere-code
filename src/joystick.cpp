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

#include <SDL.h>

#include <vector>
#include "cuboutils.hpp"
#include <sstream>
#include "luautils.hpp"
#include "luamodules.hpp"
#include "game.hpp"

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
	return SDL_JoystickName(stick);
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

void JoystickServer::AddJoystick(int mindex) {
	auto njs = std::make_unique<Joystick>(mindex, this);
	if (!(njs->GetSDLJoystick())) {
			coutlog("Joystick \""+njs->Name()+"\" (" + std::to_string(mindex+1) + ") failed to initialize", 2);
			}
	else {
			coutlog("Joystick \""+njs->Name()+"\" (" + std::to_string(mindex+1) + ") initialized", 2);
			sticks.emplace(mindex, std::move(njs));
			}
	}

void JoystickServer::RemoveJoystick(int mindex) {
	coutlog("Joystick \""+sticks.at(mindex)->Name()+"\" (" + std::to_string(mindex+1) + ") disconnected", 2);
	sticks.erase(mindex);
	}

void JoystickServer::Initialize() {
	std::ostringstream oss;
	int ns=NumJoysticks();
	if (!ns) { oss << "no joystick found"; }
	else if (ns==1) { oss << "detected a joystick"; }
	else { oss << "detected " << ns << " joysticks"; }
	coutlog(oss.str());
	if (ns == 0) { return; }


	for (int i=0; i<ns; i++) {
			AddJoystick(i);
			}
	}

void JoystickServer::Free() {
	sticks.clear();
	}


void JoystickServer::DispatchEvent(SDL_Event *ev) {
	return;
	}


void JoystickServer::HandleKeys() {
	for (auto& stick: sticks) { stick.second->HandleKeys(); }
	}

int JoystickServer::NumJoysticks() {
	return SDL_NumJoysticks();
	}

bool JoyWrapper::IsConnected() {
	return g_Game()->GetJoysticks()->IsConnected(jidx);
	}

std::vector<int> JoystickServer::ConnectedJoys() {
	std::vector<int> res;
	for (auto& e: sticks) { res.push_back(e.first); }
	return res;
	}

std::string JoyWrapper::GetGUID() {
	constexpr auto bufsize = 33;
	char buf[bufsize];
	auto guid = SDL_JoystickGetDeviceGUID(jidx);
	SDL_JoystickGetGUIDString(guid, buf, bufsize);
	return std::string(buf);
	}

std::string JoystickServer::GetName(int mindex) {
	if (IsConnected(mindex)) {
			return sticks.at(mindex)->Name();
			}
	else {
			return "<ERROR>"; // You should check it on lua side
			}
	}


std::string JoyWrapper::GetName() {
	return g_Game()->GetJoysticks()->GetName(jidx);
	}


static int JoystickT_IsConnected(lua_State* L) {
	LuaJoystick joy;
	if (joy.LuaLoad(L, 1)) {
			LUA_SET_BOOL(L, joy.GetObj()->IsConnected());
			return 1;
			}
	else { return 0; }
	}

static int JoystickT_GetGUID(lua_State* L) {
	LuaJoystick joy;
	if (joy.LuaLoad(L, 1)) {
			LUA_SET_STRING(L, joy.GetObj()->GetGUID());
			return 1;
			}
	else { return 0; }
	}

static int JoystickT_GetName(lua_State* L) {
	LuaJoystick joy;
	if (joy.LuaLoad(L, 1)) {
			LUA_SET_STRING(L, joy.GetObj()->GetName());
			return 1;
			}
	else { return 0; }
	}

static int Joystick_Get(lua_State* L) {
	auto num = LUA_GET_INT(L, 1);
	LuaJoystick joy(num);
	joy.LuaPush(L);
	return 1;
	};

static int Joystick_List(lua_State* L) {
	auto joys = g_Game()->GetJoysticks()->ConnectedJoys();
	lua_createtable(L, joys.size(), 0);
	for (size_t i = 0; i < joys.size(); ++i) {
			LUA_SET_NUMBER(L, i+1);
			LUA_SET_NUMBER(L, joys.at(i));
			lua_settable(L, -3);
			}
	return 1;
	}

static int Joystick_Count(lua_State* L) {
	LUA_SET_NUMBER(L, g_Game()->GetJoysticks()->NumJoysticks());
	return 1;
	};

void LuaJoystick::UserInit(lua_State* L) {
	AddMethod("IsConnected", JoystickT_IsConnected);
	AddMethod("GetGUID", JoystickT_GetGUID);
	AddMethod("GetName", JoystickT_GetName);
	}


LuaModuleJoystick::LuaModuleJoystick(): LuaModule("Joystick") {
	AddType(std::make_unique<LuaJoystick>());
	AddFunc("Get", Joystick_Get);
	AddFunc("List", Joystick_List);
	AddFunc("Count", Joystick_Count);
	}
