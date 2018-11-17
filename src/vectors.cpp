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

#include "vectors.hpp"
#include <string>
#include <iostream>
#include <sstream>
#include <math.h>

#include "luautils.hpp"

////////////////////////////// 2d Vectors ////////////////////////////
Vector2d Vector2d::operator+(const Vector2d& other)
	{
	Vector2d newv(u+other.u,v+other.v);
	return newv;

	}

Vector2d Vector2d::operator-(const Vector2d& other)
	{
	Vector2d newv(u-other.u,v-other.v);
	return newv;
	}

Vector2d Vector2d::operator-()
	{
	Vector2d newv(-u,-v);
	return newv;
	}

float Vector2d::operator*(const Vector2d& other)
	{
	return (u*other.u + v*other.v);
	}

Vector2d Vector2d::operator*(const float& scal)
	{
	Vector2d newv(u*scal,v*scal);
	return newv;
	}

Vector2d Vector2d::operator/(const float& scal)
	{
	Vector2d newv(u/scal,v/scal);
	return newv;
	}


float Vector2d::length()
	{
	return(sqrt(u*u+v*v));
	}

float Vector2d::sqrlength()
	{
	return((u*u+v*v));
	}

bool Vector2d::normalizeCheck()
	{
	float denom=length();
	if (denom<VECTOR_EPSILON) { return false; }
	else {
			u/=denom;
			v/=denom;
			return true;
			}
	}

void Vector2d::normalize()
	{
	float denom=length();
	u/=denom;
	v/=denom;
	}


std::string Vector2d::toString()
	{
	std::ostringstream floatStringHelper;

	floatStringHelper << "(" << u << ", " << v << ")";

	return floatStringHelper.str();
	}


////////////////////////////// 3d Vectors ////////////////////////////
const Vector3d Vector3d::operator+(const Vector3d& other)
	{
	Vector3d newv(x+other.x,y+other.y,z+other.z);
	return newv;
	}

Vector3d Vector3d::operator+(Vector3d& other)
	{
	Vector3d newv(x+other.x,y+other.y,z+other.z);
	return newv;
	}


const Vector3d Vector3d::operator-(const Vector3d& other)
	{
	Vector3d newv(x-other.x,y-other.y,z-other.z);
	return newv;
	}

Vector3d Vector3d::operator-(Vector3d& other)
	{
	Vector3d newv(x-other.x,y-other.y,z-other.z);
	return newv;
	}


const Vector3d Vector3d::operator-()
	{
	Vector3d newv(-x,-y,-z);
	return newv;
	}



float Vector3d::operator*(const Vector3d& other)
	{
	return (x*other.x + y*other.y +z*other.z);
	}

float Vector3d::operator*(Vector3d& other)
	{
	return (x*other.x + y*other.y +z*other.z);
	}

const Vector3d Vector3d::operator*(const float& scal)
	{
	Vector3d newv(x*scal,y*scal,z*scal);
	return newv;
	}

Vector3d Vector3d::operator*( float& scal)
	{
	Vector3d newv(x*scal,y*scal,z*scal);
	return newv;
	}

const Vector3d Vector3d::operator/(const float& scal)
	{
	Vector3d newv(x/scal,y/scal,z/scal);
	return newv;
	}

Vector3d Vector3d::operator/(float& scal)
	{
	Vector3d newv(x/scal,y/scal,z/scal);
	return newv;
	}


const Vector3d Vector3d::cross(const Vector3d& o)
	{
	Vector3d newv(y*o.z-z*o.y, z*o.x-x*o.z, x*o.y-y*o.x );
	return newv;
	}

float Vector3d::length()
	{
	return(sqrt(x*x+y*y+z*z));
	}

float Vector3d::sqrlength()
	{
	return((x*x+y*y+z*z));
	}

bool Vector3d::normalizeCheck()
	{
	float denom=length();
	if (denom<VECTOR_EPSILON) { return false; }
	else {
			x/=denom;
			y/=denom;
			z/=denom;
			return true;
			}
	}

void Vector3d::normalize()
	{
	float denom=length();
	x/=denom;
	y/=denom;
	z/=denom;
	}


const std::string Vector3d::toString()
	{
	std::ostringstream floatStringHelper;

	floatStringHelper << "(" << x << ", " << y << ", " << z << ")";

	return floatStringHelper.str();
	}


