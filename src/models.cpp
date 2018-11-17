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

#include "models.hpp"
#include "globals.hpp"
#include "glutils.hpp"

#define GL_GLEXT_PROTOTYPES 1

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

//#include <GL/glext.h>

#endif


#include <iostream>
#include <vector>
#include "cuboutils.hpp"
#include "game.hpp"


#define RENDERLISTS

void Tokenize0(const std::string& str,
		std::vector<std::string>& tokens,
		const std::string& delimiters = " ") {
	// Skip delimiters at beginning.
	std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
	// Find first "non-delimiter".
	std::string::size_type pos     =lastPos+1;// str.find_first_of(delimiters, lastPos);

	while (std::string::npos != pos || std::string::npos != lastPos) {
			// Found a token, add it to the vector.
			tokens.push_back(str.substr(lastPos, pos - lastPos));
			// Skip delimiters.  Note the "not_of"
			lastPos = str.find_first_not_of(delimiters, pos);
			// Find next "non-delimiter"
			pos = str.find_first_of(delimiters, lastPos);
			}
	}

std::string Untokenize(const std::vector<std::string>& tokens, int pos, const std::string &delimiter) {
	std::string res="";
	while ((unsigned int)pos<tokens.size()) {
			res=res+tokens[pos];
			pos++;
			if ((unsigned int)pos<tokens.size()) { res=res+delimiter; }
			}
	return res;
	}


void TextFileReader::RemoveComments(std::string commentindicator,bool trim,bool cutempty) {
	unsigned int i=0;
	while (i<lines.size()) {
			std::string line=lines[i];
			TrimSpaces(line);
			if (cutempty && (line=="")) { lines.erase(lines.begin()+i); linenums.erase(linenums.begin()+i); continue;}
			if ((commentindicator!="") && (BeginsWith(line,commentindicator))) { lines.erase(lines.begin()+i); linenums.erase(linenums.begin()+i); continue;}
			if (trim) { lines[i]=line; }
			i++;
			}

	}

bool TextFileReader::LoadFile(CuboFile *finfo) {
	lines.clear();
	linenums.clear();
	FILE *f;


	if (finfo->IsHDDFile()) {
			f=fopen(finfo->GetHDDName().c_str(),"rt"); if (!f) return false;
			linenr=1;
			char buff[2048];
			while(fgets(buff, 2000, f)!=NULL) {
					std::string line=buff;
					lines.push_back(line);
					linenums.push_back(linenr);
					linenr++;
					}
			fclose(f);
			}
	else {
			//coutlog("Textfile (Mdl) Load from ZIP not yet implemented");
			char *c=(char*)finfo->GetData();
			c[finfo->GetSize()]='\0';
			lines.clear();
			Tokenize(c,lines,"\n"); //TODO: Careful with windows format!
			linenums.resize(lines.size());
			for (unsigned int i=0; i<lines.size(); i++) { linenums[i]=i; }
			}




	linenr=-1;

	return true;
	}


void TextFileReader::OutToTTY() {
	for(unsigned i=0; i<lines.size(); i++) { std::cout << lines[i] << std::endl; }
	}


int TextFileReader::GetLineNr() {
	if (linenr<0) { return 0; }
	if (linenr>=(int)linenums.size()) { return 0; }
	return linenums[linenr];
	}

std::string TextFileReader::NextLine() {
	linenr++;
	if (linenr>=(int)lines.size()) { return ""; }
	return lines[linenr];
	}

bool TextFileReader::isEoF() {
	return (linenr>=(int)lines.size());
	}


bool TextFileReader::ExtractIntVect(std::string s,IntVect &intv,std::string *ress,std::string sep) {
	std::vector<std::string> tokens;
	Tokenize(s,tokens,sep);
	if (tokens.size()<3) { return false; }
	intv.x=atoi(tokens[0].c_str()); ///TODO: Errorchecking
	intv.y=atoi(tokens[1].c_str());
	intv.z=atoi(tokens[2].c_str());
	if (ress) { (*ress)=Untokenize(tokens,3," "); }
	return true;
	}

