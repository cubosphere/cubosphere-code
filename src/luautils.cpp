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

#include <iostream>
#include <string>
#include <sstream>

#ifdef WIN32
#ifdef VCPP

#else
#include <libgen.h>
#include <dirent.h>
#endif
#else
#include <libgen.h>
#include <dirent.h>
#endif

#include <string.h>
#include <algorithm>

#include "luautils.hpp"
#include <stdarg.h>
#include <cstring>
#include "vectors.hpp"
#include "game.hpp"
#include "globals.hpp"
#include "cuboactors.hpp"
#include "themes.hpp"
#include "sounds.hpp"
#include "posteffects.hpp"
#include "cubopathfind.hpp"
#include "cubolevels.hpp"
#include "cuboenemies.hpp"
#include "glutils.hpp"
#include "spriteutils.hpp"
#include "cubopathfind.hpp"
#include "console.hpp"

#include <GL/glew.h>
#include <SDL.h>

using namespace std::string_literals;

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

std::vector<LuaAccess*> LuaAccess::gAllLuaStates;

static LuaAccess *CallAccess=NULL;

LuaAccess *g_CallAccess() {return CallAccess;}



static LuaCuboLib g_cubolib;


static LuaVarHolder g_VarHolder;
static LuaVarHolder g_HighScoreHolder;

static LuaVarHolder g_TranslationHolder;
static int g_logmode;


LuaVarHolder* g_Vars() {return &g_VarHolder;}
LuaVarHolder* g_HighScores() {return &g_HighScoreHolder;}
LuaVarHolder* g_Translation() {return &g_TranslationHolder;}
int* g_LogMode() {return &g_logmode;}

LuaCuboLib* g_CuboLib() {return &g_cubolib;}

int lua_doCuboFile(lua_State *state, CuboFile *f) {
	int res;
	if (f->IsHDDFile()) { res=luaL_dofile(state,f->GetHDDName().c_str()); }
	else {
			char *c=(char*)(f->GetData());
			//cout << c << endl;
			luaL_loadbuffer(state,c,f->GetSize(),f->GetNameForLog().c_str());
			res=lua_pcall(state, 0, LUA_MULTRET, 0);
			}

	if(res) { // if non-0, then an error
			// the top of the stack should be the error string
			if (!lua_isstring(state, lua_gettop(state))) {
					coutlog("Strange Lua Error without error message...",1);
					}

			// get the top of the stack as the error and pop it off
			std::string str = lua_tostring(state, lua_gettop(state));
			coutlog("LUA-ERROR in File "+f->GetNameForLog()+" :  "+str,1);

			}

	return res;
	}




LuaAccess::LuaAccess() : state(NULL), errorstring(""), errorline(-1) {

	state=lua_open();
	if (!state) {
			errorstring="Could not init a LUA-Parser";
			errorline=0;
			}
	LoadStdLibs();
#ifdef LUAJIT
//  luaJIT_setmode(state,0,LUAJIT_MODE_ENGINE | LUAJIT_MODE_ON);
#endif

	}

LuaAccess::~LuaAccess() {
	for (unsigned int i=0; i<gAllLuaStates.size(); i++) if (gAllLuaStates[i]==this) {gAllLuaStates.erase(gAllLuaStates.begin()+i); break;}
	lua_close(state);
	state=NULL;
	}

void LuaAccess::Reset() {
	lua_close(state);
	state=lua_open();
	if (!state) {
			errorstring="Could not init a LUA-Parser";
			errorline=0;
			}
	LoadStdLibs();
	for (unsigned int i=0; i<gAllLuaStates.size(); i++) if (gAllLuaStates[i]==this) {gAllLuaStates.erase(gAllLuaStates.begin()+i); break;}

	}

void LuaAccess::LoadStdLibs() {
	if (!state) { return; }
	luaL_openlibs(state);
	}

/*
string TLuaAccess::GetStdDir()
{
 return GetFileName("",typ,"");
}
*/

bool LuaAccess::LoadFile(CuboFile *finfo,int t,int id) {
	if (!state) { coutlog("No lua state present",1); return false;}

	typ=t;

	std::string fname=finfo->GetNameForLog();
//  g_MomAccess=this;
	if (g_VerboseMode()) { coutlog("Loading Lua script: "+fname); }

	lua_pushnumber(state,t);
	lua_setglobal(state,"LUA_DEF_TYPE");
	lua_pushstring(state,finfo->GetName().c_str());
	lua_setglobal(state,"LUA_DEF_NAME");
	lua_pushstring(state,finfo->GetBasename().c_str());
	lua_setglobal(state,"LUA_DEF_BASENAME");
	lua_pushstring(state,finfo->GetDirname().c_str());
	lua_setglobal(state,"LUA_DEF_DIRNAME");
	lua_pushnumber(state,id);

	lua_setglobal(state,"LUA_DEF_ID");


// coutlog("Loading Lua " + lfname );

	int res=lua_doCuboFile(state, finfo);
	if (res) {
			std::ostringstream os;
			os << "ERROR "<< " -> " << lua_tostring(state, -1);
			coutlog(os.str(),1);
			}
	else {
			lfname=finfo->GetName();

			gAllLuaStates.push_back(this);
			}
	return (!res);
	}

char *g_luareaderblock=NULL;

const char * g_luareader(lua_State *L,void *data,size_t *size) {
	if (g_luareaderblock) { free(g_luareaderblock); }
	std::vector<std::string> * inp=(std::vector<std::string> *)data;

	if (!inp->size()) {(*size)=0; return NULL;}

	while (!((*inp)[0].length())) {
			inp->erase(inp->begin());
			if (!(inp->size())) {
					*size=0;
					return "";
					}
			}

	(*size)=(*inp)[0].length()+1;
	std::string res=(*inp)[0]+'\n';
	inp->erase(inp->begin());

	g_luareaderblock=strdup(res.c_str());
	return g_luareaderblock;
	}

