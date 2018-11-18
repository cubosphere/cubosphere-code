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

#include "filesystem.hpp"
#include <fstream>
#include <cstdlib>
#include <dirent.h>
#include <algorithm>
#include <cstdio>
#include "unzip.hpp"
#include "globals.hpp"

#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>


#ifdef __unix__
constexpr auto CLS_PATH_SEPARATOR_SYS = "/";
#elif defined _WIN32
constexpr auto CLS_PATH_SEPARATOR_SYS = "\\";
#else
constexpr auto CLS_PATH_SEPARATOR_SYS = "/";
#warning "Can't get the system architecture... Don't know, which path separator should be taken"
#endif

//The Separator used in the clsEngine
constexpr auto CLS_PATH_SEPARATOR = "/";

constexpr auto CLS_PATH_SEPARATOR_ZIP = "/\\";

constexpr auto CLS_FILE_PATTERN_MATCH_FLAGS = 0;

constexpr auto CLS_FILE_MASK_SET = 1;



//CONFIG THIS FOR YOUR PROGRAM
constexpr auto CLS_FILE_ERROR_TYPE_ERROR = 0;
constexpr auto CLS_FILE_ERROR_TYPE_WARNING = 1;
constexpr auto CLS_FILE_ERROR_TYPE_CRITICAL = 2;

#define CLS_FILE_ERROR(s,t) clsFileErrorLog(s,t)


///Exemplary for Cubosphere
#include "luautils.hpp"
static void clsFileErrorLog(std::string msg, const int t) {
	if (t==CLS_FILE_ERROR_TYPE_ERROR) { msg="ERROR: "+msg; }
	else if (t==CLS_FILE_ERROR_TYPE_WARNING) { msg="WARNING: "+msg; }
	else if (t==CLS_FILE_ERROR_TYPE_CRITICAL) { msg="CRITICAL ERROR: "+msg; }
	coutlog(msg,(t==CLS_FILE_ERROR_TYPE_WARNING ? 2 : 1));
	}

///For Checking unused files
//#define LOG_USED_FILES

#ifdef LOG_USED_FILES
#include <fstream>
std::ofstream * logufiles;
#endif

/////////////////////////////////////////////
//            SOME HELPER FUNCS            //
/////////////////////////////////////////////

bool cls_DirectoryExists(const std::string d) {
	DIR  *dip;
	if ((dip = opendir(d.c_str())) == NULL) { return false; }
	else { closedir(dip); return true; }
	}

#define MATCH_FAIL 1
#define MATCH_ERROR 2
#define MATCH_SUCCESS 3

int globMatch(const char * pattern, const char * text) {
	while(*pattern && *text) {
			char p = *pattern++;
			switch(p) {
					case '*':
						while(*text) {
								int rc = globMatch(pattern, text++);
								if(rc != MATCH_FAIL) {
										return rc;
										}
								}
						return MATCH_FAIL;
					case '\\':
						if((p = *pattern++) == 0) {
								return MATCH_ERROR;
								}
						if(p != *text++) {
								return MATCH_FAIL;
								}
						break;
					case '?':
						text++;
						break;
					default:
						if(p != *text++) {
								return MATCH_FAIL;
								}
					}
			}
	return ((*pattern == 0) && (*text == 0))? MATCH_SUCCESS: MATCH_FAIL;
	}

bool winfnmatch(std::string pattern,std::string curr) {
	return (globMatch(pattern.c_str(),curr.c_str()) == MATCH_SUCCESS);
	/*std::vector<std::string> pelems;
	coutlog("Go with |"+pattern+"|  and "+curr);
	std::string::size_type lastPos =0;
	std::string::size_type pos     = pattern.find_first_of("*", lastPos);
	while (std::string::npos != pos || std::string::npos != lastPos)
	{
	         pelems.push_back(pattern.substr(lastPos, pos - lastPos));
	         lastPos = pos+1;
	         pos = pattern.find_first_of("*", lastPos);
	         std::ostringstream oss; oss<< "added |" << pelems.back() << "| to the pattern for  " << curr << std::endl;
	         coutlog(oss.str());
	}
	*/


	}

/////////////////////////////////////////////
//       FILE BASES BASE CLASS             //
/////////////////////////////////////////////

cls_FileBaseClass::cls_FileBaseClass(const std::string bnam,const std::string dnam) :  dirname(dnam), basename(bnam) {
	std::string ps=CLS_PATH_SEPARATOR; int psl=ps.length();
	if (dirname=="") { dirname=CLS_PATH_SEPARATOR; }
	else if (dirname.substr(dirname.length()-psl,psl)!=CLS_PATH_SEPARATOR) { dirname=dirname+CLS_PATH_SEPARATOR; }
	}


std::string cls_FileBaseClass::GetName() const {return dirname+basename;}



/////////////////////////////////////////////
//       FILE FROM DIRMOUNT CLASS          //
/////////////////////////////////////////////

class cls_FileDirMountedForReading : public cls_FileReadable {
	protected:
		std::string fname;
		mutable void *data;
		mutable unsigned long size;
	public:
		cls_FileDirMountedForReading(const std::string nam,const std::string dnam,const std::string fnameOnHD) : cls_FileReadable(nam,dnam), fname(fnameOnHD), data(NULL), size(0) {}
		virtual ~cls_FileDirMountedForReading() { if (data) free(data); data=NULL; }

		virtual  std::string GetNameForLog() const {return "(( "+fname+" ))";}
		virtual  std::string GetHDDName() const {return fname;}

		virtual  int IsHDDFile() const {return 1;}
		virtual  int IsPacked() const {return 0;}

