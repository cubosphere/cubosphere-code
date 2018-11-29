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

#include "cuboactors.hpp"
#include "c3dobjects.hpp"
#include "vectors.hpp"
#include "cuboboxes.hpp"
#include "cubolevels.hpp"
#include "posteffects.hpp"
#include "game.hpp"
#include "cuboenemies.hpp"


void CuboPlayer::AddActor(int i) {

	for (unsigned int j=0; j<actorids.size(); j++) if (actorids[j]==i) { return ; } actorids.push_back(i);
	g_Game()->GetActorMovement(actorids.back())->SetPlayer(id);
	}

void CuboPlayer::RemoveActor(int i) {
	for (unsigned int j=0; j<actorids.size(); j++) if (actorids[j]==i) { actorids.erase(actorids.begin()+j);  break; }
	if (activeact<0 || activeact>=(int)actorids.size()) { if (actorids.empty()) activeact=0; else activeact=actorids.size()-1; }
	}

void CuboPlayer::CamMove(int newactor) {
	if (newactor!=activeact) {
			lastact=activeact;
			//Prepare the matrices for the camera movement

			if (!InCameraPan()) {
					g_Game()->GetActorMovement(actorids[activeact])->SetCamPos(&old_mo);
					}
			else {
					SetCameraPos(0,&old_mo);
					}

			caminterpolation=0;
			g_Game()->GetActorMovement(actorids[newactor])->SetCamPos(&new_mo);
			Matrix3d *old_rmat=old_mo.getBase();
			Matrix3d *new_rmat=new_mo.getBase();
			Matrix3d new_rmat_viewed_by_old_rmat=old_rmat->transpose();
			new_rmat_viewed_by_old_rmat=new_rmat_viewed_by_old_rmat*(*new_rmat);
			//cout << new_rmat_viewed_by_old_rmat.toString() << endl;
			pyr=new_rmat_viewed_by_old_rmat.getPitchYawRoll();
//   cout << pyr.toString() << endl;
			activeact=newactor;
			}
	}

int CuboPlayer::NextActor() {
	int newactor=activeact;
	do { newactor=(newactor+1)%(actorids.size());  }
	while ( !(g_Game()->GetActorMovement(actorids[newactor])->IsSelectable()) && newactor!=activeact);
	CamMove(newactor);
	return GetActiveActor();
	}

int CuboPlayer::SelectActor(int actorind) {
	int found=-1;
	for (unsigned int i=0; i<actorids.size(); i++) if (actorids[i]==actorind)  {found=i; break;}
	if (found==-1) { return -1; }
	if (activeact==found) { return 0; }
	CamMove(found); return 1;
	}

void CuboPlayer::SetCameraPos(float elapsed,MatrixObject *cam) {

//only call if in movement!
	float l=caminterpolation+elapsed*camspeed;


	if (l>1) { l=1; }
	auto pos=old_mo.getPos()*(1.0-l); auto npos=new_mo.getPos()*l;
	pos=pos+npos;
	cam->setPos(pos);

	Matrix3d *old_rmat=old_mo.getBase();
	Matrix3d pyrmat; pyrmat.setPitchYawRoll(pyr*l);
	pyrmat=(*old_rmat)*pyrmat;
	cam->SetBasis(pyrmat.getCol(0),pyrmat.getCol(1),pyrmat.getCol(2));

	caminterpolation=l;
	}


void BaseIdentity(TBasis *res) {
	(*res)[0].xyz(1,0,0);
	(*res)[1].xyz(0,1,0);
	(*res)[2].xyz(0,0,1);
	}

void BaseCp(TBasis *from,TBasis *to) {
	for (int i=0; i<3; i++) { (*to)[i]=(*from)[i]; }
	}

void BaseDiscretize(TBasis *b) {
	for (int i=0; i<3; i++) {
			float absmax=(*b)[i].x*(*b)[i].x; int absmaxind=0; float sign=((*b)[i].x>=0 ? 1 : -1);
			if ((*b)[i].y*(*b)[i].y>absmax) {absmax=(*b)[i].y*(*b)[i].y; absmaxind=1; sign=((*b)[i].y>=0 ? 1 : -1);}
			if ((*b)[i].z*(*b)[i].z>absmax) {absmax=(*b)[i].z*(*b)[i].z; absmaxind=2; sign=((*b)[i].z>=0 ? 1 : -1);}
			(*b)[i].xyz(sign*(absmaxind==0 ? 1 : 0),sign*(absmaxind==1 ? 1 : 0),sign*(absmaxind==2 ? 1 : 0));

			}
	}


void BaseInterpolate(TBasis *from1,TBasis *from2, float t,TBasis *to) {
	float t1=cos(t*M_PI/2.0);
	float t2=sin(t*M_PI/2.0);
	for (int i=0; i<3; i++) {
			auto v1=(*from1)[i];
			auto v2=(*from2)[i];
			v1=v1*t1;
			v2=v2*t2;
			(*to)[i]=v1+v2;
			(*to)[i].normalize();
			}
	}

std::string TCuboMovement::GetEditorInfo(std::string what,std::string def) {
	return (g_Game()->GetActorDefs()->GetDefPtr(defindex)->Call_GetEditorInfo(what,def));
	}

TCuboMovement::~TCuboMovement() {
	if (player>=0) { g_Game()->GetPlayer(player)->RemoveActor(this->id); }
	}

Vector3d TCuboMovement::AtBlockPos(CuboBlock *b,int s) {
	Vector3d res;
	res=b->GetPos();
	Vector3d offs=s_CuboNormals[s];
	res=res+offs*(CUBO_SCALE+GetRadius()+GetGroundOffset());
	return res;
	}

int TCuboMovement::DirVectToSide(Vector3d dirvect) {
	auto n = dirvect;
	n.normalize();
	for (int i=0; i<6; i++) {
			Vector3d kn;
			kn=s_CuboNormals[i];
			if ((kn*n)>0.5) { return i; }
			}
	return -1;
	}

void TCuboMovement::Init(CuboLevel *level) {
//id=0; //Only one player for now
	lvl=level;
	if (!startposset) {
			BaseIdentity(&base);
			BaseIdentity(&oldbase);
			BaseIdentity(&newbase);
			onBlock=lvl->GetBlock(0);
			BlockSideUnderMe=0;
			BlockUnderMe=onBlock;
			onSide=0;
//   prevBlock=onBlock;
			prevSide=0;
			}
	startposset=0;
	tempmovespeedmultiply=1;
	falltime=0;
	resetview=0;

	movementcheck=1;
	pos=AtBlockPos(onBlock,onSide);
	newpos=pos; oldpos=pos;
	moveInterpolate=0;
	inmove=CUBO_MOVE_NONE;
	lastmove=inmove;
	inAir=0;
	lookpos=0;
	lateforwardjumptime=0;
	forwardpresstime=1000; //Just for initing
	inhighjump=0;

	}

void TCuboMovement::SetStartPos(CuboBlockSide *sside,int srot) {

	onBlock=sside->GetBlock();
	BlockUnderMe=onBlock;
//prevBlock=onBlock;
	falltime=0;
	laststaticside=sside->GetID();
	onSide=sside->GetID() % 6;
	BlockSideUnderMe=onSide;
	prevSide=onSide;
	pos=AtBlockPos(onBlock,onSide);
	tempmovespeedmultiply=1;
	newpos=pos; oldpos=pos;
	moveInterpolate=0;
	inmove=CUBO_MOVE_NONE;
	inAir=0;
//Build the base
	base[CUBO_UPV]=sside->GetNormal();

	startrot=srot % 4;
	switch (startrot) {
			case 0:
				base[CUBO_DIRV]=sside->GetTangent()*(-1);
				base[CUBO_SIDEV]=base[CUBO_UPV].cross(base[CUBO_DIRV]);
				break;
			case 1:
				base[CUBO_SIDEV]=sside->GetTangent();
				base[CUBO_DIRV]=base[CUBO_SIDEV].cross(base[CUBO_UPV]);
				break;
			case 2:
				base[CUBO_DIRV]=sside->GetTangent()*(1.0);
				base[CUBO_SIDEV]=base[CUBO_UPV].cross(base[CUBO_DIRV]);
				break;
			case 3:
				base[CUBO_SIDEV]=sside->GetTangent()*(-1.0);
				base[CUBO_DIRV]=base[CUBO_SIDEV].cross(base[CUBO_UPV]);
				break;
			}

	BaseCp(&base,&newbase);
	BaseCp(&base,&oldbase);
	startposset=1;
	}

void TCuboMovement::SetType(int mid,std::string name) {

	id=mid;
	defindex=g_Game()->GetActorDefs()->AddDef(name);
//And call the constructor

	g_Game()->GetActorDefs()->GetDefPtr(defindex)->Call_Constructor(id);
	}

void TCuboMovement::Call_ChangeMove() {
	std::string movestr=s_CuboMoveStringsMove[inmove];
	g_Game()->GetActorDefs()->GetDefPtr(defindex)->Call_ChangeMove(id,movestr);
	}

void TCuboMovement::SpecialRender(std::string nam,int defrender) {
	if (  g_Game()->GetActorDefs()->GetDefPtr(defindex)->Call_SpecialRender(nam,id)) {}
	else {
			if (defrender==1) { Render(); }
			else if (defrender==0 && g_PostEffect()) { g_PostEffect()->CallDefaultSpecialRender(nam,"actor",id); }
			}
	}

void TCuboMovement::Render() {
	if ((!IsPlayer())  && !(g_Game()->GetCam()->SphereInFrustum(pos,2*CUBO_SCALE))) {
			return ;
			}
	g_Game()->GetActorDefs()->GetDefPtr(defindex)->Call_Render(id);
	}

void TCuboMovement::DistRender() {
	g_Game()->GetActorDefs()->GetDefPtr(defindex)->Call_DistRender(id);
	}

void TCuboMovement::PostLevelThink() {
	if ((onBlock) && (this->inmove!=CUBO_MOVE_JUMP_AHEAD) && (this->inmove!=CUBO_MOVE_JUMP_FAR)) {
			if (onBlock->Moving()) {
					Vector3d moffs=onBlock->GetMovementDelta();
					pos=pos+moffs;
					oldpos=oldpos+moffs;
					newpos=newpos+moffs;
					}
			}
	g_Game()->GetActorDefs()->GetDefPtr(defindex)->Call_PostThink(id);
	}

