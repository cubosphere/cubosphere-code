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

#include "cubopathfind.hpp"
#include "vectors.hpp"
#include "cuboboxes.hpp"
#include "game.hpp"
#include <iostream>
#include <string>

#define PATH_INFINITY 1000000

static CuboPathGraphServer gpgs;

CuboPathGraphServer * g_PathGraphs() {return &gpgs;}

CuboPathNode::CuboPathNode()
	{
	sideindex=-1;
	next[0]=next[1]=next[2]=next[3]=-1;
	}

CuboPathNode::CuboPathNode(int si)
	{
	sideindex=si;
	next[0]=next[1]=next[2]=next[3]=-1;
	}

int DirVectToSide(Vector3d dirvect)
	{
	Vector3d n=dirvect;
	n.normalize();
	for (int i=0; i<6; i++)
			{	Vector3d kn;
			kn=s_CuboNormals[i];
			if ((kn*n)>0.5) return i;
			}
	return -1;
	}


int CuboPathNode::GetNextSideID(int dir)
	{
	if (sideindex<0) return -1;
	Vector3d t,n,d;
	CuboBlockSide *bs=g_Game()->GetLevel()->GetBlockSide(sideindex);
	if (!bs) return -1;
	t= bs->GetTangent();
	n= bs->GetNormal();
	switch (dir)
			{
			case 0: d=t; break;
			case 1: d=t.cross(n); break;
			case 2: d=-t; break;
			case 3: d=n.cross(t); break;
			default:
				d=t;
			}

	Vector3d checkvect;
	checkvect=bs->GetBlock()->GetPos();
	Vector3d rv=d+n;
	checkvect=checkvect+rv*(2*CUBO_SCALE);
	CuboBlock *b;

	b=g_Game()->GetLevel()->GetBlockAtPos(checkvect);
	if (b && (b->Blocking())) //ROLL UPWARDS!
			{
			int si=DirVectToSide(-d);
			CuboBlockSide *nbs=b->GetBlockSide(si);
			return nbs->GetID();
			}
	else
			{
			b=bs->GetBlock()->GetNext(DirVectToSide(d));
			// cout << " b is " << b << endl;
			if (b)
					{
					int si=DirVectToSide(n);
					CuboBlockSide *nbs=b->GetBlockSide(si);
					return nbs->GetID();
					}
			else //The last possibility is a downward roll
					{
					b=bs->GetBlock()->GetNext(DirVectToSide(d.cross(n)));
					CuboBlock *b2=bs->GetBlock()->GetNext(DirVectToSide(n.cross(d)));
					if ((!b) && (!b2))
							{
							int si=DirVectToSide(d);
							CuboBlockSide *nbs=bs->GetBlock()->GetBlockSide(si);
							return nbs->GetID();
							}
					}
			}

	return -1;
	}


////////////////////

int CuboPathGraph::EdgeBetween(int i,int j)
	{
	for (int m=0; m<4; m++)
			{
			if (nodes[i].GetNext(m)==j) return 1;
			}
	return 0;
	}

void CuboPathGraph::GraphFromSide(int startindex,lua_State *state,std::string addcb)
	{
	nodes.clear(); path.clear(); next.clear();
	if (startindex<0) return;

	CuboBlockSide *bs=g_Game()->GetLevel()->GetBlockSide(startindex);
	if (!bs) return;

	cstate=state;
	callbfunc=addcb;

	AddNode(startindex);

	int N=nodes.size();

//Link the nodes
	for (int j=0; j<N; j++)
		for (int m=0; m<4; m++)
				{
				int ns=nodes[j].GetNextSideID(m);
				if (ns<0) continue;
				int mn=GetNodeIDFromSideID(ns);
				if (mn<0) continue;
				nodes[j].SetNext(m,mn);
				}

//


	path.resize(N*N); next.resize(N*N);

//Fill the initial path array
	for (int j=0; j<N; j++)
			{
			for (int i=0; i<N; i++)
					{
					if (i==j) path[i+N*j]=0;
					else if (EdgeBetween(i,j)) path[i+N*j]=1;
					else path[i+N*j]=PATH_INFINITY;
					next[i+N*j]=-1;
					}

			}

	//Floyd-Algo

	for (int k=0; k<N; k++)
		for (int i=0; i<N; i++)
			for (int j=0; j<N; j++)
					{
					if (path[i+k*N] + path[k+j*N] < path[i+j*N])
							{
							path[i+j*N] = path[i+k*N]+path[k+N*j];
							next[i+j*N] = k;
							}
					}
	}

