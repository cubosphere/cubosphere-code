#ifndef CONSOLE_H_G
#define CONSOLE_H_G

/**
Copyright (C) 2011 Chriddo

This program is free software;
you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program;
if not, see <http://www.gnu.org/licenses/>.
**/

#include <vector>
#include <string>
#include "luautils.hpp"

#define CUBO_CONSOLE_MAX_LINES 512

typedef struct
	{
	int k;
	std::string cmd;
	} TConsoleBinding;

class TCuboConsole
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
		TLuaAccess lua;
		std::vector <TConsoleBinding> binds;
		int Bind(std::vector<std::string> & extratoks,int unbind);
	public:
		void Init();
		int CheckBindKey(int ident,int down,int toggle);
		int GetToggleKey();
		void SetToggleKey(int tk);
		int GetLineIndex(int offs);
		int Toggle();
		int IsActive();
		TCuboConsole();
		void AddLine(std::string s,int typ=0);
		~TCuboConsole();
		void Render();
		void KeyHandle(int ident,int down,int toggle);
	};

extern TCuboConsole * g_CuboConsole();

#endif
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
