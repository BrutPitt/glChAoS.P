////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2018 Michele Morrone
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
//  This software is distributed under the terms of the BSD 2-Clause license:
//  
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//      * Redistributions of source code must retain the above copyright
//        notice, this list of conditions and the following disclaimer.
//      * Redistributions in binary form must reproduce the above copyright
//        notice, this list of conditions and the following disclaimer in the
//        documentation and/or other materials provided with the distribution.
//   
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
//  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
//  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