bool LuaAccess::ExecStrings(std::vector<std::string> & inp) {
	if (!state) { return false; }
	typ=-1;

	int res=lua_load(state, &g_luareader, &inp,NULL);
	if (!res) { res=lua_pcall(state, 0, LUA_MULTRET, 0); }
	if (res) {
			std::ostringstream os;
			os << "ERROR "<< " -> " << lua_tostring(state, -1);
			coutlog(os.str(),1);
			return false;
			}

	return true;
	}


bool LuaAccess::ExecString(std::string s) {
	if (!state) { return false; }
	typ=-1;

	int res=luaL_dostring(state,s.c_str());
	if (res) {
			std::ostringstream os;
			os << "ERROR "<< " -> " << lua_tostring(state, -1);
			coutlog(os.str(),1);
			return false;
			}

	return true;
	}


void LuaAccess::PushInt(int i) {
	lua_pushnumber(state, i);
	}

int LuaAccess::PopInt() {
	int i = (int)lua_tonumber(state, -1);
	lua_pop(state,1);
	return i;
	}


void LuaAccess::PushFloat(double f) {
	lua_pushnumber(state, f);
	}

double LuaAccess::PopFloat() {
	double f = lua_tonumber(state, -1);
	lua_pop(state,1);
	return f;
	}

void LuaAccess::PushString(std::string s) {
	lua_pushstring(state, s.c_str());
	}

std::string LuaAccess::PopString() {
	std::string s = lua_tostring(state, -1);
	lua_pop(state,1);
	//string s=cp;
	return s;
	}

bool LuaAccess::CallVA(const char* func, std::optional<LuaVAListIn> iargs, std::optional<LuaVAListOut> oargs) {
	CallAccess=this;
	int narg = 0;
	lua_getglobal(state, func);

	if (iargs) {
			narg = iargs->size();
			for (auto const& elem: *iargs) {
					std::visit(overloaded {
						[this](double arg) { LUA_SET_NUMBER(state, arg); },
						[this](float arg) { LUA_SET_NUMBER(state, arg); },
						[this](int arg) { LUA_SET_NUMBER(state, arg); },
						[this](std::string arg) { LUA_SET_STRING(state, arg); },
						[this](Vector3d* arg) { LUA_SET_VECTOR3(state, *arg); },
						[this,&func](auto) {std::ostringstream os; os << "ERROR (in calling '"<< func <<"')"<< " -> " << lua_tostring(state, -1); coutlog(os.str(),1);}
						}, elem);
					luaL_checkstack(state, 1, "too many arguments");
					}
			}
	int nres = 0;
	if (oargs) nres = oargs->size();
	/* do the call */
	if (lua_pcall(state, narg, nres, 0) != 0) { /* do the call */
			std::ostringstream os;
			os << "ERROR (in calling '"<<func <<"')"<< " -> " << lua_tostring(state, -1);
			coutlog(os.str(),1);
			return false;
			}
	if (oargs) {
			nres = -nres;  /* stack index of first result */
			auto print_err = [func]() {
				coutlog("Error running function '"s + func + "' : wrong result type"s, 1);
				};
			for (auto const& elem: *oargs) {
					std::visit(overloaded {
						[this,nres,&print_err](double* arg) {
							if (!lua_isnumber(state, nres)) { print_err(); }
							else { (*arg) = lua_tonumber(state, nres); }
							},
						[this,nres,&print_err](float* arg) {
							if (!lua_isnumber(state, nres)) { print_err(); }
							else { (*arg) = lua_tonumber(state, nres); }
							},
						[this,nres,&print_err](int* arg) {
							if (!lua_isnumber(state, nres)) { print_err(); }
							else { (*arg) = lua_tonumber(state, nres); }
							},
						[this,nres,&print_err](std::string* arg) {
							if (!lua_isnumber(state, nres)) { print_err(); }
							else {
									size_t len;
									auto pointer = lua_tolstring(state, nres, &len);
									(*arg) = std::string(pointer, len);
									}
							},
						[this,nres,&print_err](Vector3d* arg) {
							auto& vec = *arg;
							lua_getfield(state, nres, "x");
							if (lua_isnumber(state, 1)) {
									vec.x = lua_tonumber(state, 1);
									}
							lua_pop(state, 1);
							lua_getfield(state, nres, "y");
							if (lua_isnumber(state, 1)) {
									vec.y = lua_tonumber(state, 1);
									}
							lua_pop(state, 1);
							lua_getfield(state, nres, "z");
							if (lua_isnumber(state, 1)) {
									vec.z = lua_tonumber(state, 1);
									}
							lua_pop(state, 1);
							},
						[this,&func](auto) {std::ostringstream os; os << "ERROR (in calling '"<< func <<"')"<< " -> " << lua_tostring(state, -1); coutlog(os.str(),1);}
						}, elem);
					}
			}
	return true;
	}