void Vector3d::Maximize(const Vector3d tomax)
	{
	if (tomax.x>x) { x=tomax.x; }
	if (tomax.y>y) { y=tomax.y; }
	if (tomax.z>z) { z=tomax.z; }
	}
void Vector3d::Minimize(const Vector3d tomin)
	{
	if (tomin.x<x) { x=tomin.x; }
	if (tomin.y<y) { y=tomin.y; }
	if (tomin.z<z) { z=tomin.z; }
	}


float Vector3d::MinValue()
	{
	float r=x;
	if (r>y) { r=y; }
	if (r>z) { r=z; }
	return r;
	}

float Vector3d::MaxValue()
	{
	float r=x;
	if (r<y) { r=y; }
	if (r<z) { r=z; }
	return r;
	}

float Vector3d::MaxAbsValue()
	{
	Vector3d cp(x,y,z);
	Vector3d neg=cp*(-1);
	cp.Maximize(neg);
	if ((cp.x>=cp.y) && (cp.x>=cp.z)) { return cp.x; }
	else if ((cp.y>=cp.x) && (cp.y>=cp.z)) { return cp.y; }
	else { return cp.z; }
	}


////////////////////////////// 4d Vectors ////////////////////////////
const Vector4d Vector4d::operator+(const Vector4d& other)
	{
	Vector4d newv(x+other.x,y+other.y,z+other.z,w+other.w);
	return newv;
	}

const Vector4d Vector4d::operator-(const Vector4d& other)
	{
	Vector4d newv(x-other.x,y-other.y,z-other.z,w-other.w);
	return newv;
	}

const Vector4d Vector4d::operator-()
	{
	Vector4d newv(-x,-y,-z,-w);
	return newv;
	}

float Vector4d::operator*(const Vector4d& other)
	{
	return (x*other.x + y*other.y +z*other.z +w*other.w);
	}

const Vector4d Vector4d::operator*(const float& scal)
	{
	Vector4d newv(x*scal,y*scal,z*scal,w*scal);
	return newv;
	}

const Vector4d Vector4d::operator/(const float& scal)
	{
	Vector4d newv(x/scal,y/scal,z/scal,w/scal);
	return newv;
	}

float Vector4d::length()
	{
	return(sqrt(x*x+y*y+z*z+w*w));
	}

float Vector4d::sqrlength()
	{
	return((x*x+y*y+z*z+w*w));
	}

bool Vector4d::normalizeCheck()
	{
	float denom=length();
	if (denom<VECTOR_EPSILON) { return false; }
	else {
			x/=denom;
			y/=denom;
			z/=denom;
			w/=denom;
			return true;
			}
	}

void Vector4d::normalize()
	{
	float denom=length();
	x/=denom;
	y/=denom;
	z/=denom;
	w/=denom;
	}


const std::string Vector4d::toString()
	{
	std::ostringstream floatStringHelper;

	floatStringHelper << "(" << x << ", " << y << ", " << z << ", " << w<< ")";

	return floatStringHelper.str();
	}



//////////////////////MATRICES ////////////////////////////////////

Matrix3d::Matrix3d()
	{
	for (int j=0; j<3; j++)
		for (int i=0; i<3; i++)
				{
				// m[i+3*j]= i==j ? 1 : 0;
				m[i+3*j]=0;
				}
	}

void Matrix3d::Identity()
	{
	for (int j=0; j<3; j++)
		for (int i=0; i<3; i++)
				{
				m[i+3*j]= (float)(i==j ? 1.0 : 0.0);
				//m[i+3*j]=0;
				}
	}

Matrix3d::Matrix3d(float diag)
	{
	for (int j=0; j<3; j++)
		for (int i=0; i<3; i++)
				{
				m[i+3*j]= (i==j ? diag : (float)0.0);
				//  m[i+3*j]=0;
				}
	}

void Matrix3d::Copy(const Matrix3d& other)
	{
	for (int i=0; i<16; i++) { m[i]=other.m[i]; }
	}

Matrix3d::Matrix3d(Vector3d side,Vector3d up,Vector3d dir)
	{
	setCol(0,side);
	setCol(1,up);
	setCol(2,dir);
	}

const Matrix3d Matrix3d::operator+(const Matrix3d& o)
	{
	Matrix3d newm;
	for (int i=0; i<9; i++) { newm.m[i]=m[i]+o.m[i]; }
	return newm;
	}