		virtual void * GetData(const int binary=1) const {
			if (data) { return data; }
			FILE *pFile=fopen(fname.c_str(),(binary ? "rb" : "r"));
			if (!pFile) { CLS_FILE_ERROR("file reference to (("+fname+")) lost",CLS_FILE_ERROR_TYPE_ERROR)  ; return NULL; }
			fseek (pFile, 0, SEEK_END);
			size=ftell(pFile);
			rewind (pFile);
			data=malloc(sizeof(char)*(size+1));
			if (!data)  {CLS_FILE_ERROR("cannot allocate memory for reading file (("+fname+"))",CLS_FILE_ERROR_TYPE_ERROR)  ; return NULL; }
			size_t res=fread(data,1,size,pFile);
			if (res != size) { CLS_FILE_ERROR("cannot read file (("+fname+")) into memory",CLS_FILE_ERROR_TYPE_ERROR)  ; return NULL; }
			((char*)(data))[size]='\0';
			fclose(pFile);
			return data;
			}

		virtual unsigned long GetSize(const int binary=1) const {
			if (!data && size==0) {  //If it was read in once, no need to do it again
					FILE *pFile=fopen(fname.c_str(),(binary ? "rb" : "r"));
					if (!pFile) { CLS_FILE_ERROR("file reference to (("+fname+")) lost",CLS_FILE_ERROR_TYPE_ERROR)  ; return 0; }
					fseek (pFile, 0, SEEK_END);
					size=ftell (pFile);
					fclose (pFile);
					}
			return size;
			}

		virtual void DisownData() const {data=NULL;}
	};

////////////////////////////////////////

class cls_FileDirMountedForWriting : public cls_FileWriteable {
	protected:
		std::string fname;
	public:
		cls_FileDirMountedForWriting(const std::string nam,const std::string dnam,const std::string fnameOnHD) : cls_FileWriteable(nam,dnam), fname(fnameOnHD) {}

		virtual std::string GetNameForLog() const {return "(( "+fname+" ))";}
		virtual std::string GetHDDName() const {return fname;}
		virtual int IsHDDFile() const {return 1;}
		virtual int IsPacked() const {return 0;}

		virtual bool WillOverwrite() const {FILE *f; f=fopen(fname.c_str(),"rb"); if (!f) return false; fclose(f); return true;}

		virtual bool Delete() const {
			if( remove(fname.c_str() ) != 0 ) { return false; }
			return true;
			}
//  virtual void Open(const int binary) {   }  //TODO: Implement

	};


/////////////////////////////////////////////
//       FILE FROM ZIPMOUNT CLASS          //
/////////////////////////////////////////////

class cls_FileZipMountedForReading : public cls_FileReadable {
	protected:
		std::string zipname;
		mutable void *data;
		int index;
		unsigned long cdi;
		unsigned long size;
		HZIP  hz;
	public:
		cls_FileZipMountedForReading(const std::string nam,const std::string dnam,const std::string zipnam,HZIP  hzi,const int ind,const unsigned long cdip,const unsigned long siz)
			: cls_FileReadable(nam,dnam), zipname(zipnam), data(NULL), index(ind), cdi(cdip), size(siz), hz(hzi) {}

		virtual ~cls_FileZipMountedForReading() { if (data) free(data); data=NULL; }

		virtual std::string GetNameForLog() const {return "(( "+zipname + " :: " + GetName() + " ))";}
		virtual std::string GetHDDName() const {return zipname + ":" + GetName();}
		virtual int IsHDDFile() const {return 0;}
		virtual int IsPacked() const {return 1;}

		virtual void * GetData(const int binary=1) const {
			if (data) { return data; }
			data=malloc(sizeof(char)*(size+1));
			if (!data)  {CLS_FILE_ERROR("cannot allocate memory for reading zip-file entry "+GetNameForLog(),CLS_FILE_ERROR_TYPE_ERROR)  ; return NULL; }
			JumpToZipItem(hz,index,cdi);
			if (UnzipItem(hz,index,data,size)!=ZR_OK)
					{CLS_FILE_ERROR("cannot unzip zip-file entry "+GetNameForLog(),CLS_FILE_ERROR_TYPE_ERROR)  ; return NULL; }
			((char*)(data))[size]='\0';
			return data;
			}

		virtual unsigned long GetSize(const int binary=1) const { return size; }

		virtual void DisownData() const {data=NULL;}
	};

/////////////////////////////////////////////
//     SUB FILE SYSTEM BASE CLASS          //
/////////////////////////////////////////////

class cls_FileSubSystemBase {
	protected:
		std::vector<std::string> mountbase;
		virtual cls_FileReadable * virtGetFileForReading(const std::vector<std::string> & elems, int & denied) const=0;
		virtual cls_FileWriteable * virtGetFileForWriting(const std::vector<std::string> & elems, const bool autocreatedirds) const=0;

		bool IsInMountBaseScope(const std::vector<std::string> & elems) const {
			if (elems.size()<mountbase.size()) { return false; }
			for (unsigned int i=0; i<mountbase.size(); i++) if (mountbase[i]!=elems[i]) { return false; }
			return true;
			}
	public:
		cls_FileSubSystemBase(const std::vector<std::string>  mntbase) : mountbase(mntbase) {}
		virtual ~cls_FileSubSystemBase() {}
		virtual int IsMaskLayer() const {return 0;}
		virtual int IsWriteable() const =0;

		virtual bool ListDirectoryEntries(const std::vector<std::string> d,const std::vector<std::string> & lsts,
				std::vector<std::string> & newelems, const int mode,const std::string pds) const=0;

		virtual cls_FileReadable * GetFileForReading(const std::vector<std::string> & elems, int & denied) const {
			if (!IsInMountBaseScope(elems)) { return NULL; }
			return virtGetFileForReading(elems,denied);
			}

		virtual cls_FileWriteable * GetFileForWriting(const std::vector<std::string> & elems, const bool autocreatedirs, int & denied) const {
			if (!IsInMountBaseScope(elems)) { return NULL; }
			denied=1;
			return virtGetFileForWriting(elems,autocreatedirs);
			}

