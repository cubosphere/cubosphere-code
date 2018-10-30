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

#include "game.hpp"

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

#include "vectors.hpp"
#include "cuboboxes.hpp"
#include <iostream>
#include <fstream>
#include <memory>
#include "globals.hpp"
#include "shaderutil.hpp"
#include "models.hpp"
#include "cuboenemies.hpp"
#include "luautils.hpp"
#include "posteffects.hpp"
#include "cubopathfind.hpp"
#include "spriteutils.hpp"
#include "sounds.hpp"
#include "glutils.hpp"
#include <jpeglib.h>


#ifdef PARALLELIZE
#include <omp.h>
#endif

static TCuboGame *TheGame=NULL;

TCuboGame *g_Game() {return TheGame;}


static void keyhandle(int key,int down,int special)
	{
	if (!TheGame) return;
	else TheGame->KeyHandle(key,down,special);
	}
/*
static void joyaxishandle(int joys,int axis,float val,float pval)
{
 if (!TheGame) return;
  else TheGame->JoyAxisHandle(joys,axis,val,pval);

}
*/
static void joyhandle(int joys,int button,int dir,int down,int toggle)
	{
	if (!TheGame) return;
	else TheGame->DiscreteJoyHandle(joys,button,dir,down,toggle);

	}

//const GLfloat light_ambient[]  = { 0.0f, 0.0f, 0.0f, 1.0f };
//const GLfloat light_ambient[]  = { 0.1f, 0.1f, 0.1f, 1.0f };
const GLfloat light_ambient[]  = { 0.8f, 0.8f, 0.8f, 1.0f };
//const GLfloat light_ambient[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_emission[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
//const GLfloat light_diffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat light_diffuse[]  = { 0.5,0.5,0.5,1.0 };
//const GLfloat light_specular[] = { 0,0,0,0};
const GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//const GLfloat light_specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
//const GLfloat light_specular[] = { 0.6f, 0.6f, 0.6f, 1.0f };
//const GLfloat light_position[] = { 0.0f, 0.0f, 1.0f, 0.0f };
const GLfloat light_position[] = { 0.0f, 0.0f, 1.0f, 0.0f };

