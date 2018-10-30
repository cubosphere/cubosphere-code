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

#ifndef TEXTURES_H_G
#define TEXTURES_H_G

#include <vector>
#include <iostream>
#include <stdlib.h>


#define GL_GLEXT_PROTOTYPES 1

#ifdef WIN32

#ifdef MINGW_CROSS_COMPILE
#include <GL/glew.h>
#else
#include <GL\glew.h>
#endif
#include <windows.h>
#include <SDL.h>

#else

#include <GL/glew.h>
#include <SDL/SDL.h>

#endif

#include "definitions.hpp"
#include "filesystem.hpp"

////Texture Defs


class TTextureDef : public TBaseLuaDef
	{
	public:
		virtual int GetType() {return FILE_TEXTUREDEF;}
		void Call_Render(int sideid);
		void RenderPlane();
		void Render2d();
	};



typedef TBaseDefServer<TTextureDef> TTextureDefServer;




//Inherited the different formats herefrom
class TTexture
	{
	protected:
		int width,height;
	public:
		int getWidth() {return width;}
		int getHeight() {return height;}
		TTexture() : width(0), height(0) {}
		TTexture(int w,int h) : width(w), height(h) {}
		virtual void* getRGBPointer() {return NULL;}
		virtual bool loadFromFile(TCuboFile *finfo) {return false;}
		virtual int HasAlpha() {return 0;}
		virtual int CanFastResize(int maxdim) {return 0;}
		virtual void FastResize(int maxdim) {}
		virtual ~TTexture() {};
	};


class TJPEGTexture: public TTexture
	{
	protected:
		void * raw;
		int trans,channels;
		void shrink_blur(int ammount);
		void shrink_half_blur();
	public:
		virtual ~TJPEGTexture() {if (raw) free(raw);}
		TJPEGTexture() : TTexture(), raw(NULL), trans(0) {}
		TJPEGTexture(int w,int h) : TTexture(w,h), raw(NULL), trans(0) {}
		virtual void* getRGBPointer() {return raw;}
		virtual bool loadFromFile(TCuboFile *finfo);
		virtual int LoadAlphaTexture(TCuboFile *finfo);
		virtual int GetChannels() {return channels;}
		virtual int HasAlpha() {return trans;}
		virtual void ColorKeyTransparency(unsigned int ckey);
		virtual int CanFastResize(int maxdim);
		virtual void FastResize(int maxdim);
	};

/*class TPNGTexture: public TTexture
{
 protected:
   void * raw;
   int hasAlphaChannel;
   int hasTransparency;
 public:
   virtual void loadFromFile(string fname);
   virtual void* getRGBPointer() {return raw;}
   virtual int HasAlpha() {return hasAlphaChannel;} //TODO: Fix it appropriate

};*/



typedef struct
	{
	float u1,v1,u2,v2;
	} TFontExtend;


class TTextureContainer
	{
	protected:
		GLuint tind;
		std::vector<TFontExtend> chars;
		void AddChar(int x,int y,void *data,int width,int fsize);
	public:
		int duration1,duration2;
		void makeFromTTexture(TTexture* texture,int asfont,int maxsize);
		void activate();
		void activate(int stage);
		TFontExtend GetFontExtend(int num) {return chars[num];}
		void clear() {glDeleteTextures(1,&tind);}
		TTextureContainer() :tind(0) {}
	};

class TTextureServer
	{
	protected:
		std::vector<TTextureContainer> Textures;
		std::vector<std::string> filenames,alphanames;
		std::vector<int> istemp;
		std::vector<int> activetextures;
		int timer1,timer2,timer3,numloads;
		int maxsize;
		int txtenabled;
//      int makeNoiseTexture(int size) {TNoiseRGBTexture t(size,size); return addTexture(&t);} //Should not be used
		int addTexture(TTexture *tex, int asfont) {
			TTextureContainer cont; cont.makeFromTTexture(tex,asfont,maxsize);  timer2+=cont.duration1;
			timer3+=cont.duration2; Textures.push_back(cont); return (Textures.size()-1);
			} //Should not be used directly
	public:
		TTextureServer() : maxsize(512),txtenabled(-1) {};
		void DeactivateStage(int stage);
		void activate(int i,int stage);
		void activate(int i);
		void Reload();
		void EnableTexturing() {if (txtenabled!=1) {glEnable( GL_TEXTURE_2D ); txtenabled=1;}}
		void DisableTexturing() {if (txtenabled!=0) {glDisable( GL_TEXTURE_2D ); txtenabled=0;}}
		void Invalidate();
		void SetMaxTextureSize(int s) {maxsize=s;}
		int GetMaxTextureSize() {return maxsize;}
		TFontExtend GetFontExtend(int index,int num);
		void clear();
		int LoadTexture(TCuboFile *finfo,int asfont=0,unsigned int colorkey=0);
		int LoadTextureAndAlpha(TCuboFile *finfo,TCuboFile *afinfo);

		int LoadTempTexture(std::string tname, TCuboFile *finfo,int asfont=0,unsigned int colorkey=0); //For Preview-pics
		int TempTextureIndexFromName(std::string tname);
		void ResetTimerCounters();
		void CoutTimerString();
	};



extern void LUA_TEXDEF_RegisterLib();
extern void LUA_TEXTURE_RegisterLib();


#endif

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