void LuaAccess::CallVA (const char *func, const char *sig, ...) {
	CallAccess=this;
	va_list vl;
	int narg, nres;  /* number of arguments and results */

	va_start(vl, sig);
	lua_getglobal(state, func);  /* get function */

	//if (lua_isnil(state,lua_gettop(state))) { lua_pop(state,1); return false; }

	/* push arguments */
	narg = 0;
	Vector3d p;
	while (*sig) {  /* push arguments */
			switch (*sig++) {

					case 'd':  /* double argument */
						lua_pushnumber(state, va_arg(vl, double));
						break;
					/*case 'f':
					    lua_pushnumber(state, va_arg(vl, float));
					    break;
					*/
					case 'i':  /* int argument */
						lua_pushnumber(state, va_arg(vl, int));
						break;

					case 's':  /* string argument */
						lua_pushstring(state, va_arg(vl, char *));
						break;

					case 'v': //Push a 3d-Vector POINTER!!
						p=*((Vector3d*)va_arg(vl, Vector3d*));
						lua_newtable(state);

						lua_pushstring(state, "x");
						lua_pushnumber(state, p.x);
						lua_rawset(state, -3);

						lua_pushstring(state, "y");
						lua_pushnumber(state, p.y);
						lua_rawset(state, -3);

						lua_pushstring(state, "z");
						lua_pushnumber(state, p.z);
						lua_rawset(state, -3);


						break;

					case '>':
						goto endwhile;

					default:
						std::ostringstream os;
						os << "ERROR "<< " ->  Calling '"<<func << "' : invalid option ("<< "" << *(sig - 1) <<")";
						coutlog(os.str(),1);
					}
			narg++;
			luaL_checkstack(state, 1, "too many arguments");
		} endwhile:

	/* do the call */
	nres = strlen(sig);  /* number of expected results */
	if (lua_pcall(state, narg, nres, 0) != 0) { /* do the call */
			std::ostringstream os;
			os << "ERROR (in calling '"<<func <<"')"<< " -> " << lua_tostring(state, -1);
			coutlog(os.str(),1);
			}
	/* retrieve results */
	nres = -nres;  /* stack index of first result */
	while (*sig) {  /* get results */
			switch (*sig++) {

					case 'd':  /* double result */
						if (!lua_isnumber(state, nres)) {
								std::ostringstream os; os << "Error running function '" << func << "' : wrong result type" ; coutlog(os.str(),1);

								}
						*va_arg(vl, double *) = lua_tonumber(state, nres);
						lua_pop(state,1);
						break;

					case 'i':  /* int result */
						if (!lua_isnumber(state, nres)) {
								std::ostringstream os; os << "Error running function '" << func << "' : wrong result type" ; coutlog(os.str(),1);

								}
						*va_arg(vl, int *) = (int)lua_tonumber(state, nres);
						lua_pop(state,1);
						break;

					case 's':  /* string result */
						if (!lua_isstring(state, nres)) {
								std::ostringstream os; os << "Error running function '" << func << "' : wrong result type" ; coutlog(os.str(),1);

								}
						*va_arg(vl, const char **) = lua_tostring(state, nres);
						lua_pop(state,1);
						break;

					case 'v':
						/*  lua_rawgeti(state,-1,1);
						  p.x= lua_tonumber(state, -1);
						  lua_pop(state, 1);

						  lua_next(state, -3);
						  p.y= lua_tonumber(state, -1);
						  lua_pop(state, 1);

						  lua_next(state, -3);
						  p.z= lua_tonumber(state, -1);
						  lua_pop(state, 1);

						*/
						lua_pushnil(state);

						while(lua_next(state, -2)) {  // <== here is your mistake
								if(lua_isnumber(state, -1)) {
										std::string kompo = (char *)lua_tostring(state, -2);
										float v = (float)lua_tonumber(state, -1);
										if (kompo=="x") { p.x=v; }
										else if (kompo=="y") { p.y=v; }
										else if (kompo=="z") { p.z=v; }
										//use number
										}
								lua_pop(state, 1);
								}
						// lua_pop(state, 1);




						((Vector3d *)(va_arg(vl, Vector3d *)))->xyz(p.x,p.y,p.z);

						lua_pop(state, 1);
						break;



					default: {
							std::ostringstream os; os << "Error running function '" << func << "' : wrong result type" ; coutlog(os.str(),1);

							}
					}
			nres++;
			}
	va_end(vl);
	return ;
	}

bool LuaAccess::FuncExists (const char *func) {
	lua_getglobal(state, func);  /* get function */

	if (lua_isnil(state,lua_gettop(state))) { lua_pop(state,1); return false; }
	lua_pop(state,1); return true;

	}



void LuaAccess::Include(LuaCFunctions *funcs) {
	//Register them all
	funcs->RegisterToState(state);
	}


void LuaCFunctions::RegisterToState(lua_State *state) {
	for (unsigned int i=0; i<funcs.size(); i++) {
			lua_register(state, funcs[i].name.c_str(), funcs[i].func);
			}
	}

void LuaCFunctions::AddFunc(std::string name, lua_CFunction func) {
	LuaCFunc nf;
	nf.name=name;
	nf.func=func;
	funcs.push_back(nf);
	}

static FILE *g_logfile=NULL;


void closelog() {
	if (*(g_LogMode())) {
			if (g_logfile) {
					fprintf(g_logfile,"--------------\n");
					fprintf(g_logfile,"LOGFILE CLOSED\n");
					fclose(g_logfile);
					}
			}
	}

int LuaCuboLib::LOG_SetVerboseMode(lua_State *state) {
	int m=LUA_GET_INT(state);
	g_VerboseMode(m);
	return 0;
	}


int LuaCuboLib::LOG_GetVerboseMode(lua_State *state) {
	int m=g_VerboseMode();
	LUA_SET_NUMBER(state, m);
	return 1;
	}

