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

#include <vector>
#include "vectors.hpp"
#include "matrixutils.hpp"

#ifdef WIN32

#include <windows.h>

#ifdef MINGW_CROSS_COMPILE
#include <GL/glew.h>
#else
#include <GL\glew.h>
#endif
#include <SDL.h>


#else


#include <GL/glew.h>
#include <SDL/SDL.h>

#endif

#define BASE_DIR_SIDE 0
#define BASE_DIR_UP 1
#define BASE_DIR_DIR 2

#define RENORMALIZE_STEPS 1000

#define ANGLE_SPEED_MODE_XYZ 0
#define ANGLE_SPEED_MODE_XZY 1
#define ANGLE_SPEED_MODE_YXZ 2
#define ANGLE_SPEED_MODE_YZX 3
#define ANGLE_SPEED_MODE_ZXY 4
#define ANGLE_SPEED_MODE_ZYX 5

#define ANGLE_SPEED_MODE_PITCH_YAW_ROLL 6

#define ANGLE_SPEED_MODE_SIDE_UP_DIR 7
#define ANGLE_SPEED_MODE_SIDE_DIR_UP 8
#define ANGLE_SPEED_MODE_UP_SIDE_DIR 9
#define ANGLE_SPEED_MODE_UP_DIR_SIDE 10
#define ANGLE_SPEED_MODE_DIR_SIDE_UP 11
#define ANGLE_SPEED_MODE_DIR_UP_SIDE 12


#define SPEED_MODE_GLOBAL 0
#define SPEED_MODE_LOCAL 1
#define SPEED_MODE_LOCAL_AFTER_ROT 2

class TMatrixObject
	{
	protected:
		T3dMatrix base;
		T3dVector vPos,vScale,vPitchYawRoll;
		T3dVector vSpeed,vAngleSpeed;
		unsigned char speedMode;
		unsigned char rotSpeedMode;
		unsigned char useRotSpeed;
		unsigned char useSpeed;
		T4dGlutMatrix matrix;
		unsigned char changed,usescale,usePYR,pyrOverFlip;
		int numOps;
		std::vector<TMatrixObject*> children;
		TMatrixObject *parent;
		int mirr;
		void PitchYawRollToBase();

		virtual void setSide(const T3dVector& side);
		virtual void setUp(const T3dVector& up);
		virtual void setDir(const T3dVector& dir);
		virtual void setBasis(const T3dVector side,const T3dVector up,const T3dVector dir);
		virtual void orthonomalize(); // Corrects some float point errors;

		//usePRY has following values 0=nothing to do, 1=only getters were called, 2=setters were called
	public:
		TMatrixObject() : base(1), vPos(0,0,0), vScale(1,1,1), vSpeed(0,0,0), vAngleSpeed(0,0,0),speedMode(0), rotSpeedMode(0), matrix(1), changed(1),usescale(0),usePYR(0), pyrOverFlip(0),numOps(0), parent(NULL) {}
//    TMatrixObject(const T3dVector& side,const T3dVector& up,const T3dVector& dir) : base(1),vScale(1,1,1),vSpeed(0,0,0), vAngleSpeed(0,0,0),speedMode(0), rotSpeedMode(0), changed(1),usescale(0) ,usePYR(0),pyrOverFlip(0),numOps(0),parent(NULL), mirr(0) {}
		virtual ~TMatrixObject() {for (unsigned int i=0 ; i < children.size(); i++) if (children[i]) delete children[i]; }
		// Setters
		virtual void SetBasis(const T3dVector &s,const T3dVector &u,const T3dVector &d) {setSide(s); setUp(u); setDir(d);}
		virtual void SetBasisAxisRotate(const T3dVector & axis,const tfloat angle);
		virtual void CopyBasis(TMatrixObject *other);
		virtual void OnBaseChange() {} //Is called whenever the BaseMatrix is changed
		virtual void OnPYRChange() {} //Called whenever PYR is changed
		virtual void OnBaseWillChange() {} //Called whenever BaseMatrix is about to change... We can use the old matrix for some implementations
		virtual void setMatrix(const T4dGlutMatrix& m);
		virtual void setScale(const T3dVector& scal);
		virtual void setPos(const T3dVector& pos);
		virtual void rotateX(const tfloat angle); //THIS ARE THE FIXES COORDS   (1,0,0)
		virtual void rotateY(const tfloat angle); //                            (0,1,0)
		virtual void rotateZ(const tfloat angle); //                            (0,0,1)
		virtual void rotateV(const tfloat angle, const T3dVector axis);
		virtual void rotateDir(const tfloat angle);
		virtual void rotateUp(const tfloat angle);
		virtual void rotateSide(const tfloat angle);
		virtual const T3dVector getSide() {if (usePYR==2) PitchYawRollToBase(); return base.getCol(0);}
		virtual const T3dVector getUp() {if (usePYR==2) PitchYawRollToBase(); return base.getCol(1);}
		virtual T3dVector getDir() {if (usePYR==2) PitchYawRollToBase(); return base.getCol(2);}
		virtual T3dVector getPos() {return vPos;}
		virtual T3dVector getScale() {return vScale;}
		virtual T3dVector getSpeed() {return vSpeed;}
		virtual T3dVector transformToWorld(const T3dVector v);//Transforms it into Global Coords
		virtual T3dVector getWorldPos() {return transformToWorld(getPos());}
		virtual void setSpeed(const T3dVector speed) {vSpeed=speed; useSpeed=!vSpeed.isZero();}
		virtual T3dVector getAngleSpeed() {return vAngleSpeed;}
		virtual void setAngleSpeed(const T3dVector aspeed) {vAngleSpeed=aspeed; useRotSpeed=!vAngleSpeed.isZero();}
		virtual int getSpeedMode() {return speedMode;}
		virtual void setSpeedMode(const int mode) {speedMode=mode;}
		virtual int getAngleSpeedMode() {return rotSpeedMode;}
		virtual void setAngleSpeedMode(const int mode) {rotSpeedMode=mode;}
		virtual void applySpeed(const tfloat elapsed);
		virtual void setBaseVector(int toSet,T3dVector val,int toKeep); //Sets the one Vector to the normalized input and keeps the other. Last is the cross
		virtual void lookAt(int withBaseVector,T3dVector pos,int toKeepFirst); //Sets the one Vector to the normalized input and keeps the other. Last is the cross
		virtual void lookAtZWithWorldUp(T3dVector pos,T3dVector upVect); //Sets the one Vector to the normalized input and keeps the other. Last is the cross
		virtual void CalcMatrix() {changed=0;}
		virtual T3dVector getPitchYawRoll(); //Pitch(-pi/2,pi/2), Yaw(-pi,pi), Roll(-pi,pi);
		virtual T3dVector getPitchYawRollOverflip(); //Pitch(-pi,pi), Yaw(-pi,pi), Roll(-pi/2,pi/2);
		virtual void setPitchYawRoll(T3dVector pyr);
		virtual void think() {CalcMatrix();}
		virtual void PushMult() {matrix.glPushMult();}
		virtual void draw() {}
		virtual void postthink() {matrix.glPop();}
		virtual void goForward(tfloat dist) {T3dVector d=getDir(); vPos=vPos+d*dist; changed=1;}
		virtual void goSide(tfloat dist) {T3dVector d=getSide(); vPos=vPos+d*dist; changed=1;}
		virtual void goUp(tfloat dist) {T3dVector d=getUp(); vPos=vPos+d*dist; changed=1;}
		virtual void TraverseTree(const tfloat timeelapsed);
		virtual void addChild(TMatrixObject *other) {children.push_back(other); other->parent=this;}
		virtual int getChildNum() {return children.size();}
		virtual TMatrixObject *getChild(const int i) {if (i<0 || i>=getChildNum()) return NULL; else return children[i];}
		virtual T3dMatrix *getBase() {return &base;}
		virtual void Reset() {base.Identity(); vPos.xyz(0,0,0); changed=1;}
		virtual void SetMirror(int m);
		virtual int GetMirror();
	};

