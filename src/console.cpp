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

#include "console.hpp"

#ifdef WIN32

#include <windows.h>
#ifdef MINGW_CROSS_COMPILE
#include <GL/glew.h>
#else
#include <GL\glew.h>
#endif
// #include <GL\gl.h>
#include <SDL.h>

#else

#include <GL/glew.h>
#include <SDL/SDL.h>

#endif

#include <iostream>
#include "game.hpp"

CuboConsole* CuboConsole::getInstance() { // TODO: return reference instead
	static CuboConsole console;
	return &console;
}

int CuboConsole::Toggle()
	{
	isactive=!isactive;
	if (isactive) {
			g_Game()->GetKeyboard()->SetKeyRepeatTimes(0.5,0.025);
			g_Game()->GetKeyboard()->StartTextInput();
			was_mouse_snapped=g_Game()->GetMouse()->IsSnapped(); g_Game()->GetMouse()->Snap(0);
			}
	else {
			g_Game()->GetKeyboard()->SetKeyRepeatTimes(0,0);
			g_Game()->GetKeyboard()->StopTextInput();
			g_Game()->GetMouse()->Snap(was_mouse_snapped);
			}

	return isactive;
	}

int CuboConsole::IsActive()
	{
	return isactive;
	}

CuboConsole::CuboConsole() : isactive(false), currentline(-1), scrolloffs(0),  screenlines(23), lowerline_ypos(0.5),  togglekey(-2)
	{
	history.push_back("");
	hisbackup=""; hisindex=0;
	lua.Include(g_CuboLib());
	}

void CuboConsole::Init()
	{
	TCuboFile * cscript=g_BaseFileSystem()->GetFileForReading("/user/console.cfg");
	lua.Include(g_CuboLib());
	if (cscript)
			{
			char *c=(char*)cscript->GetData(0);
			c[cscript->GetSize()]='\0';
			std::vector<std::string> lines;
			Tokenize(c,lines,"\n"); //TODO: Careful with windows format!
			for (unsigned int i=0; i<lines.size(); i++) if (lines[i]!="") ParseCmdLine(lines[i]);
			delete cscript;
			}
	}


void CuboConsole::AddLine(std::string s,int typ)
	{
	std::vector<std::string> toks;
	Tokenize(s,toks,"\n");

	for (unsigned int i=0; i<toks.size(); i++)
			{
			if (lines.size()<CUBO_CONSOLE_MAX_LINES) { lines.push_back(toks[i]); types.push_back(typ); if (scrolloffs) scrolloffs++;}
			else {
					currentline++;
					if (currentline>=(int)lines.size()) currentline=0;
					lines[currentline]=toks[i];
					types[currentline]=typ;
					}
			}
	}

void CuboConsole::SetToggleKey(int tk) {togglekey=tk;}
int CuboConsole::GetToggleKey() {return togglekey;}



CuboConsole::~CuboConsole()
	{

	}

int CuboConsole::CheckBindKey(int ident,int down,int toggle)
	{
	for (unsigned int i=0; i<binds.size(); i++)
			{
			if (ident==binds[i].k)
					{
					if (down && toggle) ParseCmdLine(binds[i].cmd);
					return 1;
					}
			}
	return 0;
	}

int CuboConsole::Bind(std::vector<std::string> & extratoks,int unbind)
	{
	if (!extratoks.size()) {coutlog("Key expected!",2); return 0;}
	std::string key=extratoks[0];
	SDLKey k=g_Game()->GetKeyboard()->GetKeyConstFor(key);
	if (!k) {coutlog("Unknown key: "+key,2); return 0;}

	int bindex=-1;
	for (unsigned int i=0; i<binds.size(); i++) if (binds[i].k==k) {bindex=i; break;}

	if (unbind || extratoks.size()==1) {
			if (bindex==-1) {if (unbind) coutlog("Key "+key+" was not bound to any command",2); return 0;}
			binds.erase(binds.begin()+bindex);
			}
	else {
			if (bindex==-1) bindex=binds.size(); binds.push_back(ConsoleBinding());
			std::string cmd="";

			for (unsigned int i=1; i<extratoks.size(); i++) cmd+=extratoks[i]+" ";

			// ostringstream oss; oss << "Hallo: " << cmd << " key: " << k << "  index " << bindex << endl;
			// coutlog(oss.str());
			binds[bindex].k=k; binds[bindex].cmd=cmd;
			}

	return 1;
	}

