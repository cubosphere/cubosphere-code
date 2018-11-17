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

#include <string>
#include "globals.hpp"
#include "themes.hpp"
#include "cuboutils.hpp"

#include <dirent.h>

#include <sys/stat.h>

cls_FileSystem gBaseFileSystem;

cls_FileSystem * g_BaseFileSystem() {return &gBaseFileSystem;}

std::string activemod="";
//vector<string> modstack; //Activated Mods
//vector<string> moddirblacklist;

//Some patches from Vincent Petry - Thanks !

std::string data_dir="";
std::string profile_dir="";

int verbosemode=0;

int g_VerboseMode()
	{
	return verbosemode;
	}

void g_VerboseMode(int i)
	{
	verbosemode=i;
	}


std::string g_DataDir() {
	return data_dir;
	}


std::string g_ProfileDir() {
	return profile_dir;
	}


int FileTypeFromString(std::string which,std::string *exte)
	{
	int d=-1;
	std::string ext="";
	if (which=="blockdefs")    { d=FILE_BLOCKDEF; ext="bdef";}
	else if (which=="itemdefs")    { d=FILE_ITEMDEF; ext="idef";}
	else if (which=="userlevels")  { d=FILE_USERLEVEL; ext="ldef";}
	else if (which=="themes")      { d=FILE_THEMEDEF; ext="themedef";}
	else if (which=="actordefs")   { d=FILE_ACTORDEF; ext="adef";}
	else if (which=="language")    { d=FILE_LANGDEF; ext="ldef";}
	else if (which=="music")       { d=FILE_MUSIC; ext="mp3";}
	else if (which=="savegame")    { d=FILE_SAVEGAME; ext="sdef";}
	else if (which=="enemydefs")   { d=FILE_ENEMYDEF; ext="edef";}
	else if (which=="menudefs")   { d=FILE_MENUDEF; ext="mdef";}
	else if (which=="levels")   { d=FILE_LEVEL; ext="ldef";}
	else if (which=="editor")   { d=FILE_EDITOR; ext="";}
	else if (which=="particles")   { d=FILE_PARTICLEDEF; ext="pdef";}
	else if (which=="texdefs")   { d=FILE_TEXTUREDEF; ext="tdef";}
	else if (which=="mdldefs")   { d=FILE_MDLDEF; ext="mdef";}
	else if (which=="posteffects")   { d=FILE_POSTEFFECTDEF; ext="pdef";}
	else if (which=="skyboxes")   { d=FILE_SKYBOX; ext="sdef";}

	if (exte) {(*exte)=ext;}
	if (d<0) { coutlog("Warning: <FileTypeFromString> does not know the type "+which,2); }
	return d;
	}

/*
void ClearModDirBlacklist()
{
 moddirblacklist.clear();
}

void AddToDirBlacklist(string dn)
{
 moddirblacklist.push_back(dn);
}

void ClearModStack()
{
 modstack.clear();
}
*/

void SetCurrentMod(std::string s)
	{
	activemod=s;
	}

std::string CurrentMod()
	{
	return activemod;
// if (modstack.size()==0) return "";
//else return modstack[0];
	}

void SetProfileDir(std::string dir)
	{
	profile_dir=dir;
	}

void SetDataDir(std::string dir)
	{
	data_dir=dir;
	}


#ifdef VCPP

#include <direct.h>
int mkdir(string d) {return _mkdir(d.c_str());}
string dirname(string f) {return f.substr(0, f.find_last_of("/\\"));}
#endif

void RecursiveMKDir(std::string dir,std::string dirsep,int mode)
	{
	std::vector<std::string> ds;
	TokenizeFull(dir,ds,dirsep);

	int error=1;
	int dirindex=ds.size();
	std::string testname;
	while ((error) && (dirindex>=0))
			{
			testname=ds[0];
			for (int j=1; j<dirindex; j++) { testname=testname+dirsep+ds[j]; }
#ifdef WIN32
			error=mkdir(testname.c_str());
#else
			error=mkdir(testname.c_str(),mode);
#endif
			if (!error) { break; }
			dirindex--;
			}
	if (error) { return; } //FAIL
//Create the missing dirs successively
	for (unsigned int i=dirindex; i<ds.size(); i++)
			{
			testname=testname+dirsep+ds[i];
#ifdef WIN32
			error=mkdir(testname.c_str());
#else
			error=mkdir(testname.c_str(),mode);
#endif

			if (error) { return; } //FAIL
			}
	}