std::string CuboPathGraph::GetPath(int i,int j)
	{
	int N=nodes.size();
	if (path[i+N*j]>=PATH_INFINITY)
		return "no path";

	int intermediate = next[i+N*j];
	if (intermediate==-1)
		return " <-> ";   /* there is an edge from i to j, with no vertices between */
	else
			{
			std::ostringstream oss; oss << nodes[intermediate].GetSideID();
			return GetPath(i,intermediate)+" "+oss.str()+" " + GetPath(intermediate,j);
			}
	}


int CuboPathGraph::MayAddNode(int from,int to)
	{
	if (cstate && callbfunc!="")
			{
			lua_getglobal(cstate, callbfunc.c_str());  /* get function */
			lua_pushnumber(cstate, from);
			lua_pushnumber(cstate, to);
			if (lua_pcall(cstate, 2, 1, 0) != 0)  /* do the call */
					{
					std::ostringstream os;
					os << "ERROR (in calling '"<<callbfunc <<"')"<< " -> " << lua_tostring(cstate, -1);
					coutlog(os.str());
					return 1;
					}
			//Get the result
			if (!lua_isnumber(cstate, -1))
					{
					std::ostringstream os; os << "Error running function '" << callbfunc << "' : wrong result type" ; coutlog(os.str(),1);
					return 1;
					}
			int res = (int)lua_tonumber(cstate, -1);
			lua_pop(cstate,1);
			return res;
			}
	else return 1;
	}

CuboPathNode* CuboPathGraph::AddNode(int sindex)
	{
	int s=GetNodeIDFromSideID(sindex);
	if (s==-1)
			{
			//   cout << "Adding " << sindex << endl;
			int res=nodes.size();
			nodes.push_back(CuboPathNode(sindex));
			//Now we have to add all sides
			for (int m=0; m<4; m++)
					{
					int ns=nodes[res].GetNextSideID(m);
					if (ns>=0)
							{
							if (MayAddNode(sindex,ns))
								AddNode(ns);
							//   nodes[res].SetNext(m,AddNode(ns));
							//  int mirror[4]={2,3,0,1};
							// nodes[res].GetNext(m)->SetNext(mirror[m],&(nodes[res]));
							}
					}
			return &(nodes[res]);
			}
	else
			{
			return &(nodes[s]);
			}
	}


int CuboPathGraph::GetNodeIDFromSideID(int ID)
	{
	for (unsigned int i=0; i<nodes.size(); i++)
			{
			if (ID==nodes[i].GetSideID()) return i;
			}
	return -1;
	}


std::string CuboPathGraph::GetPathFromTo(int startbs,int endbs)
	{
	int si=GetNodeIDFromSideID(startbs);
	int ei=GetNodeIDFromSideID(endbs);
//cout << "Trace from "<<startbs<< "("<< si <<") to "<<endbs<<" (" << ei << ")" << endl;
	if (si==-1 || ei==-1) return "";
	return GetPath(si,ei);
	}

int CuboPathGraph::GetDistance(int startbs,int endbs)
	{
	int si=GetNodeIDFromSideID(startbs);
	int ei=GetNodeIDFromSideID(endbs);


	if (si==-1 || ei==-1) return -1;
	int N=nodes.size();
	if (path[si+N*ei]>=PATH_INFINITY)
		return -1; //No path
	return path[si+N*ei];
	}