void CuboConsole::ParseCmdLine(std::string cmdl)
	{
	std::string cmdline=history.back();
	if (cmdl!="") cmdline=cmdl;
	else hisindex=0;

	TrimSpaces(cmdline);

	if (cmdline=="") {coutlog(" ",0); history.back()="";  return; }


	std::vector<std::string> parts;

	/*int instring=0;int inlua=0;

	string rest=cmdline;
	string buff="";
	while (rest!="")
	{

	}*/
	parts.push_back(cmdline); ///TODO: Sep into Lua, " "-Parts and normal console statements


	for (unsigned int p=0; p<parts.size(); p++)
			{


			std::string cmdlinepart=parts[p];
			if (cmdl=="") coutlog(cmdlinepart,3);


			std::vector<std::string> toks;
			Tokenize(cmdlinepart,toks," ");

			std::vector<std::string> extratoks; for (unsigned int i=1; i<toks.size(); i++) extratoks.push_back(toks[i]);

			int validcmd=1;

			if (cmdlinepart=="quit" || cmdlinepart=="exit")  g_Game()->Quit();

			else if (toks[0]=="lua") {
					std::string s=""; for (unsigned int i=1; i<toks.size(); i++) s+=(toks[i]+" "); lua.ExecString(s);
					}
			else if (toks[0]=="reload") {  g_Game()->Reload(extratoks);  }

			else if (toks[0]=="userlevel") {
					if (toks.size()>1) {
							if (g_Game()->StartLevel(toks[1],1)) was_mouse_snapped=0;
							}
					}
			else if (toks[0]=="level") {
					if (toks.size()>1) {
							if (g_Game()->StartLevel(toks[1],0)) was_mouse_snapped=0;
							}
					}
			else if (toks[0]=="edit") {
					if (toks.size()>1)
						g_Game()->StartLevel(toks[1],2);
					else if (g_Game()->StartLevel("",2)) was_mouse_snapped=1;
					}
			else if (toks[0]=="bind") {
					if (!Bind(extratoks,0)) validcmd=2;
					}
			else if (toks[0]=="unbind") {
					if (!Bind(extratoks,1)) validcmd=2;
					}
			else if (toks[0]=="lsvars") {
					if (!extratoks.size()) g_Vars()->ListToConsole();
					else {
							std::string what=extratoks[0];
							if (extratoks.size()<2) coutlog("need an index: "+cmdline,2);
							else {
									std::istringstream iss(extratoks[1]); int index; iss >> index;
									if (index<0) coutlog("index negative: "+extratoks[1],2);
									else {
											if (what=="actor") {
													if (index>=g_Game()->GetNumActors()) coutlog("index out of bounds: "+extratoks[1],2);
													else {
															g_Game()->GetActorMovement(index)->GetVarHolder()->ListToConsole();
															}
													}
											else coutlog("unknown type: "+what,2);

											}
									}
							}

					}
			else validcmd=0;


			if (validcmd==0) coutlog("unknown command: "+cmdline,2);


			}

	if (cmdl=="")
			{
			if (history.size()>=2 && (history.back()==history[history.size()-2])) history.back()="";
			else history.push_back("");
			}
	}

void CuboConsole::KeyHandle(int ident,int down,int toggle)
	{
	if ((!toggle) || (!down)) return;

	SDL_keysym sim=g_Game()->GetKeyboard()->GetLastKeySim();

	int ident2=sim.sym;

//cout << ident << " " << ident2 << " " << sim.unicode << endl;

	if (ident2==SDLK_ESCAPE ) { Toggle(); return; }
	if (ident2==SDLK_PAGEUP && down && toggle)
			{ scrolloffs+=(screenlines*3)/4; if (scrolloffs>(int)lines.size()-screenlines/2) scrolloffs=lines.size()-screenlines/2; if (scrolloffs<0) scrolloffs=0; return; }
	else if (ident2==SDLK_PAGEDOWN && down && toggle) { scrolloffs-=(screenlines*3)/4; if (scrolloffs<0) scrolloffs=0; return; }
	else if (ident2==SDLK_HOME && down && toggle) { scrolloffs=lines.size()-screenlines/2; if (scrolloffs<0) scrolloffs=0; return; }
	else if (ident2==SDLK_END && down && toggle) { scrolloffs=0; return;}


	else if (ident2==SDLK_BACKSPACE) {if (history.back()!="") history.back()=history.back().substr(0,history.back().length()-1);}
	else if (ident2==SDLK_SPACE) history.back()+=" ";
	else if (ident2==SDLK_RETURN) ParseCmdLine();
	else if (ident2==SDLK_UP) {
			if (hisindex==0) hisbackup=history.back();
			hisindex++;
			if (hisindex>=(int)history.size())
				hisindex=history.size()-1;
			history.back()=history[history.size()-1-hisindex];
			}
	else if (ident2==SDLK_DOWN && hisindex>0)
			{	hisindex--;
			if (hisindex==0) history.back()=hisbackup;
			else history.back()=history[history.size()-1-hisindex];
			}
	else if (sim.unicode >0 && (sim.unicode & 0xFF80) == 0)
			{
			wchar_t c=sim.unicode & 0x7F;
			history.back()+=c;


			}
	/*
	string kn=g_Game()->GetKeyboard()->GetKeyName(ident);
	if (kn.length()==1) {
	  //Probably a letter
	  char c=kn[0];
	  if (c>='a' && c<='z') { history.back()+=c; }
	  else if (c>='0' && c<='9') { history.back()+=c; }

	}
	*/

	}

