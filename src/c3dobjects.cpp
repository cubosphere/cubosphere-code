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

#include "c3dobjects.hpp"
#include <iostream>


#ifdef WIN32

#include <windows.h>

#ifdef MINGW_CROSS_COMPILE
#include <GL/glew.h>
#include <GL/glu.h>
#else
#include <GL\glew.h>
#include <gl\glu.h>
#endif

#include <SDL.h>
#else
#include <GL/glew.h>
#include <SDL/SDL.h>
#endif

#include "luautils.hpp"
#include "game.hpp"

#include <cmath>

void MatrixObject::CopyBasis(MatrixObject *other)
	{
	OnBaseWillChange();
	base.Copy(other->base);
	setPos(other->getPos());
	changed=1;
	OnBaseChange();
	}

void MatrixObject::setBasis(const T3dVector side,const T3dVector up,const T3dVector dir)
	{
	OnBaseWillChange();
	base.setCol(0,side);
	base.setCol(1,up);
	base.setCol(2,dir);
	usePYR=0;
	changed=1;
	numOps++;
	OnBaseChange();
	}

void MatrixObject::setSide(const T3dVector& side)
	{
	OnBaseWillChange();
	if (usePYR==2) PitchYawRollToBase();
	base.setCol(0,side);
	usePYR=0;
	changed=1;
	numOps++;
	OnBaseChange();
	}

void MatrixObject::setUp(const T3dVector& up)
	{
	OnBaseWillChange();
	if (usePYR==2) PitchYawRollToBase();
	base.setCol(1,up);
	usePYR=0;
	changed=1;
	numOps++;
	OnBaseChange();
	}

void MatrixObject::setDir(const T3dVector& dir)
	{
	OnBaseWillChange();
	if (usePYR==2) PitchYawRollToBase();
	base.setCol(2,dir);
	usePYR=0;
	changed=1;
	numOps++;
	OnBaseChange();
	}

void MatrixObject::setPos(const T3dVector& pos)
	{
	vPos=pos;
	changed=1;
	numOps++;
	}

void MatrixObject::orthonomalize()
	{
	numOps=0;
	return;
	//Skipped, since it is not needed
	/*
	T3dVector side=getSide();
	T3dVector up=getUp();
	side.normalize();
	up.normalize();
	T3dVector dir=side.cross(up);
	setBasis(side,up,dir);
	numOps=0; //Normalized => So We can count again
	*/
	}

void MatrixObject::setScale(const T3dVector& scal)
	{
	vScale=scal;
	T3dVector diff(1,1,1);
	diff=(vScale-diff);
	if (diff.sqrlength()<VECTOR_EPSILON) usescale=0; else usescale=1;
	changed=1;
	}

void MatrixObject::rotateX(const tfloat angle)
	{
	OnBaseWillChange();
	if (usePYR==2) PitchYawRollToBase();
	T3dMatrix rx(1.0);
	rx.makeRotX(angle);
	base=rx*base;
	usePYR=0;
	changed=1;
	numOps++;
	OnBaseChange();
	}

void MatrixObject::rotateY(const tfloat angle)
	{
	OnBaseWillChange();
	if (usePYR==2) PitchYawRollToBase();
	T3dMatrix ry(1.0);
	ry.makeRotY(angle);
	base=ry*base;
	usePYR=0;
	changed=1;
	numOps++;
	OnBaseChange();
	}

void MatrixObject::rotateZ(const tfloat angle)
	{
	OnBaseWillChange();
	if (usePYR==2) PitchYawRollToBase();
	T3dMatrix rz(1.0);
	rz.makeRotZ(angle);
	base=rz*base;
	usePYR=0;
	changed=1;
	numOps++;
	OnBaseChange();
	}

void MatrixObject::rotateSide(const tfloat angle)
	{
	OnBaseWillChange();
	if (usePYR==2) PitchYawRollToBase();
	T3dMatrix rx(1.0);
	rx.makeRotX(angle);
	base=base*rx;
	usePYR=0;
	changed=1;
	numOps++;
	OnBaseChange();
	}

