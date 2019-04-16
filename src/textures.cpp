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

#include <csetjmp>
#include "textures.hpp"
#include "globals.hpp"

#include <stdio.h>
#include <iostream>
#include <stdlib.h>

#include <sstream>

#include <string.h>

#include <GL/glew.h>
#include <SDL.h>

#include "game.hpp"
#include "luautils.hpp"

using namespace std::string_view_literals;

///////////////////////////////////////////////

inline TextureType extToTexType(std::string_view ext) {
	if (ext == "jpg"sv or ext == "jpeg"sv) { return TextureType::JPEG; }
	else if (ext == "png"sv) { return TextureType::PNG; }
	else { return TextureType::Invalid; }
	}

inline std::unique_ptr<CuboFile> getTextrueFile(std::string subname,int type) {
	auto finfo=GetFileName(subname, type,".png"); // Try PNG first
	if (!finfo) { finfo=GetFileName(subname, type,".jpg"); }
	if (!finfo) { finfo=GetFileName(subname, type,".jpeg"); }
	return finfo;
	}

void TextureDef::Call_Render(int sideid) {
	lua.CallVAIfPresent("Render", {{sideid}});
	}

void TextureDef::RenderPlane() {
//OPEN GL STUFF
// glBegin(GL_QUADS);
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(0,1,0);
	glTexCoord2f(0,0); glVertex3f(-0.5,0,-0.5);
	glTexCoord2f(0,1); glVertex3f(-0.5,0,0.5);

	glTexCoord2f(1,0); glVertex3f(0.5,0,-0.5);
	glTexCoord2f(1,1); glVertex3f(0.5,0,0.5);
	glEnd();
	}

void TextureDef::Render2d() {
//OPEN GL STUFF
//glBegin(GL_QUADS);
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(0,0,-1);
	glTexCoord2f(0,1); glVertex3f(-0.5, -0.5, -10);
	glTexCoord2f(1,1); glVertex3f(0.5, -0.5, -10);
	glTexCoord2f(0,0); glVertex3f(-0.5, 0.5, -10);
	glTexCoord2f(1,0); glVertex3f(0.5, 0.5, -10);

	glEnd();


	}

///////////////////////////////////////////////

bool SDLTexture::loadFromFile(const std::unique_ptr<CuboFile>& finfo) {
	if (img) {SDL_FreeSurface(img); img = nullptr;}
	SDL_Surface* tmpimg;
	if (finfo->IsHDDFile()) {
			tmpimg = IMG_Load(finfo->GetHDDName().c_str());
			}
	else {
			tmpimg = IMG_Load_RW(finfo->GetAsRWops(), 1);
			}
	if (not tmpimg) {
			std::cout << "IMG_Load failed: " << SDL_GetError() << std::endl;
			return false;
			}
	img = SDL_ConvertSurfaceFormat(tmpimg, SDL_PIXELFORMAT_ABGR8888, 0);
	SDL_FreeSurface(tmpimg);
	if (not img) { std::cout << "SDL_ConvertSurfaceFormat failed: " << SDL_GetError() << std::endl; }
	width = img->w;
	height = img->h;
	return img;
	}

///////////////////////////////////////////////

/* Read JPEG image from a memory segment */


static int isPowerOf2(int n) {
	return (n & (n - 1)) == 0 && n != 0;
	}

static uint32_t myLog2(uint32_t x) {
	uint32_t y;
	asm ( "\tbsr %1, %0\n"
			: "=r"(y)
			: "r" (x)
		);
	return y;
	}

#define GL_TEXTURE_MAX_ANISOTROPY_EXT 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT 0x84FF

void TextureContainer::AddChar(int x,int y,void *data,int width,int fsize) {


	int max=width/fsize;
	FontExtend fe;
	fe.u1=(float)(x/fsize)/(float)max;
	fe.v1=(float)(y/fsize)/(float)max;
	fe.v2=(float)(y/fsize+0.99)/(float)max;




	float frac=0.99;

	for (int cx=fsize-2; cx>0; cx--) {
			int found=0;
			for (int cy=0; cy<fsize; cy++) {
					int p=(y+cy)*width+(x+cx);

					unsigned char a=((unsigned char *)data)[4*p+3];
					if (a) {
							frac=(float)(cx+1)/(float)fsize;
							found=1;
							break;
							}

					}
			if (found) { break; }
			}


	fe.u2=(float)(x/fsize+frac)/(float)max;

	chars.push_back(fe);
	}

