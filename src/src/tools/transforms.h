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
    glm::mat4 mMatrix   = glm::mat4(1.0f);
    glm::mat4 pMatrix   = glm::mat4(1.0f);      // Uniforms starts HERE!
    glm::mat4 mvMatrix  = glm::mat4(1.0f);
    glm::mat4 mvpMatrix = glm::mat4(1.0f);
    glm::mat4 mvLightM  = glm::mat4(1.0f);
};

class transformsClass {
#define SZ (sizeof(glm::mat4)*4)
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
    GLuint blockBinding(GLuint prog) {
        blockIndex = glGetUniformBlockIndex(prog, "_tMat");
        //glGetActiveUniformBlockiv(prog, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &minSize);
        glUniformBlockBinding(prog, blockIndex, bind::bindIdx);
        return blockIndex;
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
        tM.vMatrix = glm::lookAt( povVec,
                                  tgtVec,
                                  vec3(0.0f, 1.0f, 0.0f));
    }

    void setView(const vec3 &pov, const vec3 &tgt) {
        povVec = pov; tgtVec = tgt;
        tM.vMatrix = glm::lookAt( povVec,
                                  tgtVec,
                                  vec3(0.0f, 1.0f, 0.0f));
    }
    void setLightView(vec3 &lightPos) {
        //mat4 m(1.f);
        //m = translate(m,getPOV());
        //m = translate(m,getTrackball().getPosition());
        tM.mvLightM = glm::lookAt(lightPos * .25f +povVec,
                                  tgtVec, //getTrackball().getRotationCenter(),
                                  vec3(0.0f, 1.0f, 0.0f));
        //tM.mvLightM = tM.mvLightM * m;
        //tM.mvLightM =  tM.mMatrix * tM.mvLightM ;
    }

#define MIN_NEAR .01 //(_far*.01f)
    void setPerspective(float angle, float aspect, float _near, float _far) {        
        pAngle = angle; pAspect = aspect; pNear = _near <= MIN_NEAR ? MIN_NEAR : _near; pFar = _near>_far ? _near : _far;
        tM.pMatrix  = glm::perspectiveFov(glm::radians(pAngle),float(theApp->GetWidth()), float(theApp->GetHeight()),pNear, pFar); //the projection matrix
        //tM.pMatrix  = glm::perspective(glm::radians(angle),aspect,_near, _far); //the projection matrix
    }
    void setPerspective(float aspect) {
        pAspect = aspect;
        //tM.pMatrix  = glm::perspective(glm::radians(pAngle),pAspect,pNear, pFar); //the projection matrix
        tM.pMatrix  = glm::perspectiveFov(glm::radians(pAngle),float(theApp->GetWidth()), float(theApp->GetHeight()),pNear, pFar); //the projection matrix
    }
    void setPerspective() {
        //tM.pMatrix  = glm::perspective(glm::radians(pAngle),pAspect,pNear, pFar); //the projection matrix
        tM.pMatrix  = glm::perspectiveFov(glm::radians(pAngle),float(theApp->GetWidth()), float(theApp->GetHeight()),pNear, pFar); //the projection matrix
    }
    void setPerspective(float angle, float _near, float _far) {
        pAngle = angle; pNear = _near <= MIN_NEAR ? MIN_NEAR : _near; pFar = _near>_far ? _near : _far;
        //tM.pMatrix  = glm::perspective(glm::radians(pAngle),pAspect,pNear, pFar); //the projection matrix
        tM.pMatrix  = glm::perspectiveFov(glm::radians(pAngle),float(theApp->GetWidth()), float(theApp->GetHeight()),pNear, pFar); //the projection matrix
        
    }

//    void setLightOrtho() {
//        tM.pLightM = glm::ortho(-20.0f,20.0f,-20.0f,20.0f,pNear,pFar);
//    }
    float getPerspAngleRad() { return glm::radians(pAngle); }
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
    vec3 povVec=vec3(7.0), tgtVec=vec3(0.0);

    GLuint blockIndex;
    GLuint uBlockSize;    
    GLuint uBuffer;
    glm::uint8* ptrBuff;
    enum bind { bindIdx=4 };
    enum loc { pMat = 3, mvMat = 4, mvpMat = 5 };
    GLint minSize;
#undef SZ
};