bool TextFileReader::ExtractIntVectFace(std::string s,IntVect &intv,std::string *ress,std::string sep) {
	std::vector<std::string> tokens;
	Tokenize(s,tokens,sep);
	if (tokens.size()<3) {
			intv.x=atoi(tokens[0].c_str()); ///TODO: Errorchecking
			intv.y=1;
			intv.z=atoi(tokens[1].c_str());
			}
	else {
			intv.x=atoi(tokens[0].c_str()); ///TODO: Errorchecking
			intv.y=atoi(tokens[1].c_str());
			intv.z=atoi(tokens[2].c_str());
			}
	if (ress) { (*ress)=Untokenize(tokens,3," "); }
	return true;
	}


bool TextFileReader::ExtractIntVect0(std::string s,IntVect &intv,std::string *ress,std::string sep) {
	std::vector<std::string> tokens;
	Tokenize0(s,tokens,sep);

	if (tokens.size()<3) { return false; }
	intv.x=atoi(tokens[0].c_str()); ///TODO: Errorchecking
	intv.y=atoi(tokens[1].c_str());
	intv.z=atoi(tokens[2].c_str());
	if (ress) { (*ress)=Untokenize(tokens,3," "); }
	return true;
	}

bool TextFileReader::Extract3dVector(std::string s,Vector3d &intv,int offs) {
	std::vector<std::string> tokens;
	Tokenize(s,tokens," ");
	if (tokens.size()-offs<3) { return false; }
	intv.x=atof(tokens[offs+0].c_str()); ///TODO: Errorchecking
	intv.y=atof(tokens[offs+1].c_str());
	intv.z=atof(tokens[offs+2].c_str());
// if (ress) (*ress)=Untokenize(tokens,3," ");
	return true;
	}

bool TextFileReader::Extract2dVector(std::string s,Vector2d &intv,int offs) {
	std::vector<std::string> tokens;
	Tokenize(s,tokens," ");
	if (tokens.size()-offs<2) { return false; }
	intv.u=atof(tokens[offs+0].c_str()); ///TODO: Errorchecking
	intv.v=atof(tokens[offs+1].c_str());
// if (ress) (*ress)=Untokenize(tokens,3," ");
	return true;
	}


void TextFileReader::ReplaceChar(std::string &str,char which,char with) {
	for (unsigned int i=0; i<str.length(); i++) if (str[i]==which) { str[i]=with; }
	}


std::vector<std::string> TextFileReader::Seperate(std::string s,std::string sep) {
	std::vector<std::string> tokens;
	Tokenize(s,tokens,sep);
	return tokens;
	}

////////////////////////////////////////////////7

void BaseModel::Reload() {
	std::string fna=name;
	Clear();
	CuboFile * finfo =g_BaseFileSystem()->GetFileForReading(name);
	if (!finfo) {coutlog("Cannot reload model "+name,2); }
	LoadFromFile(finfo);
	delete finfo;
	}

////////////////////////////////////////////////7


bool OBJModel::LoadFromFile(CuboFile *finfo) {
	BaseModel::LoadFromFile(finfo);
	Clear();
	TextFileReader tr;

	if (!tr.LoadFile(finfo)) { return false; }
	tr.RemoveComments();

	int groupindex=-1;
	while (!tr.isEoF()) {
			std::string line=tr.NextLine();
//   int linenr=tr.GetLineNr(); //For Error handling

			if (BeginsWith(line,"v ")) {
					//Vertex
					Vector3d v;
					tr.ReplaceChar(line,',','.');
					if (!tr.Extract3dVector(line,v,1)) { return false; }
					verts.push_back(v);
					}
			else if (BeginsWith(line,"vt ")) {
					Vector2d v;
					tr.ReplaceChar(line,',','.');
					if (!tr.Extract2dVector(line,v,1)) { return false; }
					texcoords.push_back(v);
					}
			else if (BeginsWith(line,"vn ")) {
					//Norm
					Vector3d v;
					tr.ReplaceChar(line,',','.');
					if (!tr.Extract3dVector(line,v,1)) { return false; }
					normals.push_back(v);
					}
			else if (BeginsWith(line,"g ")) {
					groupindex=groups.size();
					groups.resize(groupindex+1);
					groups[groupindex].name.assign(line,2,line.size());
					//groups[0].materialname="";
					groups[groupindex].faces.clear();
					groups[groupindex].userenderlist=0;
					//groupindex=0;
					}
			else if (BeginsWith(line,"f ")) {
					if (groupindex==-1) {
							groups.resize(1);
							groups[0].name="_NONE_";
							groups[0].materialname="";
							groups[0].faces.clear();
							groupindex=0;
							}
					//Parse a face
					std::vector<std::string> outertokens;
					outertokens=tr.Seperate(line," ");
					if (outertokens.size()<4) { return false; }
					int faceindex=groups[groupindex].faces.size();
					groups[groupindex].faces.resize(faceindex+1);
					for (int i=1; i<=3; i++) {
							IntVect iv;
							if (!tr.ExtractIntVectFace(outertokens[i],iv,NULL,"/")) { return false; }
							groups[groupindex].faces[faceindex].vert[i-1]=iv.x-1;
							groups[groupindex].faces[faceindex].texcoord[i-1]=iv.y-1;
							groups[groupindex].faces[faceindex].norm[i-1]=iv.z-1;

							}


					}



			}

	Vector2d bla;
	if (texcoords.size()==0) { texcoords.push_back(bla); }
	return true;
	}