std::string CuboPathGraph::GetNextMove(int startbs,int startrot,int endbs)
	{
	int si=GetNodeIDFromSideID(startbs);
	int ei=GetNodeIDFromSideID(endbs);


	if (si==-1 || ei==-1) return "";
	int N=nodes.size();
	if (path[si+N*ei]>=PATH_INFINITY)
		return ""; //No path

//cout << "Trace from "<<startbs<< "("<< si <<") , rot " << startrot << "to "<<endbs<<" (" << ei << ")" << endl;


	int i=si;
	int j=ei;
	int n,m;
	m=-1;
	n = next[i+N*j];
	while (n!=-1) { m=n; n=next[i+N*n];}
//  cout << "Next side should be " << nodes[m].GetSideID() << endl;
	if (m==-1) m=ei;

	for (int drot=0; drot<4; drot++)
			{
			if (nodes[si].GetNext(drot)==m)
					{
					//Right node, obtain the move
					int deltarot=drot-startrot; if (deltarot<0) deltarot=4+deltarot;
					//   cout << "deltarot="<<drot<<" - " << startrot <<" is" << deltarot << "    ";
					switch (deltarot)
							{
							case 0: return "f";
							case 1: return "r";
							case 2: return "l";
							case 3: return "l";
							}
					}
			}

//cout << "Trace from "<<startbs<< "("<< si <<") , rot " << startrot << "to "<<endbs<<" (" << ei << ")" << endl;

	return "";
	}



std::string CuboPathGraph::GetEscapeMove(int startbs,int startrot,int endbs)
	{
	int si=GetNodeIDFromSideID(startbs);
	int ei=GetNodeIDFromSideID(endbs);

	if (si==-1 || ei==-1) return "";
	int N=nodes.size();
	if (path[si+N*ei]>=PATH_INFINITY)
		return ""; //No path

	int i=si;
	int j=ei;
	int n,m;
	m=-1;
	n = next[i+N*j];
	while (n!=-1) { m=n; n=next[i+N*n];}
//  cout << "Next side should be " << nodes[m].GetSideID() << endl;
	if (m==-1) m=ei;


	int towardsrot=0;

	for (int drot=0; drot<4; drot++)
			{
			if (nodes[si].GetNext(drot)==m)
					{
					towardsrot=drot; break;
					}
			}

	//TCuboPathNode *mnode=NULL;
	int mnode=-1;
	//First check the opposide direction
	int mirror[4]= {2,3,0,1};
	if (nodes[si].GetNext(mirror[towardsrot])!=-1)
			{
			mnode=nodes[si].GetNext(mirror[towardsrot]);
			}
	else //Check left and right
			{
			//TCuboPathNode *lnode=NULL;
			int lnode=-1;
			int rotated[4]= {1,2,3,0};
			int rotateddir=rotated[towardsrot];
			if (nodes[si].GetNext(rotateddir)!=-1)
					{
					mnode=nodes[si].GetNext(rotateddir);
					}
			if (nodes[si].GetNext(mirror[rotateddir])!=-1)
					{
					lnode=nodes[si].GetNext(mirror[rotateddir]);
					if (mnode)
							{
							//Choose left or right randomly
							if (rand()%2) mnode=lnode;
							}
					else mnode=lnode;
					}
			}

	if (!mnode) return ""; //No way to escape

//Check if we get further away by movin into this dir


	if (path[mnode+N*ei]<=path[si+N*ei]) return ""; //Will not optimize the position


	for (int drot=0; drot<4; drot++)
			{
			if (nodes[si].GetNext(drot)==mnode)
					{
					//Right node, obtain the move
					int deltarot=drot-startrot; if (deltarot<0) deltarot=4+deltarot;
					//   cout << "deltarot="<<drot<<" - " << startrot <<" is" << deltarot << "    ";
					switch (deltarot)
							{
							case 0: return "f";
							case 1: return "r";
							case 2: return "l";
							case 3: return "l";
							}
					}
			}


	return "";
	}



std::string CuboPathGraph::GetRandomMove(int startbs,int startrot) //Gives a rotation move for the Random Mover to rotate into a possible direction
	{
	std::string rmoves[3]= {"f","r","l"};
	int si=GetNodeIDFromSideID(startbs);
	if (si==-1) return rmoves[rand() % 3];
	int numways=0;
	for (unsigned int i=0; i<4; i++) if (nodes[si].GetNext(i)!=-1) numways++;

	if (!numways) return rmoves[rand() % 3];
	numways=rand()%numways;
	int myway=0;
	for (unsigned int i=0; i<4; i++) if (nodes[si].GetNext(i)!=-1) {if (numways==myway) { myway=i; break;} else myway++;}

	int deltarot=myway-startrot; if (deltarot<0) deltarot=4+deltarot;
	switch (deltarot)
			{
			case 0: return "";
			case 1: return "r";
			case 2: return "rr";
			case 3: return "l";
			}
	return "";

	}

