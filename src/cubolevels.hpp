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

#include "cuboboxes.hpp"
#include <vector>
#include <iostream>
#include <string>
#include "cuboutils.hpp"
#include "c3dobjects.hpp"
#include "definitions.hpp"
#include "textures.hpp"

#define DIST_RENDER_SIDE 0
#define DIST_RENDER_ITEM 1
#define DIST_RENDER_ACTOR 2
#define DIST_RENDER_BLOCK 3
#define DIST_RENDER_CUSTOM 4


using TraceResult = struct {
	int hit;
	float dist;
	int block;
	int side;
	};

class DistRenderObj {
	protected:
		float dist;
		int id; //The id to the object
		int type; //Blockside, Item etc.
		std::string myhint;
		lua_State *callstate;
		double CullRadius;
		Vector3d CullCenter;

	public:
		DistRenderObj(int tid,int ttype,float tdist,lua_State *ccallstate) : dist(tdist), id(tid), type(ttype), callstate(ccallstate), CullRadius(-1) {};
		const float GetDist() const {return dist;}
		const int GetID() const {return id;}
		const int GetType() const {return type;}
		const std::string GetHint() const {return myhint;}
		void SetHint(std::string h) {myhint=h;}
		void SetCulling(Vector3d cc,double cr) {CullCenter=cc; CullRadius=cr;}
		bool operator < (DistRenderObj b ) const {return dist > b.dist;}
		void DistRender(Camera *cam);
		void Render(Camera *cam);
		void SpecialRender(Camera *cam,std::string nam,int defrender);
	};

class CuboLevel {
	protected:
		std::vector<CuboBlock*> blocks;
		std::vector<CuboItem*> items;
		std::vector<DistRenderObj> distrenderlist; //Used for Blending stuff
		std::vector<DistRenderObj> rlist;
		Vector3d bbmax,bbmin; //Sizes of the bounding box
//    TTextFileReader tr;
		LuaAccess lua;
		BlockDefServer bdefs;
		TextureDefServer tdefs;
		TItemDefServer idefs;
		std::string errorstring;
		std::string filename;
		float timescale;
		float leveltime;
		std::string lastrenderedtype;
		std::string nextlevel;
		int loadnextlevel;
		int nextleveluserlevel;

		int errorline;
		//  bool ReadBlock();
		void BindBlocksToNext();
		int docollisions;
	public:
		BlockDefServer * GetBlockDefs() {return  &bdefs;}
		TextureDefServer *GetTexDefs() {return  &tdefs;}
		TItemDefServer *GetItemDefs() {return  &idefs;}
		std::string GetLastRendered() const {return lastrenderedtype;}
		void SetLastRendered(const std::string &s) {lastrenderedtype=s;}
		CuboBlock* LastBlock();
		void clear();
		void clearAll(); ///TODO: add a function to clear all the data between episodes
		void CreateBBox();
		void Reload();
		//  bool LoadFromFile(string fname);
		void SetNewLevel(std::string nfname,int ulevel) {nextlevel=nfname; loadnextlevel=1; if (ulevel>=0) nextleveluserlevel=ulevel;}
		bool LoadFromLua(std::string fname);
		void Think();
		void CheckCollides();
		void PostThink();
		void Render(Camera *cam);
		void SpecialRender(Camera *cam,std::string nam,int defrender);
		void DistRender(Camera *cam);
		void LastDistanceRenderCull(Vector3d center,double rad);
		void SpecialDistRender(std::string nam,int defrender);
		void FrameRenderStart();
		void FrameRenderEnd();
		void DrawHUD();
		Vector3d GetCenter();
		float GetRadius();
		int LoadTexDef(std::string name);
		BlockDef *GetBlockDef(int i) {return bdefs.GetDefPtr(i);}
		TextureDef *GetTexDef(int i) {return tdefs.GetDefPtr(i);}
		ItemDef *GetItemDef(int i) {return idefs.GetDefPtr(i);}
		CuboItem *GetItem(int i) {return items[i];}
		CuboBlock *GetBlockAtIPos(int x,int y,int z);
		CuboBlock *GetBlockAtPos(Vector3d p);
		CuboBlock *GetBlock(int i);
		CuboBlock *GetBlockFromType(std::string name,int i);
		TraceResult TraceLine(Vector3d start,Vector3d dir, int onlyblocking=0);
		CuboBlockSide *GetBlockSide(int id);
		std::string GetErrorString();
		void AddDistRenderItem(int id,int type,float dist,lua_State *callstate);
		void AddBlock(int x,int y,int z,std::string bdefname);
		void DeleteBlock(int i);
		void ChangeSide(int block, std::string sidestr,std::string sdefname);
		int AddItem(int block, std::string sidestr,std::string idefname);
		void LoadSky(std::string skyname);
		int NumItemsOfType(std::string itemname);
		int PointInBBox(Vector3d cp,float extrasize=40*CUBO_SCALE);
		std::string GetLevelFileName() {return filename;}
		void SendKey(int key,int down,int toggle);
		void SendJoyButton(int joy,int button,int dir,int down,int toggle);
		void JoyAxisChange(int joys,int axis,double val,double pval);
		double Elapsed();
		double Time() {return leveltime;}
		void SetTimeScale(double ts) {timescale=ts;}
		double GetTimeScale() {return timescale;}
		void ChangeBlock(int block, std::string sdefname);
		void WriteLevelData(FILE *f);
		void RemoveItemFromSide(CuboBlockSide *s);
		int GetNumBlocks() {return blocks.size();}
		CuboItem *GetItemOnSide(CuboBlockSide *s);
		int GetSideOfType(std::string tname,int startside,int offs);
		void SortDistRenderList();
		void SetCollisionChecksActive(int i) {docollisions=i;}
		int GetCollisionChecksActive() {return docollisions;}
		std::string CheckDefExchange(std::string defname,std::string deftype);
	};

extern void LUA_LEVEL_RegisterLib();

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