std::string PlattformFilename(std::string in)
	{
#ifdef WIN32
	for (unsigned int i=0; i<in.size(); i++) if (in[i]=='/') { in[i]='\\'; }
#endif
	return in;
	}


std::string GetDirName(int type)
	{
	if ((type!=FILE_USERLEVEL) && (type!=FILE_SAVEGAME))
			{
			return g_SubDirs[type];
			}
	return "";//To be handled
	}

std::string GetDirNameForMod(int type,std::string modstr="/")
	{
	if ((type!=FILE_USERLEVEL) && (type!=FILE_SAVEGAME))
			{
			return PlattformFilename(g_DataDir()+modstr+g_SubDirs[type]);
			}
	else
			{
			return PlattformFilename(g_ProfileDir()+modstr+g_Vars()->GetVarString("ActiveProfile",0)+"/"+g_SubDirs[type]);
			}
	}

std::string GetFileNameForMod(std::string subname,int type,std::string ext,std::string modstr="/")
	{
	std::string res;
	if ((type!=FILE_USERLEVEL) && (type!=FILE_SAVEGAME))
			{
			if (subname=="") { res=PlattformFilename(g_DataDir()+modstr+g_SubDirs[type]); }
			else {
					if (g_SubDirUsesTheme[type]) {
							res=Theme::FileName(g_DataDir()+modstr+g_SubDirs[type],subname,ext);
							}
					else {
							res=PlattformFilename(g_DataDir()+modstr+g_SubDirs[type]+"/"+subname+ext);
							}
					}
			}
	else
			{
			if (subname=="") { res=PlattformFilename(g_ProfileDir()+modstr+g_Vars()->GetVarString("ActiveProfile",0)+"/"+g_SubDirs[type]); }
			else { res=PlattformFilename(g_ProfileDir()+modstr+g_Vars()->GetVarString("ActiveProfile",0)+"/"+g_SubDirs[type]+"/"+subname+ext); }
			}


	return res;
	}


CuboFile* GetCuboFileFromRelativeName(std::string relname)
	{
	///TODO: Mod-Stuff
	return gBaseFileSystem.GetFileForReading(relname);
	}

CuboFile* GetFileName(std::string subname,int type,std::string ext)
	{
//   coutlog("GetFileName is not implemented yet!"); return NULL;
	if ((type!=FILE_USERLEVEL) && (type!=FILE_SAVEGAME))
			{
			///TODO: Mod-Zeugs, Dir Getting
			std::string res;
			if (g_SubDirUsesTheme[type]) {
					res=Theme::FileName(g_SubDirs[type],subname,ext);
					}
			else {
					res=(g_SubDirs[type]+"/"+subname+ext);
					}
			return gBaseFileSystem.GetFileForReading(res);
			}
	else {
			std::string res="";
			if (type==FILE_USERLEVEL) { res="/user/levels/"+subname+".ldef"; }
			else if (type==FILE_SAVEGAME) { res="/user/saves/"+subname+".sdef"; }
			return gBaseFileSystem.GetFileForReading(res);
			};

	/*
	 for (unsigned int i=0;i<modstack.size();i++)
	 {
	   string temp=GetFileNameForMod(subname,type,ext,"/mods/"+modstack[i]+"/");
	   ifstream f;
	   f.open(temp.c_str());
	   if (f.good()) {  return temp; }
	 }
	 string temp=GetFileNameForMod(subname,type,ext);
	 ifstream f;
	   f.open(temp.c_str());
	   if (!(f.good())) { ostringstream os; os << "WARNING: file " << temp << " not found"; coutlog(os.str()); }

	 return temp;
	 */
	}


