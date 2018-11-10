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

#pragma once

#include <string>
#include <cmath>

constexpr float VECTOR_EPSILON = 0.00000000001f;

#ifndef M_PI
#define M_PI    3.14159265358979323846f
#endif

class Vector2d
	{

	public:
		float u,v;  //Not good OOP but who cares... no need for getters / setters
		Vector2d() {u=0; v=0;}
		Vector2d(const float cu, const float cv) {u=cu; v=cv;} //Good constructor
		void uv(const float cu, const float cv) {u=cu; v=cv;}
		Vector2d operator+(const Vector2d&); //Addition
		Vector2d operator-(const Vector2d&); //Subtraction
		Vector2d operator-(); //Negation
		Vector2d operator*(const float&); //s-multipy
		Vector2d operator/(const float&); //s-divide
		float operator*(const Vector2d&); //dot product
		float length(); //length
		float sqrlength(); //length²
		bool normalizeCheck(); //true if we could normalize, false otherwise
		void normalize(); //Normalize without checking division by zero
		float Arg() {return atan2(v,u);}
		std::string toString(); //Gives out "(u, v)"
	};

class Vector3d
	{

	public:
		float x,y,z;  //Not good OOP but who cares... no need for getters / setters
		Vector3d() {x=0; y=0; z=0;}
		Vector3d(const float cx, const float cy, const float cz) {x=cx; y=cy; z=cz;} //Good constructor
		void xyz(const float cx, const float cy, const float cz) {x=cx; y=cy; z=cz;};
		const Vector3d operator+(const Vector3d&); //Addition
		Vector3d operator+(Vector3d&); //Addition
		const Vector3d operator-(const Vector3d&); //Subtraction
		Vector3d operator-(Vector3d&); //Subtraction
		const Vector3d operator-(); //Negation
		const Vector3d operator*(const float&); //s-multipy
		Vector3d operator*(float&); //s-multipy
		const Vector3d operator/(const float&); //s-divide
		Vector3d operator/(float&); //s-divide
		float operator*(const Vector3d&); //dot product
		float operator*(Vector3d&); //dot product
		const Vector3d cross(const Vector3d&); //cross product
		float length(); //length
		float sqrlength(); //length²
		int isZero() {return (sqrlength()<VECTOR_EPSILON*VECTOR_EPSILON);}
		bool normalizeCheck(); //true if we could normalize, true otherwise
		void Maximize(const Vector3d tomax);
		void Minimize(const Vector3d tomin);
		void normalize(); //Normalize without checking division by zero
		const std::string toString(); //Gives out "(x, y, z)"
		float MaxAbsValue();
		float MinValue();
		float MaxValue();
	};


class Vector4d
	{

	public:
		float x,y,z,w; //Not good OOP but who cares... no need for getters / setters
		Vector4d() {x=0; y=0; z=0; w=0;}
		Vector4d(const float cx, const float cy, const float cz, const float cw) {x=cx; y=cy; z=cz; w=cw;} //Good constructor
		void xyzw(const float cx, const float cy, const float cz, const float cw) {x=cx; y=cy; z=cz; w=cw;}
		const Vector4d operator+(const Vector4d&); //Addition
		const Vector4d operator-(const Vector4d&); //Subtraction
		const Vector4d operator-(); //Negation
		const Vector4d operator*(const float&); //s-multipy
		const Vector4d operator/(const float&); //s-divide
		float operator*(const Vector4d&); //dot product
		float length(); //length
		float sqrlength(); //length²
		bool normalizeCheck(); //true if we could normalize, false otherwise
		void normalize(); //Normalize without checking division by zero
		const std::string toString(); //Gives out "(x, y, z)"
	};

////////////// MATRICES ///////////////////


class Matrix3d
	{
	protected:
		float m[4*4];
	public:
		Matrix3d(); //Fills the id-Matrix
		Matrix3d(float diag); //makes a diagonal matrix
		Matrix3d(Vector3d side,Vector3d up,Vector3d dir);
		const Matrix3d operator+(const Matrix3d&); //ADD
		const Matrix3d operator-(const Matrix3d&); //SUBTR
		const Matrix3d operator*(const float&); //scalar MULT
		const Matrix3d operator*(const Matrix3d&); //matrix MULT
		const Vector3d operator*(const Vector3d&); //vector MULT
		const Vector2d operator*(const Vector2d&); //vector MULT
		const Vector3d getRow(const int i);
		void setRow(const int i,const Vector3d& v);
		const Vector3d getCol(const int i);
		void setCol(const int i,const Vector3d& v);
		void Identity();
		void setDiagonal(const Vector3d v);
		void Copy(const Matrix3d& other);
		void makeRotX(const float angle);
		void makeRotY(const float angle);
		void makeRotZ(const float angle);
		void makeRotV(const float angle, const Vector3d axis); //Rotation along an axis
		const Vector3d getPitchYawRoll();
		void setPitchYawRoll(Vector3d pyr);
		const Matrix3d inverse();
		const Matrix3d transpose();
		float det();
		float* getValueMem() {return &(m[0]);}
		const std::string toString(); //Gives out The Matrix
	};

class Matrix4d
	{
	protected:
		float m[4*4];
	public:
		Matrix4d(); //Fills the id-Matrix
		Matrix4d(float diag); //makes a diagonal matrix
		const Matrix4d operator+(const Matrix4d&); //ADD
		const Matrix4d operator-(const Matrix4d&); //SUBTR
		const Matrix4d operator*(const float&); //scalar MULT
		const Matrix4d operator*(const Matrix4d&); //matrix MULT
		const Vector3d getRow(const int i);
		void setRow(const int i,const Vector3d& v);
		const Vector3d getCol(const int i);
		void setCol(const int i,const Vector3d& v);
		void makeRotX(const float angle);
		void setSubMatrix(const Matrix3d sub);
		void getSubMatrix(Matrix3d *sub);
		void scaleBy(const Vector3d scal);
		void scaleBy2(const Vector3d scal);

		void setFromODE(const float* p, const float* R);
		void getForODE(float* p, float* R);
		virtual void Setup(Vector3d side,Vector3d up,Vector3d dir,Vector3d pos);

		float* getValueMem() {return &(m[0]);}
	};



// 2d Standard Basis. Unit Vectors
static Vector2d g_2dVectorU(1,0);
static Vector2d g_2dVectorV(0,1);

// 3d Standard Basis. Unit Vectors
static Vector3d g_3dVectorX(1,0,0);
static Vector3d g_3dVectorY(0,1,0);
static Vector3d g_3dVectorZ(0,0,1);

// 4d Standard Basis. Unit Vectors
static Vector4d g_4dVectorX(1,0,0,0);
static Vector4d g_4dVectorY(0,1,0,0);
static Vector4d g_4dVectorZ(0,0,1,0);
static Vector4d g_4dVectorW(0,0,0,1);

// Standart Matrix
//static T4dMatrix g_4dIdentityMatrix(1.0); //Diagonal 1 matrix

extern void LUA_VECTOR_RegisterLib();
extern void LUA_COLOR_RegisterLib();

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
