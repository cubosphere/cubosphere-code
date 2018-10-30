/**
Copyright (C) 2010 Chriddo

This program is free software;
you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program;
if not, see <http://www.gnu.org/licenses/>.
**/

#include <iostream>
#include <string>

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


#include "skyboxes.hpp"
#include "posteffects.hpp"
#include "vectors.hpp"
#include "globals.hpp"


void TSkyBox::Render()
	{
	if (lua.FuncExists("Render"))
		lua.CallVA("Render","");

	}

void TSkyBox::SpecialRender(string nam,int defrender)
	{
	if (lua.FuncExists("SpecialRender"))
			{
			lua.CallVA("SpecialRender","s",nam.c_str());
			}
	else
			{
			if (defrender==1) Render();
			else if (defrender==0 && g_PostEffect())  g_PostEffect()->CallDefaultSpecialRender(nam,"sky",0);
			}

	}

void TSkyBox::LoadSkybox(string basename)
	{
	if (basename=="") basename="skybox";
	SetName(basename);
	LoadDef();
	}
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
