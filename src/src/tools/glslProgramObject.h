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
#include "glslShaderObject.h"

#include <cassert>
using namespace std;

/**
 * A class for the Program Object for using shaders.
 *
 * This class encapsulates all the GLSL shading functionality.
 *
 * \author Michele Morrone
 * \version 1.0
 */
class ProgramObject
{
public:
	ProgramObject();
	virtual ~ProgramObject();
    void createProgram();
    void deleteProgram();

	void addShader(ShaderObject* shader);
	void removeShader(ShaderObject* shader);

	void link();

    void bindPipeline();
	void useProgram();
	static void reset();

    GLuint  getHandle() { return program; }
    GLuint  getProgram() { return program; }

#ifdef GLAPP_REQUIRE_OGL45
  void setUniform1f(GLuint loc, GLfloat v0)                                     { glProgramUniform1f(program, loc, v0); }  
  void setUniform2f(GLuint loc, GLfloat v0, GLfloat v1)                         { glProgramUniform2f(program, loc, v0, v1); }    
  void setUniform3f(GLuint loc, GLfloat v0, GLfloat v1, GLfloat v2)             { glProgramUniform3f(program, loc, v0, v1, v2); }    
  void setUniform4f(GLuint loc, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) { glProgramUniform4f(program, loc, v0, v1, v2, v3); } 

  void setUniform1i(GLuint loc, GLint v0)                                       { glProgramUniform1i(program, loc, v0); }  
  void setUniform2i(GLuint loc, GLint v0, GLint v1)                             { glProgramUniform2i(program, loc, v0, v1); }    
  void setUniform3i(GLuint loc, GLint v0, GLint v1, GLint v2)                   { glProgramUniform3i(program, loc, v0, v1, v2); }    
  void setUniform4i(GLuint loc, GLint v0, GLint v1, GLint v2, GLint v3)         { glProgramUniform4i(program, loc, v0, v1, v2, v3); }

  void setUniform1ui(GLuint loc, GLuint v0)                                     { glProgramUniform1ui(program, loc, v0); }  
  void setUniform2ui(GLuint loc, GLuint v0, GLuint v1)                          { glProgramUniform2ui(program, loc, v0, v1); }    
  void setUniform3ui(GLuint loc, GLuint v0, GLuint v1, GLuint v2)               { glProgramUniform3ui(program, loc, v0, v1, v2); }    
  void setUniform4ui(GLuint loc, GLuint v0, GLuint v1, GLuint v2, GLuint v3)    { glProgramUniform4ui(program, loc, v0, v1, v2, v3); }

  // Arrays
  void setUniform1fv(GLuint loc, GLsizei count, const GLfloat *v)               { glProgramUniform1fv(program, loc, count, v); }  
  void setUniform2fv(GLuint loc, GLsizei count, const GLfloat *v)               { glProgramUniform2fv(program, loc, count, v); }  
  void setUniform3fv(GLuint loc, GLsizei count, const GLfloat *v)               { glProgramUniform3fv(program, loc, count, v); }  
  void setUniform4fv(GLuint loc, GLsizei count, const GLfloat *v)               { glProgramUniform4fv(program, loc, count, v); }  
  
  void setUniform1iv(GLuint loc, GLsizei count, const GLint *v)                 { glProgramUniform1iv(program, loc, count, v); }  
  void setUniform2iv(GLuint loc, GLsizei count, const GLint *v)                 { glProgramUniform2iv(program, loc, count, v); }  
  void setUniform3iv(GLuint loc, GLsizei count, const GLint *v)                 { glProgramUniform3iv(program, loc, count, v); }  
  void setUniform4iv(GLuint loc, GLsizei count, const GLint *v)                 { glProgramUniform4iv(program, loc, count, v); }  
  
  void setUniform1uiv(GLuint loc, GLsizei count, const GLuint *v)               { glProgramUniform1uiv(program, loc, count, v); }  
  void setUniform2uiv(GLuint loc, GLsizei count, const GLuint *v)               { glProgramUniform2uiv(program, loc, count, v); }  
  void setUniform3uiv(GLuint loc, GLsizei count, const GLuint *v)               { glProgramUniform3uiv(program, loc, count, v); }  
  void setUniform4uiv(GLuint loc, GLsizei count, const GLuint *v)               { glProgramUniform4uiv(program, loc, count, v); }  
  
  // Matrix
  void setUniformMatrix2fv(GLuint loc, GLsizei count, GLboolean transpose, GLfloat *v) { glProgramUniformMatrix2fv(program, loc, count, transpose, v); }  
  void setUniformMatrix3fv(GLuint loc, GLsizei count, GLboolean transpose, GLfloat *v) { glProgramUniformMatrix3fv(program, loc, count, transpose, v); }  
  void setUniformMatrix4fv(GLuint loc, GLsizei count, GLboolean transpose, GLfloat *v) { glProgramUniformMatrix4fv(program, loc, count, transpose, v); }  
#else
  void setUniform1f(GLuint loc, GLfloat v0)                                     { glUniform1f(loc, v0); }  
  void setUniform2f(GLuint loc, GLfloat v0, GLfloat v1)                         { glUniform2f(loc, v0, v1); }    
  void setUniform3f(GLuint loc, GLfloat v0, GLfloat v1, GLfloat v2)             { glUniform3f(loc, v0, v1, v2); }    
  void setUniform4f(GLuint loc, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) { glUniform4f(loc, v0, v1, v2, v3); } 

