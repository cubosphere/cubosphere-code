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

#include "cubolevels.hpp"
#include "cuboboxes.hpp"
#include "cuboutils.hpp"
#include "c3dobjects.hpp"
#include "definitions.hpp"
#include "luautils.hpp"
#include "posteffects.hpp"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <vector>
#include "game.hpp"
#include "globals.hpp"

#ifdef WIN32
#ifdef VCPP
#include <time.h>
#endif
#ifdef MINGW_CROSS_COMPILE
#include <ctime>
#endif
#endif


#ifdef WIN32
#ifdef VCPP

#else
#include <libgen.h>
#include <dirent.h>
#endif
#else
#include <libgen.h>
#include <dirent.h>
#endif

void TDistRenderObj::DistRender(TCamera *cam)
	{
	if (CullRadius>0) {
			if (!(cam->SphereInFrustum(CullCenter,CullRadius))) { return; }
			}
	if (type!=DIST_RENDER_SIDE) g_Game()->GetLevel()->SetLastRendered("");
	if (type==DIST_RENDER_ITEM)
			{
			g_Game()->GetLevel()->GetItem(id)->DistRender();
			}
	else if (type==DIST_RENDER_SIDE)
			{
			g_Game()->GetLevel()->GetBlockSide(id)->DistRender();
			}
	else if (type==DIST_RENDER_ACTOR)
			{

			g_Game()->GetActorMovement(id)->DistRender();
			}
	else if (type==DIST_RENDER_CUSTOM)
			{
			if (!callstate) { coutlog("Can't call Custom Distance Render if no state if set",1); return;}
			lua_getglobal(callstate, "CustomDistanceRender");
			lua_pushnumber(callstate, id);
			if (lua_pcall(callstate, 1, 0, 0) != 0)  /* do the call */
					{
					std::ostringstream os;
					os << "ERROR (in calling CustomDistanceRender')"<< " -> " << lua_tostring(callstate, -1);
					coutlog(os.str(),1);
					}
			}
	}


void TDistRenderObj::Render(TCamera *cam)
	{
	if (type==DIST_RENDER_ITEM)
			{
			g_Game()->GetLevel()->GetItem(id)->Render();
			}
	else if (type==DIST_RENDER_BLOCK)
			{
			g_Game()->GetLevel()->GetBlock(id)->Render(cam);
			}
	else if (type==DIST_RENDER_SIDE)
			{
			g_Game()->GetLevel()->GetBlockSide(id)->Render();
			}
	else if (type==DIST_RENDER_ACTOR)
			{

			g_Game()->GetActorMovement(id)->Render();
			}
	}

void TDistRenderObj::SpecialRender(TCamera *cam,std::string nam,int defrender)
	{
	if (type==DIST_RENDER_ITEM)
			{
			g_Game()->GetLevel()->GetItem(id)->SpecialRender(nam,defrender);
			}
	else if (type==DIST_RENDER_SIDE)
			{
			g_Game()->GetLevel()->GetBlockSide(id)->SpecialRender(nam,defrender);
			}
	else if (type==DIST_RENDER_ACTOR)
			{

			g_Game()->GetActorMovement(id)->SpecialRender(nam,defrender);
			}
	}



double TCuboLevel::Elapsed()
	{
	if (g_Game()->GetPlayer(0)->InCameraPan()) return 0;
	return g_Game()->GetElapsed()*timescale;
	}

void TCuboLevel::AddDistRenderItem(int id,int type,float dist,lua_State *callstate)
	{
	distrenderlist.push_back(TDistRenderObj(id,type,dist,callstate));
	}

void TCuboLevel::clear()
	{
	if (g_VerboseMode())
			{
			std::ostringstream oss;
			oss << "Clearing level! Release " << blocks.size() << " blocks from their neighbors";
			coutlog(oss.str());
			}
	for (unsigned i=0; i<blocks.size(); i++)
			{
			if (blocks[i]) {
					for (unsigned int s=0; s<6; s++)
						blocks[i]->SetNext(s,NULL);
					}
			}
	if (g_VerboseMode()) coutlog("Deleting blocks");
	for (unsigned i=0; i<blocks.size(); i++) if (blocks[i]) { delete blocks[i]; blocks[i]=NULL; }
	blocks.resize(0);
	for (unsigned i=0; i<items.size(); i++) if (items[i]) { delete items[i]; items[i]=NULL; }
	items.resize(0);
	}

void TCuboLevel::clearAll()
	{
	clear();
	tdefs.clear();
	bdefs.clear();
	idefs.clear();
	}

static int SideStringToSideInt(std::string s)
	{
	if (s=="up") return CUBO_UP;
	else if (s=="down") return CUBO_DOWN;
	else if (s=="left") return CUBO_LEFT;
	else if (s=="right") return CUBO_RIGHT;
	else if (s=="front") return CUBO_FRONT;
	else if (s=="back") return CUBO_BACK;
	else return -1;
	}

void TCuboLevel::AddBlock(int x,int y,int z,std::string bdefname)
	{
	blocks.push_back(new TCuboBlock());
	LastBlock()->SetID(blocks.size()-1);
	LastBlock()->SetIPos(x,y,z);
	LastBlock()->SetBlockType(bdefs.AddDef(bdefname));
	LastBlock()->Call_Constructor();
	}

