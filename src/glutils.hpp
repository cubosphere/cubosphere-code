#ifndef GL_UTILS_H_G
#define GL_UTILS_H_G

#include "luautils.hpp"


extern TLuaCFunctions * g_GLLib();

extern void LUA_BLEND_RegisterLib();
extern void LUA_CULL_RegisterLib();
extern void LUA_DEPTH_RegisterLib();
extern void LUA_DEVICE_RegisterLib();
extern void LUA_LIGHT_RegisterLib();
extern void LUA_MATERIAL_RegisterLib();

extern void InvalidateMaterial();

#endif
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
