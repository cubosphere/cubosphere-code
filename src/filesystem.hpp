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

#include <string>
#include <vector>

///Added for Cubo
#include <GL/glew.h>
#include <SDL.h>
#include <libgen.h>

class cls_FileBaseClass {
	protected:
		std::string dirname; //Stores the dirname in absolute path
		std::string basename; //The base name
//    int MountStackDepth;??
	public:
		cls_FileBaseClass(const std::string bnam,const std::string dnam);
		virtual ~cls_FileBaseClass() {}
		virtual std::string GetNameForLog() const=0;

		std::string GetName() const;
		std::string GetDirname() const {return dirname;}
		std::string GetBasename() const {return basename;}
		virtual std::string GetHDDName() const=0;

		virtual int IsHDDFile() const=0;
		virtual int IsPacked() const=0;

	};

class cls_FileReadable : public cls_FileBaseClass {
	public:
		cls_FileReadable(const std::string nam,const std::string dnam) : cls_FileBaseClass(nam,dnam) {}
		virtual unsigned long GetSize(const int binary=1) const=0;
		virtual char *GetData(const int binary=1) const=0;
		virtual void DisownData() const=0;  //After getting data, the data will be deleted in the file destructor. Disown it to let the data at *GetData() survive
		///Added for Cubo
		SDL_RWops * GetAsRWops(int binary=1) {
			if  (IsHDDFile()) { return SDL_RWFromFile(GetHDDName().c_str(),binary==1 ? "rb" : "r"); }
			else { return SDL_RWFromMem(GetData(),GetSize()); }
			}
	};

///Added for Cubo
using CuboFile = cls_FileReadable;

class cls_FileWriteable : public cls_FileBaseClass {
	public:
		cls_FileWriteable(const std::string nam,const std::string dnam) : cls_FileBaseClass(nam,dnam) {}
//    virtual void Open(const int binary)=0; //If not called, no writing happens.. Thus, possible to check writeability by getting this object without calling open
		virtual bool WillOverwrite() const=0; //Is there already a file?
		virtual bool Delete() const =0; //Delete a file
	};


/////////////////////////////
//Mask - Layers /////////////
/////////////////////////////

/* A Mask layer protects files in lower Mount-Stack entries from being read or listed
*/


#define CLS_FILE_MASK_UNSET 0
//Remove the mask of this element
#define CLS_FILE_MASK_LIST 2
#define CLS_FILE_MASK_READ (4 | CLS_FILE_MASK_LIST)
//Reading deny mask implies listing deny mask!
#define CLS_FILE_MASK_RECURSIVE 8
//Setting the same for the sub-elements (if they are not set explicitly)

/////////////////////////////
//File - System /////////////
/////////////////////////////

#define CLS_FILE_LIST_RECURSIVE 1
#define CLS_FILE_LIST_FILES 2
#define CLS_FILE_LIST_DIRS 4
#define CLS_FILE_LIST_FULLPATH 8

#define CLS_FILE_LIST_DEFAULT (CLS_FILE_LIST_DIRS | CLS_FILE_LIST_FILES )

class cls_FileSystem {
	private:
		void * info;
	public:
		cls_FileSystem();
		~cls_FileSystem();
		std::string GetLastError(const int reset=1) const;

		void PopBottom();

		bool MountHDDDir(std::string dir,std::string mountbase="") const;
		bool MountWriteableHDDDir(std::string dir,std::string mountbase="",bool autocreatedir=true) const;
		bool MountZipFile(std::string zipf, std::string mountbase="") const;
		bool MountZipFile(const cls_FileReadable *const  fr, std::string mountbase="") const;

		cls_FileReadable * GetFileForReading(const std::string fname) const;
		cls_FileWriteable * GetFileForWriting(const std::string fname,const bool autocreatesubdirs=false) const;

		bool ListDirectoryEntries(const std::string d,std::vector<std::string> & lsts, const int mode=CLS_FILE_LIST_DEFAULT,const std::string pattern="") const; //false, if not exists
		bool DirExists(const std::string d,const bool for_write_only,const bool uselisting_deny_mask=true) const;

		int AddMaskLayer() const; //Returns an index to the new masklayer
		void DeleteMaskLayer(const int ind) const; //Deletes a mask-layer (-1 means all mask layers)
		void SetFileMask(const std::string dir_or_file,const int mode, const int layer=-1) const; //layer=-1 : Topmost masklayer, if not existant, create one atop of the stack

		//TODO: ClearStack function, ReverseStack, PopTop, PopBottom
	};

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
