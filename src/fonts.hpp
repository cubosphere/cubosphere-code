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
#include <string_view>
#include <array>

#include <SDL_ttf.h>

struct FontID {
	std::string fname;
	std::string text;
	int size;
	};

inline bool operator==(const FontID& a, const FontID& b) {
	return (a.fname == b.fname) and (a.text == b.text) and (a.size == b.size);
	}

struct FontHasher {
	std::size_t operator()(const FontID& k) const {
		using std::size_t;
		using std::hash;
		using std::string;

		size_t res = 17;
		res = res * 31 + hash<string>()( k.fname );
		res = res * 31 + hash<string>()( k.text );
		res = res * 31 + hash<int>()( k.size );
		return res;
		}
	};

class SizedFont {
	protected:
		int size;
		TTF_Font* font;
		std::shared_ptr<CuboFile> finfo; // Current loaded font
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
		std::unordered_map<int, std::shared_ptr<SizedFont>> sized; //Storing the sized fonts
		std::shared_ptr<SizedFont> GetSized(int dessize);
		int Prepare();
	public:
		void Clear();
		LoadedFont() {Clear();}
		~LoadedFont() {Clear();}
		int Load(std::string fontname);
		std::shared_ptr<SizedFont> GetBestFont(int pixelsize);
		std::string GetName() {return fname;}
	};

class Font;

//Stores a Surface with a text
class FontCache {
		friend Font;
	protected:
		int iw,ih,sw,sh;
		int mysize,fontsize;
		std::string mytext;
		std::string myfontname;
		SDL_Surface *surf;
		GLuint texture;
		int initialized;
		void Clear();
		double TimeStamp; //Last used time
	public:
		//int IsTheSame(std::string fname,std::string text,int size) {return ( (initialized==1) && (mysize==size) && (myfontname==fname) && (text==mytext));}
		FontCache() : initialized(0) {};
		~FontCache() {Clear();}
		void Setup(std::shared_ptr<LoadedFont>& font,std::string text, int size);
		void SetTime(double T) {TimeStamp=T;}
		double GetTime() {return TimeStamp;}
		GLuint GetTexture() {return texture;}
		float GetFontSize() {return fontsize;}
	};


constexpr unsigned int MAX_FONT_CACHE = 64;

class FontCaches {
	protected:
		std::unordered_map<FontID, std::unique_ptr<FontCache>, FontHasher> caches;

	public:
		void Clear();
		FontCaches() {Clear();}
		~FontCaches() {Clear();}
		std::unique_ptr<FontCache>& GetCache(std::shared_ptr<LoadedFont>& font,std::string text, int size);
	};

struct FontRemap {
	std::string oldc;
	std::string newc;
	};

class Font {
	protected:
		std::string valign; //top, center, bottom TODO: use enum?
		std::string halign; //left, center, right

		//TTF_Font* font;
		std::shared_ptr<LoadedFont> font;
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
		Font() { font = std::make_shared<LoadedFont>(); };
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
