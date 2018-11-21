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

#include "vectors.hpp"
#include "cuboboxes.hpp"
#include "cubolevels.hpp"
#include "c3dobjects.hpp"


#define CUBO_SIDEV 0
#define CUBO_UPV 1
#define CUBO_DIRV 2

#define CUBO_MOVE_NONE 0
#define CUBO_MOVE_AHEAD 1
#define CUBO_MOVE_UP 2
#define CUBO_MOVE_DOWN 3
#define CUBO_MOVE_ROTATE_LEFT 4
#define CUBO_MOVE_ROTATE_RIGHT 5
#define CUBO_MOVE_JUMP_UP 6
#define CUBO_MOVE_JUMP_AHEAD 7
#define CUBO_MOVE_FALLING 8
#define CUBO_MOVE_SLIDE_DOWN 9
#define CUBO_MOVE_JUMP_FAR 10
#define CUBO_MOVE_GRAVITY_CHANGE 11



#define CUBO_GROUND_DIST_STD (CUBO_SCALE/2.6)


#define CUBO_MOVESPEED_STD 4.0
#define CUBO_ROTSPEED_STD (1.0*CUBO_MOVESPEED)
#define CUBO_ROTUPSPEED (CUBO_MOVESPEED*CUBO_SCALE/((CUBO_SCALE-CUBO_GROUND_DIST)))
#define CUBO_ROTDOWNSPEED (CUBO_MOVESPEED*CUBO_SCALE/((CUBO_SCALE+CUBO_GROUND_DIST)))

#define CUBO_JUMP_UPVEL_STD (CUBO_SCALE*16.0)
#define CUBO_FARJUMP_UPVEL_STD (CUBO_SCALE*18)
#define CUBO_HIGHJUMP_UPVEL_STD (CUBO_SCALE*30)
#define LATE_FORWARD_JUMP_TIME_STD 0.075

#define FORWARD_PRESS_TIME_STD 0.15
#define FORWARD_PRESS_TIME_JUMP_STD 0.10

#define ROTATE_STOP_TIME_STD 0.0

// #define CUBO_JUMP_DIRVEL (CUBO_SCALE*8)
///TODO: Calc this out of the equation of motion to fit exact jump dist

#define CUBO_GRAVITY_G_STD (CUBO_SCALE*64.0)
#define CUBO_MAX_FALLSPEED_STD (CUBO_SCALE*30.0)

#define CUBO_LOOK_RELAX_SPEED 3.0
#define CUBO_LOOK_SPEED 3.0

#define CUBO_FALL_TILL_LOOKDOWN 0.4

class CuboPlayer {
	protected:
		int id;
		float caminterpolation;
		float camspeed;
		LuaVarHolder varholder;
		std::vector<int> actorids;
		int activeact;
		int lastact;
		Vector3d pyr;
		MatrixObject old_mo,new_mo;
		void CamMove(int newactor);
	public:
		CuboPlayer(int mid) : id(mid), caminterpolation(1.0),camspeed(3),activeact(0), lastact(0) {}
		void SetCamSpeed(float cs) {camspeed=cs;}
		bool InCameraPan() {return caminterpolation<1.0;}
		void SetCameraPos(float elapsed,MatrixObject *cam);
		LuaVarHolder  *GetVarHolder() {return &varholder;}
		void AddActor(int i);
		int GetActiveActor() {if ((activeact<0) || (activeact>=(int)actorids.size())) return -1; else return actorids[activeact];}
		void RemoveActor(int i);
		int NumActors() {return actorids.size();}
		int GetActor(int i) {return actorids[i];}
		int NextActor() ;
		float GetCamInterpolation() {return caminterpolation;}
		int GetLastActiveActor() {return actorids[lastact];}
		int SelectActor(int actorind);
	};



class ActorDef : public BaseLuaDef {
	public:
		virtual ~ActorDef() {}
		virtual int GetType() {return FILE_ACTORDEF;}
		void Call_Constructor(int id);
		void Call_Render(int id);
		int Call_SpecialRender(std::string nam,int index);
		void Call_DistRender(int id);
		void Call_Think(int id);
		void Call_PostThink(int id);
		void Call_ActorCollide(int id,int oid);
		void Call_ActorCollidePlayer(int id,int oid);
		void Call_ChangeMove(int id,std::string newmove);
		void Call_Event(int id,std::string ev);
		int Call_CheckLandingOnSide(int id,int side);
		void SendKey(int actor,int key,int down,int toggle);
		void SendJoyButton(int actor,int stick,int button,int dir,int down,int toggle);
		virtual int IsPlayer() {return 1;}
		std::string Call_GetEditorInfo(std::string what,std::string std);
	};

