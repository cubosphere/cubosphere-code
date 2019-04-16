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

#include "definitions.hpp"
#include <vector>

#include <GL/glew.h>
#include <SDL.h>

struct TempTexture {
	GLuint tindex;
	GLuint fbo;
	GLuint depth;
	int w,h;
	};

class PostEffect : public BaseLuaDef {
	protected:
		//TLuaAccess lua;
		std::vector<TempTexture> ttexts;
		int isprecached;
		//virtual int SendIDWhenPrecache() {return -1;}
	public:
		PostEffect() : isprecached(0) {}
		int GetType() {return FILE_POSTEFFECTDEF;}
		virtual ~PostEffect();
		void CallRender();
		int CreateTempTexture(int w, int h,int withdepth);
		void SetRenderTarget(int index);
		void TempTextureToStage(int ttex,int stage);
		void DepthToStage(int ttex,int stage);
		void DrawQuad();
		void Precache();
		void UnPrecache();
		void CallDefaultSpecialRender(std::string nam,std::string what,int index);
	};


extern PostEffect *g_PostEffect();
extern void LoadPostEffect(std::string name);

extern void LUA_EFFECT_RegisterLib();
