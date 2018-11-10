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

#ifdef WIN32

#ifdef MINGW_CROSS_COMPILE
#include <GL/glew.h>
#else
#include <GL\glew.h>
#endif
// #include <GL\gl.h>
#include <windows.h>
#include <SDL.h>


#else
#include <GL/glew.h>
#include <SDL/SDL.h>

//#include <GL/glext.h>

#endif


#include <iostream>
#include <vector>

#include "filesystem.hpp"
#include "definitions.hpp"
#include "vectors.hpp"

class TMdlDef : public BaseLuaDef
	{
	protected:
		GLuint totalrenderlist;
		std::vector<GLuint> grenderlists;
	public:
		TMdlDef() : BaseLuaDef(), totalrenderlist(0) {};
		virtual ~TMdlDef();
		virtual int GetType() {return FILE_MDLDEF;}
		void CreateRenderList(int gindex);
		void Call_RenderModel();
		void Call_RenderGroup(int g);
	};

typedef BaseDefServer<TMdlDef> TMdlDefServer;




typedef struct
	{
	int x,y,z;
	} TIntVect;

class TTextFileReader
	{
	protected:
		std::vector<std::string> lines;
		std::vector<int> linenums;
		int linenr;
	public:
		void RemoveComments(std::string commentindicator="#",bool trim=true,bool cutempty=true);
		bool LoadFile(TCuboFile * finfo);
		void OutToTTY();
		int GetLineNr();
		std::string NextLine();
		bool isEoF();
		bool ExtractIntVect(std::string s,TIntVect &intv,std::string *ress=NULL,std::string sep=" ");
		bool ExtractIntVectFace(std::string s,TIntVect &intv,std::string *ress=NULL,std::string sep=" ");

		bool ExtractIntVect0(std::string s,TIntVect &intv,std::string *ress=NULL,std::string sep=" ");
		bool Extract3dVector(std::string s,T3dVector &intv,int offs);
		bool Extract2dVector(std::string s,T2dVector &intv,int offs);
		void ReplaceChar(std::string &str,char which,char with);
		std::vector<std::string> Seperate(std::string s,std::string sep=" ");
	};



class TBaseModel
	{
	protected:
		std::string name;
		virtual void Clear() {}
	public:
		virtual ~TBaseModel() {}
		virtual void DrawGroupWithTangent(int g, std::string TangentNameInShader) {}
		virtual void DrawGroup(int g) {}
		virtual std::string GetName() {return name;}
		virtual bool LoadFromFile(TCuboFile *finfo) {name=finfo->GetName(); return true;}
		virtual void RenderByDef(int defindex) {}
		virtual void Reload();
	};

typedef struct
	{
	int vert[3],norm[3],texcoord[3];
	int tangent;
	} TOBJFace;

//typedef TOBJVertex TOBJFace[3];

typedef struct
	{
	std::string name,materialname;
	std::vector<TOBJFace> faces;
	GLuint renderlist;
	int userenderlist;
	} TOBJGroup;

class TOBJModel : public TBaseModel
	{
	protected:
		std::vector<T3dVector> verts,normals,tangents;
		std::vector<T2dVector> texcoords;
		std::vector<TOBJGroup> groups;
		// GLuint completerenderlist;
		virtual void Clear();
		virtual void SmoothSimplify();
		virtual T3dVector GetTangent(int g,int f);
	public:
		virtual void DrawGroup(int g);
		virtual void DrawGroupWithTangent(int g, std::string TangentNameInShader);
		virtual bool LoadFromFile(TCuboFile *finfo);
		// virtual void RenderAll();
		virtual void RenderByDef(int defindex);
	};


class TModelServer
	{
	protected:
		std::vector<TBaseModel*> mdls;
		virtual int GetModel(std::string fname);
	public:
		virtual void Clear();
		TModelServer();
		void Reload();
		virtual ~TModelServer();
		virtual int AddOBJ(TCuboFile *finfo);
		virtual TBaseModel *GetModelPtr(int i) {return mdls[i];}
	};

extern void LUA_MDLDEF_RegisterLib();
extern void LUA_MODEL_RegisterLib();

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
