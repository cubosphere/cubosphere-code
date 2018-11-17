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

#include "cuboboxes.hpp"
#include "globals.hpp"
#include "posteffects.hpp"

#include <vector>
#include "vectors.hpp"
#include "shaderutil.hpp"
#include "luautils.hpp"

#ifdef WIN32

#ifdef MINGW_CROSS_COMPILE
#include <GL/glew.h>
#else
#include <GL\glew.h>
#endif
// #include <GL\gl.h>
#include <windows.h>
#include <SDL.h>


#else

#include <GL/glew.h>
#include <SDL/SDL.h>

#endif

#include "game.hpp"
#include "cuboactors.hpp"
#include "cuboenemies.hpp"

//Specifies a function inside the given LuaState.
//Params: In: x,y,z -> Out: Alpha Value
static LuaAccess* s_AlphaAccess=NULL;
static std::string s_AlphaFunc="";

static float s_LastDiffuse[4];

void g_LastDiffuse(float * ld) {for (int c=0; c<4; c++) s_LastDiffuse[c]=ld[c];}

void g_SetAlphaFunc(LuaAccess *acc,std::string func)
	{
	s_AlphaAccess=acc;
	s_AlphaFunc=func;
	}

float ExecSideAlpha(float x,float y,float z)
	{
	double res;
	s_AlphaAccess->CallVA(s_AlphaFunc.c_str(),"ddd>d",x,y,z,&res);
	return (float)res;
	}

void SideAlpha(float &x,float &y,float &z)
	{
	s_LastDiffuse[3]=ExecSideAlpha(x,y,z);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   s_LastDiffuse);
	}

void CuboBlockSide::SetSideItem(std::string n)
	{
	//First remove all Items of this side
	g_Game()->GetLevel()->RemoveItemFromSide(this);
	if (n=="") { return; }
	//add the new item
	g_Game()->GetLevel()->AddItem(parent->GetID(),s_CuboSideNames[side],n);
	}

CuboItem *CuboBlockSide::GetItem()
	{
	//First remove all Items of this side
	CuboItem *res=g_Game()->GetLevel()->GetItemOnSide(this);
	return res;
	}




void CuboBlockSide::Init(int cside,CuboBlock *cparent)
	{
	side=cside;
	parent=cparent;
	}


int CuboBlockSide::GetID()
	{
	int pid=parent->GetID();
	return pid*6+side;
	}


void CuboBlockSide::Render()
	{
	//Call the RenderSide of the blockdef
	g_Game()->GetLevel()->GetBlockDef(sidetype)->Call_RenderSide(GetID());
	}

void CuboBlockSide::SpecialRender(std::string nam,int defrender)
	{
	if ( g_Game()->GetLevel()->GetBlockDef(sidetype)->Call_SpecialRender(nam,GetID())) {}
	else
			{
			if (defrender==1) { Render(); }
			else if (defrender==0 && g_PostEffect()) { g_PostEffect()->CallDefaultSpecialRender(nam,"side",GetID()); }
			}

	}

void CuboBlockSide::DistRender()
	{
	g_Game()->GetLevel()->GetBlockDef(sidetype)->Call_DistRenderSide(GetID());
	}

void CuboBlockSide::Call_CollisionCheck(int plr)
	{
	g_Game()->GetLevel()->GetBlockDef(sidetype)->Call_CollisionCheck(plr,GetID());
	}

void CuboBlockSide::Call_VarChanged(std::string var) //To observe changes done by editor
	{
	g_Game()->GetLevel()->GetBlockDef(sidetype)->Call_SideVarChanged(var,GetID());
	}



Vector2d CuboBlockSide::GetUV(int i)
	{
	return s_CuboUV[i]; ///TODO: add rotation and mirroring
	}

Vector3d CuboBlockSide::GetTangent()
	{
	Vector3d p1=s_CuboVerts[side][0];
	Vector3d p2=s_CuboVerts[side][1];
	Vector3d p3=s_CuboVerts[side][2];

	Vector2d uv1=GetUV(0);
	Vector2d uv2=GetUV(1);
	Vector2d uv3=GetUV(2);

	Vector3d diff1=p2-p1;
	Vector3d diff2=p3-p1;
	diff1=(diff1 * (uv3.v-uv1.v));
	diff2=(diff2 * (uv2.v-uv1.v));
	Vector3d tangent=  diff1-diff2;
	tangent.normalize();
	return tangent;
	}

Vector3d CuboBlockSide::GetMidpoint()
	{
	Vector3d res;
	res=s_CuboNormals[side];
	res=res*CUBO_SCALE;
	res=res+parent->GetPos();
	return res;
	}

std::string CuboBlockSide::GetTypeName()
	{
	return g_Game()->GetLevel()->GetBlockDef(sidetype)->GetName();
	}