class ActorDefServer : public BaseDefServer<ActorDef> {
	public:
		//int GetDef(std::string name, int forplayer=1);
		int AddEDef(std::string name);
	};


//Only the directions (for MayMove)
const std::string s_CuboMoveStringsDir[]= {"none","forward","forward","rolldown","left","right","jumpup","jumpahead","falling","slidedown","jumpfar","jumpahead","changegravity"};
//Full strings (for Animation)
const std::string s_CuboMoveStringsMove[]= {"none","forward","up","down","left","right","jumpup","jumpahead","falling","slidedown","jumpfar","jumphigh","changegravity"};


using TBasis = Vector3d[3];

class TCuboMovement {
	protected:
		int defindex;
		int id;
		int selectable;
		int inmove,lastmove;
		int resetview;
		int inhighjump;
		TBasis oldbase;
		TBasis newbase;
		TBasis base; //The basis, consist of side,up,dir
		float falltime;
		float j_heightoverground; //How far are we over the ground ///with respect to the last jumping height
		float j_dist,j_desdist;//How far are we jumped forward and should we jump forward
		float j_flytime;
		Vector3d pos;
		Vector3d newpos;
		Vector3d oldpos;
		CuboLevel *lvl;
		CuboBlock *onBlock;
		CuboBlock *BlockUnderMe;
		LuaVarHolder varholder;
		Vector3d camfloats;       //
		Vector3d lookupfloats;    // VARIABLES FOR THE CAMERAS POSITION
		Vector3d lookdownfloats;  //
		float CUBO_MOVESPEED,CUBO_GROUND_DIST,CUBO_GROUND_DIST_OFFS,CUBO_JUMP_UPVEL,CUBO_FARJUMP_UPVEL,CUBO_HIGHJUMP_UPVEL,CUBO_GRAVITY_G,CUBO_MAX_FALLSPEED,CUBO_ROTSPEED,LATE_FORWARD_JUMP_TIME,FORWARD_PRESS_TIME,FORWARD_PRESS_TIME_JUMP,ROTATE_STOP_TIME;
		float distance_normjump,distance_farjump,distance_highjump;
		int startposset;
		int BlockSideUnderMe;
		int inAir;
		float yvel,dvel;
		float lookpos;
		int lookdir;
		int onSide,startrot;
		int laststaticside;
		int movementcheck;
		std::string jumptype;
		//  TCuboBlock *prevBlock;
		int prevSide;
		float moveInterpolate; //From 0 to 1... how far did we move on the blocks
		float forwardpresstime; //How long is the last forward press away
		float lateforwardjumptime; //How long do we have left to press forward after jumping to invoke forward jump
		float rotatestoptime;
		float camzrot;
		int cammirror;
		double timemultiplicator;
		double gravitychangespeed;//For changing gravity
		int grav180rot; //For upside down rotation
		double tempmovespeedmultiply;
		// float radius;
		Vector3d AtBlockPos(CuboBlock *b,int s);
		int DirVectToSide(Vector3d dirvect);
		CuboBlock *GetRelBlock(CuboBlock *b,Vector3d dir);
		CuboBlock *GetRelBlock(CuboBlock *b,int side);
		void AirMove(float elapsed);
		int MayMove(int typ);
		void Call_ChangeMove();
		int player;
	public:
		void SetPlayer(int plr)  {player=plr;}
		int GetPlayer() {return player;}
		virtual ~TCuboMovement();
		int IsSelectable() {return selectable;}
		void SetSelectable(int s) {selectable=s;}
		TCuboMovement() : selectable(1), camfloats(3,3.0/2.0,3.0/4.0), lookupfloats(0.01,3.355, 3.0/4.0), lookdownfloats(0.2,1.0/5.0,2.0), CUBO_MOVESPEED(CUBO_MOVESPEED_STD),
			CUBO_GROUND_DIST(CUBO_GROUND_DIST_STD), CUBO_GROUND_DIST_OFFS(0), CUBO_JUMP_UPVEL(CUBO_JUMP_UPVEL_STD), CUBO_FARJUMP_UPVEL(CUBO_FARJUMP_UPVEL_STD), CUBO_HIGHJUMP_UPVEL(CUBO_HIGHJUMP_UPVEL_STD), CUBO_GRAVITY_G(CUBO_GRAVITY_G_STD),
			CUBO_MAX_FALLSPEED(CUBO_MAX_FALLSPEED_STD),  CUBO_ROTSPEED(CUBO_ROTSPEED_STD), LATE_FORWARD_JUMP_TIME(LATE_FORWARD_JUMP_TIME_STD),
			FORWARD_PRESS_TIME(FORWARD_PRESS_TIME_STD),FORWARD_PRESS_TIME_JUMP(FORWARD_PRESS_TIME_JUMP_STD), ROTATE_STOP_TIME(ROTATE_STOP_TIME_STD),
			distance_normjump(2),distance_farjump(3),distance_highjump(1), startposset(0), lookpos(0),startrot(0), camzrot(0), cammirror(0),timemultiplicator(1.0),player(-1) {}
		LuaVarHolder  *GetVarHolder() {return &varholder;}
		virtual void SetType(int mid,std::string name);
		void SetID(int mid) {id=mid;}
		void Render();
		void SpecialRender(std::string nam,int defrender);
		void SetRotateStopTime(double current,double def) {rotatestoptime=current; ROTATE_STOP_TIME=def;}
		void InterpolateMove(double elapsed);
		void FinalizeMove();
		void PostLevelThink();
		void Init(CuboLevel *level);
		void MoveForward();
		void RotateRight();
		void RotateLeft();
		void SendKey(int key,int down,int toggle);
		void LookUp();
		void LookDown();
		void LookReset();
		void SetMovementCheck(int mvcheck) {movementcheck=mvcheck;}
		int GetMovementCheck() {return movementcheck;}
		float GetLookPos() {return lookpos;}
		void SetLookPos(float v) {lookpos=v;}
		void Jump();
		int ChangeGravity(Vector3d newgrav,Vector3d newp,double changespeed,int do_at_same_gravity);
		void SetTimeMultiplicator(double tm) {timemultiplicator=tm;}

