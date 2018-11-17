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

#include "sounds.hpp"
#include "globals.hpp"
#include "luautils.hpp"
#include "game.hpp"

#ifdef WIN32

#ifdef MINGW_CROSS_COMPILE
#include <GL/glew.h>
#else
#include <GL\glew.h>
#endif
// #include <GL\gl.h>
#include <windows.h>
#include <SDL.h>
#include <SDL_mixer.h>


#ifdef VCPP
#define M_PI 3.14159265358979323846
#endif

#else

#include <GL/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#endif

#include <vector>
#include <string>
#include <iostream>
#include <cmath>

static SoundServer soundserv;


SoundServer *g_Sounds() {return &soundserv;}

static void s_ChannelFinished(int num)
	{
	g_Sounds()->ChannelFinished(num);
	//cout << "Channel " << num << " done " << endl;

	}



void SoundServer::ChannelFinished(int num)
	{
	Mix_UnregisterAllEffects(num);

	if (num<(int)(playchannels.size()) && num>=0) {
			playchannels[num]=-1;
			}

	}


std::string SoundServer::currenteffect="";
std::vector<int> SoundServer::effectbuffer;
int SoundServer::effectpos;
double SoundServer::postmixparam[3];
int SoundServer::bitsps;

void SoundServer::EchoEffect(void *udata, Uint8 *stream, int len)
	{
	Sint16 *Ustr=(Sint16 *)stream;
	len/=4;
	//cout << "Stream of length "<< len << endl;
	int offs=(int)(postmixparam[0]*((double)bitsps)*2);
	if (offs % 2==0) { offs++; }
	int effectpos2=effectpos-offs;
	while (effectpos2<0) { effectpos2+=effectbuffer.size(); }
	while (effectpos2>=(int)(effectbuffer.size())) { effectpos2-=effectbuffer.size(); }

	int bp=0;
	for (int i=0; i<len; i++) {
			//  int ch1=SDL_SwapLE16(Ustr[bp]) ;
			// int ch2=SDL_SwapLE16(Ustr[bp+1]) ;
			int ch1=Ustr[bp] ;
			int ch2=Ustr[bp+1] ;


			ch2=(int)(ch2*postmixparam[1]+effectbuffer[effectpos2++]*postmixparam[2]);
			ch1=(int)(ch1*postmixparam[1]+effectbuffer[effectpos2++]*postmixparam[2]);

			effectbuffer[effectpos++]=ch1;
			effectbuffer[effectpos++]=ch2;

			Ustr[bp++]=( ch1);
			Ustr[bp++]=( ch2);


			// effectpos+=2;
			if (effectpos>(int)effectbuffer.size()) { effectpos-=effectbuffer.size(); }
			//   effectpos2+=2;
			if (effectpos2>(int)effectbuffer.size()) { effectpos2-=effectbuffer.size(); }
			}
	}

void SoundServer::SetPostMix(std::string what,double p1,double p2,double p3)
	{
	if (!initialized) { return; }
	postmixparam[0]=p1; postmixparam[1]=p2; postmixparam[2]=p3;

	if (what==currenteffect) { return; }
	if (what=="") { Mix_SetPostMix(NULL,NULL); }
	else if (what=="echo")
			{
			int channels;
			Uint16 format;
			Mix_QuerySpec(NULL, &format, &channels);
			if (channels!=2 || ((format !=AUDIO_S16LSB) ) ) { return; }
			effectpos=0;
			for (unsigned int i=0; i<effectbuffer.size(); i++) { effectbuffer[i]=0; }
			Mix_SetPostMix(EchoEffect,(void*)&(postmixparam[0]));
			}
	currenteffect=what;
	}


SoundContainer::~SoundContainer()
	{
	if (sound) { Mix_FreeChunk(sound); }
	sound=NULL;
	}

int SoundContainer::Load(CuboFile *finfo)
	{
	sound = Mix_LoadWAV_RW(finfo->GetAsRWops(1),1);
	if(sound == NULL) {
			printf("Unable to load WAV file %s : %s\n", finfo->GetNameForLog().c_str(), Mix_GetError());
			return 0;
			}
	fname=finfo->GetName();
	return 1;
	}