void TCuboMovement::Think() {
	g_Game()->GetActorDefs()->GetDefPtr(defindex)->Call_Think(id);
	}

void TCuboMovement::SendKey(int key,int down,int toggle) {
	g_Game()->GetActorDefs()->GetDefPtr(defindex)->SendKey(id,key,down,toggle);
	}

void TCuboMovement::SendJoyButton(int stick,int button,int dir,int down,int toggle) {
	g_Game()->GetActorDefs()->GetDefPtr(defindex)->SendJoyButton(id,stick,button,dir,down,toggle);
	}

int TCuboMovement::GetJumpDistBlocks() {
	if (!inAir) { return 0; }
	return (int)((j_desdist-j_dist)/(2*CUBO_SCALE));
	}

void TCuboMovement::SetJumpDistBlocks(int d) {
	if (!inAir) { return; }
	j_desdist=(((int)((j_dist+d*2*CUBO_SCALE)/(2*CUBO_SCALE))))*2*CUBO_SCALE;
	}

CuboBlock *TCuboMovement::GetRelBlock(CuboBlock *b,Vector3d dir) {
	return GetRelBlock(b,DirVectToSide(dir));
	}
CuboBlock *TCuboMovement::GetRelBlock(CuboBlock *b,int side) {
	if (!b) { return NULL; }
	return b->GetNext(side);
	}

int TCuboMovement::MayMove(int typ) {
	if (!BlockUnderMe) { return 1; }
	std::string typstr=s_CuboMoveStringsDir[typ];
	return BlockUnderMe->GetBlockSide(BlockSideUnderMe)->Call_MayMove(id,typstr);
	}

void TCuboMovement::MoveForward() {
	if (inmove==CUBO_MOVE_NONE) {
			forwardpresstime=0;

			}
	if (inmove) {
			if (inmove==CUBO_MOVE_JUMP_UP) {
					//   ostringstream oss; oss << "Forward pressed in air!  " << lateforwardjumptime;
					// coutlog(oss.str());
					if (lateforwardjumptime>=0) {
							inmove=CUBO_MOVE_JUMP_AHEAD; jumptype="jumpforward"; Call_ChangeMove();
							j_desdist=distance_normjump*2*CUBO_SCALE;
							//   j_heightoverground=0;
							// j_flytime=0;
							j_dist=0;
							return ;
							}
					else { return; }
					}
			else {
					//  if ((inmove!=CUBO_MOVE_ROTATE_LEFT && inmove!=CUBO_MOVE_ROTATE_RIGHT) || moveInterpolate<0.95 )
					return;
					}
			}
	if (movementcheck && (!MayMove(CUBO_MOVE_AHEAD))) { return; }
	//First check, if we can roll "up"
	Vector3d checkvect;
	checkvect=onBlock->GetPos();
	Vector3d rv=base[CUBO_DIRV]+base[CUBO_UPV];
	checkvect=checkvect+rv*(2*CUBO_SCALE);
	CuboBlock *b;
	b=lvl->GetBlockAtPos(checkvect);
	if (b && (b->Blocking())) { //ROLL UPWARDS!
			// prevBlock=onBlock;
			onBlock=b;
			//cout << "DEBUG UPW " << onSide << endl;
			prevSide=laststaticside;
			BaseCp(&base,&oldbase);
			newbase[CUBO_SIDEV]=base[CUBO_SIDEV];
			newbase[CUBO_UPV]=-base[CUBO_DIRV];
			newbase[CUBO_DIRV]=base[CUBO_UPV];
			onSide=DirVectToSide(newbase[CUBO_UPV]);
			oldpos=pos;
			moveInterpolate=0;
			inmove=CUBO_MOVE_UP;
			Call_ChangeMove();
			}
	else { //May be we can roll simply forward
			b=GetRelBlock(onBlock,base[CUBO_DIRV]);
			if (b) { //Yes: We can... Obama greetz!

					BaseCp(&base,&oldbase);

					BaseCp(&base,&newbase);
					//BaseDiscretize(&newbase);


					onBlock=b;
					oldpos=pos;

					prevSide=laststaticside;
					moveInterpolate=0;
					inmove=CUBO_MOVE_AHEAD;
					Call_ChangeMove();
					}
			else { //Can we roll downwards?
					//We may not have left or right neighbors
					b=GetRelBlock(onBlock,base[CUBO_SIDEV]);
					CuboBlock *b2=GetRelBlock(onBlock,-base[CUBO_SIDEV]);
					if (b || b2 || (movementcheck && (!MayMove(CUBO_MOVE_DOWN)))) {
							//Ok, check if we can slide down
							if (BlockUnderMe && (BlockSideUnderMe>=0) && (BlockSideUnderMe<=5)) {
									if (BlockUnderMe->GetBlockSide(BlockSideUnderMe)->Call_MaySlideDown(id) || movementcheck==0) {


											BaseCp(&base,&oldbase);
											BaseCp(&base,&newbase);

											onBlock=NULL;
											oldpos=pos;

											prevSide=laststaticside;
											moveInterpolate=0;
											inmove=CUBO_MOVE_SLIDE_DOWN;
											Call_ChangeMove();
											newpos=base[CUBO_DIRV]*(2*CUBO_SCALE);
											newpos=oldpos+newpos;
											inAir=0;

											}
									}
							return; //No Move possible
							}
					//Ok, we will stay on this block, but rolling around it
					// prevBlock=onBlock;
					prevSide=laststaticside;
					BaseCp(&base,&oldbase);
					newbase[CUBO_SIDEV]=base[CUBO_SIDEV];
					newbase[CUBO_UPV]=base[CUBO_DIRV];
					newbase[CUBO_DIRV]=-base[CUBO_UPV];
					onSide=DirVectToSide(newbase[CUBO_UPV]);
					oldpos=pos;
					moveInterpolate=0;
					inmove=CUBO_MOVE_DOWN;
					Call_ChangeMove();
					}


			}

// base=newbase; ///TODO: intepolate move
	//BaseCp(&newbase,&base);
	newpos=AtBlockPos(onBlock,onSide);
	}



void TCuboMovement::RotateLeft() {
	if (inmove && ((inmove!=CUBO_MOVE_JUMP_UP) || yvel<0)) { return; }
	if (!MayMove(CUBO_MOVE_ROTATE_LEFT)) { return; }

	if (lastmove==CUBO_MOVE_ROTATE_LEFT) {
			if (rotatestoptime>0) { return ; }
			}

	BaseCp(&base,&oldbase);
	newbase[CUBO_SIDEV]=-base[CUBO_DIRV];
	newbase[CUBO_UPV]=base[CUBO_UPV];
	newbase[CUBO_DIRV]=base[CUBO_SIDEV];


	if (!inmove) {
			oldpos=pos;
			newpos=pos;

			j_desdist=0;

			j_heightoverground=0;
			j_flytime=0;
			j_dist=0;
			}
	moveInterpolate=0;
	inmove=CUBO_MOVE_ROTATE_LEFT;
	rotatestoptime=ROTATE_STOP_TIME;
	Call_ChangeMove();
	BaseCp(&newbase,&base); ///TODO: intepolate move
	}

void TCuboMovement::RotateRight() {
	if (inmove && ((inmove!=CUBO_MOVE_JUMP_UP) || yvel<0)) { return; }
	if (!MayMove(CUBO_MOVE_ROTATE_RIGHT)) { return; }

	if (lastmove==CUBO_MOVE_ROTATE_RIGHT) {
			if (rotatestoptime>0) { return ; }
			}

	BaseCp(&base,&oldbase);


	newbase[CUBO_SIDEV]=base[CUBO_DIRV];
	newbase[CUBO_UPV]=base[CUBO_UPV];
	newbase[CUBO_DIRV]=-base[CUBO_SIDEV];

	if (!inmove) {
			oldpos=pos;
			newpos=pos;

			j_desdist=0;

			j_heightoverground=0;
			j_flytime=0;
			j_dist=0;
			}
	moveInterpolate=0;
	inmove=CUBO_MOVE_ROTATE_RIGHT;
	rotatestoptime=ROTATE_STOP_TIME;
	Call_ChangeMove();
	BaseCp(&newbase,&base); ///TODO: intepolate move
	}


