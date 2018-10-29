/**
Copyright (C) 2010 Chriddo

This program is free software;
you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program;
if not, see <http://www.gnu.org/licenses/>.
**/


#ifndef CUBO_BOXES_H_G
#define CUBO_BOXES_H_G

#include <iostream>
#include "vectors.hpp"
#include <vector>
#include "c3dobjects.hpp"
#include "luautils.hpp"
#include "definitions.hpp"

using namespace std;

//CAREFUL!! DO NOT CHANGE THIS VALUE
#define CUBO_SCALE 20.0
//CAREFUL!! DO NOT CHANGE THIS VALUE
//It is directly written into the levels (e.g. start/cam position)!!!





class TBlockDef : public TBaseLuaDef
{
  public:
   virtual int GetType() {return FILE_BLOCKDEF;}
    void Call_RenderSide(int sideid);
    int Call_SpecialRender(string nam,int index);
     void Call_DistRenderSide(int sideid);
     void Call_OnBlockEvent(int blockid,int actorid);
     void Call_OnSideEvent(int sideid,int actorid);
     int Call_MayMove(int sideid,int actorid,string movetype);
     int Call_MaySlideDown(int sideid,int actorid);
     int Call_MayCull(int sideid);
     int Call_HasTransparency(int blockid);
     int Call_IsPassable(int blockid);
     int Call_IsMoving(int blockid);
     void Call_BlockThink(int blockid);
     void Call_SideVarChanged(string var, int id);
     void Call_SideThink(int sideid);
     void Call_BlockConstructor(int id);
     void Call_SideConstructor(int id);
     void Call_CollisionCheck(int actorid, int itemid);
     string Call_GetEditorInfo(string what,string std);
};


typedef TBaseDefServer<TBlockDef> TBlockDefServer;





class TItemDef : public TBaseLuaDef
{
  public:
     virtual ~TItemDef() {}
     virtual int GetType() {return FILE_ITEMDEF;}
     void Call_Render(int itemindex);
     int Call_SpecialRender(string nam,int index);
     void Call_DistRender(int itemindex);
     void Call_Constructor(int id);
     void Call_CollisionCheck(int actorid,int itemid);
     void Call_Think(int itemid);
     string Call_GetEditorInfo(string what,string std);
};


typedef TBaseDefServer<TItemDef> TItemDefServer;





#define CUBO_UP 0
#define CUBO_DOWN 1
#define CUBO_LEFT 2
#define CUBO_RIGHT 3
#define CUBO_FRONT 4
#define CUBO_BACK 5

static const T3dVector s_CuboNormals[6]={T3dVector(0,1,0),T3dVector(0,-1,0),T3dVector(1,0,0),T3dVector(-1,0,0),T3dVector(0,0,1),T3dVector(0,0,-1)};

static const T3dVector s_CuboVerts[6][4]={
    {T3dVector(CUBO_SCALE,CUBO_SCALE,-CUBO_SCALE),T3dVector(-CUBO_SCALE,CUBO_SCALE,-CUBO_SCALE),T3dVector(CUBO_SCALE,CUBO_SCALE,CUBO_SCALE),T3dVector(-CUBO_SCALE,CUBO_SCALE,CUBO_SCALE)},
    {T3dVector(-CUBO_SCALE,-CUBO_SCALE,-CUBO_SCALE),T3dVector(CUBO_SCALE,-CUBO_SCALE,-CUBO_SCALE),T3dVector(-CUBO_SCALE,-CUBO_SCALE,CUBO_SCALE),T3dVector(CUBO_SCALE,-CUBO_SCALE,CUBO_SCALE)},

    {T3dVector(CUBO_SCALE,CUBO_SCALE,CUBO_SCALE),T3dVector(CUBO_SCALE,-CUBO_SCALE,CUBO_SCALE),T3dVector(CUBO_SCALE,CUBO_SCALE,-CUBO_SCALE),T3dVector(CUBO_SCALE,-CUBO_SCALE,-CUBO_SCALE)},
    {T3dVector(-CUBO_SCALE,CUBO_SCALE,-CUBO_SCALE),T3dVector(-CUBO_SCALE,-CUBO_SCALE,-CUBO_SCALE),T3dVector(-CUBO_SCALE,CUBO_SCALE,CUBO_SCALE),T3dVector(-CUBO_SCALE,-CUBO_SCALE,CUBO_SCALE)},

    {T3dVector(-CUBO_SCALE,CUBO_SCALE,CUBO_SCALE),T3dVector(-CUBO_SCALE,-CUBO_SCALE,CUBO_SCALE),T3dVector(CUBO_SCALE,CUBO_SCALE,CUBO_SCALE),T3dVector(CUBO_SCALE,-CUBO_SCALE,CUBO_SCALE)},
    {T3dVector(CUBO_SCALE,CUBO_SCALE,-CUBO_SCALE),T3dVector(CUBO_SCALE,-CUBO_SCALE,-CUBO_SCALE),T3dVector(-CUBO_SCALE,CUBO_SCALE,-CUBO_SCALE),T3dVector(-CUBO_SCALE,-CUBO_SCALE,-CUBO_SCALE)}

    };


