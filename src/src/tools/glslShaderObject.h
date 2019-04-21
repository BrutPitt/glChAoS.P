////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2018-2019 Michele Morrone
//  All rights reserved.
//
//  mailto:me@michelemorrone.eu
//  mailto:brutpitt@gmail.com
//  
//  https://github.com/BrutPitt
//
//  https://michelemorrone.eu
//  https://BrutPitt.com
//
//  This software is distributed under the terms of the BSD 2-Clause license
//  
////////////////////////////////////////////////////////////////////////////////
#pragma once

#ifdef __EMSCRIPTEN__
    #include <GLES3/gl3.h>
#else
    #include <glad/glad.h>
#endif

#include <iostream> 
#include <fstream>
#include <cassert>
using namespace std;

//  BaseClass
/////////////////////////////////////////////////
class ShaderObject
{
	protected:
		ShaderObject();

	public:
		virtual ~ShaderObject();

        void Load(const char *name);
        //void Load(int numShaders, ...) { Load(NULL, numShaders, ...); }
        void Load(const char *defines, int numShaders, ...);
        void Compile(const GLchar *code);

		GLuint& getShader();

	protected:
		void getFileContents(const char* fileName, string &s);

		GLuint  shader;
};

//  Fragment 
/////////////////////////////////////////////////
class FragmentShader : public ShaderObject
{
	public:
		FragmentShader() : ShaderObject() { shader = glCreateShader(GL_FRAGMENT_SHADER);  }
} ;

//  Vertex
/////////////////////////////////////////////////
class VertexShader : public ShaderObject
{
	public:
		VertexShader() : ShaderObject() { shader = glCreateShader(GL_VERTEX_SHADER);  }
};

#ifndef __EMSCRIPTEN__
//  geometry
/////////////////////////////////////////////////
class GeometryShader : public ShaderObject
{
	public:
		GeometryShader() : ShaderObject() { shader = glCreateShader(GL_GEOMETRY_SHADER);  }
};
#endif

#ifndef NDEBUG
void CheckErrorsGL( const char* location = NULL,
		                std::ostream& ostr = std::cerr );
#else
inline void CheckErrorsGL( const char* location = NULL,
			                     std::ostream& ostr = std::cerr )
{}
#endif

#if !defined(NDEBUG)
#define CHECK_GL_ERROR() {\
    CheckGLError(__FILE__, __LINE__);\
    GLenum err = glGetError();\
    if(err!=GL_NO_ERROR) cout << "OpenGL Error:" << err << endl;\
}
#else
#define CHECK_GL_ERROR()
#endif

#define FORCE_CHECK_GL_ERROR() {\
    CheckGLError(__FILE__, __LINE__);\
    GLenum err = glGetError();\
    if(err!=GL_NO_ERROR) cout << "OpenGL Error:" << err << endl;\
}


#if !defined(NDEBUG)
#define CHECK_SHADER_ERROR(hProg) CheckShaderError(hProg)
#else
#define CHECK_SHADER_ERROR(hProg) 
#endif



void getCompilerLog(GLuint handle, GLint blen, bool isShader);

int CheckGLError(const char *file, int line);
void CheckShaderError(GLuint hProg);