void TextureContainer::makeFromTexture(Texture* texture,int asfont,int maxsize) {
	int clo=clock();
	int canfastresize=texture->CanFastResize(maxsize) and (!asfont);
	if (canfastresize) {
			texture->FastResize(maxsize);

			}
	void *data=texture->getRGBPointer();

	int width=texture->getWidth();
	int height=texture->getHeight();
	int scaled=0;
	if (  ((width>maxsize) || (height>maxsize)) && (!asfont) && (!canfastresize)) {
			//Rescale it
			int owidth=width;
			int oheight=height;
			GLenum type=GL_RGB;
			void *newdata;
			if (texture->HasAlpha()) { type=GL_RGBA; }
			if (width>maxsize) { width=maxsize; }
			if (height>maxsize) { height=maxsize; }
			newdata=malloc(width*height*(3+texture->HasAlpha()));
			gluScaleImage(type,owidth,oheight,GL_UNSIGNED_BYTE,data,width,height,GL_UNSIGNED_BYTE,newdata);
			data=newdata;
			scaled=1;
			}
	duration1=clock()-clo;


	if (asfont) {
			//Building the font with (asfont x asfont) sized letters
			//First the simplest approach:
			int maxy=texture->getHeight()/asfont;
			int maxx=texture->getWidth()/asfont;
			for (int y=0; y<maxy; y++) {
					for (int x=0; x<maxx; x++) {
							AddChar(x*asfont,y*asfont,data,texture->getWidth(),asfont);
							}
					}
			}

	glGenTextures( 1, &tind );

	glBindTexture( GL_TEXTURE_2D, tind );
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);
//ANIS
	if (strstr((char*)glGetString(GL_EXTENSIONS),
			"GL_EXT_texture_filter_anisotropic")) {
			float maxAnisotropy;
			glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
			}

	clo=clock();

	if (!asfont) { // No Mipmaps for fonts
			if (!texture->HasAlpha())
				gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width,height,
						GL_RGB, GL_UNSIGNED_BYTE, data );
			else
				gluBuild2DMipmaps( GL_TEXTURE_2D, 4, width,height,
						GL_RGBA, GL_UNSIGNED_BYTE, data );
			}
	else {
			glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, width,height, 0, GL_RGB,GL_UNSIGNED_BYTE, data);
			}
	duration2=clock()-clo;
	if (scaled) { free(data); }
	}


void TextureContainer::activate() {


	glBindTexture( GL_TEXTURE_2D, tind );
	}

void TextureContainer::activate(int stage) {
	if (g_Game()->HasGLSL() && (stage>0)) {
			glActiveTexture(GL_TEXTURE0+stage);
			}

	glBindTexture( GL_TEXTURE_2D, tind );

	if (g_Game()->HasGLSL() && (stage>0)) {
			glActiveTexture(GL_TEXTURE0);
			}
	}


void TextureServer::Invalidate() {
	for (unsigned int i=0; i<activetextures.size(); i++) { activetextures[i]=-2333; }
	}

void TextureServer::activate(int i) {
	if (activetextures.size()==0) { activetextures.push_back(i); }
	else if  (activetextures[0]==i) { return; }
	if (i<0) { DeactivateStage(0); }
	else { Textures[i].activate(); }
	}
void TextureServer::activate(int i,int stage) {
	if ((int)(activetextures.size())<=stage) { activetextures.resize(stage+1,-23255); }
	if  (activetextures[0]==i) { return; }

	if (i<0) { DeactivateStage(stage); }
	else { Textures[i].activate(stage); }
	}

FontExtend TextureServer::GetFontExtend(int index,int num) {
	return Textures[index].GetFontExtend(num);
	}