const Matrix3d Matrix3d::operator-(const Matrix3d& o)
	{
	Matrix3d newm;
	for (int i=0; i<9; i++) { newm.m[i]=m[i]-o.m[i]; }
	return newm;

	}

const Matrix3d Matrix3d::operator*(const float& s)
	{
	Matrix3d newm;
	for (int i=0; i<9; i++) { newm.m[i]=m[i]*s; }
	return newm;
	}

const Vector3d Matrix3d::operator*(const Vector3d& v)
	{
	Vector3d newv,t;
	t=getRow(0);
	newv.x=t*v;
	t=getRow(1);
	newv.y=t*v;
	t=getRow(2);
	newv.z=t*v;
	return newv;
	}

const Vector2d Matrix3d::operator*(const Vector2d& nv)
	{
	Vector3d newv,t,v;
	v.xyz(nv.u,nv.v,1.0);
	t=getRow(0);
	newv.x=t*v;
	t=getRow(1);
	newv.y=t*v;
	t=getRow(2);
	newv.z=t*v;
	Vector2d res;
	res.uv(newv.x,newv.y);
	return res;
	}

const Matrix3d Matrix3d::operator*(const Matrix3d& o)
	{
	int i, j, k;
	Matrix3d newm(0.0);
	for(i=0; i<3; i++)
			{
			for(j=0; j<3; j++)
					{
					for(k=0; k<3; k++)
							{
							newm.m[i+3*j] += m[i+3*k] * o.m[k+3*j];
							}
					}
			}
	return newm;
	}




const Vector3d Matrix3d::getRow(const int i)
	{
	Vector3d nv(m[i],m[i+3],m[i+6]);
	return nv;
	}

void Matrix3d::setDiagonal(const Vector3d v)
	{
	m[0]=v.x;
	m[3+1]=v.y;
	m[6+2]=v.z;
	}

void Matrix3d::setRow(const int i,const Vector3d& v)
	{
	m[i]=v.x;
	m[i+3]=v.y;
	m[i+6]=v.z;
	}


const Vector3d Matrix3d::getCol(const int i)
	{
	Vector3d nv(m[3*i],m[3*i+1],m[3*i+2]);
	return nv;
	}

void Matrix3d::setCol(const int i,const Vector3d& v)
	{
	m[3*i]=v.x;
	m[3*i+1]=v.y;
	m[3*i+2]=v.z;
	}

void Matrix3d::makeRotX(const float angle)
	{
	float s=sin(angle);
	float c=cos(angle);
	m[1+3]=c;
	m[2+3]=-s;
	m[1+2*3]=s;
	m[2+2*3]=c;
	}

void Matrix3d::makeRotV(const float angle,const Vector3d axis)
	{
	float rcos = cos(angle);
	float rsin = sin(angle);
	float oneminuscos=1-rcos;
	m[0+3*0] =      rcos + axis.x*axis.x*oneminuscos;
	m[1+3*0] =  axis.z * rsin + axis.y*axis.x*oneminuscos;
	m[2+3*0] = -axis.y * rsin + axis.z*axis.x*oneminuscos;
	m[0+3*1] = -axis.z * rsin + axis.x*axis.y*oneminuscos;
	m[1+3*1] =      rcos + axis.y*axis.y*oneminuscos;
	m[2+3*1] =  axis.x * rsin + axis.z*axis.y*oneminuscos;
	m[0+3*2] =  axis.y * rsin + axis.x*axis.z*oneminuscos;
	m[1+3*2] = -axis.x * rsin + axis.y*axis.z*oneminuscos;
	m[2+3*2] =      rcos + axis.z*axis.z*oneminuscos;
	}



const Vector3d Matrix3d::getPitchYawRoll()
	{

	Vector3d newv;
	newv.x=atan2(-m[1+2*3],sqrt(m[0+2*3]*m[0+2*3]+m[2+2*3]*m[2+2*3]));
	if ((newv.x-M_PI/2)*(newv.x-M_PI/2)<VECTOR_EPSILON)
			{	newv.z=0;
			newv.y=atan2(m[0+1*3],m[2+1*3]);
			return newv;
			}
	if ((newv.x+M_PI/2)*(newv.x+M_PI/2)<VECTOR_EPSILON)
			{
			newv.z=0;
			newv.y=-atan2(m[0+1*3],m[2+1*3]);
			if (newv.y>3.14159 || newv.y<-3.14159) { newv.y=0; } //Just a small hack
			//cout << "SPECIAL A  " << m[0+1*3]<< "  " << m[2+1*3] << endl;
			return newv;
			}
	float ic=((float)1.0)/cos(newv.x);
	newv.y=atan2(m[0+2*3]*ic,m[2+2*3]*ic);
	newv.z=atan2(m[1+0*3]*ic,m[1+1*3]*ic);
	return newv;
	}




