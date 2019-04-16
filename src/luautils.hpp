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
#include <variant>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <any>
#include "vectors.hpp"
#include "filesystem.hpp"

#if LUA_VERSION_NUM > 501
#define lua_getlen(...) lua_rawlen(__VA_ARGS__);
#else
#define lua_getlen(...) lua_objlen(__VA_ARGS__);
#endif

using LuaVATypesIn = std::variant<
		double,
		float,
		int,
		bool,
		std::string,
		Vector3d *
		>;

using LuaVATypesOut = std::variant<
		Vector3d *,
		double *,
		float *,
		int *,
		bool *,
		std::string *
		>;

using LuaVAListIn = std::initializer_list<LuaVATypesIn>;
using LuaVAListOut = std::initializer_list<LuaVATypesOut>;

inline bool lua_isfulluserdata(lua_State *L, int index) {
	return lua_type(L, index) == LUA_TUSERDATA;
	};

inline std::string LUA_GET_STRING(lua_State* state, int idx = -1) { // Complicated due to zero-terminators and garbage collection
	size_t len;
	auto pointer = lua_tolstring(state, idx, &len);
	auto str = std::string(pointer, len);
	if(idx == -1) { lua_pop(state,1); }
	return str;
	}

inline bool LUA_GET_BOOL(lua_State* state, int idx = -1) {
	bool res = lua_toboolean(state,idx);
	if(idx == -1) { lua_pop(state,1); }
	return res;
	}

inline int LUA_GET_INT(lua_State* state, int idx = -1) {
	int res = lua_tointeger(state,idx);
	if(idx == -1) { lua_pop(state,1); }
	return res;
	}

inline unsigned long int LUA_GET_ULINT(lua_State* state, int idx = -1) {
	unsigned long int res = lua_tonumber(state,idx);
	if(idx == -1) { lua_pop(state,1); }
	return res;
	}


inline double LUA_GET_DOUBLE(lua_State* state, int idx = -1) {
	double res = lua_tonumber(state,idx);
	if(idx == -1) { lua_pop(state,1); }
	return res;
	}

inline float LUA_GET_FLOAT(lua_State* state, int idx = -1) {
	float res = lua_tonumber(state,idx);
	if(idx == -1) { lua_pop(state,1); }
	return res;
	}

inline std::optional<Vector3d> LUA_GET_VECTOR3(lua_State* state, int idx = -1) {
	if (!lua_istable(state, idx)) { return std::nullopt; }
	Vector3d vec;
	lua_getfield(state, idx, "x");
	if (lua_isnumber(state, -1)) {
			vec.x = lua_tonumber(state, 1);
			}
	lua_pop(state, 1);
	lua_getfield(state, idx, "y");
	if (lua_isnumber(state, -1)) {
			vec.y = lua_tonumber(state, 1);
			}
	lua_pop(state, 1);
	lua_getfield(state, idx, "z");
	if (lua_isnumber(state, -1)) {
			vec.z = lua_tonumber(state, 1);
			}
	lua_pop(state, 1);
	return vec;
	}

inline void LUA_SET_NUMBER(lua_State* state, const lua_Number& num) {
	lua_pushnumber(state,num);
	}

inline void LUA_SET_BOOL(lua_State* state, const bool& b) {
	lua_pushboolean(state,b);
	}

inline void LUA_SET_STRING(lua_State* state, const std::string& str) {
	lua_pushlstring(state,str.c_str(),str.length());
	}