void SoundContainer::Reload()
	{

	CuboFile* finfo=g_BaseFileSystem()->GetFileForReading(fname);
	if (!finfo) {coutlog("Cannot reload sound " +fname,2); return;}

	if (sound) { Mix_FreeChunk(sound); }
	sound=NULL;

	Load(finfo); delete finfo;
	}


MusicContainer::~MusicContainer()
	{
	if (music) { Mix_FreeMusic(music); }
	music=NULL;
	}

int MusicContainer::Load(CuboFile *finfo)
	{
	if (!finfo->IsHDDFile())
			{
			coutlog("Music from ZIP can be problematic...",2);
			music = Mix_LoadMUS_RW(finfo->GetAsRWops(1));
			}
	else {
			music = Mix_LoadMUS(finfo->GetHDDName().c_str());
			}

	if(music == NULL) {
			std::ostringstream oss; oss << "Unable to load music file " << finfo->GetNameForLog().c_str() << Mix_GetError() ;
			coutlog(oss.str(),2);
			return 0;
			}
	fname=finfo->GetName();
	hasloop=false;
	currentpos=0;
	return 1;
	}

void MusicContainer::SetLoop(double from,double to)
	{
	if (((from==0) &&(to==0)) || (from>to)) {hasloop=false; return;}
	hasloop=true;
	loopfrom=from;
	loopto=to;
	}

void MusicContainer::JumpTo(double pos)
	{
	currentpos=pos;
	Mix_RewindMusic();
	Mix_SetMusicPosition(pos);
	}

void MusicContainer::Think(double elapsed)
	{
	currentpos+=elapsed;
	if (!hasloop) { return; }
	if (currentpos<loopto) { return; }

	double dest=currentpos-loopto+loopfrom;
	while (dest>loopto) { dest-=loopto-loopfrom; }
	Mix_RewindMusic();
	if (dest>0) { Mix_SetMusicPosition(dest); }
	else { dest=0; }

	currentpos=dest;
	}

void MusicContainer::Reload()
	{

	CuboFile* finfo=g_BaseFileSystem()->GetFileForReading(fname);
	if (!finfo) {coutlog("Cannot reload music " +fname,2); return;}

	if (music) { Mix_FreeMusic(music); }
	music=NULL;

	Load(finfo); delete finfo;
	}




void SoundServer::KillSound()
	{
	if (!initialized) { return; }
	Mix_HaltChannel(-1);
	Mix_HaltMusic();

	for (unsigned int i=0; i<sounds.size(); i++)
			{
			if (sounds[i]) { delete sounds[i]; }
			sounds[i]=NULL;
			}
	for (unsigned int i=0; i<musics.size(); i++)
			{
			if (musics[i]) { delete musics[i]; }
			musics[i]=NULL;
			}
	sounds.clear();
	musics.clear();
//  playchannels.clear();

	Mix_CloseAudio();

	initialized=0;
	}

int SoundServer::InitSound(int freq,int bits,int stereo,int buffsize)
	{

	printf("Sound init with Freq(%d), Bits(%d), Stereo(%d), Buffer(%d) \n",freq,bits,stereo,buffsize);
	KillSound();
	if (bits==8) { bits=AUDIO_S8; }
	else { bits=AUDIO_S16SYS; }
	//else bits=AUDIO_S16;
	printf("... \n");
	if(Mix_OpenAudio(freq, bits, stereo, buffsize) != 0) {
			printf("Unable to initialize audio: %s\n", Mix_GetError());
			initialized=0;
			return 0;
			}
	bitsps=bits;
	printf("... \n");
	Mix_ChannelFinished(s_ChannelFinished);
	initialized=1;
	SetNumChannels(16);
	currenteffect="";
	effectbuffer.resize(256*buffsize,0);
	printf("ok\n");
	return 1;

	}

int SoundServer::SetNumChannels(int nchan)
	{
	if (nchan>=0) {
			int oldsize=playchannels.size();
			if (nchan<oldsize) { playchannels.resize(nchan); }
			else for (int i=oldsize; i<nchan; i++) { playchannels.push_back(-1); }
			std::cout << "Channels are " << playchannels.size() << std::endl;
			}
	return Mix_AllocateChannels(nchan);
	}