using TCommandLineEntry = struct
	{
	std::string spec;
	std::string val;
	};

std::vector<TCommandLineEntry> cmdlines;

void SetCmdLine(int argc,char *argv[])
	{
	int inswitch=0;
	int allset=0;
	TCommandLineEntry cmd;
	for (int i=0; i<argc; i++)
			{
			std::string current=argv[i];
			if (!inswitch)
					{
					if (current[0]=='-')
							{
							cmd.val="";
							cmd.spec.assign(current,1,current.length()-1);
							inswitch=1;
							allset=0;
							}
					else {
							cmd.spec="";
							cmd.val=current;
							cmdlines.push_back(cmd);
							allset=1;
							}
					}
			else
					{
					if (current[0]=='-')
							{
							cmd.val="";
							cmdlines.push_back(cmd);
							//cmd.val="";
							cmd.spec.assign(current,1,current.length()-1);
							allset=0;
							}
					else {
							cmd.val=current;
							inswitch=0;
							cmdlines.push_back(cmd);
							//cmdlines.push_back(cmd);
							allset=1;
							}
					}
			}
	if (!allset) { cmdlines.push_back(cmd); }
	}

/*
vector <string> LSSingleDir(string which, string modstr)
{
    string d,ext;

 int ty=FileTypeFromString(which,&ext);
 d=GetDirNameForMod(ty,modstr);
 //d=GetDirName(ty);

 vector <string> lsfiles;

 //coutlog("LS in "+d);

//#ifndef VCPP

 DIR *hdir;
    struct dirent *entry;

    hdir = opendir(d.c_str());
    if (!hdir) return lsfiles; //Non Exists

    do
    {
        entry = readdir(hdir);
        if (entry)
        {

            string e=entry->d_name;

            //Check if the extension fits
            vector<string> tok;
            Tokenize(e,tok,".");
            if (!tok.size()) continue;
            if (tok[tok.size()-1]==ext)
            {
             //Build the result without extension
              e="";
              for (unsigned int i=0;i<tok.size()-1;i++) e=e+tok[i];
              if (e!="_selection")
              lsfiles.push_back(e);
            }
        }
    } while (entry);
    closedir(hdir);
//#else
 //TODO: Add stuff
// lsfiles.push_back("NOT IMPLEMENTED!");
//#endif
 return lsfiles;
}

void MergeTwoLS(vector <string> & current, vector <string> & res)
{
for (unsigned int c=0;c<current.size();c++)
   {
    int found=0;
    for (unsigned int r=0;r<res.size();r++)
    {
     if (res[r]==current[c]) { found = 1; break;}
    }
    if (found) continue;
    res.push_back(current[c]);
   }
}

vector <string> LSCuboSubDir(string which)
{
 vector<string> res;
 int blacklisted=0;

 string d,ext;
 int ty;
 ty=FileTypeFromString(which,&ext);

 for (unsigned int b=0;b<moddirblacklist.size();b++) if (which==moddirblacklist[b]) blacklisted=1; //Do not allow to access on the old (non-mod) subdir

 if (ty==FILE_USERLEVEL || ty==FILE_SAVEGAME)
 {
 for (unsigned int i=0;i<modstack.size();i++)
 {
  if ((!blacklisted) || (i==0))
  {
   vector<string> current=LSSingleDir(which,"/mods/"+modstack[i]+"/");
   MergeTwoLS(current,res);
  }
 }

 if ((!blacklisted) || (modstack.size()==0))
 {
   vector<string> current=LSSingleDir(which,"/");
   MergeTwoLS(current,res);
 }

  return res;
 }

// d=GetDirNameForMod(ty,"/");
 d=GetDirName(ty);



//cout << "LS for " << which << " os " << blacklisted << endl;

 for (unsigned int i=0;i<modstack.size();i++)
 {
  if ((!blacklisted) || (i==0))
  {
   vector<string> current=LSSingleDir(which,"/mods/"+modstack[i]+"/");
   MergeTwoLS(current,res);
  }
 }

 if ((!blacklisted) || (modstack.size()==0))
 {
   vector<string> current;
  // cerr << "LS " << d << endl;
   gBaseFileSystem.ListDirectoryEntries(d,current);
   MergeTwoLS(current,res);
 }

vector<string>::iterator iter = res.begin();
while ( iter != res.end() )
{
 if ((*iter)=="_selection."+ext || (iter->rfind(ext)!=iter->length()-ext.length()) )
 {
   //  cerr << "deleting " << (*iter) << " from list " << (iter->rfind(ext)) << " vs "  << (iter->length()-ext.length()) << endl;
   iter = res.erase( iter);
 }
 else {
   //Remove extension
   if (ext!="") (*iter)=iter->substr(0,iter->length()-ext.length()-1);
   iter++;
 }
}

 return res;
}


//Remember: File is seeked at first in modstack[0], modstack[1] ... and at last in the base cubo dir
int AddModToStack(string nm)
{
   string temp=GetFileNameForMod("init",FILE_MENUDEF,".mdef","/mods/"+nm+"/");
   ifstream f;
   f.open(temp.c_str());
   if (f.good()) { f.close(); modstack.push_back(nm); return 1;}
   return 0;

}
*/

