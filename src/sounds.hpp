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

class TSoundContainer
	{
	protected:
		Mix_Chunk *sound;
		std::string fname;
	public:
		TSoundContainer() : sound(NULL), fname("") {}
		~TSoundContainer();
		Mix_Chunk *GetSound() {return sound;}
		int Load(TCuboFile *finfo);
		void Reload();
		std::string Filename() {return fname;}
	};

class TMusicContainer
	{
	protected:
		Mix_Music *music;
		std::string fname;
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
		std::string Filename() {return fname;}
	};

class TSoundServer
	{
	protected:
		static std::string currenteffect;
		static std::vector<int> effectbuffer;
		static int effectpos;
		static int bitsps;
		static double postmixparam[3];
		int initialized;
		std::vector<TSoundContainer*> sounds;
		std::vector<TMusicContainer*> musics;
		std::vector<int> playchannels;
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
		void SetPostMix(std::string what,double p1,double p2,double p3);
		void Reload();
		void Think(double elapsed);
	};

extern TSoundServer *g_Sounds();

extern void LUA_SOUND_RegisterLib();

#endif
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