int SoundServer::LoadSound(CuboFile *finfo)
	{
	if (!initialized) { return -1; }
	for (unsigned int i=0; i<sounds.size(); i++)
			{
			if (finfo->GetNameForLog()==sounds[i]->Filename()) { return i; }
			}
//Otherwise add it
	if (g_VerboseMode()) { coutlog("Loading Sound: "+finfo->GetNameForLog()); }
	SoundContainer *news=new SoundContainer();

	int res=news->Load(finfo);
	if (res) {
			sounds.push_back(news);
			return sounds.size()-1;
			}
	delete news;
	return -1;
	}

int SoundServer::LoadMusic(CuboFile *finfo)
	{
	if (!initialized) { return -1; }
	for (unsigned int i=0; i<musics.size(); i++)
			{
			if (finfo->GetNameForLog()==musics[i]->Filename()) { return i; }
			}
	if (g_VerboseMode()) { coutlog("Loading Music: "+finfo->GetNameForLog()); }
//Otherwise add it
//However, limit the size to one.. so we must delete the old one
	musics.resize(0);
	MusicContainer *newm=new MusicContainer();

	int res=newm->Load(finfo);
	if (res) {
			musics.push_back(newm);
			return musics.size()-1;
			}
	delete newm;
	return -1;
	}

int SoundServer::SoundPlayedByChannel(int i)
	{
	if (i<0 || i>=(int)(playchannels.size())) { return -1; }
	return playchannels[i];
	return -1;
	};


SoundContainer *SoundServer::GetSound(int i)
	{
	if (i<0) { return NULL; }
	if ((unsigned int)i>=sounds.size()) { return NULL; }
	return sounds[i];
	}
MusicContainer *SoundServer::GetMusic(int i)
	{
	if (i<0) { return NULL; }
	if ((unsigned int)i>=musics.size()) { return NULL; }
	return musics[i];
	}

int SoundServer::PlaySound(int index, int channel)
	{
	if (!initialized) { return -1; }
	SoundContainer *s=GetSound(index);
	if (!s) { return -1; }
	int c=Mix_PlayChannel(channel,s->GetSound(),0);
	if (c<0) { return c; }
	if (c>=(int)(playchannels.size())) { playchannels.resize(c+1); }
	playchannels[c]=index;
	return c;

	}

int SoundServer::PlaySoundLooped(int index, int channel, int numloops)
	{
	if (!initialized) { return -1; }
	SoundContainer *s=GetSound(index);
	if (!s) { return -1; }
	int c=Mix_PlayChannel(channel,s->GetSound(),numloops);
	if (c<0) { return c; }
	if (c>=(int)(playchannels.size())) { playchannels.resize(c+1); }
	playchannels[c]=index;
	return c;
	}

void SoundServer::StopChannel(int chind)
	{
	if (!initialized) { return; }
	Mix_HaltChannel(chind);
	if (chind>=0 && chind<(int)(playchannels.size())) { playchannels[chind]=-1; }
	}

int SoundServer::PlayMusic(int index)
	{
	if (!initialized) { return -1; }
	MusicContainer *m=GetMusic(index);
	if (!m) { return -1; }
	return Mix_PlayMusic(m->GetMusic(),-1);
	}

void SoundServer::MusicJumpTo(double p)
	{
	if (!initialized) { return ; }
	MusicContainer *m=GetMusic(0);
	if (!m) { return ; }
	m->JumpTo(p);
	}


int SoundServer::PlayingMusic()
	{
	if (!initialized) { return 0; }
	return Mix_PlayingMusic();
	}

void SoundServer::RewindMusic()
	{
	if (!initialized) { return; }
	MusicContainer *m=GetMusic(0);
	if (m) { m->Rewind(); }
	Mix_RewindMusic();
	}

void SoundServer::Think(double elapsed)
	{
	if (!initialized) { return; }
	MusicContainer *m=GetMusic(0);
	if (m) { m->Think(elapsed*PlayingMusic()); }
	}

void SoundServer::SetMusicVolume(int perc)
	{
	if (!initialized) { return; }
	float r=(float)perc/100.0*MIX_MAX_VOLUME;
	Mix_VolumeMusic((int)r);
	}

void SoundServer::SetVolume(int perc)
	{
	if (!initialized) { return; }
	float r=(float)perc/100.0*MIX_MAX_VOLUME;
	Mix_Volume(-1,(int)r);
	}

void SoundServer::SetPosition(int channel,float norm_dist,float angl)
	{
	Mix_SetPosition(channel,(Sint16)(angl/M_PI*180),(Uint8)(norm_dist*255));
	}



