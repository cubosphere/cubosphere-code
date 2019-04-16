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

#include "cuboenemies.hpp"
#include "game.hpp"

void CuboEnemy::SetType(int mid,std::string name) {
	id=mid;
	defindex=g_Game()->GetActorDefs()->AddEDef(name);
	g_Game()->GetActorDefs()->GetDefPtr(defindex)->Call_Constructor(id);
	}



/////LUA IMPLEMENT//////////////

int ENEMY_New(lua_State *state) {
	std::string defname=LUA_GET_STRING(state);
	defname=g_Game()->GetLevel()->CheckDefExchange(defname,"enemy");
	int res=g_Game()->AddEnemy(defname);
	LUA_SET_NUMBER(state, res);
	return 1;

	}

void LUA_ENEMY_RegisterLib() {
	g_CuboLib()->AddFunc("ENEMY_New",ENEMY_New);
	}
