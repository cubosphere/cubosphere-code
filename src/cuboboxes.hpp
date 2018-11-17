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

#include <iostream>
#include "vectors.hpp"
#include <vector>
#include "c3dobjects.hpp"
#include "luautils.hpp"
#include "definitions.hpp"

//CAREFUL!! DO NOT CHANGE THIS VALUE
#define CUBO_SCALE 20.0
//CAREFUL!! DO NOT CHANGE THIS VALUE
//It is directly written into the levels (e.g. start/cam position)!!!





class BlockDef : public BaseLuaDef {
	public:
		virtual int GetType() {return FILE_BLOCKDEF;}
		void Call_RenderSide(int sideid);
		int Call_SpecialRender(std::string nam,int index);
		void Call_DistRenderSide(int sideid);
		void Call_OnBlockEvent(int blockid,int actorid);
		void Call_OnSideEvent(int sideid,int actorid);
		int Call_MayMove(int sideid,int actorid,std::string movetype);
		int Call_MaySlideDown(int sideid,int actorid);
		int Call_MayCull(int sideid);
		int Call_HasTransparency(int blockid);
		int Call_IsPassable(int blockid);
		int Call_IsMoving(int blockid);
		void Call_BlockThink(int blockid);
		void Call_SideVarChanged(std::string var, int id);
		void Call_SideThink(int sideid);
		void Call_BlockConstructor(int id);
		void Call_SideConstructor(int id);
		void Call_CollisionCheck(int actorid, int itemid);
		std::string Call_GetEditorInfo(std::string what,std::string std);
	};


using BlockDefServer = BaseDefServer<BlockDef>;

class ItemDef : public BaseLuaDef {
	public:
		virtual ~ItemDef() {}
		virtual int GetType() {return FILE_ITEMDEF;}
		void Call_Render(int itemindex);
		int Call_SpecialRender(std::string nam,int index);
		void Call_DistRender(int itemindex);
		void Call_Constructor(int id);
		void Call_CollisionCheck(int actorid,int itemid);
		void Call_Think(int itemid);
		std::string Call_GetEditorInfo(std::string what,std::string std);
	};


using TItemDefServer = BaseDefServer<ItemDef>;

#define CUBO_UP 0
#define CUBO_DOWN 1
#define CUBO_LEFT 2
#define CUBO_RIGHT 3
#define CUBO_FRONT 4
#define CUBO_BACK 5

static const Vector3d s_CuboNormals[6]= {Vector3d(0,1,0),Vector3d(0,-1,0),Vector3d(1,0,0),Vector3d(-1,0,0),Vector3d(0,0,1),Vector3d(0,0,-1)};

static const Vector3d s_CuboVerts[6][4]= {
		{Vector3d(CUBO_SCALE,CUBO_SCALE,-CUBO_SCALE),Vector3d(-CUBO_SCALE,CUBO_SCALE,-CUBO_SCALE),Vector3d(CUBO_SCALE,CUBO_SCALE,CUBO_SCALE),Vector3d(-CUBO_SCALE,CUBO_SCALE,CUBO_SCALE)},
		{Vector3d(-CUBO_SCALE,-CUBO_SCALE,-CUBO_SCALE),Vector3d(CUBO_SCALE,-CUBO_SCALE,-CUBO_SCALE),Vector3d(-CUBO_SCALE,-CUBO_SCALE,CUBO_SCALE),Vector3d(CUBO_SCALE,-CUBO_SCALE,CUBO_SCALE)},

		{Vector3d(CUBO_SCALE,CUBO_SCALE,CUBO_SCALE),Vector3d(CUBO_SCALE,-CUBO_SCALE,CUBO_SCALE),Vector3d(CUBO_SCALE,CUBO_SCALE,-CUBO_SCALE),Vector3d(CUBO_SCALE,-CUBO_SCALE,-CUBO_SCALE)},
		{Vector3d(-CUBO_SCALE,CUBO_SCALE,-CUBO_SCALE),Vector3d(-CUBO_SCALE,-CUBO_SCALE,-CUBO_SCALE),Vector3d(-CUBO_SCALE,CUBO_SCALE,CUBO_SCALE),Vector3d(-CUBO_SCALE,-CUBO_SCALE,CUBO_SCALE)},

		{Vector3d(-CUBO_SCALE,CUBO_SCALE,CUBO_SCALE),Vector3d(-CUBO_SCALE,-CUBO_SCALE,CUBO_SCALE),Vector3d(CUBO_SCALE,CUBO_SCALE,CUBO_SCALE),Vector3d(CUBO_SCALE,-CUBO_SCALE,CUBO_SCALE)},
		{Vector3d(CUBO_SCALE,CUBO_SCALE,-CUBO_SCALE),Vector3d(CUBO_SCALE,-CUBO_SCALE,-CUBO_SCALE),Vector3d(-CUBO_SCALE,CUBO_SCALE,-CUBO_SCALE),Vector3d(-CUBO_SCALE,-CUBO_SCALE,-CUBO_SCALE)}

	};


