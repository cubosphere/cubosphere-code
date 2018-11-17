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

#include <iostream>
#include <vector>
#include <string>
#include <array>

#ifdef WIN32

//#include <windows.h>
//#include <GL\glew.h>
//#include <GL\gl.h>
//#include <SDL.h>
#include <SDL_ttf.h>

#else

//#include <GL/glew.h>
//#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#endif

class SizedFont {
	protected:
		int size;
		TTF_Font* font;
		void DestructFont();
	public:
		SizedFont() : size(0), font(NULL) {}
		~SizedFont() {DestructFont();}
		int Load(std::string fontname,int fontsize);
		int Load(SDL_RWops *rwops,int fontsize);
		int GetSize() {return size;}
		TTF_Font *GetFont() {return font;}
	};

constexpr std::array<int, 6> g_FontSizes = {12,14,18,24,32,64};

class LoadedFont {
	protected:
		std::string fname; //Name of the font
		std::vector<SizedFont*> sized; //Storing the sized fonts
		SizedFont* GetSized(int dessize);
		int Prepare(); ///TODO: Load the std sizes
	public:
		void Clear();
		LoadedFont() {Clear();}
		~LoadedFont() {Clear();}
		int Load(std::string fontname);
		SizedFont *GetBestFont(int pixelsize);
		std::string GetName() {return fname;}
	};

//Stores a Surface with a text
class FontCache {
	protected:
		int mysize,fontsize;
		std::string mytext;
		std::string myfontname;
		SDL_Surface *surf;
		GLuint texture;
		int initialized;
		void Clear();
		double TimeStamp; //Last used time
	public:
		int iw,ih,sw,sh; //Bad, but who cares
		int IsTheSame(std::string fname,std::string text,int size) {return ( (initialized==1) && (mysize==size) && (myfontname==fname) && (text==mytext));}
		FontCache() : initialized(0) {};
		~FontCache() {Clear();}
		void Setup(LoadedFont *font,std::string text, int size);
		void SetTime(double T) {TimeStamp=T;}
		double GetTime() {return TimeStamp;}
		GLuint GetTexture() {return texture;}
		float GetFontSize() {return fontsize;}

	};


constexpr unsigned int MAX_FONT_CACHE = 16;
class FontCaches {
	protected:
		std::vector<FontCache*> caches;

	public:
		void Clear();
		FontCaches() {Clear();}
		~FontCaches() {Clear();}
		FontCache *GetCache(LoadedFont *font,std::string text, int size);
	};

using FontRemap = struct {
	std::string oldc;
	std::string newc;
	};

class Font {
	protected:
		std::string valign; //top, center, bottom
		std::string halign; //left, center, right

		//TTF_Font* font;
		LoadedFont font;
		FontCaches cache;
		///OLD
		//   int tindex;

		std::vector <FontRemap> remaps;
		float xpos,ypos;
		float scalex;
		float scaley;
		//  void Character(int s);
		//  float TextHeight(string s); //Takes only a single line!
		//  float TextWidth(string s); //dito
		void RenderText(std::string text);
		std::string RemapString(std::string text);
		std::string cname;
	public:
		std::string GetFontName() {return cname;}
		void Init();
		void SetSize(float s);
		void TextOut(std::string s);
		void Load(std::string textname);
		void Test();
		void Begin();
		void Goto(float cx,float cy) {xpos=cx; ypos=cy;}
		void End();
		void SetAlign(std::string ha,std::string va) {halign=ha; valign=va;}
		void ClearRemaps();
		void AddRemap(std::string oldc,std::string newc);
		void ClearCache();
		void StopFontEngine();
		// ~TFont() {if (font) TTF_CloseFont(font);}
	};

extern void LUA_FONT_RegisterLib();

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
