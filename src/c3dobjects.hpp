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

#include "glew.hpp"
#include <SDL2/SDL.h>

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

class MatrixObject {
	protected:
		Matrix3d base;
		Vector3d vPos,vScale,vPitchYawRoll;
		Vector3d vSpeed,vAngleSpeed;
		unsigned char speedMode;
		unsigned char rotSpeedMode;
		unsigned char useRotSpeed;
		unsigned char useSpeed;
		GlutMatrix4d matrix;
		unsigned char changed,usescale,usePYR,pyrOverFlip;
		int numOps;
		std::vector<MatrixObject*> children;
		MatrixObject *parent;
		int mirr;
		void PitchYawRollToBase();

		virtual void setSide(const Vector3d& side);
		virtual void setUp(const Vector3d& up);
		virtual void setDir(const Vector3d& dir);
		virtual void setBasis(const Vector3d side,const Vector3d up,const Vector3d dir);
		virtual void orthonomalize(); // Corrects some float point errors;

		//usePRY has following values 0=nothing to do, 1=only getters were called, 2=setters were called
	public:
		MatrixObject() : base(1), vPos(0,0,0), vScale(1,1,1), vSpeed(0,0,0), vAngleSpeed(0,0,0),speedMode(0), rotSpeedMode(0), matrix(1), changed(1),usescale(0),usePYR(0), pyrOverFlip(0),numOps(0), parent(NULL) {}
//    TMatrixObject(const T3dVector& side,const T3dVector& up,const T3dVector& dir) : base(1),vScale(1,1,1),vSpeed(0,0,0), vAngleSpeed(0,0,0),speedMode(0), rotSpeedMode(0), changed(1),usescale(0) ,usePYR(0),pyrOverFlip(0),numOps(0),parent(NULL), mirr(0) {}
		virtual ~MatrixObject() {for (unsigned int i=0 ; i < children.size(); i++) if (children[i]) delete children[i]; }
		// Setters
		virtual void SetBasis(const Vector3d &s,const Vector3d &u,const Vector3d &d) {setSide(s); setUp(u); setDir(d);}
		virtual void SetBasisAxisRotate(const Vector3d & axis,const float angle);
		virtual void CopyBasis(MatrixObject *other);
		virtual void OnBaseChange() {} //Is called whenever the BaseMatrix is changed
		virtual void OnPYRChange() {} //Called whenever PYR is changed
		virtual void OnBaseWillChange() {} //Called whenever BaseMatrix is about to change... We can use the old matrix for some implementations
		virtual void setMatrix(const GlutMatrix4d& m);
		virtual void setScale(const Vector3d& scal);
		virtual void setPos(const Vector3d& pos);
		virtual void rotateX(const float angle); //THIS ARE THE FIXES COORDS   (1,0,0)
		virtual void rotateY(const float angle); //                            (0,1,0)
		virtual void rotateZ(const float angle); //                            (0,0,1)
		virtual void rotateV(const float angle, const Vector3d axis);
		virtual void rotateDir(const float angle);
		virtual void rotateUp(const float angle);
		virtual void rotateSide(const float angle);
		virtual const Vector3d getSide() {if (usePYR==2) PitchYawRollToBase(); return base.getCol(0);}
		virtual const Vector3d getUp() {if (usePYR==2) PitchYawRollToBase(); return base.getCol(1);}
		virtual Vector3d getDir() {if (usePYR==2) PitchYawRollToBase(); return base.getCol(2);}
		virtual Vector3d getPos() {return vPos;}
		virtual Vector3d getScale() {return vScale;}
		virtual Vector3d getSpeed() {return vSpeed;}
		virtual Vector3d transformToWorld(const Vector3d v);//Transforms it into Global Coords
		virtual Vector3d getWorldPos() {return transformToWorld(getPos());}
		virtual void setSpeed(const Vector3d speed) {vSpeed=speed; useSpeed=!vSpeed.isZero();}
		virtual Vector3d getAngleSpeed() {return vAngleSpeed;}
		virtual void setAngleSpeed(const Vector3d aspeed) {vAngleSpeed=aspeed; useRotSpeed=!vAngleSpeed.isZero();}
		virtual int getSpeedMode() {return speedMode;}
		virtual void setSpeedMode(const int mode) {speedMode=mode;}
		virtual int getAngleSpeedMode() {return rotSpeedMode;}
		virtual void setAngleSpeedMode(const int mode) {rotSpeedMode=mode;}
		virtual void applySpeed(const float elapsed);
		virtual void setBaseVector(int toSet,Vector3d val,int toKeep); //Sets the one Vector to the normalized input and keeps the other. Last is the cross
		virtual void lookAt(int withBaseVector,Vector3d pos,int toKeepFirst); //Sets the one Vector to the normalized input and keeps the other. Last is the cross
		virtual void lookAtZWithWorldUp(Vector3d pos,Vector3d upVect); //Sets the one Vector to the normalized input and keeps the other. Last is the cross
		virtual void CalcMatrix() {changed=0;}
		virtual Vector3d getPitchYawRoll(); //Pitch(-pi/2,pi/2), Yaw(-pi,pi), Roll(-pi,pi);
		virtual Vector3d getPitchYawRollOverflip(); //Pitch(-pi,pi), Yaw(-pi,pi), Roll(-pi/2,pi/2);
		virtual void setPitchYawRoll(Vector3d pyr);
		virtual void think() {CalcMatrix();}
		virtual void PushMult() {matrix.glPushMult();}
		virtual void draw() {}
		virtual void postthink() {matrix.glPop();}
		virtual void goForward(float dist) {Vector3d d=getDir(); vPos=vPos+d*dist; changed=1;}
		virtual void goSide(float dist) {Vector3d d=getSide(); vPos=vPos+d*dist; changed=1;}
		virtual void goUp(float dist) {Vector3d d=getUp(); vPos=vPos+d*dist; changed=1;}
		virtual void TraverseTree(const float timeelapsed);
		virtual void addChild(MatrixObject *other) {children.push_back(other); other->parent=this;}
		virtual int getChildNum() {return children.size();}
		virtual MatrixObject *getChild(const int i) {if (i<0 || i>=getChildNum()) return NULL; else return children[i];}
		virtual Matrix3d *getBase() {return &base;}
		virtual void Reset() {base.Identity(); vPos.xyz(0,0,0); changed=1;}
		virtual void SetMirror(int m);
		virtual int GetMirror();
	};

