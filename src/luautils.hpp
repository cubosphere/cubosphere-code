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

#include "lua.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <stdarg.h>
#include <sstream>
#include "vectors.hpp"
#include "filesystem.hpp"

inline std::string LUA_GET_STRING(lua_State* state) // Complicated due to zero-terminators and garbage collection
	{
	size_t len;
	auto pointer = lua_tolstring(state, -1, &len);
	auto str = std::string(pointer, len);
	lua_pop(state,1);
	return str;
	}

inline bool LUA_GET_BOOL(lua_State* state)
	{
	bool res = lua_toboolean(state,-1);
	lua_pop(state,1);
	return res;
	}

inline int LUA_GET_INT(lua_State* state)
	{
	int res = lua_tonumber(state,-1);
	lua_pop(state,1);
	return res;
	}

inline unsigned long int LUA_GET_ULINT(lua_State* state)
	{
	unsigned long int res = lua_tonumber(state,-1);
	lua_pop(state,1);
	return res;
	}


inline double LUA_GET_DOUBLE(lua_State* state)
	{
	double res = lua_tonumber(state,-1);
	lua_pop(state,1);
	return res;
	}

inline void LUA_SET_NUMBER(lua_State* state, const lua_Number& num)
	{
	lua_pushnumber(state,num);
	}

inline void LUA_SET_STRING(lua_State* state, const std::string& str)
	{
	lua_pushlstring(state,str.c_str(),str.length());
	}

inline void LUA_SET_VECTOR3(lua_State* state, const Vector3d& v)
	{
	lua_newtable(state);
	lua_pushstring(state, "x");
	lua_pushnumber(state, v.x);
	lua_settable(state, -3);
	lua_pushstring(state, "y");
	lua_pushnumber(state, v.y);
	lua_settable(state, -3);
	lua_pushstring(state, "z");
	lua_pushnumber(state, v.z);
	lua_settable(state, -3);
	}

inline void LUA_SET_COLOR(lua_State* state, const Vector4d& v)
	{
	lua_newtable(state);
	lua_pushstring(state, "r");
	lua_pushnumber(state, v.x);
	lua_settable(state, -3);
	lua_pushstring(state, "g");
	lua_pushnumber(state, v.y);
	lua_settable(state, -3);
	lua_pushstring(state, "b");
	lua_pushnumber(state, v.z);
	lua_settable(state, -3);
	lua_pushstring(state, "a");
	lua_pushnumber(state, v.w);
	lua_settable(state, -3);
	}

extern Vector3d Vector3FromStack(lua_State *state);
extern Vector4d Vector4FromStack(lua_State *state);
extern float getfloatfield (lua_State *L, const char *key);

class LuaCFunctions;

class LuaBaseVar
	{
	protected:
		std::string myname;
	public:
		virtual ~LuaBaseVar() {}
		virtual void SetName(std::string name) {myname=name;}
		virtual std::string GetName() {return myname;}
		virtual void ReadFromState(lua_State *state) { lua_remove(state,1);}
		virtual void WriteInState(lua_State *state) {}
		virtual std::string GetVarString(int forscript=1) {return "";}
	};

class LuaNumberVar : public LuaBaseVar
	{
	protected:
		double var;
	public:
		LuaNumberVar() : var(0) {}
		LuaNumberVar(double n) : var(n) {}
		virtual void ReadFromState(lua_State *state) { var=lua_tonumber(state,1); LuaBaseVar::ReadFromState(state); }
		virtual void WriteInState(lua_State *state) {lua_pushnumber(state,var);}
		virtual std::string GetVarString(int forscript=1) {
			std::stringstream ns;
			ns << var;
			return ns.str();
			}
	};

class LuaStringVar : public LuaBaseVar
	{
	protected:
		std::string var;

	public:
		LuaStringVar() : var("") {}
		LuaStringVar(std::string n) : var(n) {}
		virtual void ReadFromState(lua_State *state) { var=lua_tostring(state,1); LuaBaseVar::ReadFromState(state); }
		virtual void WriteInState(lua_State *state) {lua_pushstring(state,var.c_str());}
		virtual std::string GetVarString(int forscript=1) {if (forscript) return "\""+var+"\""; else return var;}
	};

class LuaVarHolder //Used to encapsulate member values for items etc.
	{
	protected:
		std::vector<LuaBaseVar*> vars;
		virtual LuaBaseVar **RefForStore(std::string name);
		virtual LuaBaseVar **RefForRead(std::string name);
		virtual LuaBaseVar **Ref(std::string name);
	public:
		LuaVarHolder() {}
		virtual void clear() { for (unsigned int i=0; i<vars.size(); i++) if (vars[i]) {delete vars[i]; vars[i]=NULL;} vars.resize(0); }
		virtual ~LuaVarHolder() {clear();}
		virtual void StoreVar(lua_State *fromstate);
		virtual void GetVar(lua_State *tostate);
		virtual std::string GetVarString(std::string varname,int forscript=1);
		virtual int VarDefined(std::string vn);
		virtual void SetVar(std::string vname,std::string value);
		virtual void SetVar(std::string vname,double value);
		virtual void ListToConsole();
	};


