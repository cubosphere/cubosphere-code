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

#include "posteffects.hpp"
#include "globals.hpp"
#include "luautils.hpp"
#include "game.hpp"

PostEffect *gPostEffect=NULL;
PostEffect *g_PostEffect() {return gPostEffect;}

void LoadPostEffect(std::string name) {
	if (gPostEffect) { delete gPostEffect; }
	gPostEffect=NULL;
	if (name=="") { return; }
	gPostEffect=new PostEffect();
	gPostEffect->SetName(name);
	gPostEffect->LoadDef();
	}

void PostEffect::CallDefaultSpecialRender(std::string nam,std::string what,int index) {
	if (!isprecached) { Precache(); }
	if (lua.FuncExists("DefaultSpecialRender")) {

			lua.CallVA("DefaultSpecialRender","ssi",nam.c_str(),what.c_str(),index);
			}
	}

void PostEffect::CallRender() {
	if (!isprecached) { Precache(); }
	if (lua.FuncExists("Render")) {

			lua.CallVA("Render","");
			}
	}

void PostEffect::Precache() {
	isprecached=1;
	if (lua.FuncExists("Precache")) {
			lua.CallVA("Precache","");
			}

	}

int PostEffect::CreateTempTexture(int w, int h,int withdepth) {

	if (!isprecached) { Precache(); }
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,  GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,  GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h,  0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
	///TODO: Set Active Texture
	glBindTexture(GL_TEXTURE_2D, 0);
	std::cout << "Currently CreateTempTexture not supported" << std::endl;
	GLuint depth=0;
	if (withdepth) {
			/* glGenRenderbuffers(1, &depth);
			 glBindRenderbuffer(GL_RENDERBUFFER, depth);
			 glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
			 glBindRenderbuffer(GL_RENDERBUFFER, 0);
			*/
			glGenTextures(1, &depth);
			glBindTexture(GL_TEXTURE_2D, depth);
			glTexImage2D (GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24,w, h, 0,GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);


			}

	GLuint nFBO;
	glGenFramebuffers(1, &nFBO);
	glBindFramebuffer(GL_FRAMEBUFFER,nFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	if (withdepth) {
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
					GL_TEXTURE_2D, depth, 0);
			//   glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth);
			}
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	/*
		data = (GLuint *)calloc( 1, ((w * h)* 4 * sizeof(GLuint)) );

		glGenTextures(1, &txtnumber);
		glBindTexture(GL_TEXTURE_2D, txtnumber);
		glTexImage2D(GL_TEXTURE_2D, 0, 4, w, h, 0,	GL_RGBA, GL_UNSIGNED_BYTE, data);

	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

		free( data );
	*/
	TempTexture nt;
	nt.fbo=nFBO;
	nt.tindex=texture;
	nt.w=w;
	nt.h=h;
	nt.depth=depth;

	ttexts.push_back(nt);
	return ttexts.size()-1;

	}

