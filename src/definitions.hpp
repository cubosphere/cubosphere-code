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
#include <unordered_map>
#include <memory>

#include "luautils.hpp"

/* Here we provide a parser for block and texture definitions */


#define COND_LUA_CALL(nam,stret,pspec,...) if (!(lua.FuncExists(nam))) return stret; lua.CallVA(nam,pspec, ##__VA_ARGS__)

class BaseLuaDef {
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

template<typename T> class BaseDefServer { // TODO: Get rid of `x_to_x` usage, use smart pointers in correct way
	protected:
		//std::vector<T*> defs;
		std::unordered_map<int, std::unique_ptr<T>> defs;
		std::unordered_map<std::string, int> name_to_id;
		int max_id = 0;
	public:
		void clear() {
			defs.clear();
			name_to_id.clear();
			max_id = 0;
			}
		virtual ~BaseDefServer() {}
		void Reload() {
			for(auto& elem: defs) { elem.second->Reload(); }
			}
		int AddDef(std::string name) {
			if (name_to_id.count(name)) { return name_to_id.at(name); }
			int id = max_id++;
			name_to_id.emplace(name, id);
			defs.emplace(id, std::make_unique<T>());
			auto& obj = defs.at(id);
			obj->SetName(name);
			obj->SetID(id);
			obj->LoadDef();
			return id;
			}
		T* GetDefPtr(int i) {return defs[i].get();}
	};

////////////////////////////////////

class Menu : public BaseLuaDef {
	protected:
		int change;
		std::string nextname;
	public:
		virtual int GetType() {return FILE_MENUDEF;}
		Menu() : BaseLuaDef(), change(0) {};
		virtual void LoadDef() { BaseLuaDef::LoadDef(); };
		virtual void LoadDef(std::string cname);
		// virtual void Reload();
		void Render();
		void Think();
		void PostThink();
		void SendKey(int key, int down, int toggle);
		void SendTextInput(std::string&);
		void SendJoyButton(int joy,int button,int dir,int down,int toggle);
		void JoyAxisChange(int joys,int axis,double val,double pval);
	};

extern void LUA_MENU_RegisterLib();

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