void TCuboMovement::AirMove(float elapsed) {

//Apply gravity
//elapsed*=timemultiplicator;
	j_flytime+=elapsed;
	yvel-=elapsed*CUBO_GRAVITY_G;
	if (yvel<-CUBO_MAX_FALLSPEED) { yvel=-CUBO_MAX_FALLSPEED; }


//Apply yspeed
//Vector3d yspeed=base[CUBO_UPV];
//yspeed=yspeed*(yvel*elapsed);

	j_heightoverground+=yvel*elapsed;
//pos=pos+yspeed;

	//Recalc the d_vel
	if (j_heightoverground>0) {
			float vdurchg=yvel/(CUBO_GRAVITY_G);
			float tres=vdurchg+sqrt(vdurchg*vdurchg+2*j_heightoverground/(CUBO_GRAVITY_G));

			dvel=(j_desdist-j_dist)/(tres+elapsed);
			if (inhighjump) {
					dvel*=1.75;
					}
			}
	else {
			dvel=0;
			}

	if (j_heightoverground<=0) {
			dvel=0;
			j_dist=j_desdist;
			/*Vector3d temp=base[CUBO_DIRV];
			temp=temp*j_dist;
			pos=pos+oldpos;*/
			}
	else {
//   yspeed=base[CUBO_DIRV];
//  yspeed=yspeed*(dvel*elapsed);
			j_dist+=dvel*elapsed;
//  pos=pos+yspeed;
			}

	Vector3d upoffs,diroffs;
	upoffs=base[CUBO_UPV];
	upoffs=upoffs*j_heightoverground;
	diroffs=base[CUBO_DIRV];
	diroffs=diroffs*j_dist;

	pos=oldpos+diroffs;
	pos=pos+upoffs;

//Get the travelled distance
//Vector3d dist=pos-oldpos;
//float dirdist=dist*base[CUBO_DIRV];
	/*if (dirdist>1.5*2*CUBO_SCALE)
	{
	    if (dirdist>2*2*CUBO_SCALE)
	    dvel=0; //move not further than this, todo: Make it continouus
	    else {
	      //Fit it... slow it down to hit the ground in the desired center
	      //float restdist=2*2*CUBO_SCALE-dirdist;
	      ///TODO: Calc the best fitting speed
	    }
	}*/
//For forward moving we need to add the dir_speed
//Check, if we hit the ground
	CuboBlock *b;
	Vector3d checkpos;

	if ((yvel<0)) {
			falltime+=elapsed;
			if (falltime>CUBO_FALL_TILL_LOOKDOWN) { LookDown(); }
			checkpos=base[CUBO_UPV]*(-1*GetRadius());
			checkpos=pos+checkpos;
			b=lvl->GetBlockAtPos(checkpos);
			if (b && (b->Blocking()) && lvl->GetCollisionChecksActive()) {
					inhighjump=0;
					if (inmove==CUBO_MOVE_ROTATE_LEFT || inmove==CUBO_MOVE_ROTATE_RIGHT) {
							BaseCp(&newbase,&base);
							BaseCp(&newbase,&oldbase);
							}
					inAir=0;
					falltime=0;

					tempmovespeedmultiply=1;
					inmove=CUBO_MOVE_NONE;
					Call_ChangeMove();
					onBlock=b; //Should be itself, but ok
					onSide=DirVectToSide(base[CUBO_UPV]);
					Vector3d ps;
					Vector3d posbackup=pos;
					pos=AtBlockPos(onBlock,onSide);
					// prevBlock=onBlock;
					//Vector3d ps;
					ps=base[CUBO_UPV]*(1.1*(CUBO_GROUND_DIST+CUBO_GROUND_DIST_OFFS));
					ps=pos-ps;
					BlockUnderMe=lvl->GetBlockAtPos(ps);
					if (BlockUnderMe && (BlockUnderMe->Blocking())) {
							Vector3d relpos;
							relpos=BlockUnderMe->GetPos();
							relpos=pos-relpos;
							BlockSideUnderMe=DirVectToSide(relpos);
							if (!g_Game()->GetActorDefs()->GetDefPtr(defindex)->Call_CheckLandingOnSide(id,BlockUnderMe->GetBlockSide(BlockSideUnderMe)->GetID())) {
									BlockUnderMe=NULL;
									inAir=1;
									onBlock=NULL;
									onSide=-1;
									BlockSideUnderMe=-1;
									pos=posbackup;

									return;

									}
							if (lvl->GetCollisionChecksActive()) {
									BlockUnderMe->Call_OnBlockEvent(id);
									g_Game()->GetActorDefs()->GetDefPtr(defindex)->Call_Event(id,"hitground");
									}
							if ((BlockSideUnderMe>=0) && (BlockSideUnderMe<=5) && (lvl->GetCollisionChecksActive() )) {
									BlockUnderMe->GetBlockSide(BlockSideUnderMe)->Call_OnSideEvent(id);
									}
							laststaticside=BlockUnderMe->GetBlockSide(BlockSideUnderMe)->GetID();
							prevSide=laststaticside;
							}
					else { BlockUnderMe=NULL; }
					}
			}
	else if ((yvel>0)) {
			checkpos=base[CUBO_UPV]*(1*GetRadius());
			checkpos=pos+checkpos;
			b=lvl->GetBlockAtPos(checkpos);
			if (b && (b->Blocking())) {

					inhighjump=0;
					yvel=-yvel*0.7;
					CuboBlockSide* hs=b->GetBlockSide(s_CuboOpposingDir[DirVectToSide(base[CUBO_UPV])]);
					Vector3d hn=hs->GetNormal();
					Vector3d hpd=hs->GetMidpoint();
					if (j_desdist>0) {
							hpd=oldpos-hpd;
							j_desdist=hpd*hn;
							}
					g_Game()->GetActorDefs()->GetDefPtr(defindex)->Call_Event(id,"hitup");

					}
			}

//Check collision with front side

	if (dvel>0) {

			checkpos=base[CUBO_DIRV]*(1*GetRadius());
			checkpos=pos+checkpos;
			b=lvl->GetBlockAtPos(checkpos);
			if (!b) {
					Vector3d checkpos2=base[CUBO_DIRV]-base[CUBO_UPV]; checkpos2.normalize();
					checkpos2=checkpos2*GetRadius();
					checkpos2=pos+checkpos2;
					b=lvl->GetBlockAtPos(checkpos2);
					}

			if (b && (b->Blocking())) {

					inhighjump=0;
					CuboBlockSide* hs=b->GetBlockSide(s_CuboOpposingDir[DirVectToSide(base[CUBO_DIRV])]);

					Vector3d hn=hs->GetNormal();
					Vector3d hpd=hn*CUBO_SCALE;
					hpd=hpd+hs->GetMidpoint();
					hpd=oldpos-hpd;
					j_desdist=hpd*hn;
					yvel=0;
					g_Game()->GetActorDefs()->GetDefPtr(defindex)->Call_Event(id,"hitfront");
					}
			}


	}


void TCuboMovement::FrontSideRebounce() {
	inhighjump=0;
	Vector3d hn=base[CUBO_DIRV]*(-1);
	Vector3d hpd=hn*(CUBO_SCALE);
	hpd=hpd+pos;
	//hpd=hpd+hs->GetMidpoint();
	hpd=oldpos-hpd;
	j_desdist=hpd*hn;
	//  cout << j_desdist << " set " << endl;
	yvel=0;

	}

float clampinterpolation(float in,float ls,float rs) {
	if (in<ls) { return 0; }
	if (in>rs) { return 1; }
	return (in-ls)/(rs-ls);
	}

void TCuboMovement::SetRotSpeed(float v) {
	CUBO_ROTSPEED=v;
	}

float TCuboMovement::s_MoveSpeed(int move) {
	double mvs[]= {0,CUBO_MOVESPEED,CUBO_ROTUPSPEED,CUBO_ROTDOWNSPEED,CUBO_ROTSPEED,CUBO_ROTSPEED,CUBO_MOVESPEED,CUBO_MOVESPEED,CUBO_MOVESPEED,CUBO_MOVESPEED,CUBO_MOVESPEED};
	return mvs[move];
	}

void TCuboMovement::FinalizeMove() {
	if (falltime<=CUBO_FALL_TILL_LOOKDOWN) {
			lookdir=0;
			}
	}

