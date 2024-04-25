//------------------------------------------------------------------------------
//  Copyright (c) 2018-2024 Michele Morrone
//  All rights reserved.
//
//  https://michelemorrone.eu - https://brutpitt.com
//
//  X: https://x.com/BrutPitt - GitHub: https://github.com/BrutPitt
//
//  direct mail: brutpitt(at)gmail.com - me(at)michelemorrone.eu
//
//  This software is distributed under the terms of the BSD 2-Clause license
//------------------------------------------------------------------------------
#include "glslShaderObject.h"

#define ANSI
#ifdef ANSI             // ANSI compatible version
#include <stdarg.h>
#include <vector>
#else                   // UNIX compatible version
#include <varargs.h>
#endif

#define MAX_ERRORS_TO_SHOW 25

#if !defined(NDEBUG)
    #if !defined(GLAPP_WEBGL)
        void GLAPIENTRY openglCallbackFunction(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
                                               const GLchar* message, const void* userParam)
        {
            static int count = 0;
            if(type!=GL_DEBUG_TYPE_OTHER /*&& count<MAX_ERRORS_TO_SHOW*/) {
                cout << endl << "----- debug message -----" << endl;
                cout << "message: "<< message << endl;
                cout << "type: ";
                switch (type) {
                case GL_DEBUG_TYPE_ERROR:
                    cout << "ERROR";
                    break;
                case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
                    cout << "DEPRECATED_BEHAVIOR";
                    break;
                case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
                    cout << "UNDEFINED_BEHAVIOR";
                    break;
                case GL_DEBUG_TYPE_PORTABILITY:
                    cout << "PORTABILITY";
                    break;
                case GL_DEBUG_TYPE_PERFORMANCE:
                    cout << "PERFORMANCE";
                    break;
                case GL_DEBUG_TYPE_OTHER:
                    cout << "OTHER";
                    break;
                }
                cout << endl;

                cout << "id: " << id << endl;
                cout << "severity: ";
                switch (severity){
                case GL_DEBUG_SEVERITY_LOW:
                    cout << "LOW";
                    break;
                case GL_DEBUG_SEVERITY_MEDIUM:
                    cout << "MEDIUM";
                    break;
                case GL_DEBUG_SEVERITY_HIGH:
                    cout << "HIGH";
                    break;
                default :
                    cout << severity;
                }
                count++;
                cout << endl;
            }
        }

        void GetFirstNMessages(GLuint numMsgs)
        {
            GLint maxMsgLen = 0;
            glGetIntegerv(GL_MAX_DEBUG_MESSAGE_LENGTH, &maxMsgLen);

            std::vector<GLchar> msgData(numMsgs * maxMsgLen);
            std::vector<GLenum> sources(numMsgs);
            std::vector<GLenum> types(numMsgs);
            std::vector<GLenum> severities(numMsgs);
            std::vector<GLuint> ids(numMsgs);
            std::vector<GLsizei> lengths(numMsgs);

            GLuint numFound = glGetDebugMessageLog(numMsgs, msgData.size(), &sources[0], &types[0], &ids[0], &severities[0], &lengths[0], &msgData[0]);

            sources.resize(numFound);
            types.resize(numFound);
            severities.resize(numFound);
            ids.resize(numFound);
            lengths.resize(numFound);

            std::vector<std::string> messages;
            messages.reserve(numFound);

            std::vector<GLchar>::iterator currPos = msgData.begin();
            for(size_t msg = 0; msg < lengths.size(); ++msg)
            {
               messages.push_back(std::string(currPos, currPos + lengths[msg] - 1));
               currPos = currPos + lengths[msg];
            }

            for(int i=0; i<numMsgs; i++)
                cout << "num: "<< numFound << " - src: " << sources[i] << " - type: " << types[i] << " - id: " << ids[i] << " - sev: " << severities[i] << endl << " ***** " << messages[i] << endl;
        }
    #endif
// GL ERROR CHECK
int CheckGLError(const char *file, int line)
{
    static int count = 0;

    GLenum glErr;
    int    retCode = 0;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR)  {

        if(count++<MAX_ERRORS_TO_SHOW)
            cout << "GL error (" << (glErr) << ") " << " in File " << file << " at line: " << line << endl;
//        GetFirstNMessages(50);

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
#endif
/*
void checkDeletedShader(GLuint shader)
{
    GLint deleted;
    glGetShaderiv( shader, GL_DELETE_STATUS, &deleted );

    cout << "Shader#: " << shader << (deleted == GL_TRUE ? " flagged for delete" : " still active") << endl;
}
*/
void checkShader(GLuint shader)
{
    GLint compiled;
    glGetShaderiv( shader, GL_COMPILE_STATUS, &compiled );

#ifndef NDEBUG
    if(compiled == GL_FALSE) {
        GLint len;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH , &len);

        getCompilerLog(shader, len, true);
        exit(-1);
    }
#endif
}

void checkProgram(GLuint program)
{
    GLint linked;
    glGetProgramiv( program, GL_LINK_STATUS, &linked );	
    //CHECK_GL_ERROR(); 

#ifndef NDEBUG
    if(linked == GL_FALSE) {
        GLint len;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH , &len);

        getCompilerLog(program, len, false);
        exit(-1);
    }
#endif
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

//#define GLAPP_PRINT_SHADER_SOURCE
#if !defined(NDEBUG) && defined(GLAPP_PRINT_SHADER_SOURCE)
#define GLAPP_DEBUG_PRINT_SEPARATOR "****************************************************\n"
    cout << endl << GLAPP_DEBUG_PRINT_SEPARATOR;
    cout << str ;
    cout << endl << GLAPP_DEBUG_PRINT_SEPARATOR;
#endif
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

#ifdef __EMSCRIPTEN__
/*
  To print HLSL code in ANGLE ==> subst _glCompileShader function in JS
  function _glCompileShader(shader) {
      GLctx.compileShader(GL.shaders[shader]);
      var hlsl = GLctx.getExtension("WEBGL_debug_shaders").getTranslatedShaderSource(GL.shaders[shader]);
      console.log(hlsl);
    }
*/
#endif
    
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
    cout << fileName << "NOT OPENed..."<< endl;
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