		virtual bool DirExists(const std::vector<std::string> & elems, int & denied) const {
			if (!IsInMountBaseScope(elems)) { return false; }
			if (IsWriteable()) { denied=1; }
			return true;
			}

	};



/////////////////////////////////////////////
//       FILE STACK MASK CLASS             //
/////////////////////////////////////////////


class cls_FileMountStackMaskEntry {
	protected:
		int mode;
		std::vector<cls_FileMountStackMaskEntry*> subs;
		std::vector<std::string> names;
	public:
		cls_FileMountStackMaskEntry() : mode(0) {} // No mode set initially
		~cls_FileMountStackMaskEntry()  { for (unsigned int i=0; i<subs.size(); i++) if (subs[i]) {delete subs[i]; subs[i]=NULL;} subs.clear(); names.clear();}
		int GetMode() const {return mode;}
		void SetMode(const int m) {if (m==CLS_FILE_MASK_UNSET) mode=m; else mode=(m | CLS_FILE_MASK_SET);}
		cls_FileMountStackMaskEntry * GetSubEntry(const std::string s) {
			for (unsigned int i=0; i<names.size(); i++) if (names[i]==s) { return  subs[i]; }
			return NULL;
			}
		cls_FileMountStackMaskEntry * AddSubEntry(const std::string n) {
			cls_FileMountStackMaskEntry *res=GetSubEntry(n);
			if (res) { return res; }
			names.push_back(n);
			subs.push_back(new cls_FileMountStackMaskEntry());
			return subs.back();
			}
	};

class cls_FileMountStackMask : public cls_FileSubSystemBase {
	protected:
		cls_FileMountStackMaskEntry * rootdir;

	public:
		cls_FileMountStackMask() : cls_FileSubSystemBase(std::vector<std::string>()) {rootdir=new cls_FileMountStackMaskEntry();}
		virtual ~cls_FileMountStackMask() {delete rootdir;}
		virtual bool ListDirectoryEntries(const std::vector<std::string> d,const std::vector<std::string> & lsts,
				std::vector<std::string> & newelems, const int mode,const std::string pds) const {return false;}

		virtual int IsWriteable() const {return 0;}
		virtual int IsMaskLayer() const {return 1;}

		int GetMode(const std::vector<std::string> & elems) const {
			int thismode=0;
			int resumode=0;
			cls_FileMountStackMaskEntry *fe=rootdir;
			for (unsigned int i=0; i<elems.size(); i++) {
					fe=fe->GetSubEntry(elems[i]);
					if (!fe) { return resumode; }
					thismode=fe->GetMode();
					if ( (thismode & (CLS_FILE_MASK_SET | CLS_FILE_MASK_RECURSIVE)) == (CLS_FILE_MASK_SET | CLS_FILE_MASK_RECURSIVE)  ) { resumode=thismode; }
					}
			return thismode;
			}

		void SetFileMask(std::vector<std::string> & elems, const int mode) {
			cls_FileMountStackMaskEntry *fe=rootdir;
			for (unsigned int i=0; i<elems.size(); i++) {
					fe=fe->AddSubEntry(elems[i]);
					}
			fe->SetMode(mode);
			}
	protected:
		virtual cls_FileReadable * virtGetFileForReading(const std::vector<std::string> & elems, int & denied) const {
			int mode=GetMode(elems);
			if ( (mode & (CLS_FILE_MASK_SET | CLS_FILE_MASK_READ)) == (CLS_FILE_MASK_SET | CLS_FILE_MASK_READ)  ) { denied=1; }
			return NULL;
			}
		virtual cls_FileWriteable * virtGetFileForWriting(const std::vector<std::string> & elems, const bool autocreatedirds) const {return NULL;}
	};



/////////////////////////////////////////////
//      DIR MOUNT SUB SYSTEM CLASS         //
/////////////////////////////////////////////

class cls_FileSubSystemDirMount : public cls_FileSubSystemBase {
	public:

		static bool RecursiveMkDir(const std::string dir) {

			std::vector<std::string> ds;

			std::string::size_type lastPos = 0;
			std::string::size_type pos     = dir.find_first_of(CLS_PATH_SEPARATOR_SYS, lastPos);
			while (std::string::npos != pos || std::string::npos != lastPos) {
					ds.push_back(dir.substr(lastPos, pos - lastPos));
					lastPos = dir.find_first_not_of(CLS_PATH_SEPARATOR_SYS, pos);
					pos = dir.find_first_of(CLS_PATH_SEPARATOR_SYS, lastPos);
					}

			int error=1;
			int dirindex=ds.size();
			std::string testname;
			while ((error) && (dirindex>=0)) {
					testname=ds[0];
					for (int j=1; j<dirindex; j++) { testname=testname+CLS_PATH_SEPARATOR_SYS+ds[j]; }
#ifdef WIN32
					error=mkdir(testname.c_str());
#else
					int mode=0755; //The creation mode for Unix
					error=mkdir(testname.c_str(),mode);
#endif
					if (!error) { break; }
					dirindex--;
					}
			if (error) { return false; } //FAIL
			for (unsigned int i=dirindex; i<ds.size(); i++) {
					testname=testname+CLS_PATH_SEPARATOR_SYS+ds[i];
#ifdef WIN32
					error=mkdir(testname.c_str());
#else
					int mode=0755; //The creation mode for Unix
					error=mkdir(testname.c_str(),mode);
#endif
					if (error) { return false; } //FAIL
					}
			return true;
			}

	protected:
		int writeable;
		std::string baseOnHD;



