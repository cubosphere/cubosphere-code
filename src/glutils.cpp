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

#include "glutils.hpp"
#include "luautils.hpp"
#include "game.hpp"
#include "globals.hpp"




class TLuaGLLib : public TLuaCFunctions
	{
	protected:

		static int LglBegin(lua_State *state)
			{
			std::string s=LUA_GET_STRING;
			GLuint t;

			if (s=="GL_POINTS") t=GL_POINTS;
			else if (s=="GL_LINES") t=GL_LINES;
			else if (s=="GL_LINE_STRIP") t=GL_LINE_STRIP;
			else if (s=="GL_LINE_LOOP") t=GL_LINE_LOOP;
			else if (s=="GL_TRIANGLES") t=GL_TRIANGLES;
			else if (s=="GL_TRIANGLE_STRIP") t=GL_TRIANGLE_STRIP;
			else if (s=="GL_TRIANGLE_FAN") t=GL_TRIANGLE_FAN;
			else if (s=="GL_QUADS") t=GL_QUADS;
			else if (s=="GL_QUAD_STRIP") t=GL_QUAD_STRIP;
			else if (s=="GL_POLYGON") t=GL_POLYGON;
			else t=GL_TRIANGLES;
			glBegin(t);
			return 0;
			}

		static int LglEnd(lua_State *state)
			{
			glEnd();
			return 0;
			}

		static int LglNormal(lua_State *state)
			{
			T3dVector v=Vector3FromStack(state);
			glNormal3f(v.x,v.y,v.z);
			return 0;
			}

		static int LglNormal3f(lua_State *state)
			{
			double z=LUA_GET_DOUBLE;
			double y=LUA_GET_DOUBLE;
			double x=LUA_GET_DOUBLE;
			glNormal3f(x,y,z);

			return 0;
			}

		static int LglVertex(lua_State *state)
			{
			T3dVector v=Vector3FromStack(state);
			glVertex3f(v.x,v.y,v.z);
			return 0;
			}

		static int LglVertex3f(lua_State *state)
			{
			double z=LUA_GET_DOUBLE;
			double y=LUA_GET_DOUBLE;
			double x=LUA_GET_DOUBLE;
			glVertex3f(x,y,z);

			return 0;
			}

		static int LglVertex2f(lua_State *state)
			{
			double y=LUA_GET_DOUBLE;
			double x=LUA_GET_DOUBLE;
			glVertex2f(x,y);
			return 0;
			}

		static int LglTexCoord2f(lua_State *state)
			{
			double y=LUA_GET_DOUBLE;
			double x=LUA_GET_DOUBLE;
			glTexCoord2f(x,y);
			return 0;
			}

		static int LglGenLists(lua_State *state)
			{
			int num=LUA_GET_INT;
			num=glGenLists((GLsizei)num);
			LUA_SET_INT(num);
			return 1;
			}

		static int LglDeleteLists(lua_State *state)
			{
			int num=LUA_GET_INT;
			int start=LUA_GET_INT;
			glDeleteLists(start,num);
			return 0;
			}

		static int LglBeginListCompile(lua_State *state)
			{
			int lst=LUA_GET_INT;
			InvalidateMaterial();
			glNewList(lst,GL_COMPILE);
			return 0;
			}

		static int LglEndListCompile(lua_State *state)
			{
			glEndList();
			InvalidateMaterial();
			return 0;
			}

		static int LglCallList(lua_State *state)
			{
			int lst=LUA_GET_INT;
			glCallList(lst);
			return 0;
			}


		static int LglFogMode(lua_State *state)
			{
			std::string arg=LUA_GET_STRING;

			if (arg=="LINEAR") {glEnable(GL_FOG); glHint(GL_FOG_HINT,GL_NICEST); glFogf(GL_FOG_MODE,GL_LINEAR); }
			else  if (arg=="EXP") {glEnable(GL_FOG); glHint(GL_FOG_HINT,GL_NICEST); glFogf(GL_FOG_MODE,GL_EXP); }
			else  if (arg=="EXP2") {glEnable(GL_FOG); glHint(GL_FOG_HINT,GL_NICEST); glFogf(GL_FOG_MODE,GL_EXP2); }
			else  glDisable(GL_FOG);

			return 0;
			}

		static int LglFogParams(lua_State *state)
			{
			GLfloat dens=LUA_GET_DOUBLE;
			GLfloat end=LUA_GET_DOUBLE;
			GLfloat start=LUA_GET_DOUBLE;
			glFogf(GL_FOG_DENSITY,dens);
			glFogf(GL_FOG_END,end);
			glFogf(GL_FOG_START,start);

			return 0;
			}


		static int LglFogColor(lua_State *state)
			{
			GLfloat vs[4];
			vs[0]=getfloatfield(state,"r");
			vs[1]=getfloatfield(state,"g");
			vs[2]=getfloatfield(state,"b");
			vs[3]=getfloatfield(state,"a");
			lua_pop(state,1);

			glFogfv(GL_FOG_COLOR,vs);

			return 0;
			}

		static int LglGetFogColor(lua_State *state)
			{

			GLfloat vs[4];
			glGetFloatv(GL_FOG_COLOR,vs);
			T4dVector v(vs[0],vs[1],vs[2],vs[3]);
			LUA_SET_COLOR(v);
			return 1;
			}

		static int LglInvertColors(lua_State *state)
			{
			int st=LUA_GET_INT;
			if (st!=0)
					{
					glLogicOp(GL_INVERT);
					glEnable(GL_COLOR_LOGIC_OP);
					}
			else glDisable(GL_COLOR_LOGIC_OP);
			return 0;
			}


	public:
		TLuaGLLib()
			{
			AddFunc("glBegin",LglBegin);
			AddFunc("glEnd",LglEnd);
			AddFunc("glNormal",LglNormal);
			AddFunc("glNormal3f",LglNormal3f);
			AddFunc("glVertex",LglVertex);
			AddFunc("glVertex2f",LglVertex2f);
			AddFunc("glVertex3f",LglVertex3f);
			AddFunc("glTexCoord2f",LglTexCoord2f);
			AddFunc("glGenLists",LglGenLists);
			AddFunc("glDeleteLists",LglDeleteLists);
			AddFunc("glBeginListCompile",LglBeginListCompile);
			AddFunc("glEndListCompile",LglEndListCompile);
			AddFunc("glCallList",LglCallList);
			AddFunc("glFogMode",LglFogMode);
			AddFunc("glFogColor",LglFogColor);
			AddFunc("glGetFogColor",LglGetFogColor);
			AddFunc("glFogParams",LglFogParams);
			AddFunc("glInvertColors",LglInvertColors);
			}
	};

