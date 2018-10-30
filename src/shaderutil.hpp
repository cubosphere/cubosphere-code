/**
Copyright (C) 2010 Chriddo

This program is free software;
you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program;
if not, see <http://www.gnu.org/licenses/>.
**/


/**
 * This part has been taken from (with some changes):
 ####################################################
 * Utilities for OpenGL shading language
 *
 * Brian Paul
 * 9 April 2008
 ####################################################
 Thanks!
 */


#ifndef SHADER_UTIL_H
#define SHADER_UTIL_H


#include <string>
#include <vector>
#include "vectors.hpp"
#include "filesystem.hpp"


#define GL_GLEXT_PROTOTYPES 1

#ifdef WIN32

#ifdef MINGW_CROSS_COMPILE
#include <GL/glew.h>
#else
#include <GL\glew.h>
#endif
// #include <GL\gl.h>
#include <windows.h>
#include <SDL.h>

#else

#include <GL/glew.h>
#include <SDL/SDL.h>

#endif

class TShaderUniformLocation
	{
	public:
		std::string name;
		GLint loc;
	};


class TShaderServer;

class TBaseShader
	{
	protected:
		std::string filename; //Holds the base name
		GLuint vertexref,fragmentref,programref;
		std::vector<TShaderUniformLocation> ulocs;
		std::vector<TShaderUniformLocation> alocs;
	public:
		GLint GetUniformLocation(std::string s);
		GLint GetAttributeLocation(std::string s);
		std::string GetName() {return filename;}
		void Load(TShaderServer* ss, std::string fname);
		void Activate();
		void Deactivate();
		~TBaseShader();
		GLuint GetProgramRef() {return programref;}
	};


class TShaderServer
	{
	protected:
		std::vector<TBaseShader*> shaderlist;
		int momshader;
	public:
		TShaderServer() : momshader(-1) {};
		// GLboolean ShadersSupported(void);
		GLuint CompileShaderText(GLenum shaderType, const char *text);
		GLuint CompileShaderFile(GLenum shaderType, const char *filename);
		GLuint CompileShaderCuboFile(GLenum shaderType, TCuboFile *finfo);
		GLuint LinkShaders(GLuint vertShader, GLuint fragShader);
		GLboolean ValidateShaderProgram(GLuint program);
//   void SetUniformValues(GLuint program, struct uniform_info uniforms[]);
//  GLuint GetUniforms(GLuint program, struct uniform_info uniforms[]);
		// void PrintUniforms(const struct uniform_info uniforms[]);
		//GLuint GetAttribs(GLuint program, struct attrib_info attribs[]);
		//void PrintAttribs(const struct attrib_info attribs[]);
		bool InitShaders(std::string dirname);
		int GetShader(std::string name);
		TBaseShader *GetShaderPtr(std::string name);
		int AddShader(std::string name);
		bool RegisterShader(TBaseShader *sh);
		bool FreeShaders();
		bool Activate(int index);
		bool Deactivate();
		void SetInt(std::string ref,int i);
		void SetFloat(std::string ref,float f);
		void SetVector3(std::string ref,T3dVector v);
		void SetVector4(std::string ref,T4dVector v);
		GLint GetAttributeLocation(std::string name);
		void clear();
	};

extern void LUA_SHADER_RegisterLib();

#endif /* SHADER_UTIL_H */
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