		float GetRadius() {return CUBO_GROUND_DIST;}
		void SetRadius(float r) {CUBO_GROUND_DIST=r; }

		float GetGroundOffset() {return CUBO_GROUND_DIST_OFFS;}
		void SetGroundOffset(float d) {CUBO_GROUND_DIST_OFFS=d; }

		float GetSpeed() {return CUBO_MOVESPEED;}
		void SetSpeed(float v) {CUBO_MOVESPEED=v;}
		void SetJumpParams(float uv,float faruv,float highuv, float g,float mspeed) {CUBO_JUMP_UPVEL=uv; CUBO_HIGHJUMP_UPVEL=highuv; CUBO_FARJUMP_UPVEL=faruv; CUBO_GRAVITY_G=g; CUBO_MAX_FALLSPEED=mspeed;}
		void SetJumpDistances(float uv,float faruv,float highuv) {distance_normjump=uv; distance_farjump=faruv; distance_highjump=highuv;}
		void SetJumpTiming(float fwpt,float fwptj,float lateforward) {FORWARD_PRESS_TIME=fwpt; FORWARD_PRESS_TIME_JUMP=fwptj;  LATE_FORWARD_JUMP_TIME=lateforward; }
		void SetForwardPressTime(float fpt) {forwardpresstime=fpt;}
		void SetCamParams(std::string what,Vector3d params);
		void SetCamZRotation(float zr,int mirror);
		float s_MoveSpeed(int move);
		void HighJump();
		void FarJump(); //3 Blocks
		void JumpUp(); //Ensures that we only jump up.. For callmove on onedir blocks
		void SetCamPos(MatrixObject *cam);
		int GetID() {return id;}
		Vector3d GetPos() {return pos;}
		Vector3d GetOldPos() {return oldpos;}
		Vector3d GetSide() {return base[CUBO_SIDEV];}
		Vector3d GetUp() {return base[CUBO_UPV];}
		Vector3d GetDir() {return base[CUBO_DIRV];}
		int GetOnSideID();
		int GetLastOnSideID();
		int GetPrevOnSideID();
		int TraceOnSideID();
		void Think();
		void DistRender();
		void SetStartPos(CuboBlockSide *sside,int srot);
		int GetJumpDistBlocks();
		void SetJumpDistBlocks(int d);
		void ResetForwardPressTime() {forwardpresstime=0;}
		virtual int IsPlayer() {return 1;}
		float GetUpVel() {return yvel;}
		void SetUpVel(double v) {yvel=v;}
		std::string GetMoveType();
		void CheckEnemyCollision(TCuboMovement * other);
		void CheckPlayerCollision(TCuboMovement * other);
		std::string GetType();
		int GetStartRotation() {return startrot;}
		std::string GetEditorInfo(std::string what,std::string def);
		void SetRotSpeed(float v);
		void SendJoyButton(int stick,int button,int dir,int down,int toggle);
		void Rebounce(TCuboMovement *other);
		void SetTemporaryMoveSpeedMultiplier(double mltp) {tempmovespeedmultiply=mltp;}
		double GetTemporaryMoveSpeedMultiplier() {return tempmovespeedmultiply;}
		void FrontSideRebounce();
	};


extern void LUA_ACTOR_RegisterLib();
extern void LUA_PLAYER_RegisterLib();

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