void Matrix3d::setPitchYawRoll(Vector3d pyr)
	{
	float cx,sx,cy,sy,cz,sz;
	cx=cos(pyr.x); sx=sin(pyr.x);
	cy=cos(pyr.y); sy=sin(pyr.y);
	cz=cos(pyr.z); sz=sin(pyr.z);
	m[0+0*3]=cz*cy+sz*sy*sx;
	m[1+0*3]=cx*sz;
	m[2+0*3]=-sy*cz+cy*sx*sz;

	m[0+1*3]=-sz*cy+cz*sy*sx;
	m[1+1*3]=cx*cz;
	m[2+1*3]=sy*sz+cz*cy*sx;

	m[0+2*3]=sy*cx;
	m[1+2*3]=-sx;
	m[2+2*3]=cx*cy;

	}


void Matrix3d::makeRotY(const float angle)
	{
	float s=sin(angle);
	float c=cos(angle);
	m[0+3*0]=c;
	m[0+3*2]=s;
	m[2+3*0]=-s;
	m[2+3*2]=c;
	}

void Matrix3d::makeRotZ(const float angle)
	{
	float s=sin(angle);
	float c=cos(angle);
	m[0+0*3]=c;
	m[0+1*3]=-s;
	m[1+0*3]=s;
	m[1+1*3]=c;
	}

float Matrix3d::det()
	{
	float d;
	d=m[0+3*0]*m[1+3*1]*m[2+3*2]
			+m[1+3*0]*m[2+3*1]*m[0+3*2]
			+m[2+3*0]*m[0+3*1]*m[1+3*2]

			-m[2+3*0]*m[1+3*1]*m[0+3*2]
			-m[1+3*0]*m[0+3*1]*m[2+3*2]
			-m[0+3*0]*m[2+3*1]*m[1+3*2] ;
	return d;
	}

const Matrix3d Matrix3d::inverse()
	{
	float d=det();
	Matrix3d result2(0.0);
	if (d*d < VECTOR_EPSILON)
			{
			return result2;
			}
	float invdet=((float)1.0)/d;
	result2.m[0+3*0] =  (m[1+3*1]*m[2+3*2]-m[2+3*1]*m[1+3*2])*invdet;
	result2.m[0+3*1] = -(m[0+3*1]*m[2+3*2]-m[0+3*2]*m[2+3*1])*invdet;
	result2.m[0+3*2] =  (m[0+3*1]*m[1+3*2]-m[0+3*2]*m[1+3*1])*invdet;
	result2.m[1+3*0] = -(m[1+3*0]*m[2+3*2]-m[1+3*2]*m[2+3*0])*invdet;
	result2.m[1+3*1] =  (m[0+3*0]*m[2+3*2]-m[0+3*2]*m[2+3*0])*invdet;
	result2.m[1+3*2] = -(m[0+3*0]*m[1+3*2]-m[1+3*0]*m[0+3*2])*invdet;
	result2.m[2+3*0] =  (m[1+3*0]*m[2+3*1]-m[2+3*0]*m[1+3*1])*invdet;
	result2.m[2+3*1] = -(m[0+3*0]*m[2+3*1]-m[2+3*0]*m[0+3*1])*invdet;
	result2.m[2+3*2] =  (m[0+3*0]*m[1+3*1]-m[1+3*0]*m[0+3*1])*invdet;
	return result2;
	}


const Matrix3d Matrix3d::transpose()
	{
	Matrix3d result2(0.0);
	for (int i=0; i<3; i++)
		for (int j=0; j<3; j++) {
				result2.m[i+3*j]=m[j+3*i];
				}
	return result2;
	}

const std::string Matrix3d::toString()
	{
	std::ostringstream floatStringHelper;
	Vector3d v;
	v=getRow(0);
	floatStringHelper << v.toString() << std::endl;
	v=getRow(1);
	floatStringHelper << v.toString() << std::endl;
	v=getRow(2);
	floatStringHelper << v.toString() << std::endl;

	return floatStringHelper.str();
	}