void CuboBlockSide::RenderQuad()
	{
// glColor3f(1,1,1);
	Vector3d ps=parent->GetPos();
//  glBegin(GL_QUADS);
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(s_CuboNormals[side].x,s_CuboNormals[side].y,s_CuboNormals[side].z);
	for (int i=0; i<4; i++)
			{
			int texind=i;
			glTexCoord2f(s_CuboUV[texind].u,s_CuboUV[texind].v);
			Vector3d offs=s_CuboVerts[side][i];
			offs=offs*parent->GetScale();
			Vector3d p=ps+offs;
			//Here we have to check whether we have to call a SetDiffuse for each Texture Coordinate
			if ((s_AlphaAccess) && (s_AlphaFunc!="")) {
					SideAlpha(p.x,p.y,p.z);
					}
			glVertex3d(p.x,p.y,p.z);
			}
	glEnd();
	}



std::string CuboBlockSide::GetEditorInfo(std::string what,std::string def)
	{
	return ((g_Game()->GetLevel()->GetBlockDef(this->sidetype)->Call_GetEditorInfo(what,def)));
	}

std::string CuboBlock::GetEditorInfo(std::string what,std::string def)
	{
	return ((g_Game()->GetLevel()->GetBlockDef(this->blocktype)->Call_GetEditorInfo(what,def)));
	}


void CuboBlock::InitSides()
	{
	sides.resize(6);
	next.resize(6);
	for (unsigned int i=0; i<6; i++)
			{
			CuboBlockSide bs;
			bs.Init(i,this);
			sides[i]=bs;
			next[i]=NULL;
			}
	}


bool CuboBlock::HasNoTransparency()
	{
	return (!(g_Game()->GetLevel()->GetBlockDef(this->blocktype)->Call_HasTransparency(GetID())));
	}

int CuboBlock::Blocking()
	{
	return (!(g_Game()->GetLevel()->GetBlockDef(this->blocktype)->Call_IsPassable(GetID())));
	}

int CuboBlock::Moving()
	{
	return ((g_Game()->GetLevel()->GetBlockDef(this->blocktype)->Call_IsMoving(GetID())));
	}

int CuboBlock::IsEditorSelector()
	{
	if (g_Game()->GetLevel()->GetBlockDef(this->blocktype)->GetName()=="_selection") { return 1; }
	return 0;
	}

void CuboBlockSide::Think()
	{
	g_Game()->GetLevel()->GetBlockDef(this->sidetype)->Call_SideThink(GetID());

	}

void CuboBlock::Think()
	{
	oldpos=pos;
	g_Game()->GetLevel()->GetBlockDef(this->blocktype)->Call_BlockThink(GetID());
	for (unsigned int i=0; i<6; i++)
			{
			if (next[i])
					{
					if (next[i]->HasNoTransparency()) {  continue; }
					}
			sides[i].Think();
			}
	}

void CuboItem::Think()
	{
	g_Game()->GetLevel()->GetItemDef(this->itemtype)->Call_Think(GetID());

	}

std::string CuboItem::GetEditorInfo(std::string what,std::string def)
	{
	return ((g_Game()->GetLevel()->GetItemDef(this->itemtype)->Call_GetEditorInfo(what,def)));
	}

CuboBlock::CuboBlock()
	{
	InitSides();
	pos.xyz(0,0,0);
	oldpos=pos;
	scale=1;
	cullradius=sqrt(3.0)*CUBO_SCALE;
	}

CuboBlock::~CuboBlock()
	{
	ReleaseMeFromNext();
	}


void CuboBlock::SetIPos(int x,int y,int z)
	{
	pos.xyz(x*CUBO_SCALE*2,y*CUBO_SCALE*2,z*CUBO_SCALE*2);
	oldpos=pos;
	}

void CuboBlock::Render(Camera *cam)
	{
	//Perform a precull
	//Get the campos
	Vector3d cp=cam->getPos();
	cp=pos-cp;

	for (unsigned int i=0; i<6; i++)
			{
			if (next[i])
					{
					if (next[i]->HasNoTransparency()) { continue; }
					}
			float dist=cp*s_CuboNormals[i];
			if (-dist>CUBO_SCALE || (!(sides[i].Call_MayCull()))) {
					sides[i].Render();
					}
			}
	}

void CuboBlock::MustRenderSides(Camera *cam,int  mustrender[])
	{
	//Perform a precull
	//Get the campos
	Vector3d cp=cam->getPos();
	cp=pos-cp;

	for (unsigned int i=0; i<6; i++)
			{
			if (next[i])
					{
					if (next[i]->HasNoTransparency()) { mustrender[i]=0; continue; }
					}
			//else
				{
				float dist=cp*s_CuboNormals[i];
				if (-dist>CUBO_SCALE || (!(sides[i].Call_MayCull()))) {
						mustrender[i]=1;
						}
				else { mustrender[i]=0; }
				}
			}
	}
Vector3d CuboBlock::GetPos()
	{
	return pos;
	}


void CuboBlock::SetNext(int side,CuboBlock *n)
	{

	next[side]=n;
	}

CuboBlock *CuboBlock::GetNext(int side)
	{
	return next[side];
	}