void SoundServer::Reload()
	{
	int playmusic=PlayingMusic();
	Mix_HaltChannel(-1);
	Mix_HaltMusic();

	for (unsigned int i=0; i<playchannels.size(); i++) { playchannels[i]=-1; }

//Reload the sounds
	for (unsigned int i=0; i<sounds.size(); i++) { sounds[i]->Reload(); }
	for (unsigned int i=0; i<musics.size(); i++) { musics[i]->Reload(); }


	if (playmusic) { PlayMusic(0); }
	}

void SoundServer::SetMusicLoop(double from,double to)
	{
	if (!initialized) { return ; }
	MusicContainer *m=GetMusic(0);
	if (!m) { return ; }
	m->SetLoop(from,to);
	}


/////////////LUA IMPLEMENT////////////////



int SOUND_SetPostEffect(lua_State *state)
	{
	double p3=LUA_GET_DOUBLE(state);
	double p2=LUA_GET_DOUBLE(state);
	double p1=LUA_GET_DOUBLE(state);
	std::string pe=LUA_GET_STRING(state) ;
	g_Sounds()->SetPostMix(pe,p1,p2,p3);
	return 0;
	}

int SOUND_Init(lua_State *state)
	{
	int buffsize=LUA_GET_INT(state);
	int stereo=LUA_GET_INT(state);
	int bitsize=LUA_GET_INT(state);
	int freq=LUA_GET_INT(state);
	int res=g_Sounds()->InitSound(freq,bitsize,stereo,buffsize);
	LUA_SET_NUMBER(state, res);
	return 1;
	}

int SOUND_AllocateChannels(lua_State *state)
	{
	int nchan=LUA_GET_INT(state);
	int res=g_Sounds()->SetNumChannels(nchan); //Negative value => Get current numchans
	LUA_SET_NUMBER(state, res);
	return 1;
	}

int SOUND_PlayedByChannel(lua_State *state)
	{
	int nchan=LUA_GET_INT(state);
	int res=g_Sounds()->SoundPlayedByChannel(nchan); //Negative value => Get current numchans
	LUA_SET_NUMBER(state, res);
	return 1;
	}


int SOUND_Load(lua_State *state)
	{
	std::string fname=LUA_GET_STRING(state);
	CuboFile * finfo=GetFileName(fname,FILE_SOUND,".wav");
	if (!finfo) {coutlog("Sound "+fname+ ".wav not found!",2); LUA_SET_NUMBER(state, -1); return 1;}
	int res=g_Sounds()->LoadSound(finfo);
	delete finfo;
	LUA_SET_NUMBER(state, res);
	return 1;
	}

int SOUND_LoadMusic(lua_State *state)
	{
	std::string fname=LUA_GET_STRING(state);
	CuboFile *finfo=GetFileName(fname,FILE_MUSIC,".mp3");
	if (!finfo) {coutlog("Music "+fname+ ".mp3 not found!",2); LUA_SET_NUMBER(state, -1); return 1;}
//if (g_VerboseMode()) coutlog("Loading Music : "+fname);
	int res=g_Sounds()->LoadMusic(finfo);
	delete finfo;
	LUA_SET_NUMBER(state, res);

	finfo=GetFileName(fname,FILE_MUSIC,".mdef");
	if (finfo) {
			//Execute the Music script
			LuaAccess acc;
			acc.Include(g_CuboLib());
			acc.LoadFile(finfo,-1,-1);
			}

	return 1;
	}

int SOUND_Play(lua_State *state)
	{
	int channel=LUA_GET_INT(state);
	int snd=LUA_GET_INT(state);
	int res=g_Sounds()->PlaySound(snd,channel);
	LUA_SET_NUMBER(state, res);
	return 1;
	}

int SOUND_PlayLooped(lua_State *state)
	{
	int loops=LUA_GET_INT(state);
	int channel=LUA_GET_INT(state);
	int snd=LUA_GET_INT(state);
	int res=g_Sounds()->PlaySoundLooped(snd,channel,loops);
	LUA_SET_NUMBER(state, res);
	return 1;
	}

int SOUND_StopChannel(lua_State *state)
	{
	int channel=LUA_GET_INT(state);
	g_Sounds()->StopChannel(channel);
	return 0;
	}