int LuaCuboLib::LOG_Mode(lua_State *state) {
	int prior=*g_LogMode();
	*(g_LogMode())=LUA_GET_INT(state);
	if ((!prior) && (*(g_LogMode()))) {
			std::string s=g_ProfileDir()+"/logfile.txt";

			s=PlattformFilename(s);
			std::cout << "Start logging to " << s << std::endl;
			g_logfile=fopen(s.c_str(),"w");
			if (!g_logfile) {
					std::cout << "Could not write to logfile " << s << std::endl;
					*(g_LogMode())=0;
					}
			else {
					fprintf(g_logfile,"CUBOSPHERE LOGFILE\n");
					fprintf(g_logfile,"-------------------\n");
					fflush(g_logfile);
					//fclose(g_logfile);
					}
			}
	else if ((prior) && (!(*(g_LogMode())))) { closelog(); }
	///TODO: Create File if set to 1
	return 0;
	}

int LUA_ExecInState(lua_State *state) {
	std::string cmd=LUA_GET_STRING(state);
	std::string statename=LUA_GET_STRING(state);
	for (unsigned int i=0; i<LuaAccess::gAllLuaStates.size(); i++) {
			// coutlog("Comparing "+statename+" and "+TLuaAccess::gAllLuaStates[i]->GetFileName());
			if (statename==LuaAccess::gAllLuaStates[i]->GetFileName()) {
					int res=luaL_dostring(LuaAccess::gAllLuaStates[i]->GetLuaState(),cmd.c_str());
					LUA_SET_NUMBER(state, res);
					return 1;
					}
			}
	coutlog("No Lua state with name "+statename+" loaded",2);
	LUA_SET_NUMBER(state, 1);
	return 1;
	}

void coutlog(std::string s,int typ) {
	std::cout << s << std::endl;

	if (*(g_LogMode())) {
			//  string fn=g_ProfileDir()+"/logfile.txt";
			//  FILE *f=fopen(fn.c_str(),"a");
			if (!g_logfile) {
					std::cout << "Could not write to logfile " << s << std::endl;
					*(g_LogMode())=0;
					}
			else {
					fprintf(g_logfile,"%s\n",s.c_str());
					fflush(g_logfile);
					//fclose(f);
					}
			}

	CuboConsole::GetInstance()->AddLine(s,typ);
	}

int LuaCuboLib::mycout(lua_State *state) {
	int typ=0;
	int argc=lua_gettop(state);
	std::string s;
	if (argc==2) {
			typ=lua_tonumber(state, -1);
			s=lua_tostring(state, -2);
			}
	else { s=lua_tostring(state, -1); }
	coutlog(s,typ);


	return 0;
	//  lua_pushnumber(state, 0);
	//return 1;
	}




LuaCuboLib::LuaCuboLib() {

	AddFunc("LOG_Mode",LOG_Mode);
	AddFunc("LOG_SetVerboseMode",LOG_SetVerboseMode);
	AddFunc("LOG_GetVerboseMode",LOG_GetVerboseMode);
	AddFunc("print",mycout);


	LUA_ACTOR_RegisterLib();
	LUA_BASIS_RegisterLib();
	LUA_BLEND_RegisterLib();
	LUA_BLOCK_RegisterLib();
	LUA_CAM_RegisterLib();
	LUA_COLOR_RegisterLib();
	LUA_CULL_RegisterLib();
	LUA_DEPTH_RegisterLib();
	LUA_DEVICE_RegisterLib();
	LUA_EFFECT_RegisterLib();
	LUA_ENEMY_RegisterLib();
	LUA_FONT_RegisterLib();
	LUA_GAME_RegisterLib();
	LUA_ITEM_RegisterLib();
	LUA_KEYB_RegisterLib();
	LUA_LEVEL_RegisterLib();
	LUA_LIGHT_RegisterLib();
	LUA_MATERIAL_RegisterLib();
	LUA_MATRIX_RegisterLib();
	LUA_MDLDEF_RegisterLib();
	LUA_MENU_RegisterLib();
	LUA_MODEL_RegisterLib();
	LUA_MOUSE_RegisterLib();
	LUA_PLAYER_RegisterLib();
	LUA_SHADER_RegisterLib();
	LUA_SIDE_RegisterLib();
	LUA_SOUND_RegisterLib();
	LUA_TEXDEF_RegisterLib();
	LUA_TEXTURE_RegisterLib();
	LUA_THEME_RegisterLib();
	LUA_VECTOR_RegisterLib();







	AddFunc("ARGS_Count",ARGS_Count);
	AddFunc("ARGS_Key",ARGS_Key);
	AddFunc("ARGS_Val",ARGS_Val);



	AddFunc("CONFIG_Load",CONFIG_Load);
	AddFunc("CONFIG_Open",CONFIG_Open);
	AddFunc("CONFIG_Write",CONFIG_Write);
	AddFunc("CONFIG_Close",CONFIG_Close);


// AddFunc("DIR_GetFiles",DIR_GetFiles);
// AddFunc("DIR_GetFile",DIR_GetFile);
	AddFunc("DIR_GetProfileDir",DIR_GetProfileDir);
	AddFunc("DIR_GetDataDir",DIR_GetDataDir);
// AddFunc("DIR_FileExists",DIR_FileExists);


	AddFunc("GLOBAL_GetTime",GLOBAL_GetTime);
	AddFunc("GLOBAL_GetElapsed",GLOBAL_GetElapsed);
	AddFunc("GLOBAL_GetScale",GLOBAL_GetScale);
	AddFunc("GLOBAL_GetFPS",GLOBAL_GetFPS);
	AddFunc("GLOBAL_GetVar",GLOBAL_GetVar);//Those vars should be used for interaction of the game/menu
	AddFunc("GLOBAL_SetVar",GLOBAL_SetVar);
	AddFunc("GLOBAL_VarDefined",GLOBAL_VarDefined);
	AddFunc("GLOBAL_Quit",GLOBAL_Quit);
	AddFunc("GLOBAL_SetMaxElapsed",GLOBAL_SetMaxElapsed);
	AddFunc("GLOBAL_SetMinFrames",GLOBAL_SetMinFrames);
	AddFunc("GLOBAL_SetMaxFrames",GLOBAL_SetMaxFrames);
	AddFunc("GLOBAL_StartDeveloperMode",GLOBAL_StartDeveloperMode);

	AddFunc("SCORE_GetVar",SCORE_GetVar);
	AddFunc("SCORE_SetVar",SCORE_SetVar);
	AddFunc("SCORE_VarDefined",SCORE_VarDefined);
	AddFunc("SCORE_Load",SCORE_Load);
	AddFunc("SCORE_Open",SCORE_Open);
	AddFunc("SCORE_Write",SCORE_Write);
	AddFunc("SCORE_Close",SCORE_Close);


	AddFunc("SAVE_Load",SAVE_Load);
	AddFunc("SAVE_Open",SAVE_Open);
	AddFunc("SAVE_Write",SAVE_Write);
	AddFunc("SAVE_Close",SAVE_Close);

//AddFunc("MOD_Clear",MOD_Clear);
	AddFunc("MOD_SetName",MOD_SetName);
	AddFunc("MOD_GetName",MOD_GetName);
//AddFunc("MOD_ClearBlacklist",MOD_ClearBlacklist);
//AddFunc("MOD_AddDirToBlacklist",MOD_AddDirToBlackList);




	AddFunc("USING",USING);
	AddFunc("INCLUDE",INCLUDE);
	AddFunc("INCLUDEABSOLUTE",INCLUDEABSOLUTE);




	AddFunc("TRANS_Add",TRANS_Add);
	AddFunc("TRANS_Clear",TRANS_Clear);
	AddFunc("TRANS_Str",TRANS_Str);
	AddFunc("TRANS_StrD",TRANS_StrD);
	AddFunc("TRANS_Load",TRANS_Load);

	AddFunc("DEBUG",DEBUG);

	AddFunc("LUA_ExecInState",LUA_ExecInState);

	}