void TCuboMovement::InterpolateMove(double elapsed) {
	elapsed*=timemultiplicator*tempmovespeedmultiply;
	// cout <<"Time multi" << timemultiplicator << endl;



	if (inmove!=CUBO_MOVE_NONE) { lastmove=inmove; }
	if (!lookdir) {
			if (lookpos>0) {
					if (resetview) { lookpos-=elapsed/tempmovespeedmultiply*CUBO_LOOK_RELAX_SPEED; }
					if (lookpos<0) { lookpos=0; resetview=0;}
					}
			else if (lookpos<0) {
					if (resetview) { lookpos+=elapsed/tempmovespeedmultiply*CUBO_LOOK_RELAX_SPEED; }
					if (lookpos>0) { lookpos=0; resetview=0;}
					}
			}
	else {
			lookpos+=elapsed*lookdir/tempmovespeedmultiply*CUBO_LOOK_SPEED;
			if (lookpos*lookdir>1) { lookpos=lookdir; }
			}


	if (inmove==CUBO_MOVE_GRAVITY_CHANGE) {
			double ipos;
			if (!grav180rot) {
					moveInterpolate+=elapsed*gravitychangespeed;
					ipos=moveInterpolate;
					}
			else {
					moveInterpolate+=elapsed*gravitychangespeed*(1.5);
					ipos=moveInterpolate*0.5;
					if (grav180rot==1 && moveInterpolate>=1.0) {
							Vector3d tupv=newbase[CUBO_UPV]; //Might be wrong!
							Vector3d tsidev=newbase[CUBO_SIDEV];

							BaseCp(&base,&oldbase);
							newbase[CUBO_UPV]=-tsidev;
							newbase[CUBO_SIDEV]=tupv;
							grav180rot=2;
							moveInterpolate=0;
							tempmovespeedmultiply=1;
							}
					else if (grav180rot==2) { ipos+=0.5; }

					}
			BaseInterpolate(&oldbase,&newbase,moveInterpolate,&base);
			Vector3d p1=oldpos*(1.0-ipos);
			Vector3d p2=newpos*ipos;
			pos=p1+p2;
			if (moveInterpolate>=1.0) {
					BaseCp(&newbase,&base);
					BaseCp(&newbase,&oldbase);
					inmove=CUBO_MOVE_FALLING;
					tempmovespeedmultiply=1;
					pos=newpos;
					oldpos=pos;
					}
			movementcheck=1;
			return;
			}

	forwardpresstime+=abs(elapsed);
	lateforwardjumptime-=abs(elapsed);
	if (inmove!=CUBO_MOVE_ROTATE_LEFT && inmove!=CUBO_MOVE_ROTATE_RIGHT) { rotatestoptime-=elapsed/tempmovespeedmultiply; }



	Vector3d ps;
	ps=base[CUBO_UPV]*(1.05*(CUBO_GROUND_DIST+CUBO_GROUND_DIST_OFFS));
	ps=pos-ps;
	BlockUnderMe=lvl->GetBlockAtPos(ps);
	if (BlockUnderMe) if ((!BlockUnderMe->Blocking()) && lvl->GetCollisionChecksActive()) { BlockUnderMe=NULL; }
	//Call the blocks OnBlockScript

	if (inAir) {
			BlockUnderMe=NULL;
			}
	if (BlockUnderMe) {

			//Call the Sides OnSideScript
			BlockUnderMe->Call_OnBlockEvent(id);
			//Get the relative vector
			Vector3d relpos;
			relpos=BlockUnderMe->GetPos();
			relpos=pos-relpos;
			BlockSideUnderMe=DirVectToSide(relpos);
			if ((BlockSideUnderMe>=0) && (BlockSideUnderMe<=5) && (lvl->GetCollisionChecksActive()) ) {
					BlockUnderMe->GetBlockSide(BlockSideUnderMe)->Call_OnSideEvent(id);
					}
			laststaticside=BlockUnderMe->GetBlockSide(BlockSideUnderMe)->GetID();

			}


	if ((inmove==CUBO_MOVE_NONE) && (!BlockUnderMe) ) {


			onBlock=NULL;
			BlockSideUnderMe=0;
			BlockUnderMe=onBlock;
			j_desdist=0;
			oldpos=pos;
			newpos=pos;

			j_heightoverground=0;
			j_flytime=0;
			j_dist=0;

			inmove=CUBO_MOVE_FALLING;
			Call_ChangeMove();
			moveInterpolate=0;
			movementcheck=1;
			inAir=1;
			dvel=0;
			yvel=0;

			}
	/*else if ((!BlockUnderMe) && (!inAir))
	{

	    inmove=CUBO_MOVE_FALLING;
	    Call_ChangeMove();
	    moveInterpolate=0;
	    pos=newpos;
	    onBlock=NULL;
	    BaseCp(&newbase,&base);
	    BaseCp(&newbase,&oldbase);
	    //BlockSideUnderMe=-1;
	    //BlockUnderMe
	    inAir=1;
	    dvel=0;
	    yvel=0;

	}*/


	if (!inmove) { movementcheck=1; return; }
	//moveInterpolate+=elapsed*s_MoveSpeed[inmove];
	moveInterpolate+=elapsed*s_MoveSpeed(inmove);





	if (inAir) {
			BlockUnderMe=NULL;
			AirMove(elapsed);
			//BaseCp(&newbase,&base);
			if (inmove==CUBO_MOVE_ROTATE_LEFT || inmove==CUBO_MOVE_ROTATE_RIGHT) {
					BaseInterpolate(&oldbase,&newbase,moveInterpolate,&base);
					if (moveInterpolate>=1) {
							BaseCp(&newbase,&base);
							BaseCp(&newbase,&oldbase);
							inmove=CUBO_MOVE_JUMP_UP;
							}
					}
			movementcheck=1;
			return;
			}




	if (!inAir) { falltime=0; }

	if (moveInterpolate>1 || moveInterpolate<0) {
			movementcheck=1;
			if (moveInterpolate>0) {
					BaseCp(&newbase,&base);
					BaseCp(&newbase,&oldbase);
					pos=newpos;
					oldpos=newpos;
					}
			else {

					BaseCp(&oldbase,&newbase);
					BaseCp(&newbase,&base);
					//pos=newpos;
					pos=oldpos;
					newpos=pos;
					/*onSide= DirVectToSide(oldbase[CUBO_UPV]);;
					tempmovespeedmultiply=1;
					inmove=CUBO_MOVE_NONE;
					BlockSideUnderMe=laststaticside;
					  moveInterpolate=0;*/

					}
			ps=base[CUBO_UPV]*(1.01*(CUBO_GROUND_DIST+CUBO_GROUND_DIST_OFFS));
			ps=pos-ps;
			BlockUnderMe=lvl->GetBlockAtPos(ps);
			if (BlockUnderMe) if (!BlockUnderMe->Blocking()) { BlockUnderMe=NULL;  }
			onBlock=BlockUnderMe;
			onSide= DirVectToSide(base[CUBO_UPV]);;
			if ((inmove==CUBO_MOVE_SLIDE_DOWN) || (!BlockUnderMe) ) {
					tempmovespeedmultiply=1;
					inmove=CUBO_MOVE_FALLING;
					// inmove=0;
					Call_ChangeMove();
					moveInterpolate=0;
					j_desdist=0;
					j_heightoverground=0;
					j_flytime=0;
					j_dist=0;
					inAir=1;
					dvel=0;
					yvel=0;
					pos=newpos;
					onBlock=NULL;
					// prevBlock=NULL;

					}
			else  { inmove=CUBO_MOVE_NONE; tempmovespeedmultiply=1; Call_ChangeMove(); }



			return;
			}
	//Ok.. now the interpolation work has to be done
	float bt=moveInterpolate; //Interpolation for the Base.. here: Time Identity
	float pt=moveInterpolate;

	//First try a linear interpolation for the pos
	if ((inmove==CUBO_MOVE_UP) || (inmove==CUBO_MOVE_DOWN)) {
			Vector3d p1=oldpos;
			Vector3d diff=newpos-oldpos;
			float proj1=diff*oldbase[CUBO_DIRV];
			float proj2=diff*newbase[CUBO_DIRV];
			if (pt<0.5) {
					p1=oldbase[CUBO_DIRV]*(proj1*2*pt);
					pos=oldpos+p1;
					}
			else {
					p1=oldbase[CUBO_DIRV]*(proj1);
					pos=oldpos+p1;
					pt-=0.5;
					p1=newbase[CUBO_DIRV]*(proj2*2*pt);
					pos=pos+p1;
					}
			float btborder=0.15;
			if (inmove==CUBO_MOVE_DOWN) { bt=clampinterpolation(bt,btborder,1-btborder); }
			}
	else {
			if (!inAir) {
					Vector3d p1=oldpos*(1-pt);
					Vector3d p2=newpos*pt;
					pos=p1+p2;
					}
			}


	BaseInterpolate(&oldbase,&newbase,bt,&base);
	}

std::string TCuboMovement::GetType() {
	return g_Game()->GetActorDefs()->GetDefPtr(this->defindex)->GetName();
	}

int TCuboMovement::GetOnSideID() {
	if (BlockUnderMe) {
			if ((BlockSideUnderMe>=0) && (BlockSideUnderMe<=5) && (!inAir)) {
					return BlockUnderMe->GetBlockSide(BlockSideUnderMe)->GetID();
					}
			else { return -1; }
			}
	else { return -1; }
	}

int TCuboMovement::GetPrevOnSideID() {

	return prevSide;
	}


int TCuboMovement::GetLastOnSideID() {
	//cout << "BlockSideUnderMe: " << laststaticside <<  " " << endl;
	return laststaticside;
	}

int TCuboMovement::TraceOnSideID() {
	if (!inAir) {

			TraceResult tr;
			Vector3d d=base[CUBO_UPV]*(-1.0);
			tr=g_Game()->GetLevel()->TraceLine(pos,d,1);

			if (!tr.hit) { return -1; }
			else { return tr.side+tr.block*6; }
			}
	else { return -1; }
	}


void TCuboMovement::SetCamParams(std::string what,Vector3d params) {
	if (what=="normal") { camfloats=params; }
	else if (what=="lookup") { lookupfloats=params; }
	else if (what=="lookdown") { lookdownfloats=params; }
	}

void TCuboMovement::SetCamZRotation(float zr,int mirror) {
	cammirror=mirror;
	camzrot=zr;
	}

void TCuboMovement::SetCamPos(MatrixObject *cam) {
//Get the Position
	Vector3d res;
	res=pos;
//float factor=2.5;
//float factor=3;
	/* float dist=CUBO_SCALE*factor;
	float height=CUBO_SCALE*factor/3.0;
	float lookheight=CUBO_SCALE*factor/4.0; */


	float lookup=0;
	if (lookpos>0) { lookup=lookpos; }
	float lookdown=0;
	if (lookpos<0) { lookdown=-lookpos; }

	float f=1-lookup-lookdown;
	Vector3d cp=camfloats*f;
	Vector3d add1=lookdownfloats*lookdown;
	Vector3d add2=lookupfloats*lookup;
	add1=add1+add2;
	cp=cp+add1;

	float dist=CUBO_SCALE*cp.x;
	float height=CUBO_SCALE*cp.y;
	float lookheight=CUBO_SCALE*cp.z;

	Vector3d offs;
	offs=base[CUBO_DIRV]*(dist);
	res=res-offs;
	offs=base[CUBO_UPV]*(height);
	res=res+offs;
	cam->setPos(res);
//Now look at this point:
	res=pos;
	offs=base[CUBO_UPV]*(lookheight);
	res=res+offs;
	cam->lookAtZWithWorldUp(res,base[CUBO_UPV]);
	cam->rotateDir(camzrot);
	cam->SetMirror(cammirror);
	}

void TCuboMovement::Rebounce(TCuboMovement *other) {
	jumptype="farjump";
	inmove=CUBO_MOVE_JUMP_FAR;
	Call_ChangeMove();
	j_desdist=distance_farjump*2*CUBO_SCALE;
	j_flytime=0;
	falltime=0;
	j_dist=0; ///TODO: it is not always really zero! We can be walked forward a bit... get it

	dvel=(j_desdist-j_dist)*CUBO_GRAVITY_G/(2.0*(float)CUBO_JUMP_UPVEL);
	inAir=1;
	onSide=prevSide;
	//prevSide=onSide;
	Vector3d diff=(this->GetPos()-other->GetPos());
	double proj=diff*this->GetUp();
	double h=other->GetRadius()-this->GetRadius()+proj;
	double disc=CUBO_FARJUMP_UPVEL*CUBO_FARJUMP_UPVEL-2*CUBO_GRAVITY_G*h;
	if (disc<0.1*CUBO_FARJUMP_UPVEL*CUBO_FARJUMP_UPVEL) { disc=0.1*CUBO_FARJUMP_UPVEL*CUBO_FARJUMP_UPVEL; }
	yvel=sqrt(disc);
	j_heightoverground=h;
	//dvel=CUBO_JUMP_DIRVEL;
	BaseCp(&oldbase,&newbase);
	if (inmove==CUBO_MOVE_DOWN) { BaseCp(&oldbase,&base); }
	BaseCp(&oldbase,&base);
	oldpos=pos;
	}

