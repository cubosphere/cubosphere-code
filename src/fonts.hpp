/**
Copyright (C) 2010 Chriddo

This program is free software;
you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program;
if not, see <http://www.gnu.org/licenses/>.
**/

#ifndef FONTS_H_G
#define FONTS_H_G


#include <iostream>
#include <vector>
#include <string>

#ifdef WIN32

//#include <windows.h>
//#include <GL\glew.h>
// #include <GL\gl.h>
//#include <SDL.h>
#include <SDL_ttf.h>

#else

//#include <GL/glew.h>
//#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#endif



using namespace std;

class TSizedFont
	{
	protected:
		int size;
		TTF_Font* font;
		void DestructFont();
	public:
		TSizedFont() : size(0), font(NULL) {}
		~TSizedFont() {DestructFont();}
		int Load(string fontname,int fontsize);
		int Load(SDL_RWops *rwops,int fontsize);
		int GetSize() {return size;}
		TTF_Font *GetFont() {return font;}
	};

#define NUM_FONT_SIZES 6
const int g_FontSizes[NUM_FONT_SIZES]= {12,14,18,24,32,64};

class TLoadedFont
	{
	protected:
		string fname; //Name of the font
		vector<TSizedFont*> sized; //Storing the sized fonts
		TSizedFont* GetSized(int dessize);
		int Prepare(); ///TODO: Load the std sizes
	public:
		void Clear();
		TLoadedFont() {Clear();}
		~TLoadedFont() {Clear();}
		int Load(string fontname);
		TSizedFont *GetBestFont(int pixelsize);
		string GetName() {return fname;}
	};

//Stores a Surface with a text
class TFontCache
	{
	protected:
		int mysize,fontsize;
		string mytext;
		string myfontname;
		SDL_Surface *surf;
		GLuint texture;
		int initialized;
		void Clear();
		double TimeStamp; //Last used time
	public:
		int iw,ih,sw,sh; //Bad, but who cares
		int IsTheSame(string fname,string text,int size) {return ( (initialized==1) && (mysize==size) && (myfontname==fname) && (text==mytext));}
		TFontCache() : initialized(0) {};
		~TFontCache() {Clear();}
		void Setup(TLoadedFont *font,string text, int size);
		void SetTime(double T) {TimeStamp=T;}
		double GetTime() {return TimeStamp;}
		GLuint GetTexture() {return texture;}
		float GetFontSize() {return fontsize;}

	};


#define MAX_FONT_CACHE 16
class TFontCaches
	{
	protected:
		vector<TFontCache*> caches;

	public:
		void Clear();
		TFontCaches() {Clear();}
		~TFontCaches() {Clear();}
		TFontCache *GetCache(TLoadedFont *font,string text, int size);
	};

typedef struct
	{
	string oldc;
	string newc;
	} TFontRemap;

class TFont
	{
	protected:
		string valign; //top, center, bottom
		string halign; //left, center, right

		//TTF_Font* font;
		TLoadedFont font;
		TFontCaches cache;
		///OLD
		//   int tindex;

		vector <TFontRemap> remaps;
		float xpos,ypos;
		float scalex;
		float scaley;
		//  void Character(int s);
		//  float TextHeight(string s); //Takes only a single line!
		//  float TextWidth(string s); //dito
		void RenderText(string text);
		string RemapString(string text);
		string cname;
	public:
		string GetFontName() {return cname;}
		void Init();
		void SetSize(float s);
		void TextOut(string s);
		void Load(string textname);
		void Test();
		void Begin();
		void Goto(float cx,float cy) {xpos=cx; ypos=cy;}
		void End();
		void SetAlign(string ha,string va) {halign=ha; valign=va;}
		void ClearRemaps();
		void AddRemap(string oldc,string newc);
		void ClearCache();
		void StopFontEngine();
		// ~TFont() {if (font) TTF_CloseFont(font);}
	};

extern void LUA_FONT_RegisterLib();

#endif
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