int LuaCuboLib::DEBUG(lua_State *state) {
	GLfloat v[4];
	glGetMaterialfv(GL_FRONT,GL_AMBIENT,v);
	std::cout << "Ambient : " << v[0] << "  " << v[1] << "  "<< v[2] << "  " << v[3] << std::endl;
	return 0;
	}


int LuaCuboLib::GLOBAL_StartDeveloperMode(lua_State *state) {

	std::string consolekey=LUA_GET_STRING(state);
	SDLKey k=g_Game()->GetKeyboard()->GetKeyConstFor(consolekey);
	CuboConsole::GetInstance()->SetToggleKey(k);
	return 0;
	}



int LuaCuboLib::GLOBAL_GetTime(lua_State *state) {
	double t=g_Game()->GetTime();
	LUA_SET_NUMBER(state, t);
	return 1;
	}

int LuaCuboLib::GLOBAL_GetElapsed(lua_State *state) {
	double t=g_Game()->GetElapsed();
	LUA_SET_NUMBER(state, t);
	return 1;
	}

int LuaCuboLib::GLOBAL_SetMaxElapsed(lua_State *state) {
	double t=LUA_GET_DOUBLE(state);
	g_Game()->SetMaxPhysElapsed(t);
	return 0;
	}

int LuaCuboLib::GLOBAL_SetMinFrames(lua_State *state) {
	double t=LUA_GET_DOUBLE(state);
	g_Game()->SetMinFrames(t);
	return 0;
	}

int LuaCuboLib::GLOBAL_SetMaxFrames(lua_State *state) {
	double t=LUA_GET_DOUBLE(state);
	g_Game()->SetMaxFrames(t);
	return 0;
	}

int LuaCuboLib::GLOBAL_VarDefined(lua_State *state) {
	std::string s=LUA_GET_STRING(state);
	int t=g_Vars()->VarDefined(s);
	LUA_SET_NUMBER(state, t);
	return 1;
	}

int LuaCuboLib::GLOBAL_GetFPS(lua_State *state) {
	int t=g_Game()->GetFPS();
	LUA_SET_NUMBER(state, t);
	return 1;
	}


int LuaCuboLib::GLOBAL_GetScale(lua_State *state) {
	double t=CUBO_SCALE;
	LUA_SET_NUMBER(state, t);
	return 1;
	}

int LuaCuboLib::GLOBAL_GetVar(lua_State *state) {
	g_Vars()->GetVar(state);
	return 1;
	}

int LuaCuboLib::GLOBAL_Quit(lua_State *state) {
	g_Game()->Quit();
	return 0;
	}

int LuaCuboLib::GLOBAL_SetVar(lua_State *state) {

	g_Vars()->StoreVar(state);

	return 0;
	}



int LuaCuboLib::SCORE_VarDefined(lua_State *state) {
	std::string s=LUA_GET_STRING(state);
	int t=g_HighScores()->VarDefined(s);
	LUA_SET_NUMBER(state, t);
	return 1;
	}

int LuaCuboLib::SCORE_GetVar(lua_State *state) {
	g_HighScores()->GetVar(state);
	return 1;
	}

int LuaCuboLib::SCORE_SetVar(lua_State *state) {

	g_HighScores()->StoreVar(state);

	return 0;
	}