  void setUniform1i(GLuint loc, GLint v0)                                       { glUniform1i(loc, v0); }  
  void setUniform2i(GLuint loc, GLint v0, GLint v1)                             { glUniform2i(loc, v0, v1); }    
  void setUniform3i(GLuint loc, GLint v0, GLint v1, GLint v2)                   { glUniform3i(loc, v0, v1, v2); }    
  void setUniform4i(GLuint loc, GLint v0, GLint v1, GLint v2, GLint v3)         { glUniform4i(loc, v0, v1, v2, v3); }

  void setUniform1ui(GLuint loc, GLuint v0)                                     { glUniform1ui(loc, v0); }  
  void setUniform2ui(GLuint loc, GLuint v0, GLuint v1)                          { glUniform2ui(loc, v0, v1); }    
  void setUniform3ui(GLuint loc, GLuint v0, GLuint v1, GLuint v2)               { glUniform3ui(loc, v0, v1, v2); }    
  void setUniform4ui(GLuint loc, GLuint v0, GLuint v1, GLuint v2, GLuint v3)    { glUniform4ui(loc, v0, v1, v2, v3); }

  // Arrays
  void setUniform1fv(GLuint loc, GLsizei count, const GLfloat *v)               { glUniform1fv(loc, count, v); }  
  void setUniform2fv(GLuint loc, GLsizei count, const GLfloat *v)               { glUniform2fv(loc, count, v); }  
  void setUniform3fv(GLuint loc, GLsizei count, const GLfloat *v)               { glUniform3fv(loc, count, v); }  
  void setUniform4fv(GLuint loc, GLsizei count, const GLfloat *v)               { glUniform4fv(loc, count, v); }  
  
  void setUniform1iv(GLuint loc, GLsizei count, const GLint *v)                 { glUniform1iv(loc, count, v); }  
  void setUniform2iv(GLuint loc, GLsizei count, const GLint *v)                 { glUniform2iv(loc, count, v); }  
  void setUniform3iv(GLuint loc, GLsizei count, const GLint *v)                 { glUniform3iv(loc, count, v); }  
  void setUniform4iv(GLuint loc, GLsizei count, const GLint *v)                 { glUniform4iv(loc, count, v); }  
  
  void setUniform1uiv(GLuint loc, GLsizei count, const GLuint *v)               { glUniform1uiv(loc, count, v); }  
  void setUniform2uiv(GLuint loc, GLsizei count, const GLuint *v)               { glUniform2uiv(loc, count, v); }  
  void setUniform3uiv(GLuint loc, GLsizei count, const GLuint *v)               { glUniform3uiv(loc, count, v); }  
  void setUniform4uiv(GLuint loc, GLsizei count, const GLuint *v)               { glUniform4uiv(loc, count, v); }  
  
  // Matrix
  void setUniformMatrix2fv(GLuint loc, GLsizei count, GLboolean transpose, GLfloat *v) { glUniformMatrix2fv(loc, count, transpose, v); }  
  void setUniformMatrix3fv(GLuint loc, GLsizei count, GLboolean transpose, GLfloat *v) { glUniformMatrix3fv(loc, count, transpose, v); }  
  void setUniformMatrix4fv(GLuint loc, GLsizei count, GLboolean transpose, GLfloat *v) { glUniformMatrix4fv(loc, count, transpose, v); }  
#endif                       
  // Receive Uniform variables:
  void getUniformfv(GLuint loc, GLfloat* v)  { glGetUniformfv(program,  loc, v); }
  void getUniformiv(GLuint loc, GLint*   v)  { glGetUniformiv(program,  loc, v); }
  //Requires GL_EXT_gpu_shader4 
  void getUniformuiv(GLuint loc, GLuint*  v) { glGetUniformuiv(program, loc, v); }


  //! This method simply calls glBindAttribLocation for the current ProgramObject.
  void BindAttribLocation(GLint index, GLchar* name) { glBindAttribLocation(program, index, name); }