void TCuboMovement::FarJump() {
	if (inAir) { return; }
	inhighjump=0;
	if (!((inmove==CUBO_MOVE_AHEAD) || ( ( (inmove==CUBO_MOVE_NONE) || (inmove==CUBO_MOVE_DOWN) || (inmove==CUBO_MOVE_SLIDE_DOWN) ) && (forwardpresstime<FORWARD_PRESS_TIME)))) { return; }

	if (!MayMove(CUBO_MOVE_JUMP_FAR)) {  return; }

	inmove=CUBO_MOVE_JUMP_AHEAD; jumptype="farjump"; Call_ChangeMove(); //TODO: Add a new movetype for farjumping
	j_desdist=distance_farjump*2*CUBO_SCALE;
	j_heightoverground=0;
	j_flytime=0;
	j_dist=0; ///TODO: it is not always really zero! We can be walked forward a bit... get it
	if (BlockUnderMe && ((BlockSideUnderMe>=0) && (BlockSideUnderMe<=5))) { ///MAYBE THIS FIXES IT
			CuboBlockSide *bs=BlockUnderMe->GetBlockSide(BlockSideUnderMe);
			Vector3d mp=bs->GetMidpoint();
			mp=pos-mp;
			j_dist=mp*base[CUBO_DIRV];
			}
	dvel=(j_desdist-j_dist)*CUBO_GRAVITY_G/(2.0*(float)CUBO_JUMP_UPVEL);
	inAir=1;
	onSide=prevSide;
	//prevSide=onSide;
	yvel=CUBO_FARJUMP_UPVEL;
	//dvel=CUBO_JUMP_DIRVEL;
	BaseCp(&oldbase,&newbase);
	if (inmove==CUBO_MOVE_DOWN) { BaseCp(&oldbase,&base); }
	BaseCp(&oldbase,&base);
	oldpos=pos;

	}


void TCuboMovement::HighJump() {
	if (inAir) { return; }
	if (!((inmove==CUBO_MOVE_AHEAD) || ( ( (inmove==CUBO_MOVE_NONE) || (inmove==CUBO_MOVE_DOWN) || (inmove==CUBO_MOVE_SLIDE_DOWN) ) && (forwardpresstime<FORWARD_PRESS_TIME)))) { return; }
	inmove=CUBO_MOVE_JUMP_AHEAD; jumptype="highjump"; Call_ChangeMove(); //TODO: Add a new movetype for farjumping
	j_desdist=distance_highjump*2*CUBO_SCALE;
	j_heightoverground=0;
	j_flytime=0;
	j_dist=0; ///TODO: it is not always really zero! We can be walked forward a bit... get it
	if (BlockUnderMe && ((BlockSideUnderMe>=0) && (BlockSideUnderMe<=5))) { ///MAYBE THIS FIXES IT
			CuboBlockSide *bs=BlockUnderMe->GetBlockSide(BlockSideUnderMe);
			Vector3d mp=bs->GetMidpoint();
			mp=pos-mp;
			j_dist=mp*base[CUBO_DIRV];
			}
	dvel=(j_desdist-j_dist)*CUBO_GRAVITY_G/(2.0*(float)CUBO_HIGHJUMP_UPVEL);
	inAir=1;
	onSide=prevSide;
	//prevSide=onSide;
	yvel=CUBO_HIGHJUMP_UPVEL;
	//dvel=CUBO_JUMP_DIRVEL;
	BaseCp(&oldbase,&newbase);
	if (inmove==CUBO_MOVE_DOWN) { BaseCp(&oldbase,&base); }
	BaseCp(&oldbase,&base);
	oldpos=pos;
	inhighjump=1;
	}


int TCuboMovement::ChangeGravity(Vector3d newgrav,Vector3d newp,double changespeed,int do_at_same_gravity) {
	if (inmove==CUBO_MOVE_GRAVITY_CHANGE) { return -1; }
	//Test the change

	newgrav.normalize();
	TBasis tb;
	BaseCp(&newbase,&tb);
	grav180rot=0;
	double dot=newgrav*tb[CUBO_UPV];
	if (dot<-0.8) {
			if (do_at_same_gravity==0) {
					return 5;
					}
			}
	else { do_at_same_gravity=0; }

	onBlock=NULL;
	BlockSideUnderMe=0;
	BlockUnderMe=onBlock;
	j_desdist=0;
	oldpos=pos;
	newpos=newp;

	j_heightoverground=0;
	j_flytime=0;
	j_dist=0;
	moveInterpolate=0;
	inAir=1;
	dvel=0;
	yvel=0;
	gravitychangespeed=changespeed;
	if (gravitychangespeed<0) { gravitychangespeed*=-1; }
	else if (gravitychangespeed==0) { gravitychangespeed=0.0001; }

	inmove=CUBO_MOVE_GRAVITY_CHANGE;
	Call_ChangeMove();
	BaseCp(&base,&oldbase);

	int res=0;

	if (do_at_same_gravity!=0) {
			res=5;
			}
	else if (dot*dot<0.1) { //Four cases...
			double rdot=newgrav*tb[CUBO_SIDEV];
			if (rdot>0.8) {   newbase[CUBO_SIDEV]=tb[CUBO_UPV];  newbase[CUBO_UPV]=-tb[CUBO_SIDEV];  newbase[CUBO_DIRV]=tb[CUBO_DIRV]; res=1;} //Left
			else if (rdot<-0.8) {  newbase[CUBO_SIDEV]=-tb[CUBO_UPV];  newbase[CUBO_UPV]=tb[CUBO_SIDEV];  newbase[CUBO_DIRV]=tb[CUBO_DIRV]; res=2;} //Right
			else {
					double ddot=newgrav*tb[CUBO_DIRV];
					if (ddot>0.8) { newbase[CUBO_SIDEV]=tb[CUBO_SIDEV];  newbase[CUBO_UPV]=-tb[CUBO_DIRV];  newbase[CUBO_DIRV]=tb[CUBO_UPV]; res=3;} //Front
					else if (ddot<-0.8) { newbase[CUBO_SIDEV]=tb[CUBO_SIDEV];  newbase[CUBO_UPV]=tb[CUBO_DIRV];  newbase[CUBO_DIRV]=-tb[CUBO_UPV]; res=4;} //Back
					else { return 0;} //Unknown, should not happen
					}

			}
	else { // 180-degrees
			grav180rot=1;

			newbase[CUBO_SIDEV]=tb[CUBO_UPV];  newbase[CUBO_UPV]=-tb[CUBO_SIDEV];  newbase[CUBO_DIRV]=tb[CUBO_DIRV];

			res=0;
			}



	moveInterpolate=0;
	inAir=1;
	return res;
	}

void TCuboMovement::Jump() {
	if (inAir) { return; }
	inhighjump=0;
	//We have to check, if we are moving
	if ((inmove==CUBO_MOVE_AHEAD) || ( ( (inmove==CUBO_MOVE_NONE) || (inmove==CUBO_MOVE_DOWN) || (inmove==CUBO_MOVE_SLIDE_DOWN) ) && (forwardpresstime<FORWARD_PRESS_TIME)))
		//forward jump
			{
			if  (forwardpresstime>FORWARD_PRESS_TIME_JUMP) { return; } //Too late to jump;

			if (!MayMove(CUBO_MOVE_JUMP_AHEAD)) { return; }

			inmove=CUBO_MOVE_JUMP_AHEAD; jumptype="jumpforward"; Call_ChangeMove();
			j_desdist=distance_normjump*2*CUBO_SCALE;
			j_heightoverground=0;
			j_flytime=0;
			j_dist=0; ///TODO: it is not always really zero! We can be walked forward a bit... get it
			if (BlockUnderMe && ((BlockSideUnderMe>=0) && (BlockSideUnderMe<=5))) { ///MAYBE THIS FIXES IT
					CuboBlockSide *bs=BlockUnderMe->GetBlockSide(BlockSideUnderMe);
					Vector3d mp=bs->GetMidpoint();
					mp=pos-mp;
					j_dist=mp*base[CUBO_DIRV];
					}
			dvel=(j_desdist-j_dist)*CUBO_GRAVITY_G/(2.0*(float)CUBO_JUMP_UPVEL);
			inAir=1;
			onSide=prevSide;
			//prevSide=onSide;
			yvel=CUBO_JUMP_UPVEL;
			//dvel=CUBO_JUMP_DIRVEL;
			BaseCp(&oldbase,&newbase);
			if (inmove==CUBO_MOVE_DOWN) { BaseCp(&oldbase,&base); }
			BaseCp(&oldbase,&base);
			oldpos=pos;
			}
	else if (inmove==CUBO_MOVE_NONE) {
			if (!MayMove(CUBO_MOVE_JUMP_UP)) { return; }

			if (forwardpresstime!=1000)
				if (MayMove(CUBO_MOVE_JUMP_AHEAD)) {lateforwardjumptime=LATE_FORWARD_JUMP_TIME;}


			inmove=CUBO_MOVE_JUMP_UP; Call_ChangeMove();
			inAir=1;
			yvel=CUBO_JUMP_UPVEL;
			j_desdist=0;
			j_heightoverground=0;
			j_dist=0;
			j_flytime=0;
			dvel=0;
			BaseCp(&oldbase,&newbase);
			BaseCp(&oldbase,&base);
			oldpos=pos;
			}

	}


std::string TCuboMovement::GetMoveType() {
	switch (inmove) {
			case CUBO_MOVE_NONE: return "none"; break;
			case CUBO_MOVE_AHEAD: return "forward"; break;
			case CUBO_MOVE_UP: return "up"; break;
			case CUBO_MOVE_DOWN: return "down"; break;
			case CUBO_MOVE_JUMP_UP: return "jumpup"; break;
			case CUBO_MOVE_SLIDE_DOWN: return "slidedown"; break;
			case CUBO_MOVE_ROTATE_LEFT: return "left"; break;
			case CUBO_MOVE_ROTATE_RIGHT: return "right"; break;
			case CUBO_MOVE_FALLING: return "falling"; break;
			case CUBO_MOVE_GRAVITY_CHANGE: return "changegravity"; break;
			case CUBO_MOVE_JUMP_AHEAD:

				return jumptype;

				break;
			}
	return "unknown";
	}

void TCuboMovement::LookUp() {
	if (lookdir>=0) { lookdir=1; }
	resetview=0;
	}


void TCuboMovement::LookDown() {
// if (lookdir<=0)
	lookdir=-1;
	resetview=0;
	}

void TCuboMovement::LookReset() {
	resetview=1;
	}



void TCuboMovement::JumpUp() {
	forwardpresstime=1000;
	Jump();
	}


void TCuboMovement::CheckEnemyCollision(TCuboMovement* other) {
//Radial check
	Vector3d p1=this->GetPos();
	Vector3d p2=other->GetPos();
	p1=p1-p2;
	float dsqr=p1*p1;
	float rsqr=this->GetRadius()+other->GetRadius();
	rsqr*=rsqr;
	if (dsqr<rsqr) {
			//Call a collision
			g_Game()->GetActorDefs()->GetDefPtr(this->defindex)->Call_ActorCollide(this->GetID(),other->GetID());
			}
	}

