/**
Copyright (C) 2010 Chriddo

This program is free software;
you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program;
if not, see <http://www.gnu.org/licenses/>.
**/


#ifndef GLOBALS_H_G
#define GLOBALS_H_G

#include <iostream>
#include <string>
#include <vector>
#include "filesystem.hpp"
#include "luautils.hpp"

#ifdef WIN32
#define STD_DIR_SEP "\\"
#else
#define STD_DIR_SEP "/"
#endif

#ifdef VCPP
extern string dirname(string s);
extern int mkdir(string d);
#endif



extern void RecursiveMKDir(std::string dir,std::string dirsep=STD_DIR_SEP,int mode=0755);

extern int g_VerboseMode();
extern void g_VerboseMode(int i);

extern int g_CmdLineCount();
extern std::string g_CmdLineKey(int i);
extern std::string g_CmdLineVal(int i);

extern void SetCmdLine(int argc,char *argv[]);
extern std::string g_DataDir();
extern std::string g_ProfileDir();

extern void SetBaseDir(std::string bd);
extern void SetProfileDir(std::string dir);
extern void SetDataDir(std::string dir);

//Remember: File is seeked at first in modstack[0], modstack[1] ... and at last in the base cubo dir
//extern int AddModToStack(string nm);
//extern void ClearModStack();
extern std::string CurrentMod();
extern void SetCurrentMod(std::string s);

extern void ClearModDirBlacklist();
extern void AddToDirBlacklist(std::string dn);

extern std::vector <std::string> LSCuboSubDir(std::string which);

extern std::string PlattformFilename(std::string in);
//extern string GetFileName(string subname,int type,string ext);
extern TCuboFile* GetFileName(std::string subname,int type,std::string ext);
extern TCuboFile* GetCuboFileFromRelativeName(std::string relname);

extern int FileTypeFromString(std::string which,std::string *exte=NULL);




#define FILE_TEXTURE 0
#define FILE_TEXTUREDEF 1
#define FILE_BLOCKDEF 2
#define FILE_SKYBOX 3
#define FILE_SHADER 4
//#define FILE_SKYDEF 5
#define FILE_LEVEL 6
#define FILE_MDLDEF 7
#define FILE_MDL 8
#define FILE_ITEMDEF 9
#define FILE_ACTORDEF 10
#define FILE_MENUDEF 11
#define FILE_USERLEVEL 12
#define FILE_THEMEDEF 13
#define FILE_SOUND 14
#define FILE_MUSIC 15
#define FILE_LANGDEF 16
#define FILE_FONT 17
#define FILE_SAVEGAME 18
#define FILE_ENEMYDEF 19
#define FILE_EDITOR 20
#define FILE_POSTEFFECTDEF 21
#define FILE_PARTICLEDEF 22

static const std::string g_SubDirs[] = {"textures","texdefs","blockdefs","skyboxes","shaders",">>>skydefs<<<","levels","mdldefs","mdls","itemdefs","actordefs","menudefs","levels","themedefs","sounds","music","language","fonts","saves","enemydefs","editor","posteffects","particles"};

static const int g_SubDirUsesTheme[] = {1,1,1,1,1,1,0,1,1,1,1,0,0,0,1,1,0,0,0,1,0,1,1};


extern TLuaCFunctions* g_FileSysLib();

extern bool StartBootScript(std::string name="boot.lua");
extern cls_FileSystem * g_BaseFileSystem();

#endif
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