static TLuaGLLib g_gllib;
TLuaCFunctions* g_GLLib() {return &g_gllib;}


//////////////LUA-IMPLEMENT///////////////////

int BLEND_Activate(lua_State *state)
	{
	glEnable(GL_BLEND);
	return 0;
	}

int BLEND_Function(lua_State *state)
	{
	int v2=LUA_GET_INT;
	int v1=LUA_GET_INT;
	glBlendFunc(v1,v2);
	return 0;
	}


int BLEND_Deactivate(lua_State *state)
	{
	glDisable(GL_BLEND);
	return 0;
	}


void LUA_BLEND_RegisterLib()
	{
	g_CuboLib()->AddFunc("BLEND_Activate",BLEND_Activate);
	g_CuboLib()->AddFunc("BLEND_Deactivate",BLEND_Deactivate);
	g_CuboLib()->AddFunc("BLEND_Function",BLEND_Function);
	}


//////////////////////////

int CULL_Mode(lua_State *state) //0: Disable, 1: Backfaces culled, 2: Front faces
	{
	int m=LUA_GET_INT;
	if (!m) glDisable(GL_CULL_FACE);
	else {
			glEnable(GL_CULL_FACE);
			int f;
			if (m==2) f=GL_FRONT; else f=GL_BACK;
			glCullFace(f);
			}
	return 0;
	}