///4D////////////////

void Matrix4d::setFromODE(const float* p, const float* R)
	{
	m[0]  = R[0]; m[1]  = R[4]; m[2]  = R[8];  m[3]  = 0;
	m[4]  = R[1]; m[5]  = R[5]; m[6]  = R[9];  m[7]  = 0;
	m[8]  = R[2]; m[9]  = R[6]; m[10] = R[10]; m[11] = 0;
	m[12] = p[0]; m[13] = p[1]; m[14] = p[2];  m[15] = 1;
	}

void Matrix4d::getForODE(float* p, float* R)
	{
	R[0]  = m[0]; R[4]  = m[1]; R[8]  = m[2];
	R[1]  = m[4]; R[5]  = m[5]; R[9]  = m[6];
	R[2]  = m[8]; R[6]  = m[9]; R[10] = m[10];
	p[0] = m[12]; p[1] = m[13]; p[2] = m[14];

	R[3]=0;
	}

void Matrix4d::setSubMatrix(Matrix3d sub)
	{
	float *mom=sub.getValueMem();
	for (int j=0; j<3; j++)
		for (int i=0; i<3; i++)
				{
				// m[i+4*j]= i==j ? 1 : 0;

				m[i+4*j]=mom[i+3*j];
				}
	}

void Matrix4d::getSubMatrix(Matrix3d *sub)
	{
	float *mom=sub->getValueMem();
	for (int j=0; j<3; j++)
		for (int i=0; i<3; i++)
				{
				// m[i+4*j]= i==j ? 1 : 0;

				mom[i+3*j]=m[i+4*j];
				}
	}

void Matrix4d::scaleBy(const Vector3d scal)
	{
	m[0]*=scal.x;
	m[1]*=scal.x;
	m[2]*=scal.x;
	m[0+4*1]*=scal.y;
	m[1+4*1]*=scal.y;
	m[2+4*1]*=scal.y;
	m[0+4*2]*=scal.z;
	m[1+4*2]*=scal.z;
	m[2+4*2]*=scal.z;
	}

void Matrix4d::scaleBy2(const Vector3d scal)
	{
	m[0]*=scal.x;
	m[1]*=scal.y;
	m[2]*=scal.z;
	m[0+4*1]*=scal.x;
	m[1+4*1]*=scal.y;
	m[2+4*1]*=scal.z;
	m[0+4*2]*=scal.x;
	m[1+4*2]*=scal.y;
	m[2+4*2]*=scal.z;
	}

Matrix4d::Matrix4d()
	{
	for (int j=0; j<4; j++)
		for (int i=0; i<4; i++)
				{
				// m[i+4*j]= i==j ? 1 : 0;
				m[i+4*j]=0;
				}
	}

Matrix4d::Matrix4d(float diag)
	{
	for (int j=0; j<4; j++)
		for (int i=0; i<4; i++)
				{
				m[i+4*j]= i==j ? diag : 0;
				//  m[i+4*j]=0;
				}
	}

const Matrix4d Matrix4d::operator+(const Matrix4d& o)
	{
	Matrix4d newm;
	for (int i=0; i<16; i++) { newm.m[i]=m[i]+o.m[i]; }
	return newm;
	}

const Matrix4d Matrix4d::operator-(const Matrix4d& o)
	{
	Matrix4d newm;
	for (int i=0; i<16; i++) { newm.m[i]=m[i]-o.m[i]; }
	return newm;

	}

const Matrix4d Matrix4d::operator*(const float& s)
	{
	Matrix4d newm;
	for (int i=0; i<16; i++) { newm.m[i]=m[i]*s; }
	return newm;
	}

const Matrix4d Matrix4d::operator*(const Matrix4d& o)
	{
	int i, j, k;
	Matrix4d newm;
	for(i=0; i<4; i++)
			{
			for(j=0; j<4; j++)
					{
					for(k=0; k<4; k++)
							{
							newm.m[i+4*j] += m[i+4*k] * o.m[k+4*j];
							}
					}
			}
	return newm;
	}


const Vector3d Matrix4d::getRow(const int i)
	{
	Vector3d nv(m[i],m[i+4],m[i+8]);
	return nv;
	}

void Matrix4d::setRow(const int i,const Vector3d& v)
	{
	m[i]=v.x;
	m[i+4]=v.y;
	m[i+8]=v.z;
	}


