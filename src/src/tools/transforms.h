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
#if !defined(__EMSCRIPTEN__)
#include <glad/glad.h>
#endif

#include "vGizmo.h"
#include "vGizmoMath.h"


#define GLAPP_USE_MATRIX_BUFFERS

struct transfMatrix {
    mat4 mMatrix   = mat4(1.0f);
    mat4 vMatrix   = mat4(1.0f);
    mat4 pMatrix   = mat4(1.0f);  // Uniforms block starts HERE!!!
    mat4 invP      = mat4(1.0f);
    mat4 mvMatrix  = mat4(1.0f);
    mat4 invMV     = mat4(1.0f);
    mat4 mvpMatrix = mat4(1.0f);
    mat4 mvLightM  = mat4(1.0f);
    mat4 mvpLightM = mat4(1.0f);
};

class transformsClass {
#define SZ (sizeof(transfMatrix)-offsetof(transfMatrix, vMatrix))
#define PTR value_ptr(tM.vMatrix)

//#define SZ (sizeof(transfMatrix))
//#define PTR value_ptr(tM.mMatrix)
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
        glNamedBufferSubData(uBuffer, 0, SZ, PTR); 
    #else
        glGenBuffers(1, &uBuffer);
        glBindBuffer(GL_UNIFORM_BUFFER,uBuffer);

        glBufferData(GL_UNIFORM_BUFFER,  uBlockSize, nullptr, GL_STATIC_DRAW);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, SZ, PTR); 
    #endif
    }
    ~transformsClass() {
        glDeleteBuffers(1, &uBuffer);
    }

    void updateBufferData() {
    #ifdef GLAPP_REQUIRE_OGL45
        glNamedBufferSubData(uBuffer, 0, SZ, PTR); 
    #else
        glBindBuffer(GL_UNIFORM_BUFFER,uBuffer);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, SZ, PTR);
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

    void updatePmatrix  (GLuint loc) { glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(tM.pMatrix)  ); }
    void updateMmatrix  (GLuint loc) { glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(tM.mMatrix)  ); }
    void updateVmatrix  (GLuint loc) { glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(tM.vMatrix)  ); }
    void updateMVmatrix (GLuint loc) { glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(tM.mvMatrix) ); }
    void updateMVPmatrix(GLuint loc) { glUniformMatrix4fv(loc, 1, GL_FALSE, value_ptr(tM.mvpMatrix)); }

#if !defined(GLAPP_NO_GLSL_PIPELINE)
    void updatePmatrix  (GLuint prog, GLuint loc) { glProgramUniformMatrix4fv(prog, loc, 1, GL_FALSE, value_ptr(tM.pMatrix)  ); }
    void updateMmatrix  (GLuint prog, GLuint loc) { glProgramUniformMatrix4fv(prog, loc, 1, GL_FALSE, value_ptr(tM.mMatrix)  ); }
    void updateVmatrix  (GLuint prog, GLuint loc) { glProgramUniformMatrix4fv(prog, loc, 1, GL_FALSE, value_ptr(tM.vMatrix)  ); }
    void updateMVmatrix (GLuint prog, GLuint loc) { glProgramUniformMatrix4fv(prog, loc, 1, GL_FALSE, value_ptr(tM.mvMatrix) ); }
    void updateMVPmatrix(GLuint prog, GLuint loc) { glProgramUniformMatrix4fv(prog, loc, 1, GL_FALSE, value_ptr(tM.mvpMatrix)); }