extern LuaVarHolder* g_Vars();

class LuaAccess
	{
	protected:
		lua_State *state;
		std::string errorstring;
		int errorline;
		int typ;
		std::string lfname;
		virtual void LoadStdLibs();
	public:
		lua_State * GetLuaState() {return state;}
		static std::vector<LuaAccess*> gAllLuaStates;
		LuaAccess();
		void Reset();
		virtual ~LuaAccess();
		std::string GetFileName() {return lfname;}
		bool LoadFile(CuboFile *finfo,int t,int id);
		void Include(LuaCFunctions *funcs);
		void CallVA (const char *func, const char *sig, ...);
		bool FuncExists (const char *func);
		void PushInt(int i);
		int PopInt();
		void PushFloat(double f);
		double PopFloat();
		void PushString(std::string s);
		std::string PopString();
//   string GetDir() {return dir;}
		// string GetStdDir();
		int GetType() {return typ;}
		bool ExecStrings(std::vector<std::string> & inp);
		bool ExecString(std::string s);
	};



using LuaCFunc = struct
	{
	std::string name;
	lua_CFunction func;
	};

//Class holding a bunch of C-Functions exported to LUA
//Should be declared Virtual, so that we can add our needed funcs by Inheritance
//Simply Add it in the Constructor


class LuaCFunctions
	{
	protected:
		std::vector<LuaCFunc> funcs;
		// TLuaAccess *access;
	public:
		void RegisterToState(lua_State *state);
		void AddFunc(std::string name, lua_CFunction func);
		//void SetAccess(TLuaAccess *acc) {access=acc;}
	};




class LuaCuboLib : public LuaCFunctions
	{
	protected:

		static int LOG_Mode(lua_State *state);
		static int LOG_SetVerboseMode(lua_State *state);
		static int LOG_GetVerboseMode(lua_State *state);

		static int mycout(lua_State *state);






		static int CONFIG_Load(lua_State *state);
		static int CONFIG_Open(lua_State *state);
		static int CONFIG_Write(lua_State *state);
		static int CONFIG_Close(lua_State *state);


		//Command Line
		static int ARGS_Count(lua_State *state);
		static int ARGS_Key(lua_State *state);
		static int ARGS_Val(lua_State *state);


		static int TRANS_Clear(lua_State *state);
		static int TRANS_Add(lua_State *state);
		static int TRANS_Str(lua_State *state);
		static int TRANS_StrD(lua_State *state);
		static int TRANS_Load(lua_State *state);




		static int DEBUG(lua_State *state);

		static int GLOBAL_StartDeveloperMode(lua_State *state);
		static int GLOBAL_GetTime(lua_State *state);
		static int GLOBAL_GetElapsed(lua_State *state);
		static int GLOBAL_GetScale(lua_State *state);
		static int GLOBAL_GetFPS(lua_State *state);
		static int GLOBAL_SetVar(lua_State *state);
		static int GLOBAL_GetVar(lua_State *state);
		static int GLOBAL_VarDefined(lua_State *state);
		static int GLOBAL_Quit(lua_State *state);
		static int GLOBAL_SetMaxElapsed(lua_State *state);
		static int GLOBAL_SetMaxFrames(lua_State *state);
		static int GLOBAL_SetMinFrames(lua_State *state);

		static int SCORE_SetVar(lua_State *state);
		static int SCORE_GetVar(lua_State *state);
		static int SCORE_VarDefined(lua_State *state);

		static int SCORE_Load(lua_State *state);
		static int SCORE_Open(lua_State *state);
		static int SCORE_Write(lua_State *state);
		static int SCORE_Close(lua_State *state);

		static int SAVE_Load(lua_State *state);
		static int SAVE_Open(lua_State *state);
		static int SAVE_Write(lua_State *state);
		static int SAVE_Close(lua_State *state);



//    static int MOD_Clear(lua_State *state);
//    static int MOD_Add(lua_State *state);
		//  static int MOD_ClearBlacklist(lua_State *state);
		//  static int MOD_AddDirToBlackList(lua_State *state);
		static int MOD_GetName(lua_State *state);
		static int MOD_SetName(lua_State *state);






		//   static int DIR_GetFiles(lua_State *state);
		//  static int DIR_GetFile(lua_State *state);
		static int DIR_GetProfileDir(lua_State *state);
		static int DIR_GetDataDir(lua_State *state);
		//  static int DIR_FileExists(lua_State *state);



		static int INCLUDE(lua_State *state); //Global Scope only!
		static int INCLUDEABSOLUTE(lua_State *state); //Global Scope only!
		static int USING(lua_State *state); //Load another internal library




	public:
		LuaCuboLib();
		//void SetUsage(TTextureServer *ts);
	};

extern LuaCuboLib* g_CuboLib();




extern int *g_LogMode();
extern void coutlog(std::string s,int typ=0);
extern void closelog();

extern void  ReloadLanguage();

extern LuaAccess* g_CallAccess();

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
