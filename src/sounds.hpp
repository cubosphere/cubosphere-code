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

#include "glew.hpp"
#include <SDL.h>
#include <SDL_mixer.h>

#include <string>
#include <vector>

#include "filesystem.hpp"

class SoundContainer {
	protected:
		Mix_Chunk *sound;
		std::string fname;
	public:
		SoundContainer() : sound(NULL), fname("") {}
		~SoundContainer();
		Mix_Chunk *GetSound() {return sound;}
		int Load(const std::unique_ptr<CuboFile>& finfo);
		void Reload();
		std::string Filename() {return fname;}
	};

class MusicContainer {
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
		MusicContainer() : music(NULL), fname("") {}
		~MusicContainer();
		int Load(const std::unique_ptr<CuboFile>& finfo);
		void Reload();
		Mix_Music *GetMusic() {return music;}
		std::string Filename() {return fname;}
	};

class SoundServer {
	protected:
		static std::string currenteffect;
		static std::vector<int> effectbuffer;
		static int effectpos;
		static int bitsps;
		static double postmixparam[3];
		int initialized;
		std::vector<SoundContainer*> sounds;
		std::vector<MusicContainer*> musics;
		std::vector<int> playchannels;
		SoundContainer *GetSound(int i);
		MusicContainer *GetMusic(int i);
		static void EchoEffect(void *udata, Uint8 *stream, int len);
	public:
		SoundServer() : initialized(0) {};
		int InitSound(int freq,int bits,int stereo,int buffsize);
		void KillSound();
		int LoadSound(const std::unique_ptr<CuboFile>& finfo);
		int LoadMusic(const std::unique_ptr<CuboFile>& finfo);
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

extern SoundServer *g_Sounds();

extern void LUA_SOUND_RegisterLib();