inline void LUA_SET_VECTOR3(lua_State* state, const Vector3d& v) {
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

inline void LUA_SET_COLOR(lua_State* state, const Vector4d& v) {
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

class LuaPushable;
class LuaCFunctions;
class LuaModule;

struct LuaCXXDataStorage { // POD struct
	std::any* obj; // Must contain std::shared_ptr<OBJ_TYPE>
	};

class LuaPushable {
	public:
		virtual void LuaPush(lua_State*) = 0;
		virtual ~LuaPushable() {};
	};

class LuaGettable {
	public:
		virtual bool LuaCheckType(lua_State*, int) = 0;
		virtual bool LuaLoad(lua_State*, int) = 0;
		virtual bool LuaPop(lua_State* state) { bool res = LuaLoad(state, -1); if(res) lua_pop(state, 1); return res; };
		virtual ~LuaGettable() {};
	};

class LuaType: public LuaPushable, public LuaGettable {
	public:
		virtual bool LuaInited(lua_State*) { return true; };
		virtual void LuaInit(lua_State*) {};
		virtual ~LuaType() {};
	};

constexpr auto LuaIndexMetamethod = "__index";
constexpr auto LuaGCMetamethod = "__gc";
constexpr auto LuaEqalityMetamethod = "__eq";

inline int LuaCXXDataGC(lua_State* L) {
	auto storage = (LuaCXXDataStorage*)lua_touserdata(L, -1);
	delete storage->obj;
	return 0;
	};

inline int LuaCXXDataEQ(lua_State* L) { // FIXME: always false?
	auto storage1 = (LuaCXXDataStorage*)lua_touserdata(L, -1);
	auto storage2 = (LuaCXXDataStorage*)lua_touserdata(L, -2);
	LUA_SET_BOOL(L, storage1->obj == storage2->obj);
	return 1;
	};


template<typename T> class LuaCXXData: public LuaType { // Stand back everybody! Template's black magic is coming!
	protected:
		virtual void UserInit(lua_State*) {}; // Override to cusomize

		std::string LuaName; // Name on lua side
		std::shared_ptr<T> obj;
		bool LuaTypeExtraCheck(lua_State* L, int idx) {
			lua_getmetatable(L, idx);
			luaL_getmetatable(L, LuaName.c_str());
			bool res = lua_equal(L, -1, -2);
			lua_pop(L, 2);
			return res;
			};

		std::unordered_map<std::string, lua_CFunction> methods;
		std::unordered_map<std::string, lua_CFunction> metamethods = {
				{LuaGCMetamethod, LuaCXXDataGC}, // Default GC. Override with caution!
				{LuaEqalityMetamethod, LuaCXXDataEQ} // Default comparator. Objects are eqaul if they share same C++ object.
			};
	public:
		LuaCXXData(std::string name): LuaName(name) {};
		template <typename... Ts> LuaCXXData(std::string name, Ts&&... args): LuaName(name) { emplace(std::forward<Ts>(args)...); };
		virtual ~LuaCXXData() {};
		std::shared_ptr<T> GetObj() { return obj; };
		void SetObj(std::shared_ptr<T> o) { obj = o; };
		template <typename... Ts> void emplace(Ts&&... args) { obj = std::make_shared<T>(std::forward<Ts>(args)...); };
		virtual bool LuaCheckType(lua_State* L, int idx) override {
			if (not (lua_isfulluserdata(L, idx) and LuaTypeExtraCheck(L, idx))) { return false; }
			auto storage = (LuaCXXDataStorage*)lua_touserdata(L, idx);
			return storage and storage->obj and storage->obj->type() == typeid(std::shared_ptr<T>);
			};
		virtual bool LuaLoad(lua_State* L, int idx) override {
			if (LuaCheckType(L, idx)) {
					SetObj(*std::any_cast<std::shared_ptr<T>>(((LuaCXXDataStorage*)lua_touserdata(L, idx))->obj)); // Shouldn't throw or segfault
					return true;
					}
			else {
					return false;
					}
			};
		virtual void LuaPush(lua_State* L) override {
			auto storage = (LuaCXXDataStorage*) lua_newuserdata(L, sizeof(LuaCXXDataStorage));
			storage->obj = new std::any(obj); // Deleted by LuaCXXDataGC (lua GC)
			luaL_getmetatable(L, LuaName.c_str());
			lua_setmetatable(L, -2);
			};

		void AddMethod(std::string name, lua_CFunction func) { methods.emplace(name, func); }; // Call these two in your LuaInit and then call parrent one
		void AddMetamethod(std::string name, lua_CFunction func) { metamethods.emplace(name, func); };

		bool LuaInited(lua_State* L) override {
			luaL_getmetatable(L, LuaName.c_str());
			bool res = lua_istable(L, -1);
			lua_pop(L, 1);
			return res;
			};
		virtual void LuaInit(lua_State* L) override {
			if (luaL_newmetatable(L, LuaName.c_str())) { // We aren't inited yet
					UserInit(L); // Load stuff
					for(auto& e: metamethods) {
							LUA_SET_STRING(L, e.first);
							lua_pushcfunction(L, e.second);
							lua_settable(L, -3);
							};
					// Lua stack: Metatable
					if (not metamethods.count(LuaIndexMetamethod)) { // If there is no custom index method, use methods table
							lua_createtable(L, 0, methods.size());
							// Lua stack: Metatable, Methods table
							lua_pushvalue(L, -1);
							// Lua stack: Metatable, Methods table, Methods table
							lua_setfield(L, -3, LuaIndexMetamethod);
							// Lua stack: Metatable, Methods table
							for(auto& e: methods) {
									LUA_SET_STRING(L, e.first);
									lua_pushcfunction(L, e.second);
									lua_settable(L, -3);
									};
							// Lua stack: Metatable, Methods table
							lua_pop(L, 1);
							};
					};
			// Lua stack: Metatable
			lua_pop(L, 1);
			};
	};

class LuaBaseVar {
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

class LuaNumberVar : public LuaBaseVar {
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

class LuaStringVar : public LuaBaseVar {
	protected:
		std::string var;

	public:
		LuaStringVar() : var("") {}
		LuaStringVar(std::string n) : var(n) {}
		virtual void ReadFromState(lua_State *state) { var=lua_tostring(state,1); LuaBaseVar::ReadFromState(state); }
		virtual void WriteInState(lua_State *state) {lua_pushstring(state,var.c_str());}
		virtual std::string GetVarString(int forscript=1) {if (forscript) return "\""+var+"\""; else return var;}
	};

class LuaVarHolder { //Used to encapsulate member values for items etc.
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

class LuaAccess final {
	protected:
		lua_State *state;
		std::string errorstring;
		int errorline;
		int typ;
		std::string lfname;
		void LoadStdLibs();
	public:
		lua_State * GetLuaState() {return state;}
		void LoadUserLibs();
		static std::unordered_map<std::string, LuaAccess*> gAllLuaStates;
		LuaAccess();
		void Reset();
		virtual ~LuaAccess();
		std::string GetFileName() {return lfname;}
		bool LoadFile(const std::unique_ptr<CuboFile>& finfo,int t,int id);
		void Include(LuaCFunctions *funcs);
		//void Include(LuaModule &funcs);
		bool CallVA(const char* func, std::optional<LuaVAListIn> iargs = std::nullopt, std::optional<LuaVAListOut> oargs = std::nullopt);
		bool CallVAIfPresent(const char* func, std::optional<LuaVAListIn> iargs = std::nullopt, std::optional<LuaVAListOut> oargs = std::nullopt);
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

//Class holding a bunch of C-Functions exported to LUA
//Should be declared Virtual, so that we can add our needed funcs by Inheritance
//Simply Add it in the Constructor


class LuaCFunctions {
	protected:
		std::unordered_map<std::string, lua_CFunction> funcs; // It also makes correct overriding scheme
		// TLuaAccess *access;
	public:
		virtual void RegisterToState(lua_State *state);
		virtual void AddFunc(const std::string& name, lua_CFunction func);
		//void SetAccess(TLuaAccess *acc) {access=acc;}
	};

class LuaModule: public LuaCFunctions {
	protected:
		std::string modname;
		std::unordered_set<std::unique_ptr<LuaType>> types;
	public:
		LuaModule(const std::string& name);
		virtual ~LuaModule() {};

		virtual void InitToState(lua_State *state);
		virtual void RegisterToState(lua_State *state) override;
		virtual void PushToState(lua_State *state);
		virtual void AddType(std::unique_ptr<LuaType>&& type);
	};

class LuaCuboLib : public LuaCFunctions {
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
