#pragma once
#include "luautils.hpp"

// All implementations are stored in correct .cpp's

class LuaModuleJoystick: public LuaModule {
public:
	LuaModuleJoystick();
	virtual ~LuaModuleJoystick() {};
};

class LuaModulePlayer: public LuaModule {
	public:
		LuaModulePlayer();
		virtual ~LuaModulePlayer() {};
	};

void LuaRegisterLoader(lua_State*);

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
