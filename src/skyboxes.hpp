/**
Copyright (C) 2010 Chriddo

This program is free software;
you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program;
if not, see <http://www.gnu.org/licenses/>.
**/


#ifndef SKYBOXES_H_G
#define SKYBOXES_H_G

#include "definitions.hpp"
#include "vectors.hpp"
#include "textures.hpp"
#include <vector>



using namespace std;



class TSkyBox : public TBaseLuaDef
	{

	public:
		virtual ~TSkyBox() {};
		virtual int GetType() {return FILE_SKYBOX;}
		void  LoadSkybox(string basename);
		void Render();
		void SpecialRender(string nam,int defrender);
	};





#endif
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