int LuaCuboLib::SCORE_Load(lua_State *state) {
	std::string s=LUA_GET_STRING(state);
	CuboFile *cf=g_BaseFileSystem()->GetFileForReading(s);
	if (!cf) { LUA_SET_NUMBER(state, 0); return 1 ; }
	if (!cf->IsHDDFile()) { LUA_SET_NUMBER(state, 0); return 1 ; }
	FILE *f=fopen(cf->GetHDDName().c_str(),"rt");
	if (f==NULL) {
			LUA_SET_NUMBER(state, 0);
			delete cf;
			return 1;
			}
	fclose(f);
	luaL_dofile(state,cf->GetHDDName().c_str());
	LUA_SET_NUMBER(state, 1);
	delete cf;
	return 1;
	}

int LuaCuboLib::SAVE_Open(lua_State *state) {
	std::string s=LUA_GET_STRING(state);
	cls_FileWriteable *fw=g_BaseFileSystem()->GetFileForWriting(s,true);
	if (!fw) { LUA_SET_NUMBER(state, 0); return 1;}
	FILE* f=fopen(fw->GetHDDName().c_str(),"wt");
	unsigned long int i=(unsigned long int)f;
	delete fw;
	LUA_SET_NUMBER(state, i);
	return 1;
	}

int LuaCuboLib::SAVE_Write(lua_State *state) {
	std::string k=LUA_GET_STRING(state);
	unsigned long int c=LUA_GET_ULINT(state);
	FILE *f=(FILE *)c;
	//string s="SCORE_SetVar(\""+k+"\","+g_HighScores()->GetVarString(k)+");\n";
	fprintf(f,"%s",k.c_str());
	return 0;
	}

int LuaCuboLib::SAVE_Close(lua_State *state) {
	unsigned long int c=LUA_GET_ULINT(state);
	FILE *f=(FILE *)c;
	fclose(f);
	return 0;
	}

int LuaCuboLib::SAVE_Load(lua_State *state) {
	std::string s=LUA_GET_STRING(state);

	CuboFile *finfo=GetFileName(s,FILE_SAVEGAME,".sdef");
	if (!finfo) {
			LUA_SET_NUMBER(state, 0);
			return 1;
			}
	if (!finfo->IsHDDFile()) {
			LUA_SET_NUMBER(state, 0);
			return 1;
			}
	s=finfo->GetHDDName();
	delete finfo;

	FILE *f=fopen(s.c_str(),"rt");
	if (f==NULL) {
			LUA_SET_NUMBER(state, 0);
			return 1;
			}
	fclose(f);
	luaL_dofile(state,s.c_str());
	LUA_SET_NUMBER(state, 1);
	return 1;
	}

int LuaCuboLib::SCORE_Open(lua_State *state) {
	std::string s=LUA_GET_STRING(state);
	cls_FileWriteable *fw=g_BaseFileSystem()->GetFileForWriting(s,true);
	if (!fw) { LUA_SET_NUMBER(state, 0); return 1;}
	FILE* f=fopen(fw->GetHDDName().c_str(),"wt");
	unsigned long int i=(unsigned long int)f;
	delete fw;
	LUA_SET_NUMBER(state, i);
	return 1;
	}

int LuaCuboLib::SCORE_Write(lua_State *state) {
	std::string k=LUA_GET_STRING(state);
	unsigned long int c=LUA_GET_ULINT(state);
	FILE *f=(FILE *)c;
	std::string s="SCORE_SetVar(\""+k+"\","+g_HighScores()->GetVarString(k)+");\n";
	fprintf(f,"%s",s.c_str());
	return 0;
	}

int LuaCuboLib::SCORE_Close(lua_State *state) {
	unsigned long int c=LUA_GET_ULINT(state);
	FILE *f=(FILE *)c;
	fclose(f);
	return 0;
	}

/*
int TLuaCuboLib::MOD_ClearBlacklist(lua_State *state)
{
///TODO ClearModDirBlacklist();
 return 0;
}
*/
int LuaCuboLib::MOD_SetName(lua_State *state) {
	std::string nm=LUA_GET_STRING(state);
	SetCurrentMod(nm);
	return 0;
	}

/*
int TLuaCuboLib::MOD_AddDirToBlackList(lua_State *state)
{
///TODO string nm=LUA_GET_STRING(state);
///TODO AddToDirBlacklist(nm);
///TODO return 0;
}
*/

int LuaCuboLib::MOD_GetName(lua_State *state) {
	std::string nm=CurrentMod();
	lua_pushstring(state,nm.c_str());
	return 1;
	}


float getfloatfield (lua_State *L, const char *key) {
	//double dresult;
	float result;
	lua_pushstring(L, key);
	lua_gettable(L, -2);  /* get background[key] */
	// if (!lua_isnumber(L, -1))
	//  error(L, "invalid component in background color");
	result = (float)lua_tonumber(L, -1);
	lua_pop(L, 1);  /* remove number */
	//result=1.0*dresult;
	return result;
	}






Vector3d Vector3FromStack(lua_State *state) {
	Vector3d v;
//stackDump(state); return 0;
	float vs[3]= {0,0,0};
	vs[0]=getfloatfield(state,"x");
	vs[1]=getfloatfield(state,"y");
	vs[2]=getfloatfield(state,"z");
	v.xyz( vs[0],vs[1],vs[2]);
//v=getvector3(state);
//
	lua_pop(state, 1);
	return v;
	}

Vector4d Vector4FromStack(lua_State *state) {
	Vector4d v;
//stackDump(state); return 0;
	float vs[4]= {0,0,0,0};
	vs[0]=getfloatfield(state,"r");
	vs[1]=getfloatfield(state,"g");
	vs[2]=getfloatfield(state,"b");
	vs[3]=getfloatfield(state,"a");
	v.xyzw( vs[0],vs[1],vs[2],vs[3]);
//v=getvector3(state);
//
	lua_pop(state, 1);
	return v;
	}