int CuboBlock::GetNeighbor(Vector3d norm)
	{
	Vector3d n=norm;
	n.normalize();
	int d=-1;
	for (int i=0; i<6; i++)
			{	Vector3d kn;
			kn=s_CuboNormals[i];
			if ((kn*n)>0.5) {d=i; break;}
			}
	if (d<0) { return -1; }
	CuboBlock *ne=next[d];
	if (!ne) { return -1; }
	return ne->GetID();
	}

void CuboBlock::WriteLevelData(FILE *f)
	{
	//First add the Block by adding:
	std::string typen=g_Game()->GetLevel()->GetBlockDef(blocktype)->GetName();
	fprintf(f,"\n  LEVEL_AddBlock(%d,%d,%d,\"%s\");\n",(int)(pos.x/(2*CUBO_SCALE)),(int)(pos.y/(2*CUBO_SCALE)),(int)(pos.z/(2*CUBO_SCALE)),typen.c_str());

	//Block only block vars
	if (GetBlockSide(0)->GetEditorInfo("BlockOnly","") == "yes")
			{

			if (GetBlockSide(0)->GetEditorInfo("NumVars","") != "")  //Get the exported variables
					{
					std::string nvs=GetBlockSide(0)->GetEditorInfo("NumVars","0");
					int sidevars=atoi(nvs.c_str());
					for (int sv=1; sv<=sidevars; sv++)
							{
							std::string svarname=GetBlockSide(0)->GetEditorInfo("Var"+std::to_string(sv),"");
							std::string svarval=GetVarHolder()->GetVarString(svarname,1);
							fprintf(f,"    BLOCK_SetVar(LEVEL_LastBlock(), \"%s\", %s);\n",svarname.c_str(),svarval.c_str());

							}
					}
			}

	//Now we apply the side changes
	for (int s=0; s<6; s++)
			{
			std::string stypen=GetBlockSide(s)->GetTypeName();
			if (stypen!=typen) {
					fprintf(f,"    LEVEL_ChangeSide(LEVEL_LastBlock(),\"%s\",\"%s\");\n",s_CuboSideNames[s].c_str(),stypen.c_str());
					}


			if (GetBlockSide(s)->GetEditorInfo("BlockOnly","") != "yes")
					{

					if (GetBlockSide(s)->GetEditorInfo("NumVars","") != "")  //Get the exported variables
							{
							std::string nvs=GetBlockSide(s)->GetEditorInfo("NumVars","0");
							int sidevars=atoi(nvs.c_str());
							for (int sv=1; sv<=sidevars; sv++)
									{
									std::string svarname=GetBlockSide(s)->GetEditorInfo("Var"+std::to_string(sv),"");
									std::string svarval=GetBlockSide(s)->GetVarHolder()->GetVarString(svarname,1);
									fprintf(f,"      SIDE_SetVar(LEVEL_LastBlock()*6+%d, \"%s\", %s);\n",s,svarname.c_str(),svarval.c_str());

									}
							}
					}


			//Check the existance of an item
			CuboItem *item=GetBlockSide(s)->GetItem();
			if (item!=NULL)
					{
					///TODO: Check if it is in a block
					Vector3d checkpos=GetBlockSide(s)->GetMidpoint();
					Vector3d normal=GetBlockSide(s)->GetNormal();
					normal=normal*CUBO_SCALE;
					checkpos=checkpos+normal;
					if (g_Game()->GetLevel()->GetBlockAtPos(checkpos))
							{
							fprintf(f,"    --LEVEL_AddItem(LEVEL_LastBlock(),\"%s\",\"%s\");  -- (Is in another block!)\n",s_CuboSideNames[s].c_str(),item->GetName().c_str());

							}
					else { fprintf(f,"    local item=LEVEL_AddItem(LEVEL_LastBlock(),\"%s\",\"%s\");\n",s_CuboSideNames[s].c_str(),item->GetName().c_str()); }

					if (item->GetEditorInfo("NumVars","") != "")  //Get the exported variables
							{
							std::string nvs=item->GetEditorInfo("NumVars","0");
							int itemvars=atoi(nvs.c_str());
							for (int iv=1; iv<=itemvars; iv++)
									{
									std::string ivarname=item->GetEditorInfo("Var"+std::to_string(iv),"");
									std::string ivarval=item->GetVarHolder()->GetVarString(ivarname,1);
									fprintf(f,"      ITEM_SetVar(item, \"%s\", %s);\n",ivarname.c_str(),ivarval.c_str());

									}
							}

					}

			//And the one of an enemy
			CuboEnemy* e=NULL;
			for (int ind=0; ind<g_Game()->GetNumActors(); ind++)
					{
					TCuboMovement *en=g_Game()->GetActorMovement(ind);
					if (en->IsPlayer()) { continue; }
					if (en->GetOnSideID()==GetBlockSide(s)->GetID())
							{
							e=(CuboEnemy*)en; //Found an enemy
							break;
							}
					}

			if (e!=NULL)
					{
					Vector3d checkpos=GetBlockSide(s)->GetMidpoint();
					Vector3d normal=GetBlockSide(s)->GetNormal();
					normal=normal*CUBO_SCALE;
					checkpos=checkpos+normal;
					if (g_Game()->GetLevel()->GetBlockAtPos(checkpos))
							{
							fprintf(f,"    --ENEMY_New(...); not used -- (Is in another block!)\n");

							}
					else
							{
							fprintf(f,"    local enemy=ENEMY_New(\"%s\");\n",e->GetType().c_str());
							fprintf(f,"      ACTOR_SetStart(enemy,6*LEVEL_LastBlock()+%d,%d);\n",s,e->GetStartRotation());
							if (e->GetEditorInfo("NumVars","") != "")  //Get the exported variables
									{
									std::string nvs=e->GetEditorInfo("NumVars","0");
									int sidevars=atoi(nvs.c_str());
									for (int sv=1; sv<=sidevars; sv++)
											{
											std::string svarname=e->GetEditorInfo("Var"+std::to_string(sv),"");
											std::string svarval=e->GetVarHolder()->GetVarString(svarname,1);
											fprintf(f,"      ACTOR_SetVar(enemy, \"%s\", %s);\n",svarname.c_str(),svarval.c_str());

											}
									}
							}
					}
			}
	}