		virtual cls_FileWriteable * virtGetFileForWriting(const std::vector<std::string> & elems, const bool autocreatedirs) const {
			//Mountpoint is already checked, thus check the existance of the Dir, where we like to write into
			std::string sname="";
			for (unsigned int i=mountbase.size(); i+1<elems.size(); i++) { sname=sname+CLS_PATH_SEPARATOR_SYS+elems[i]; }
			std::string f=baseOnHD+sname;
			DIR  *dip;
			if ((dip = opendir(f.c_str())) == NULL) {
					if (!autocreatedirs) { return NULL; } //No creation of this dir
					if (!RecursiveMkDir(f)) {
							CLS_FILE_ERROR("(("+f+")) cannot be created, when try to open (("+(f+CLS_PATH_SEPARATOR_SYS+elems.back())+"))  with autocreate-dirs",CLS_FILE_ERROR_TYPE_ERROR);
							return NULL;
							}
					}
			else { closedir(dip); }
			f=f+CLS_PATH_SEPARATOR_SYS+elems.back(); //This is the filename on HDD
			std::string dname="";
			for (unsigned int i=0; i+1<elems.size(); i++) { dname=dname+CLS_PATH_SEPARATOR+elems[i]; }
			return new cls_FileDirMountedForWriting(elems.back(),dname,f);
			}


		virtual cls_FileReadable * virtGetFileForReading(const std::vector<std::string> & elems, int & denied) const {
			std::string sname="";
			for (unsigned int i=mountbase.size(); i<elems.size(); i++) { sname=sname+CLS_PATH_SEPARATOR_SYS+elems[i]; }
			std::string f=baseOnHD+sname;
			std::ifstream ifs(f.c_str());
			if (ifs.fail()) { return NULL; }
			ifs.close();
			std::string dnam="";   for (unsigned int i=0; i+1<elems.size(); i++) dnam=dnam+CLS_PATH_SEPARATOR+elems[i];
			return new cls_FileDirMountedForReading(elems.back(),dnam,f);
			}

	public:
		cls_FileSubSystemDirMount(const std::vector<std::string>  mntbase,const std::string hdbase,bool writeabl) : cls_FileSubSystemBase(mntbase), writeable(writeabl), baseOnHD(hdbase) {}

		virtual bool ListDirectoryEntries(const std::vector<std::string> d,const std::vector<std::string> & lsts,
				std::vector<std::string> & newelems, const int mode,const std::string pds) const {
			if (d.size()<mountbase.size()) {
					if ( (mode & (CLS_FILE_LIST_DIRS | CLS_FILE_LIST_RECURSIVE )) ==0) { return false; } //No need to show
					for (unsigned int i=0; i<d.size(); i++) { if (mountbase[i]!=d[i]) return false;} // not the same
					if ((mode & CLS_FILE_LIST_DIRS) != 0) { newelems.push_back(pds+mountbase[d.size()]+CLS_PATH_SEPARATOR); }
					std::vector<std::string> nd(d); nd.push_back(mountbase[d.size()]);
					return ListDirectoryEntries(nd,lsts,newelems,mode,pds+mountbase[d.size()]+CLS_PATH_SEPARATOR);
					}
			else  for (unsigned int i=0; i<mountbase.size(); i++) { if (mountbase[i]!=d[i]) return false;}

			std::string dname=baseOnHD;
			for (unsigned int i=mountbase.size(); i<d.size(); i++) { dname=dname+CLS_PATH_SEPARATOR_SYS+d[i]; }

			DIR  *dip;
			if ((dip = opendir(dname.c_str())) == NULL) { return false; }
			struct dirent *entry;
			while((entry = readdir(dip))) {
					std::string curr=entry->d_name;
					if (curr=="." || curr=="..") { continue; }
#ifndef _WIN32
					int isdir=(entry->d_type == DT_DIR);
#else
					struct stat statres;
					int isdir=0;
					string path=dname+CLS_PATH_SEPARATOR_SYS+curr;
					if( stat(path.c_str(),&statres) == 0 )  if( statres.st_mode & S_IFDIR ) { isdir=1; }
					// DIR *tdip;
					// int isdir=0;
					// if ((tdip = opendir(curr.c_str())) != NULL) {closedir(tdip); isdir=1;}
#endif
					if ((isdir && ((mode & CLS_FILE_LIST_DIRS) !=0) ) || ((!isdir) && ((mode & CLS_FILE_LIST_FILES) !=0))) {
							newelems.push_back(pds+curr+(isdir ? CLS_PATH_SEPARATOR : ""));
							}
					//  string s="Curr "+curr+(isdir ? "  dir" : "  file");
					//  coutlog(s,0);
					if (isdir && ((mode & CLS_FILE_LIST_RECURSIVE)!=0) ) {
							std::vector<std::string> nd(d); nd.push_back(curr);
							//    cout << "ENTERING " ; for (unsigned int _n=0;_n<nd.size();_n++) cout << nd[_n] << "/"; cout << endl;
							ListDirectoryEntries(nd,lsts,newelems,mode,pds+curr+CLS_PATH_SEPARATOR);
							}
					}
			closedir(dip);
			return true;
			}

		virtual int IsWriteable() const {return writeable;}

		virtual bool DirExists(const std::vector<std::string> & elems, int & denied) const {
			if (!cls_FileSubSystemBase::DirExists(elems,denied)) { return false; } //Wrong mount point and/or set the denied flag for writeables
			std::string sname="";
			for (unsigned int i=mountbase.size(); i<elems.size(); i++) { sname=sname+CLS_PATH_SEPARATOR_SYS+elems[i]; }
			std::string f=baseOnHD+sname;
			DIR  *dip;
			if ((dip = opendir(f.c_str())) == NULL) { return false; }
			closedir(dip);
			return true;
			}

	};



/////////////////////////////////////////////
//      ZIP MOUNT SUB SYSTEM CLASS         //
/////////////////////////////////////////////

//Entry classes