int g_CmdLineCount()
	{return cmdlines.size();}

std::string g_CmdLineKey(int i)
	{return cmdlines[i].spec;}
std::string g_CmdLineVal(int i)
	{return cmdlines[i].val;}


////LUA STUFF/////////////////////////////////////////

class TLuaFileSysLib : public LuaCFunctions
	{
	protected:
		static int FILESYS_PlatformFilename(lua_State *state)
			{
			std::string dironhdd=LUA_GET_STRING(state);
			dironhdd=PlattformFilename(dironhdd);
			LUA_SET_STRING(dironhdd);
			return 1;
			}
		static int FILESYS_MountHDDDir(lua_State *state)
			{
			std::string mountbase=LUA_GET_STRING(state);
			std::string dironhdd=LUA_GET_STRING(state);
			dironhdd=(dironhdd);
			int res=gBaseFileSystem.MountHDDDir(dironhdd,mountbase);
			LUA_SET_INT(res);
			return 1;
			}

		static int FILESYS_MountWriteableHDDDir(lua_State *state)
			{
			std::string mountbase=LUA_GET_STRING(state);
			std::string dironhdd=LUA_GET_STRING(state);
			dironhdd=(dironhdd);
			int res=gBaseFileSystem.MountWriteableHDDDir(dironhdd,mountbase);
			LUA_SET_INT(res);
			return 1;
			}

		static int FILESYS_MountZip(lua_State *state)
			{
			std::string mountbase=LUA_GET_STRING(state);
			std::string zipf=LUA_GET_STRING(state);
			cls_FileReadable *zf=gBaseFileSystem.GetFileForReading(zipf);
			int res;
			if (!zf) { res=0; }
			else { res=gBaseFileSystem.MountZipFile(zf,mountbase); }
			if (gBaseFileSystem.GetLastError(0)!="") { coutlog(gBaseFileSystem.GetLastError(1),1); }
			LUA_SET_INT(res);
			return 1;
			}

		static int FILESYS_PopBottom(lua_State *state)
			{
			gBaseFileSystem.PopBottom();
			return 0;
			}

		static std::vector<std::string> entries;

		static int FILESYS_StartListDirectory(lua_State *state)
			{
			entries.clear();
			std::string mask=LUA_GET_STRING(state);
			bool fullpath=LUA_GET_BOOL(state);
			bool recu=LUA_GET_BOOL(state);
			bool lsdirs=LUA_GET_BOOL(state);
			bool lsfiles=LUA_GET_BOOL(state);
			std::string dir=LUA_GET_STRING(state);
			int typ=(fullpath ? CLS_FILE_LIST_FULLPATH : 0) | (recu ? CLS_FILE_LIST_RECURSIVE : 0) | (lsdirs ? CLS_FILE_LIST_DIRS : 0) | (lsfiles ? CLS_FILE_LIST_FILES : 0);
			gBaseFileSystem.ListDirectoryEntries(dir,entries,typ,mask);
			typ=entries.size();
			LUA_SET_INT(typ);
			return 1;
			}

		static int FILESYS_GetListDirectoryEntry(lua_State *state)
			{
			int index=LUA_GET_INT(state);
			std::string res;
			if (index<0 || index>=(int)entries.size()) { res=""; }
			else { res=entries[index]; }
			LUA_SET_STRING(res);
			return 1;
			}

		static int FILESYS_FileExists(lua_State *state)
			{
			std::string f=LUA_GET_STRING(state);
			CuboFile * cf=gBaseFileSystem.GetFileForReading(f);
			if (cf)
					{
					delete cf;
					LUA_SET_INT(1);
					}
			else { LUA_SET_INT(0); }
			return 1;
			}

		static int FILESYS_WillOverwrite(lua_State *state)
			{
			std::string f=LUA_GET_STRING(state);
			cls_FileWriteable * cf=gBaseFileSystem.GetFileForWriting(f,false);
			if (cf)
					{

					LUA_SET_INT(cf->WillOverwrite());
					delete cf;

					}
			else { LUA_SET_INT(0); }
			return 1;
			}

		static int FILESYS_Delete(lua_State *state)
			{
			std::string fname=LUA_GET_STRING(state);
			cls_FileWriteable * f=gBaseFileSystem.GetFileForWriting(fname,false);
			if (!f) {LUA_SET_INT(0); return 1;}
			LUA_SET_INT(f->Delete());
			delete f;
			return 1;
			}

		static int FILESYS_AddFileMask(lua_State *state)
			{
			bool recu=LUA_GET_BOOL(state);
			bool read=LUA_GET_BOOL(state);
			bool list=LUA_GET_BOOL(state);
			std::string fname=LUA_GET_STRING(state);
			int mode=(recu==true ? CLS_FILE_MASK_RECURSIVE : 0) |  (read==true ? CLS_FILE_MASK_READ : 0) | (list==true ? CLS_FILE_MASK_LIST : 0);
			gBaseFileSystem.SetFileMask(fname,mode,-1);
			return 0;
			}


	public:
		TLuaFileSysLib()
			{
			AddFunc("FILESYS_MountHDDDir",FILESYS_MountHDDDir);
			AddFunc("FILESYS_MountWriteableHDDDir",FILESYS_MountWriteableHDDDir);
			AddFunc("FILESYS_MountZip",FILESYS_MountZip);
			AddFunc("FILESYS_PopBottom",FILESYS_PopBottom);
			AddFunc("FILESYS_StartListDirectory",FILESYS_StartListDirectory);
			AddFunc("FILESYS_GetListDirectoryEntry",FILESYS_GetListDirectoryEntry);
			AddFunc("FILESYS_FileExists",FILESYS_FileExists);
			AddFunc("FILESYS_AddFileMask",FILESYS_AddFileMask);
			AddFunc("FILESYS_WillOverwrite",FILESYS_WillOverwrite);
			AddFunc("FILESYS_Delete",FILESYS_Delete);
			AddFunc("FILESYS_PlatformFilename",FILESYS_PlatformFilename);
			}
	};

std::vector<std::string> TLuaFileSysLib::entries;

static TLuaFileSysLib fslib;

LuaCFunctions* g_FileSysLib()
	{
	return &fslib;
	}




bool StartBootScript(std::string name)
	{
	// gBaseFileSystem.MountZipFile(dir+"/data.zip");
	gBaseFileSystem.MountHDDDir(g_DataDir(),"/");
	CuboFile * cf=gBaseFileSystem.GetFileForReading(name);
	if (!cf) { gBaseFileSystem.PopBottom(); return false;}
	LuaAccess acc;
	acc.Include(g_CuboLib());
	int res=acc.LoadFile(cf,-1,-1);
	delete cf;
	return res;
	}
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
