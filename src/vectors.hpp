/**
Copyright (C) 2010 Chriddo

This program is free software;
you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program;
if not, see <http://www.gnu.org/licenses/>.
**/


#ifndef VECTORS_H_G
#define VECTORS_H_G

#include <string>
#include <cmath>

//typedef float tfloat;
#define tfloat float


#define VECTOR_EPSILON 0.00000000001

#define VECTOR_OK 0
#define VECTOR_NORMALIZE_ERROR 1

#ifndef M_PI
#define M_PI    3.14159265358979323846f
#endif

class T2dVector
	{

	public:
		tfloat u,v;  //Not good OOP but who cares... no need for getters / setters
		T2dVector() {u=0; v=0;}
		T2dVector(const tfloat cu, const tfloat cv) {u=cu; v=cv;} //Good constructor
		void uv(const tfloat cu, const tfloat cv) {u=cu; v=cv;}
		T2dVector operator+(const T2dVector&); //Addition
		T2dVector operator-(const T2dVector&); //Subtraction
		T2dVector operator-(); //Negation
		T2dVector operator*(const tfloat&); //s-multipy
		T2dVector operator/(const tfloat&); //s-divide
		tfloat operator*(const T2dVector&); //dot product
		tfloat length(); //length
		tfloat sqrlength(); //length²
		int normalizeCheck(); //VECTOR_OK if we could normalize, VECTOR_NORMALIZE_ERROR otherwise
		void normalize(); //Normalize without checking division by zero
		tfloat Arg() {return atan2(v,u);}
		std::string toString(); //Gives out "(u, v)"
	};

class T3dVector
	{

	public:
		tfloat x,y,z;  //Not good OOP but who cares... no need for getters / setters
		T3dVector() {x=0; y=0; z=0;}
		T3dVector(const tfloat cx, const tfloat cy, const tfloat cz) {x=cx; y=cy; z=cz;} //Good constructor
		void xyz(const tfloat cx, const tfloat cy, const tfloat cz) {x=cx; y=cy; z=cz;};
		const T3dVector operator+(const T3dVector&); //Addition
		T3dVector operator+(T3dVector&); //Addition
		const T3dVector operator-(const T3dVector&); //Subtraction
		T3dVector operator-(T3dVector&); //Subtraction
		const T3dVector operator-(); //Negation
		const T3dVector operator*(const tfloat&); //s-multipy
		T3dVector operator*(tfloat&); //s-multipy
		const T3dVector operator/(const tfloat&); //s-divide
		T3dVector operator/(tfloat&); //s-divide
		tfloat operator*(const T3dVector&); //dot product
		tfloat operator*(T3dVector&); //dot product
		const T3dVector cross(const T3dVector&); //cross product
		tfloat length(); //length
		tfloat sqrlength(); //length²
		int isZero() {return (sqrlength()<VECTOR_EPSILON*VECTOR_EPSILON);}
		int normalizeCheck(); //VECTOR_OK if we could normalize, VECTOR_NORMALIZE_ERROR otherwise
		void Maximize(const T3dVector tomax);
		void Minimize(const T3dVector tomin);
		void normalize(); //Normalize without checking division by zero
		const std::string toString(); //Gives out "(x, y, z)"
		tfloat MaxAbsValue();
		tfloat MinValue();
		tfloat MaxValue();
	};


class T4dVector
	{

	public:
		tfloat x,y,z,w; //Not good OOP but who cares... no need for getters / setters
		T4dVector() {x=0; y=0; z=0; w=0;}
		T4dVector(const tfloat cx, const tfloat cy, const tfloat cz, const tfloat cw) {x=cx; y=cy; z=cz; w=cw;} //Good constructor
		void xyzw(const tfloat cx, const tfloat cy, const tfloat cz, const tfloat cw) {x=cx; y=cy; z=cz; w=cw;}
		const T4dVector operator+(const T4dVector&); //Addition
		const T4dVector operator-(const T4dVector&); //Subtraction
		const T4dVector operator-(); //Negation
		const T4dVector operator*(const tfloat&); //s-multipy
		const T4dVector operator/(const tfloat&); //s-divide
		tfloat operator*(const T4dVector&); //dot product
		tfloat length(); //length
		tfloat sqrlength(); //length²
		int normalizeCheck(); //VECTOR_OK if we could normalize, VECTOR_NORMALIZE_ERROR otherwise
		void normalize(); //Normalize without checking division by zero
		const std::string toString(); //Gives out "(x, y, z)"
	};