void LUA_CULL_RegisterLib()
	{
	g_CuboLib()->AddFunc("CULL_Mode",CULL_Mode);
	}


//////////////////////////


int DEPTH_Offset(lua_State *state)
	{
	float v2=LUA_GET_DOUBLE;
	float v1=LUA_GET_DOUBLE;
	if ((v1*v1<0.0000001) && (v2*v2<0.0000001))
			{
			glPolygonOffset(0,0);
			glDisable(GL_POLYGON_OFFSET_FILL);
			return 0;
			}
	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(v1,v2);
	return 0;
	}

int DEPTH_Enable(lua_State *state)
	{
	glEnable(GL_DEPTH_TEST);
	return 0;
	}

int DEPTH_Disable(lua_State *state)
	{
	glDisable(GL_DEPTH_TEST);
	return 0;
	}

int DEPTH_Mask(lua_State *state)
	{
	int i=LUA_GET_INT;
	glDepthMask(i);
	return 0;
	}

int DEPTH_Func(lua_State *state)
	{
	int i=LUA_GET_INT;
	glDepthFunc(i);
	return 0;
	}

int DEPTH_Clear(lua_State *state)
	{
	glClear(GL_DEPTH_BUFFER_BIT);
	return 0;
	}



void LUA_DEPTH_RegisterLib()
	{
	g_CuboLib()->AddFunc("DEPTH_Offset",DEPTH_Offset);
	g_CuboLib()->AddFunc("DEPTH_Enable",DEPTH_Enable);
	g_CuboLib()->AddFunc("DEPTH_Disable",DEPTH_Disable);
	g_CuboLib()->AddFunc("DEPTH_Mask",DEPTH_Mask);
	g_CuboLib()->AddFunc("DEPTH_Clear",DEPTH_Clear);
	g_CuboLib()->AddFunc("DEPTH_Func",DEPTH_Func);
	}

////////////////////////////

static std::vector<int> videowidths;
static std::vector<int> videoheights;

int GetModes(int hw,int fs)
	{
	SDL_Rect** modes;
	int i;

	/* Get available fullscreen/hardware modes */
	Uint32 flags=SDL_OPENGL;
	if (fs) flags|=SDL_FULLSCREEN;
	if (hw) flags|=SDL_HWSURFACE |SDL_DOUBLEBUF; else flags|=SDL_SWSURFACE;
	modes = SDL_ListModes(NULL, flags);

	videowidths.clear();
	videoheights.clear();
	/* Check if there are any modes available */
	if (modes == (SDL_Rect**)0) {
			return 0;
			}

	/* Check if our resolution is restricted */
	if (modes == (SDL_Rect**)-1) {
			///TODO: Add a few standart resolutions
			videowidths.push_back(1024); videoheights.push_back(768);
			videowidths.push_back(800); videoheights.push_back(600);
			videowidths.push_back(640); videoheights.push_back(480);
			//  videowidths.push_back(320); videoheights.push_back(240);

			}
	else {
			/* Print valid modes */
			for (i=0; modes[i]; ++i)
					{
					videowidths.push_back(modes[i]->w);
					videoheights.push_back(modes[i]->h);
					}
			}

	return videoheights.size();

	}

int DEVICE_Viewport(lua_State *state)
	{
	int h=LUA_GET_INT;
	int w=LUA_GET_INT;
	glViewport(0,0,w,h);
	return 0;
	}

int DEVICE_Ortho(lua_State *state)
	{
	glMatrixMode(GL_PROJECTION);								// Select Projection
	glPushMatrix();												// Push The Matrix
	glLoadIdentity();											// Reset The Matrix
	glOrtho(-0.5,0.5,-0.5,0.5,-1,1);
//	glOrtho( 0, 640 , 480 , 0, -1, 1 );							// Select Ortho Mode (640x480)
	glMatrixMode(GL_MODELVIEW);									// Select Modelview Matrix
	glPushMatrix();												// Push The Matrix
	glLoadIdentity();											// Reset The Matrix
	return 0;
	}