int TextureServer::TempTextureIndexFromName(std::string tname) {
	for (unsigned int i=0; i<filenames.size(); i++) if ((tname==filenames[i]) && (istemp[i]) ) { return i; }
	return -1;

	}

int TextureServer::LoadTempTexture(std::string tname, const std::unique_ptr<CuboFile>& finfo,int asfont,unsigned int colorkey) {
	int index=TempTextureIndexFromName(tname);
	TextureContainer *cont;
	if (index==-1) {
			Textures.resize(Textures.size()+1);
			index=Textures.size()-1;
			filenames.push_back(tname);
			alphanames.push_back(finfo->GetName());
			istemp.push_back(1);
			}
	else { //Load texture into existing sloz
			//Free the texture in this slot
			Textures[index].clear();
			}
	cont=&Textures[index];


	Texture *tex=NULL;
	std::string ext;
	std::string fname=finfo->GetName();
	auto extind = fname.rfind('.');
	if (extind != std::string::npos) {
			ext=fname.substr(extind+1);
			}
	else { ext=""; }
	auto texType = extToTexType(ext);
	switch(texType) {
			case TextureType::JPEG:
				tex = new SDLTexture;
				break;
			case TextureType::PNG:
				tex = new SDLTexture;
				break;
			default:
				coutlog("Unknown texture type for "+fname+"\n", 2);
				return -1;
			};

	if (!tex) { coutlog("Failed to load "+fname+"\n", 2); return -1;}
	if (!tex->loadFromFile(finfo)) { coutlog("Failed to load "+fname+"\n", 2); return -1;}
	//if (colorkey && texType == TextureType::JPEG) {
	//		((JPEGTexture *)tex)->ColorKeyTransparency(colorkey);
	//		}

	cont->makeFromTexture(tex,asfont,maxsize);

	delete tex;

	return index;
	}

/*
int TextureServer::LoadTextureAndAlpha(const std::unique_ptr<CuboFile>& finfo,std::unique_ptr<CuboFile>& finfoa) { // JPEG only, deprecated
	std::string fname=finfo->GetName();
	std::string aname=finfoa->GetName();
	for (unsigned int i=0; i<filenames.size(); i++) if (fname==filenames[i] && aname==alphanames[i]) { return i; }
	if (g_VerboseMode()) {coutlog("Loading Texture: "+finfo->GetNameForLog());   coutlog("  -> using AlphaTexture: "+finfoa->GetNameForLog());}
//Find the right container Type
	Texture *tex=NULL;
	std::string ext;
	auto extind = fname.rfind('.');
	if (extind != std::string::npos) {
			ext=fname.substr(extind+1);
			}
	else { ext=""; }
	if (ext=="jpg" || ext=="jpeg") { tex=new JPEGTexture; }
	if (!tex) { std::ostringstream os; os << "Cannot load Texture "<< fname << std::endl; coutlog(os.str(),2); return -1;}
	int clo=clock();
	tex->loadFromFile(finfo);
	if (!(((JPEGTexture *)(tex))->LoadAlphaTexture(finfoa))) { std::ostringstream os; os << "Cannot load Alpha-Texture "<< aname << std::endl; coutlog(os.str(),2); delete tex; return -1;}
	timer1+=clock()-clo;
	int res=addTexture(tex,0);
	delete tex;
	numloads++;
	filenames.push_back(fname);
	alphanames.push_back(aname);
	istemp.push_back(0);
	return res;

	}*/