static const T2dVector s_CuboUV[4] = { T2dVector(0,1),T2dVector(0,0),T2dVector(1,1),T2dVector(1,0)};

static const int s_CuboOpposingDir[6]={1,0,3,2,5,4}; //Switches the direction to the opposing side

static const string s_CuboSideNames[6]={"up","down","left","right","front","back"};


class TCuboBlock;
class TCuboItem;

class TCuboBlockSide
{
 protected:
  int side;
  int sidetype;
  TCuboBlock *parent;
   TLuaVarHolder varholder;
 public:
  void SetSideType(int i) {sidetype=i;}
  void Init(int cside,TCuboBlock *parent);
  int  GetID();
  void Render();
  void SpecialRender(string nam,int defrender);
  void RenderQuad();
  T2dVector GetUV(int i);
  T3dVector GetTangent();
  T3dVector GetMidpoint();
  T3dVector GetNormal() {return s_CuboNormals[side];}
  void Call_OnSideEvent(int actorid);
  int Call_MayMove(int actorid,string movestr);
  int Call_MaySlideDown(int actorid);
  int Call_MayCull();
  void Call_Constructor();
  void Call_CollisionCheck(int plr);
  void Call_VarChanged(string var); //To observe changes done by editor
  void DistRender();
  void Think();
  string GetEditorInfo(string what,string def);
  TLuaVarHolder  *GetVarHolder() {return &varholder;}
  string GetTypeName();
  void SetSideItem(string n);
  TCuboItem *GetItem();
  TCuboBlock *GetBlock() {return parent;}
};

class TCuboItem
{
  protected:
    int itemtype;
    int myid;
    TLuaVarHolder varholder;
    TCuboBlockSide *sideptr;
  public:
    TCuboItem(int id, int type,TCuboBlockSide *side);
    T3dVector GetPos();
    void Render();
    void SpecialRender(string nam,int defrender);
    TCuboBlockSide *GetSide() {return sideptr;}
    void DistRender();
    void Call_Constructor();
    TLuaVarHolder  *GetVarHolder() {return &varholder;}
    void CollisionCheckWithActor(int actorid);
    string GetEditorInfo(string what,string def);
    string GetName();
    void Think();
    int GetID() {return myid;}
    void SetID(int id) {myid=id;}
};

class TCuboBlock
{
 protected:
  int id;
  T3dVector pos;
  T3dVector oldpos;
  tfloat scale;
  vector<TCuboBlockSide> sides;
  vector<TCuboBlock*> next;
  TLuaVarHolder varholder;
  float cullradius;
  int blocktype;
  void InitSides();


 public:
  int GetNeighbor(T3dVector norm);
     bool HasNoTransparency();
   void ReleaseMeFromNext();
   void ReAttachMeToNext();
  TCuboBlock();
  tfloat GetScale() {return scale;}
  void SetScale(tfloat s) {scale=s;}
  virtual ~TCuboBlock();
  void Render(TCamera *cam);
  //void SpecialRender(string nam,int defrender);
  void MustRenderSides(TCamera *cam,int  mustrender[]);
  T3dVector GetPos();
  int Moving();
  void SetIPos(int x,int y,int z);
  void SetPos(T3dVector p) {pos=p;}
  void SetNext(int side,TCuboBlock *n);
  TCuboBlock *GetNext(int side);
  void SetBlockType(int i);
  void SetID(int i) {id=i;}
  int GetID() {return id;}
  TCuboBlockSide *GetBlockSide(int s) {return &(sides[s]);}
  void Call_OnBlockEvent(int actorid);
  void Call_Constructor();
  int Blocking(); //Can we pass through it
  string GetEditorInfo(string what,string def);
  int BlockInRay(T3dVector start,T3dVector dir,float *dist,T3dVector *hitpt); //-1 if not hit
  TLuaVarHolder  *GetVarHolder() {return &varholder;}
  void Think();
  T3dVector GetBBMax();
  T3dVector GetBBMin();
  void Call_CollisionCheck(int plr);
  int IsEditorSelector();
  void WriteLevelData(FILE *f);
  float GetCullRadius() {return cullradius;}
  void SetCullRadiusIfHigher(float r) {cullradius=r<cullradius ? cullradius : r;}
  T3dVector GetMovementDelta() {return pos-oldpos;}
  string GetName();
};


extern void g_SetAlphaFunc(TLuaAccess *acc,string func);
extern void g_LastDiffuse(float * ld);

extern void LUA_ITEM_RegisterLib();
extern void LUA_BLOCK_RegisterLib();
extern void LUA_SIDE_RegisterLib();


#endif