void OBJModel::Clear() {
	verts.clear();
	normals.clear();
	texcoords.clear();
	tangents.clear();
	for (unsigned int i=0; i<groups.size(); i++) {
			groups[i].faces.clear();
			if (groups[i].userenderlist) { glDeleteLists(groups[i].renderlist, 1); }
			}
	groups.clear();
	}

int g_norenderlist=0;

void OBJModel::DrawGroup(int g) {
	//glShadeModel(GL_SMOOTH);

//Generation of a render list
#ifdef RENDERLISTS
	if (!g_norenderlist) {
			if (!groups[g].userenderlist) {
					groups[g].renderlist= glGenLists(1);
					glNewList(groups[g].renderlist, GL_COMPILE_AND_EXECUTE);
					}
			else {
					glCallList(groups[g].renderlist);
					return;
					}
			}
#endif

	glBegin(GL_TRIANGLES);
	for (unsigned int f=0; f<groups[g].faces.size(); f++)
		for (unsigned int v=0; v<3; v++) {
				Vector2d tc=texcoords[groups[g].faces[f].texcoord[v]];
				Vector3d n=normals[groups[g].faces[f].norm[v]];
				Vector3d vect=verts[groups[g].faces[f].vert[v]];
				glTexCoord2f(tc.u,tc.v);
				glNormal3f(n.x,n.y,n.z);
				glVertex3f(vect.x,vect.y,vect.z);
				}
	glEnd();

#ifdef RENDERLISTS
	if (!groups[g].userenderlist && !(g_norenderlist)) {
			glEndList();
			groups[g].userenderlist=1;
			}
#endif

	}

/*void TOBJModel::RenderAll()
{
 for (unsigned int g=0;g<groups.size();g++) DrawGroup(g);
}
*/



void OBJModel::RenderByDef(int defindex) {

	for (unsigned int g=0; g<groups.size(); g++) { g_Game()->GetModelDefs()->GetDefPtr(defindex)->Call_RenderGroup(g); }
	}


void OBJModel::SmoothSimplify() {
//Check if doublicate Verts are defined and simplify them
	std::vector<int> mapper;
	for (unsigned int i=0; i<verts.size(); i++) {
			mapper.push_back(i); //Load identity in the mapper
			}

	for (unsigned int i=0; i<verts.size(); i++) {
			Vector3d v1=verts[i];
			for (unsigned int j=i; j<verts.size(); j++) {
					Vector3d v2=verts[j];
					v2=v2-v1;
					if (v2.isZero()) {
							mapper[j]=i;
							//And remove this Vertex from the list, shifting all others back by one
							///TODO: Program further...
							}
					}
			}
	}