void TCuboLevel::RemoveItemFromSide(TCuboBlockSide *s)
	{
	for (unsigned int i=0; i<items.size(); i++)
		if (items[i]->GetSide()==s)
				{
				delete items[i];
				items[i]=NULL;
				items.erase(items.begin()+i);
				}

	//REBUILDING BLOCK INDICES
	for (unsigned int i=0; i<items.size(); i++)
			{
			items[i]->SetID(i);
			}
	}

TCuboItem *TCuboLevel::GetItemOnSide(TCuboBlockSide *s)
	{
	for (unsigned int i=0; i<items.size(); i++)
		if (items[i]->GetSide()==s)
				{
				return (items[i]);
				}
	return NULL;
	}



void TCuboLevel::WriteLevelData(FILE *f)
	{
	for (unsigned int i=0; i<blocks.size(); i++) if (!blocks[i]->IsEditorSelector()) blocks[i]->WriteLevelData(f);
	}

void TCuboLevel::DeleteBlock(int i)
	{
	//Kill all attached items.
	for (int s=0; s<6; s++) RemoveItemFromSide(blocks[i]->GetBlockSide(s));
	delete blocks[i];
	blocks[i]=NULL;
	blocks.erase(blocks.begin()+ i);
//REBUILDING BLOCK INDICES
	for (unsigned int i=0; i<blocks.size(); i++)
			{
			blocks[i]->SetID(i);
			}
	}

void TCuboLevel::ChangeSide(int block, std::string sidestr,std::string sdefname)
	{
	TCuboBlock *b=GetBlock(block);
	int side=SideStringToSideInt(sidestr);
	if (side==-1)  { errorstring="Wrong Side: "+sidestr; return; }
	b->GetBlockSide(side)->SetSideType(bdefs.AddDef(sdefname));
	b->GetBlockSide(side)->Call_Constructor();
	}


void TCuboLevel::ChangeBlock(int block, std::string sdefname)
	{
	TCuboBlock *b=GetBlock(block);
	b->SetBlockType(bdefs.AddDef(sdefname));
	b->Call_Constructor();
	}


int TCuboLevel::AddItem(int block, std::string sidestr,std::string idefname)
	{
	TCuboBlock *b=GetBlock(block);
	int side=SideStringToSideInt(sidestr);
	if (side==-1)  { errorstring="Wrong Side: "+sidestr; return -1; }
	int idef=idefs.AddDef(idefname);
	int iid=items.size();
	items.push_back(new TCuboItem(iid,idef, b->GetBlockSide(side)));
	items[iid]->Call_Constructor();
	return iid;
	}

void TCuboLevel::LoadSky(std::string skyname)
	{
	g_Game()->LoadSky(skyname);
	}

int TCuboLevel::NumItemsOfType(std::string itemname)
	{
	if (itemname=="") return items.size();
	int num=0;
	for (unsigned int i=0; i<items.size(); i++)
			{
			if (items[i]->GetName()==itemname)
					{
					num++;
					}
			}
	return num;
	}



/*bool TCuboLevel::ReadBlock()
{
 string line="";
 int loopnr=0;
 blocks.push_back(new TCuboBlock());
 LastBlock()->SetID(blocks.size()-1);
 while ((!tr.isEoF()) && (line!="END"))
 {
   line=tr.NextLine();
   int linenr=tr.GetLineNr();
   loopnr++;
   if (loopnr==1) //Get an Int-Vector
   {
     TIntVect iv;
     if (!tr.ExtractIntVect(line,iv)) { errorline=linenr; errorstring="Can't parse Integer-Vector for Position: "+line; return false; }
     LastBlock()->SetIPos(iv.x,iv.y,iv.z);
   }
   else if (loopnr==2)
   {
     LastBlock()->SetBlockType(bdefs.AddDef(line));
     LastBlock()->Call_Constructor();
   }
   else
   {
     vector<string> tokens=tr.Seperate(line);
     if (tokens[0]=="SIDE")
     {
      if (tokens.size()<3) { errorline=linenr; errorstring="Wrong Side-Type Defintion: "+line; return false; }
       string sidestr=tokens[1];
       string typestr=tokens[2];
       int side=SideStringToSideInt(sidestr);
       if (side==-1)  { errorline=linenr; errorstring="Wrong Side: "+sidestr; return false; }

       LastBlock()->GetBlockSide(side)->SetSideType(bdefs.AddDef(typestr));
       LastBlock()->GetBlockSide(side)->Call_Constructor();
     }
     else if (tokens[0]=="ITEM")
     {

      if (tokens.size()<3) { errorline=linenr; errorstring="Wrong Item-Type Defintion: "+line; return false; }
       string sidestr=tokens[1];
       string typestr=tokens[2];
       int side=SideStringToSideInt(sidestr);
       if (side==-1)  { errorline=linenr; errorstring="Wrong Side: "+sidestr; return false; }
       int idef=idefs.AddDef(typestr);
       int iid=items.size();
       items.push_back(new TCuboItem(iid,idef, LastBlock()->GetBlockSide(side)));
       items[iid]->Call_Constructor();
     }
   }
 }
 return true;
} */

