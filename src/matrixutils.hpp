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

#ifndef MATRIXUTILS_H_G
#define MATRIXUTILS_H_G

#include "vectors.hpp"
/*
class T2dGlutVector : public T2dVector
{
 public:
  void glutPosition();
    void glutTexture();
};

class T3dGlutVector : public T3dVector
{
 public:
  void glutPosition();
  void glutNormal();
  void glutColor();
};

class T4dGlutVector : public T4dVector
{
 public:
  void glutPosition();
  void glutColor();
};
*/

class T4dGlutMatrix: public T4dMatrix
	{
	protected:
	public:
		int mode;
		T4dGlutMatrix();
		T4dGlutMatrix(tfloat scal);
		void glPushMult();
		void glMult();
		void glPop();
		void glPushLoad();
		void glLoad();
		const T3dVector getPos(); //Returns the last col
		void setPos(const T3dVector&); //sets the last col
		void assign(T4dMatrix* other);
	};


extern void LUA_MATRIX_RegisterLib();

#endif
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
