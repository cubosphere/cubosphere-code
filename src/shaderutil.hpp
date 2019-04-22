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

#pragma once

#include <string>
#include <vector>
#include "vectors.hpp"
#include "filesystem.hpp"


#define GL_GLEXT_PROTOTYPES 1

#include <GL/glew.hpp>
#include <SDL.h>

class ShaderUniformLocation {
	public:
		std::string name;
		GLint loc;
	};


class ShaderServer;

class BaseShader {
	protected:
		std::string filename; //Holds the base name
		GLuint vertexref,fragmentref,programref;
		std::vector<ShaderUniformLocation> ulocs;
		std::vector<ShaderUniformLocation> alocs;
	public:
		GLint GetUniformLocation(std::string s);
		GLint GetAttributeLocation(std::string s);
		std::string GetName() {return filename;}
		void Load(ShaderServer* ss, std::string fname);
		void Activate();
		void Deactivate();
		~BaseShader();
		GLuint GetProgramRef() {return programref;}
	};


class ShaderServer {
	protected:
		std::vector<BaseShader*> shaderlist;
		int momshader;
	public:
		ShaderServer() : momshader(-1) {};
		// GLboolean ShadersSupported(void);
		GLuint CompileShaderText(GLenum shaderType, const char *text);
		GLuint CompileShaderFile(GLenum shaderType, const char *filename);
		GLuint CompileShaderCuboFile(GLenum shaderType, const std::unique_ptr<CuboFile>& finfo);
		GLuint LinkShaders(GLuint vertShader, GLuint fragShader);
		GLboolean ValidateShaderProgram(GLuint program);
//   void SetUniformValues(GLuint program, struct uniform_info uniforms[]);
//  GLuint GetUniforms(GLuint program, struct uniform_info uniforms[]);
		// void PrintUniforms(const struct uniform_info uniforms[]);
		//GLuint GetAttribs(GLuint program, struct attrib_info attribs[]);
		//void PrintAttribs(const struct attrib_info attribs[]);
		bool InitShaders(std::string dirname);
		int GetShader(std::string name);
		BaseShader *GetShaderPtr(std::string name);
		int AddShader(std::string name);
		bool RegisterShader(BaseShader *sh);
		bool FreeShaders();
		bool Activate(int index);
		bool Deactivate();
		void SetInt(std::string ref,int i);
		void SetFloat(std::string ref,float f);
		void SetVector3(std::string ref,Vector3d v);
		void SetVector4(std::string ref,Vector4d v);
		GLint GetAttributeLocation(std::string name);
		void clear();
	};

extern void LUA_SHADER_RegisterLib();