/*bool TCuboLevel::LoadFromFile(string fname)
{
 clear();
// bdefs.SetTextureDefServer(&tdefs);
 filename=fname;
 errorstring="";
 errorline=-1;
 if (!tr.LoadFile(fname)) { errorline=0;errorstring="Can't open "+fname; return false; }
 tr.RemoveComments();
 //Ok, begin parsing:
 while (!tr.isEoF())
 {
   string line=tr.NextLine();
   int linenr=tr.GetLineNr(); //For Error handling

   if (line=="BLOCK") if (!ReadBlock()) { clear(); return false; }


 }


 BindBlocksToNext();

// tdefs.Precache();

 return true;
} */

void TCuboLevel::CreateBBox()
	{
	bbmin.xyz(1000000,1000000,1000000);
	bbmax=bbmin*(-1.0);
	for (unsigned int i=0; i<blocks.size(); i++)
			{
			T3dVector t=blocks[i]->GetBBMax();
			bbmax.Maximize(t);
			t=blocks[i]->GetBBMin();
			bbmin.Minimize(t);
			}
//Expand it a bit
	/*T3dVector expand;
	float extrasize=40*CUBO_SCALE;
	expand.xyz(extrasize,extrasize,extrasize);
	bbmin=bbmin-expand;
	bbmax=bbmax+expand;*/
	}

int TCuboLevel::PointInBBox(T3dVector cp,float extrasize)
	{
	T3dVector diff=cp;
	diff=diff-bbmin;
	if (diff.MinValue()<-extrasize) return 0;
	diff=bbmax-cp;
	if (diff.MinValue()<-extrasize) return 0;
	return 1;
	}

void TCuboLevel::Reload()
	{
	LoadFromLua(filename);
	}

bool TCuboLevel::LoadFromLua(std::string fname)
	{
	//docollisions=1;
	g_Game()->GetTextures()->ResetTimerCounters();
	g_Game()->GetJoysticks()->ResetButtons();
	int clo=clock();
	loadnextlevel=0;
	g_Game()->Clear();
	lua.Reset();
	lua.Include(g_CuboLib());

	filename=fname;
	TCuboFile * finfo=GetFileName(fname,nextleveluserlevel==1 ? FILE_USERLEVEL : FILE_LEVEL,".ldef");
	if (!finfo) {std::string uls=(nextleveluserlevel==1 ? "Userlevel" : "Level"); coutlog("Cannot find "+uls+": "+fname,2); return false;}
	bool lief=lua.LoadFile(finfo,nextleveluserlevel==1 ? FILE_USERLEVEL : FILE_LEVEL,-1);
	delete finfo;
	if (!lief) return false;

	if (g_VerboseMode()) coutlog("Calling Level-Function");
	lua.CallVA("Level","");

	if (g_VerboseMode()) coutlog("Merging block sides");
	if (!(g_Vars()->GetVarString("EditorMode")=="1"))
			{
			BindBlocksToNext();

			}
	if (g_VerboseMode()) coutlog("Creating Bounding box");
	CreateBBox();
	if (g_VerboseMode()) coutlog("Init actors");
	for (unsigned int i=0; i<g_Game()->NumActors(); i++) g_Game()->GetActorMovement(i)->Init(this);
	leveltime=0;
	//timescale=1;
	g_Game()->GetTextures()->CoutTimerString();
	std::ostringstream os;
	os << ((clock()-clo)/(float)CLOCKS_PER_SEC) ;
	coutlog("Loading level (and data) took "+ os.str()+" seconds");
	return true;
	}

TCuboBlock* TCuboLevel::LastBlock()
	{
	if (blocks.size()==0) return NULL;
	return blocks[blocks.size()-1];
	}

TCuboBlock* TCuboLevel::GetBlock(int i)
	{
	if ((unsigned int)i>=blocks.size()) return NULL;
	return blocks[i];
	}

TCuboBlock *TCuboLevel::GetBlockFromType(std::string name,int i)
	{
	for (unsigned int b=i; b<blocks.size(); b++)
		if (blocks[b]->GetName()==name) return blocks[b];
	return NULL;
	}

void TCuboLevel::CheckCollides()
	{
	if (!docollisions) return ;
	for (unsigned int a=0; a<g_Game()->NumActors(); a++)
			{
#ifdef PARALLELIZE2
			#pragma omp parallel for
#endif
			for (unsigned int i=0; i<items.size(); i++) items[i]->CollisionCheckWithActor(a);
			}

	for (unsigned int a=0; a<g_Game()->NumActors(); a++)
			{
#ifdef PARALLELIZE2
			#pragma omp parallel for
#endif
			for (unsigned int i=0; i<blocks.size(); i++) blocks[i]->Call_CollisionCheck(a);
			}

	}

void TCuboLevel::Think()
	{
	leveltime+=Elapsed();

	if (lua.FuncExists("Think"))
			{
			lua.CallVA("Think","");
			}

#ifdef PARALLELIZE2
	#pragma omp parallel for
#endif
	for (unsigned int i=0; i<blocks.size(); i++) blocks[i]->Think();
#ifdef PARALLELIZE2
	#pragma omp parallel for
#endif
	for (unsigned int i=0; i<items.size(); i++) items[i]->Think();





	//And call the levels WinLooseConditions
	if (lua.FuncExists("CheckWinAndLoose"))
			{
			lua.CallVA("CheckWinAndLoose","");
			}
	}