const Vector3d Matrix4d::getCol(const int i)
	{
	Vector3d nv(m[4*i],m[4*i+1],m[4*i+2]);
	return nv;
	}

void Matrix4d::setCol(const int i,const Vector3d& v)
	{
	m[4*i]=v.x;
	m[4*i+1]=v.y;
	m[4*i+2]=v.z;
	}

void Matrix4d::makeRotX(const float angle)
	{
	float s=sin(angle);
	float c=cos(angle);
	m[5]=c;
	m[6]=-s;
	m[9]=s;
	m[10]=c;
	}


void Matrix4d::Setup(Vector3d side,Vector3d up,Vector3d dir,Vector3d pos)
	{
	setCol(0,side);
	setCol(1,up);
	setCol(2,dir);
	//setPos(pos);
	}


//////////////LUA-IMLEMENT//////////////////////////////


int VECTOR_New(lua_State *state)
	{
	float z=(float)LUA_GET_DOUBLE(state);
	float y=(float)LUA_GET_DOUBLE(state);
	float x=(float)LUA_GET_DOUBLE(state);
	Vector3d v;
	v.xyz(x,y,z);
	LUA_SET_VECTOR3(state, v);
	return 1;
	}

int VECTOR_Scale(lua_State *state)
	{
	float s=LUA_GET_DOUBLE(state);
	Vector3d v=Vector3FromStack(state);
	v=v*s;
	LUA_SET_VECTOR3(state, v);
	return 1;
	}

int VECTOR_Add(lua_State *state)
	{
	Vector3d v2=Vector3FromStack(state);
	Vector3d v1=Vector3FromStack(state);
	v1=v1+v2;
	LUA_SET_VECTOR3(state, v1);
	return 1;
	}

int VECTOR_Dot(lua_State *state)
	{
	Vector3d v2=Vector3FromStack(state);
	Vector3d v1=Vector3FromStack(state);
	double d=v1*v2;
	LUA_SET_DOUBLE(state, d);
	return 1;
	}


int VECTOR_Sub(lua_State *state)
	{
	Vector3d v2=Vector3FromStack(state);
	Vector3d v1=Vector3FromStack(state);
	v1=v1-v2;
	LUA_SET_VECTOR3(state, v1);
	return 1;
	}

int VECTOR_ToString(lua_State *state)
	{
	Vector3d v1=Vector3FromStack(state);
	std::string s=v1.toString();
	LUA_SET_STRING(state, s);
	return 1;
	}



int VECTOR_Length(lua_State *state)
	{
	Vector3d v1=Vector3FromStack(state);
	double res=v1.length();
	LUA_SET_DOUBLE(state, res);
	return 1;
	}

int VECTOR_Cross(lua_State *state)
	{
	Vector3d v2=Vector3FromStack(state);
	Vector3d v1=Vector3FromStack(state);
	Vector3d v3=v1.cross(v2);
	LUA_SET_VECTOR3(state, v3);
	return 1;
	}



void LUA_VECTOR_RegisterLib()
	{
	g_CuboLib()->AddFunc("VECTOR_New",VECTOR_New);
	g_CuboLib()->AddFunc("VECTOR_Scale",VECTOR_Scale);
	g_CuboLib()->AddFunc("VECTOR_Add",VECTOR_Add);
	g_CuboLib()->AddFunc("VECTOR_Dot",VECTOR_Dot);
	g_CuboLib()->AddFunc("VECTOR_Cross",VECTOR_Cross);
	g_CuboLib()->AddFunc("VECTOR_Sub",VECTOR_Sub);
	g_CuboLib()->AddFunc("VECTOR_Length",VECTOR_Length);
	g_CuboLib()->AddFunc("VECTOR_ToString",VECTOR_ToString);
	}


/////////////////////


int COLOR_New(lua_State *state)
	{
	float a=(float)LUA_GET_DOUBLE(state);
	float b=(float)LUA_GET_DOUBLE(state);
	float g=(float)LUA_GET_DOUBLE(state);
	float r=(float)LUA_GET_DOUBLE(state);
	Vector4d v;
	v.xyzw(r,g,b,a);
	LUA_SET_COLOR(state, v);
	return 1;
	}

void LUA_COLOR_RegisterLib()
	{
	g_CuboLib()->AddFunc("COLOR_New",COLOR_New);
	}




// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