void MatrixObject::rotateUp(const tfloat angle)
	{
	OnBaseWillChange();
	if (usePYR==2) PitchYawRollToBase();
	T3dMatrix ry(1.0);
	ry.makeRotY(angle);
	base=base*ry;
	usePYR=0;
	changed=1;
	numOps++;
	OnBaseChange();
	}

void MatrixObject::rotateDir(const tfloat angle)
	{
	OnBaseWillChange();
	if (usePYR==2) PitchYawRollToBase();
	T3dMatrix rz(1.0);
	rz.makeRotZ(angle);
	base=base*rz;
	usePYR=0;
	changed=1;
	numOps++;
	OnBaseChange();
	}

void MatrixObject::rotateV(const tfloat angle, const T3dVector axis)
	{
	OnBaseWillChange();
	if (usePYR==2) PitchYawRollToBase();
	T3dMatrix rv(1.0);
	T3dVector ax=axis;
	ax.normalize();
	rv.makeRotV(angle,ax);
	base=rv*base;
	usePYR=0;
	changed=1;
	numOps++;
	OnBaseChange();
	}

void MatrixObject::setMatrix(const T4dGlutMatrix& m)
	{
	matrix=m; //Think of setting the BASE! Base Change ETC
	changed=0;
	}

void MatrixObject::setBaseVector(int toSet,T3dVector val,int toKeep) //RIGHT SO?
	{
	if (toSet==toKeep) return; //So gehts nicht
	val.normalize();
	T3dVector keep,remaining;
	if (toSet==BASE_DIR_SIDE)
			{
			setSide(val);
			if (toKeep==BASE_DIR_UP) { keep=getUp(); remaining=val.cross(keep); remaining.normalize(); setDir(remaining);}
			else { keep=getDir(); remaining=keep.cross(val); remaining.normalize(); setUp(remaining);}
			}

	else if (toSet==BASE_DIR_UP)
			{
			setUp(val);

			if (toKeep==BASE_DIR_SIDE) { keep=getSide(); remaining=keep.cross(val); remaining.normalize(); setDir(remaining);}
			else { keep=getDir(); remaining=val.cross(keep); remaining.normalize(); setSide(remaining);}
			}

	else if (toSet==BASE_DIR_DIR)
			{
			setDir(val);
			if (toKeep==BASE_DIR_SIDE) { keep=getSide(); remaining=val.cross(keep); remaining.normalize(); setUp(remaining); }
			else { keep=getUp(); remaining=keep.cross(val); remaining.normalize(); setSide(remaining);}

			}
	}

void MatrixObject::lookAt(int withBaseVector,T3dVector pos,int toKeepFirst)
	{
	if (withBaseVector==toKeepFirst) return;
	T3dVector diff=pos-vPos;
	setBaseVector(withBaseVector,diff,toKeepFirst);
//Find the new keep first (=keep last  ^^)
	if (withBaseVector==BASE_DIR_SIDE) {
			if (toKeepFirst==BASE_DIR_UP) toKeepFirst=BASE_DIR_DIR;
			else toKeepFirst=BASE_DIR_UP;
			}
	else if (withBaseVector==BASE_DIR_UP) {
			if (toKeepFirst==BASE_DIR_SIDE) toKeepFirst=BASE_DIR_DIR;
			else toKeepFirst=BASE_DIR_SIDE;
			}
	else {
			if (toKeepFirst==BASE_DIR_SIDE) toKeepFirst=BASE_DIR_UP;
			else toKeepFirst=BASE_DIR_SIDE;
			}
	setBaseVector(withBaseVector,diff,toKeepFirst);
	}


void MatrixObject::lookAtZWithWorldUp(T3dVector pos,T3dVector upVect)
	{
	upVect.normalize();
	T3dVector diff=vPos-pos;
	if (diff.normalizeCheck()==VECTOR_NORMALIZE_ERROR) return;
	float dot=diff*upVect;
	if (dot*dot>1-VECTOR_EPSILON*VECTOR_EPSILON) return;
	setUp(upVect);
	setBaseVector(BASE_DIR_DIR,diff,BASE_DIR_UP);
	setBaseVector(BASE_DIR_DIR,diff,BASE_DIR_SIDE);
	}

T3dVector MatrixObject::getPitchYawRoll()
	{
	if (!usePYR || pyrOverFlip)
			{
			vPitchYawRoll=base.getPitchYawRoll();
			}
	pyrOverFlip=0;
	usePYR=1;
	return vPitchYawRoll;
	}