#endif

    void applyTransforms(const mat4& m = mat4(1.0f)) { 
        applyTransforms(trackball, m);
    }

    void applyTransforms(vg::vGizmo3D& trackball, const mat4& m = mat4(1.0f)) { 
        tM.mMatrix      = m;
        trackball.applyTransform(tM.mMatrix); 
        build_MV_MVP();
    }

    void buid_invMV() {
        tM.invMV = inverse(tM.mvMatrix);
    }
    void buid_invP() {
        tM.invP = inverse(tM.pMatrix);
    }

    void setModelMatrix(const mat4& m) { tM.mMatrix = m; }
    void setViewMatrix (const mat4& m) { tM.vMatrix = m; }
    void setProjMatrix (const mat4& m) { tM.pMatrix = m; }

    mat4& getModelMatrix() { return tM.mMatrix; }
    mat4& getViewMatrix () { return tM.vMatrix; }
    mat4& getProjMatrix () { return tM.pMatrix; }

    void build_MV_MVP()
    {
        tM.mvMatrix  = tM.vMatrix * tM.mMatrix;
        tM.mvpMatrix = tM.pMatrix * tM.mvMatrix;
    }

    void setView() {
        tM.vMatrix = lookAt( povVec,
                             tgtVec,
                             vec3(0.0f, 1.0f, 0.0f));
    }

    void setView(const vec3 &pov, const vec3 &tgt) {
        povVec = pov; tgtVec = tgt;
        tM.vMatrix = lookAt( povVec,
                             tgtVec,
                             vec3(0.0f, 1.0f, 0.0f));
    }
    void setLightView(const vec3 &lightPos) {
        tM.mvLightM = lookAt(lightPos,
                             tgtVec, //getTrackball().getRotationCenter(),
                             vec3(0.0f, 1.0f, 0.0f));
    }
    void setLightView(const vec3 &pov, const vec3 &tgt) {
        tM.mvLightM = lookAt(pov,
                             tgt, //getTrackball().getRotationCenter(),
                             vec3(0.0f, 1.0f, 0.0f));
    }

    float getOverallDistance() { return getPOV().z - getTrackball().getDollyPosition().z; }

#define MIN_NEAR .01f //(_far*.01f)
    void setPerspective(float angle, float aspect, float _near, float _far) {        
        pAngle = angle; pAspect = aspect; pNear = _near <= MIN_NEAR ? MIN_NEAR : _near; pFar = _near>_far ? _near : _far;
        tM.pMatrix  = perspectiveFov(radians(pAngle),float(theApp->GetWidth()), float(theApp->GetHeight()),pNear, pFar); //the projection matrix
        buid_invP();
        //tM.pMatrix  = perspective(radians(angle),aspect,_near, _far); //the projection matrix
    }
    void setPerspective(float aspect) {
        pAspect = aspect;
        //tM.pMatrix  = perspective(radians(pAngle),pAspect,pNear, pFar); //the projection matrix
        tM.pMatrix  = perspectiveFov(radians(pAngle),float(theApp->GetWidth()), float(theApp->GetHeight()),pNear, pFar); //the projection matrix
        buid_invP();
    }
    void setPerspective() {
        //tM.pMatrix  = perspective(radians(pAngle),pAspect,pNear, pFar); //the projection matrix
        tM.pMatrix  = perspectiveFov(radians(pAngle),float(theApp->GetWidth()), float(theApp->GetHeight()),pNear, pFar); //the projection matrix
        buid_invP();
    }
    void setPerspective(float angle, float _near, float _far) {
        pAngle = angle; pNear = _near <= MIN_NEAR ? MIN_NEAR : _near; pFar = _near>_far ? _near : _far;
        //tM.pMatrix  = perspective(radians(pAngle),pAspect,pNear, pFar); //the projection matrix
        tM.pMatrix  = perspectiveFov(radians(pAngle),float(theApp->GetWidth()), float(theApp->GetHeight()),pNear, pFar); //the projection matrix
        buid_invP();
    }

//    void setLightOrtho() {
//        tM.pLightM = ortho(-20.0f,20.0f,-20.0f,20.0f,pNear,pFar);
//    }
    float getPerspAngleRad() { return radians(pAngle); }
    float getPerspAngle() { return pAngle; }
    float getPerspNear() { return pNear; }
    float getPerspFar() { return pFar; }

    vec3 &getPOV() { return povVec; }
    vec3 &getTGT() { return tgtVec; }

    void setPOV(const vec3 &v) { povVec = v; setView(); }
    void setTGT(const vec3 &v) { tgtVec = v; setView(); }


    vg::vGizmo3D &getTrackball() { return trackball; }
    transfMatrix tM;


private:
    vg::vGizmo3D trackball; 

    float pAngle=30.f, pNear=.01f, pFar=2.0f, pAspect = 1.0f;
    vec3 povVec=vec3(7.f), tgtVec=vec3(0.f);

    GLuint blockIndex;
    GLuint uBlockSize;    
    GLuint uBuffer;
    enum bind { bindIdx=4 };
    enum loc { pMat = 3, mvMat = 4, mvpMat = 5 };
#undef SZ
#undef PTR
};
