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

#include <GL/glew.h>
#include <SDL.h>

#include <iostream>
#include <vector>

#include "filesystem.hpp"
#include "definitions.hpp"
#include "vectors.hpp"

class MdlDef : public BaseLuaDef {
	protected:
		GLuint totalrenderlist;
		std::vector<GLuint> grenderlists;
	public:
		MdlDef() : BaseLuaDef(), totalrenderlist(0) {};
		virtual ~MdlDef();
		virtual int GetType() {return FILE_MDLDEF;}
		void CreateRenderList(int gindex);
		void Call_RenderModel();
		void Call_RenderGroup(int g);
	};

using MdlDefServer = BaseDefServer<MdlDef>;

struct IntVect {
	int x,y,z;
	};

class TextFileReader {
	protected:
		std::vector<std::string> lines;
		std::vector<int> linenums;
		int linenr;
	public:
		void RemoveComments(std::string commentindicator="#",bool trim=true,bool cutempty=true);
		bool LoadFile(const std::unique_ptr<CuboFile>& finfo);
		void OutToTTY();
		int GetLineNr();
		std::string NextLine();
		bool isEoF();
		bool ExtractIntVect(std::string s,IntVect &intv,std::string *ress=NULL,std::string sep=" ");
		bool ExtractIntVectFace(std::string s,IntVect &intv,std::string *ress=NULL,std::string sep=" ");

		bool ExtractIntVect0(std::string s,IntVect &intv,std::string *ress=NULL,std::string sep=" ");
		bool Extract3dVector(std::string s,Vector3d &intv,int offs);
		bool Extract2dVector(std::string s,Vector2d &intv,int offs);
		void ReplaceChar(std::string &str,char which,char with);
		std::vector<std::string> Seperate(std::string s,std::string sep=" ");
	};



class BaseModel {
	protected:
		std::string name;
		virtual void Clear() {}
	public:
		virtual ~BaseModel() {}
		virtual void DrawGroupWithTangent(int g, std::string TangentNameInShader) {}
		virtual void DrawGroup(int g) {}
		virtual std::string GetName() {return name;}
		virtual bool LoadFromFile(const std::unique_ptr<CuboFile>& finfo) {name=finfo->GetName(); return true;}
		virtual void RenderByDef(int defindex) {}
		virtual void Reload();
	};

struct OBJFace {
	int vert[3],norm[3],texcoord[3];
	int tangent;
	};

struct OBJGroup {
	std::string name,materialname;
	std::vector<OBJFace> faces;
	GLuint renderlist;
	int userenderlist;
	};

class OBJModel : public BaseModel {
	protected:
		std::vector<Vector3d> verts,normals,tangents;
		std::vector<Vector2d> texcoords;
		std::vector<OBJGroup> groups;
		// GLuint completerenderlist;
		virtual void Clear();
		virtual void SmoothSimplify();
		virtual Vector3d GetTangent(int g,int f);
	public:
		virtual void DrawGroup(int g);
		virtual void DrawGroupWithTangent(int g, std::string TangentNameInShader);
		virtual bool LoadFromFile(const std::unique_ptr<CuboFile>& finfo);
		// virtual void RenderAll();
		virtual void RenderByDef(int defindex);
	};


class ModelServer {
	protected:
		std::vector<BaseModel*> mdls;
		virtual int GetModel(std::string fname);
	public:
		virtual void Clear();
		ModelServer();
		void Reload();
		virtual ~ModelServer();
		virtual int AddOBJ(const std::unique_ptr<CuboFile>& finfo);
		virtual BaseModel *GetModelPtr(int i) {return mdls[i];}
	};

extern void LUA_MDLDEF_RegisterLib();
extern void LUA_MODEL_RegisterLib();