void TCuboLevel::DrawHUD()
	{
	if (lua.FuncExists("DrawHUD"))
			{
			lua.CallVA("DrawHUD","");
			}
	}

void TCuboLevel::SendKey(int key,int down,int toggle)
	{
	if (lua.FuncExists("OnKeyPressed"))
			{
			lua.CallVA("OnKeyPressed","iii",key,down,toggle);
			}
	}

void TCuboLevel::SendJoyButton(int joy, int button,int dir,int down, int toggle)
	{
	if (lua.FuncExists("OnJoyButton"))
			{

			lua.CallVA("OnJoyButton","iiiii",joy,button,dir,down,toggle);
			}
	}


void TCuboLevel::JoyAxisChange(int joys,int axis,double val,double pval)
	{
	if (lua.FuncExists("OnJoyAxisChange"))
			{

			lua.CallVA("OnJoyAxisChange","iidd",joys,axis,val,pval);
			}

	}

T3dVector TCuboLevel::GetCenter()
	{
	T3dVector res=bbmin+bbmax;
	return res*0.5;
	}

tfloat TCuboLevel::GetRadius()
	{
	T3dVector res=bbmin-bbmax;
	return (res.length()*0.5);
	}

void TCuboLevel::PostThink()
	{
	if (loadnextlevel) {
			bool res=this->LoadFromLua(nextlevel);
			if (!res)
					{
					std::string s="levelnotexisting";
					SetNewLevel(s,0);
					}
			}
	}


bool sortfunc(const TDistRenderObj  o1, const TDistRenderObj o2 )
	{

	if (o1.GetHint()<o2.GetHint()) return 1;
	else if (o1.GetHint()>o2.GetHint()) return 0;
	else return o1.GetDist() > o2.GetDist();
	}


void TCuboLevel::FrameRenderEnd()
	{
	if (lua.FuncExists("FrameRenderEnd"))
			{
			lua.CallVA("FrameRenderEnd","");
			}
	}

void TCuboLevel::FrameRenderStart()
	{
	if (lua.FuncExists("FrameRenderStart"))
			{
			lua.CallVA("FrameRenderStart","");
			}
	}

void TCuboLevel::Render(TCamera *cam)
	{
	glEnable(GL_LIGHTING);


	if (lua.FuncExists("PreRender"))
			{
			lua.CallVA("PreRender","");
			}


	if (rlist.capacity()<6*blocks.size())  rlist.reserve(6*blocks.size());
//  cout << "pre "<< rlist.capacity() << endl;
	rlist.clear();
//  cout << rlist.capacity() << endl;

	int siderender[6];
	for (unsigned int i=0; i<blocks.size(); i++)
			{

			if (!(cam->SphereInFrustum(blocks[i]->GetPos(),blocks[i]->GetCullRadius()))) continue;
			T3dVector diff=blocks[i]->GetPos()-cam->getPos();
			tfloat dist=diff*diff;
			blocks[i]->MustRenderSides(cam,siderender);
			for (unsigned int s=0; s<6; s++)
					{
					// cout << "Block i=" << i << "  side " << s << " has " << siderender[1] << endl;
					if (siderender[s])
							{
							rlist.push_back(TDistRenderObj(blocks[i]->GetBlockSide(s)->GetID(),DIST_RENDER_SIDE,dist,NULL ));
							rlist.back().SetHint(blocks[i]->GetBlockSide(s)->GetTypeName() );
							}
					}
			//rlist.push_back(TDistRenderObj(i,DIST_RENDER_BLOCK,-dist ));
			//blocks[i]->Render(cam);
			}
	sort(rlist.begin(),rlist.end(),sortfunc);
	SetLastRendered("");
	for (unsigned int i=0; i<rlist.size(); i++) {
			//cout << "Rendering " <<rlist[i].GetHint() << " dist " << rlist[i].GetDist() << endl;
			rlist[i].Render(cam);
			}
	SetLastRendered("");
	if (lua.FuncExists("PostRender"))
			{
			lua.CallVA("PostRender","");
			}

///TODO: ITEMS CULLING??? CAN BE BAD.. ESP by Item->GetPos is a fixed c-intern result
	for (unsigned int i=0; i<items.size(); i++)
			{
			if (!(cam->SphereInFrustum(items[i]->GetPos(),sqrt(3.0)*CUBO_SCALE))) continue;
			items[i]->Render();
			}


	}


