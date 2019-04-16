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

#include <iostream>
#include <string>

#include <GL/glew.h>
#include <SDL.h>

#include "skyboxes.hpp"
#include "posteffects.hpp"
#include "vectors.hpp"
#include "globals.hpp"
#include "definitions.hpp"


void SkyBox::Render() {
	lua.CallVAIfPresent("Render");
	}

void SkyBox::SpecialRender(std::string nam,int defrender) {
	if (!lua.CallVAIfPresent("SpecialRender", {{nam}})) {
		if (defrender==1) { Render(); }
		else if (defrender==0 && g_PostEffect()) { g_PostEffect()->CallDefaultSpecialRender(nam,"sky",0); }
		}

	}

void SkyBox::LoadSkybox(std::string basename) {
	if (basename=="") { basename="skybox"; }
	SetName(basename);
	LoadDef();
	}