Vector3d OBJModel::GetTangent(int g,int f) {
	Vector3d p1=verts[groups[g].faces[f].vert[0]];
	Vector3d p2=verts[groups[g].faces[f].vert[1]];
	Vector3d p3=verts[groups[g].faces[f].vert[2]];

	Vector2d uv1=texcoords[groups[g].faces[f].texcoord[0]];
	Vector2d uv2=texcoords[groups[g].faces[f].texcoord[1]];
	Vector2d uv3=texcoords[groups[g].faces[f].texcoord[2]];

	Vector3d diff1=p2-p1;
	Vector3d diff2=p3-p1;
	diff1=(diff1 * (uv3.v-uv1.v));
	diff2=(diff2 * (uv2.v-uv1.v));
	Vector3d tangent=  diff1-diff2;
	tangent.normalize();
	return tangent;
	}

void OBJModel::DrawGroupWithTangent(int g, std::string TangentNameInShader) {
	if (tangents.size()==0) { //BUILD THE TANGENTS FOR THIS MODEL
			for (unsigned int gr=0; gr<groups.size(); gr++) {
					for (unsigned int f=0; f<groups[gr].faces.size(); f++) {
							int index=tangents.size();
							tangents.push_back(GetTangent(gr,f));
							groups[gr].faces[f].tangent=index;
							}
					}
			}


	GLint loc=g_Game()->GetShaders()->GetAttributeLocation(TangentNameInShader);
	glBegin(GL_TRIANGLES);
	for (unsigned int f=0; f<groups[g].faces.size(); f++) {

			glVertexAttrib3f(loc,tangents[groups[g].faces[f].tangent].x,tangents[groups[g].faces[f].tangent].y,tangents[groups[g].faces[f].tangent].z);
			// glVertexAttrib3fNV(loc,tangents[groups[g].faces[f].tangent].x,tangents[groups[g].faces[f].tangent].y,tangents[groups[g].faces[f].tangent].z);
			for (unsigned int v=0; v<3; v++) {
					Vector2d tc=texcoords[groups[g].faces[f].texcoord[v]];
					Vector3d n=normals[groups[g].faces[f].norm[v]];
					Vector3d vect=verts[groups[g].faces[f].vert[v]];
					glTexCoord2f(tc.u,tc.v);
					glNormal3f(n.x,n.y,n.z);
					glVertex3f(vect.x,vect.y,vect.z);
					}
			}
	glEnd();
	}





void ModelServer::Clear() {
	for (unsigned int i=0; i<mdls.size(); i++) if (mdls[i]) { delete mdls[i]; mdls[i]=NULL;}
	mdls.resize(0);
	}

ModelServer::ModelServer() {

	}

ModelServer::~ModelServer() {
	Clear();
	}

void ModelServer::Reload() {
	for (unsigned int i=0; i<mdls.size(); i++) {
			mdls[i]->Reload();
			}
	}

int ModelServer::GetModel(std::string fname) {
	for (unsigned int i=0; i<mdls.size(); i++) {
			if (mdls[i]->GetName()==fname) { return i; }
			}
	return -1;
	}

int ModelServer::AddOBJ(CuboFile * finfo) {

//TCuboFile * finfo=GetFileName(fname,FILE_MDL,".obj")
	std::string fname=finfo->GetName();

	int res=GetModel(fname);
	if (res!=-1) { return res; }

	if (g_VerboseMode()) { coutlog("Loading Model: "+finfo->GetNameForLog()); }
	OBJModel *newmdl=new OBJModel();


	if (!newmdl->LoadFromFile(finfo)) { delete newmdl; return -1;}

	res=mdls.size();
	mdls.push_back(newmdl);
	return res;
	}


///////////////////////////////////////////////


void MdlDef::CreateRenderList(int gindex) {
	if (gindex<0) {
			//cout << "Renderlist creation " << this->GetName() << endl;
			totalrenderlist=glGenLists(1);
			glNewList(totalrenderlist, GL_COMPILE);
			if (g_VerboseMode()) {coutlog(" -- Creating a total renderlist for a model");}
			g_norenderlist=1;
			Call_RenderModel();
			g_norenderlist=0;
			glEndList();
			}
	else {
			if (gindex>=(int)grenderlists.size()) {grenderlists.resize(gindex+1,0);}
			grenderlists[gindex]=glGenLists(1);
			glNewList(grenderlists[gindex], GL_COMPILE);
			if (g_VerboseMode()) {coutlog(" -- Creating a group renderlist for a model");}
			g_norenderlist=1;
			Call_RenderGroup(gindex);
			g_norenderlist=0;
			glEndList();
			}
	}