void TCuboLevel::SpecialRender(TCamera *cam,std::string nam,int defrender)
	{
	glEnable(GL_LIGHTING);

	if (lua.FuncExists("PreSpecialRender"))
			{
			lua.CallVA("PreSpecialRender","");
			}
	else if (defrender==1)
			{	if (lua.FuncExists("PreRender"))
					{
					lua.CallVA("PreRender","");
					}
			}
	else if (defrender==0 && g_PostEffect())  g_PostEffect()->CallDefaultSpecialRender(nam,"prelevel",0);

	if (rlist.capacity()<6*blocks.size())  rlist.reserve(6*blocks.size());
//  cout << "pre "<< rlist.capacity() << endl;
	rlist.clear();
//  cout << rlist.capacity() << endl;

	int siderender[6];
	for (unsigned int i=0; i<blocks.size(); i++)
			{

			if (!(cam->SphereInFrustum(blocks[i]->GetPos(),blocks[i]->GetCullRadius()))) continue;
			T3dVector diff=blocks[i]->GetPos()-cam->getPos();
			tfloat dist=diff*diff;
			blocks[i]->MustRenderSides(cam,siderender); ///TODO: change to MustSpecialRender?
			for (unsigned int s=0; s<6; s++)
					{
					// cout << "Block i=" << i << "  side " << s << " has " << siderender[1] << endl;
					if (siderender[s])
							{
							rlist.push_back(TDistRenderObj(blocks[i]->GetBlockSide(s)->GetID(),DIST_RENDER_SIDE,dist,NULL ));
							rlist.back().SetHint(blocks[i]->GetBlockSide(s)->GetTypeName() );
							}
					}
			//rlist.push_back(TDistRenderObj(i,DIST_RENDER_BLOCK,-dist ));
			//blocks[i]->Render(cam);
			}
	sort(rlist.begin(),rlist.end(),sortfunc);
	SetLastRendered("");
	for (unsigned int i=0; i<rlist.size(); i++) {

			rlist[i].SpecialRender(cam,nam,defrender);
			}
	SetLastRendered("");
	if (lua.FuncExists("PostSpecialRender"))
			{
			lua.CallVA("PostSpecialRender","");
			}
	else if (defrender==1)
			{	if (lua.FuncExists("PostRender"))
					{
					lua.CallVA("PostRender","");
					}
			}
	else if (defrender==0 && g_PostEffect())  g_PostEffect()->CallDefaultSpecialRender(nam,"postlevel",0);

///TODO: ITEMS CULLING??? CAN BE BAD.. ESP by Item->GetPos is a fixed c-intern result
	for (unsigned int i=0; i<items.size(); i++)
			{
			if (!(cam->SphereInFrustum(items[i]->GetPos(),sqrt(3.0)*CUBO_SCALE))) continue;
			items[i]->SpecialRender(nam,defrender);
			}
	}

void TCuboLevel::SortDistRenderList()
	{
	sort(distrenderlist.begin(),distrenderlist.end() );


	}

void TCuboLevel::LastDistanceRenderCull(T3dVector center,double rad)
	{
	distrenderlist.back().SetCulling(center,rad);
	}

void TCuboLevel::DistRender(TCamera *cam)
	{
	SetLastRendered("");
	glEnable(GL_BLEND);
	for (unsigned int i=0; i<distrenderlist.size(); i++)
			{
			distrenderlist[i].DistRender(cam);
			}
	distrenderlist.clear();
	SetLastRendered("");
	}

TCuboBlock *TCuboLevel::GetBlockAtPos(T3dVector p)
	{
	for (unsigned int i=0; i<blocks.size(); i++)
			{
			T3dVector diff=p-blocks[i]->GetPos();
			//Now check if we are inside the block
			if (diff.MaxAbsValue()<=CUBO_SCALE)
					{
					if (!(blocks[i]->IsEditorSelector()))
						return blocks[i];
					}
			}
	return NULL;
	}


TCuboBlock *TCuboLevel::GetBlockAtIPos(int x,int y,int z)
	{
	T3dVector p(2*CUBO_SCALE*x,2*CUBO_SCALE*y,2*CUBO_SCALE*z);
	return GetBlockAtPos(p);
	}


void TCuboLevel::BindBlocksToNext()
	{
	for (unsigned int i=0; i<blocks.size(); i++)
			{
			for (int j=0; j<6; j++)
					{
					T3dVector p=blocks[i]->GetPos();
					T3dVector n=s_CuboNormals[j];
					p=p+(n*(2*CUBO_SCALE));
					TCuboBlock *nb=GetBlockAtPos(p);
					if (!nb) continue;
					blocks[i]->SetNext(j,nb);
					}
			}
	}

std::string TCuboLevel::GetErrorString()
	{
	if (errorline==-1) return "";
	return ">LEVEL("+filename+" : "+IntToString(errorline)+"): \n     "+errorstring;
	}


int TCuboLevel::LoadTexDef(std::string name)
	{
	return tdefs.AddDef(name);
	}


TCuboBlockSide *TCuboLevel::GetBlockSide(int id)
	{
	//ok, we find the parent block by the division by 6
	int p=id/6;
	return (blocks[p]->GetBlockSide(id % 6));
	}