void PostEffect::SetRenderTarget(int index) {
	if (!isprecached) { Precache(); }
	if ((index<0) || (index>=(int)(ttexts.size()))) { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
	else { glBindFramebuffer(GL_FRAMEBUFFER, ttexts[index].fbo); }
	}


void PostEffect::UnPrecache() {
	isprecached=0;
	for (unsigned int i=0; i<ttexts.size(); i++) {
			glDeleteFramebuffers(1,&(ttexts[i].fbo));
			glDeleteTextures(1,&(ttexts[i].tindex));
			//  if (ttexts[i].depth) glDeleteRenderbuffers(1,&(ttexts[i].depth));
			if (ttexts[i].depth) { glDeleteTextures(1,&(ttexts[i].depth)); }
			}
	ttexts.resize(0);

	}

PostEffect::~PostEffect() {
	UnPrecache();
	}

void PostEffect::DrawQuad() {
	if (!isprecached) { Precache(); }
// glBegin(GL_QUADS);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2f(0,0); glVertex2f(-0.5, -0.5);
	glTexCoord2f(1,0); glVertex2f(0.5, -0.5);

	glTexCoord2f(0,1); glVertex2f(-0.5, 0.5);
	glTexCoord2f(1,1); glVertex2f(0.5, 0.5);
	glEnd();
	}


void PostEffect::DepthToStage(int ttex,int stage) {
	if (!isprecached) { Precache(); }
	if ((ttex<0) || (ttex>=(int)(ttexts.size()))) { return; }

	std::cout << "Temp Depth to Stage not defined" << std::endl;
///TODO: Set the active Texture g_lastActiveTexture
	// if (g_Game()->HasGLSL())
	//glActiveTexture(GL_TEXTURE0+stage);
	//glBindTexture( GL_TEXTURE_2D, ttexts[ttex].depth);
	}

void PostEffect::TempTextureToStage(int ttex,int stage) {
	if (!isprecached) { Precache(); }
	if ((ttex<0) || (ttex>=(int)(ttexts.size()))) { return; }
	std::cout << "Temp Texture to Stage not defined" << std::endl;
///TODO: Set the active Texture g_lastActiveTexture
	//if (g_Game()->HasGLSL())
//    glActiveTexture(GL_TEXTURE0+stage);
	//  glBindTexture( GL_TEXTURE_2D, ttexts[ttex].tindex );
	}



/////////////LUA IMPLEMENT /////////////////7


int EFFECT_RenderPass(lua_State *state) {
	g_Game()->RenderPass();
	return 0;
	}

int EFFECT_SpecialRenderPass(lua_State *state) {
	int defaultrender=LUA_GET_INT(state);
	std::string nam=LUA_GET_STRING(state);

	g_Game()->SpecialRenderPass(nam,defaultrender);
	return 0;
	}


int EFFECT_DoRenderPart(lua_State *state) {
	int index=LUA_GET_INT(state);
	std::string tname=LUA_GET_STRING(state);
	if (tname=="sky") {g_Game()->GetSky()->Render(); }
	else if (tname=="item") {g_Game()->GetLevel()->GetItem(index)->Render();}
	else if (tname=="side") {g_Game()->GetLevel()->GetBlockSide(index)->Render();}
	else if (tname=="actor") {g_Game()->GetActorMovement(index)->Render();}
	return 0;
	}


int EFFECT_CreateTempTexture(lua_State *state) {
	int wdepth=LUA_GET_INT(state);
	int h=LUA_GET_INT(state);
	int w=LUA_GET_INT(state);
	if (g_PostEffect()) { w=g_PostEffect()->CreateTempTexture(w,h,wdepth); }
	LUA_SET_NUMBER(state, w);
	return 1;
	}


int EFFECT_SetRenderTarget(lua_State *state) {
	int i=LUA_GET_INT(state);
	if (g_PostEffect()) { g_PostEffect()->SetRenderTarget(i); }
	return 0;
	}

int EFFECT_TempTextureToStage(lua_State *state) {
	int stage=LUA_GET_INT(state);
	int tex=LUA_GET_INT(state);
	if (g_PostEffect()) { g_PostEffect()->TempTextureToStage(tex,stage); }
	return 0;
	}

int EFFECT_DepthToStage(lua_State *state) {
	int stage=LUA_GET_INT(state);
	int tex=LUA_GET_INT(state);
	if (g_PostEffect()) { g_PostEffect()->DepthToStage(tex,stage); }
	return 0;
	}

int EFFECT_DrawQuad(lua_State *state) {
	if (g_PostEffect()) { g_PostEffect()->DrawQuad(); }
	return 0;
	}


int EFFECT_Load(lua_State *state) {
	std::string name=LUA_GET_STRING(state);
	LoadPostEffect(name);
	return 0;
	}



void LUA_EFFECT_RegisterLib() {
	g_CuboLib()->AddFunc("EFFECT_Load",EFFECT_Load);
	g_CuboLib()->AddFunc("EFFECT_RenderPass",EFFECT_RenderPass);
	g_CuboLib()->AddFunc("EFFECT_SpecialRenderPass",EFFECT_SpecialRenderPass);
	g_CuboLib()->AddFunc("EFFECT_CreateTempTexture",EFFECT_CreateTempTexture);
	g_CuboLib()->AddFunc("EFFECT_SetRenderTarget",EFFECT_SetRenderTarget);
	g_CuboLib()->AddFunc("EFFECT_TempTextureToStage",EFFECT_TempTextureToStage);
	g_CuboLib()->AddFunc("EFFECT_DepthToStage",EFFECT_DepthToStage);
	g_CuboLib()->AddFunc("EFFECT_DrawQuad",EFFECT_DrawQuad);
	g_CuboLib()->AddFunc("EFFECT_DoRenderPart",EFFECT_DoRenderPart);
	}
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
