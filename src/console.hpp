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

#include <vector>
#include <string>
#include "luautils.hpp"

#define CUBO_CONSOLE_MAX_LINES 512

using ConsoleBinding = struct
	{
	int k;
	std::string cmd;
	};

class CuboConsole
	{
	protected:
		bool isactive;
		std::vector<std::string> lines;
		std::vector<std::string> history;
		std::string hisbackup;
		int hisindex;
		int was_mouse_snapped;
		std::vector<int> types; //Error, warning, normal
		int currentline; //Used, when lines.size() has reached CUBO_CONSOLE_MAX_LINES
		int scrolloffs;
		int screenlines;
		double lowerline_ypos;
		int togglekey;
		void ParseCmdLine(std::string cmdl="");
		LuaAccess lua;
		std::vector<ConsoleBinding> binds;
		int Bind(std::vector<std::string> & extratoks,int unbind);
	public:
		static CuboConsole* GetInstance();

		void Init();
		int CheckBindKey(int ident,int down,int toggle);
		int GetToggleKey();
		void SetToggleKey(int tk);
		int GetLineIndex(int offs);
		int Toggle();
		int IsActive();
		CuboConsole();
		void AddLine(std::string s,int typ=0);
		~CuboConsole();
		void Render();
		void KeyHandle(int ident,int down,int toggle);
	};

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