int TextureServer::LoadTexture(const std::unique_ptr<CuboFile>& finfo,int asfont,unsigned int colorkey) {
//Is it loaded already?
	std::string s=finfo->GetName();
	for (unsigned int i=0; i<filenames.size(); i++) if (s==filenames[i] && alphanames[i]=="") { return i; }
	if (g_VerboseMode()) { coutlog("Loading Texture: "+finfo->GetNameForLog()); }
//Find the right container Type
	Texture *tex=NULL;
	std::string ext;
	auto extind = s.rfind('.');
	if (extind != std::string::npos) {
			ext=s.substr(extind+1);
			}
	else { ext=""; }
	auto texType = extToTexType(ext);
	switch(texType) {
			case TextureType::JPEG:
				tex = new SDLTexture;
				break;
			case TextureType::PNG:
				tex = new SDLTexture;
				break;
			default:
				coutlog("Unknown texture type for "+s+"\n", 2);
				return -1;
			};

	if (!tex) { std::ostringstream os; os << "Cannot load Texture "<< s << std::endl; coutlog(os.str(),2); return -1;}
	int clo=clock();
	tex->loadFromFile(finfo);
	timer1+=clock()-clo;
	//if (colorkey && texType == TextureType::JPEG) {
	//		((JPEGTexture *)tex)->ColorKeyTransparency(colorkey);
	//		}
	int res=addTexture(tex,asfont);

	delete tex;

	numloads++;
	filenames.push_back(s);
	alphanames.push_back("");
	istemp.push_back(0);

	return res;

	}

void TextureServer::Reload() {
	for (unsigned int i=0; i<activetextures.size(); i++) { activate(-1,i); }

	for (unsigned int i=0; i<Textures.size(); i++) {
			Textures[i].clear();
			}

	Textures.clear();

	std::vector<std::string> ofilenames;
	for (unsigned int i=0; i<filenames.size(); i++) { ofilenames.push_back(filenames[i]); }
	filenames.clear();

	for (unsigned int i=0; i<ofilenames.size(); i++) {

			std::unique_ptr<CuboFile> finfo;

			std::string nam;
			if (istemp[i]) {
					nam=alphanames[i];
					}
			else {
					nam=ofilenames[i];
					}
			finfo=g_BaseFileSystem()->GetFileForReading(nam);



			if (!finfo) { coutlog("Texture "+nam+ " not found!",2); Textures.push_back(TextureContainer()); filenames.push_back(nam);  continue;}



			if (alphanames[i]=="" && (!istemp[i])) {
					LoadTexture(finfo,false);
					}
			else if (!istemp[i]) {
					std::unique_ptr<CuboFile> finfoa=g_BaseFileSystem()->GetFileForReading(alphanames[i]);
					if (!finfoa) {coutlog("Alpha Texture "+alphanames[i]+" not found!",2); Textures.push_back(TextureContainer()); filenames.push_back(ofilenames[i]);  continue; }
					//LoadTextureAndAlpha(finfo,finfoa);
					}
			else {
					LoadTempTexture(ofilenames[i],finfo);
					}
			alphanames.pop_back();
			istemp.pop_back();

			//ostringstream oss; oss << "Storing texture " << i << " at " << r << "  fname :"  << filenames[i];
			//coutlog(oss.str(),2);

			}
	}

void TextureServer::clear() {

	for (unsigned int i=0; i<Textures.size(); i++) { Textures[i].clear(); }
	activetextures.clear();
	Textures.clear();
	filenames.clear();
	alphanames.clear();
	istemp.clear();
	ResetTimerCounters();
	}


void TextureServer::DeactivateStage(int stage) {
	if (g_Game()->HasGLSL() && (stage>0)) {
			glActiveTexture(GL_TEXTURE0+stage);
			}
	glBindTexture( GL_TEXTURE_2D, 0 );
	if (g_Game()->HasGLSL() && (stage>0)) {
			glActiveTexture(GL_TEXTURE0);
			}
	}


void TextureServer::ResetTimerCounters() {
	numloads=timer1=timer2=timer3=0;
	}

void TextureServer::CoutTimerString() {
	float t1=timer1/(float)CLOCKS_PER_SEC;
	float t2=timer2/(float)CLOCKS_PER_SEC;
	float t3=timer3/(float)CLOCKS_PER_SEC;
	std::ostringstream os;
	os << "Texture loading times: Loaded "<<numloads << " textures. " << std::endl;
	os <<"             Durations: Loading files (" << t1 << " s)" << std::endl;
	os <<"                        Resizing (" << t2 << " s)" << std::endl;
	os <<"                        Uploading/Mipmaps (" << t3 << " s)" << std::endl;
	coutlog(os.str());
	}