class TCamera : public TMatrixObject
	{
	protected:
		T2dVector nearfar;
		T2dVector screenwh;
		T2dVector fovxy;
		T3dMatrix invbase;
		tfloat zoom;
		int AttachedOnPlayer;
		tfloat frustum[6][4];
		unsigned char ortho,fovchanged;
		void ExtractFrustum();
	public:
		TMatrixObject * distort;
		TCamera() : TMatrixObject(), nearfar(2.0,8000.0), screenwh(800.0,600.0), fovxy(1.0,1.0), zoom(1.0), AttachedOnPlayer(-1),ortho(0),fovchanged(1),distort(NULL) {};
		virtual ~TCamera() {}
		virtual void CalcMatrix();
		virtual void AttachToPlayer(int i) {AttachedOnPlayer=i;}
		virtual int GetPlayer() {return AttachedOnPlayer;}
		virtual void Invalidate() {fovchanged=1;}
		virtual void think() {TMatrixObject::think(); matrix.glPushMult(); ExtractFrustum();}
		T2dVector getNearFar() {return nearfar;}
		T2dVector getScreenWidthHeight() {return screenwh;}
		T2dVector getFOVXY() {return fovxy;}
		int getOrtho() {return ortho;}
		tfloat getZoom() {return zoom;}
		void setNearFar(T2dVector v) {nearfar=v; fovchanged=1;}
		void setScreenWidthHeight(T2dVector v) {screenwh=v; fovchanged=1;}
		void setFOVXY(T2dVector v) {fovxy=v; fovchanged=1;}
		void setZoom(const tfloat z) {zoom=z; fovchanged=1;}
		void setOrtho(const int on) {ortho=on; fovchanged=1;}
		T3dVector PixelPosToDir(T2dVector ppos,int local=0);
		int PointInFrustum( T3dVector pos );
		int SphereInFrustum( T3dVector pos,float rad);
		int VertsVisible(  T3dVector *pt,int numpoints);

		virtual void postthink();


	};

class TWorldObject : public TMatrixObject
	{
	protected:

	public:
		virtual void CalcMatrix();
		virtual void think() {TMatrixObject::think(); matrix.glPushMult();}
		virtual ~TWorldObject() {}
	};


extern void LUA_CAM_RegisterLib();

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