void CuboBlock::ReleaseMeFromNext()
	{
	for (int i=0; i<6; i++)
		if (next[i]) { next[i]->SetNext(s_CuboOpposingDir[i],NULL); }
	}


void CuboBlock::ReAttachMeToNext()
	{
	for (int i=0; i<6; i++)
		if (next[i]) { next[i]->SetNext(s_CuboOpposingDir[i],this); }
	}

void CuboBlock::SetBlockType(int i)
	{
	blocktype=i;
	for (int j=0; j<6; j++) { sides[j].SetSideType(blocktype); }
	}

void CuboBlock::Call_OnBlockEvent(int actorid)
	{
	//Get the LUA script
	g_Game()->GetLevel()->GetBlockDef(blocktype)->Call_OnBlockEvent(id,actorid);
	}


void CuboBlockSide::Call_OnSideEvent(int actorid)
	{
	//Get the LUA script
	g_Game()->GetLevel()->GetBlockDef(this->sidetype)->Call_OnSideEvent(GetID(),actorid);
	}


int CuboBlockSide::Call_MayMove(int actorid,std::string movestr)
	{
	//Get the LUA script
	return g_Game()->GetLevel()->GetBlockDef(this->sidetype)->Call_MayMove(GetID(),actorid,movestr);
	}

int CuboBlockSide::Call_MayCull()
	{
	//Get the LUA script
	return g_Game()->GetLevel()->GetBlockDef(this->sidetype)->Call_MayCull(GetID());
	}

int CuboBlockSide::Call_MaySlideDown(int actorid)
	{
	return g_Game()->GetLevel()->GetBlockDef(this->sidetype)->Call_MaySlideDown(GetID(),actorid);
	}


int CuboBlock::BlockInRay(Vector3d start,Vector3d dir,float *dist,Vector3d *hitpt) //-1 if not hit otherwise the side
	{
//First a distance check:
	Vector3d diff=pos;
	diff=diff-start;
	float sqrdist=diff.sqrlength();
	if (sqrdist-3*CUBO_SCALE*CUBO_SCALE>(*dist)*(*dist)) { return -1; }
//Now go through the sides
	int theside=-1;
	for (int i=0; i<6; i++)
			{
			///Plane Equation: n*x=d
			/// d can be obtained by pos*n+CUBO_SCALE
			/// Now the ray x(t) has the form: x(t)=s+dir*t
			/// n*s+n*dir*t=d
			//
			float planedist=pos*s_CuboNormals[i];
			planedist+=CUBO_SCALE;
			float ndotd=dir*s_CuboNormals[i];
			float ndots=start*s_CuboNormals[i];
			//if (ndots<0) continue;
			planedist-=ndots;
			if (ndotd>0.00000001) { continue; } //parallel
			planedist/=ndotd;
			if (planedist<0) { continue; }
			if (planedist<*dist)
					{
					///Only check if we are inside
					Vector3d onpos=dir*planedist;
					onpos=onpos+start;
					//Subtract the normal
					Vector3d vgl=s_CuboNormals[i];
					vgl=vgl*CUBO_SCALE;
					vgl=onpos-vgl;
					//Remains to check the maximum of vgl smaller than the CUBO_DIST
					if (vgl.MaxAbsValue()<=CUBO_SCALE)
							{
							*dist=planedist;
							theside=i;
							*hitpt=onpos;
							}
					}
			}
	return theside;
	}


CuboItem::CuboItem(int id,int type,CuboBlockSide *side)
	{
	myid=id;
	itemtype=type;
	sideptr=side;
	}

