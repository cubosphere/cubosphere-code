/**
Copyright (C) 2010 Chriddo

This program is free software;
you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program;
if not, see <http://www.gnu.org/licenses/>.
**/


#include "matrixutils.hpp"

#ifdef WIN32

#ifdef MINGW_CROSS_COMPILE
#include <GL/glew.h>
#else
#include <GL\glew.h>
#endif
// #include <GL\gl.h>
#include <windows.h>
#include <SDL.h>


#else

#include <GL/glew.h>
#include <SDL/SDL.h>

#endif

#include "luautils.hpp"


T4dGlutMatrix::T4dGlutMatrix(): mode(GL_MODELVIEW) {}
T4dGlutMatrix::T4dGlutMatrix(tfloat scal): T4dMatrix(scal), mode(GL_MODELVIEW) {}


void T4dGlutMatrix::glPushMult()
	{
//glMatrixMode(mode);
	glPushMatrix();
	glMultMatrixf((GLfloat*)(&m));
	}

void T4dGlutMatrix::glMult()
	{
//glMatrixMode(mode);
	glMultMatrixf((GLfloat*)(&m));
	}


void T4dGlutMatrix::glPop()
	{
//glMatrixMode(mode);
	glPopMatrix();
	}

void T4dGlutMatrix::glPushLoad()
	{
//glMatrixMode(mode);
	glPushMatrix();
	glLoadMatrixf((GLfloat*)(&m));
	}

void T4dGlutMatrix::glLoad()
	{
//glMatrixMode(mode);
	glLoadMatrixf((GLfloat*)(&m));
	}

const T3dVector T4dGlutMatrix::getPos() //Returns the last col
	{
	T3dVector res(m[12],m[13],m[14]);
	return res;
	}

void T4dGlutMatrix::setPos(const T3dVector& v) //sets the last c
	{
	setCol(3,v);
	}

void T4dGlutMatrix::assign(T4dMatrix* other)
	{
	tfloat* mom=other->getValueMem();
	for (int i=0; i<16; i++)
			{
			T4dMatrix::m[i]=mom[i];
			}
	}


/////////////LUA-IMPLEMENT///////////////////


int MATRIX_Push(lua_State *state)
	{
	glPushMatrix();
	return 0;
	}

int MATRIX_Pop(lua_State *state)
	{
	glPopMatrix();
	return 0;
	}
int MATRIX_Translate(lua_State *state)
	{
	T3dVector v=Vector3FromStack(state);
	glTranslatef(v.x,v.y,v.z);
	return 0;
	}

int MATRIX_Identity(lua_State *state)
	{
	glLoadIdentity();
	return 0;
	}

int MATRIX_AxisRotate(lua_State *state)
	{
	float angle=LUA_GET_DOUBLE;
	T3dVector v=Vector3FromStack(state);
	glRotatef(angle,v.x,v.y,v.z);
	return 0;
	}

int MATRIX_ScaleUniform(lua_State *state)
	{
	float f=LUA_GET_DOUBLE;
	glScalef(f,f,f);
	return 0;
	}

int MATRIX_Scale(lua_State *state)
	{
	T3dVector v=Vector3FromStack(state);
	glScalef(v.x,v.y,v.z);
	return 0;
	}

int MATRIX_MultBase(lua_State *state)
	{
	T3dVector p=Vector3FromStack(state);
	T3dVector d=Vector3FromStack(state);
	T3dVector u=Vector3FromStack(state);
	T3dVector s=Vector3FromStack(state);

	T3dMatrix m(s,u,d);

	T4dGlutMatrix m4(1);
	m4.setSubMatrix(m);
	m4.setPos(p);

	m4.glMult();


	return 0;
	}



void LUA_MATRIX_RegisterLib()
	{
	g_CuboLib()->AddFunc("MATRIX_Push",MATRIX_Push);
	g_CuboLib()->AddFunc("MATRIX_Pop",MATRIX_Pop);
	g_CuboLib()->AddFunc("MATRIX_Translate",MATRIX_Translate);
	g_CuboLib()->AddFunc("MATRIX_MultBase",MATRIX_MultBase);
	g_CuboLib()->AddFunc("MATRIX_ScaleUniform",MATRIX_ScaleUniform);
	g_CuboLib()->AddFunc("MATRIX_AxisRotate",MATRIX_AxisRotate);
	g_CuboLib()->AddFunc("MATRIX_Identity",MATRIX_Identity);
	g_CuboLib()->AddFunc("MATRIX_Scale",MATRIX_Scale);
	}
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