MdlDef::~MdlDef() {
	if (totalrenderlist) {glDeleteLists(totalrenderlist,1);}
	for (unsigned int i=0; i<grenderlists.size(); i++) if (grenderlists[i]) { glDeleteLists(grenderlists[i],1); }
	}

void MdlDef::Call_RenderModel() {

	if (totalrenderlist && (!g_norenderlist)) {glCallList(totalrenderlist); InvalidateMaterial(); }
	else if (lua.FuncExists("Render")) {

			lua.CallVA("Render","i",myid);

			}

	}

void MdlDef::Call_RenderGroup(int g) {
	if (g>=(int)grenderlists.size()) { grenderlists.resize(g+1,0); }
	if (grenderlists[g] && (!g_norenderlist)) {glCallList(grenderlists[g]);  }
	else if (lua.FuncExists("RenderGroup")) {

			lua.CallVA("RenderGroup","i",g);

			}

	}


///////////////////////////////////////////////


int MDLDEF_Load(lua_State *state) {
	std::string s=LUA_GET_STRING(state);
	int r=g_Game()->GetModelDefs()->AddDef(s);
	LUA_SET_NUMBER(state, r);
	return 1;
	}

int MDLDEF_Render(lua_State *state) {
	int mdldef=LUA_GET_INT(state);
	g_Game()->GetModelDefs()->GetDefPtr(mdldef)->Call_RenderModel();
	return 0;
	}

int MDLDEF_CreateRenderList(lua_State *state) {
	int g=LUA_GET_INT(state);
	lua_getglobal(state,"LUA_DEF_ID");
	int mdldef=LUA_GET_INT(state);
	InvalidateMaterial();
	g_Game()->GetModelDefs()->GetDefPtr(mdldef)->CreateRenderList(g);
	InvalidateMaterial();
	return 0;
	}


void LUA_MDLDEF_RegisterLib() {
	g_CuboLib()->AddFunc("MDLDEF_Load",MDLDEF_Load);
	g_CuboLib()->AddFunc("MDLDEF_Render",MDLDEF_Render);
	g_CuboLib()->AddFunc("MDLDEF_CreateRenderList",MDLDEF_CreateRenderList);

	}

////////////LUA-IMPLEMENT/////////////////////7


int MODEL_Render(lua_State *state) {
	int mdlindex=LUA_GET_INT(state);
	int defindex=LUA_GET_INT(state);
	g_Game()->GetModels()->GetModelPtr(mdlindex)->RenderByDef(defindex);
	return 0;
	}


int MODEL_LoadOBJ(lua_State *state) {
	std::string fname=LUA_GET_STRING(state);
	CuboFile *finfo=GetFileName(fname,FILE_MDL,".obj");
	if (!finfo) {coutlog("OBJ model "+fname+ " not found!",1); LUA_SET_NUMBER(state, -1) ; return 1;}
	int r=g_Game()->GetModels()->AddOBJ(finfo);
	delete finfo;
	LUA_SET_NUMBER(state, r);
	return 1;
	}

int MODEL_DrawGroup(lua_State *state) {
	int group=LUA_GET_INT(state);
	int model=LUA_GET_INT(state);
	g_Game()->GetModels()->GetModelPtr(model)->DrawGroup(group);
	return 0;
	}

int MODEL_DrawGroupWithTangent(lua_State *state) {
	std::string TangentNameInShader=LUA_GET_STRING(state);
	int group=LUA_GET_INT(state);
	int model=LUA_GET_INT(state);
	g_Game()->GetModels()->GetModelPtr(model)->DrawGroupWithTangent(group,TangentNameInShader);
	return 0;
	}


void LUA_MODEL_RegisterLib() {
	g_CuboLib()->AddFunc("MODEL_Render",MODEL_Render);
	g_CuboLib()->AddFunc("MODEL_LoadOBJ",MODEL_LoadOBJ);
	g_CuboLib()->AddFunc("MODEL_DrawGroup",MODEL_DrawGroup);
	g_CuboLib()->AddFunc("MODEL_DrawGroupWithTangent",MODEL_DrawGroupWithTangent);
	}
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