int DEVICE_UnOrtho(lua_State *state)
	{
	glMatrixMode( GL_PROJECTION );								// Select Projection
	glPopMatrix();												// Pop The Matrix
	glMatrixMode( GL_MODELVIEW );								// Select Modelview
	glPopMatrix();
	return 0;
	}

int DEVICE_Clear(lua_State *state)
	{
	float vs[4]= {0,0,0,0};
//vs[0]=cv.x; vs[1]=cv.y; vs[2]=cv.z; vs[3]=cv.w;
	vs[0]=getfloatfield(state,"r");
	vs[1]=getfloatfield(state,"g");
	vs[2]=getfloatfield(state,"b");
	vs[3]=getfloatfield(state,"a");

	lua_pop(state,1);

	glClearColor(vs[0], vs[1], vs[2], vs[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	return 0;
	}

int DEVICE_GetVideoModes(lua_State *state)
	{
	//Make the Video-Modes
	int fs=LUA_GET_INT;
	int hw=LUA_GET_INT;
	int i=GetModes(hw,fs);
	LUA_SET_INT(i);
	return 1;
	}

int DEVICE_GetVideoWidths(lua_State *state)
	{
	int index=LUA_GET_INT;
	LUA_SET_INT(videowidths[index]);
	return 1;
	}

int DEVICE_ScreenShot(lua_State *state)
	{
	g_Game()->ScreenShot();
	return 0;
	}

int DEVICE_SaveFramePic(lua_State *state)
	{
	int h=LUA_GET_INT;
	int w=LUA_GET_INT;
	std::string s=LUA_GET_STRING;
	cls_FileWriteable *fw= g_BaseFileSystem()->GetFileForWriting(s,true);
	if (!fw) return 0;
	if (!fw->IsHDDFile()) return 0;
	g_Game()->SaveFramePic(fw->GetHDDName(),w,h);
	delete fw;
	return 0;
	}


int DEVICE_HasGLSL(lua_State *state)
	{
	LUA_SET_INT(g_Game()->HasGLSL());
	return 1;
	}


int DEVICE_Init(lua_State *state)
	{
	int bpp=LUA_GET_INT;
	int fs=LUA_GET_INT;
	int hw=LUA_GET_INT;
	int h=LUA_GET_INT;
	int w=LUA_GET_INT;
	bool res=g_Game()->InitGL(w,h,hw,fs,bpp);
	LUA_SET_INT((res==true ? 1 : 0));
	return 1;
	}

int DEVICE_GetVideoHeights(lua_State *state)
	{
	int index=LUA_GET_INT;
	LUA_SET_INT(videoheights[index]);
	return 1;
	}

int DEVICE_Reload(lua_State *state)
	{
	g_Game()->FreeMedia();
	return 0;
	}


int DEVICE_SetAntiAliasing(lua_State *state)
	{
	int aa=LUA_GET_INT;
	g_Game()->SetAntiAliasing(aa);
	return 0;
	}


void LUA_DEVICE_RegisterLib()
	{
	g_CuboLib()->AddFunc("DEVICE_Viewport",DEVICE_Viewport);
	g_CuboLib()->AddFunc("DEVICE_Ortho",DEVICE_Ortho);
	g_CuboLib()->AddFunc("DEVICE_UnOrtho",DEVICE_UnOrtho);
	g_CuboLib()->AddFunc("DEVICE_Clear",DEVICE_Clear);
	g_CuboLib()->AddFunc("DEVICE_Init",DEVICE_Init);
	g_CuboLib()->AddFunc("DEVICE_Reload",DEVICE_Reload);
	g_CuboLib()->AddFunc("DEVICE_GetVideoModes",DEVICE_GetVideoModes);
	g_CuboLib()->AddFunc("DEVICE_GetVideoWidths",DEVICE_GetVideoWidths);
	g_CuboLib()->AddFunc("DEVICE_GetVideoHeights",DEVICE_GetVideoHeights);
	g_CuboLib()->AddFunc("DEVICE_HasGLSL",DEVICE_HasGLSL);
	g_CuboLib()->AddFunc("DEVICE_ScreenShot",DEVICE_ScreenShot);
	g_CuboLib()->AddFunc("DEVICE_SaveFramePic",DEVICE_SaveFramePic);
	g_CuboLib()->AddFunc("DEVICE_SetAntiAliasing",DEVICE_SetAntiAliasing);
	}


//////////////////////////



int LIGHT_Activate(lua_State *state)
	{
	int l=LUA_GET_INT;
	glEnable(GL_LIGHT0+l);
	return 0;
	}

int LIGHT_Deactivate(lua_State *state)
	{
	int l=LUA_GET_INT;
	glDisable(GL_LIGHT0+l);
	return 0;
	}

int LIGHT_Enable(lua_State *state)
	{
	glEnable(GL_LIGHTING);
	return 0;
	}

int LIGHT_Disable(lua_State *state)
	{
	glDisable(GL_LIGHTING);
	return 0;
	}

int LIGHT_SetPosition(lua_State *state)
	{
	T3dVector v=Vector3FromStack(state);
	int l=LUA_GET_INT;
	GLfloat p[4]= {v.x,v.y,v.z,0};
	glLightfv(GL_LIGHT0+l, GL_POSITION, p);
	return 0;
	}

int LIGHT_SetAmbient(lua_State *state)
	{
//T4dVector cv;
//LUA_GET_COLOR_VECTOR(cv);
	float vs[4]= {0,0,0,0};
//vs[0]=cv.x; vs[1]=cv.y; vs[2]=cv.z; vs[3]=cv.w;
	vs[0]=getfloatfield(state,"r");
	vs[1]=getfloatfield(state,"g");
	vs[2]=getfloatfield(state,"b");
	vs[3]=getfloatfield(state,"a");
	lua_pop(state,1);
	int light=LUA_GET_INT;

	glLightfv(GL_LIGHT0+light, GL_AMBIENT,   vs);
	return 0;
	}

int LIGHT_SetGlobalAmbient(lua_State *state)
	{
//T4dVector cv;
//LUA_GET_COLOR_VECTOR(cv);
	float vs[4]= {0,0,0,0};
//vs[0]=cv.x; vs[1]=cv.y; vs[2]=cv.z; vs[3]=cv.w;
	vs[0]=getfloatfield(state,"r");
	vs[1]=getfloatfield(state,"g");
	vs[2]=getfloatfield(state,"b");
	vs[3]=getfloatfield(state,"a");
	lua_pop(state,1);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, vs);
	return 0;
	}

int LIGHT_SetDiffuse(lua_State *state)
	{
	float vs[4]= {0,0,0,0};
//vs[0]=cv.x; vs[1]=cv.y; vs[2]=cv.z; vs[3]=cv.w;
	vs[0]=getfloatfield(state,"r");
	vs[1]=getfloatfield(state,"g");
	vs[2]=getfloatfield(state,"b");
	vs[3]=getfloatfield(state,"a");
	lua_pop(state,1);
	int light=LUA_GET_INT;

	glLightfv(GL_LIGHT0+light, GL_DIFFUSE,   vs);
	return 0;
	}

int LIGHT_SetSpecular(lua_State *state)
	{
	//T4dVector cv;
//LUA_GET_COLOR_VECTOR(cv);
	float vs[4]= {0,0,0,0};
//vs[0]=cv.x; vs[1]=cv.y; vs[2]=cv.z; vs[3]=cv.w;
	vs[0]=getfloatfield(state,"r");
	vs[1]=getfloatfield(state,"g");
	vs[2]=getfloatfield(state,"b");
	vs[3]=getfloatfield(state,"a");

	lua_pop(state,1);
	int light=LUA_GET_INT;

	glLightfv(GL_LIGHT0+light, GL_SPECULAR,   vs);
	return 0;
	}



void LUA_LIGHT_RegisterLib()
	{
	g_CuboLib()->AddFunc("LIGHT_Activate",LIGHT_Activate);
	g_CuboLib()->AddFunc("LIGHT_Deactivate",LIGHT_Deactivate);
	g_CuboLib()->AddFunc("LIGHT_SetPosition",LIGHT_SetPosition);
	g_CuboLib()->AddFunc("LIGHT_SetAmbient",LIGHT_SetAmbient);
	g_CuboLib()->AddFunc("LIGHT_SetGlobalAmbient",LIGHT_SetGlobalAmbient);

	g_CuboLib()->AddFunc("LIGHT_SetDiffuse",LIGHT_SetDiffuse);
	g_CuboLib()->AddFunc("LIGHT_SetSpecular",LIGHT_SetSpecular);
	g_CuboLib()->AddFunc("LIGHT_Enable",LIGHT_Enable); ///Activates the complete OpenGL Lighting Model
	g_CuboLib()->AddFunc("LIGHT_Disable",LIGHT_Disable);
	}


//////////////////////////

float g_materialmultiply[4]= {1,1,1,1};
float g_lastambient[4]= {-1,-1.2,-1.3,-1.4};
float g_lastdiffuse[4]= {-1,-1.2,-1.3,-1.4};
float g_lastspecular[4]= {-1,-1.2,-1.3,-1.4};
float g_lastemission[4]= {-1,-1.2,-1.3,-1.4};
float g_lastcolor[4]= {-1,-1.2,-1.3,-1.4};



void InvalidateMaterial()
	{
	for (unsigned int i=0; i<4; i++)
			{
			g_lastambient[i]=-1;
			g_lastdiffuse[i]=-1;
			g_lastspecular[i]=-1;
			g_lastemission[i]=-1;
			g_lastcolor[i]=-1;
			}
	g_Game()->GetTextures()->Invalidate();
	}

int MATERIAL_Invalidate(lua_State *state)
	{
	InvalidateMaterial();
	return 0;
	}

int MATERIAL_SetMultiply(lua_State *state)
	{
//T4dVector cv;
//LUA_GET_COLOR_VECTOR(cv);
	g_materialmultiply[0]=getfloatfield(state,"r");
	g_materialmultiply[1]=getfloatfield(state,"g");
	g_materialmultiply[2]=getfloatfield(state,"b");
	g_materialmultiply[3]=getfloatfield(state,"a");
	lua_pop(state,1);




	return 0;
	}



int MATERIAL_SetAmbient(lua_State *state)
	{
//T4dVector cv;
//LUA_GET_COLOR_VECTOR(cv);

	float vs[4]; for (unsigned int i=0; i<4; i++) { vs[i]=g_materialmultiply[i]; }
//vs[0]=cv.x; vs[1]=cv.y; vs[2]=cv.z; vs[3]=cv.w;
	vs[0]*=getfloatfield(state,"r");
	vs[1]*=getfloatfield(state,"g");
	vs[2]*=getfloatfield(state,"b");
	vs[3]*=getfloatfield(state,"a");
	lua_pop(state,1);

	bool same=true;
	for (unsigned int i=0; i<4; i++)  {if (g_lastambient[i]!=vs[i]) {same=false; break;}}
	if (same) return 0;
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   vs);
	for (unsigned int i=0; i<4; i++) g_lastambient[i]=vs[i];
	return 0;
	}

