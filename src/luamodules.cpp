#include "luamodules.hpp"
#include <memory>

static std::unordered_map<std::string, std::shared_ptr<LuaModule>> LuaMods = { // Everything what we export
	{"Player", std::make_shared<LuaModulePlayer>()},
	{"Joystick", std::make_shared<LuaModuleJoystick>()}
};

static int LuaModuleEcho(lua_State* L) {
	auto modname = LUA_GET_STRING(L, lua_upvalueindex(1));
	auto mod = LuaMods.at(modname);
	mod->InitToState(L);
	mod->PushToState(L);
	return 1;
};

static int LuaModuleLoader(lua_State* L) {
	auto modname = LUA_GET_STRING(L, 1);
	if (LuaMods.count(modname)) {
		LUA_SET_STRING(L, modname);
		lua_pushcclosure(L, LuaModuleEcho, 1);
		return 1;
	}
	return 0;
};

void LuaRegisterLoader(lua_State* L) {
	lua_getfield(L, LUA_GLOBALSINDEX, "package");
	// +1
	lua_getfield(L, -1, "loaders");
	// +2
	auto len = lua_getlen(L, -1);
	lua_pushcfunction(L, LuaModuleLoader);
	// +3
	lua_rawseti(L, -2, len + 1);
	// +2
	lua_pop(L, 2);
}

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
