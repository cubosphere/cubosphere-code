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

#include "globals.hpp"
#include <iostream>
#include <vector>

#include "luautils.hpp"

/* Here we provide a parser for block and texture definitions */


#define COND_LUA_CALL(nam,stret,pspec,...) if (!(lua.FuncExists(nam))) return stret; lua.CallVA(nam,pspec, ##__VA_ARGS__)


class BaseLuaDef
	{
	protected:
		std::string name;
		std::string fname;
		LuaAccess lua;
		int isloaded;
		int myid;
		virtual int SendIDWhenPrecache() {return 0;}
	public:
		virtual ~BaseLuaDef();
		BaseLuaDef() : name(""), isloaded(0), myid(-1) {}
		virtual int GetType()=0;
		void SetName(std::string cname) {name=cname;}
		std::string GetName() {return name;}
		int IsLoaded() {return isloaded;}
		void SetID(int cid) {myid=cid;}
		virtual void LoadDef();
		virtual void Reload();
	};

template<typename T> class BaseDefServer
	{
	protected:
		std::vector<T*> defs;
	public:
		void clear()
			{
			for (unsigned int i=0; i<defs.size(); i++) if (defs[i]) { delete defs[i]; defs[i]=NULL;}
			defs.resize(0);
			}
		virtual ~BaseDefServer()
			{
			clear();
			}
		int GetDef(std::string name)
			{
			for (unsigned int i=0; i<defs.size(); i++) if (defs[i]->GetName()==name) { return (i); }
			return -1;
			}
		void Reload()
			{
			for (unsigned int i=0; i<defs.size(); i++) { defs[i]->Reload(); }
			}
		int AddDef(std::string name)
			{
			int def=GetDef(name);
			if (def>-1) { return def; } //Have it already
			defs.push_back(new T());
			defs.back()->SetName(name);
			def=defs.size()-1;
			defs[def]->SetID(def);
			defs[def]->LoadDef();
			return def;
			}
		T* GetDefPtr(int i) {return defs[i];}
	};

////////////////////////////////////

class Menu : public BaseLuaDef
	{
	protected:
		int change;
		std::string nextname;
	public:
		virtual int GetType() {return FILE_MENUDEF;}
		Menu() : BaseLuaDef(), change(0) {};
		virtual void LoadDef(std::string cname);
		// virtual void Reload();
		void Render();
		void Think();
		void PostThink();
		void SendKey(int key, int down, int toggle);
		void SendJoyButton(int joy,int button,int dir,int down,int toggle);
		void JoyAxisChange(int joys,int axis,double val,double pval);
	};

extern void LUA_MENU_RegisterLib();

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