Vector3d CuboItem::GetPos()
	{
//ok, get the sides midpoint and add a bit
	Vector3d pos=sideptr->GetMidpoint();
	Vector3d norm=sideptr->GetNormal();
	norm=norm*CUBO_SCALE;
	pos=pos+norm;
	return pos;
	}

void CuboItem::SpecialRender(std::string nam,int defrender)
	{
	if ( g_Game()->GetLevel()->GetItemDef(itemtype)->Call_SpecialRender(nam,myid)) {}
	else
			{
			if (defrender==1) { Render(); }
			else if (defrender==0 && g_PostEffect()) { g_PostEffect()->CallDefaultSpecialRender(nam,"item",myid); }
			}

	}

void CuboItem::Render()
	{
	g_Game()->GetLevel()->GetItemDef(itemtype)->Call_Render(myid);
	}

void CuboItem::DistRender()
	{
	g_Game()->GetLevel()->GetItemDef(itemtype)->Call_DistRender(myid);
	}

std::string CuboItem::GetName()
	{
	return g_Game()->GetLevel()->GetItemDef(itemtype)->GetName();
	}

void CuboItem::Call_Constructor()
	{
	varholder.clear();
	g_Game()->GetLevel()->GetItemDef(itemtype)->Call_Constructor(myid);
	}

void CuboItem::CollisionCheckWithActor(int actorid)
	{
	g_Game()->GetLevel()->GetItemDef(itemtype)->Call_CollisionCheck(actorid,myid);
	}

void CuboBlock::Call_Constructor()
	{
	varholder.clear();
	g_Game()->GetLevel()->GetBlockDef(blocktype)->Call_BlockConstructor(GetID());
	//And the same thing for all sides
	for (unsigned int i=0; i<6; i++) { sides[i].Call_Constructor(); }
	}


void CuboBlock::Call_CollisionCheck(int plr)
	{
	for (unsigned int i=0; i<6; i++) { sides[i].Call_CollisionCheck(plr); }
	}


void CuboBlockSide::Call_Constructor()
	{
	varholder.clear();
	g_Game()->GetLevel()->GetBlockDef(sidetype)->Call_SideConstructor(GetID());
	}

Vector3d CuboBlock::GetBBMax()
	{
	Vector3d res;
	res.xyz(CUBO_SCALE,CUBO_SCALE,CUBO_SCALE);
	res=pos+res;
	return res;
	}

Vector3d CuboBlock::GetBBMin()
	{
	Vector3d res;
	res.xyz(CUBO_SCALE,CUBO_SCALE,CUBO_SCALE);
	res=pos-res;
	return res;
	}


std::string CuboBlock::GetName()
	{
	return g_Game()->GetLevel()->GetBlockDef(blocktype)->GetName();
	}



////////////////////////////////////////////////


void BlockDef::Call_CollisionCheck(int actorid, int itemid) {    COND_LUA_CALL("SideCollisionCheck",,"ii",actorid,itemid);}
std::string BlockDef::Call_GetEditorInfo(std::string what,std::string std) {char *res;  COND_LUA_CALL("GetEditorInfo",std,"ss>s",what.c_str(),std.c_str(),&res);  return res; }
void BlockDef::Call_RenderSide(int sideid) { COND_LUA_CALL("RenderSide",,"i",sideid);}
void BlockDef::Call_DistRenderSide(int sideid) {COND_LUA_CALL("DistRenderSide",,"i",sideid);}
void BlockDef::Call_OnBlockEvent(int blockid,int actorid) { COND_LUA_CALL("Event_OnBlock",,"ii",blockid,actorid);}
void BlockDef::Call_OnSideEvent(int sideid,int actorid) { COND_LUA_CALL("Event_OnSide",,"ii",sideid,actorid);}
int BlockDef::Call_MayMove(int sideid,int actorid,std::string movetype) { int res; COND_LUA_CALL("MayMove",1,"iis>i",sideid,actorid,movetype.c_str(),&res); return res;}
int BlockDef::Call_HasTransparency(int blockid) { int res; COND_LUA_CALL("HasTransparency",0,"i>i",blockid,&res); return res;}
int BlockDef::Call_IsPassable(int blockid) { int res; COND_LUA_CALL("IsPassable",0,"i>i",blockid,&res); return res;}
int BlockDef::Call_IsMoving(int blockid) { int res; COND_LUA_CALL("IsMoving",0,"i>i",blockid,&res); return res;}
void BlockDef::Call_BlockThink(int blockid) {COND_LUA_CALL("BlockThink",,"i",blockid);}
int BlockDef::Call_SpecialRender(std::string nam,int index) { COND_LUA_CALL("SpecialRender",0,"si",nam.c_str(),index); return 1;}
void BlockDef::Call_SideThink(int sideid) {COND_LUA_CALL("SideThink",,"i",sideid);}
void BlockDef::Call_SideVarChanged(std::string var,int sideid) {COND_LUA_CALL("SideVarChanged",,"si",var.c_str(),sideid);}
int BlockDef::Call_MaySlideDown(int sideid,int actorid) {int res; COND_LUA_CALL("MaySlideDown",0,"ii>i",sideid,actorid,&res); return res;}
int BlockDef::Call_MayCull(int sideid) { int res; COND_LUA_CALL("MayCull",1,"i>i",sideid,&res); return res;}
void BlockDef::Call_BlockConstructor(int id) {COND_LUA_CALL("BlockConstructor",,"i",id);}
void BlockDef::Call_SideConstructor(int id) {COND_LUA_CALL("SideConstructor",,"i",id);}



