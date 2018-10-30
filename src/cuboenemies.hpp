#ifndef ENEMIES_H_G
#define ENEMIES_H_G

#include "cuboactors.hpp"
//#include <iostream>

class TEnemyDef : public TActorDef
	{
	public:
		virtual int GetType() {return FILE_ENEMYDEF;}
		virtual int IsPlayer() {return 0;}
	};



class TCuboEnemy : public TCuboMovement
	{
	public:
		virtual int IsPlayer() {return 0;}
		virtual void SetType(int mid,std::string name);
	};


extern void LUA_ENEMY_RegisterLib();

#endif
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
