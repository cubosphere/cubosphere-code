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

class Theme {
	public:
		Theme() = delete;
		~Theme() = delete;
		static std::string FileName(std::string dir,std::string fname,std::string ext);
		static std::string GetName();
		static void Clear();
		static void AddDir(std::string subdir);
		static void Load(std::string n);
	protected:
		static std::string activeTheme;
		static std::vector<std::string> themedirs;
	};

extern void LUA_THEME_RegisterLib();