void ItemDef::Call_Think(int itemid) { COND_LUA_CALL("Think",,"i",itemid);}
int ItemDef::Call_SpecialRender(std::string nam,int index) {   COND_LUA_CALL("SpecialRender",0,"i",nam.c_str(),index); return 1;}
std::string ItemDef::Call_GetEditorInfo(std::string what,std::string std) {char *res; COND_LUA_CALL("GetEditorInfo",std,"ss>s",what.c_str(),std.c_str(),&res); return res;}
void ItemDef::Call_Render(int itemindex) {   COND_LUA_CALL("Render",,"i",itemindex);}
void ItemDef::Call_CollisionCheck(int actorid, int itemid) {  COND_LUA_CALL("CollisionCheck",,"ii",actorid,itemid);}
void ItemDef::Call_DistRender(int itemindex) {   COND_LUA_CALL("DistRender",,"i",itemindex);}
void ItemDef::Call_Constructor(int id) {   COND_LUA_CALL("Constructor",,"i",id);}


///////////////////LUA IMPLEMENT////////////////


int ITEM_SetVar(lua_State *state)
	{
	int item=(int)lua_tonumber(state,1);
	lua_remove(state,1);
	if (item<0) { return 0; }
	g_Game()->GetLevel()->GetItem(item)->GetVarHolder()->StoreVar(state);

	return 0;
	}


int ITEM_GetVar(lua_State *state)
	{
	int item=(int)lua_tonumber(state,1);
	lua_remove(state,1);
	g_Game()->GetLevel()->GetItem(item)->GetVarHolder()->GetVar(state);

	return 1;
	}


int ITEM_GetType(lua_State *state)
	{
	int ind=LUA_GET_INT(state);
	std::string res=g_Game()->GetLevel()->GetItem(ind)->GetName();
	LUA_SET_STRING(res);
	return 1;
	}

int ITEM_GetEditorInfo(lua_State *state)
	{
	std::string def=LUA_GET_STRING(state);
	std::string what=LUA_GET_STRING(state);

	int b=LUA_GET_INT(state);
	std::string v=g_Game()->GetLevel()->GetItem(b)->GetEditorInfo(what,def);
	LUA_SET_STRING(v);
	return 1;
	}

int ITEM_GetSide(lua_State *state)
	{
	int item=LUA_GET_INT(state);
	int side=g_Game()->GetLevel()->GetItem(item)->GetSide()->GetID();
	LUA_SET_INT(side);
	return 1;
	}

int ITEM_DistanceRender(lua_State *state)
	{
	float dist=LUA_GET_DOUBLE(state);
	int item=LUA_GET_INT(state);
	g_Game()->GetLevel()->AddDistRenderItem(item,DIST_RENDER_ITEM,dist,state);
	return 0;
	}


void LUA_ITEM_RegisterLib()
	{
	g_CuboLib()->AddFunc("ITEM_GetSide",ITEM_GetSide);
	g_CuboLib()->AddFunc("ITEM_DistanceRender",ITEM_DistanceRender);
	g_CuboLib()->AddFunc("ITEM_SetVar",ITEM_SetVar);
	g_CuboLib()->AddFunc("ITEM_GetVar",ITEM_GetVar);
	g_CuboLib()->AddFunc("ITEM_GetEditorInfo",ITEM_GetEditorInfo);
	g_CuboLib()->AddFunc("ITEM_GetType",ITEM_GetType);
	}



//////////////////////////



int SIDE_RenderQuad(lua_State *state)
	{
	int side=LUA_GET_INT(state);
	g_Game()->GetLevel()->GetBlockSide(side)->RenderQuad();
	return 0;
	}

int SIDE_GetTangent(lua_State *state)
	{
	int side=LUA_GET_INT(state);
	Vector3d v= g_Game()->GetLevel()->GetBlockSide(side)->GetTangent();
	LUA_SET_VECTOR3(v);
	return 1;
	}

int SIDE_GetNormal(lua_State *state)
	{
	int side=LUA_GET_INT(state);
	Vector3d v= g_Game()->GetLevel()->GetBlockSide(side)->GetNormal();
	LUA_SET_VECTOR3(v);
	return 1;
	}

int SIDE_GetMidpoint(lua_State *state)
	{
	int side=LUA_GET_INT(state);
	Vector3d v= g_Game()->GetLevel()->GetBlockSide(side)->GetMidpoint();
	LUA_SET_VECTOR3(v);
	return 1;
	}