int CuboPathGraph::GetNodeSideID(int n)
	{
	if (n<0 || n>=GetNumNodes()) return -1;
	else return nodes[n].GetSideID();
	}

/////////////////////


void CuboPathGraphServer::Clear()
	{
	pgs.clear();
	}

int CuboPathGraphServer::New(int startside,lua_State *state,std::string addcb)
	{
	int res=pgs.size();
	pgs.push_back(CuboPathGraph());
	pgs[res].GraphFromSide(startside,state,addcb);
	return res;
	}

CuboPathGraph * CuboPathGraphServer::GetGraph(int i)
	{
	if (i<0 || i>=(int)(pgs.size())) return NULL;
	return &(pgs[i]);
	}


////////////////////////////7





int LuaPathfindingLib::PATH_GetNextMove(lua_State *state)
	{
	int endside=LUA_GET_INT;
	int rot=LUA_GET_INT;
	int startside=LUA_GET_INT;
	int graph=LUA_GET_INT;
	std::string res=g_PathGraphs()->GetGraph(graph)->GetNextMove(startside,rot,endside);
	LUA_SET_STRING(res);
	return 1;
	}

int LuaPathfindingLib::PATH_GetEscapeMove(lua_State *state)
	{
	int endside=LUA_GET_INT;
	int rot=LUA_GET_INT;
	int startside=LUA_GET_INT;
	int graph=LUA_GET_INT;
	std::string res=g_PathGraphs()->GetGraph(graph)->GetEscapeMove(startside,rot,endside);
	LUA_SET_STRING(res);
	return 1;
	}

int LuaPathfindingLib::PATH_GetRandomMove(lua_State *state)
	{
	int rot=LUA_GET_INT;
	int startside=LUA_GET_INT;
	int graph=LUA_GET_INT;
	std::string res=g_PathGraphs()->GetGraph(graph)->GetRandomMove(startside,rot);
	LUA_SET_STRING(res);
	return 1;
	}

int LuaPathfindingLib::PATH_NewGraph(lua_State *state)
	{
	std::string cbfunc=LUA_GET_STRING;
	int startside=LUA_GET_INT;
	int res=g_PathGraphs()->New(startside,state,cbfunc);
	LUA_SET_INT(res);
	return 1;
	}

int LuaPathfindingLib::PATH_GetNumNodes(lua_State *state)
	{
	int gr=LUA_GET_INT;
	int res=g_PathGraphs()->GetGraph(gr)->GetNumNodes();
	LUA_SET_INT(res);
	return 1;
	}

int LuaPathfindingLib::PATH_GetNode(lua_State *state)
	{
	int n=LUA_GET_INT;
	int gr=LUA_GET_INT;
	int res=g_PathGraphs()->GetGraph(gr)->GetNodeSideID(n);
	LUA_SET_INT(res);
	return 1;
	}

int LuaPathfindingLib::PATH_GetDistance(lua_State *state)
	{
	int endside=LUA_GET_INT;
	int startside=LUA_GET_INT;
	int graph=LUA_GET_INT;
	int res=g_PathGraphs()->GetGraph(graph)->GetDistance(startside,endside);
	LUA_SET_INT(res);
	return 1;
	}


LuaPathfindingLib::LuaPathfindingLib()
	{
	AddFunc("PATH_NewGraph",PATH_NewGraph);
	AddFunc("PATH_GetNextMove",PATH_GetNextMove);
	AddFunc("PATH_GetEscapeMove",PATH_GetEscapeMove);
	AddFunc("PATH_GetRandomMove",PATH_GetRandomMove);
	AddFunc("PATH_GetNumNodes",PATH_GetNumNodes);
	AddFunc("PATH_GetNode",PATH_GetNode);
	AddFunc("PATH_GetDistance",PATH_GetDistance);
	}


static LuaPathfindingLib g_pflib;
LuaPathfindingLib* g_PathFindingLib() {return &g_pflib;}


// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