////////////LUA-IMPLEMENTATION///////////////////////////


int TEXDEF_Load(lua_State *state) {

	std::string name = LUA_GET_STRING(state);


	int r=g_Game()->GetLevel()->LoadTexDef(name);


	LUA_SET_NUMBER(state, r);

	return 1;
	}

int _TEXTDEF_ResetMeanwhile=0;

int TEXDEF_Render(lua_State *state) {
	int side= LUA_GET_INT(state);
	int tind= LUA_GET_INT(state);
	_TEXTDEF_ResetMeanwhile=1;
	g_Game()->GetLevel()->GetTexDef(tind)->Call_Render(side);
	if (_TEXTDEF_ResetMeanwhile==1)    {g_Game()->GetLevel()->SetLastRendered(g_Game()->GetLevel()->GetTexDef(tind)->GetName());}
	else {  _TEXTDEF_ResetMeanwhile=0;  g_Game()->GetLevel()->SetLastRendered(""); }

// g_Game()->GetLevel()->SetLastRendered("");
	return 0;
	}

int TEXDEF_RenderDirect(lua_State *state) {
	int tind= LUA_GET_INT(state);
	g_Game()->GetLevel()->GetTexDef(tind)->RenderPlane();
	return 0;
	}

int TEXDEF_Render2d(lua_State *state) {
	int tind= LUA_GET_INT(state);
	g_Game()->GetLevel()->GetTexDef(tind)->Render2d();
	return 0;
	}


int TEXDEF_GetLastRenderedType(lua_State *state) {
	std::string tn=g_Game()->GetLevel()->GetLastRendered();
	LUA_SET_STRING(state, tn);
	return 1;
	}

int TEXDEF_ResetLastRenderedType([[maybe_unused]] lua_State *state) {
	g_Game()->GetLevel()->SetLastRendered("");
	if (_TEXTDEF_ResetMeanwhile) { _TEXTDEF_ResetMeanwhile=2; }
	return 0;
	}


void LUA_TEXDEF_RegisterLib() {
	g_CuboLib()->AddFunc("TEXDEF_Load",TEXDEF_Load);
	g_CuboLib()->AddFunc("TEXDEF_Render",TEXDEF_Render);
	g_CuboLib()->AddFunc("TEXDEF_RenderDirect",TEXDEF_RenderDirect);
	g_CuboLib()->AddFunc("TEXDEF_Render2d",TEXDEF_Render2d);
	g_CuboLib()->AddFunc("TEXDEF_GetLastRenderedType",TEXDEF_GetLastRenderedType);
	g_CuboLib()->AddFunc("TEXDEF_ResetLastRenderedType",TEXDEF_ResetLastRenderedType);
	}



////////////////////////////////////////////////////////

int TEXTURE_LoadSkyTexture(lua_State *state) {
	std::string name = LUA_GET_STRING(state);
	auto finfo=getTextrueFile(name,FILE_SKYBOX);
	if (!finfo) {coutlog("SkyTexture "+name+ " not found!",2); LUA_SET_NUMBER(state, -1); return 1;}
	int r=g_Game()->GetTextures()->LoadTexture(finfo,false);
	LUA_SET_NUMBER(state, r);
	return 1;
	}

int TEXTURE_SetClamp(lua_State *state) {
	int mode=LUA_GET_INT(state);
	int m;
	if (mode==2) { m=GL_CLAMP_TO_EDGE; }
	else if (mode==1) { m=GL_CLAMP; }
	else { m=GL_REPEAT; }
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m);
	return 0;
	}


int TEXTURE_Load(lua_State *state) {
	//string name = lua_tostring(state, -1);
	//lua_pop(state,1);
	std::string name = LUA_GET_STRING(state);

	auto finfo=getTextrueFile(name,FILE_TEXTURE);
// coutlog("Loading Texture "+Texturename);
	if(finfo) {
			int r=g_Game()->GetTextures()->LoadTexture(finfo,false);
			LUA_SET_NUMBER(state, r);
			}
	else {
			LUA_SET_NUMBER(state, -1);
			}

	return 1;
	}