TTraceResult TCuboLevel::TraceLine(T3dVector start,T3dVector dir,int onlyblocking)
	{
	TTraceResult res;
	res.hit=0;
	float mindist=100000000*CUBO_SCALE;
	int minblock=-1;
	int minside=-1;

	for (unsigned int b=0; b<blocks.size(); b++)
			{
			if (blocks[b]->IsEditorSelector()) continue;

			T3dVector diff=blocks[b]->GetPos();
			diff=diff-start;
			if (res.hit)  if (diff.length()-sqrt(3.0)*CUBO_SCALE>mindist) continue; //Early check
			//now shoot a ray at each side
			for (unsigned int s=0; s<6; s++)
					{
					TCuboBlockSide *bs=blocks[b]->GetBlockSide(s);
					T3dVector n=bs->GetNormal();
					if (n*dir>-0.0000001) continue;
					T3dVector t=bs->GetTangent();
					T3dVector bi=n.cross(t);
					//Enter them into a matrix
					T3dMatrix M;
					M.setCol(0,t);
					M.setCol(1,bi);
					M.setCol(2,dir*(-1.0));
					T3dMatrix invM=M.inverse();
					T3dVector munulambda=start-bs->GetMidpoint();
					munulambda=invM*munulambda;
					if ( (abs(munulambda.x)>CUBO_SCALE) || (abs(munulambda.y)>CUBO_SCALE)) continue;
					if ((mindist<munulambda.z) || (munulambda.z<0)) continue;
					if (!(blocks[b]->Blocking()) && onlyblocking) continue;
					res.hit=1;
					mindist=munulambda.z;
					minblock=b;
					minside=s;
					}
			}

	if (res.hit)
			{
			res.dist=mindist;
			res.block=minblock;
			res.side=minside;
			}
	else
			{
			res.side=-1; res.block=-1; res.dist=-1;
			}

	return res;
	}


int TCuboLevel::GetSideOfType(std::string tname,int startside,int offs)
	{
	std::vector<int> lookup;
	for (unsigned int i=0; i<blocks.size(); i++)
			{
			for (unsigned int s=0; s<6; s++)
					{
					if (blocks[i]->GetBlockSide(s)->GetTypeName()==tname)
							{
							lookup.push_back(blocks[i]->GetBlockSide(s)->GetID());
							}
					}
			}
	//Ok, we now know all sides with this type.. Go and find the start index
	int startindex=-1;
	for (unsigned int i=0; i<lookup.size()-1; i++)
			{
			if ((startside<=lookup[i]) && (startside>lookup[i+1]))
					{
					startindex=i;
					break;
					}
			}
	if (startindex==-1) startindex=lookup.size()-1;
	int index=(startindex+offs);
	if (index<0) index=-index;
	index=index % (lookup.size());
	return lookup[index];
	}

std::string TCuboLevel::CheckDefExchange(std::string defname,std::string deftype)
	{
	if (lua.FuncExists("CheckDefExchange"))
			{
			char *res=NULL;
			//coutlog("Calling CheckDefEx with "+defname+"  "+deftype);
			lua.CallVA("CheckDefExchange","ss>s",defname.c_str(),deftype.c_str(),&res);
			//coutlog(res);
			if (!res) return defname;
			return res;
			}
	else return defname;
	}









////////////////////LUA-IMPLEM////////////////////


int LEVEL_GetEditorSelector(lua_State *state)
	{
	TCuboBlock* b=g_Game()->GetLevel()->GetBlockFromType("_selection",0);
	int i;
	if (b) i=b->GetID(); else i=-1;
	LUA_SET_INT(i);
	return 1;
	}


int LEVEL_Clear(lua_State *state)
	{
	g_Game()->GetLevel()->clear();
	return 0;
	}

int LEVEL_AddBlock(lua_State *state)
	{
	std::string bdefname=LUA_GET_STRING;
	int z=LUA_GET_INT;
	int y=LUA_GET_INT;
	int x=LUA_GET_INT;

	std::string nbdefname=g_Game()->GetLevel()->CheckDefExchange(bdefname,"block");

	if (g_VerboseMode()) { T3dVector v(x,y,z); coutlog("  Adding block "+nbdefname+" ("+bdefname+") at "+v.toString()); }

	g_Game()->GetLevel()->AddBlock(x,y,z,nbdefname);
	return 0;
	}

int LEVEL_DeleteBlock(lua_State *state)
	{
	int i=LUA_GET_INT;
	g_Game()->GetLevel()->DeleteBlock(i);
	return 0;
	}

int LEVEL_ChangeSide(lua_State *state)
	{
	std::string bdefname=LUA_GET_STRING;
	std::string sidestr=LUA_GET_STRING;
	int blockid=LUA_GET_INT;
	std::string nbdefname=g_Game()->GetLevel()->CheckDefExchange(bdefname,"side");
	if (g_VerboseMode()) { std::ostringstream oss; oss<<blockid<<" - " << sidestr; coutlog("  Changing block side to "+nbdefname+" ("+bdefname+") of block"+oss.str()); }

	g_Game()->GetLevel()->ChangeSide(blockid,sidestr,nbdefname);
	return 0;
	}

int LEVEL_ChangeBlock(lua_State *state)
	{
	std::string bdefname=LUA_GET_STRING;
	int blockid=LUA_GET_INT;
	bdefname=g_Game()->GetLevel()->CheckDefExchange(bdefname,"block");
	g_Game()->GetLevel()->ChangeBlock(blockid,bdefname);
	return 0;
	}


int LEVEL_CustomDistanceRender(lua_State *state)
	{
	float dist=LUA_GET_DOUBLE;
	int id=LUA_GET_INT;
	g_Game()->GetLevel()->AddDistRenderItem(id,DIST_RENDER_CUSTOM,dist,state);
	return 0;
	}

int LEVEL_LastDistanceRenderCull(lua_State *state)
	{
	double rad=LUA_GET_DOUBLE;
	T3dVector center=Vector3FromStack(state);
	g_Game()->GetLevel()->LastDistanceRenderCull(center,rad);
	return 0;
	}

