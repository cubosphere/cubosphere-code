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

#define GL_GLEXT_PROTOTYPES 1

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <GL/glew.h>

#include <GL/glew.h>
#include <SDL.h>

#include "shaderutil.hpp"
#include <vector>
#include <iostream>
#include "globals.hpp"
#include "game.hpp"

GLboolean ExtensionSupported(std::string name) {
	char *extensions = (char *)glGetString(GL_EXTENSIONS);
// Check your extension
	if (strstr(extensions, name.c_str())) {
			return true;
			}
	else {
			return false;
			}
	}




GLint BaseShader::GetUniformLocation(std::string name) {
	for (unsigned int i=0; i<ulocs.size(); i++) {if(ulocs[i].name==name) return ulocs[i].loc;}

	GLint loc   = glGetUniformLocation(programref, name.c_str());
	ulocs.push_back(ShaderUniformLocation());
	ulocs.back().name=name;
	ulocs.back().loc=loc;
	return loc;
	}

GLint BaseShader::GetAttributeLocation(std::string name) {
	for (unsigned int i=0; i<alocs.size(); i++) {if(alocs[i].name==name) return alocs[i].loc;}

	GLint loc   = glGetAttribLocation(programref, name.c_str());
	alocs.push_back(ShaderUniformLocation());
	alocs.back().name=name;
	alocs.back().loc=loc;
	return loc;
	}


void BaseShader::Load(ShaderServer *ss,std::string fname) {
	this->filename=fname;
	auto temp1=GetFileName(fname,FILE_SHADER,".vert");
	auto temp2=GetFileName(fname,FILE_SHADER,".frag");

	if (!temp1) {coutlog("Shader "+fname+ ".vert not found!",1); return;}
	if (!temp2) {coutlog("Shader "+fname+ ".frag not found!",1); return;}

	vertexref=ss->CompileShaderCuboFile(GL_VERTEX_SHADER,temp1);
	fragmentref=ss->CompileShaderCuboFile(GL_FRAGMENT_SHADER,temp2);
	programref=ss->LinkShaders(vertexref,fragmentref);
	assert(glIsProgram(programref));
	assert(glIsShader(fragmentref));
	assert(glIsShader(vertexref));
	}


void BaseShader::Activate() {
	glUseProgram(programref);
	}

BaseShader::~BaseShader() {
	glDeleteProgram(programref);
	glDeleteShader(fragmentref);
	glDeleteShader(vertexref);
	}


void BaseShader::Deactivate() {
	glUseProgram(0);
	}

GLuint
ShaderServer::CompileShaderText(GLenum shaderType, const char *text) {
	GLuint shader;
	GLint stat;

	shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, (const GLchar **) &text, NULL);

	glCompileShader(shader);


	glGetShaderiv(shader, GL_COMPILE_STATUS, &stat);
	if (!stat) {
			GLchar log[1000];
			GLsizei len;
			glGetShaderInfoLog(shader, 1000, &len, log);
			std::string lg=log;
			coutlog("Error: problem compiling shader: "+ lg+"\n",1);
			// exit(1);
			}
	return shader;
	}


/**
 * Read a shader from a file.
 */
GLuint
ShaderServer::CompileShaderFile(GLenum shaderType, const char *filename) {
	const int max = 100*1000;
	int n;
	char *buffer = (char*) malloc(max);
	GLuint shader;
	FILE *f;
	f = fopen(filename, "r");
	if (!f) {
			std::string lg=filename;
			coutlog("Error: Unable to open shader file: "+ lg+"\n",1);
			return 0;
			}

	n = fread(buffer, 1, max, f);

	/*printf("read %d bytes from shader file %s\n", n, filename);*/
	if (n > 0) {
			buffer[n] = 0;
			shader = CompileShaderText(shaderType, buffer);
			}
	else {
			return 0;
			}

	fclose(f);
	free(buffer);

	return shader;
	}


GLuint ShaderServer::CompileShaderCuboFile(GLenum shaderType, std::unique_ptr<CuboFile>& finfo) {
	if (finfo->IsHDDFile()) {return CompileShaderFile(shaderType,finfo->GetHDDName().c_str());}
	else {
			char * buffer=(char*)finfo->GetData();  buffer[finfo->GetSize()]='\0';
			return  CompileShaderText(shaderType, buffer);
			}
	}

GLuint
ShaderServer::LinkShaders(GLuint vertShader, GLuint fragShader) {
	GLuint program = glCreateProgram();
	assert(vertShader || fragShader);

	if (fragShader) {
			glAttachShader(program, fragShader);
			}
	if (vertShader) {
			glAttachShader(program, vertShader);
			}

	glLinkProgram(program);


		{
		GLint stat;
		glGetProgramiv(program, GL_LINK_STATUS, &stat);
		if (!stat) {
				GLchar log[1000];
				GLsizei len;
				glGetProgramInfoLog(program, 1000, &len, log);
				std::string lg=log;
				coutlog("Shader link error: "+ lg+"\n",1);
				return 0;
				}
		}

	return program;
	}


GLboolean
ShaderServer::ValidateShaderProgram(GLuint program) {
	GLint stat;
	glValidateProgram(program);
	glGetProgramiv(program, GL_VALIDATE_STATUS, &stat);

	if (!stat) {
			GLchar log[1000];
			GLsizei len;
			glGetProgramInfoLog(program, 1000, &len, log);
			std::string lg=log;
			coutlog("Shader program validation error:"+ lg+"\n");
			return 0;
			}

	return (GLboolean) stat;
	}





