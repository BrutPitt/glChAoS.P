//------------------------------------------------------------------------------
//  Copyright (c) 2018-2020 Michele Morrone
//  All rights reserved.
//
//  https://michelemorrone.eu - https://BrutPitt.com
//
//  twitter: https://twitter.com/BrutPitt - github: https://github.com/BrutPitt
//
//  mailto:brutpitt@gmail.com - mailto:me@michelemorrone.eu
//  
//  This software is distributed under the terms of the BSD 2-Clause license
//------------------------------------------------------------------------------
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
    public:
        ShaderObject() {}
        enum { unassigned, attached, wantDetach, wantDelete };

        virtual ~ShaderObject() {  deleteShader(); } 

        void Load(const char *name);
        //void Load(int numShaders, ...) { Load(NULL, numShaders, ...); }
        void Load(const char *defines, int numShaders, ...);
        void Compile(const GLchar *code);

        GLuint& getShader() { return(shaderID); }

        void detachShader(GLuint prog, bool wntDelete) {
            if(getStatus() == ShaderObject::attached) {
                glDetachShader(prog, getShader());
                statusWantDetach();
                if(wntDelete) deleteShader(); 
            }
        }

        void deleteShader() {
            if(getStatus() == ShaderObject::attached || getStatus() == ShaderObject::wantDetach) {
                glDeleteShader(getShader()); 
                statusWantDelete();
            }
        }

        void statusAttached()   { status = attached; }
        void statusWantDetach() { status = wantDetach; }
        void statusWantDelete() { status = wantDelete; }
        int getStatus() { return status; }
  
    protected:

        GLuint  shaderID=0;
        int status = unassigned;

};

//  Fragment 
/////////////////////////////////////////////////
class FragmentShader : public ShaderObject
{
    public:
        FragmentShader() : ShaderObject() { shaderID = glCreateShader(GL_FRAGMENT_SHADER);  }
};

//  Vertex
/////////////////////////////////////////////////
class VertexShader : public ShaderObject
{
    public:
        VertexShader() : ShaderObject() { shaderID = glCreateShader(GL_VERTEX_SHADER);  }
};

#ifndef __EMSCRIPTEN__
//  geometry
/////////////////////////////////////////////////
class GeometryShader : public ShaderObject
{
    public:
        GeometryShader() : ShaderObject() { shaderID = glCreateShader(GL_GEOMETRY_SHADER);  }
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
    static int count=0;\
    if(count++<5) {\
        CheckGLError(__FILE__, __LINE__);\
        GLenum err = glGetError();\
        if(err!=GL_NO_ERROR) cout << "OpenGL Error:" << err << endl;\
    }\
}
#define CHECK_GL_ERROR_MSG(X) {\
    static int count=0;\
    if(count++<5) {\
        CheckGLError(__FILE__, __LINE__);\
        GLenum err = glGetError();\
        if(err!=GL_NO_ERROR) cout << X << " - OpenGL Error:" << err << endl;\
    }\
}
#else
#define CHECK_GL_ERROR()
#define CHECK_GL_ERROR_MSG(X)
#endif

#define FORCE_CHECK_GL_ERROR() {\
    CheckGLError(__FILE__, __LINE__);\
    GLenum err = glGetError();\
    if(err!=GL_NO_ERROR) cout << "OpenGL Error:" << err << endl;\
}


#if !defined(NDEBUG)
#define CHECK_SHADER_ERROR(hProg) { CheckShaderError(hProg); }
#else
#define CHECK_SHADER_ERROR(hProg) 
#endif

void getFileContents(const char* fileName, string &str);

void checkDeletedShader(GLuint shader);
void getCompilerLog(GLuint handle, GLint blen, bool isShader);

int CheckGLError(const char *file, int line);
void CheckShaderError(GLuint hProg);
