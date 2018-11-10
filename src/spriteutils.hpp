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
#include "definitions.hpp"
#include <string>
#include <vector>

class TSpriteEmitter;


#define SPRITE_ENV_ATTACHED_TO_WORLD 0
#define SPRITE_ENV_ATTACHED_TO_SIDE 1
#define SPRITE_ENV_ATTACHED_TO_ACTOR 2
///Todo: Add more, Actor, Basis, etc ....


class TSpriteEnvironments;

//Store the physics, Gravity(direction,strength,relativeattachment)
//And the Bounding-Box



class TSpriteEnvironment
	{
	protected:
		friend class TSpriteEnvironments;
		friend class TSpriteEmitter;
		int id;
		double elapsed;
		Vector3d gravity_in_ownbase;
		Vector3d gravity_worldcoords;
		int attachtype; //i.e. World, side etc
		int attachedid; //For sides etc...
		Vector3d vup,vdir,vside,vpos;
		Matrix3d base;
		std::vector<TSpriteEmitter*> emitters;
		int died;
		void RefreshBasis();
	public:
		Vector3d GetGravity() {return vup*(-1);}//TODO: MAke it changeable
		Vector3d GetPos() {return vpos;}
		TSpriteEmitter * GetEmitter(int i) {return emitters[i];}
		void SetID(int i) {id=i;}
		int GetID() {return id;}
		int AddEmitter(int defindex);
		void AttachOnSide(int sid);
		void AttachOnActor(int aid);
		~TSpriteEnvironment();
		void Clear();
		TSpriteEnvironment() : attachtype(0), vup(0,1,0), vdir(0,0,1), vside(1,0,0), vpos(0,0,0), died(0) {}
		int Died() {return died;}
		void Think(double elaps);
		// void RenderAfterLevel();
	};


typedef struct
	{
	int typ;
	Vector3d pos,vel;
	Vector4d col;
	float lifetime;
	float timeleft;
	float  scale;
	float  grav;
	float  rotspeed;
	float  rot;
	float  rgbphase;
	} TSprite;


//: public TSprite :: EMITTER KEIN SPRITE... Notfalls einen ortsfesten SPRITE erstellen und adden

class TSpriteEmitter
	{
	protected:
		int id;
		TSpriteEnvironment *env;
		int defindex;
		int died;
		std::vector<TSprite> sprites;
		int activesprites;
		int relpos;
		Vector3d pos;
		Vector3d vel;
		Vector4d colmultiply;
		Vector3d grav;
		double gravfactor; //Own "Mass" on gravity
		double scalefactor;
		TLuaVarHolder varholder;
		double maxtimeinterval;
		Vector3d cull_center;
		double cull_radius;
		double cam_dist;
		int lastsprite;
	public:
		static bool cmp_emitterdists(const TSpriteEmitter *a, const TSpriteEmitter *b);
		double GetCullInformation();
		void SetMaxTimeInterval(double mti) {maxtimeinterval=mti;}
		double GetMaxTimeInterval() {return maxtimeinterval;}
		void Die() {died=1;}
		void Clear() {sprites.clear();}
		void SetPos(Vector3d p,int rel) ;
		void SetVel(Vector3d v) {vel=v;}
		void SetGrav(Vector3d v,double f) {grav=v; gravfactor=f;}
		void SetColorMultiply(Vector4d c) {colmultiply=c;}
		void SetScaleMultiply(double sm) {scalefactor=sm;}
		TLuaVarHolder  *GetVarHolder() {return &varholder;}
		int GetActiveSprites() {return activesprites;}
		Vector3d GetPos(int rel);
		void SpawnSprite(int typ,double theta,double phi,double vel);
		void SetID(int i) {id=i;}
		int GetID() {return id;}
		void Init(int defind,TSpriteEnvironment *envi);
		TSpriteEmitter() : died(0),activesprites(0), relpos(1), pos(0,0,0), vel(0,0,0),colmultiply(1,1,1,1), grav(0,0,0), gravfactor(0), scalefactor(1), maxtimeinterval(-1), lastsprite(-1) {}
		int Died() {return died;}
		void Think();
		Vector3d RelativePosToWorldPos(Vector3d rp);
		void RenderAfterLevel();
		TSprite *GetLastSprite() {return &(sprites[lastsprite]);}
	};

////////////////////////////////////

class TInterpolationFunction
	{
	protected:
		double p1,p2,p3,p4; //Parameters
		int clampmode; //0: Clamped, 1:Periodic
		double Clamp(double arg) { if  (!clampmode) {if (arg<0) return 0; else if (arg>1) return 1; return arg;} else return arg-(int)arg;}
	public:
		virtual ~TInterpolationFunction() {}
		void SetParameters(double pa1,double pa2,double pa3,double pa4,int cm) {p1=pa1; p2=pa2; p3=pa3; p4=pa4; clampmode=cm;}
		TInterpolationFunction() : p1(0),p2(0),p3(0),p4(0),  clampmode(0) {}
		TInterpolationFunction(double c) : p1(c),p2(0),p3(0),p4(0),  clampmode(0) {}
		virtual double GetValue(double arg) {return Clamp(arg);}
	};

