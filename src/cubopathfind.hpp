/**
Copyright (C) 2011 Chriddo

This program is free software;
you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program;
if not, see <http://www.gnu.org/licenses/>.
**/



#ifndef CUBOPATHFIND_H_G
#define CUBOPATHFIND_H_G

#include <vector>
#include <string>


#include "luautils.hpp"

using namespace std;

//A node indexing a block-side and its neighbours, which can be reached by the movement into the four directions
class TCuboPathNode
	{
	protected:
		int sideindex; //Index to the side of this node
//  TCuboPathNode * next[4]; //When t is the tangential vector and n the normal vector, then the neighbours are defined as follows
		// 0 ~ t , 1 ~ txn , 2 ~ -t , 3 ~ -txn
		int next[4];
	public:
		TCuboPathNode();
		TCuboPathNode(int si);
		int GetNextSideID(int dir);
		int GetSideID() const {return sideindex;}
		//void SetNext(int m,TCuboPathNode *n) {next[m]=n;}
		//TCuboPathNode* GetNext(int m) {return next[m];}

		void SetNext(int m, int n) {next[m]=n;}
		int GetNext(int m) {return next[m];}
	};

//A graph of nodes (sides) and
class TCuboPathGraph
	{
	protected:
		lua_State *cstate;
		string callbfunc;
		TCuboPathNode * AddNode(int sindex);
		vector<TCuboPathNode> nodes;
		vector<int> path,next;
		int GetNodeIDFromSideID(int ID);
		int EdgeBetween(int i,int j);
		string GetPath(int i,int j);
		int MayAddNode(int from,int to);
	public:
		void GraphFromSide(int startindex,lua_State *state,string addcb);
		string GetPathFromTo(int startbs,int endbs);
		string GetNextMove(int startbs,int startrot,int endbs);
		string GetRandomMove(int startbs,int startrot);
		string GetEscapeMove(int startbs,int startrot,int endbs);
		int GetNumNodes() {return nodes.size();}
		int GetNodeSideID(int n);
		int GetDistance(int startbs,int endbs);
	};


class TCuboPathGraphServer
	{
	protected:
		vector<TCuboPathGraph> pgs;
	public:
		void Clear();
		int New(int startside,lua_State *state,string addcb);
		TCuboPathGraph * GetGraph(int i);
	};

extern TCuboPathGraphServer * g_PathGraphs();





class TLuaPathfindingLib : public TLuaCFunctions
	{
	protected:
		static int PATH_NewGraph(lua_State *state);
		static int PATH_GetNextMove(lua_State *state);
		static int PATH_GetRandomMove(lua_State *state);
		static int PATH_GetEscapeMove(lua_State *state);
		static int PATH_GetNumNodes(lua_State *state);
		static int PATH_GetNode(lua_State *state);
		static int PATH_GetDistance(lua_State *state);
	public:
		TLuaPathfindingLib();
	};

extern TLuaPathfindingLib* g_PathFindingLib();


#endif
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
