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
#include "glslShaderObject.h"

#define ANSI
#ifdef ANSI             // ANSI compatible version
#include <stdarg.h>
#else                   // UNIX compatible version
#include <varargs.h>
#endif

// GL ERROR CHECK
int CheckGLError(const char *file, int line)
{
    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {

     cout << "GL Error #" << glErr << "(" << (glErr) << ") " << " in File " << file << " at line: " << line << endl;

     retCode = 1;
     glErr = glGetError();
    }
    return retCode;
}

void CheckShaderError(GLuint hProg)
{
    GLint len;
    glGetProgramiv(hProg, GL_INFO_LOG_LENGTH , &len);
    getCompilerLog(hProg, len, true);

}

void getCompilerLog(GLuint handle, GLint blen, bool isShader)
{
    GLint len;
    GLchar* compilerLog;


    if (blen > 1) {
        compilerLog = new GLchar[blen];

        if(isShader) glGetShaderInfoLog( handle, blen, &len, compilerLog );
        else         glGetProgramInfoLog( handle, blen, &len, compilerLog );

        CHECK_GL_ERROR()

        cout << compilerLog << "\n" << endl;

        delete[] compilerLog;
    }
}

void checkDeletedShader(GLuint shader)
{
    GLint deleted;
    glGetShaderiv( shader, GL_DELETE_STATUS, &deleted );

    cout << "Shader#: " << shader << (deleted == GL_TRUE ? " flagged for delete" : " still active") << endl;
}

void checkShader(GLuint shader)
{
    GLint compiled;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );

    if(compiled == GL_FALSE) {
        GLint len;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH , &len);
        
        getCompilerLog(shader, len, true);
    }
}

void checkProgram(GLuint program)
{
    GLint linked;
    glGetProgramiv( program, GL_LINK_STATUS, &linked );	
    //CHECK_GL_ERROR(); 

    if(linked == GL_FALSE) {
        GLint len;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH , &len);
        getCompilerLog(program, len, false);
    }
}

void ShaderObject::Load(const char *name)
{
    string str;

    // Load shader source code
    getFileContents(name, str);
    Compile((GLchar *) str.data());
}

void ShaderObject::Load(const char *defines, int numShaders, ...) 
{
    va_list argList;
    va_start( argList, numShaders);

    // Load shader source code

    string str;

    if(defines!=NULL) str.append(defines);

    for(int i=0; i<numShaders; i++) {
        getFileContents(va_arg(argList,const char *), str);
    }
    
    Compile((GLchar *) str.data());

    va_end(argList);
}

/////////////////////////////////////////////////
void ShaderObject::Compile(const GLchar *code)
{

    // Load source code into shaders
    glShaderSource(shaderID, 1, &code, NULL);
    CHECK_GL_ERROR()

    // Compile the shader 
    glCompileShader(shaderID);
    
    checkShader(shaderID);
}

//#define USE_STDIO_TO_LOAD_SHADERS
#ifndef USE_STDIO_TO_LOAD_SHADERS
void getFileContents(const char* fileName, string &str)
{
    std::streamoff length;
    char* buffer;

#if !defined(NDEBUG)
    cout << fileName << "..."<< endl;
#endif

    // Load the file
    ifstream input(fileName,std::ifstream::binary);

    // Check to see that the file is open
    if (!input.is_open()) {
#if !defined(NDEBUG)
    cout << fileName << "NOT OPEN..."<< endl;
#endif
        return;
    }

    // Get length of file:
    input.seekg(0, ios::end);
    length = input.tellg();
    input.seekg(0, ios::beg);

    // Allocate memory:
    buffer = new char[length+1];

    // Read data as a block:
    //input.getline(buffer, length, '\0');

    input.read(buffer, length);
    buffer[length]=0;
    str+=buffer;

    // Close the input file
    input.close();

    // Return the shader code
    delete[] buffer;
}
#else // USE STDIO
#include <stdio.h>
#include <stdlib.h>
void getFileContents(const char* fileName, string &str)
{
    int length;
    char* buffer;
    FILE* pf;

#if !defined(NDEBUG)
    cout << fileName << "..."<< endl;
#endif

    // Load the file
    pf = fopen(fileName, "rb");

    // Check to see that the file is open
    //if (pf==NULL) return(NULL);

    // Get length of file:
    fseek(pf, 0, SEEK_END);
    length = ftell(pf);
    fseek(pf, 0, SEEK_SET);

    // Allocate memory:
    buffer = new char[length+5];

    // Read data as a block:
    fread(buffer, 1, length, pf);
    buffer[length]=0;

    str+=buffer;

    // Close the input file
    fclose(pf);

    // Return the shader code
    delete buffer;
}
#endif

#undef USE_IOSTREMA_TO_LOAD_SHADERS