int LuaCuboLib::USING(lua_State *state) {
	std::string s=LUA_GET_STRING(state);

	if (s=="GL") { g_GLLib()->RegisterToState(state); }
	else if (s=="PATH") {
			g_PathFindingLib()->RegisterToState(state);
			}
	else if (s=="PARTICLE") { g_ParticleLib()->RegisterToState(state); }
	else if (s=="FILESYS") { g_FileSysLib()->RegisterToState(state); }
	return 0;
	}


int LuaCuboLib::INCLUDEABSOLUTE(lua_State *state) {
	std::string s=LUA_GET_STRING(state);
	CuboFile *finfo=g_BaseFileSystem()->GetFileForReading(s);
	if (!finfo) {coutlog("Cannot include script "+s+ ", since it was not found!",1); return 0;}
	if (g_VerboseMode()) {
			lua_getglobal(state,"LUA_DEF_NAME");
			std::string f=LUA_GET_STRING(state);

			coutlog("  Including : "+finfo->GetNameForLog()+ " (into file: "+f+")");
			}
	lua_doCuboFile(state,finfo);
	delete finfo;
	return 0;
	}


int LuaCuboLib::INCLUDE(lua_State *state) {
	std::string s=LUA_GET_STRING(state);
	lua_getglobal(state,"LUA_DEF_DIRNAME");
	std::string dn=LUA_GET_STRING(state);
	lua_getglobal(state,"LUA_DEF_TYPE");
	int typ=LUA_GET_INT(state);

///TODO: this is a Hack for backwards compatibilty, since INCLUDE in the level-header doesn't work without it for Beta 0.2 or below
	if (typ==FILE_USERLEVEL || typ==FILE_LEVEL) { dn="/levels/"; }
///TODO: Remove in Beta 0.3

	CuboFile *finfo=g_BaseFileSystem()->GetFileForReading(dn+s);
	if (!finfo) {coutlog("Cannot include script "+dn+s+ ", since it was not found!",1); return 0;}
	if (g_VerboseMode()) {
			lua_getglobal(state,"LUA_DEF_NAME");
			std::string f=LUA_GET_STRING(state);

			coutlog("  Including : "+finfo->GetNameForLog()+ " (into file: "+f+")");
			}
	lua_doCuboFile(state,finfo);
	delete finfo;
	return 0;
	}

int LuaCuboLib::CONFIG_Load(lua_State *state) {
	std::string s=LUA_GET_STRING(state);
	CuboFile *cf=g_BaseFileSystem()->GetFileForReading(s);
	if (!cf) { LUA_SET_NUMBER(state, 0); return 1 ; }
	if (!cf->IsHDDFile()) { LUA_SET_NUMBER(state, 0); return 1 ; }
	FILE *f=fopen(cf->GetHDDName().c_str(),"rt");
	if (f==NULL) {
			LUA_SET_NUMBER(state, 0);
			delete cf;
			return 1;
			}
	fclose(f);
	luaL_dofile(state,cf->GetHDDName().c_str());
	LUA_SET_NUMBER(state, 1);
	delete cf;
	return 1;
	}

int LuaCuboLib::CONFIG_Open(lua_State *state) {
	std::string s=LUA_GET_STRING(state);
	/* string dir=g_ProfileDir();
	 s=dir+s;
	 s=PlattformFilename(s);


	 char dirs[2048] ;
	 sprintf(dirs,"%s",s.c_str());
	 string thedir=dirname(dirs);
	// coutlog("Trying to create the dir: "+thedir);
	 RecursiveMKDir(thedir);
	*/
	cls_FileWriteable *fw=g_BaseFileSystem()->GetFileForWriting(s,true);
	if (!fw) { LUA_SET_NUMBER(state, 0); return 1;}


	FILE* f=fopen(fw->GetHDDName().c_str(),"wt");
	unsigned long int i=(unsigned long int)f;
	delete fw;
	LUA_SET_NUMBER(state, i);
	return 1;
	}

int LuaCuboLib::CONFIG_Write(lua_State *state) {
	std::string k=LUA_GET_STRING(state);
	unsigned long int c=LUA_GET_ULINT(state);
	FILE *f=(FILE *)c;
	std::string s="GLOBAL_SetVar(\""+k+"\","+g_Vars()->GetVarString(k)+");\n";
	fprintf(f,"%s",s.c_str());
	return 0;
	}

int LuaCuboLib::CONFIG_Close(lua_State *state) {
	unsigned long int c=LUA_GET_ULINT(state);
	FILE *f=(FILE *)c;
	fclose(f);
	return 0;
	}



std::vector<std::string> lsfiles;
/*
int TLuaCuboLib::DIR_GetFiles(lua_State *state)
{
///TODO  string s=LUA_GET_STRING(state);
///TODO  lsfiles=LSCuboSubDir(s);
///TODO  int r=lsfiles.size();
///TODO  LUA_SET_NUMBER(state, r);
///TODO  sort(lsfiles.begin(), lsfiles.end());
///TODO  return 1;
}
*/


int LuaCuboLib::DIR_GetDataDir(lua_State *state) {
	LUA_SET_STRING(state, g_DataDir());
	return 1;
	}


int LuaCuboLib::DIR_GetProfileDir(lua_State *state) {
	LUA_SET_STRING(state, g_ProfileDir());
	return 1;
	}
/*
int TLuaCuboLib::DIR_GetFile(lua_State *state)
{
///TODO int i=LUA_GET_INT(state);
///TODO LUA_SET_STRING(state, lsfiles[i]);
///TODO return 1;
}
*/







LuaBaseVar **LuaVarHolder::Ref(std::string name) {
	for (unsigned int i=0; i<vars.size(); i++)
		if (vars[i]) if (vars[i]->GetName()==name) { return &(vars[i]); }
	return NULL;
	}