BaseShader *ShaderServer::GetShaderPtr(std::string name) {
	for (unsigned int i=0; i<shaderlist.size(); i++) {
			if (shaderlist[i]->GetName()==name) { return shaderlist[i]; }
			}
	return NULL;
	}

int ShaderServer::GetShader(std::string name) {
	for (unsigned int i=0; i<shaderlist.size(); i++) {
			if (shaderlist[i]->GetName()==name) { return i; }
			}
	return -1;
	}


bool ShaderServer::FreeShaders() {
	for (unsigned int i=0; i<shaderlist.size(); i++) {
			if (shaderlist[i]) { delete shaderlist[i]; }
			}
	shaderlist.resize(0);
	return true;
	}

int ShaderServer::AddShader(std::string name) {
	int f=GetShader(name);
	if (f>-1) { return f; }
	if (g_VerboseMode()) { coutlog("Loading Shader: "+name); }
	BaseShader *sh=new BaseShader;
	sh->Load(this,name);
	shaderlist.push_back(sh);
	return shaderlist.size()-1;
	}


bool ShaderServer::Activate(int index) {
	if (index==momshader) { return true; }
	momshader=index;
	shaderlist[momshader]->Activate();
	return true;
	}

bool ShaderServer::Deactivate() {
	if (momshader>=0) {
			shaderlist[momshader]->Deactivate();   g_Game()->GetTextures()->DeactivateStage(0);
			}
	momshader=-1;
	return true;
	}

void ShaderServer::SetInt(std::string ref,int i) {
	if (momshader<0) { return; }
	glUniform1i(shaderlist[momshader]->GetUniformLocation(ref), i);
	}

void ShaderServer::SetFloat(std::string ref,float f) {

	if (momshader<0) { return; }
	glUniform1f(shaderlist[momshader]->GetUniformLocation(ref), f);
	}

void ShaderServer::SetVector3(std::string ref,Vector3d v) {
	if (momshader<0) { return; }
	glUniform3f(shaderlist[momshader]->GetUniformLocation(ref), v.x,v.y,v.z);
	}

void ShaderServer::SetVector4(std::string ref,Vector4d v) {
	if (momshader<0) { return; }
	glUniform4f(shaderlist[momshader]->GetUniformLocation(ref), v.x,v.y,v.z,v.w);
	}

GLint ShaderServer::GetAttributeLocation(std::string name) {
	return shaderlist[momshader]->GetAttributeLocation(name);
	}


void ShaderServer::clear() {

	for (unsigned int i=0; i<shaderlist.size(); i++) {
			if (shaderlist[i]) { delete shaderlist[i]; }
			shaderlist[i]=NULL;
			}
	shaderlist.resize(0);

	}



/////////////LUA-IMPLEMENT///////////////////////


int SHADER_Load(lua_State *state) {
	//string name = lua_tostring(state, -1);
	//lua_pop(state,1);
	std::string name = LUA_GET_STRING(state);

	//string Texturename=GetFileName(name,FILE_TEXTURE)+".jpg";
	int r=g_Game()->GetShaders()->AddShader(name);
	LUA_SET_NUMBER(state, r);

	return 1;
	}


int SHADER_Activate(lua_State *state) {
	int index= LUA_GET_INT(state);
	g_Game()->GetShaders()->Activate(index);
	return 0;
	}

int SHADER_Deactivate(lua_State *state) {
	g_Game()->GetShaders()->Deactivate();
	return 0;
	}


int SHADER_SetInt(lua_State *state) {
	int i=LUA_GET_INT(state);
	std::string s=LUA_GET_STRING(state);

	g_Game()->GetShaders()->SetInt(s,i);
	return 0;
	}

int SHADER_SetFloat(lua_State *state) {
	float f=LUA_GET_DOUBLE(state);
	std::string s=LUA_GET_STRING(state);

	g_Game()->GetShaders()->SetFloat(s,f);
	return 0;
	}



int SHADER_SetVector3(lua_State *state) {

//stackDump(state); return 0;
	Vector3d v=Vector3FromStack(state);
	std::string s;
	s=LUA_GET_STRING(state);

	g_Game()->GetShaders()->SetVector3(s,v);
	return 0;
	}

int SHADER_SetVector4(lua_State *state) {

//stackDump(state); return 0;
	Vector4d v=Vector4FromStack(state);
	std::string s;
	s=LUA_GET_STRING(state);

	g_Game()->GetShaders()->SetVector4(s,v);
	return 0;
	}



void LUA_SHADER_RegisterLib() {
	g_CuboLib()->AddFunc("SHADER_Load",SHADER_Load);
	g_CuboLib()->AddFunc("SHADER_Activate",SHADER_Activate);
	g_CuboLib()->AddFunc("SHADER_Deactivate",SHADER_Deactivate);
	g_CuboLib()->AddFunc("SHADER_SetInt",SHADER_SetInt);
	g_CuboLib()->AddFunc("SHADER_SetFloat",SHADER_SetFloat);
	g_CuboLib()->AddFunc("SHADER_SetVector3",SHADER_SetVector3);
	g_CuboLib()->AddFunc("SHADER_SetVector4",SHADER_SetVector4);
	}
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4; 
