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
#include "glslShaderObject.h"

#include <cassert>
using namespace std;

class ProgramObject
{
public:
    ProgramObject();
    virtual ~ProgramObject();
    void createProgram();
    void deleteProgram();

    void addShader(ShaderObject* shader);
    void removeShader(ShaderObject* shader, bool wantDelete = false);
    void deleteShader(ShaderObject* shader);

    void link();

    void bindPipeline();
    void useProgram();
    static void reset();

    GLuint  getHandle() { return program; }
    GLuint  getProgram() { return program; }
    GLuint  getPipeline() { return pipeline; }

#ifdef GLAPP_NO_GLSL_PIPELINE
#define USE_PROGRAM useProgram();
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
#else
#define USE_PROGRAM
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
#if !defined(__EMSCRIPTEN__)
    GeometryShader  *geomObj = nullptr;
#endif
} ;

class mainProgramObj : public ProgramObject
{
public:
    mainProgramObj() { createProgram(); } 
    virtual ~mainProgramObj() { removeAllShaders(false); deleteAllShaders(); }

    void useVertex(VertexShader *VS) { vertObj = VS; vsCloned = true; }
    void useVertex()   { vertObj = new VertexShader; }
    void useFragment() { fragObj = new FragmentShader; }
    void useAll()      { useVertex(); useGeometry(); useFragment(); }

    void removeVertex(bool wantDelete = false)     { removeShader(vertObj, vsCloned ? false : wantDelete); }
    void removeFragment(bool wantDelete = false)   { removeShader(fragObj, wantDelete); }
    void removeAllShaders(bool wantDelete = false) { removeVertex(wantDelete); removeFragment(wantDelete); removeGeometry(wantDelete); }

    void deleteVertex()     { if(!vsCloned) { deleteShader(vertObj); delete vertObj; vertObj = nullptr; } }
    void deleteFragment()   { deleteShader(fragObj); delete fragObj; fragObj = nullptr; }
    void deleteAllShaders() { deleteVertex(); deleteFragment(); deleteGeometry(); }

    void addVertex()      { addShader(vertObj); }
    void addFragment()    { addShader(fragObj); }

    //virtual void create() = PURE_VIRTUAL;

    VertexShader   *getVertex()   { return vertObj; }
    FragmentShader *getFragment() { return fragObj; }

#if !defined(__EMSCRIPTEN__)
    void useGeometry()       { geomObj = new GeometryShader; }
    void removeGeometry(bool wantDelete = false)    { removeShader(geomObj, wantDelete); }
    void deleteGeometry() { deleteShader(geomObj); delete geomObj; geomObj = nullptr; }

    void addGeometry()       { addShader(geomObj); }

    GeometryShader *getGeometry() { return geomObj; }

    bool gsCloned = false;
#else
    void useGeometry() {}
    void removeGeometry(bool wantDelete = false) {}
    void deleteGeometry() {}
#endif

    bool vsCloned = false,  fsCloned = false;

};


class uniformBlocksClass {
public:

     uniformBlocksClass() { }
    ~uniformBlocksClass() { glDeleteBuffers(1, &uBuffer); }

// getting aligment for min size block allocation
    void getAlignment() {
        GLint uBufferMinSize(0);
        glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uBufferMinSize);
        uBlockSize = (GLint(realDataSize)/ uBufferMinSize) * uBufferMinSize;
        if(realDataSize%uBufferMinSize) uBlockSize += uBufferMinSize;
    }

    static GLuint bindIndex(GLuint prog, const char *nameUBlock, GLuint idx)
    {
        GLuint blockIndex = glGetUniformBlockIndex(prog, nameUBlock);
        glUniformBlockBinding(prog, blockIndex, idx);

        return blockIndex;
    }

#ifdef GLAPP_REQUIRE_OGL45
    void create(GLuint size, void *pData, GLuint idx = GLuint(bind::bindIdx)) 
#else
    void create(GLuint size, void *pData, GLuint prog, const char *nameUBlock, GLuint idx = GLuint(bind::bindIdx))
#endif
    {
        bindingLocation = idx;
        realDataSize = size;
        ptrData = pData;

        getAlignment();

#ifdef GLAPP_REQUIRE_OGL45
        glCreateBuffers(1, &uBuffer);
        glNamedBufferStorage(uBuffer,  uBlockSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
        if(ptrData) glNamedBufferSubData(uBuffer, 0, realDataSize, ptrData);
#else
        glGenBuffers(1,    &uBuffer);
        GLuint blockIndex = bindIndex(prog, nameUBlock, bindingLocation);
//get min size block sending
        GLint minBlockSize;
        glGetActiveUniformBlockiv(prog, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &minBlockSize);

        glBindBuffer(GL_UNIFORM_BUFFER,uBuffer);
// now we alloc min size permitted, but copy realDataSize
        glBufferData(GL_UNIFORM_BUFFER,  uBlockSize < minBlockSize ? minBlockSize : uBlockSize, nullptr, GL_STATIC_DRAW);
        if(ptrData) glBufferSubData(GL_UNIFORM_BUFFER, 0, realDataSize, ptrData);
#endif
    }

    void updateBufferData(void *data=nullptr) {
#ifdef GLAPP_REQUIRE_OGL45
        glNamedBufferSubData(uBuffer, 0, realDataSize, data ? data : ptrData);
#else
        glBindBuffer(GL_UNIFORM_BUFFER,uBuffer);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, realDataSize, data ? data : ptrData); 
#endif
        glBindBufferBase(GL_UNIFORM_BUFFER, bindingLocation, uBuffer);
    }


    enum bind { bindIdx=2 };  //internal binding indel location
private:
    void *ptrData;
    GLuint bindingLocation;
    GLuint uBuffer;
    GLuint realDataSize, uBlockSize; 
};