void TCuboMovement::CheckPlayerCollision(TCuboMovement* other) {
//Radial check
	Vector3d p1=this->GetPos();
	Vector3d p2=other->GetPos();
	p1=p1-p2;
	float dsqr=p1*p1;
	float rsqr=this->GetRadius()+other->GetRadius();
	rsqr*=rsqr;
	if (dsqr<rsqr) {
			//Call a collision
			g_Game()->GetActorDefs()->GetDefPtr(this->defindex)->Call_ActorCollidePlayer(this->GetID(),other->GetID());
			}
	}



///////////////////////////////////////////////////////////////////


/*
int ActorDefServer::AddEDef(std::string name) {
	int def=GetDef(name,0);
	if (def>-1) { return def; } //Have it already
	defs.push_back(new EnemyDef());
	defs.back()->SetName(name);
	def=defs.size()-1;
	defs[def]->LoadDef();
	defs[def]->SetID(def);
	return def;
	}

int ActorDefServer::GetDef(std::string name,int forplayer) {
	for (unsigned int i=0; i<defs.size(); i++) if ((defs[i]->GetName()==name) && (forplayer==defs[i]->IsPlayer())) { return (i); }
	return -1;
	}
*/

int ActorDefServer::AddEDef(std::string name) { // NOTE: there was check for IsPlayer()
	if (name_to_id.count(name)) { return name_to_id.at(name); }
	int id = max_id++;
	name_to_id.emplace(name, id);
	defs.emplace(id, std::make_unique<EnemyDef>());
	auto& obj = defs.at(id);
	obj->SetName(name);
	obj->SetID(id);
	obj->LoadDef();
	return id;
	}


void ActorDef::Call_Constructor(int id) {
	lua.CallVAIfPresent("Constructor", {{id}});
	}

void ActorDef::Call_ActorCollide(int id,int oid) {
	lua.CallVAIfPresent("ActorCollide", {{id,oid}});

	}


void ActorDef::Call_ActorCollidePlayer(int id,int oid) {
	lua.CallVAIfPresent("ActorCollidePlayer", {{id,oid}});

	}

void ActorDef::Call_Render(int id) {
	lua.CallVAIfPresent("Render", {{id}});

	}

int ActorDef::Call_SpecialRender(std::string nam,int index) {
	return lua.CallVAIfPresent("SpecialRender", {{nam,index}});
	}

void ActorDef::Call_DistRender(int id) {
	lua.CallVAIfPresent("DistRender", {{id}});
	}

void ActorDef::Call_PostThink(int id) {
	lua.CallVAIfPresent("PostThink", {{id}});
	}

void ActorDef::Call_Think(int id) {
	lua.CallVAIfPresent("Think", {{id}});
	}

void ActorDef::SendKey(int actor,int key,int down,int toggle) {
	lua.CallVAIfPresent("OnKeyPressed", {{actor,key,down,toggle}});
	}

void ActorDef::SendJoyButton(int actor,int stick,int button,int dir,int down,int toggle) {
	lua.CallVAIfPresent("OnJoyButton", {{actor,stick,button,dir,down,toggle}});
	}


void ActorDef::Call_ChangeMove(int id,std::string newmove) {
	lua.CallVAIfPresent("ChangeMove", {{id, newmove}});
	}

int ActorDef::Call_CheckLandingOnSide(int id,int side) {
	int res = 1;
	lua.CallVAIfPresent("CheckLandingOnSide", {{id,side}}, {{&res}});
	return res;
	}

void ActorDef::Call_Event(int id,std::string ev) {
	lua.CallVAIfPresent("Event", {{id, ev}});
	}

std::string ActorDef::Call_GetEditorInfo(std::string what,std::string std) {
	std::string res = std::move(std);
	lua.CallVAIfPresent("GetEditorInfo", {{what, std}}, {{&res}});
	return res;
	}



//////////////////LUA IMPEMENTATION////////////////////////////////



int ACTOR_IsPlayer(lua_State *state) {
	int a=LUA_GET_INT(state);
	int temp=g_Game()->GetActorMovement(a)->IsPlayer();


	LUA_SET_NUMBER(state, temp);
	return 1;

	}

int ACTOR_GetVar(lua_State *state) {
	int actor=(int)lua_tonumber(state,1);
	lua_remove(state,1);


	g_Game()->GetActorMovement(actor)->GetVarHolder()->GetVar(state);

	return 1;
	}

int ACTOR_SetTimeMultiplicator(lua_State *state) {
	double tm=LUA_GET_DOUBLE(state);
	int actor=LUA_GET_INT(state);

	g_Game()->GetActorMovement(actor)->SetTimeMultiplicator(tm);

	return 0;
	}

int ACTOR_SetVar(lua_State *state) {
	int actor=(int)lua_tonumber(state,1);
	lua_remove(state,1);
	g_Game()->GetActorMovement(actor)->GetVarHolder()->StoreVar(state);

	return 0;
	}

int ACTOR_SendJoyButton(lua_State *state) {
	int toggle=LUA_GET_INT(state);
	int down=LUA_GET_INT(state);
	int dir=LUA_GET_INT(state);
	int button=LUA_GET_INT(state);
	int stick=LUA_GET_INT(state);
	int actor=LUA_GET_INT(state);
	g_Game()->GetActorMovement(actor)->SendJoyButton(stick,button,dir,down,toggle);
	return 0;
	}

int ACTOR_SendKey(lua_State *state) {
	int toggle=LUA_GET_INT(state);
	int down=LUA_GET_INT(state);
	int key=LUA_GET_INT(state);
	int actor=LUA_GET_INT(state);
	g_Game()->GetActorMovement(actor)->SendKey(key,down,toggle);
	return 0;
	}




int ACTOR_New(lua_State *state) {
	std::string defname=LUA_GET_STRING(state);
	defname=g_Game()->GetLevel()->CheckDefExchange(defname,"actor");
	int res=g_Game()->AddActor(defname);
	LUA_SET_NUMBER(state, res);
	return 1;

	}

int ACTOR_Delete(lua_State *state) {
	int ind=LUA_GET_INT(state);
	g_Game()->DeleteActor(ind);
//cout << "Called actor delete" << endl;
	return 0;
	}

int ACTOR_GetStartRotation(lua_State *state) {
	int ind=LUA_GET_INT(state);
	int res=g_Game()->GetActorMovement(ind)->GetStartRotation();
	LUA_SET_NUMBER(state, res);
	return 1;
	}



int ACTOR_GetType(lua_State *state) {
	int ind=LUA_GET_INT(state);
	std::string res=g_Game()->GetActorMovement(ind)->GetType();
	LUA_SET_STRING(state, res);
	return 1;
	}


int ACTOR_GetPos(lua_State *state) {
	int a=LUA_GET_INT(state);
	Vector3d p=g_Game()->GetActorMovement(a)->GetPos();
	LUA_SET_VECTOR3(state, p);
	return 1;

	}

int ACTOR_GetOldPos(lua_State *state) {
	int a=LUA_GET_INT(state);
	Vector3d p=g_Game()->GetActorMovement(a)->GetOldPos();
	LUA_SET_VECTOR3(state, p);
	return 1;
	}


int ACTOR_GetSide(lua_State *state) {
	int a=LUA_GET_INT(state);
	Vector3d p=g_Game()->GetActorMovement(a)->GetSide();
	LUA_SET_VECTOR3(state, p);
	return 1;

	}

int ACTOR_GetUp(lua_State *state) {
	int a=LUA_GET_INT(state);
	Vector3d p=g_Game()->GetActorMovement(a)->GetUp();
	LUA_SET_VECTOR3(state, p);
	return 1;
	}

int ACTOR_GetDir(lua_State *state) {
	int a=LUA_GET_INT(state);
	Vector3d p=g_Game()->GetActorMovement(a)->GetDir();
	LUA_SET_VECTOR3(state, p);
	return 1;

	}

//How many blocks will we jump more
int ACTOR_GetJumpDistBlocks(lua_State *state) {
	int actorid=LUA_GET_INT(state);
	int res=g_Game()->GetActorMovement(actorid)->GetJumpDistBlocks();
	LUA_SET_NUMBER(state, res);
	return 1;
	}

//How many blocks will we jump more
int ACTOR_SetJumpDistBlocks(lua_State *state) {
	int b=LUA_GET_INT(state);
	int actorid=LUA_GET_INT(state);
	g_Game()->GetActorMovement(actorid)->SetJumpDistBlocks(b);
	return 0;
	}

int ACTOR_GetOnSide(lua_State *state) {
	int actorid=LUA_GET_INT(state);
	int res=g_Game()->GetActorMovement(actorid)->GetOnSideID();
	LUA_SET_NUMBER(state, res);
	return 1;
	}

int ACTOR_PrevOnSide(lua_State *state) {
	int actorid=LUA_GET_INT(state);
	int res=g_Game()->GetActorMovement(actorid)->GetPrevOnSideID();
	LUA_SET_NUMBER(state, res);
	return 1;
	}

int ACTOR_GetLastOnSide(lua_State *state) {
	int actorid=LUA_GET_INT(state);
	int res=g_Game()->GetActorMovement(actorid)->GetLastOnSideID();
	LUA_SET_NUMBER(state, res);
	return 1;
	}

int ACTOR_NumActors(lua_State *state) {
	int res=g_Game()->GetNumActors();
	LUA_SET_NUMBER(state, res);
	return 1;
	}

int ACTOR_TraceOnSide(lua_State *state) {
	int actorid=LUA_GET_INT(state);
	int res=g_Game()->GetActorMovement(actorid)->TraceOnSideID();
	LUA_SET_NUMBER(state, res);
	return 1;
	}

int ACTOR_GetRadius(lua_State *state) {
	int actorid=LUA_GET_INT(state);
	double rad=g_Game()->GetActorMovement(actorid)->GetRadius();
	LUA_SET_NUMBER(state, rad);
	return 1;

	}

int ACTOR_SetRadius(lua_State *state) {
	double rad=LUA_GET_DOUBLE(state);
	int actorid=LUA_GET_INT(state);
	g_Game()->GetActorMovement(actorid)->SetRadius(rad);
	return 0;

	}