int LEVEL_Activate(lua_State *state)
	{
	g_Game()->SetGameActive(1);
	return 0;
	}
int LEVEL_Deactivate(lua_State *state)
	{
	g_Game()->SetGameActive(0);
	return 0;
	}

int LEVEL_AddItem(lua_State *state)
	{
	std::string idefname=LUA_GET_STRING;
	std::string sidestr=LUA_GET_STRING;
	int blockid=LUA_GET_INT;
	std::string nidefname=g_Game()->GetLevel()->CheckDefExchange(idefname,"item");
	int res;
//cout <<"Idefname |" << idefname << "| " << endl;
	if (g_VerboseMode()) { std::ostringstream oss; oss<<blockid<<" - " << sidestr; coutlog("  Adding item "+nidefname+" ("+idefname+") of block"+oss.str()); }
	if (nidefname=="") res=-1;
	else
		res=g_Game()->GetLevel()->AddItem(blockid,sidestr,nidefname);
	LUA_SET_INT(res);
	return 1;
	}
int LEVEL_LastBlock(lua_State *state)
	{
	int b=g_Game()->GetLevel()->LastBlock()->GetID();
	LUA_SET_INT(b);
	return 1;
	}
int LEVEL_LoadSky(lua_State *state)
	{
//string sname=LUA_GET_STRING;
	g_Game()->GetLevel()->LoadSky("");
	return 0;
	}

int LEVEL_Load(lua_State *state)
	{
	std::string lname=LUA_GET_STRING;
	g_Game()->GetLevel()->SetNewLevel(lname,0);
	return 0;
	}

int LEVEL_LoadUserLevel(lua_State *state)
	{
	std::string lname=LUA_GET_STRING;
	g_Game()->GetLevel()->SetNewLevel(lname,1);

	return 0;
	}

int LEVEL_SetCollisionChecksActive(lua_State *state)
	{
	int i=LUA_GET_INT;

	g_Game()->GetLevel()->SetCollisionChecksActive(i);
	return 0;
	}

int LEVEL_TraceLine(lua_State *state)
	{
	T3dVector dir=Vector3FromStack(state);
	T3dVector start=Vector3FromStack(state);
	TTraceResult tr=g_Game()->GetLevel()->TraceLine(start,dir);
	lua_newtable(state);
	lua_pushstring(state, "hit");
	lua_pushnumber(state, tr.hit);
	lua_rawset(state, -3);
	lua_pushstring(state, "dist");
	lua_pushnumber(state, tr.dist);
	lua_rawset(state, -3);
	lua_pushstring(state, "block");
	lua_pushnumber(state, tr.block);
	lua_rawset(state, -3);
	lua_pushstring(state, "side");
	lua_pushnumber(state, 6*tr.block+tr.side);
	lua_rawset(state, -3);
	return 1;
	}

int LEVEL_Render(lua_State *state) //Used for preview pics
	{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);


	g_Game()->PreRender(viewport[2],viewport[3]);
	g_Game()->RenderPass();
	g_Game()->AfterRenderLevel();
	return 0;
	}

int LEVEL_Restart(lua_State *state)
	{
	g_Game()->GetLevel()->SetNewLevel(g_Game()->GetLevel()->GetLevelFileName(),-1);
	return 0;
	}

int LEVEL_NumItems(lua_State *state)
	{
	std::string iname=LUA_GET_STRING;
	int num=g_Game()->GetLevel()->NumItemsOfType(iname);
	LUA_SET_INT(num);
	return 1;
	}

int LEVEL_NumBlocks(lua_State *state)
	{
//string iname=LUA_GET_STRING;
	int num=g_Game()->GetLevel()->GetNumBlocks();
	LUA_SET_INT(num);
	return 1;
	}


int LEVEL_GetCenter(lua_State *state)
	{
	T3dVector c=g_Game()->GetLevel()->GetCenter();
	LUA_SET_VECTOR3(c);
	return 1;
	}

int LEVEL_GetTime(lua_State *state)
	{
	double c=g_Game()->GetLevel()->Time();
	LUA_SET_DOUBLE(c);
	return 1;
	}

int LEVEL_GetElapsed(lua_State *state)
	{
	double c=g_Game()->GetLevel()->Elapsed();
	LUA_SET_DOUBLE(c);
	return 1;
	}

int LEVEL_SetSideItem(lua_State *state)
	{
	std::string iname=LUA_GET_STRING;
	int s=LUA_GET_INT;
	iname=g_Game()->GetLevel()->CheckDefExchange(iname,"item");
	g_Game()->GetLevel()->GetBlockSide(s)->SetSideItem(iname);
	return 0;
	}

int LEVEL_SetTimeScale(lua_State *state)
	{
	double c=LUA_GET_DOUBLE;
	g_Game()->GetLevel()->SetTimeScale(c);
//LUA_SET_DOUBLE(0.0);
	return 0;
	}

int LEVEL_GetTimeScale(lua_State *state)
	{
	LUA_SET_DOUBLE(g_Game()->GetLevel()->GetTimeScale());
	return 1;
	}