int SOUND_SetMusicLoop(lua_State *state)
	{
	double to=LUA_GET_DOUBLE(state);
	double from=LUA_GET_DOUBLE(state);
	g_Sounds()->SetMusicLoop(from,to);
	return 0;
	}


int SOUND_MusicJumpTo(lua_State *state)
	{
	double to=LUA_GET_DOUBLE(state);
	g_Sounds()->MusicJumpTo(to);
	return 0;
	}


int SOUND_PlayMusic(lua_State *state)
	{
	int mus=LUA_GET_INT(state);
	int res=g_Sounds()->PlayMusic(mus);
	LUA_SET_NUMBER(state, res);
	return 1;
	}

int SOUND_PlayingMusic(lua_State *state)
	{
	int r= g_Sounds()->PlayingMusic();
	LUA_SET_NUMBER(state, r);
	return 1;
	}

int SOUND_RewindMusic(lua_State *state)
	{
	g_Sounds()->RewindMusic();
	return 0;
	}

int SOUND_SetMusicVolume(lua_State *state)
	{
	int perc=LUA_GET_INT(state);
	g_Sounds()->SetMusicVolume(perc);
	return 0;
	}

int SOUND_SetVolume(lua_State *state)
	{
	int perc=LUA_GET_INT(state);
	g_Sounds()->SetVolume(perc);
	return 0;
	}

int SOUND_Set3dFromCam(lua_State *state)
	{
	float maxdist=LUA_GET_DOUBLE(state);
	Vector3d p=Vector3FromStack(state);
	int channel=LUA_GET_INT(state);
	Vector3d rel=g_Game()->GetCam()->getPos();
	rel=p-rel;
	float dist=rel.length();
	if (dist>maxdist) { dist=1; }
	else { dist=dist/maxdist; }
	//Now get the angle. First project the Vector into the Cams UP-Plane
	Vector3d proj;
	Vector3d up=g_Game()->GetCam()->getUp();
	float pd=-(up*rel);
	proj=up*pd;
	proj=proj+rel;
	// Now proj is in the cams plane
	float ang=0;
	if (proj.sqrlength()<0.01) {
			ang=0;
			}
	else {
			Vector3d dir=g_Game()->GetCam()->getDir();
			Vector3d side=g_Game()->GetCam()->getSide();
			float x=proj*side;
			float y=proj*dir;
			ang=atan2(x,y);
			}
	g_Sounds()->SetPosition(channel,dist,ang);

	return 0;
	}

void LUA_SOUND_RegisterLib()
	{
	g_CuboLib()->AddFunc("SOUND_Init",SOUND_Init);
	g_CuboLib()->AddFunc("SOUND_SetPostEffect",SOUND_SetPostEffect);
	g_CuboLib()->AddFunc("SOUND_AllocateChannels",SOUND_AllocateChannels);
	g_CuboLib()->AddFunc("SOUND_PlayedByChannel",SOUND_PlayedByChannel);
	g_CuboLib()->AddFunc("SOUND_Load",SOUND_Load);
	g_CuboLib()->AddFunc("SOUND_LoadMusic",SOUND_LoadMusic);
	g_CuboLib()->AddFunc("SOUND_SetMusicLoop",SOUND_SetMusicLoop);
	g_CuboLib()->AddFunc("SOUND_Play",SOUND_Play);
	g_CuboLib()->AddFunc("SOUND_PlayMusic",SOUND_PlayMusic);
	g_CuboLib()->AddFunc("SOUND_MusicJumpTo",SOUND_MusicJumpTo);
	g_CuboLib()->AddFunc("SOUND_PlayingMusic",SOUND_PlayingMusic);
	g_CuboLib()->AddFunc("SOUND_RewindMusic",SOUND_RewindMusic);
	g_CuboLib()->AddFunc("SOUND_SetMusicVolume",SOUND_SetMusicVolume);
	g_CuboLib()->AddFunc("SOUND_SetVolume",SOUND_SetVolume);
	g_CuboLib()->AddFunc("SOUND_PlayLooped",SOUND_PlayLooped);
	g_CuboLib()->AddFunc("SOUND_StopChannel",SOUND_StopChannel);
	g_CuboLib()->AddFunc("SOUND_Set3dFromCam",SOUND_Set3dFromCam);
	}
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