class cls_ZipMountFileEntry {
	protected:
		std::string name;
		int index;
		unsigned long pos;
		unsigned long size;
	public:
		int GetIndex() const {return index;}
		unsigned long GetCDI() const {return pos;}
		unsigned long GetSize() const {return size;}
		cls_ZipMountFileEntry(const std::string n, const  int i, const unsigned long ps, const unsigned long s)
			: name(n), index(i), pos(ps), size(s) {}
		std::string GetName() const {return name;}
	};

class cls_ZipMountDirEntry {
	protected:
		std::string name;
		std::vector<cls_ZipMountDirEntry*> subdirs;
		std::vector<cls_ZipMountFileEntry*> files;
	public:
		cls_ZipMountDirEntry(const std::string n) : name(n) {}

		std::string GetName() const {return name;}
		std::vector<cls_ZipMountDirEntry*> & GetSubDirs()  {return subdirs;}
		std::vector<cls_ZipMountFileEntry*> & GetFiles() {return files;}

		virtual ~cls_ZipMountDirEntry() {
			for (unsigned int i=0; i<subdirs.size(); i++) if (subdirs[i]) {delete subdirs[i]; subdirs[i]=NULL;}
			for (unsigned int i=0; i<files.size(); i++) if (files[i]) {delete files[i]; files[i]=NULL;}
			subdirs.clear(); files.clear();
			}

		cls_ZipMountDirEntry * GetSubDir(const std::string s, const int autocreate) {
			int ind=-1;
			for (unsigned int i=0; i<subdirs.size(); i++) if (subdirs[i]->name==s) {ind=i; break;}
			if (ind!=-1) { return subdirs[ind]; }
			else if (autocreate) {subdirs.push_back(new cls_ZipMountDirEntry(s)); return subdirs.back();}
			else { return NULL; }
			}

		void AddFile(cls_ZipMountFileEntry *f) {  files.push_back(f); }

		cls_ZipMountFileEntry * GetFile(const std::string name) const {
			for (unsigned int i=0; i<files.size(); i++) if (files[i]->GetName()==name) {return files[i];}
			return NULL;
			}


	};




// Zip Mount Sub System class

class cls_FileSubSystemZipMount : public cls_FileSubSystemBase {
	protected:
		std::string zipname;
		HZIP hz;
		cls_ZipMountDirEntry *rootdir;
		int numitems;

		virtual cls_FileWriteable * virtGetFileForWriting(const std::vector<std::string> & elems, const bool autocreatedirds) const {
			return NULL; //Can't write in zips
			}

		virtual cls_FileReadable * virtGetFileForReading(const std::vector<std::string> & elems, int & denied) const {

			cls_ZipMountDirEntry *de=rootdir;
			for (unsigned int si=mountbase.size(); si+1<elems.size(); si++) { de=de->GetSubDir(elems[si],0); if (!de) return NULL; }
			cls_ZipMountFileEntry *fe=de->GetFile(elems.back());
			if (!fe) { return NULL; }
			std::string dname="";
			for (unsigned int i=mountbase.size(); i+1<elems.size(); i++) { dname=dname+CLS_PATH_SEPARATOR+elems[i]; }
			return new cls_FileZipMountedForReading(elems.back(),dname,zipname,hz,fe->GetIndex(),fe->GetCDI(),fe->GetSize());
			}

		void Tokenize(const std::string& str,std::vector<std::string>& tokens) {
			std::string::size_type lastPos = str.find_first_not_of(CLS_PATH_SEPARATOR_ZIP, 0);
			std::string::size_type pos     = str.find_first_of(CLS_PATH_SEPARATOR_ZIP, lastPos);
			while (std::string::npos != pos || std::string::npos != lastPos) {
					tokens.push_back(str.substr(lastPos, pos - lastPos));
					lastPos = str.find_first_not_of(CLS_PATH_SEPARATOR_ZIP, pos);
					pos = str.find_first_of(CLS_PATH_SEPARATOR_ZIP, lastPos);
					}
			}


	public:
		cls_FileSubSystemZipMount(const std::vector<std::string>  mntbase,const std::string zipf) : cls_FileSubSystemBase(mntbase), zipname(zipf) {
			rootdir=new cls_ZipMountDirEntry("");

			hz=OpenZip(zipname.c_str(),0);
			if (!hz) {
					CLS_FILE_ERROR("Cannot load zipfile (("+zipname+"))",CLS_FILE_ERROR_TYPE_ERROR);
					return;
					}

			std::vector<std::string> fnames;
			std::vector<unsigned long> cdis;
			std::vector<unsigned long> sizes;
			std::vector<bool> isdirs;
			ZIPENTRY ze; GetZipItem(hz,-1,&ze);
			numitems=ze.index;
			fnames.resize(numitems); cdis.resize(numitems);
			sizes.resize(numitems); isdirs.resize(numitems);
			for (int i=0; i<numitems; i++) {
					GetZipItem(hz,i,&ze);
					fnames[i]=ze.name;
					GetCurrentCentralDirIndex(hz,&(cdis[i]));
					sizes[i]=ze.unc_size;
					isdirs[i]=(S_ISDIR(ze.attr)!=0);
					}
			for (int i=0; i<numitems; i++) if (isdirs[i]) {
						std::vector<std::string> sds;
						Tokenize(fnames[i],sds);
						cls_ZipMountDirEntry *de=rootdir;
						for (unsigned int si=0; si<sds.size(); si++) { de=de->GetSubDir(sds[si],1); }
						}
			for (int i=0; i<numitems; i++) if (!(isdirs[i])) {
						std::vector<std::string> sds;
						Tokenize(fnames[i],sds);
						cls_ZipMountDirEntry *de=rootdir;
						for (unsigned int si=0; si+1<sds.size(); si++) { de=de->GetSubDir(sds[si],1); }
						de->AddFile(new cls_ZipMountFileEntry(sds.back(),i,cdis[i],sizes[i]));
						}
			}

