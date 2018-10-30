#include "cuboenemies.hpp"
#include "game.hpp"

using namespace std;

void TCuboEnemy::SetType(int mid,string name)
	{
	id=mid;
	defindex=g_Game()->GetActorDefs()->AddEDef(name);
	g_Game()->GetActorDefs()->GetDefPtr(defindex)->Call_Constructor(id);
	}



/////LUA IMPLEMENT//////////////

int ENEMY_New(lua_State *state)
	{
	string defname=LUA_GET_STRING;
	defname=g_Game()->GetLevel()->CheckDefExchange(defname,"enemy");
	int res=g_Game()->AddEnemy(defname);
	LUA_SET_INT(res);
	return 1;

	}

void LUA_ENEMY_RegisterLib()
	{
	g_CuboLib()->AddFunc("ENEMY_New",ENEMY_New);
	}
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