int LEVEL_FileBegin(lua_State *state)
	{
	g_Game()->GetLevel()->CreateBBox(); //For creating the picture

	std::string s=LUA_GET_STRING;
	cls_FileWriteable *fw=g_BaseFileSystem()->GetFileForWriting(s,true);
	if (!fw) {LUA_SET_INT(0); return 1;}
	if (!fw->IsHDDFile()) {LUA_SET_INT(0); return 1;}
	s=fw->GetHDDName();
	delete fw;


	char dirs[2048] ;
	sprintf(dirs,"%s",s.c_str());
	std::string thedir=dirname(dirs);
	//coutlog("Trying to create the dir: "+thedir);
	RecursiveMKDir(thedir);

	FILE* f=fopen(s.c_str(),"wt");
	unsigned long int i=(unsigned long int)f;
	LUA_SET_INT(i);
	return 1;
	}

int LEVEL_FileWrite(lua_State *state)
	{
	std::string k=LUA_GET_STRING;
	unsigned long int c=LUA_GET_ULINT;
	FILE *f=(FILE *)c;
	fprintf(f,"%s\n",k.c_str());
	return 0;
	}

int LEVEL_FileData(lua_State *state)
	{
	unsigned long int c=LUA_GET_ULINT;
	FILE *f=(FILE *)c;
	g_Game()->GetLevel()->WriteLevelData(f);
	//fprintf(f,"%s",k.c_str());
	////TODO: Call the LEVEL to export the data
	return 0;
	}

int LEVEL_FileEnd(lua_State *state)
	{
	unsigned long int c=LUA_GET_ULINT;
	FILE *f=(FILE *)c;
	fclose(f);
	return 0;
	}

int LEVEL_GetRadius(lua_State *state)
	{
	double c=g_Game()->GetLevel()->GetRadius();
	LUA_SET_DOUBLE(c);
	return 1;
	}



void LUA_LEVEL_RegisterLib()
	{

	g_CuboLib()->AddFunc("LEVEL_LastDistanceRenderCull",LEVEL_LastDistanceRenderCull);
	g_CuboLib()->AddFunc("LEVEL_CustomDistanceRender",LEVEL_CustomDistanceRender);
	g_CuboLib()->AddFunc("LEVEL_Clear",LEVEL_Clear);
	g_CuboLib()->AddFunc("LEVEL_AddBlock",LEVEL_AddBlock);
	g_CuboLib()->AddFunc("LEVEL_DeleteBlock",LEVEL_DeleteBlock);
	g_CuboLib()->AddFunc("LEVEL_ChangeSide",LEVEL_ChangeSide);
	g_CuboLib()->AddFunc("LEVEL_ChangeBlock",LEVEL_ChangeBlock);
	g_CuboLib()->AddFunc("LEVEL_AddItem",LEVEL_AddItem);
	g_CuboLib()->AddFunc("LEVEL_LastBlock",LEVEL_LastBlock);
	g_CuboLib()->AddFunc("LEVEL_LoadSky",LEVEL_LoadSky);
	g_CuboLib()->AddFunc("LEVEL_NumItems",LEVEL_NumItems);
	g_CuboLib()->AddFunc("LEVEL_Load",LEVEL_Load);
	g_CuboLib()->AddFunc("LEVEL_LoadUserLevel",LEVEL_LoadUserLevel);
	g_CuboLib()->AddFunc("LEVEL_Restart",LEVEL_Restart);
	g_CuboLib()->AddFunc("LEVEL_GetCenter",LEVEL_GetCenter);
	g_CuboLib()->AddFunc("LEVEL_GetRadius",LEVEL_GetRadius);
	g_CuboLib()->AddFunc("LEVEL_Activate",LEVEL_Activate);
	g_CuboLib()->AddFunc("LEVEL_Deactivate",LEVEL_Deactivate);
	g_CuboLib()->AddFunc("LEVEL_GetTime",LEVEL_GetTime);
	g_CuboLib()->AddFunc("LEVEL_GetElapsed",LEVEL_GetElapsed);
	g_CuboLib()->AddFunc("LEVEL_SetTimeScale",LEVEL_SetTimeScale);
	g_CuboLib()->AddFunc("LEVEL_GetTimeScale",LEVEL_GetTimeScale);
	g_CuboLib()->AddFunc("LEVEL_TraceLine",LEVEL_TraceLine);
	g_CuboLib()->AddFunc("LEVEL_SetSideItem",LEVEL_SetSideItem);
	g_CuboLib()->AddFunc("LEVEL_FileBegin",LEVEL_FileBegin);
	g_CuboLib()->AddFunc("LEVEL_FileEnd",LEVEL_FileEnd);
	g_CuboLib()->AddFunc("LEVEL_FileWrite",LEVEL_FileWrite);
	g_CuboLib()->AddFunc("LEVEL_FileData",LEVEL_FileData);
	g_CuboLib()->AddFunc("LEVEL_GetEditorSelector",LEVEL_GetEditorSelector);
	g_CuboLib()->AddFunc("LEVEL_NumBlocks",LEVEL_NumBlocks);
	g_CuboLib()->AddFunc("LEVEL_Render",LEVEL_Render);
	g_CuboLib()->AddFunc("LEVEL_SetCollisionChecksActive",LEVEL_SetCollisionChecksActive);
	}
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