		virtual ~cls_FileSubSystemZipMount() {
			if (hz) { CloseZip(hz); }
			if (rootdir) { delete rootdir; }
			}

		virtual bool ListDirectoryEntries(const std::vector<std::string> d,const std::vector<std::string> & lsts,
				std::vector<std::string> & newelems, const int mode,const std::string pds) const {
			if (d.size()<mountbase.size()) {
					if ( (mode & (CLS_FILE_LIST_DIRS | CLS_FILE_LIST_RECURSIVE )) ==0) { return false; } //No need to show
					for (unsigned int i=0; i<d.size(); i++) { if (mountbase[i]!=d[i]) return false;} // not the same
					if ((mode & CLS_FILE_LIST_DIRS) != 0) { newelems.push_back(pds+mountbase[d.size()]+CLS_PATH_SEPARATOR); }
					std::vector<std::string> nd(d); nd.push_back(mountbase[d.size()]);
					return ListDirectoryEntries(nd,lsts,newelems,mode,pds+mountbase[d.size()]+CLS_PATH_SEPARATOR);
					}

			cls_ZipMountDirEntry * de=rootdir;
			for (unsigned int i=mountbase.size(); i<d.size(); i++) { de=de->GetSubDir(d[i],0); if (!de) return false;}
			//Iterate through the contents
			if ((mode & CLS_FILE_LIST_FILES) !=0) {
					for (unsigned int i=0; i<de->GetFiles().size(); i++) { newelems.push_back(pds+de->GetFiles()[i]->GetName()); }
					}

			for (unsigned int i=0; i<de->GetSubDirs().size(); i++) {
					if ((mode & CLS_FILE_LIST_DIRS) !=0) { newelems.push_back(pds+(de->GetSubDirs()[i]->GetName())+CLS_PATH_SEPARATOR); }
					if ((mode & CLS_FILE_LIST_RECURSIVE)!=0) {
							std::vector<std::string> nd(d); nd.push_back(de->GetSubDirs()[i]->GetName());
							ListDirectoryEntries(nd,lsts,newelems,mode,pds+de->GetSubDirs()[i]->GetName()+CLS_PATH_SEPARATOR);
							}
					}
			return true;
			}
		virtual int IsWriteable() const {return 0;}


		virtual bool DirExists(const std::vector<std::string> & elems, int & denied) const {
			if (!cls_FileSubSystemBase::DirExists(elems,denied)) { return false; } //Wrong mount point
			cls_ZipMountDirEntry *de=rootdir;
			for (unsigned int si=mountbase.size(); si<elems.size(); si++) { de=de->GetSubDir(elems[si],0); if (!de) return false; }
			return true;
			}

	};


/////////////////////////////////////////////
//         FILE SYSTEM CLASS               //
/////////////////////////////////////////////

//First the internal data
class cls_FileSystem_Info_ {
	protected:
		int skiptop;
		std::string lasterror;
		std::vector<cls_FileSubSystemBase*> subsys;
		void ErrorF(const std::string err,int typ=CLS_FILE_ERROR_TYPE_ERROR) {
			CLS_FILE_ERROR(err,typ);
			lasterror=err;
			}
	public:
		cls_FileSystem_Info_() : skiptop(0), lasterror("") {}

		int StrToElems(const std::string str,std::vector<std::string> & elems) const {
			const std::string delimiters=CLS_PATH_SEPARATOR;
			std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
			std::string::size_type pos     = str.find_first_of(delimiters, lastPos);
			while (std::string::npos != pos || std::string::npos != lastPos) {
					if (pos - lastPos>0) { elems.push_back(str.substr(lastPos, pos - lastPos));  }
					lastPos = str.find_first_not_of(delimiters, pos);
					pos = str.find_first_of(delimiters, lastPos);
					}
			std::vector<std::string>::iterator iter = elems.begin();
			while( iter != elems.end() ) {
					if ((*iter)==".") { iter = elems.erase( iter ) ; }
					else if ((*iter)=="..") { if (iter==elems.begin()) return 0; iter=elems.erase(iter-1, iter+1 );  if (!elems.size()) return 1; }
					else { iter++; }
					}
			return 1;
			}

		bool ListDirectoryEntries(const std::string d,std::vector<std::string> & lsts, const int mode,const char * const pattern) const {
			bool res=false;
			std::vector<std::string> direlems; StrToElems(d,direlems);
			std::vector<std::string> newentries;
			std::string pds="";
			if (mode & CLS_FILE_LIST_FULLPATH)  { for (unsigned int i=0; i<direlems.size(); i++) pds=pds+CLS_PATH_SEPARATOR+direlems[i]; pds+=CLS_PATH_SEPARATOR; }
			std::string fpds="";
				{ for (unsigned int i=0; i<direlems.size(); i++) fpds=fpds+CLS_PATH_SEPARATOR+direlems[i]; fpds+=CLS_PATH_SEPARATOR; }

			for (int i=subsys.size()-1-skiptop; i>=0; i--) {
					if (subsys[i]->IsMaskLayer()) { continue; }
					res=res | subsys[i]->ListDirectoryEntries(direlems,lsts,newentries,mode,pds);
					for (unsigned int ne=0; ne<newentries.size(); ne++) {
							bool alreadyin=false;
							std::string curr=newentries[ne];
							for (unsigned int oe=0; oe<lsts.size(); oe++) if (lsts[oe]==curr)  {alreadyin=true; break;}
							if (alreadyin) { continue; }


							if (pattern) {
									/*       #ifndef _WIN32
									      int matchres=fnmatch(pattern, curr.c_str(),CLS_FILE_PATTERN_MATCH_FLAGS);
									      if (matchres==FNM_NOMATCH) continue;
									      #else
									        ///TODO: Windows-Match*/
									if (!winfnmatch(pattern,curr)) { continue; }
									// #endif
									}

							std::vector<std::string> newelems;
							std::string curr2;
							if (mode & CLS_FILE_LIST_FULLPATH) { curr2=curr; } // TODO: make sure that it is correct
							else { curr2=fpds+curr; }
							StrToElems(curr2,newelems);

							int blocked=0;
							for (int j=subsys.size()-1-skiptop; j>i; j--) {
									if (!subsys[j]->IsMaskLayer()) { continue; }
									cls_FileMountStackMask *sm=dynamic_cast<cls_FileMountStackMask *>(subsys[j]);
									if (!sm) { continue; }
									int mode=sm->GetMode(newelems);
									//   std::cout << "Element " << curr << " has mode " << mode << std::endl;
									if ( (mode & (CLS_FILE_MASK_SET | CLS_FILE_MASK_LIST))==(CLS_FILE_MASK_SET | CLS_FILE_MASK_LIST)) { blocked=1; break;}
									}
							if (blocked) { continue; }

							lsts.push_back(curr);
							}
					newentries.clear();
					}
			sort(lsts.begin(),lsts.end());
			return res;
			}