int SIDE_DistanceRender(lua_State *state)
	{
	float dist=LUA_GET_DOUBLE(state);
	int side=LUA_GET_INT(state);
	g_Game()->GetLevel()->AddDistRenderItem(side,DIST_RENDER_SIDE,dist,state);
	return 0;
	}


int SIDE_SetVar(lua_State *state)
	{
	int item=(int)lua_tonumber(state,1);
	lua_remove(state,1);
	g_Game()->GetLevel()->GetBlockSide(item)->GetVarHolder()->StoreVar(state);

	return 0;
	}


int SIDE_FindOfType(lua_State *state)
	{
	int offs=LUA_GET_INT(state);
	int start=LUA_GET_INT(state);
	std::string tn=LUA_GET_STRING(state);
	int res=g_Game()->GetLevel()->GetSideOfType(tn,start,offs);
	LUA_SET_INT(res);
	return 1;
	}



int SIDE_SetAlphaFunc(lua_State *state)
	{
	std::string f=LUA_GET_STRING(state);

	if (f=="") { g_SetAlphaFunc(NULL,""); }
	else { g_SetAlphaFunc(g_CallAccess(),f); }
	return 0;
	}

int SIDE_GetVar(lua_State *state)
	{
	int item=(int)lua_tonumber(state,1);
	lua_remove(state,1);
	g_Game()->GetLevel()->GetBlockSide(item)->GetVarHolder()->GetVar(state);

	return 1;
	}



int SIDE_GetBlock(lua_State *state)
	{
	int s=LUA_GET_INT(state);
	s=g_Game()->GetLevel()->GetBlockSide(s)->GetID();
	s/=6;
	LUA_SET_INT(s);
	return 1;
	}

int SIDE_GetType(lua_State *state)
	{
	int s=LUA_GET_INT(state);
	std::string str=g_Game()->GetLevel()->GetBlockSide(s)->GetTypeName();
	LUA_SET_STRING(str);
	return 1;
	}

int SIDE_CallVarChanged(lua_State *state)
	{
	std::string vn=LUA_GET_STRING(state);
	int s=LUA_GET_INT(state);
	g_Game()->GetLevel()->GetBlockSide(s)->Call_VarChanged(vn);
	return 0;
	}


int SIDE_GetEditorInfo(lua_State *state)
	{
	std::string def=LUA_GET_STRING(state);
	std::string what=LUA_GET_STRING(state);

	int b=LUA_GET_INT(state);
	std::string v=g_Game()->GetLevel()->GetBlockSide(b)->GetEditorInfo(what,def);
	LUA_SET_STRING(v);
	return 1;
	}



void LUA_SIDE_RegisterLib()
	{
	g_CuboLib()->AddFunc("SIDE_RenderQuad",SIDE_RenderQuad);
	g_CuboLib()->AddFunc("SIDE_GetTangent",SIDE_GetTangent);
	g_CuboLib()->AddFunc("SIDE_GetNormal",SIDE_GetNormal);
	g_CuboLib()->AddFunc("SIDE_GetMidpoint",SIDE_GetMidpoint);
	g_CuboLib()->AddFunc("SIDE_DistanceRender",SIDE_DistanceRender);
	g_CuboLib()->AddFunc("SIDE_GetBlock",SIDE_GetBlock);
	g_CuboLib()->AddFunc("SIDE_SetVar",SIDE_SetVar);
	g_CuboLib()->AddFunc("SIDE_GetVar",SIDE_GetVar);
	g_CuboLib()->AddFunc("SIDE_GetType",SIDE_GetType);
	g_CuboLib()->AddFunc("SIDE_GetEditorInfo",SIDE_GetEditorInfo);
	g_CuboLib()->AddFunc("SIDE_SetAlphaFunc",SIDE_SetAlphaFunc);
	g_CuboLib()->AddFunc("SIDE_FindOfType",SIDE_FindOfType);
	g_CuboLib()->AddFunc("SIDE_CallVarChanged",SIDE_CallVarChanged);
	}




////////////////////////////////////////////////


int BLOCK_GetNeighbor(lua_State *state)
	{
	Vector3d norm=Vector3FromStack(state);
	int blockid=LUA_GET_INT(state);
	int res=g_Game()->GetLevel()->GetBlock(blockid)->GetNeighbor(norm);
	LUA_SET_INT(res);
	return 1;
	}

int BLOCK_SetVar(lua_State *state)
	{
	int item=(int)lua_tonumber(state,1);
	lua_remove(state,1);
	g_Game()->GetLevel()->GetBlock(item)->GetVarHolder()->StoreVar(state);

	return 0;
	}

int BLOCK_SetCullRadius(lua_State *state)
	{
	double rad=LUA_GET_DOUBLE(state);
	int block=LUA_GET_INT(state);
	g_Game()->GetLevel()->GetBlock(block)->SetCullRadiusIfHigher(rad);
	return 0;
	}