int CuboConsole::GetLineIndex(int offs)
	{
	if (lines.size()>=CUBO_CONSOLE_MAX_LINES)
			{
			if (offs>=CUBO_CONSOLE_MAX_LINES) return -1;
			return (currentline+lines.size()-offs) % lines.size();

			}
	else
			{
			int pos=lines.size()-1-offs;
			if (pos<0) return -1;
			else return pos;
			}
	}

void CuboConsole::Render()
	{
	if ((!isactive) && lowerline_ypos>=0.5) return;

	if (isactive==false)  {
			lowerline_ypos+=0.2;
			if (lowerline_ypos>0.5) { lowerline_ypos=0.5; return ; }
			}
	else {
			lowerline_ypos-=0.2; if (lowerline_ypos<-0.2) lowerline_ypos=-0.2;

			}

	if (g_Game()->GetFont()->GetFontName()=="") return;
	glMatrixMode(GL_PROJECTION);								// Select Projection
	glPushMatrix();												// Push The Matrix
	glLoadIdentity();											// Reset The Matrix
	glOrtho(-0.5,0.5,-0.5,0.5,-1,128);
	glMatrixMode(GL_MODELVIEW);									// Select Modelview Matrix
	glPushMatrix();												// Push The Matrix
	glLoadIdentity();											// Reset The Matrix

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

//BACKGROUND
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	g_Game()->GetTextures()->DeactivateStage(0);


//tfloat lowerline_ypos=-0.2;


//glBegin(GL_QUADS);
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(0,0,-1);
	glColor4f(0.05,0.05,0.05,0.96);
	glVertex3f(-0.5, lowerline_ypos, -10);
	glVertex3f(0.5, lowerline_ypos, -10);

	glVertex3f(-0.5, 0.5, -10);
	glVertex3f(0.5, 0.5, -10);
	glEnd();


//glDisable(GL_DEPTH_TEST);
	if (scrolloffs>0) //Scroll Arrows
			{
			glBegin(GL_TRIANGLES);
			glColor4f(0.8,0.8,0.8,1);
			glVertex3f(0.49, lowerline_ypos+0.05, -10);
			glVertex3f(0.47, lowerline_ypos+0.05, -10);
			glVertex3f(0.48, lowerline_ypos+0.01, -10);
			glEnd();
			}
	if (scrolloffs<(int)lines.size()-screenlines/2)
			{
			glBegin(GL_TRIANGLES);
			glColor4f(0.8,0.8,0.8,1);
			glVertex3f(0.47, 0.45, -10);
			glVertex3f(0.49, 0.45, -10);
			glVertex3f(0.48, 0.49, -10);
			glEnd();
			}


//Lines
	tfloat fontsize=0.015;
	tfloat distfactor=2;

	lowerline_ypos+=fontsize;

	g_Game()->GetFont()->Begin();

	g_Game()->GetFont()->SetSize(fontsize);


	g_Game()->GetFont()->SetAlign("left","bottom");

	tfloat linkrement=distfactor*fontsize;

	for (int c=0; c<screenlines; c++)
			{
			tfloat ypos=lowerline_ypos+(c+1.5)*linkrement;
			if (ypos>0.5) break;
			int lindex=GetLineIndex(c+scrolloffs);
			if (lindex<0) break;
			g_Game()->GetFont()->Goto(-0.495,ypos);
			switch (types[lindex])
					{
					case 1: glColor4f(0.8,0.2,0.2,1); break;
					case 2: glColor4f(0.8,0.8,0.2,1); break;
					case 3: glColor4f(0.2,0.2,0.8,1); break;
					case 4: glColor4f(0.2,0.8,0.2,1); break;
					default :glColor4f(0.8,0.8,0.8,1); break;
					}
			g_Game()->GetFont()->TextOut(lines[lindex]);

			}

	glColor4f(1,1,1,1);
	g_Game()->GetFont()->Goto(-0.495,lowerline_ypos);
	g_Game()->GetFont()->TextOut("> "+history.back());

	g_Game()->GetFont()->End();
//Endl



	glDisable(GL_BLEND);

	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);

	glMatrixMode( GL_PROJECTION );								// Select Projection
	glPopMatrix();												// Pop The Matrix
	glMatrixMode( GL_MODELVIEW );								// Select Modelview
	glPopMatrix();

	}
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
