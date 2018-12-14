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
#if !defined(__EMSCRIPTEN__)
#include <glad/glad.h>
#endif

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "virtualGizmo.h"


#define GLAPP_USE_MATRIX_BUFFERS

using namespace glm;

struct transfMatrix {
    glm::mat4 vMatrix   = glm::mat4(1.0f);
    glm::mat4 mMatrix   = glm::mat4(1.0f);      // Store the model matrix
    glm::mat4 pMatrix   = glm::mat4(1.0f);
    glm::mat4 mvMatrix  = glm::mat4(1.0f);
    glm::mat4 mvpMatrix = glm::mat4(1.0f);
};

class transformsClass {
#define SZ sizeof(glm::mat4)*3 
public:
    transformsClass() {
        //setView(attractorsList.get()->getPOV(), attractorsList.get()->getTGT());         
        //setPerspective(30.f, float(theApp->GetWidth())/float(theApp->GetHeight()), 0.f, 100.f);
#ifdef GLAPP_USE_MATRIX_BUFFERS
		GLint uBufferMinSize(0);
		glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT, &uBufferMinSize);
        uBlockSize = (GLint(SZ)/ uBufferMinSize) * uBufferMinSize;
        if(SZ%uBufferMinSize) uBlockSize += uBufferMinSize;
    #ifdef GLAPP_REQUIRE_OGL45
        glCreateBuffers(1, &uBuffer);
        glNamedBufferStorage(uBuffer,  uBlockSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
        glNamedBufferSubData(uBuffer, 0, SZ, glm::value_ptr(tM.pMatrix)); 
    #else
        glGenBuffers(1, &uBuffer);
        glBindBuffer(GL_UNIFORM_BUFFER,uBuffer);
        glBufferData(GL_UNIFORM_BUFFER,  uBlockSize, nullptr, GL_STATIC_DRAW);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, SZ, glm::value_ptr(tM.pMatrix)); 
    #endif
    }
    ~transformsClass() {
        glDeleteBuffers(1, &uBuffer);
    }

    void updateBufferData() {
    #ifdef GLAPP_REQUIRE_OGL45
        glNamedBufferSubData(uBuffer, 0, SZ, glm::value_ptr(tM.pMatrix)); 
    #else
        glBindBuffer(GL_UNIFORM_BUFFER,uBuffer);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, SZ, glm::value_ptr(tM.pMatrix));
    #endif
        glBindBufferBase(GL_UNIFORM_BUFFER, bind::bindIdx, uBuffer);
    }
#else
    }
#endif
    void blockBinding(GLuint prog) {
        blockIndex = glGetUniformBlockIndex(prog, "_tMat");
        glUniformBlockBinding(prog, blockIndex, bind::bindIdx);
    }

    void updatePmatrix(GLuint loc) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(tM.pMatrix));
    }
    void updateMmatrix(GLuint loc) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(tM.mMatrix));
    }
    void updateVmatrix(GLuint loc) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(tM.vMatrix));
    }
    void updateMVmatrix(GLuint loc) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(tM.mvMatrix));
    }
    void updateMVPmatrix(GLuint loc) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(tM.mvpMatrix));
    }

    void applyTransforms(const glm::mat4& m = glm::mat4(1.0f)) { 
        applyTransforms(trackball, m);
    }

    void applyTransforms(vfGizmo3DClass& trackball, const glm::mat4& m = glm::mat4(1.0f)) { 
        tM.mMatrix      = m;
        trackball.applyTransform(tM.mMatrix); 
        build_MV_MVP();
    }

    void setModelMatrix(const glm::mat4& m) { tM.mMatrix = m; }
    void setViewMatrix (const glm::mat4& m) { tM.vMatrix = m; }
    void setProjMatrix (const glm::mat4& m) { tM.pMatrix = m; }


    void build_MV_MVP()
    {
        tM.mvMatrix  = tM.vMatrix * tM.mMatrix;
        tM.mvpMatrix = tM.pMatrix * tM.mvMatrix;
    }

    void setView() {         
        tM.vMatrix = glm::lookAt( povVec ,
                                  tgtVec,
                                  vec3(0.0f, 1.0f, 0.0f));
    }

    void setView(const vec3 &pov, const vec3 &tgt) {
        povVec = pov; tgtVec = tgt;
        tM.vMatrix = glm::lookAt( povVec ,
                                  tgtVec,
                                  vec3(0.0f, 1.0f, 0.0f));
    }

    void setPerspective(float angle, float aspect, float _near, float _far) {
        pAngle = angle; pAspect = aspect; pNear = _near; pFar = _far;
        tM.pMatrix  = glm::perspective(glm::radians(angle),aspect,_near, _far); //the projection matrix
    }
    void setPerspective(float aspect) {
        pAspect = aspect;
        tM.pMatrix  = glm::perspective(glm::radians(pAngle),pAspect,pNear, pFar); //the projection matrix
    }
    void setPerspective(float angle, float _near, float _far) {
        pAngle = angle; pNear = _near; pFar = _far;
        tM.pMatrix  = glm::perspective(glm::radians(pAngle),pAspect,pNear, pFar); //the projection matrix
    }

    float getPerspAngle() { return pAngle; }
    float getPerspNear() { return pNear; }
    float getPerspFar() { return pFar; }

    vec3 &getPOV() { return povVec; }
    vec3 &getTGT() { return tgtVec; }

    void setPOV(const vec3 &v) { povVec = v; setView(); }
    void setTGT(const vec3 &v) { tgtVec = v; setView(); }


    vfGizmo3DClass &getTrackball() { return trackball; }
    transfMatrix tM;


private:
    vfGizmo3DClass trackball; 

    float pAngle=30, pNear=.01, pFar=2.0, pAspect = 1.0;
    vec3 povVec, tgtVec;

    GLuint blockIndex;
    GLuint uBlockSize;    
    GLuint uBuffer;
    glm::uint8* ptrBuff;
    enum bind { bindIdx=4 };
    enum loc { pMat = 3, mvMat = 4, mvpMat = 5 };
#undef SZ
};