T3dVector MatrixObject::getPitchYawRollOverflip()
	{
	if (!usePYR || !pyrOverFlip)
			{
			vPitchYawRoll=base.getPitchYawRoll();
			if (abs(vPitchYawRoll.z)>M_PI/2)
					{
					// We have to flip the y-Value by PI
					vPitchYawRoll.y+=M_PI;
					if (vPitchYawRoll.y>M_PI) vPitchYawRoll.y-=2*M_PI;

					vPitchYawRoll.x+=M_PI;
					if (vPitchYawRoll.x>M_PI) vPitchYawRoll.x-=2*M_PI;
					//TODO: FLIP IT RIGHT!
					//if ((vPitchYawRoll.z)>M_PI/2) vPitchYawRoll.z=(vPitchYawRoll.z-M_PI);
					// else vPitchYawRoll.z=(M_PI+vPitchYawRoll.z);
					}
			}
	pyrOverFlip=1;
	usePYR=1;
	return vPitchYawRoll;
	}

void MatrixObject::setPitchYawRoll(T3dVector pyr)
	{
	usePYR=2;
	vPitchYawRoll=pyr;
	changed=1;
	numOps++;
	OnPYRChange();
	}

void MatrixObject::PitchYawRollToBase()
	{
	base.setPitchYawRoll(vPitchYawRoll);
	usePYR=1;
	changed=1;
	}

void MatrixObject::applySpeed(const tfloat elapsed)
	{
	if (!useSpeed && !useRotSpeed) return;
	T3dVector s,as,p,a;
	s=vSpeed*(elapsed);
	as=vAngleSpeed*(elapsed);

	if (speedMode!=SPEED_MODE_LOCAL_AFTER_ROT && useSpeed)
			{
			if (speedMode!=SPEED_MODE_GLOBAL)
					{
					p=getPos();
					p=p+s;
					setPos(p);
					}
			else
					{
					p=getPos();
					s=(base*s);
					p=p+s;
					setPos(p);
					}
			}

//ROTATION
	if (useRotSpeed)
		switch (rotSpeedMode)
				{
				case ANGLE_SPEED_MODE_XYZ: rotateX(as.x);  rotateY(as.y); rotateZ(as.z); break;
				}

	if (speedMode==SPEED_MODE_LOCAL_AFTER_ROT && useSpeed)
			{
			p=getPos();
			s=(base*s);
			p=p+s;
			setPos(p);
			}
	}

void MatrixObject::TraverseTree(const tfloat timeelapsed)
	{
	applySpeed(timeelapsed);
	think();
	draw();
	for (unsigned int i=0; i<children.size(); i++)
			{
			if (children[i]) children[i]->TraverseTree(timeelapsed);
			}
	postthink();
	}


T3dVector MatrixObject::transformToWorld(const T3dVector v)
	{
	if (!parent) return v;
	else
			{
			T3dVector p=v;
			p=parent->base*p;
			p=p+parent->getWorldPos();
			return p;
			}
	}

void MatrixObject::SetMirror(int m)
	{
	mirr=(m==0 ? 0 : 1);
	if (mirr) glFrontFace(GL_CW); else  glFrontFace(GL_CCW);
	}

int MatrixObject::GetMirror()
	{
	return mirr;
	}

void MatrixObject::SetBasisAxisRotate(const T3dVector & axis,const tfloat angle)
	{
//TODO: Implement
	}

//////////CAM///////////////////////

void Camera::postthink()
	{
	MatrixObject::postthink();
//Deactivate Mirror, so it has to be set each frame
	SetMirror(0);
	if (distort) { glPopMatrix(); distort=NULL;}
	}