int BLOCK_RemoveFromNeighbors(lua_State *state)
	{
	int block=LUA_GET_INT(state);
	g_Game()->GetLevel()->GetBlock(block)->ReleaseMeFromNext();
	return 0;
	}

int BLOCK_AttachToNeighbors(lua_State *state)
	{
	int block=LUA_GET_INT(state);
	g_Game()->GetLevel()->GetBlock(block)->ReAttachMeToNext();
	return 0;
	}

int BLOCK_HasTransparency(lua_State *state)
	{
	int block=LUA_GET_INT(state);
	int res=g_Game()->GetLevel()->GetBlock(block)->HasNoTransparency();
	LUA_SET_INT(!res);
	return 1;
	}

int BLOCK_GetVar(lua_State *state)
	{
	int item=(int)lua_tonumber(state,1);
	lua_remove(state,1);
	g_Game()->GetLevel()->GetBlock(item)->GetVarHolder()->GetVar(state);

	return 1;
	}



int BLOCK_GetPos(lua_State *state)
	{
	int b=LUA_GET_INT(state);
	Vector3d v=g_Game()->GetLevel()->GetBlock(b)->GetPos();
	LUA_SET_VECTOR3(v);
	return 1;
	}

int BLOCK_GetEditorInfo(lua_State *state)
	{
	std::string def=LUA_GET_STRING(state);
	std::string what=LUA_GET_STRING(state);

	int b=LUA_GET_INT(state);
	std::string v=g_Game()->GetLevel()->GetBlock(b)->GetEditorInfo(what,def);
	LUA_SET_STRING(v);
	return 1;
	}


int BLOCK_AtPos(lua_State *state)
	{
	Vector3d v=Vector3FromStack(state);
	CuboBlock* b=g_Game()->GetLevel()->GetBlockAtPos(v);
	int i;
	if (b) { i=b->GetID(); }
	else { i=-1; }
	LUA_SET_INT(i);
	return 1;
	}


int BLOCK_SetPos(lua_State *state)
	{
	Vector3d np=Vector3FromStack(state);
	int b=LUA_GET_INT(state);
	g_Game()->GetLevel()->GetBlock(b)->SetIPos((int)(np.x/(2*CUBO_SCALE)),(int)(np.y/(2*CUBO_SCALE)),(int)(np.z/(2*CUBO_SCALE)));

	return 0;
	}

int BLOCK_GetBlocking(lua_State *state)
	{
	int b=LUA_GET_INT(state);
	int res=0;
	if (b>=0) {
			res=g_Game()->GetLevel()->GetBlock(b)->Blocking();
			}
	LUA_SET_INT(res);
	return 1;
	}

int BLOCK_SetPosf(lua_State *state)
	{
	Vector3d np=Vector3FromStack(state);
	int b=LUA_GET_INT(state);
	g_Game()->GetLevel()->GetBlock(b)->SetPos(np);

	return 0;
	}

int BLOCK_GetScale(lua_State *state)
	{
	int b=LUA_GET_INT(state);
	float v=g_Game()->GetLevel()->GetBlock(b)->GetScale();
	LUA_SET_DOUBLE(v);
	return 1;
	}

int BLOCK_SetScale(lua_State *state)
	{
	double s=LUA_GET_DOUBLE(state);
	int b=LUA_GET_INT(state);
	g_Game()->GetLevel()->GetBlock(b)->SetScale(s);
	return 0;
	}






void LUA_BLOCK_RegisterLib()
	{
	g_CuboLib()->AddFunc("BLOCK_GetBlocking",BLOCK_GetBlocking);
	g_CuboLib()->AddFunc("BLOCK_GetNeighbor",BLOCK_GetNeighbor);
	g_CuboLib()->AddFunc("BLOCK_AtPos",BLOCK_AtPos);
	g_CuboLib()->AddFunc("BLOCK_GetPos",BLOCK_GetPos);
	g_CuboLib()->AddFunc("BLOCK_SetPos",BLOCK_SetPos);
	g_CuboLib()->AddFunc("BLOCK_SetPosf",BLOCK_SetPosf);
	g_CuboLib()->AddFunc("BLOCK_SetVar",BLOCK_SetVar);
	g_CuboLib()->AddFunc("BLOCK_GetVar",BLOCK_GetVar);
	g_CuboLib()->AddFunc("BLOCK_AttachToNeighbors",BLOCK_AttachToNeighbors);
	g_CuboLib()->AddFunc("BLOCK_RemoveFromNeighbors",BLOCK_RemoveFromNeighbors);
	g_CuboLib()->AddFunc("BLOCK_HasTransparency",BLOCK_HasTransparency);
	g_CuboLib()->AddFunc("BLOCK_SetScale",BLOCK_SetScale);
	g_CuboLib()->AddFunc("BLOCK_GetScale",BLOCK_GetScale);
	g_CuboLib()->AddFunc("BLOCK_GetEditorInfo",BLOCK_GetEditorInfo);
	g_CuboLib()->AddFunc("BLOCK_SetCullRadius",BLOCK_SetCullRadius);
	}
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
