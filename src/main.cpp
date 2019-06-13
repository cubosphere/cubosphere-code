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

#include <string.h>


#ifdef WIN32
#define SDL_MAIN_HANDLED
#include <windows.h>
#endif

#include "glew.hpp"
#include <SDL.h>
#include <SDL_image.h>
#include <libgen.h>

#include <stdlib.h>

#include "globals.hpp"
#include "game.hpp"
#include "sounds.hpp"
#include "log.hpp"

#ifdef WIN32
#include <cstdio>
#include <fcntl.h>
#include <io.h>
#endif

#ifndef WIN32
#include <sys/stat.h>
#endif

#include <iostream>
#include <vector>
#include <list>

#if defined(__GNUC__) && __GNUC__ < 8
#  include <experimental/filesystem>
   namespace filesystem = std::experimental::filesystem;
#else
#  include <filesystem>
   namespace filesystem = std::filesystem;
#endif

CuboGame game;

void MakeConsole() {
#ifdef WIN32

	freopen("CON", "wb", stdout);
	freopen("CON", "wb", stderr);

#endif
	}

void KillConsole() {
#ifdef WIN32

	fflush(stdout);
	fflush(stderr);
	fclose(stdout);
	fclose(stderr);

#endif
	}

int main(int argc, char *argv[]) {
#ifdef WIN32
	SDL_SetMainReady();
#endif
	SetCmdLine(argc,argv);
	std::string dir = dirname(argv[0]);

// Patch from Vincent Petry - Thanks! (It was messy…)
#ifndef WIN32
	std::string ConfigDir(getenv("HOME"));
	ConfigDir.append("/.cubosphere");
	std::string LevelDir(ConfigDir + "/levels");
	std::string SaveDir(ConfigDir + "/saves");
	// check whether config folder exists
	struct stat s;
	const std::list<std::string> CheckDirs = {ConfigDir, LevelDir, SaveDir};
	for(auto &&dir: CheckDirs) {
			auto cdir = dir.c_str();
			if (stat(cdir, &s) != 0) {
					mkdir(cdir, S_IRWXU);
					Log::info("Init", "Creating dir: %s", dir.c_str());
					}
			}

	SetProfileDir(ConfigDir);
	Log::info("Init", "Using config directory: %s", ConfigDir.c_str());
#else
	SetProfileDir(dir+PlattformFilename("/user"));
#endif

    Log::openOutputFiles(g_ProfileDir()+"/stdout.log");

	if (filesystem::exists(filesystem::path(dir+PlattformFilename("/data/boot.lua")))) {
		SetDataDir(dir+PlattformFilename("/data"));
	}
	else if (filesystem::exists(filesystem::path(dir+PlattformFilename("/../data/boot.lua")))) {
		SetDataDir(dir+PlattformFilename("/../data"));
	}
	else if (filesystem::exists(filesystem::path(dir+PlattformFilename("/../../data/boot.lua")))) {
		SetDataDir(dir+PlattformFilename("/../../data"));
	}
	else if (filesystem::exists(filesystem::path(dir+PlattformFilename("/../../../data/boot.lua")))) {
		SetDataDir(dir+PlattformFilename("/../../../data"));
	}
#ifdef DATADIR
	else if (filesystem::exists(DATADIR)) {
		SetDataDir(DATADIR);
	}
#endif
	else {
		Log::fatal("Init", "Could not find data directory!");
	}

	MakeConsole();

	Log::info("Init", "SDL_Init returned status %i",
			SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK));
	IMG_Init(IMG_INIT_PNG);

	/*
	SDL_WM_SetCaption("Cubosphere","Cubosphere");

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

	//SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	//SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	*/

	atexit(closelog);

	game.Init();
	game.Start();
	game.FreeMedia();

	if (g_VerboseMode()) { coutlog("Ending game engine"); }
	game.End();

	if (g_VerboseMode()) { coutlog("Stopping sound"); }
	g_Sounds()->KillSound();

	if (g_VerboseMode()) { coutlog("SDL quit"); }
	SDL_Quit();

	if (g_VerboseMode()) { coutlog("Umount console"); }
	KillConsole();

//   return 0;

	if (g_VerboseMode()) { coutlog("-> Good bye!"); }
	return EXIT_SUCCESS;
	}