		cls_FileReadable * GetFileForReading(const std::string fname) const {
			std::vector<std::string> elems;
			if (!StrToElems(fname,elems)) { return NULL; }
			for (int i=subsys.size()-1-skiptop; i>=0; i--) {
					int denied=0;
					cls_FileReadable * rf=subsys[i]->GetFileForReading(elems,denied);
					if (rf || denied) { return rf; }
					}
			return NULL;
			}

		cls_FileWriteable * GetFileForWriting(const std::string fname,const bool autocreatesubdirs) const {
			std::vector<std::string> elems;
			if (!StrToElems(fname,elems)) { return NULL; }
			for (int i=subsys.size()-1-skiptop; i>=0; i--) {
					if (!subsys[i]->IsWriteable()) { continue; }
					int denied=0;
					cls_FileWriteable * rf=subsys[i]->GetFileForWriting(elems,autocreatesubdirs,denied);
					if (rf || denied) { return rf; }
					}
			return NULL;
			}

		void Clear()  { for (unsigned int i=0; i<subsys.size(); i++) {delete  subsys[i];}  subsys.clear();}

		~cls_FileSystem_Info_() {Clear();}

		bool MountHDDDir(std::string dir,bool writeable,std::string mountbase,bool autocreate_write_dir) {
			std::vector<std::string> mntb;
			while (dir.length()>1 && dir.substr(dir.length()-1,1)==CLS_PATH_SEPARATOR_SYS) { dir=dir.substr(0,dir.length()-1); }
			if (!cls_DirectoryExists(dir)) {
					int dircreated=0;
					if (writeable && autocreate_write_dir) {
							dircreated=(cls_FileSubSystemDirMount::RecursiveMkDir(dir) ? true : false);
							}
					if (!dircreated) {
							ErrorF("cannot mount directory (( "+dir+" )), because it was not found"+(writeable && autocreate_write_dir ? " and could not be created" : ""));
							return false;
							}
					}
			if (!StrToElems(mountbase,mntb)) {ErrorF("cannot mount directory (( "+dir+" )) at invalid mountpoint (( "+mountbase+" ))"); return false;}
			cls_FileSubSystemDirMount * md=new cls_FileSubSystemDirMount(mntb,dir,writeable);
			subsys.push_back(md);
			return true;
			}

		bool MountZipFile(std::string zipf,std::string mountbase="") {
			std::vector<std::string> mntb;
			if (!StrToElems(mountbase,mntb)) {ErrorF("cannot mount zip-file (( "+zipf+" )) at invalid mountpoint (( "+mountbase+" ))"); return false;}
			cls_FileSubSystemZipMount * md=new cls_FileSubSystemZipMount(mntb,zipf);
//    if (md->GetLastError()!="") {delete md; return false; }
			//Now check the version
			std::vector<std::string> elems;
			elems.push_back("package_version");
			int dendumm;
			cls_FileReadable * vers= md->GetFileForReading(elems,dendumm);
			if (vers) {
					char * conts=(char *)vers->GetData(0); std::string pversion=conts;
					std::string cversion=g_Vars()->GetVarString("CuboVersion",0);
					float pfloat=atof(pversion.c_str());
					float cfloat=atof(cversion.c_str());
					if (pfloat<cfloat) { delete vers; vers=NULL;}

					}

			if (!vers) {
					std::string msg="Package "+zipf+" skipped! Version mismatch!";
					coutlog(msg,2);
					delete md;
					return false;
					}
			delete vers;

			subsys.push_back(md);
			return true;
			}

		bool DirExists(const std::string d,const bool for_write_only, const bool uselisting_deny_mask) const {
			std::vector<std::string> elems;
			if (!StrToElems(d,elems)) { return false; } //Since it is a dir parent to root "/"
			for (int i=subsys.size()-1-skiptop; i>=0; i--) {
					bool wable=subsys[i]->IsWriteable();
					if (for_write_only && (!(wable))) { continue; }
					int denied=0;
					if (!wable && subsys[i]->IsMaskLayer()) {
							cls_FileMountStackMask *msm=dynamic_cast<cls_FileMountStackMask*>(subsys[i]);
							if (msm) {
									int mode=msm->GetMode(elems);
									int mask=CLS_FILE_MASK_SET | (uselisting_deny_mask==true ? CLS_FILE_MASK_LIST : CLS_FILE_MASK_READ);
									if ( (mode & mask) == mask) { denied=1; }
									}
							if (denied) { return false; }
							}
					bool res=subsys[i]->DirExists(elems,denied);
					if (res || (denied && for_write_only)) { return res; }
					}
			return false;
			}