/*
int TEXTURE_LoadWithAlpha(lua_State *state) {
	std::string aname = LUA_GET_STRING(state);
	std::string name = LUA_GET_STRING(state);

	auto finfo=GetFileName(name,FILE_TEXTURE,".jpg");
	if (!finfo) {coutlog("Texture "+name+ ".jpg not found!",2); LUA_SET_NUMBER(state, -1); return 1;}
	auto finfoa=GetFileName(aname,FILE_TEXTURE,".jpg");
	if (!finfoa) {coutlog("Alpha Texture "+aname+ ".jpg not found!",2); LUA_SET_NUMBER(state, -1); return 1;}

	int r=g_Game()->GetTextures()->LoadTextureAndAlpha(finfo,finfoa);
	LUA_SET_NUMBER(state, r);

	return 1;
	}*/

int TEXTURE_GetTempTexture(lua_State *state) {
	std::string tname=LUA_GET_STRING(state);
	int r=g_Game()->GetTextures()->TempTextureIndexFromName(tname);
	LUA_SET_NUMBER(state, r);
	return 1;
	}

int TEXTURE_LoadTempTexture(lua_State *state) {
	//string name = lua_tostring(state, -1);
	//lua_pop(state,1);

	std::string fname = LUA_GET_STRING(state);
	std::string tname= LUA_GET_STRING(state);
	auto cf=GetCuboFileFromRelativeName(fname);
	int r=0;
	if (cf) {
			r=g_Game()->GetTextures()->LoadTempTexture(tname,cf,false);
			}
	LUA_SET_NUMBER(state, r);

	return 1;
	}


int TEXTURE_SetMaxSize(lua_State *state) {
	int s=LUA_GET_INT(state);
	g_Game()->GetTextures()->SetMaxTextureSize(s);
	return 0;
	}


int TEXTURE_GetMaxSize(lua_State *state) {
	LUA_SET_NUMBER(state, g_Game()->GetTextures()->GetMaxTextureSize());
	return 1;
	}

int TEXTURE_Activate(lua_State *state) {
	int stage=LUA_GET_INT(state);
	int txt=LUA_GET_INT(state);
	g_Game()->GetTextures()->EnableTexturing();
	g_Game()->GetTextures()->activate(txt,stage);
	return 0;
	}

int TEXTURE_MatrixMode(lua_State *state) {
	int onoff=LUA_GET_INT(state);

	if (onoff) { glMatrixMode(GL_TEXTURE); }
	else { glMatrixMode(GL_MODELVIEW); }
	return 0;
	}

int TEXTURE_Deactivate(lua_State *state) {
	int stage=LUA_GET_INT(state);
//g_Game()->GetTextures()->EnableTexturing();
	g_Game()->GetTextures()->activate(-1,stage);
	return 0;
	}

void LUA_TEXTURE_RegisterLib() {
	g_CuboLib()->AddFunc("TEXTURE_Load",TEXTURE_Load);
	//g_CuboLib()->AddFunc("TEXTURE_LoadWithAlpha",TEXTURE_LoadWithAlpha);
	g_CuboLib()->AddFunc("TEXTURE_Activate",TEXTURE_Activate);
	g_CuboLib()->AddFunc("TEXTURE_Deactivate",TEXTURE_Deactivate);
	g_CuboLib()->AddFunc("TEXTURE_SetMaxSize",TEXTURE_SetMaxSize);
	g_CuboLib()->AddFunc("TEXTURE_GetMaxSize",TEXTURE_GetMaxSize);
	g_CuboLib()->AddFunc("TEXTURE_LoadTempTexture",TEXTURE_LoadTempTexture);
	g_CuboLib()->AddFunc("TEXTURE_GetTempTexture",TEXTURE_GetTempTexture);
	g_CuboLib()->AddFunc("TEXTURE_MatrixMode",TEXTURE_MatrixMode);
	g_CuboLib()->AddFunc("TEXTURE_SetClamp",TEXTURE_SetClamp);
	g_CuboLib()->AddFunc("TEXTURE_LoadSkyTexture",TEXTURE_LoadSkyTexture);
	}