int ACTOR_GetGroundOffset(lua_State *state) {
	int actorid=LUA_GET_INT(state);
	double rad=g_Game()->GetActorMovement(actorid)->GetGroundOffset();
	LUA_SET_NUMBER(state, rad);
	return 1;

	}

int ACTOR_SetGroundOffset(lua_State *state) {
	double rad=LUA_GET_DOUBLE(state);
	int actorid=LUA_GET_INT(state);
	g_Game()->GetActorMovement(actorid)->SetGroundOffset(rad);
	return 0;

	}


int ACTOR_SetJumpTiming(lua_State *state) {
	double latefw=LUA_GET_DOUBLE(state);
	double fwptj=LUA_GET_DOUBLE(state);
	double fwpt=LUA_GET_DOUBLE(state);
	int actor=LUA_GET_INT(state);
	g_Game()->GetActorMovement(actor)->SetJumpTiming(fwpt,fwptj,latefw);
	return 0;
	}

int ACTOR_SetJumpParams(lua_State *state) {
	double mspeed=LUA_GET_DOUBLE(state);
	double grav=LUA_GET_DOUBLE(state);
	double huv=LUA_GET_DOUBLE(state);

	double faruv=LUA_GET_DOUBLE(state);
	double uv=LUA_GET_DOUBLE(state);
	int actorid=LUA_GET_INT(state);
	g_Game()->GetActorMovement(actorid)->SetJumpParams(uv,faruv,huv,grav,mspeed);
	return 0;

	}

int ACTOR_SetJumpDistances(lua_State *state) {
	double huv=LUA_GET_DOUBLE(state);
	double faruv=LUA_GET_DOUBLE(state);
	double uv=LUA_GET_DOUBLE(state);
	int actorid=LUA_GET_INT(state);
	g_Game()->GetActorMovement(actorid)->SetJumpDistances(uv,faruv,huv);
	return 0;

	}

int ACTOR_GetEditorInfo(lua_State *state) {
	std::string def=LUA_GET_STRING(state);
	std::string what=LUA_GET_STRING(state);

	int b=LUA_GET_INT(state);
	std::string v=g_Game()->GetActorMovement(b)->GetEditorInfo(what,def);
	LUA_SET_STRING(state, v);
	return 1;
	}

int ACTOR_SetCamParams(lua_State *state) {
	Vector3d p=Vector3FromStack(state);
	std::string what=LUA_GET_STRING(state);
	int actorid=LUA_GET_INT(state);
	g_Game()->GetActorMovement(actorid)->SetCamParams(what,p);
	return 0;

	}

int ACTOR_SetCamZRotation(lua_State *state) {
	int mirr=LUA_GET_INT(state);
	double rot=LUA_GET_DOUBLE(state);
	int actorid=LUA_GET_INT(state);
	g_Game()->GetActorMovement(actorid)->SetCamZRotation(rot,mirr);
	return 0;

	}

int ACTOR_GetSpeed(lua_State *state) {
	int actorid=LUA_GET_INT(state);
	double v=g_Game()->GetActorMovement(actorid)->GetSpeed();
	LUA_SET_NUMBER(state, v);
	return 1;

	}

int ACTOR_SetSpeed(lua_State *state) {
	double v=LUA_GET_DOUBLE(state);
	int actorid=LUA_GET_INT(state);
	g_Game()->GetActorMovement(actorid)->SetSpeed(v);
	return 0;

	}

int ACTOR_SetRotSpeed(lua_State *state) {
	double v=LUA_GET_DOUBLE(state);
	int actorid=LUA_GET_INT(state);
	g_Game()->GetActorMovement(actorid)->SetRotSpeed(v);
	return 0;
	}

int ACTOR_SetStart(lua_State *state) {
	int rot=LUA_GET_INT(state);
	int sind=LUA_GET_INT(state);
	int actorid=LUA_GET_INT(state);
	g_Game()->GetActorMovement(actorid)->SetStartPos(g_Game()->GetLevel()->GetBlockSide(sind),rot);
	return 0;

	}


int ACTOR_InsideLevel(lua_State *state) {
	int actorid=LUA_GET_INT(state);
	int res=g_Game()->GetLevel()->PointInBBox(g_Game()->GetActorMovement(actorid)->GetPos());
	LUA_SET_NUMBER(state, res);
	return 1;

	}

int ACTOR_CurrentMove(lua_State *state) {
	int actorid=LUA_GET_INT(state);
	std::string s=g_Game()->GetActorMovement(actorid)->GetMoveType();
	LUA_SET_STRING(state, s);
	return 1;
	}

int ACTOR_SetRotateStopTime(lua_State *state) {
	double def=LUA_GET_DOUBLE(state);
	double now=LUA_GET_DOUBLE(state);
	int actorid=LUA_GET_INT(state);
	g_Game()->GetActorMovement(actorid)->SetRotateStopTime(now,def);
	return 0;
	}

int ACTOR_SetTemporaryMoveSpeedMultiplier(lua_State *state) {
	double mlt=LUA_GET_DOUBLE(state);
	int actorid=LUA_GET_INT(state);
	g_Game()->GetActorMovement(actorid)->SetTemporaryMoveSpeedMultiplier(mlt);
	return 0;
	}

int ACTOR_GetTemporaryMoveSpeedMultiplier(lua_State *state) {

	int actorid=LUA_GET_INT(state);
	double mlt=g_Game()->GetActorMovement(actorid)->GetTemporaryMoveSpeedMultiplier();
	LUA_SET_NUMBER(state, mlt);
	return 1;
	}


int ACTOR_GetUpVel(lua_State *state) {
	int actorid=LUA_GET_INT(state);
	double uv=g_Game()->GetActorMovement(actorid)->GetUpVel();
	LUA_SET_NUMBER(state, uv);
	return 1;
	}

int ACTOR_SetUpVel(lua_State *state) {
	double uv=LUA_GET_DOUBLE(state);
	int actorid=LUA_GET_INT(state);
	g_Game()->GetActorMovement(actorid)->SetUpVel(uv);
	return 0;
	}

int ACTOR_CallMove(lua_State *state) {
	std::string move=LUA_GET_STRING(state);
	int actorid=LUA_GET_INT(state);
///TODO: Get the right actor
	TCuboMovement *mv=g_Game()->GetActorMovement(actorid);
	if (move=="forward") {
			mv->MoveForward();
			}
	else if (move=="jump") {
			mv->Jump();
			}
	else if (move=="jumpup") {
			mv->JumpUp();
			}
	else if (move=="left") {
			mv->RotateLeft();
			}
	else if (move=="right") {
			mv->RotateRight();
			}
	else if (move=="farjump") {
			mv->FarJump();
			}
	else if (move=="highjump") {
			mv->HighJump();
			}
	else if (move=="lookup") {
			mv->LookUp();
			}
	else if (move=="lookdown") {
			mv->LookDown();
			}
	else if (move=="lookreset") {
			mv->LookReset();
			}
	return 0;

	}

int ACTOR_DistanceRender(lua_State *state) {
	float dist=LUA_GET_DOUBLE(state);
	int actor=LUA_GET_INT(state);
	g_Game()->GetLevel()->AddDistRenderItem(actor,DIST_RENDER_ACTOR,dist,state);
	return 0;
	}

int ACTOR_ChangeGravity(lua_State *state) {
	int do_at_same_grav=LUA_GET_INT(state);
	double speed=LUA_GET_DOUBLE(state);
	Vector3d np=Vector3FromStack(state);
	Vector3d ng=Vector3FromStack(state);
	int actorid=LUA_GET_INT(state);
	int res=g_Game()->GetActorMovement(actorid)->ChangeGravity(ng,np,speed,do_at_same_grav);
	LUA_SET_NUMBER(state, res);
	return 1;
	}

int ACTOR_SetForwardPressTime(lua_State *state) {
	double fpt=LUA_GET_DOUBLE(state);
	int actor=LUA_GET_INT(state);
	g_Game()->GetActorMovement(actor)->SetForwardPressTime(fpt);

	return 0;
	}

int ACTOR_GetPlayer(lua_State *state) {
	int actor=LUA_GET_INT(state);
	int plr=g_Game()->GetActorMovement(actor)->GetPlayer();
	LUA_SET_NUMBER(state, plr);
	return 1;
	}

int ACTOR_SetSelectable(lua_State *state) {
	int selectable=LUA_GET_INT(state);
	int actor=LUA_GET_INT(state);
	g_Game()->GetActorMovement(actor)->SetSelectable(selectable);
	return 0;
	}

int ACTOR_SetLookPos(lua_State *state) {
	float lp=LUA_GET_DOUBLE(state);
	int actor=LUA_GET_INT(state);
	g_Game()->GetActorMovement(actor)->SetLookPos(lp);
	return 0;
	}


int ACTOR_GetLookPos(lua_State *state) {
	int actor=LUA_GET_INT(state);
	double lp=g_Game()->GetActorMovement(actor)->GetLookPos();
	LUA_SET_NUMBER(state, lp);
	return 1;
	}

int ACTOR_Rebounce(lua_State *state) {
	int oth=LUA_GET_INT(state);
	int actor=LUA_GET_INT(state);
	g_Game()->GetActorMovement(actor)->Rebounce(g_Game()->GetActorMovement(oth));
	return 0;
	}

int ACTOR_SetMovementCheck(lua_State *state) {
	int mv=LUA_GET_INT(state);
	int actor=LUA_GET_INT(state);
	g_Game()->GetActorMovement(actor)->SetMovementCheck(mv);
	return 0;
	}

int ACTOR_GetMovementCheck(lua_State *state) {
	int actor=LUA_GET_INT(state);
	int res=g_Game()->GetActorMovement(actor)->GetMovementCheck();
	LUA_SET_NUMBER(state, res);
	return 1;
	}

int ACTOR_FrontSideRebounce(lua_State *state) {
	int actor=LUA_GET_INT(state);
	g_Game()->GetActorMovement(actor)->FrontSideRebounce();
	return 0;
	}