		std::string GetError(const int reset) {if (lasterror=="") return lasterror; if (reset) { std::string res=lasterror; lasterror=""; return res;} return lasterror;}

		int AddMaskLayer() {
			subsys.push_back(new cls_FileMountStackMask());
			int res=-1; for (unsigned int i=0; i<subsys.size(); i++) if (subsys[i]->IsMaskLayer()) res++;
			return res;
			}

		cls_FileMountStackMask * GetMaskLayer(int index) {
			int thindex=-1;
			int topmost=-1;
			cls_FileMountStackMask * msk=NULL;
			for (unsigned int i=0; i<subsys.size(); i++)
				if (subsys[i]->IsMaskLayer()) {
						thindex++;
						if (thindex==index) {msk=dynamic_cast<cls_FileMountStackMask*>(subsys[i]); if (msk) break;}
						if (index==-1) { topmost=i; }
						}
			if (topmost !=-1) { msk=dynamic_cast<cls_FileMountStackMask*>(subsys[topmost]); }
			if (!msk && index==-1) { AddMaskLayer(); msk=dynamic_cast<cls_FileMountStackMask*>(subsys.back()); }
			return msk;
			}

		void DeleteMaskLayer(const int index) {
			int thindex=-1;
			int i=0;
			while (i<(int)subsys.size()) {
					if (subsys[i]->IsMaskLayer()) {
							thindex++;
							if (thindex==index || index==-1) {
									cls_FileMountStackMask* msk=dynamic_cast<cls_FileMountStackMask*>(subsys[i]);  if (!msk) { i++; continue; }
									delete msk;  subsys[i]=NULL;
									subsys.erase(subsys.begin()+i);
									i--;
									}
							}
					i++;
					}
			}

		void SetFileMask(const std::string dir_or_file,const int mode,const int layer) {
			cls_FileMountStackMask *msk=GetMaskLayer(layer); if (!msk) return;
			std::vector<std::string> elems;
			if (!StrToElems(dir_or_file,elems)) {ErrorF("cannot mask the dir/file (( "+dir_or_file+" )) since it is an invalid path/filename"); return;}
			msk->SetFileMask(elems,mode);
			}

		void PopBottom() {
			if (subsys.size()==0) { return; }
			delete subsys.front();
			subsys.erase(subsys.begin());
			}
	};




////////////////
#define FSINFO ((cls_FileSystem_Info_*)(info))

//The wrapper hiding most of the used stuff to prevent a huge header file with tons of classes, which are only used in this cpp file

cls_FileSystem::cls_FileSystem() {
	info=new cls_FileSystem_Info_();

#ifdef LOG_USED_FILES
	logufiles=new std::ofstream("_used_files.lst");
#endif

	}

cls_FileReadable * cls_FileSystem::GetFileForReading(const std::string fname) const {
#ifdef LOG_USED_FILES
	cls_FileReadable *res=FSINFO->GetFileForReading(fname);
	if (res) { (*logufiles) << fname << std::endl; }
	return res;
#else
	return FSINFO->GetFileForReading(fname);
#endif
	}

cls_FileWriteable * cls_FileSystem::GetFileForWriting(const std::string fname,bool autocreatesubdirs) const {
	return FSINFO->GetFileForWriting(fname,autocreatesubdirs);
	}

cls_FileSystem::~cls_FileSystem() {
	delete FSINFO;
#ifdef LOG_USED_FILES
	logufiles->close();
	delete logufiles;
#endif
	}

bool cls_FileSystem::MountWriteableHDDDir(std::string dir,std::string mountbase,bool autocreatedir) const {
	return FSINFO->MountHDDDir(dir,true,mountbase,autocreatedir);
	}

bool cls_FileSystem::MountHDDDir(std::string dir,std::string mountbase) const {
	return FSINFO->MountHDDDir(dir,false,mountbase,false);
	}

bool cls_FileSystem::MountZipFile(std::string zipfile, std::string mountbase) const {
	return FSINFO->MountZipFile(zipfile,mountbase);
	}

bool cls_FileSystem::MountZipFile(const cls_FileReadable *  const fr,std::string mountbase) const {
	if (!fr) { CLS_FILE_ERROR("try to mount a NULL pointer ZIP file at mountbase "+mountbase, CLS_FILE_ERROR_TYPE_ERROR); return false;}
	if (!fr->IsHDDFile())  { CLS_FILE_ERROR("can only mount ZIP files from HDD, not from location "+fr->GetNameForLog(), CLS_FILE_ERROR_TYPE_ERROR);  return false;}
	return MountZipFile(fr->GetHDDName(),mountbase);
	}


std::string cls_FileSystem::GetLastError(int reset) const {
	return FSINFO->GetError(reset);
	}

bool cls_FileSystem::ListDirectoryEntries(const std::string d,std::vector<std::string> & lsts, const int mode,const std::string pattern) const {
	if (pattern!="") {
			return FSINFO->ListDirectoryEntries(d,lsts, mode,pattern.c_str());
			}
	else {
			return FSINFO->ListDirectoryEntries(d,lsts, mode,NULL);
			}
	}

bool cls_FileSystem::DirExists(const std::string d,const bool for_write_only, const bool uselisting_deny_mask) const {
	return FSINFO->DirExists(d,for_write_only,uselisting_deny_mask);
	}


int  cls_FileSystem::AddMaskLayer() const {
	return FSINFO->AddMaskLayer();
	}

void cls_FileSystem::DeleteMaskLayer(const int ind) const {
	FSINFO->DeleteMaskLayer(ind);
	}

void  cls_FileSystem::SetFileMask(const std::string dir_or_file,const int mode, const int layer) const {
	FSINFO->SetFileMask(dir_or_file,mode,layer);
	}

void cls_FileSystem::PopBottom() {
	FSINFO->PopBottom();
	}

#undef FSINFO
////////////////

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