  //GLfloat      
  void setVertexAttrib1f(GLuint index, GLfloat v0)                                     { glVertexAttrib1f(index, v0); }
  void setVertexAttrib2f(GLuint index, GLfloat v0, GLfloat v1)                         { glVertexAttrib2f(index, v0, v1); }
  void setVertexAttrib3f(GLuint index, GLfloat v0, GLfloat v1, GLfloat v2)             { glVertexAttrib3f(index, v0, v1, v2); }
  void setVertexAttrib4f(GLuint index, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) { glVertexAttrib4f(index, v0, v1, v2, v3); }

#ifndef __EMSCRIPTEN__
  //GLdouble
  void setVertexAttrib1d(GLuint index, GLdouble v0)                                        { glVertexAttrib1d(index, v0); }
  void setVertexAttrib2d(GLuint index, GLdouble v0, GLdouble v1)                           { glVertexAttrib2d(index, v0, v1); }
  void setVertexAttrib3d(GLuint index, GLdouble v0, GLdouble v1, GLdouble v2)              { glVertexAttrib3d(index, v0, v1, v2); }
  void setVertexAttrib4d(GLuint index, GLdouble v0, GLdouble v1, GLdouble v2, GLdouble v3) { glVertexAttrib4d(index, v0, v1, v2, v3); }
  //GLshort
  void setVertexAttrib1s(GLuint index, GLshort v0)                                     { glVertexAttrib1s(index, v0); }
  void setVertexAttrib2s(GLuint index, GLshort v0, GLshort v1)                         { glVertexAttrib2s(index, v0, v1); }
  void setVertexAttrib3s(GLuint index, GLshort v0, GLshort v1, GLshort v2)             { glVertexAttrib3s(index, v0, v1, v2); }
  void setVertexAttrib4s(GLuint index, GLshort v0, GLshort v1, GLshort v2, GLshort v3) { glVertexAttrib4s(index, v0, v1, v2, v3); }

  // Normalized Byte 
  void setVertexAttribNormalizedByte(GLuint index, GLbyte v0, GLbyte v1, GLbyte v2, GLbyte v3) { glVertexAttrib4Nub(index, v0, v1, v2, v3); } 
        
  //GLint (Requires GL_EXT_gpu_shader4)
  void setVertexAttrib1i(GLuint index, GLint v0)                               { glVertexAttribI1i(index, v0); }
  void setVertexAttrib2i(GLuint index, GLint v0, GLint v1)                     { glVertexAttribI2i(index, v0, v1); }
  void setVertexAttrib3i(GLuint index, GLint v0, GLint v1, GLint v2)           { glVertexAttribI3i(index, v0, v1, v2); }
  void setVertexAttrib4i(GLuint index, GLint v0, GLint v1, GLint v2, GLint v3) { glVertexAttribI4i(index, v0, v1, v2, v3); }

  //GLuint (Requires GL_EXT_gpu_shader4)
  void setVertexAttrib1ui(GLuint index, GLuint v0)                                  { glVertexAttribI1ui(index, v0); }
  void setVertexAttrib2ui(GLuint index, GLuint v0, GLuint v1)                       { glVertexAttribI2ui(index, v0, v1); }
  void setVertexAttrib3ui(GLuint index, GLuint v0, GLuint v1, GLuint v2)            { glVertexAttribI3ui(index, v0, v1, v2); }
  void setVertexAttrib4ui(GLuint index, GLuint v0, GLuint v1, GLuint v2, GLuint v3) { glVertexAttribI4ui(index, v0, v1, v2, v3); }

#endif


  //void ActiveVarying(GLchar* name) { glActiveVarying(program,name); }

  GLint getUniformLocation(const GLchar* name);


protected:
	/** The handle to the program object */
	GLuint  program;
    GLuint pipeline;

    VertexShader    *vertObj = nullptr;
    FragmentShader  *fragObj = nullptr;
#ifndef __EMSCRIPTEN__
    GeometryShader  *geomObj = nullptr;
#endif
} ;

#ifdef __EMSCRIPTEN__
class mainProgramObj : public ProgramObject
{
public:
    mainProgramObj() { } 
    virtual ~mainProgramObj() { deleteAll(); }

    void useVertex()   { vertObj = new VertexShader; }
    void useFragment() { fragObj = new FragmentShader; }
    void useAll()      { useVertex(); useFragment(); }

    void deleteVertex()   { delete vertObj; }
    void deleteFragment() { delete fragObj; }
    void deleteAll()      { deleteVertex(); deleteFragment(); }

    void addVertex()      { addShader(vertObj); }
    void addFragment()    { addShader(fragObj); }

    //virtual void create() = PURE_VIRTUAL;

    VertexShader   *getVertex()   { return vertObj; }
    FragmentShader *getFragment() { return fragObj; }


};

#else
class mainProgramObj : public ProgramObject
{
public:
    mainProgramObj() { } 
    virtual ~mainProgramObj() { deleteAll(); }

    void useVertex()   { vertObj = new VertexShader; }
    void useGeometry() { geomObj = new GeometryShader; }
    void useFragment() { fragObj = new FragmentShader; }
    void useAll()      { useVertex(); useGeometry(); useFragment(); }

    void deleteVertex()   { delete vertObj; }
    void deleteGeometry() { delete geomObj; }
    void deleteFragment() { delete fragObj; }
    void deleteAll()      { deleteVertex(); deleteGeometry(); deleteFragment(); }

    void addVertex()      { addShader(vertObj); }
    void addGeometry()    { addShader(geomObj); }
    void addFragment()    { addShader(fragObj); }

    //virtual void create() = PURE_VIRTUAL;

    VertexShader   *getVertex()   { return vertObj; }
    GeometryShader *getGeometry() { return geomObj; }
    FragmentShader *getFragment() { return fragObj; }


};

#endif