void Camera::CalcMatrix()
	{
	if (fovchanged || mirr)
			{
			//Set the new fov
			const float ar = (float) screenwh.u / (float) screenwh.v;

			glViewport(0, 0, (GLint)(screenwh.u), (GLint)(screenwh.v));
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			int sign=(mirr==0 ? 1 : -1);
			if (!ortho) glFrustum(-sign*ar*fovxy.u/zoom, sign*ar*fovxy.u/zoom, -1.0*fovxy.v/zoom, 1.0*fovxy.v/zoom, nearfar.u, nearfar.v);
			else {

					glOrtho(-ar*fovxy.u/zoom, ar*fovxy.u/zoom, -1.0*fovxy.v/zoom, 1.0*fovxy.v/zoom, nearfar.u, nearfar.v);
					T3dVector d;
					d.xyz(0,1,0);
					setDir(d);
					d.xyz(1,0,0);
					setSide(d);
					d.xyz(0,0,1);
					setUp(d);
					}

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity() ;
			fovchanged=0;
			}
	if (!changed) return;
	if (usePYR==2) PitchYawRollToBase();
	if (numOps>=RENORMALIZE_STEPS) orthonomalize();
	T3dVector campos;
	T3dVector v;
	tfloat v1,v2,v3;



	invbase=base.transpose(); //FASTER
	v=base.getCol(0);
	v1=-(vPos*v);
	v=base.getCol(1);
	v2=-(vPos*v);
	v=base.getCol(2);
	v3=-(vPos*v);





	campos.xyz(v1,v2,v3);

//campos=vPos;
	matrix.setPos(campos);

	matrix.setSubMatrix(invbase);


	if (distort)
			{

			T4dGlutMatrix disto(1);
			disto.mode=GL_MODELVIEW;
			disto.setSubMatrix(*(distort->getBase()));
			disto.glPushMult();

			}


	if (usescale)
			{
			matrix.scaleBy(vScale);
			}
//The rest should be unchanged
	changed=0;


	}

#define FRUSTUM_EPSILON 0.0001

int Camera::PointInFrustum( T3dVector pos )
	{
	int p;

	for( p = 0; p < 6; p++ )
			{
			tfloat dist=frustum[p][0] * pos.x + frustum[p][1] * pos.y + frustum[p][2] * pos.z + frustum[p][3];

			if( dist <= -FRUSTUM_EPSILON )
				return 0;

			}
	return 1;
	}

int Camera::SphereInFrustum( T3dVector pos,float rad)
	{
	int p;

	for( p = 0; p < 6; p++ )
			{
			tfloat dist=frustum[p][0] * pos.x + frustum[p][1] * pos.y + frustum[p][2] * pos.z + frustum[p][3];

			if( dist <= -FRUSTUM_EPSILON-rad )
				return 0;

			}
	return 1;
	}

int Camera::VertsVisible(  T3dVector *pt,int numpoints)
	{


	int p;

	for( p = 0; p < 5; p++ ) //DO NOT USE THE NEAR CLIP PLANE HERE... IT WONT WORK FOR PORTALS -> ITERATE TO 5, NOT TO 6
			{
			int allout=1;
			for (int i=0; i<numpoints; i++)
					{

					if( frustum[p][0] * pt[i].x + frustum[p][1] * pt[i].y + frustum[p][2] * pt[i].z + frustum[p][3] <= -FRUSTUM_EPSILON )
							{
							//Point outside this plane
							}
					else
							{
							allout=0;

							break; //This plane is intersected at least once
							}
					}
			if (allout) return 0;
			}
	return 1;
	}




T3dVector Camera::PixelPosToDir(T2dVector ppos,int local)
	{
	if (usePYR==2) PitchYawRollToBase();
	T3dVector res=getDir();
	T2dVector rel;
	if (!local) rel.uv((ppos.u-screenwh.u/2)/screenwh.u,(ppos.v-screenwh.v/2)/screenwh.v);
	else rel.uv((ppos.u)/screenwh.u,(ppos.v)/screenwh.v);
	rel.uv(rel.u*fovxy.u,rel.v*fovxy.v);
	T3dVector v;
	v=getSide();
	res=res+v*(rel.u);
	v=getUp();
	res=res-v*(rel.v);
	return(res);
	}