const GLfloat mat_ambient[]    = { 0.7f, 0.7f, 0.7f, 1.0f };
const GLfloat mat_diffuse[]    = { 0.8f, 0.8f, 0.8f, 1.0f };
const GLfloat mat_specular[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
const GLfloat high_shininess[] = { 10.0f };


void TGame::End()
	{
	font.StopFontEngine();
	joysticks.Free();
	}

void TGame::CheckNeededExtensions()
	{

	if ((const char *) glGetString(GL_VERSION))
			{
			std::string version = (const char *) glGetString(GL_VERSION);
			coutlog("INIT: OpenGL version "+version);
			}
	else   coutlog("INIT: unknown OpenGL version ");

	if (glewIsSupported("GL_VERSION_2_0"))
			{
			std::string slangvers=((const char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
			coutlog("      Shaders with the GLSL version "+slangvers+" found");
			float vers=atof(slangvers.c_str());
			//  ostringstream oss; oss << vers;
			//  coutlog(oss.str());
			if (vers>=1.2999)
				supportingShaders=1;
			else {   coutlog("       > GLSL version not supported"); supportingShaders=0; }

			}

	else
			{

			supportingShaders=0;
			}


	}

/*
void OutVideoInfo()
{
  const SDL_VideoInfo *inf=SDL_GetVideoInfo();




}*/

bool TGame::InitGL(int w,int h,int hw,int fs,int bpp)
	{
//  PrintModes();
	textures.clear();
	Uint32 flags=SDL_OPENGL  ;
	if (fs) flags|=SDL_FULLSCREEN;
	if (hw) flags|=SDL_HWSURFACE | SDL_DOUBLEBUF; else flags|=SDL_SWSURFACE;

	SDL_Surface *screen;


	if (AntiAliasing)
			{
// SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
			int naa=4;
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, naa);
			}
	else
			{
// SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 0);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
			SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
			}

	//OutVideoInfo();
	screenwidth=w;
	screenheight=h;
	screen = SDL_SetVideoMode( w, h, bpp, flags );
	if (!screen) return false;
	SDL_WM_SetCaption("Cubosphere","Cubosphere");


	//  atexit(SDL_Quit);

	;

	//OutVideoInfo();
	//screen = SDL_SetVideoMode( 800, 600, 24, SDL_OPENGL | SDL_HWSURFACE | SDL_FULLSCREEN);
	//screen = SDL_SetVideoMode( 1600, 900, 24, SDL_OPENGL | SDL_HWSURFACE | SDL_FULLSCREEN);
	//screen = SDL_SetVideoMode( 1024, 768, 24, SDL_OPENGL | SDL_HWSURFACE | SDL_FULLSCREEN);
//#ifdef WIN32

	glewInit();
//#endif

	CheckNeededExtensions();

	glClearColor(1,1,1,1);
	//glClearColor(0,0,0,1);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);

	glLightfv(GL_LIGHT0, GL_AMBIENT,  light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
//    glLightfv(GL_LIGHT0, GL_EMISSION, light_emission);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glMaterialfv(GL_FRONT, GL_AMBIENT,   mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE,   mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);

	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	InvalidateMaterial();
	glReady=1;
	return true;
	}

void TGame::Quit()
	{
	events.Close();
	}

void TGame::HandleInput()
	{
	keyboard.HandleKeys();
	joysticks.HandleKeys();
	}


static int framecounter=0;
static double ftime=0;

void TGame::HandleEvents()
	{
	events.HandleEvents();
	}

void TGame::GameLoopFrame()
	{
	if (GameLua.FuncExists("GameLoop"))
			{
			GameLua.CallVA("GameLoop","");
			}
	else
			{
			this->HandleEvents(); //This will parse keyboard and mouse input
			//Call the input's Handler
			this->HandleInput();
			//Do the physics
			this->Think();
			//Do the Render-Work
			this->Render();
			}

	}

void TGame::GameLoop()
	{

	double maxframeaccu=0;


	while (!events.Closed())
			{
			//Get the elapsed time
			oldtime=time;
			time = SDL_GetTicks();
			elapsed=time-oldtime;
			elapsed/=1000.0;

			maxframeaccu+=elapsed;
			if (maxframes>0)
					{
					if (maxframeaccu<1.0/maxframes) continue;
					else
							{
							elapsed=maxframeaccu;
							maxframeaccu=0;
							}
					}

			framecounter++;
			ftime+=elapsed;
			if (ftime>=1.0)
					{
					double fps=framecounter/ftime;

					FPS=(int)fps;
					ftime=0;
					framecounter=0;
					}

			if (minframes>0)
					{
					if (elapsed>1.0/minframes)
							{
							elapsed=1.0/minframes;
							}
					}





			TheGame=(TCuboGame*)this;

			GameLoopFrame();


			}
	}


int TGame::Init()
	{

	TheGame=(TCuboGame*)this;
	mouse.Initialize();
	joysticks.Initialize();
	events.SetMouse(&mouse);
	events.SetKeyboard(&keyboard);
	events.SetJoystick(&joysticks);
	FPS=0;
	glReady=0;
	return 0;
	}

void TGame::SetGameLoopSource(std::string s)
	{
	if (s=="") GameLoopSource="-"; //stdin
	else GameLoopSource=s;
	}

void TGame::Start()
	{
	TheGame=(TCuboGame*)this;
	g_CuboConsole()->Init();



	GameLua.Include(g_CuboLib());


//Prepare Looper
	if (GameLoopSource!="")
			{
			std::string line;
			std::istream * inp=NULL;
			std::ifstream file;
			if (GameLoopSource=="-") inp=&std::cin;
			else {
					file.open(GameLoopSource.c_str());
					inp=&file;
					}
			std::vector<std::string> cmds;
			while (inp->good())
					{
					getline((*inp), line);
					cmds.push_back(line);
					}
			GameLua.ExecStrings(cmds);
			}
	if (GameLoopSource=="")
			{
			std::vector<std::string> defaultcmds;
			defaultcmds.push_back("function GameLoop()");
			defaultcmds.push_back(" GAME_HandleEvents();");
			defaultcmds.push_back(" GAME_HandleInput();");
			defaultcmds.push_back(" GAME_Think();");
			defaultcmds.push_back(" GAME_Render();");
			defaultcmds.push_back("end;");
			GameLua.ExecStrings(defaultcmds);
			}

	if (GameLua.FuncExists("Init")) GameLua.CallVA("Init","");

	time=oldtime=SDL_GetTicks();
	GameLoop();
	}






////////////////////////////////////////////////////////////////7

void TCuboGame::SaveFramePic(std::string fname, int nw,int nh)
	{

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	int w=viewport[2];
	int h=viewport[3];

	FILE *fScreenshot;
	int nSize = w*h* 3;

	auto pixels = std::make_unique<GLubyte[]>(nSize);
	//GLubyte *pixels = new GLubyte [nSize];
	if (pixels == NULL) return;

	fScreenshot = fopen(fname.c_str(),"wb");

	glReadPixels(0, 0, w, h, GL_RGB,
			GL_UNSIGNED_BYTE, pixels.get());

	//Resizing
	if ( ((nw>=0) && (w!=nw)) || ((nh>=0) && (h!=nh)))
			{
			if (nw<0) nw=w; if (nh<0) nh=h;
			int nSize2 = nw*nh* 3;
			auto pixels2 = std::make_unique<GLubyte[]>(nSize2);
			gluScaleImage(GL_RGB,w,h,GL_UNSIGNED_BYTE,pixels.get(),nw,nh,GL_UNSIGNED_BYTE,pixels2.get());
			pixels = std::move(pixels2);
			w=nw; h=nh;
			nSize=nSize2;
			}

	std::string ext=fname.substr(fname.find_last_of(".") + 1);
	if (ext=="tga")
			{
			//convert to BGR format
			unsigned char temp;
			int i = 0;
			while (i < nSize)
					{
					temp = pixels[i];       //grab blue
					pixels[i] = pixels[i+2];//assign red to blue
					pixels[i+2] = temp;     //assign blue to red

					i += 3;     //skip to next blue byte
					}

			unsigned char TGAheader[12]= {0,0,2,0,0,0,0,0,0,0,0,0};
			unsigned char header[6] = {(unsigned char)(w%256),(unsigned char)(w/256),
									   (unsigned char)(h%256),(unsigned char)(h/256),24,0
									  };

			fwrite(TGAheader, sizeof(unsigned char), 12, fScreenshot);
			fwrite(header, sizeof(unsigned char), 6, fScreenshot);
			fwrite(pixels.get(), sizeof(GLubyte), nSize, fScreenshot);
			fclose(fScreenshot);



			}
	else if (ext=="jpg")
			{
			struct jpeg_compress_struct cinfo;
			struct jpeg_error_mgr jerr;

			JSAMPROW row_pointer[1];
			FILE *outfile;
			outfile = fopen( fname.c_str(), "wb" );
			cinfo.err = jpeg_std_error( &jerr );
			jpeg_create_compress(&cinfo);
			jpeg_stdio_dest(&cinfo, outfile);

			cinfo.image_width      = w;
			cinfo.image_height     = h;
			cinfo.input_components = 3;
			cinfo.in_color_space   = JCS_RGB;
			jpeg_set_defaults( &cinfo );
			jpeg_set_quality(&cinfo, 100, false);
			jpeg_start_compress( &cinfo, TRUE );
			while( cinfo.next_scanline < cinfo.image_height )
					{
					row_pointer[0] = &pixels[ (h-1-cinfo.next_scanline) * cinfo.image_width *  cinfo.input_components];
					jpeg_write_scanlines( &cinfo, row_pointer, 1 );
					}
			/* similar to read file, clean up after we're done compressing */
			jpeg_finish_compress( &cinfo );
			jpeg_destroy_compress( &cinfo );
			fclose( outfile );



			}

	}


void TCuboGame::ScreenShot(void)
	{

	std::string ext="jpg";

	char cFileName[64];
	std::string fname;
	int nShot=1;
	while (nShot < 128)
			{
			std::string form="screenshot%03d."+ext;
			sprintf(cFileName,form.c_str(),nShot);
			fname=cFileName;
			cls_FileWriteable *fw= g_BaseFileSystem()->GetFileForWriting("/user/"+fname);
			if (!fw) return;
			if (fw->WillOverwrite()) {
					delete fw;
					++nShot;
					if (nShot > 127)
							{
							coutlog("Screenshot limit of 128 reached! Delete some in your Cubosphere User dir",2);
							return;
							}
					continue;
					}
			fname=fw->GetHDDName();
			break;



			}
	SaveFramePic(fname);

	}

void TCuboGame::JoyAxisHandle(int joys,int axis,float val,float pval)
	{
	if (g_CuboConsole()->IsActive()) return ;

	if (MenuActive)
		menu.JoyAxisChange(joys,axis,val,pval);
	if (GameActive)
		lvl.JoyAxisChange(joys,axis,val,pval);
	}

int TCuboGame::StartLevel(std::string lname,int normal_user_edit)
	{

	if (normal_user_edit!=2 || lname!="")
			{
			TCuboFile * finfo=GetFileName(lname,normal_user_edit!=0 ? FILE_USERLEVEL : FILE_LEVEL,".ldef");
			if (!finfo) {std::string uls=(normal_user_edit!=0 ? "Userlevel" : "Level"); coutlog("Cannot find "+uls+": "+lname,2); return 0 ;}
			delete finfo;
			}

	if (normal_user_edit==1)
			{
			g_Vars()->SetVar("StartUserLevel",lname);
			g_Vars()->SetVar("StartLevel","");
			g_Vars()->SetVar("Editor_LoadLevel","");
			g_Vars()->SetVar("EditorMode",0);
			menu.LoadDef("startgame");
			}
	else if (normal_user_edit==0)
			{
			g_Vars()->SetVar("StartUserLevel","");
			g_Vars()->SetVar("StartLevel",lname);
			g_Vars()->SetVar("Editor_LoadLevel","");
			g_Vars()->SetVar("EditorMode",0);
			menu.LoadDef("startgame");
			}
	else if (normal_user_edit==2)
			{
			g_Vars()->SetVar("StartUserLevel","");
			g_Vars()->SetVar("Editor_LoadLevel",lname);
			g_Vars()->SetVar("StartLevel","");
			g_Vars()->SetVar("EditorMode",1);
			menu.LoadDef("editorstart");

			}

	GameActive=0;
	MenuActive=1;
	menu.PostThink();
	return 1;
	}


void TCuboGame::KeyHandle(int ident,int down,int toggle)
	{
	if (ident==-1) exit(0);
//cout << ident << " " << down << " " << toggle << endl;
	if ((ident==g_CuboConsole()->GetToggleKey() ) && down && toggle)
			{
			g_CuboConsole()->Toggle();
			return;
			}

	if (g_CuboConsole()->IsActive())
			{
			//Send key to console
			g_CuboConsole()->KeyHandle(ident,down,toggle);
			return;
			}

	if (g_CuboConsole()->CheckBindKey(ident,down,toggle)) {return;}

	if (MenuActive)
		menu.SendKey(ident,down,toggle);
	if (GameActive)
		lvl.SendKey(ident,down,toggle);
	}

void TCuboGame::DiscreteJoyHandle(int joy,int button,int dir,int down,int toggle)
	{
	if (g_CuboConsole()->IsActive()) return;

	if (MenuActive)
		menu.SendJoyButton(joy,button,dir,down,toggle);
	if (GameActive)
		lvl.SendJoyButton(joy,button,dir,down,toggle);
	}


int TCuboGame::AddActor(std::string aname)
	{
	int res=move.size();
	move.resize(move.size()+1);
	move[res]=new TCuboMovement();
	move[res]->Init(&lvl);
	move[res]->SetType(res,aname);
	move[res]->SetID(res);
	return res;
	}

void TCuboGame::DeleteActor(int index)
	{
	if ((index<0) || (index>=(int)(move.size()))) return;
	if (move[index]) {delete move[index]; move[index]=NULL;}
	move.erase(move.begin()+index); //CAREFUL! SHIFTS THE ACTORS INDICES
	for (unsigned int i=0; i<move.size(); i++) move[i]->SetID(i);
	}


int TCuboGame::AddEnemy(std::string aname)
	{
	int res=move.size();
	move.resize(move.size()+1);
	move[res]=new TCuboEnemy();
	move[res]->Init(&lvl);
	move[res]->SetType(res,aname);
	return res;
	}

void TCuboGame::Clear()
	{
	g_SpriteEnvs()->Clear();
	for (unsigned int i=0; i<move.size(); i++) if (move[i]) {delete move[i]; move[i]=NULL;}
	move.clear();
	basis.clear();
//move.resize(0);
	g_PathGraphs()->Clear();
	lvl.clear();
	}

int TCuboGame::AddBasis()
	{
	int res=basis.size();
	basis.resize(res+1);
	return res;
	}


int TCuboGame::Init()
	{
	TGame::Init();
	freecam=0;
	T2dVector wh;
	maxphyselapsed=1000; //Means only one phys calc
	FlushOrFinishBeforeSwap=0;
	StartBootScript("boot.lua");

	//menu.LoadDef("init");
	// if (CurrentMod()!="") menu.PostThink(); //To invoke another Call of Loading the "init.mdef"
	MenuActive=1;
	GameActive=0;
	NewMenuActive=1;
	NewGameActive=0;
	RenderPassID=0;
	keyboard.Init();
	keyboard.SetHandler(keyhandle);

	//joysticks.SetAxisHandler(joyaxishandle);
	joysticks.SetDiscreteHandler(joyhandle);


	player.push_back(new TCuboPlayer(0));

	cam.goUp(2*CUBO_SCALE);
	T2dVector nearfar(2.0,6000);
	cam.setNearFar(nearfar);
	font.Init();
	return 0;
	}


void TCuboGame::LoadSky(std::string name)
	{
// sky.LoadTextures(name,&textures);
	sky.LoadSkybox(name);
	}


void TCuboGame::Think()
	{

	//if (mouse.GetButton(0).pressed) move[0].Jump();


	/*   T2dVector ppos;
	   ppos=mouse.getRelativeMotion();
	   cam.rotateSide(ppos.v*0.002);
	   cam.rotateY(-ppos.u*0.002);
	*/
	g_Sounds()->Think(GetElapsed());

	if (GameActive)
			{

#ifdef PARALLELIZE
//cout << "Game loop with " << omp_get_num_threads() << " of " << omp_get_max_threads() << " threads" << endl;
			omp_set_num_threads(omp_get_max_threads());

#endif
			int numcalcs=1;
			double oldelapsed=elapsed;
			double interpolelapsed=elapsed;

			if (GetElapsed()>maxphyselapsed)
					{
					numcalcs=(int)ceil(GetElapsed()/maxphyselapsed);

					interpolelapsed=elapsed/(tfloat)numcalcs;
					}

#ifdef PARALLELIZE
			#pragma omp parallel for
#endif
			for (unsigned int i=0; i<move.size(); i++) {

					move[i]->Think();
					}

			elapsed=interpolelapsed;
			for (int calcstep=1; calcstep<=numcalcs; calcstep++)
					{
#ifdef PARALLELIZE
					#pragma omp parallel for
#endif
					for (unsigned int i=0; i<move.size(); i++) {
							//move[i].Think();
							move[i]->InterpolateMove(lvl.Elapsed());
							}


					//  cout << "lvl activeis " << lvl.GetCollisionChecksActive() << endl;
					lvl.CheckCollides();
					if (lvl.GetCollisionChecksActive())
							{
							for (unsigned int a=0; a<g_Game()->NumActors(); a++)
									{
									if (!(move[a]->IsPlayer())) continue;
									for (unsigned int e=0; e<g_Game()->NumActors(); e++)
										if (!(move[e]->IsPlayer())) move[a]->CheckEnemyCollision(move[e]);
										else if (a!=e) move[a]->CheckPlayerCollision(move[e]);
									}
							}
					}
			elapsed=oldelapsed;

#ifdef PARALLELIZE
			#pragma omp parallel for
#endif

			for (unsigned int i=0; i<move.size(); i++) {
					move[i]->FinalizeMove();

					}



			lvl.Think();

#ifdef PARALLELIZE
			#pragma omp parallel for
#endif

			for (unsigned int i=0; i<move.size(); i++) {
					move[i]->PostLevelThink();

					}


			if (!(player[0]->InCameraPan()))
					{
					int CamPlayer=cam.GetPlayer();
					if (CamPlayer>=0) move[CamPlayer]->SetCamPos(&cam);
					}
			else
					{
					player[0]->SetCameraPos(elapsed,&cam);
					}
			cam.Invalidate(); //This will recalc everythink

			g_SpriteEnvs()->Think(elapsed);

			}
	if (MenuActive) menu.Think();
	}

void TCuboGame::PreRender(int wo,int ho)
	{

	const SDL_VideoInfo* vidinfo = SDL_GetVideoInfo();
	T2dVector widthheight;

	if (wo<=0) wo=vidinfo->current_w;
	if (ho<=0) ho=vidinfo->current_h;


	widthheight.uv(wo,ho);
	cam.setScreenWidthHeight(widthheight);
	if (GameActive) lvl.FrameRenderStart();
	cam.think();

	}

void TCuboGame::AfterRenderLevel()
	{

	cam.postthink();
	if (GameActive) lvl.FrameRenderEnd();
	}

void TCuboGame::Render()
	{

	if (!glReady) return;


//AntiAliasing=1;

	if ((!g_PostEffect()) || (!GameActive))
			{
			if (!AntiAliasing)
					{
					PreRender();
					glClearColor(0,0,0,1);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					RenderPass();
					AfterRenderLevel();
					}
			else
					{
//      glClearAccum(0, 0, 0, 0);
					//    glClear(GL_ACCUM_BUFFER_BIT);
					glEnable(GL_MULTISAMPLE);
					PreRender();
					glClearColor(0,0,0,1);
					glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					RenderPass();
					AfterRenderLevel();
					glDisable(GL_MULTISAMPLE);
					//  glAccum(GL_ACCUM, 1.0);

					//glAccum(GL_RETURN, 0.5);
					}
			}
	else {
			PreRender();
			g_PostEffect()->CallRender();
			AfterRenderLevel();
			}





	if (GameActive) lvl.DrawHUD();
	if (MenuActive) menu.Render();

	g_CuboConsole()->Render();

	if (FlushOrFinishBeforeSwap==1)
		glFlush();
	else if (FlushOrFinishBeforeSwap==2)
		glFinish();

	SDL_GL_SwapBuffers();


	if (GameActive || NewGameActive) lvl.PostThink();
	if (MenuActive || NewMenuActive) menu.PostThink();

	GameActive=NewGameActive;
	MenuActive=NewMenuActive;
	}

void TCuboGame::RenderPass()
	{
	if (GameActive)
			{
			//sky.Render(cam.getPos());
//cout << "RENDERPASS" << endl;
			glColor4f(1,1,1,1);

			sky.Render();

			lvl.Render(&cam);
			for (unsigned int i=0; i<move.size(); i++) move[i]->Render();


			lvl.SortDistRenderList();
			lvl.DistRender(&cam);
			g_SpriteEnvs()->RenderAfterLevel();
			InvalidateMaterial();
			// lvl.FrameRenderEnd();
			RenderPassID++;
			}
	}

void TCuboGame::SpecialRenderPass(std::string nam,int defrender)
	{
	if (GameActive)
			{
			sky.SpecialRender(nam,defrender);
			lvl.SpecialRender(&cam,nam,defrender);
			for (unsigned int i=0; i<move.size(); i++) move[i]->SpecialRender(nam,defrender);
			lvl.SortDistRenderList();
			lvl.DistRender(&cam); //You have to keep in mind, that the special DistRender has to be set up in the individual lua script with SetVar, GetVar etc
			}
	}

void TCuboGame::Reload(std::vector<std::string> & extratoks)
	{
	int all=0; // Implicits => All
	int lua=0; // Implicits =>
	int textures=0; // Implicits =>
	int models=0; // Implicits =>
	int sounds=0; // Implicits =>
	int level=0; // Implicits =>
	int actordefs=0;
	int mdldefs=0;
	int menudefs=0;
	int tdefs=0, bdefs=0, idefs=0;
	int skydef=0;
	int language=0;

	if (!extratoks.size())  {
			FreeMedia();
			if (GameActive) lvl.Reload();
			if (MenuActive) menu.Reload(); return;
			}


	for (unsigned int i=0; i<extratoks.size(); i++)
			{
			if (extratoks[i]=="all") all=1;
			else if (extratoks[i]=="lua") lua=1;

			else if (extratoks[i]=="actordefs") actordefs=1;
			else if (extratoks[i]=="blockdefs") bdefs=1;
			else if (extratoks[i]=="enemydefs") actordefs=1;
			else if (extratoks[i]=="itemdefs") idefs=1;
			else if (extratoks[i]=="mdldefs") mdldefs=1;
			else if (extratoks[i]=="menudefs") menudefs=1;
			else if (extratoks[i]=="textures") textures=1;
			else if (extratoks[i]=="mdls") models=1;
			else if (extratoks[i]=="sounds") sounds=1;
			else if (extratoks[i]=="texdefs") tdefs=1;
			else if (extratoks[i]=="level") level=1;
			else if (extratoks[i]=="skydef") skydef=1;
			else if (extratoks[i]=="language") language=1;
			else { coutlog("switch "+extratoks[i]+" not defined",2); }
			}



	if (textures || all) this->textures.Reload();
	if (sounds || all) g_Sounds()->Reload();
	if (models || all) mdls.Reload();

	if (actordefs || all || lua) adefs.Reload();
	if (mdldefs || all || lua) mdefs.Reload();

	if (tdefs || all || lua) lvl.GetTexDefs()->Reload();
	if (bdefs || all || lua) lvl.GetBlockDefs()->Reload();
	if (idefs || all || lua) lvl.GetItemDefs()->Reload();

	if (skydef || all || lua) sky.Reload();
	if (language || all ) ReloadLanguage();

	if (menudefs || all || lua) menu.Reload();

	if (level || all ) lvl.Reload();

	}

void TCuboGame::FreeMedia()
	{
	// glReady=0;
	mdefs.clear();
	mdls.Clear();
	adefs.clear();
	g_SpriteEnvs()->Clear();
	g_ParticleDefs()->clear();
	lvl.clearAll();
	textures.clear();
///TODO: Free the Font cache!
///What about the sounds?
	font.ClearCache();
	GetShaders()->clear();
	if (g_PostEffect()) g_PostEffect()->UnPrecache();
	}

bool TCuboGame::InitGL(int w,int h,int hw,int fs,int bpp)
	{
	FreeMedia();
	return TGame::InitGL(w,h,hw,fs,bpp);

	}

TCuboGame::~TCuboGame()
	{
	for (unsigned int i=0; i<player.size(); i++) if (player[i]) {delete player[i]; player[i]=NULL;}

	}

void TCuboBasis::InvertMatrix()
	{
	T3dMatrix sub;
	matrix.getSubMatrix(&sub);
	sub=sub.inverse();
	matrix.setSubMatrix(sub);
	}



//////////////////////////LUA IMPLEMENTATION//////////////////////////


int BASIS_New(lua_State *state)
	{
	int res=g_Game()->AddBasis();
	LUA_SET_INT(res);
	return 1;
	}

int BASIS_SetAxisRotate(lua_State *state)
	{
	T3dVector pos=Vector3FromStack(state);
	tfloat angle=LUA_GET_DOUBLE;
	T3dVector axis=Vector3FromStack(state);

	int index=LUA_GET_INT;


	TCuboBasis *b=g_Game()->GetBasis(index);
	b->setPos(pos);
	b->SetBasisAxisRotate(axis,angle);
	return 0;
	}

int BASIS_Set(lua_State *state)
	{
	T3dVector pos=Vector3FromStack(state);
	T3dVector dir=Vector3FromStack(state);
	T3dVector up=Vector3FromStack(state);
	T3dVector side=Vector3FromStack(state);

	int index=LUA_GET_INT;


	TCuboBasis *b=g_Game()->GetBasis(index);
	b->setPos(pos);
	b->SetBasis(side,up,dir);
	return 0;
	}

int BASIS_Invert(lua_State *state)
	{

	int index=LUA_GET_INT;


	TCuboBasis *b=g_Game()->GetBasis(index);
	b->InvertMatrix();
	return 0;
	}

int BASIS_SetScale(lua_State *state)
	{
	T3dVector s=Vector3FromStack(state);
	int index=LUA_GET_INT;

	g_Game()->GetBasis(index)->setScale(s);

	return 0;
	}


int BASIS_SetPos(lua_State *state)
	{
	T3dVector s=Vector3FromStack(state);
	int index=LUA_GET_INT;

	g_Game()->GetBasis(index)->setPos(s);

	return 0;
	}

int BASIS_GetSide(lua_State *state)
	{
	int index=LUA_GET_INT;
	T3dVector v=g_Game()->GetBasis(index)->getSide();
	LUA_SET_VECTOR3(v);
	return 1;
	}

int BASIS_GetUp(lua_State *state)
	{
	int index=LUA_GET_INT;
	T3dVector v=g_Game()->GetBasis(index)->getUp();
	LUA_SET_VECTOR3(v);
	return 1;
	}

int BASIS_GetDir(lua_State *state)
	{
	int index=LUA_GET_INT;
	T3dVector v=g_Game()->GetBasis(index)->getDir();
	LUA_SET_VECTOR3(v);
	return 1;
	}

int BASIS_AxisRotate(lua_State *state)
	{
	float angle=LUA_GET_DOUBLE;
	T3dVector s=Vector3FromStack(state);
	int index=LUA_GET_INT;

	g_Game()->GetBasis(index)->rotateV(angle,s);

	return 0;
	}

int BASIS_Push(lua_State *state)
	{
	int index=LUA_GET_INT;
	g_Game()->GetBasis(index)->think();
// g_Game()->GetBasis(index)->PushMult();
	return 0;
	}

int BASIS_Pop(lua_State *state)
	{
	int index=LUA_GET_INT;
	g_Game()->GetBasis(index)->postthink();
	return 0;
	}

int BASIS_ChaseCam(lua_State *state)
	{
	int actor=LUA_GET_INT;
	int index=LUA_GET_INT;
	g_Game()->GetActorMovement(actor)->SetCamPos (g_Game()->GetBasis(index));
	return 0;
	}



void LUA_BASIS_RegisterLib()
	{
	g_CuboLib()->AddFunc("BASIS_New",BASIS_New);
	g_CuboLib()->AddFunc("BASIS_Invert",BASIS_Invert);
	g_CuboLib()->AddFunc("BASIS_SetScale",BASIS_SetScale);
	g_CuboLib()->AddFunc("BASIS_Push",BASIS_Push);
	g_CuboLib()->AddFunc("BASIS_Pop",BASIS_Pop);
	g_CuboLib()->AddFunc("BASIS_SetPos",BASIS_SetPos);
	g_CuboLib()->AddFunc("BASIS_AxisRotate",BASIS_AxisRotate);
	g_CuboLib()->AddFunc("BASIS_SetAxisRotate",BASIS_SetAxisRotate);

	g_CuboLib()->AddFunc("BASIS_ChaseCam",BASIS_ChaseCam); ///Gets the position from a ACTOR-Chase-Cam
	g_CuboLib()->AddFunc("BASIS_Set",BASIS_Set);
	g_CuboLib()->AddFunc("BASIS_GetSide",BASIS_GetSide);
	g_CuboLib()->AddFunc("BASIS_GetUp",BASIS_GetUp);
	g_CuboLib()->AddFunc("BASIS_SetDir",BASIS_GetDir);
	}




int GAME_HandleEvents(lua_State *state)
	{
	g_Game()->HandleEvents();
	return 0;
	}

int GAME_Think(lua_State *state)
	{
	g_Game()->Think();
	return 0;
	}


int GAME_HandleInput(lua_State *state)
	{
	g_Game()->HandleInput();
	return 0;
	}

int GAME_GetRenderPassID(lua_State *state)
	{
	LUA_SET_INT(g_Game()->GetRenderPassID());
	return 1;
	}

int GAME_Render(lua_State *state)
	{
	g_Game()->Render();
	return 0;
	}

int GAME_SetGameLoopSource(lua_State *state)
	{
	std::string s;
	s=LUA_GET_STRING;
	g_Game()->SetGameLoopSource(s);
	return 0;
	}

int GAME_SetFlushOrFinishBeforeSwap(lua_State *state)
	{
	int i=LUA_GET_INT;
	g_Game()->SetFlushOrFinishBeforeSwap(i);
	return 0;
	}

int GAME_GetFlushOrFinishBeforeSwap(lua_State *state)
	{
	int i= g_Game()->GetFlushOrFinishBeforeSwap();
	LUA_SET_INT(i);
	return 1;
	}

void LUA_GAME_RegisterLib()
	{
	g_CuboLib()->AddFunc("GAME_SetGameLoopSource",GAME_SetGameLoopSource);
	g_CuboLib()->AddFunc("GAME_HandleEvents",GAME_HandleEvents);
	g_CuboLib()->AddFunc("GAME_HandleInput",GAME_HandleInput);
	g_CuboLib()->AddFunc("GAME_Think",GAME_Think);
	g_CuboLib()->AddFunc("GAME_Render",GAME_Render);
	g_CuboLib()->AddFunc("GAME_GetRenderPassID",GAME_GetRenderPassID);
	g_CuboLib()->AddFunc("GAME_SetFlushOrFinishBeforeSwap",GAME_SetFlushOrFinishBeforeSwap);
	g_CuboLib()->AddFunc("GAME_GetFlushOrFinishBeforeSwap",GAME_GetFlushOrFinishBeforeSwap);


	}
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
