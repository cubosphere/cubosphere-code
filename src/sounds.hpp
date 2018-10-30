#ifndef SOUNDS_H_G
#define SOUNDS_H_G

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

#else

#include <GL/glew.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>

#endif

#include <string>
#include <vector>

#include "filesystem.hpp"


using namespace std;


class TSoundContainer
	{
	protected:
		Mix_Chunk *sound;
		string fname;
	public:
		TSoundContainer() : sound(NULL), fname("") {}
		~TSoundContainer();
		Mix_Chunk *GetSound() {return sound;}
		int Load(TCuboFile *finfo);
		void Reload();
		string Filename() {return fname;}
	};

class TMusicContainer
	{
	protected:
		Mix_Music *music;
		string fname;
		bool hasloop;
		double loopfrom;
		double loopto;
		double currentpos;
	public:
		void JumpTo(double pos);
		void Rewind() {currentpos=0;}
		void Think(double elapsed);
		void SetLoop(double from,double to);
		TMusicContainer() : music(NULL), fname("") {}
		~TMusicContainer();
		int Load(TCuboFile *finfo);
		void Reload();
		Mix_Music *GetMusic() {return music;}
		string Filename() {return fname;}
	};

class TSoundServer
	{
	protected:
		static string currenteffect;
		static vector<int> effectbuffer;
		static int effectpos;
		static int bitsps;
		static double postmixparam[3];
		int initialized;
		vector<TSoundContainer*> sounds;
		vector<TMusicContainer*> musics;
		vector<int> playchannels;
		TSoundContainer *GetSound(int i);
		TMusicContainer *GetMusic(int i);
		static void EchoEffect(void *udata, Uint8 *stream, int len);
	public:
		TSoundServer() : initialized(0) {};
		int InitSound(int freq,int bits,int stereo,int buffsize);
		void KillSound();
		int LoadSound(TCuboFile *finfo);
		int LoadMusic(TCuboFile *finfo);
		int PlaySound(int index,int channel);
		int PlaySoundLooped(int index,int channel,int numloops);
		void StopChannel(int chind);
		int PlayMusic(int index);
		int PlayingMusic();
		void RewindMusic();
		void SetMusicLoop(double from,double to);
		void MusicJumpTo(double pos);
		void SetMusicVolume(int perc);
		void SetVolume(int perc);
		void ChannelFinished(int num);
		void SetPosition(int channel,float norm_dist,float angl);
		int SetNumChannels(int nchan);
		int SoundPlayedByChannel(int i);
		void SetPostMix(string what,double p1,double p2,double p3);
		void Reload();
		void Think(double elapsed);
	};

extern TSoundServer *g_Sounds();

extern void LUA_SOUND_RegisterLib();

#endif
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
