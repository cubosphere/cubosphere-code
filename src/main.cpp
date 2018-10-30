/**
Copyright (C) 2010 Chriddo

This program is free software;
you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program;
if not, see <http://www.gnu.org/licenses/>.
**/




#include <string.h>


#ifdef WIN32
#ifdef MINGW_CROSS_COMPILE
#include <GL/glew.h>
#else
#include <GL\glew.h>
#endif
#include <windows.h>
#include <SDL.h>

#ifdef VCPP

#else
#include <libgen.h>
#endif

#else
#include <GL/glew.h>
#include <SDL/SDL.h>
#include <libgen.h>
#endif


#include <stdlib.h>

#include "shaderutil.hpp"

#include "cuboboxes.hpp"
#include "textures.hpp"
#include "mouse.hpp"
#include "keyboard.hpp"
#include "vectors.hpp"
#include "c3dobjects.hpp"
#include "cuboutils.hpp"
#include "cubolevels.hpp"
#include "skyboxes.hpp"
#include "cuboactors.hpp"
#include "events.hpp"
#include "globals.hpp"
#include "game.hpp"
#include "sounds.hpp"
#include "luautils.hpp"

#ifdef WIN32
#include <cstdio>
#include <iostream>
#include <fcntl.h>
#include <io.h>
#endif

#ifndef WIN32
#include <sys/stat.h>
#endif

#include <vector>

using namespace std;


TCuboGame game;


void MakeConsole()
	{
#ifdef WIN32

	freopen("CON", "wb", stdout);
	freopen("CON", "wb", stderr);

#endif
	}

void KillConsole()
	{
#ifdef WIN32

	fflush(stdout);
	fflush(stderr);
	fclose(stdout);
	fclose(stderr);

#endif
	}



#ifdef __cplusplus
extern "C" {
#endif

int main(int argc, char *argv[])
	{
	SetCmdLine(argc,argv);
	char *dc;
	dc=strdup(argv[0]);
	string dir=dirname(dc);
	free(dc);
	SetBaseDir(dir);


//Patch from Vincent Petry - Thanks!
#ifndef WIN32
	string configDir( getenv("HOME") );
	string configSubDir;
	configDir.append("/.cubosphere");
	configSubDir = configDir + "/levels";
	string SaveDir = configDir + "/saves";
	// check whether config folder exists
	struct stat s;
	if (stat(configSubDir.c_str(), &s) != 0)
			{
			// if not, create it
			configSubDir = configDir;
			mkdir(configSubDir.c_str(), S_IRWXU);
			configSubDir += "/levels";
			mkdir(configSubDir.c_str(), S_IRWXU);
			printf("Creating dir: %s\n", configSubDir.c_str());
			}
	if (stat(SaveDir.c_str(), &s) != 0)
			{
			mkdir(SaveDir.c_str(), S_IRWXU);
			printf("Creating dir: %s\n", SaveDir.c_str());
			}
	SetProfileDir(configDir);
	printf("Using config dir: %s\n", configDir.c_str());
#else
	SetProfileDir(dir+PlattformFilename("/user"));
#endif

#ifdef DATADIR
	SetDataDir(DATADIR);
#else
	SetDataDir(dir+PlattformFilename("/data"));
#endif

	MakeConsole();

	cout << "STARTING in basedir: " << dir << endl << "SDL_Init returns: "<<
			SDL_Init( SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) << endl;

	//SDL_Init( SDL_INIT_EVERYTHING) << endl;

	SDL_WM_SetCaption("Cubosphere","Cubosphere");

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);

//SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
//SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);





	atexit(closelog);



	game.Init();
	game.Start();
	game.FreeMedia();

	if (g_VerboseMode()) coutlog("Ending game engine");
	game.End();

	if (g_VerboseMode()) coutlog("Stopping sound");
	g_Sounds()->KillSound();

	if (g_VerboseMode()) coutlog("SDL quit");
	SDL_Quit();

	if (g_VerboseMode()) coutlog("Umount console");
	KillConsole();

//   return 0;

	if (g_VerboseMode()) coutlog("-> Good bye!");
	return EXIT_SUCCESS;
	}


#ifdef __cplusplus
	}
#endif
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