class Camera : public MatrixObject {
	protected:
		Vector2d nearfar;
		Vector2d screenwh;
		Vector2d fovxy;
		Matrix3d invbase;
		float zoom;
		int AttachedOnPlayer;
		float frustum[6][4];
		unsigned char ortho,fovchanged;
		void ExtractFrustum();
	public:
		MatrixObject * distort;
		Camera() : MatrixObject(), nearfar(2.0,8000.0), screenwh(800.0,600.0), fovxy(1.0,1.0), zoom(1.0), AttachedOnPlayer(-1),ortho(0),fovchanged(1),distort(NULL) {};
		virtual ~Camera() {}
		virtual void CalcMatrix();
		virtual void AttachToPlayer(int i) {AttachedOnPlayer=i;}
		virtual int GetPlayer() {return AttachedOnPlayer;}
		virtual void Invalidate() {fovchanged=1;}
		virtual void think() {MatrixObject::think(); matrix.glPushMult(); ExtractFrustum();}
		Vector2d getNearFar() {return nearfar;}
		Vector2d getScreenWidthHeight() {return screenwh;}
		Vector2d getFOVXY() {return fovxy;}
		int getOrtho() {return ortho;}
		float getZoom() {return zoom;}
		void setNearFar(Vector2d v) {nearfar=v; fovchanged=1;}
		void setScreenWidthHeight(Vector2d v) {screenwh=v; fovchanged=1;}
		void setFOVXY(Vector2d v) {fovxy=v; fovchanged=1;}
		void setZoom(const float z) {zoom=z; fovchanged=1;}
		void setOrtho(const int on) {ortho=on; fovchanged=1;}
		Vector3d PixelPosToDir(Vector2d ppos,int local=0);
		int PointInFrustum( Vector3d pos );
		int SphereInFrustum( Vector3d pos,float rad);
		int VertsVisible(  Vector3d *pt,int numpoints);

		virtual void postthink();


	};

class WorldObject : public MatrixObject {
	protected:

	public:
		virtual void CalcMatrix();
		virtual void think() {MatrixObject::think(); matrix.glPushMult();}
		virtual ~WorldObject() {}
	};


extern void LUA_CAM_RegisterLib();
