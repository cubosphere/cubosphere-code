#ifndef SPRITEUTILS_H_G
#define SPRITEUTILS_H_G

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
		T3dVector gravity_in_ownbase;
		T3dVector gravity_worldcoords;
		int attachtype; //i.e. World, side etc
		int attachedid; //For sides etc...
		T3dVector vup,vdir,vside,vpos;
		T3dMatrix base;
		std::vector<TSpriteEmitter*> emitters;
		int died;
		void RefreshBasis();
	public:
		T3dVector GetGravity() {return vup*(-1);}//TODO: MAke it changeable
		T3dVector GetPos() {return vpos;}
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
	T3dVector pos,vel;
	T4dVector col;
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
		T3dVector pos;
		T3dVector vel;
		T4dVector colmultiply;
		T3dVector grav;
		double gravfactor; //Own "Mass" on gravity
		double scalefactor;
		TLuaVarHolder varholder;
		double maxtimeinterval;
		T3dVector cull_center;
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
		void SetPos(T3dVector p,int rel) ;
		void SetVel(T3dVector v) {vel=v;}
		void SetGrav(T3dVector v,double f) {grav=v; gravfactor=f;}
		void SetColorMultiply(T4dVector c) {colmultiply=c;}
		void SetScaleMultiply(double sm) {scalefactor=sm;}
		TLuaVarHolder  *GetVarHolder() {return &varholder;}
		int GetActiveSprites() {return activesprites;}
		T3dVector GetPos(int rel);
		void SpawnSprite(int typ,double theta,double phi,double vel);
		void SetID(int i) {id=i;}
		int GetID() {return id;}
		void Init(int defind,TSpriteEnvironment *envi);
		TSpriteEmitter() : died(0),activesprites(0), relpos(1), pos(0,0,0), vel(0,0,0),colmultiply(1,1,1,1), grav(0,0,0), gravfactor(0), scalefactor(1), maxtimeinterval(-1), lastsprite(-1) {}
		int Died() {return died;}
		void Think();
		T3dVector RelativePosToWorldPos(T3dVector rp);
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

		T4dVector GetCol(double arg,double rgbphase) {return T4dVector(colrf->GetValue(arg+rgbphase),colgf->GetValue(arg+rgbphase),colbf->GetValue(arg+rgbphase),alphaf->GetValue(arg));}

		void SetLifeTime(double lt) {lifetime=lt;}
		void Render(TSprite &s);
	};

class TParticleDef : public TBaseLuaDef
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

typedef TBaseDefServer<TParticleDef> TParticleDefServer;

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






#endif
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