void LUA_ACTOR_RegisterLib() {
	g_CuboLib()->AddFunc("ACTOR_FrontSideRebounce",ACTOR_FrontSideRebounce);
	g_CuboLib()->AddFunc("ACTOR_GetMovementCheck",ACTOR_GetMovementCheck);
	g_CuboLib()->AddFunc("ACTOR_SetMovementCheck",ACTOR_SetMovementCheck);
	g_CuboLib()->AddFunc("ACTOR_GetLookPos",ACTOR_GetLookPos);
	g_CuboLib()->AddFunc("ACTOR_SetLookPos",ACTOR_SetLookPos);
	g_CuboLib()->AddFunc("ACTOR_NumActors",ACTOR_NumActors);
	g_CuboLib()->AddFunc("ACTOR_New",ACTOR_New);
	g_CuboLib()->AddFunc("ACTOR_SetTimeMultiplicator",ACTOR_SetTimeMultiplicator);
	g_CuboLib()->AddFunc("ACTOR_Delete",ACTOR_Delete);
	g_CuboLib()->AddFunc("ACTOR_GetType",ACTOR_GetType);
	g_CuboLib()->AddFunc("ACTOR_IsPlayer",ACTOR_IsPlayer);
	g_CuboLib()->AddFunc("ACTOR_CallMove",ACTOR_CallMove);
	g_CuboLib()->AddFunc("ACTOR_GetPos",ACTOR_GetPos);
	g_CuboLib()->AddFunc("ACTOR_GetOldPos",ACTOR_GetOldPos);
	g_CuboLib()->AddFunc("ACTOR_GetSide",ACTOR_GetSide); //This does not give the blockside, but it gives the Side-Vector
	g_CuboLib()->AddFunc("ACTOR_GetUp",ACTOR_GetUp);
	g_CuboLib()->AddFunc("ACTOR_GetDir",ACTOR_GetDir);
	g_CuboLib()->AddFunc("ACTOR_GetSpeed",ACTOR_GetSpeed);
	g_CuboLib()->AddFunc("ACTOR_SetSpeed",ACTOR_SetSpeed);
	g_CuboLib()->AddFunc("ACTOR_SetRotSpeed",ACTOR_SetRotSpeed);
	g_CuboLib()->AddFunc("ACTOR_SetCamParams",ACTOR_SetCamParams);
	g_CuboLib()->AddFunc("ACTOR_SetCamZRotation",ACTOR_SetCamZRotation);
	g_CuboLib()->AddFunc("ACTOR_GetRadius",ACTOR_GetRadius);
	g_CuboLib()->AddFunc("ACTOR_SetRadius",ACTOR_SetRadius);
	g_CuboLib()->AddFunc("ACTOR_GetGroundOffset",ACTOR_GetGroundOffset);
	g_CuboLib()->AddFunc("ACTOR_SetGroundOffset",ACTOR_SetGroundOffset);
	g_CuboLib()->AddFunc("ACTOR_SetJumpParams",ACTOR_SetJumpParams);
	g_CuboLib()->AddFunc("ACTOR_SetRotateStopTime",ACTOR_SetRotateStopTime);
	g_CuboLib()->AddFunc("ACTOR_SetJumpTiming",ACTOR_SetJumpTiming);
	g_CuboLib()->AddFunc("ACTOR_SetJumpDistances",ACTOR_SetJumpDistances);
	g_CuboLib()->AddFunc("ACTOR_GetOnSide",ACTOR_GetOnSide);
	g_CuboLib()->AddFunc("ACTOR_GetLastOnSide",ACTOR_GetLastOnSide);
	g_CuboLib()->AddFunc("ACTOR_PrevOnSide",ACTOR_PrevOnSide);
	g_CuboLib()->AddFunc("ACTOR_TraceOnSide",ACTOR_TraceOnSide);
	g_CuboLib()->AddFunc("ACTOR_GetVar",ACTOR_GetVar);
	g_CuboLib()->AddFunc("ACTOR_SetVar",ACTOR_SetVar);
	g_CuboLib()->AddFunc("ACTOR_InsideLevel",ACTOR_InsideLevel);
	g_CuboLib()->AddFunc("ACTOR_SendKey",ACTOR_SendKey);
	g_CuboLib()->AddFunc("ACTOR_SendJoyButton",ACTOR_SendJoyButton);
	g_CuboLib()->AddFunc("ACTOR_DistanceRender",ACTOR_DistanceRender);
	g_CuboLib()->AddFunc("ACTOR_SetStart",ACTOR_SetStart);
	g_CuboLib()->AddFunc("ACTOR_GetJumpDistBlocks",ACTOR_GetJumpDistBlocks);
	g_CuboLib()->AddFunc("ACTOR_SetJumpDistBlocks",ACTOR_SetJumpDistBlocks);
	g_CuboLib()->AddFunc("ACTOR_GetUpVel",ACTOR_GetUpVel);
	g_CuboLib()->AddFunc("ACTOR_SetUpVel",ACTOR_SetUpVel);
	g_CuboLib()->AddFunc("ACTOR_CurrentMove",ACTOR_CurrentMove);
	g_CuboLib()->AddFunc("ACTOR_GetStartRotation",ACTOR_GetStartRotation);
	g_CuboLib()->AddFunc("ACTOR_GetEditorInfo",ACTOR_GetEditorInfo);
	g_CuboLib()->AddFunc("ACTOR_ChangeGravity",ACTOR_ChangeGravity);
	g_CuboLib()->AddFunc("ACTOR_SetForwardPressTime",ACTOR_SetForwardPressTime);
	g_CuboLib()->AddFunc("ACTOR_GetPlayer",ACTOR_GetPlayer);
	g_CuboLib()->AddFunc("ACTOR_SetSelectable",ACTOR_SetSelectable);
	g_CuboLib()->AddFunc("ACTOR_Rebounce",ACTOR_Rebounce);
	g_CuboLib()->AddFunc("ACTOR_SetTemporaryMoveSpeedMultiplier",ACTOR_SetTemporaryMoveSpeedMultiplier);
	g_CuboLib()->AddFunc("ACTOR_GetTemporaryMoveSpeedMultiplier",ACTOR_GetTemporaryMoveSpeedMultiplier);

	}


/////////////////////////////////////////////////////////

int PLAYER_GetVar(lua_State *state) {
	int plr=(int)lua_tonumber(state,1);
	lua_remove(state,1);
	g_Game()->GetPlayer(plr)->GetVarHolder()->GetVar(state);
	return 1;
	}


int PLAYER_SetVar(lua_State *state) {
	int plr=(int)lua_tonumber(state,1);
	lua_remove(state,1);
	g_Game()->GetPlayer(plr)->GetVarHolder()->StoreVar(state);
	return 0;
	}

int PLAYER_AddActor(lua_State *state) {
	int act=LUA_GET_INT(state);
	int plr=LUA_GET_INT(state);
	g_Game()->GetPlayer(plr)->AddActor(act);
	return 0;
	}

int PLAYER_GetActiveActor(lua_State *state) {
	int plr=LUA_GET_INT(state);
	int act=g_Game()->GetPlayer(plr)->GetActiveActor();
	LUA_SET_NUMBER(state, act);
	return 1;
	}

int PLAYER_NumActors(lua_State *state) {
	int plr=LUA_GET_INT(state);
	int act=g_Game()->GetPlayer(plr)->NumActors();
	LUA_SET_NUMBER(state, act);
	return 1;
	}

int PLAYER_GetActor(lua_State *state) {
	int act=LUA_GET_INT(state);
	int plr=LUA_GET_INT(state);
	act=g_Game()->GetPlayer(plr)->GetActor(act);
	LUA_SET_NUMBER(state, act);
	return 1;
	}

int PLAYER_NextActor(lua_State *state) {
	int plr=LUA_GET_INT(state);
	int act=g_Game()->GetPlayer(plr)->NextActor();
	LUA_SET_NUMBER(state, act);
	return 1;
	}

int PLAYER_InCameraPan(lua_State *state) {
	int plr=LUA_GET_INT(state);
	int act=g_Game()->GetPlayer(plr)->InCameraPan();
	LUA_SET_NUMBER(state, act);
	return 1;
	}

int PLAYER_SetCameraSpeed(lua_State *state) {
	float speed=LUA_GET_DOUBLE(state);
	int plr=LUA_GET_INT(state);
	g_Game()->GetPlayer(plr)->SetCamSpeed(speed);
	return 0;
	}

int PLAYER_SelectActor(lua_State *state) {
	int aid=LUA_GET_INT(state);
	int plr=LUA_GET_INT(state);
	aid=g_Game()->GetPlayer(plr)->SelectActor(aid);
	LUA_SET_NUMBER(state, aid);
	return 1;
	}

int PLAYER_GetLastActiveActor(lua_State *state) {
	int plr=LUA_GET_INT(state);
	plr=g_Game()->GetPlayer(plr)->GetLastActiveActor();
	LUA_SET_NUMBER(state, plr);
	return 1;
	}

int PLAYER_GetCamInterpolation(lua_State *state) {
	int plr=LUA_GET_INT(state);
	double ci=g_Game()->GetPlayer(plr)->GetCamInterpolation();
	LUA_SET_NUMBER(state, ci);
	return 1;
	}


void LUA_PLAYER_RegisterLib() {
	g_CuboLib()->AddFunc("PLAYER_GetVar",PLAYER_GetVar);
	g_CuboLib()->AddFunc("PLAYER_SetVar",PLAYER_SetVar);
	g_CuboLib()->AddFunc("PLAYER_AddActor",PLAYER_AddActor);
	g_CuboLib()->AddFunc("PLAYER_GetActiveActor",PLAYER_GetActiveActor);
	g_CuboLib()->AddFunc("PLAYER_NumActors",PLAYER_NumActors);
	g_CuboLib()->AddFunc("PLAYER_GetActor",PLAYER_GetActor);
	g_CuboLib()->AddFunc("PLAYER_NextActor",PLAYER_NextActor);
	g_CuboLib()->AddFunc("PLAYER_SetCameraSpeed",PLAYER_SetCameraSpeed);
	g_CuboLib()->AddFunc("PLAYER_InCameraPan",PLAYER_InCameraPan);
	g_CuboLib()->AddFunc("PLAYER_SelectActor",PLAYER_SelectActor);
	g_CuboLib()->AddFunc("PLAYER_GetCamInterpolation",PLAYER_GetCamInterpolation);
	g_CuboLib()->AddFunc("PLAYER_GetLastActiveActor",PLAYER_GetLastActiveActor);



	}
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