static const Vector2d s_CuboUV[4] = { Vector2d(0,1),Vector2d(0,0),Vector2d(1,1),Vector2d(1,0)};

static const int s_CuboOpposingDir[6]= {1,0,3,2,5,4}; //Switches the direction to the opposing side

static const std::string s_CuboSideNames[6]= {"up","down","left","right","front","back"};


class CuboBlock;
class CuboItem;

class CuboBlockSide {
	protected:
		int side;
		int sidetype;
		CuboBlock *parent;
		LuaVarHolder varholder;
	public:
		void SetSideType(int i) {sidetype=i;}
		void Init(int cside,CuboBlock *parent);
		int  GetID();
		void Render();
		void SpecialRender(std::string nam,int defrender);
		void RenderQuad();
		Vector2d GetUV(int i);
		Vector3d GetTangent();
		Vector3d GetMidpoint();
		Vector3d GetNormal() {return s_CuboNormals[side];}
		void Call_OnSideEvent(int actorid);
		int Call_MayMove(int actorid,std::string movestr);
		int Call_MaySlideDown(int actorid);
		int Call_MayCull();
		void Call_Constructor();
		void Call_CollisionCheck(int plr);
		void Call_VarChanged(std::string var); //To observe changes done by editor
		void DistRender();
		void Think();
		std::string GetEditorInfo(std::string what,std::string def);
		LuaVarHolder  *GetVarHolder() {return &varholder;}
		std::string GetTypeName();
		void SetSideItem(std::string n);
		CuboItem *GetItem();
		CuboBlock *GetBlock() {return parent;}
	};

class CuboItem {
	protected:
		int itemtype;
		int myid;
		LuaVarHolder varholder;
		CuboBlockSide *sideptr;
	public:
		CuboItem(int id, int type,CuboBlockSide *side);
		Vector3d GetPos();
		void Render();
		void SpecialRender(std::string nam,int defrender);
		CuboBlockSide *GetSide() {return sideptr;}
		void DistRender();
		void Call_Constructor();
		LuaVarHolder  *GetVarHolder() {return &varholder;}
		void CollisionCheckWithActor(int actorid);
		std::string GetEditorInfo(std::string what,std::string def);
		std::string GetName();
		void Think();
		int GetID() {return myid;}
		void SetID(int id) {myid=id;}
	};

class CuboBlock {
	protected:
		int id;
		Vector3d pos;
		Vector3d oldpos;
		float scale;
		std::vector<CuboBlockSide> sides;
		std::vector<CuboBlock*> next;
		LuaVarHolder varholder;
		float cullradius;
		int blocktype;
		void InitSides();


	public:
		int GetNeighbor(Vector3d norm);
		bool HasNoTransparency();
		void ReleaseMeFromNext();
		void ReAttachMeToNext();
		CuboBlock();
		float GetScale() {return scale;}
		void SetScale(float s) {scale=s;}
		virtual ~CuboBlock();
		void Render(Camera *cam);
		//void SpecialRender(string nam,int defrender);
		void MustRenderSides(Camera *cam,int  mustrender[]);
		Vector3d GetPos();
		int Moving();
		void SetIPos(int x,int y,int z);
		void SetPos(Vector3d p) {pos=p;}
		void SetNext(int side,CuboBlock *n);
		CuboBlock *GetNext(int side);
		void SetBlockType(int i);
		void SetID(int i) {id=i;}
		int GetID() {return id;}
		CuboBlockSide *GetBlockSide(int s) {return &(sides[s]);}
		void Call_OnBlockEvent(int actorid);
		void Call_Constructor();
		int Blocking(); //Can we pass through it
		std::string GetEditorInfo(std::string what,std::string def);
		int BlockInRay(Vector3d start,Vector3d dir,float *dist,Vector3d *hitpt); //-1 if not hit
		LuaVarHolder  *GetVarHolder() {return &varholder;}
		void Think();
		Vector3d GetBBMax();
		Vector3d GetBBMin();
		void Call_CollisionCheck(int plr);
		int IsEditorSelector();
		void WriteLevelData(FILE *f);
		float GetCullRadius() {return cullradius;}
		void SetCullRadiusIfHigher(float r) {cullradius=r<cullradius ? cullradius : r;}
		Vector3d GetMovementDelta() {return pos-oldpos;}
		std::string GetName();
	};


extern void g_SetAlphaFunc(LuaAccess *acc,std::string func);
extern void g_LastDiffuse(float * ld);

extern void LUA_ITEM_RegisterLib();
extern void LUA_BLOCK_RegisterLib();
extern void LUA_SIDE_RegisterLib();

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
