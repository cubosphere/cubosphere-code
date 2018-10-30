/**
Copyright (C) 2010 Chriddo

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
T2dVector T2dVector::operator+(const T2dVector& other)
	{
	T2dVector newv(u+other.u,v+other.v);
	return newv;

	}

T2dVector T2dVector::operator-(const T2dVector& other)
	{
	T2dVector newv(u-other.u,v-other.v);
	return newv;
	}

T2dVector T2dVector::operator-()
	{
	T2dVector newv(-u,-v);
	return newv;
	}

tfloat T2dVector::operator*(const T2dVector& other)
	{
	return (u*other.u + v*other.v);
	}

T2dVector T2dVector::operator*(const tfloat& scal)
	{
	T2dVector newv(u*scal,v*scal);
	return newv;
	}

T2dVector T2dVector::operator/(const tfloat& scal)
	{
	T2dVector newv(u/scal,v/scal);
	return newv;
	}


tfloat T2dVector::length()
	{
	return(sqrt(u*u+v*v));
	}

tfloat T2dVector::sqrlength()
	{
	return((u*u+v*v));
	}

int T2dVector::normalizeCheck()
	{
	tfloat denom=length();
	if (denom<VECTOR_EPSILON) return VECTOR_NORMALIZE_ERROR;
	else {
			u/=denom;
			v/=denom;
			return VECTOR_OK;
			}
	}

void T2dVector::normalize()
	{
	tfloat denom=length();
	u/=denom;
	v/=denom;
	}


std::string T2dVector::toString()
	{
	std::ostringstream floatStringHelper;

	floatStringHelper << "(" << u << ", " << v << ")";

	return floatStringHelper.str();
	}


////////////////////////////// 3d Vectors ////////////////////////////
const T3dVector T3dVector::operator+(const T3dVector& other)
	{
	T3dVector newv(x+other.x,y+other.y,z+other.z);
	return newv;
	}

T3dVector T3dVector::operator+(T3dVector& other)
	{
	T3dVector newv(x+other.x,y+other.y,z+other.z);
	return newv;
	}


const T3dVector T3dVector::operator-(const T3dVector& other)
	{
	T3dVector newv(x-other.x,y-other.y,z-other.z);
	return newv;
	}

T3dVector T3dVector::operator-(T3dVector& other)
	{
	T3dVector newv(x-other.x,y-other.y,z-other.z);
	return newv;
	}


const T3dVector T3dVector::operator-()
	{
	T3dVector newv(-x,-y,-z);
	return newv;
	}



tfloat T3dVector::operator*(const T3dVector& other)
	{
	return (x*other.x + y*other.y +z*other.z);
	}

tfloat T3dVector::operator*(T3dVector& other)
	{
	return (x*other.x + y*other.y +z*other.z);
	}

const T3dVector T3dVector::operator*(const tfloat& scal)
	{
	T3dVector newv(x*scal,y*scal,z*scal);
	return newv;
	}

T3dVector T3dVector::operator*( tfloat& scal)
	{
	T3dVector newv(x*scal,y*scal,z*scal);
	return newv;
	}

const T3dVector T3dVector::operator/(const tfloat& scal)
	{
	T3dVector newv(x/scal,y/scal,z/scal);
	return newv;
	}

T3dVector T3dVector::operator/(tfloat& scal)
	{
	T3dVector newv(x/scal,y/scal,z/scal);
	return newv;
	}


const T3dVector T3dVector::cross(const T3dVector& o)
	{
	T3dVector newv(y*o.z-z*o.y, z*o.x-x*o.z, x*o.y-y*o.x );
	return newv;
	}

tfloat T3dVector::length()
	{
	return(sqrt(x*x+y*y+z*z));
	}

tfloat T3dVector::sqrlength()
	{
	return((x*x+y*y+z*z));
	}

int T3dVector::normalizeCheck()
	{
	tfloat denom=length();
	if (denom<VECTOR_EPSILON) return VECTOR_NORMALIZE_ERROR;
	else {
			x/=denom;
			y/=denom;
			z/=denom;
			return VECTOR_OK;
			}
	}

void T3dVector::normalize()
	{
	tfloat denom=length();
	x/=denom;
	y/=denom;
	z/=denom;
	}


const std::string T3dVector::toString()
	{
	std::ostringstream floatStringHelper;

	floatStringHelper << "(" << x << ", " << y << ", " << z << ")";

	return floatStringHelper.str();
	}


void T3dVector::Maximize(const T3dVector tomax)
	{
	if (tomax.x>x) x=tomax.x;
	if (tomax.y>y) y=tomax.y;
	if (tomax.z>z) z=tomax.z;
	}
void T3dVector::Minimize(const T3dVector tomin)
	{
	if (tomin.x<x) x=tomin.x;
	if (tomin.y<y) y=tomin.y;
	if (tomin.z<z) z=tomin.z;
	}


tfloat T3dVector::MinValue()
	{
	tfloat r=x;
	if (r>y) r=y;
	if (r>z) r=z;
	return r;
	}

tfloat T3dVector::MaxValue()
	{
	tfloat r=x;
	if (r<y) r=y;
	if (r<z) r=z;
	return r;
	}

tfloat T3dVector::MaxAbsValue()
	{
	T3dVector cp(x,y,z);
	T3dVector neg=cp*(-1);
	cp.Maximize(neg);
	if ((cp.x>=cp.y) && (cp.x>=cp.z)) return cp.x;
	else if ((cp.y>=cp.x) && (cp.y>=cp.z)) return cp.y;
	else return cp.z;
	}


////////////////////////////// 4d Vectors ////////////////////////////
const T4dVector T4dVector::operator+(const T4dVector& other)
	{
	T4dVector newv(x+other.x,y+other.y,z+other.z,w+other.w);
	return newv;
	}

const T4dVector T4dVector::operator-(const T4dVector& other)
	{
	T4dVector newv(x-other.x,y-other.y,z-other.z,w-other.w);
	return newv;
	}

const T4dVector T4dVector::operator-()
	{
	T4dVector newv(-x,-y,-z,-w);
	return newv;
	}

tfloat T4dVector::operator*(const T4dVector& other)
	{
	return (x*other.x + y*other.y +z*other.z +w*other.w);
	}

const T4dVector T4dVector::operator*(const tfloat& scal)
	{
	T4dVector newv(x*scal,y*scal,z*scal,w*scal);
	return newv;
	}

const T4dVector T4dVector::operator/(const tfloat& scal)
	{
	T4dVector newv(x/scal,y/scal,z/scal,w/scal);
	return newv;
	}

tfloat T4dVector::length()
	{
	return(sqrt(x*x+y*y+z*z+w*w));
	}

tfloat T4dVector::sqrlength()
	{
	return((x*x+y*y+z*z+w*w));
	}

int T4dVector::normalizeCheck()
	{
	tfloat denom=length();
	if (denom<VECTOR_EPSILON) return VECTOR_NORMALIZE_ERROR;
	else {
			x/=denom;
			y/=denom;
			z/=denom;
			w/=denom;
			return VECTOR_OK;
			}
	}

void T4dVector::normalize()
	{
	tfloat denom=length();
	x/=denom;
	y/=denom;
	z/=denom;
	w/=denom;
	}


const std::string T4dVector::toString()
	{
	std::ostringstream floatStringHelper;

	floatStringHelper << "(" << x << ", " << y << ", " << z << ", " << w<< ")";

	return floatStringHelper.str();
	}



//////////////////////MATRICES ////////////////////////////////////

T3dMatrix::T3dMatrix()
	{
	for (int j=0; j<3; j++)
		for (int i=0; i<3; i++)
				{
				// m[i+3*j]= i==j ? 1 : 0;
				m[i+3*j]=0;
				}
	}

void T3dMatrix::Identity()
	{
	for (int j=0; j<3; j++)
		for (int i=0; i<3; i++)
				{
				m[i+3*j]= (tfloat)(i==j ? 1.0 : 0.0);
				//m[i+3*j]=0;
				}
	}

T3dMatrix::T3dMatrix(tfloat diag)
	{
	for (int j=0; j<3; j++)
		for (int i=0; i<3; i++)
				{
				m[i+3*j]= (i==j ? diag : (tfloat)0.0);
				//  m[i+3*j]=0;
				}
	}

void T3dMatrix::Copy(const T3dMatrix& other)
	{
	for (int i=0; i<16; i++) m[i]=other.m[i];
	}

T3dMatrix::T3dMatrix(T3dVector side,T3dVector up,T3dVector dir)
	{
	setCol(0,side);
	setCol(1,up);
	setCol(2,dir);
	}

const T3dMatrix T3dMatrix::operator+(const T3dMatrix& o)
	{
	T3dMatrix newm;
	for (int i=0; i<9; i++) newm.m[i]=m[i]+o.m[i];
	return newm;
	}

const T3dMatrix T3dMatrix::operator-(const T3dMatrix& o)
	{
	T3dMatrix newm;
	for (int i=0; i<9; i++) newm.m[i]=m[i]-o.m[i];
	return newm;

	}

const T3dMatrix T3dMatrix::operator*(const tfloat& s)
	{
	T3dMatrix newm;
	for (int i=0; i<9; i++) newm.m[i]=m[i]*s;
	return newm;
	}

const T3dVector T3dMatrix::operator*(const T3dVector& v)
	{
	T3dVector newv,t;
	t=getRow(0);
	newv.x=t*v;
	t=getRow(1);
	newv.y=t*v;
	t=getRow(2);
	newv.z=t*v;
	return newv;
	}

const T2dVector T3dMatrix::operator*(const T2dVector& nv)
	{
	T3dVector newv,t,v;
	v.xyz(nv.u,nv.v,1.0);
	t=getRow(0);
	newv.x=t*v;
	t=getRow(1);
	newv.y=t*v;
	t=getRow(2);
	newv.z=t*v;
	T2dVector res;
	res.uv(newv.x,newv.y);
	return res;
	}

const T3dMatrix T3dMatrix::operator*(const T3dMatrix& o)
	{
	int i, j, k;
	T3dMatrix newm(0.0);
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




const T3dVector T3dMatrix::getRow(const int i)
	{
	T3dVector nv(m[i],m[i+3],m[i+6]);
	return nv;
	}

void T3dMatrix::setDiagonal(const T3dVector v)
	{
	m[0]=v.x;
	m[3+1]=v.y;
	m[6+2]=v.z;
	}

void T3dMatrix::setRow(const int i,const T3dVector& v)
	{
	m[i]=v.x;
	m[i+3]=v.y;
	m[i+6]=v.z;
	}


const T3dVector T3dMatrix::getCol(const int i)
	{
	T3dVector nv(m[3*i],m[3*i+1],m[3*i+2]);
	return nv;
	}

void T3dMatrix::setCol(const int i,const T3dVector& v)
	{
	m[3*i]=v.x;
	m[3*i+1]=v.y;
	m[3*i+2]=v.z;
	}

void T3dMatrix::makeRotX(const tfloat angle)
	{
	tfloat s=sin(angle);
	tfloat c=cos(angle);
	m[1+3]=c;
	m[2+3]=-s;
	m[1+2*3]=s;
	m[2+2*3]=c;
	}

void T3dMatrix::makeRotV(const tfloat angle,const T3dVector axis)
	{
	tfloat rcos = cos(angle);
	tfloat rsin = sin(angle);
	tfloat oneminuscos=1-rcos;
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



const T3dVector T3dMatrix::getPitchYawRoll()
	{

	T3dVector newv;
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
			if (newv.y>3.14159 || newv.y<-3.14159) newv.y=0; //Just a small hack
			//cout << "SPECIAL A  " << m[0+1*3]<< "  " << m[2+1*3] << endl;
			return newv;
			}
	tfloat ic=((tfloat)1.0)/cos(newv.x);
	newv.y=atan2(m[0+2*3]*ic,m[2+2*3]*ic);
	newv.z=atan2(m[1+0*3]*ic,m[1+1*3]*ic);
	return newv;
	}




void T3dMatrix::setPitchYawRoll(T3dVector pyr)
	{
	tfloat cx,sx,cy,sy,cz,sz;
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


void T3dMatrix::makeRotY(const tfloat angle)
	{
	tfloat s=sin(angle);
	tfloat c=cos(angle);
	m[0+3*0]=c;
	m[0+3*2]=s;
	m[2+3*0]=-s;
	m[2+3*2]=c;
	}

void T3dMatrix::makeRotZ(const tfloat angle)
	{
	tfloat s=sin(angle);
	tfloat c=cos(angle);
	m[0+0*3]=c;
	m[0+1*3]=-s;
	m[1+0*3]=s;
	m[1+1*3]=c;
	}

tfloat T3dMatrix::det()
	{
	tfloat d;
	d=m[0+3*0]*m[1+3*1]*m[2+3*2]
			+m[1+3*0]*m[2+3*1]*m[0+3*2]
			+m[2+3*0]*m[0+3*1]*m[1+3*2]

			-m[2+3*0]*m[1+3*1]*m[0+3*2]
			-m[1+3*0]*m[0+3*1]*m[2+3*2]
			-m[0+3*0]*m[2+3*1]*m[1+3*2] ;
	return d;
	}

const T3dMatrix T3dMatrix::inverse()
	{
	tfloat d=det();
	T3dMatrix result2(0.0);
	if (d*d < VECTOR_EPSILON)
			{
			return result2;
			}
	tfloat invdet=((tfloat)1.0)/d;
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


const T3dMatrix T3dMatrix::transpose()
	{
	T3dMatrix result2(0.0);
	for (int i=0; i<3; i++)
		for (int j=0; j<3; j++)
			result2.m[i+3*j]=m[j+3*i];
	return result2;
	}

const std::string T3dMatrix::toString()
	{
	std::ostringstream floatStringHelper;
	T3dVector v;
	v=getRow(0);
	floatStringHelper << v.toString() << std::endl;
	v=getRow(1);
	floatStringHelper << v.toString() << std::endl;
	v=getRow(2);
	floatStringHelper << v.toString() << std::endl;

	return floatStringHelper.str();
	}

///4D////////////////

void T4dMatrix::setFromODE(const float* p, const float* R)
	{
	m[0]  = R[0]; m[1]  = R[4]; m[2]  = R[8];  m[3]  = 0;
	m[4]  = R[1]; m[5]  = R[5]; m[6]  = R[9];  m[7]  = 0;
	m[8]  = R[2]; m[9]  = R[6]; m[10] = R[10]; m[11] = 0;
	m[12] = p[0]; m[13] = p[1]; m[14] = p[2];  m[15] = 1;
	}

void T4dMatrix::getForODE(float* p, float* R)
	{
	R[0]  = m[0]; R[4]  = m[1]; R[8]  = m[2];
	R[1]  = m[4]; R[5]  = m[5]; R[9]  = m[6];
	R[2]  = m[8]; R[6]  = m[9]; R[10] = m[10];
	p[0] = m[12]; p[1] = m[13]; p[2] = m[14];

	R[3]=0;
	}

void T4dMatrix::setSubMatrix(T3dMatrix sub)
	{
	tfloat *mom=sub.getValueMem();
	for (int j=0; j<3; j++)
		for (int i=0; i<3; i++)
				{
				// m[i+4*j]= i==j ? 1 : 0;

				m[i+4*j]=mom[i+3*j];
				}
	}

void T4dMatrix::getSubMatrix(T3dMatrix *sub)
	{
	tfloat *mom=sub->getValueMem();
	for (int j=0; j<3; j++)
		for (int i=0; i<3; i++)
				{
				// m[i+4*j]= i==j ? 1 : 0;

				mom[i+3*j]=m[i+4*j];
				}
	}

void T4dMatrix::scaleBy(const T3dVector scal)
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

void T4dMatrix::scaleBy2(const T3dVector scal)
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

T4dMatrix::T4dMatrix()
	{
	for (int j=0; j<4; j++)
		for (int i=0; i<4; i++)
				{
				// m[i+4*j]= i==j ? 1 : 0;
				m[i+4*j]=0;
				}
	}

T4dMatrix::T4dMatrix(tfloat diag)
	{
	for (int j=0; j<4; j++)
		for (int i=0; i<4; i++)
				{
				m[i+4*j]= i==j ? diag : 0;
				//  m[i+4*j]=0;
				}
	}

const T4dMatrix T4dMatrix::operator+(const T4dMatrix& o)
	{
	T4dMatrix newm;
	for (int i=0; i<16; i++) newm.m[i]=m[i]+o.m[i];
	return newm;
	}

const T4dMatrix T4dMatrix::operator-(const T4dMatrix& o)
	{
	T4dMatrix newm;
	for (int i=0; i<16; i++) newm.m[i]=m[i]-o.m[i];
	return newm;

	}

const T4dMatrix T4dMatrix::operator*(const tfloat& s)
	{
	T4dMatrix newm;
	for (int i=0; i<16; i++) newm.m[i]=m[i]*s;
	return newm;
	}

const T4dMatrix T4dMatrix::operator*(const T4dMatrix& o)
	{
	int i, j, k;
	T4dMatrix newm;
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


const T3dVector T4dMatrix::getRow(const int i)
	{
	T3dVector nv(m[i],m[i+4],m[i+8]);
	return nv;
	}

void T4dMatrix::setRow(const int i,const T3dVector& v)
	{
	m[i]=v.x;
	m[i+4]=v.y;
	m[i+8]=v.z;
	}


const T3dVector T4dMatrix::getCol(const int i)
	{
	T3dVector nv(m[4*i],m[4*i+1],m[4*i+2]);
	return nv;
	}

void T4dMatrix::setCol(const int i,const T3dVector& v)
	{
	m[4*i]=v.x;
	m[4*i+1]=v.y;
	m[4*i+2]=v.z;
	}

void T4dMatrix::makeRotX(const tfloat angle)
	{
	tfloat s=sin(angle);
	tfloat c=cos(angle);
	m[5]=c;
	m[6]=-s;
	m[9]=s;
	m[10]=c;
	}


void T4dMatrix::Setup(T3dVector side,T3dVector up,T3dVector dir,T3dVector pos)
	{
	setCol(0,side);
	setCol(1,up);
	setCol(2,dir);
	//setPos(pos);
	}


//////////////LUA-IMLEMENT//////////////////////////////


int VECTOR_New(lua_State *state)
	{
	float z=(float)LUA_GET_DOUBLE;
	float y=(float)LUA_GET_DOUBLE;
	float x=(float)LUA_GET_DOUBLE;
	T3dVector v;
	v.xyz(x,y,z);
	LUA_SET_VECTOR3(v);
	return 1;
	}

int VECTOR_Scale(lua_State *state)
	{
	float s=LUA_GET_DOUBLE;
	T3dVector v=Vector3FromStack(state);
	v=v*s;
	LUA_SET_VECTOR3(v);
	return 1;
	}

int VECTOR_Add(lua_State *state)
	{
	T3dVector v2=Vector3FromStack(state);
	T3dVector v1=Vector3FromStack(state);
	v1=v1+v2;
	LUA_SET_VECTOR3(v1);
	return 1;
	}

int VECTOR_Dot(lua_State *state)
	{
	T3dVector v2=Vector3FromStack(state);
	T3dVector v1=Vector3FromStack(state);
	double d=v1*v2;
	LUA_SET_DOUBLE(d);
	return 1;
	}


int VECTOR_Sub(lua_State *state)
	{
	T3dVector v2=Vector3FromStack(state);
	T3dVector v1=Vector3FromStack(state);
	v1=v1-v2;
	LUA_SET_VECTOR3(v1);
	return 1;
	}

int VECTOR_ToString(lua_State *state)
	{
	T3dVector v1=Vector3FromStack(state);
	std::string s=v1.toString();
	LUA_SET_STRING(s);
	return 1;
	}



int VECTOR_Length(lua_State *state)
	{
	T3dVector v1=Vector3FromStack(state);
	double res=v1.length();
	LUA_SET_DOUBLE(res);
	return 1;
	}

int VECTOR_Cross(lua_State *state)
	{
	T3dVector v2=Vector3FromStack(state);
	T3dVector v1=Vector3FromStack(state);
	T3dVector v3=v1.cross(v2);
	LUA_SET_VECTOR3(v3);
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
	float a=(float)LUA_GET_DOUBLE;
	float b=(float)LUA_GET_DOUBLE;
	float g=(float)LUA_GET_DOUBLE;
	float r=(float)LUA_GET_DOUBLE;
	T4dVector v;
	v.xyzw(r,g,b,a);
	LUA_SET_COLOR(v);
	return 1;
	}

void LUA_COLOR_RegisterLib()
	{
	g_CuboLib()->AddFunc("COLOR_New",COLOR_New);
	}




// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