class TInterpolationFunctionConst : public TInterpolationFunction
	{
	public:
		TInterpolationFunctionConst(double c) : TInterpolationFunction(c) {}
		virtual double GetValue(double arg) {return p1;}
	};

class TInterpolationFunctionLinear : public TInterpolationFunction
	{
	public:
		virtual double GetValue(double arg) {arg=Clamp(arg); if (arg<=p1) return p3; if (arg>=p2) return p4; return (p4-p3)/(p2-p1)*(arg-p1)+p3;  }
	};

class TInterpolationFunctionSin : public TInterpolationFunction
	{
	public:
		virtual double GetValue(double arg) {arg=Clamp(arg); return p1*sin(6.28318531*(p2*arg+p3))+p4;  }
	};


class TParticleDef;

class TSpriteDef
	{
	protected:
		friend class TParticleDef;
		friend class TSpriteEmitter;
		int textureind;
		double lifetime;
		//double scale;
		TInterpolationFunction *scalef;
		TInterpolationFunction *colrf;
		TInterpolationFunction *colgf;
		TInterpolationFunction *colbf;
		TInterpolationFunction *alphaf;
		TInterpolationFunction *gravf;

	public:
		TSpriteDef() : textureind(-1), lifetime(1), scalef(new TInterpolationFunctionConst(1)),
			colrf(new TInterpolationFunctionConst(1)),colgf(new TInterpolationFunctionConst(1)),
			colbf(new TInterpolationFunctionConst(1)), alphaf(new TInterpolationFunctionConst(1)),
			gravf(new TInterpolationFunctionConst(0)) {}
		void SetScalef(TInterpolationFunction *cf) {if (scalef) delete scalef; scalef=cf;}
		void SetGravityf(TInterpolationFunction *cf) {if (gravf) delete gravf; gravf=cf;}
		void SetColorRf(TInterpolationFunction *cf) {if (colrf) delete colrf; colrf=cf;}
		void SetColorGf(TInterpolationFunction *cf) {if (colgf) delete colgf; colgf=cf;}
		void SetColorBf(TInterpolationFunction *cf) {if (colbf) delete colbf; colbf=cf;}
		void SetColorAf(TInterpolationFunction *cf) {if (alphaf) delete alphaf; alphaf=cf;}

		Vector4d GetCol(double arg,double rgbphase) {return Vector4d(colrf->GetValue(arg+rgbphase),colgf->GetValue(arg+rgbphase),colbf->GetValue(arg+rgbphase),alphaf->GetValue(arg));}

		void SetLifeTime(double lt) {lifetime=lt;}
		void Render(TSprite &s);
	};

class TParticleDef : public BaseLuaDef
	{
	protected:
		std::vector<TSpriteDef*> spritedefs;
		virtual int SendIDWhenPrecache() {return 1;}
	public:
		virtual int GetType() {return FILE_PARTICLEDEF;}
		std::vector<TSpriteDef*> & GetSpriteDefs() {return spritedefs;}
		virtual ~TParticleDef();
		void Call_EmitterConstructor(int eid);
		void Call_EmitterThink(int eid,double elapsed);
		int NewSpriteType(int tind);
	};

typedef BaseDefServer<TParticleDef> TParticleDefServer;

extern TParticleDefServer * g_ParticleDefs();

//////////////////


class TSpriteEnvironments
	{
	protected:
		std::vector<TSpriteEnvironment*> envs;
		std::vector<TSpriteEmitter*> renderemitters; //Storing all emitters, that are in the current frustum
		double timescale;
	public:
		TSpriteEnvironments() : timescale(1) {}
		TSpriteEnvironment* GetEnv(int i);
		int CreateEnv();
		void Clear();
		void RenderAfterLevel();
		void Think(double elapsed);
		void SetTimeScale(double ts) {timescale=ts;}
		~TSpriteEnvironments();
	};

extern TSpriteEnvironments * g_SpriteEnvs();

//////////////////


class TLuaParticleLib : public TLuaCFunctions
	{
	protected:

	public:
		TLuaParticleLib();
	};


extern TLuaParticleLib* g_ParticleLib();

//Sprite-Defs (Lua) für Emitter

///LUA-Vorgehensweise:
/*

///////////////////////////
//Z.B von Sand-Bdef
Precache:
    Lade Emitter-Def Sandcloud
    Erstelle Environment, gebe Physikalische Eigenschaften und Attache and die Seite

Player Springt auf Sand:
    Füge Sandcloud-Emitter hinzu

///////////////////////////
//Emitterdef:
Precache:
    Lade Sprite-Texturen
    Füge Sprite-Info zum Emitter hinzu (Sprite-Params werden gesetzt)


Think:
   Spawne Sprites wie es beliebt

*/

// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