int MATERIAL_SetDiffuse(lua_State *state)
	{
//T4dVector cv;
//LUA_GET_COLOR_VECTOR(cv);
	float vs[4]= {0,0,0,0}; for (unsigned int i=0; i<4; i++) vs[i]=g_materialmultiply[i];
//vs[0]=cv.x; vs[1]=cv.y; vs[2]=cv.z; vs[3]=cv.w;
	vs[0]*=getfloatfield(state,"r");
	vs[1]*=getfloatfield(state,"g");
	vs[2]*=getfloatfield(state,"b");
	vs[3]*=getfloatfield(state,"a");
	lua_pop(state,1);




	bool same=true;
	for (unsigned int i=0; i<4; i++)  {if (g_lastdiffuse[i]!=vs[i]) {same=false; break;}}
	if (same) return 0;
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   vs);
	for (unsigned int i=0; i<4; i++) g_lastdiffuse[i]=vs[i];

	g_LastDiffuse(vs);


	return 0;
	}

int MATERIAL_SetSpecular(lua_State *state)
	{
//T4dVector cv;
//LUA_GET_COLOR_VECTOR(cv);
	float vs[4]= {0,0,0,0}; for (unsigned int i=0; i<4; i++) vs[i]=g_materialmultiply[i];
//vs[0]=cv.x; vs[1]=cv.y; vs[2]=cv.z; vs[3]=cv.w;
	vs[0]*=getfloatfield(state,"r");
	vs[1]*=getfloatfield(state,"g");
	vs[2]*=getfloatfield(state,"b");
	vs[3]*=getfloatfield(state,"a");
	lua_pop(state,1);

	bool same=true;
	for (unsigned int i=0; i<4; i++)  {if (g_lastspecular[i]!=vs[i]) {same=false; break;}}
	if (same) return 0;
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,   vs);
	for (unsigned int i=0; i<4; i++) g_lastspecular[i]=vs[i];


	return 0;
	}

