#include "posteffects.hpp"
#include "globals.hpp"
#include "luautils.hpp"
#include "game.hpp"

using namespace std;

TPostEffect *gPostEffect=NULL;
TPostEffect *g_PostEffect() {return gPostEffect;}

void LoadPostEffect(string name)
{
  if (gPostEffect) delete gPostEffect;
  gPostEffect=NULL;
  if (name=="") return;
  gPostEffect=new TPostEffect();
  gPostEffect->SetName(name);
  gPostEffect->LoadDef();
}

void TPostEffect::CallDefaultSpecialRender(string nam,string what,int index)
{
       if (!isprecached) Precache();
    if (lua.FuncExists("DefaultSpecialRender"))
  {

   lua.CallVA("DefaultSpecialRender","ssi",nam.c_str(),what.c_str(),index);
  }
}

void TPostEffect::CallRender()
{
    if (!isprecached) Precache();
if (lua.FuncExists("Render"))
  {

   lua.CallVA("Render","");
  }
}

void TPostEffect::Precache()
{
       isprecached=1;
if (lua.FuncExists("Precache"))
  {
   lua.CallVA("Precache","");
  }

}

int TPostEffect::CreateTempTexture(int w, int h,int withdepth)
{

      if (!isprecached) Precache();
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,  GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,  GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA , w, h,  0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
  ///TODO: Set Active Texture
  glBindTexture(GL_TEXTURE_2D, 0);
cout << "Currently CreateTempTexture not supported" << endl;
  GLuint depth=0;
  if (withdepth)
  {
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
    TTempTexture nt;
    nt.fbo=nFBO;
    nt.tindex=texture;
    nt.w=w;
    nt.h=h;
    nt.depth=depth;

    ttexts.push_back(nt);
    return ttexts.size()-1;

}

void TPostEffect::SetRenderTarget(int index)
{
        if (!isprecached) Precache();
 if ((index<0) || (index>=(int)(ttexts.size()))) glBindFramebuffer(GL_FRAMEBUFFER, 0);
 else glBindFramebuffer(GL_FRAMEBUFFER, ttexts[index].fbo);
}


void TPostEffect::UnPrecache()
{
 isprecached=0;
  for (unsigned int i=0;i<ttexts.size();i++)
    {
         glDeleteFramebuffers(1,&(ttexts[i].fbo));
       glDeleteTextures(1,&(ttexts[i].tindex));
     //  if (ttexts[i].depth) glDeleteRenderbuffers(1,&(ttexts[i].depth));
       if (ttexts[i].depth) glDeleteTextures(1,&(ttexts[i].depth));
    }
    ttexts.resize(0);

}

TPostEffect::~TPostEffect()
{ UnPrecache();
}

void TPostEffect::DrawQuad()
{
       if (!isprecached) Precache();
// glBegin(GL_QUADS);
glBegin(GL_TRIANGLE_STRIP);
 glTexCoord2f(0,0); glVertex2f(-0.5, -0.5);
 glTexCoord2f(1,0); glVertex2f(0.5, -0.5);

 glTexCoord2f(0,1); glVertex2f(-0.5, 0.5);
  glTexCoord2f(1,1); glVertex2f(0.5, 0.5);
 glEnd();
}


void TPostEffect::DepthToStage(int ttex,int stage)
{
       if (!isprecached) Precache();
 if ((ttex<0) || (ttex>=(int)(ttexts.size()))) return;

cout << "Temp Depth to Stage not defined" << endl;
///TODO: Set the active Texture g_lastActiveTexture
  // if (g_Game()->HasGLSL())
    //glActiveTexture(GL_TEXTURE0+stage);
    //glBindTexture( GL_TEXTURE_2D, ttexts[ttex].depth);
}

void TPostEffect::TempTextureToStage(int ttex,int stage)
{
       if (!isprecached) Precache();
 if ((ttex<0) || (ttex>=(int)(ttexts.size()))) return;
cout << "Temp Texture to Stage not defined" << endl;
///TODO: Set the active Texture g_lastActiveTexture
   //if (g_Game()->HasGLSL())
//    glActiveTexture(GL_TEXTURE0+stage);
  //  glBindTexture( GL_TEXTURE_2D, ttexts[ttex].tindex );
}



/////////////LUA IMPLEMENT /////////////////7


int EFFECT_RenderPass(lua_State *state)
{
 g_Game()->RenderPass();
 return 0;
}

int EFFECT_SpecialRenderPass(lua_State *state)
{
    int defaultrender=LUA_GET_INT;
 string nam=LUA_GET_STRING;

 g_Game()->SpecialRenderPass(nam,defaultrender);
 return 0;
}


int EFFECT_DoRenderPart(lua_State *state)
{
 int index=LUA_GET_INT;
 string tname=LUA_GET_STRING;
 if (tname=="sky") {g_Game()->GetSky()->Render(); }
 else if (tname=="item") {g_Game()->GetLevel()->GetItem(index)->Render();}
 else if (tname=="side") {g_Game()->GetLevel()->GetBlockSide(index)->Render();}
 else if (tname=="actor") {g_Game()->GetActorMovement(index)->Render();}
 return 0;
}


int EFFECT_CreateTempTexture(lua_State *state)
{
 int wdepth=LUA_GET_INT;
 int h=LUA_GET_INT;
 int w=LUA_GET_INT;
 if (g_PostEffect()) w=g_PostEffect()->CreateTempTexture(w,h,wdepth);
 LUA_SET_INT(w);
 return 1;
}


int EFFECT_SetRenderTarget(lua_State *state)
{
 int i=LUA_GET_INT;
 if (g_PostEffect()) g_PostEffect()->SetRenderTarget(i);
 return 0;
}

int EFFECT_TempTextureToStage(lua_State *state)
 {
 int stage=LUA_GET_INT;
 int tex=LUA_GET_INT
 if (g_PostEffect()) g_PostEffect()->TempTextureToStage(tex,stage);
 return 0;
}

int EFFECT_DepthToStage(lua_State *state)
 {
 int stage=LUA_GET_INT;
 int tex=LUA_GET_INT
 if (g_PostEffect()) g_PostEffect()->DepthToStage(tex,stage);
 return 0;
}

int EFFECT_DrawQuad(lua_State *state)
 {
 if (g_PostEffect()) g_PostEffect()->DrawQuad();
 return 0;
}


int EFFECT_Load(lua_State *state)
{
 string name=LUA_GET_STRING;
 LoadPostEffect(name);
 return 0;
}



void LUA_EFFECT_RegisterLib()
{
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