////////////// MATRICES ///////////////////


class T3dMatrix
	{
	protected:
		tfloat m[4*4];
	public:
		T3dMatrix(); //Fills the id-Matrix
		T3dMatrix(tfloat diag); //makes a diagonal matrix
		T3dMatrix(T3dVector side,T3dVector up,T3dVector dir);
		const T3dMatrix operator+(const T3dMatrix&); //ADD
		const T3dMatrix operator-(const T3dMatrix&); //SUBTR
		const T3dMatrix operator*(const tfloat&); //scalar MULT
		const T3dMatrix operator*(const T3dMatrix&); //matrix MULT
		const T3dVector operator*(const T3dVector&); //vector MULT
		const T2dVector operator*(const T2dVector&); //vector MULT
		const T3dVector getRow(const int i);
		void setRow(const int i,const T3dVector& v);
		const T3dVector getCol(const int i);
		void setCol(const int i,const T3dVector& v);
		void Identity();
		void setDiagonal(const T3dVector v);
		void Copy(const T3dMatrix& other);
		void makeRotX(const tfloat angle);
		void makeRotY(const tfloat angle);
		void makeRotZ(const tfloat angle);
		void makeRotV(const tfloat angle, const T3dVector axis); //Rotation along an axis
		const T3dVector getPitchYawRoll();
		void setPitchYawRoll(T3dVector pyr);
		const T3dMatrix inverse();
		const T3dMatrix transpose();
		tfloat det();
		tfloat* getValueMem() {return &(m[0]);}
		const std::string toString(); //Gives out The Matrix
	};

class T4dMatrix
	{
	protected:
		tfloat m[4*4];
	public:
		T4dMatrix(); //Fills the id-Matrix
		T4dMatrix(tfloat diag); //makes a diagonal matrix
		const T4dMatrix operator+(const T4dMatrix&); //ADD
		const T4dMatrix operator-(const T4dMatrix&); //SUBTR
		const T4dMatrix operator*(const tfloat&); //scalar MULT
		const T4dMatrix operator*(const T4dMatrix&); //matrix MULT
		const T3dVector getRow(const int i);
		void setRow(const int i,const T3dVector& v);
		const T3dVector getCol(const int i);
		void setCol(const int i,const T3dVector& v);
		void makeRotX(const tfloat angle);
		void setSubMatrix(const T3dMatrix sub);
		void getSubMatrix(T3dMatrix *sub);
		void scaleBy(const T3dVector scal);
		void scaleBy2(const T3dVector scal);

		void setFromODE(const float* p, const float* R);
		void getForODE(float* p, float* R);
		virtual void Setup(T3dVector side,T3dVector up,T3dVector dir,T3dVector pos);

		tfloat* getValueMem() {return &(m[0]);}
	};



// 2d Standard Basis. Unit Vectors
static T2dVector g_2dVectorU(1,0);
static T2dVector g_2dVectorV(0,1);

// 3d Standard Basis. Unit Vectors
static T3dVector g_3dVectorX(1,0,0);
static T3dVector g_3dVectorY(0,1,0);
static T3dVector g_3dVectorZ(0,0,1);

// 4d Standard Basis. Unit Vectors
static T4dVector g_4dVectorX(1,0,0,0);
static T4dVector g_4dVectorY(0,1,0,0);
static T4dVector g_4dVectorZ(0,0,1,0);
static T4dVector g_4dVectorW(0,0,0,1);

// Standart Matrix
//static T4dMatrix g_4dIdentityMatrix(1.0); //Diagonal 1 matrix

extern void LUA_VECTOR_RegisterLib();
extern void LUA_COLOR_RegisterLib();


#endif
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