void Camera::ExtractFrustum()
	{
	float   proj[16];
	float   modl[16];
	float   clip[16];
	float   t;

	/* Get the current PROJECTION matrix from OpenGL */
	glGetFloatv( GL_PROJECTION_MATRIX, proj );

	/* Get the current MODELVIEW matrix from OpenGL */
	glGetFloatv( GL_MODELVIEW_MATRIX, modl );

	/* Combine the two matrices (multiply projection by modelview) */
	clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
	clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
	clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
	clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

	clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
	clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
	clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
	clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

	clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
	clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
	clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
	clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

	clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
	clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
	clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
	clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

	/* Extract the numbers for the RIGHT plane */
	frustum[0][0] = clip[ 3] - clip[ 0];
	frustum[0][1] = clip[ 7] - clip[ 4];
	frustum[0][2] = clip[11] - clip[ 8];
	frustum[0][3] = clip[15] - clip[12];

	/* Normalize the result */
	t = sqrt( frustum[0][0] * frustum[0][0] + frustum[0][1] * frustum[0][1] + frustum[0][2] * frustum[0][2] );
	frustum[0][0] /= t;
	frustum[0][1] /= t;
	frustum[0][2] /= t;
	frustum[0][3] /= t;

	/* Extract the numbers for the LEFT plane */
	frustum[1][0] = clip[ 3] + clip[ 0];
	frustum[1][1] = clip[ 7] + clip[ 4];
	frustum[1][2] = clip[11] + clip[ 8];
	frustum[1][3] = clip[15] + clip[12];

	/* Normalize the result */
	t = sqrt( frustum[1][0] * frustum[1][0] + frustum[1][1] * frustum[1][1] + frustum[1][2] * frustum[1][2] );
	frustum[1][0] /= t;
	frustum[1][1] /= t;
	frustum[1][2] /= t;
	frustum[1][3] /= t;

	/* Extract the BOTTOM plane */
	frustum[2][0] = clip[ 3] + clip[ 1];
	frustum[2][1] = clip[ 7] + clip[ 5];
	frustum[2][2] = clip[11] + clip[ 9];
	frustum[2][3] = clip[15] + clip[13];

	/* Normalize the result */
	t = sqrt( frustum[2][0] * frustum[2][0] + frustum[2][1] * frustum[2][1] + frustum[2][2] * frustum[2][2] );
	frustum[2][0] /= t;
	frustum[2][1] /= t;
	frustum[2][2] /= t;
	frustum[2][3] /= t;

	/* Extract the TOP plane */
	frustum[3][0] = clip[ 3] - clip[ 1];
	frustum[3][1] = clip[ 7] - clip[ 5];
	frustum[3][2] = clip[11] - clip[ 9];
	frustum[3][3] = clip[15] - clip[13];

	/* Normalize the result */
	t = sqrt( frustum[3][0] * frustum[3][0] + frustum[3][1] * frustum[3][1] + frustum[3][2] * frustum[3][2] );
	frustum[3][0] /= t;
	frustum[3][1] /= t;
	frustum[3][2] /= t;
	frustum[3][3] /= t;

	/* Extract the FAR plane */
	frustum[4][0] = clip[ 3] - clip[ 2];
	frustum[4][1] = clip[ 7] - clip[ 6];
	frustum[4][2] = clip[11] - clip[10];
	frustum[4][3] = clip[15] - clip[14];

	/* Normalize the result */
	t = sqrt( frustum[4][0] * frustum[4][0] + frustum[4][1] * frustum[4][1] + frustum[4][2] * frustum[4][2] );
	frustum[4][0] /= t;
	frustum[4][1] /= t;
	frustum[4][2] /= t;
	frustum[4][3] /= t;

	/* Extract the NEAR plane */
	frustum[5][0] = clip[ 3] + clip[ 2];
	frustum[5][1] = clip[ 7] + clip[ 6];
	frustum[5][2] = clip[11] + clip[10];
	frustum[5][3] = clip[15] + clip[14];

	/* Normalize the result */
	t = sqrt( frustum[5][0] * frustum[5][0] + frustum[5][1] * frustum[5][1] + frustum[5][2] * frustum[5][2] );
	frustum[5][0] /= t;
	frustum[5][1] /= t;
	frustum[5][2] /= t;
	frustum[5][3] /= t;
	}



/////////// WORLD OBJECT /////////////////

