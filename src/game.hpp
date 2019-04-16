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

#include "c3dobjects.hpp"
#include "mouse.hpp"
#include "textures.hpp"
#include "keyboard.hpp"
#include "joystick.hpp"
#include "events.hpp"
#include "cuboactors.hpp"
#include "skyboxes.hpp"
#include "cubolevels.hpp"
#include "shaderutil.hpp"
#include "models.hpp"
#include "fonts.hpp"

class Game {
	protected:
		//It owns a cam, a texture server and input devices
		int AntiAliasing;
		Camera cam;
		TextureServer textures;
		Mouse mouse;
		Keyboard keyboard;
		JoystickServer joysticks;
		EventManager events;
		ShaderServer shaders;
		Font font;
		bool IsHWRender = true;
		int screenwidth;
		int screenheight;
		int supportingShaders;
		int glReady;
		float minframes,maxframes; //actually this arent the frames but it ensures that the time is slowed down if we are below
		std::string GameLoopSource;
		double time,elapsed,oldtime;
		float maxphyselapsed;
		int FPS;
		virtual void GameLoop();
		virtual void GameLoopFrame();
		virtual void CheckNeededExtensions();
		LuaAccess GameLua;
		SDL_Window *sdlWindow;
		SDL_Renderer *sdlRenderer;
	public:
		virtual void SetGameLoopSource(std::string s);
		Game() :  AntiAliasing(0), maxframes(0), GameLoopSource("") {}
		virtual ~Game() {}
		virtual void HandleInput();
		virtual void Think() {};
		virtual void Render() {};
		virtual void HandleEvents();
		virtual void SetAntiAliasing(int aa) {AntiAliasing=aa;}
		//void SetVSync(bool type);

		virtual void End();
		virtual Vector2d GetScreenSize() {Vector2d res(screenwidth,screenheight); return res;}
		virtual void KeyHandle(int ident,int down,int special) {}
		virtual void TextInputHandle(std::string&) {}
		virtual void JoyAxisHandle(int joys,int axis,float val,float pval) {}
		virtual int Init();
		virtual void SetMinFrames(float t) {minframes=t;}
		virtual void SetMaxFrames(float t) {maxframes=t;}
		virtual void Quit();
		virtual void Start();
		virtual TextureServer* GetTextures() {return &textures;}
		virtual ShaderServer* GetShaders() {return &shaders;}
		virtual double GetTime() {return time/1000.0;}
		virtual double GetElapsed() {return elapsed;}
		virtual Camera *GetCam() {return &cam;}
		virtual int GetFPS() {return FPS;}
		virtual Keyboard *GetKeyboard() {return &keyboard;}
		virtual JoystickServer *GetJoysticks() {return &joysticks;}
		virtual bool InitGL(int w,int h,int hw,int fs);
		bool UpdateWindow(int w, int h, int hw, int fs);
		virtual bool SetHWRender(bool hw);
		virtual int HasGLSL() {return supportingShaders;}
		virtual void DiscreteJoyHandle(int joys,int button,int dir,int down,int toggle) {}
		virtual void SetMaxPhysElapsed(double t) {if (t>0) maxphyselapsed=t; else maxphyselapsed=100000;}
	};

class CuboBasis : public WorldObject {
	public:
		virtual void InvertMatrix();
	};

class CuboGame : public Game {
	protected:
		int freecam;
		std::vector<TCuboMovement*> move;
		std::vector<CuboPlayer*> player;
		std::vector<CuboBasis> basis; //3d-Matrices
		CuboLevel lvl;
		SkyBox sky;
		MdlDefServer mdefs;
		ModelServer mdls;
		ActorDefServer adefs;

		Menu menu;
		int RenderPassID;
		int MenuActive;
		int GameActive;
		int NewMenuActive,NewGameActive; //This will be set at the end of a frame
		int FlushOrFinishBeforeSwap;

	public:
		~CuboGame();
		void SetFlushOrFinishBeforeSwap(int i) {FlushOrFinishBeforeSwap=i;}
		int GetFlushOrFinishBeforeSwap() {return FlushOrFinishBeforeSwap;}
		virtual int StartLevel(std::string lname,int normal_user_edit);
		SkyBox * GetSky() {return &sky;}
		virtual void SaveFramePic(std::string fname, int nw=-1, int nh=-1);
		virtual void PreRender(int wo=-1,int ho=-1);
		virtual void AfterRenderLevel();
		virtual void Render();
		virtual void Think();
		virtual void ScreenShot();
		virtual void KeyHandle(int ident,int down,int toggle);
		virtual void TextInputHandle(std::string&);
		virtual void JoyAxisHandle(int joys,int axis,float val,float pval);
		virtual void DiscreteJoyHandle(int joys,int button,int dir,int down,int toggle);
		virtual int Init();
		virtual CuboLevel *GetLevel() {return &lvl;}
		virtual TCuboMovement *GetActorMovement(int i) {return (move[i]);}
		virtual int GetNumActors() {return (move.size());}
		virtual CuboPlayer *GetPlayer(int i) {return (player[i]);}
		virtual int GetNumPlayers() {return (player.size());}
		virtual void DeleteActor(int index);
		virtual unsigned int NumActors() {return move.size();}
		virtual MdlDefServer *GetModelDefs() {return &mdefs;}
		virtual ModelServer *GetModels() {return &mdls;}
		virtual ActorDefServer *GetActorDefs() {return &adefs;}
		virtual Mouse *GetMouse() {return &mouse;}
		virtual void LoadSky(std::string name);
		virtual int AddActor(std::string aname);
		virtual int AddEnemy(std::string aname);
		virtual void Clear();
		virtual int AddBasis();
		virtual void SetMenuActive(int m) {NewMenuActive=m;}
		virtual void SetGameActive(int g) {NewGameActive=g;}
		virtual CuboBasis *GetBasis(int i) {return &(basis[i]);}
		virtual Font* GetFont() {return &font;}
		virtual Menu* GetMenu() {return &menu;}
		bool InitGL(int w, int h, int hw, int fs);
		virtual bool UpdateWindow(int w,int h,int hw,int fs);
		virtual void FreeMedia();
		virtual void RenderPass();
		virtual void SpecialRenderPass(std::string nam,int defrender);
		virtual void Reload(std::vector<std::string> & extratoks);
		virtual int GetRenderPassID() const {return RenderPassID;}
		//  virtual int GetGlobalVarInt(string name);
	};
CuboGame *g_Game();


void LUA_BASIS_RegisterLib();
void LUA_GAME_RegisterLib();
