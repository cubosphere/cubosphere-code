#ifndef POSTEFFECTS_H_G
#define POSTEFFECTS_H_G


#include "definitions.hpp"
#include <vector>

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

typedef struct
	{
	GLuint tindex;
	GLuint fbo;
	GLuint depth;
	int w,h;
	} TTempTexture;

class TPostEffect : public TBaseLuaDef
	{
	protected:
		//TLuaAccess lua;
		std::vector<TTempTexture> ttexts;
		int isprecached;
		//virtual int SendIDWhenPrecache() {return -1;}
	public:
		TPostEffect() : isprecached(0) {}
		int GetType() {return FILE_POSTEFFECTDEF;}
		virtual ~TPostEffect();
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


extern TPostEffect *g_PostEffect();
extern void LoadPostEffect(std::string name);

extern void LUA_EFFECT_RegisterLib();

#endif
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