void WorldObject::CalcMatrix()
	{
	if (!changed) return;
	if (usePYR==2) PitchYawRollToBase();
	if (numOps>=RENORMALIZE_STEPS) orthonomalize();

	matrix.setSubMatrix(base);
	matrix.setPos(vPos);

	if (usescale)
			{
			matrix.scaleBy2(vScale);
			}

	changed=0;
	}


////////////////LUA-IMPLEMENT////////////////////



int CAM_GetPos(lua_State *state)
	{
	T3dVector v=g_Game()->GetCam()->getPos();
	LUA_SET_VECTOR3(v);
	return 1;
	}

int CAM_GetDir(lua_State *state)
	{
	T3dVector v=g_Game()->GetCam()->getDir();
	LUA_SET_VECTOR3(v);
	return 1;
	}

int CAM_GetSide(lua_State *state)
	{
	T3dVector v=g_Game()->GetCam()->getSide();
	LUA_SET_VECTOR3(v);
	return 1;
	}

int CAM_GetUp(lua_State *state)
	{
	T3dVector v=g_Game()->GetCam()->getUp();
	LUA_SET_VECTOR3(v);
	return 1;
	}

int CAM_Think(lua_State *state)
	{
	g_Game()->GetCam()->think();
	return 0;
	}

int CAM_SetPos(lua_State *state)
	{
	T3dVector v=Vector3FromStack(state);
	g_Game()->GetCam()->setPos(v);
	return 0;
	}

int CAM_FromBasis(lua_State *state)
	{
	int bi=LUA_GET_INT;

	TCuboBasis *b=g_Game()->GetBasis(bi);
	g_Game()->GetCam()->CopyBasis(b);
	return 0;
	}

int CAM_Attach(lua_State *state)
	{
	int ind=LUA_GET_INT;
	g_Game()->GetCam()->AttachToPlayer(ind);
	return 0;
	}

int CAM_Identity(lua_State *state)
	{
	g_Game()->GetCam()->Reset();
	return 0;
	}

int CAM_SetDistortionBase(lua_State *state)
	{
	int b=LUA_GET_INT;
	if (b<0)
		g_Game()->GetCam()->distort=NULL;
	else g_Game()->GetCam()->distort=g_Game()->GetBasis(b);
	return 0;
	}

int CAM_AxisRotate(lua_State *state)
	{
	float angle=LUA_GET_DOUBLE;
	T3dVector s=Vector3FromStack(state);

	g_Game()->GetCam()->rotateV(angle,s);


	return 0;
	}


int CAM_Distance(lua_State *state)
	{
	T3dVector d=Vector3FromStack(state);
	T3dVector c=g_Game()->GetCam()->getPos();
	d=d-c;
	float dist=d.length();
	LUA_SET_DOUBLE(dist);
	return 1;
	}

int CAM_ZDistance(lua_State *state)
	{
	T3dVector d=Vector3FromStack(state);
	T3dVector c=g_Game()->GetCam()->getPos();
	d=d-c;
	T3dVector camdir=-g_Game()->GetCam()->getDir();
	float dist=d*camdir;
	LUA_SET_DOUBLE(dist);
	return 1;
	}


void LUA_CAM_RegisterLib()
	{
	g_CuboLib()->AddFunc("CAM_GetPos",CAM_GetPos);
	g_CuboLib()->AddFunc("CAM_GetDir",CAM_GetDir);
	g_CuboLib()->AddFunc("CAM_GetUp",CAM_GetUp);
	g_CuboLib()->AddFunc("CAM_GetSide",CAM_GetSide);
	g_CuboLib()->AddFunc("CAM_Distance",CAM_Distance);
	g_CuboLib()->AddFunc("CAM_ZDistance",CAM_ZDistance);
	g_CuboLib()->AddFunc("CAM_SetPos",CAM_SetPos);
	g_CuboLib()->AddFunc("CAM_AxisRotate",CAM_AxisRotate);
	g_CuboLib()->AddFunc("CAM_Attach",CAM_Attach);
	g_CuboLib()->AddFunc("CAM_FromBasis",CAM_FromBasis);
	g_CuboLib()->AddFunc("CAM_Identity",CAM_Identity);
	g_CuboLib()->AddFunc("CAM_Think",CAM_Think);
	g_CuboLib()->AddFunc("CAM_SetDistortionBase",CAM_SetDistortionBase);
	}
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