int MATERIAL_SetEmissive(lua_State *state)
	{
//T4dVector cv;
//LUA_GET_COLOR_VECTOR(cv);
	float vs[4]= {0,0,0,0}; for (unsigned int i=0; i<4; i++) vs[i]=g_materialmultiply[i];
//vs[0]=cv.x; vs[1]=cv.y; vs[2]=cv.z; vs[3]=cv.w;
	vs[0]*=getfloatfield(state,"r");
	vs[1]*=getfloatfield(state,"g");
	vs[2]*=getfloatfield(state,"b");
	vs[3]*=getfloatfield(state,"a");

	lua_pop(state,1);

	bool same=true;
	for (unsigned int i=0; i<4; i++)  {if (g_lastemission[i]!=vs[i]) {same=false; break;}}
	if (same) return 0;
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION,   vs);
	for (unsigned int i=0; i<4; i++) g_lastemission[i]=vs[i];

	return 0;
	}

int MATERIAL_SetColor(lua_State *state)
	{
//T4dVector cv;
//LUA_GET_COLOR_VECTOR(cv);
	float vs[4]= {0,0,0,0}; for (unsigned int i=0; i<4; i++) vs[i]=g_materialmultiply[i];
//vs[0]=cv.x; vs[1]=cv.y; vs[2]=cv.z; vs[3]=cv.w;
	vs[0]*=getfloatfield(state,"r");
	vs[1]*=getfloatfield(state,"g");
	vs[2]*=getfloatfield(state,"b");
	vs[3]*=getfloatfield(state,"a");

	lua_pop(state,1);

	bool same=true;
	for (unsigned int i=0; i<4; i++)  {if (g_lastcolor[i]!=vs[i]) {same=false; break;}}
	if (same) return 0;
	glColor4fv(vs);
	for (unsigned int i=0; i<4; i++) g_lastcolor[i]=vs[i];


	return 0;
	}



int MATERIAL_SetSpecularPower(lua_State *state)
	{
	double dpower=LUA_GET_DOUBLE;

	float power=1.0*dpower;
	if (power<0) power=0; else if (power>128) power=128.0;
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS,   power);
	return 0;
	}



void LUA_MATERIAL_RegisterLib()
	{
	g_CuboLib()->AddFunc("MATERIAL_SetMultiply",MATERIAL_SetMultiply);
	g_CuboLib()->AddFunc("MATERIAL_Invalidate",MATERIAL_Invalidate);
	g_CuboLib()->AddFunc("MATERIAL_SetAmbient",MATERIAL_SetAmbient);
	g_CuboLib()->AddFunc("MATERIAL_SetDiffuse",MATERIAL_SetDiffuse);
	g_CuboLib()->AddFunc("MATERIAL_SetSpecular",MATERIAL_SetSpecular);
	g_CuboLib()->AddFunc("MATERIAL_SetEmissive",MATERIAL_SetEmissive);
	g_CuboLib()->AddFunc("MATERIAL_SetColor",MATERIAL_SetColor);
	g_CuboLib()->AddFunc("MATERIAL_SetSpecularPower",MATERIAL_SetSpecularPower);
	}


/////////////////////////////////////////

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