LuaBaseVar **LuaVarHolder::RefForStore(std::string name) {
	LuaBaseVar **ref;
	ref=Ref(name);
	if (!ref) {
			//New Var:
			vars.push_back(NULL);
			return &(vars[vars.size()-1]);
			}
	else {
			delete (*ref);
			return ref;
			}
	}

LuaBaseVar **LuaVarHolder::RefForRead(std::string name) {
	LuaBaseVar **ref=Ref(name);
	if (!ref) {
			//NOT GOOD AT ALL
			return NULL;
			}
	else {
			return ref;
			}
	}


void LuaVarHolder::SetVar(std::string vname,std::string value) {
	LuaBaseVar **ref=RefForStore(vname);
	(*ref)=new LuaStringVar(value);
	(*ref)->SetName(vname);
	}

void LuaVarHolder::SetVar(std::string vname,double value) {
	LuaBaseVar **ref=RefForStore(vname);
	(*ref)=new LuaNumberVar(value);
	(*ref)->SetName(vname);
	}


void LuaVarHolder::ListToConsole() {
	for (unsigned int i=0; i<vars.size(); i++) {
			std::string s=vars[i]->GetName()+"   =   "+vars[i]->GetVarString();
			coutlog(s,4);
			}
	}

void LuaVarHolder::StoreVar(lua_State *fromstate) {
	std::string varname=lua_tostring(fromstate,1);
	lua_remove(fromstate,1);
	int t=lua_type(fromstate,1);
	LuaBaseVar **ref=RefForStore(varname);
	switch (t) {
			case  LUA_TNUMBER:
				(*ref)=new LuaNumberVar();
				break;
			case LUA_TSTRING:
				(*ref)=new LuaStringVar();
				break;
			case LUA_TBOOLEAN:

				break;
			case LUA_TTABLE:
				//Hard work here
				break;
			}

//stackDump(fromstate);
	(*ref)->SetName(varname);

	(*ref)->ReadFromState(fromstate);
//stackDump(fromstate);
	}

int LuaVarHolder::VarDefined(std::string vn) {
	if (!Ref(vn)) { return 0; }
	else { return 1; }
	}

void LuaVarHolder::GetVar(lua_State *state) {

	std::string varname=lua_tostring(state,1);
	lua_remove(state,1);

	LuaBaseVar **ref=RefForRead(varname);
	if (!ref) {
			lua_getglobal(state,"LUA_DEF_NAME");
			std::string f=LUA_GET_STRING(state);

			std::ostringstream os; os << "WARNING in " << f << ": Can't get member variable " << varname; coutlog(os.str(),2); return;
			}
	(*ref)->WriteInState(state);
	}

std::string LuaVarHolder::GetVarString(std::string varname,int forscript) {
	LuaBaseVar **ref=RefForRead(varname);
	if (!ref)
			{     std::ostringstream os; os << "WARNING : Can't get member variable " << varname; coutlog(os.str(),2); return "";}
	return (*ref)->GetVarString(forscript);
	}


int LuaCuboLib::ARGS_Count(lua_State *state) {
	LUA_SET_NUMBER(state, g_CmdLineCount());
	return 1;
	}

int LuaCuboLib::ARGS_Key(lua_State *state) {
	int i=LUA_GET_INT(state);
	LUA_SET_STRING(state, g_CmdLineKey(i));
	return 1;
	}

int LuaCuboLib::ARGS_Val(lua_State *state) {
	int i=LUA_GET_INT(state);
	LUA_SET_STRING(state, g_CmdLineVal(i));
	return 1;
	}








int LuaCuboLib::TRANS_Clear(lua_State *state) {
	g_Translation()->clear();
	return 0;
	}

int LuaCuboLib::TRANS_Add(lua_State *state) {
	g_Translation()->StoreVar(state);
	return 0;
	}

int LuaCuboLib::TRANS_Str(lua_State *state) {
	std::string varname=lua_tostring(state,1);
	if (g_Translation()->VarDefined(varname)) {
			g_Translation()->GetVar(state);
			return 1;
			}
	else {
			return 1;
			}
	}

int LuaCuboLib::TRANS_StrD(lua_State *state) {
	std::string def=LUA_GET_STRING(state);
	std::string varname=LUA_GET_STRING(state);
	if (g_Translation()->VarDefined(varname)) {
			LUA_SET_STRING(state, varname);
			g_Translation()->GetVar(state);
			return 1;
			}
	else {
			LUA_SET_STRING(state, def);
			return 1;
			}
	}

std::string g_languagefile="";




void ReloadLanguage() {
	LuaAccess lua;
	//coutlog("Reloading lang: "+g_languagefile,2);
	CuboFile *finfo=g_BaseFileSystem()->GetFileForReading(g_languagefile);
	if (!finfo) {coutlog("Cannot reload translation "+g_languagefile+ ", since it was not found!",1); }
	lua.Include(g_CuboLib());
	lua.LoadFile(finfo,FILE_LANGDEF,-1);
	delete finfo;
	}


int LuaCuboLib::TRANS_Load(lua_State *state) {
	std::string s=LUA_GET_STRING(state);
	LuaAccess lua;
	CuboFile *finfo=GetFileName(s,FILE_LANGDEF,".ldef");
	if (!finfo) {coutlog("Cannot load translation "+s+ ", since it was not found!",1); return 0;}
	lua.Include(g_CuboLib());
	lua.LoadFile(finfo,FILE_LANGDEF,-1);
	g_languagefile=finfo->GetName();
	delete finfo;
	return 0;
	}

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
