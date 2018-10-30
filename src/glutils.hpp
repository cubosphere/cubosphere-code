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
