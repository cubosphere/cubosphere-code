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

#include <vector>
#include <iostream>
#include <stdlib.h>
#include <memory>

#define GL_GLEXT_PROTOTYPES 1

#include <GL/glew.h>
#include <SDL.h>
#include <SDL_image.h>

#include "definitions.hpp"
#include "filesystem.hpp"

////Texture Defs

struct membuf : std::streambuf {
	membuf(char* begin, char* end) {
		this->setg(begin, begin, end);
		}
	};

enum class TextureType {
	Invalid,
	JPEG,
	PNG
	};

class TextureDef : public BaseLuaDef {
	public:
		virtual int GetType() {return FILE_TEXTUREDEF;}
		void Call_Render(int sideid);
		void RenderPlane();
		void Render2d();
	};

using TextureDefServer = BaseDefServer<TextureDef>;

//Inherited the different formats herefrom
class Texture {
	protected:
		unsigned int width,height;
	public:
		int getWidth() {return width;}
		int getHeight() {return height;}
		Texture() : width(0), height(0) {}
		Texture(int w,int h) : width(w), height(h) {}
		virtual void* getRGBPointer() {return NULL;}
		virtual bool loadFromFile(const std::unique_ptr<CuboFile>& finfo) = 0;
		virtual int HasAlpha() {return 0;}
		virtual int CanFastResize([[maybe_unused]] int maxdim) {return 0;}
		virtual void FastResize([[maybe_unused]] int maxdim) {}
		virtual ~Texture() {};
	};


class SDLTexture: public Texture { // Uniform loader using SDL_Image
	protected:
		SDL_Surface* img = nullptr;
	public:
		virtual ~SDLTexture() { if(img) SDL_FreeSurface(img); };
		virtual void* getRGBPointer() override {return img->pixels;}; // Happy segfaults…
		virtual bool loadFromFile(const std::unique_ptr<CuboFile>& finfo) override;
		virtual int HasAlpha() override {return img->format->BytesPerPixel == 4;}
	};

struct FontExtend {
	float u1,v1,u2,v2;
	};


class TextureContainer {
	protected:
		GLuint tind;
		std::vector<FontExtend> chars;
		void AddChar(int x,int y,void *data,int width,int fsize);
	public:
		int duration1,duration2;
		void makeFromTexture(Texture* texture,int asfont,int maxsize);
		void activate();
		void activate(int stage);
		FontExtend GetFontExtend(int num) {return chars[num];}
		void clear() {glDeleteTextures(1,&tind);}
		TextureContainer() :tind(0) {}
	};

class TextureServer { // FIXME: Use unordered_map
	protected:
		std::vector<TextureContainer> Textures;
		std::vector<std::string> filenames,alphanames;
		std::vector<int> istemp;
		std::vector<int> activetextures;
		int timer1,timer2,timer3,numloads;
		int maxsize;
		bool txtenabled = false;
//      int makeNoiseTexture(int size) {TNoiseRGBTexture t(size,size); return addTexture(&t);} //Should not be used
		int addTexture(Texture *tex, int asfont) {
			TextureContainer cont; cont.makeFromTexture(tex,asfont,maxsize);  timer2+=cont.duration1;
			timer3+=cont.duration2; Textures.push_back(cont); return (Textures.size()-1);
			} //Should not be used directly
	public:
		TextureServer() : maxsize(512) {};
		void DeactivateStage(int stage);
		void activate(int i,int stage);
		void activate(int i);
		void Reload();
		void EnableTexturing() {if (not txtenabled) {glEnable( GL_TEXTURE_2D ); txtenabled = true;}}
		void DisableTexturing() {if (txtenabled) {glDisable( GL_TEXTURE_2D ); txtenabled = false;}}
		void Invalidate();
		void SetMaxTextureSize(int s) {maxsize=s;}
		int GetMaxTextureSize() {return maxsize;}
		FontExtend GetFontExtend(int index,int num);
		void clear();
		int LoadTexture(const std::unique_ptr<CuboFile>& finfo,int asfont=0,unsigned int colorkey=0);
		int LoadTextureAndAlpha(const std::unique_ptr<CuboFile>& finfo,std::unique_ptr<CuboFile>& afinfo);

		int LoadTempTexture(std::string tname, const std::unique_ptr<CuboFile>& finfo,int asfont=0,unsigned int colorkey=0); //For Preview-pics
		int TempTextureIndexFromName(std::string tname);
		void ResetTimerCounters();
		void CoutTimerString();
	};



extern void LUA_TEXDEF_RegisterLib();
extern void LUA_TEXTURE_RegisterLib();

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
