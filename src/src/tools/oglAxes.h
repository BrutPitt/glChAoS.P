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
#pragma once

#include <vector>

#include "glslProgramObject.h"
#include "../glApp.h"
#include "transforms.h"

//#define GLAPP_MINIMIZE_VERTEX


#ifdef GLAPP_MINIMIZE_VERTEX
void buildConeFan(std::vector<vec3> &vtx, const float x0, const float x1, const float radius, const int slices);
void buildCylStrip(std::vector<vec3> &vtx, const float x0, const float x1, const float radius, const int slices);
void buildCapFan(std::vector<vec3> &coneVtx, const float x0, const float radius, const int slices);
#else
void buildCone(std::vector<vec3> &vtx, const float x0, const float x1, const float radius, const int slices);
void buildCyl (std::vector<vec3> &vtx, const float x0, const float x1, const float radius, const int slices);
void buildCap (std::vector<vec3> &vtx, const float x0, const float x1, const float radius, const int slices);
#endif
void buildCube(std::vector<vec3> &vtx, const float size);


template <class T> class vaoAxesClass {
public:
    vaoAxesClass(void *vtx, GLuint nVtx) : nVertex(nVtx/2) {
        build(vtx, nVtx);
    }

    void build(void *vtx, GLuint nVtx) {
#ifdef GLAPP_REQUIRE_OGL45
        glCreateVertexArrays(1, &vao);
        glCreateBuffers(1, &vaoBuffer);
        glNamedBufferStorage(vaoBuffer, nVtx*nElemVtx*sizeof(T), vtx, 0); 

        glVertexArrayAttribBinding(vao,loc::vtxID, 0);
        glVertexArrayAttribFormat(vao, loc::vtxID, nElemVtx, GL_FLOAT, GL_FALSE, 0);
        glEnableVertexArrayAttrib(vao, loc::vtxID);        

        glVertexArrayAttribBinding(vao,loc::normID, 0);
        glVertexArrayAttribFormat(vao, loc::normID, nElemVtx, GL_FLOAT, GL_FALSE, nElemVtx*sizeof(T));
        glEnableVertexArrayAttrib(vao, loc::normID);        

        glVertexArrayVertexBuffer(vao, 0, vaoBuffer, 0, 2*nElemVtx*sizeof(T));

#else
        glGenVertexArrays(1, &vao); 
        glGenBuffers(1, &vaoBuffer);
        glBindBuffer(GL_ARRAY_BUFFER,vaoBuffer);
        glBufferData(GL_ARRAY_BUFFER,nVtx*nElemVtx*sizeof(T), vtx, GL_STATIC_READ);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vaoBuffer);
        glVertexAttribPointer(loc::vtxID, nElemVtx, GL_FLOAT, GL_FALSE, 2*nElemVtx*sizeof(T), 0L);
        glEnableVertexAttribArray(loc::vtxID);

        glVertexAttribPointer(loc::normID, nElemVtx, GL_FLOAT, GL_FALSE, 2*nElemVtx*sizeof(T), (void *) (nElemVtx*sizeof(T)));
        glEnableVertexAttribArray(loc::normID);
#endif
    }

    ~vaoAxesClass() {
        glDeleteBuffers(1, &vaoBuffer);
        glDeleteVertexArrays(1, &vao);
    }

    void draw() {
        glBindVertexArray(vao);        
        glDrawArrays(GL_TRIANGLES, 0, nVertex);

    }
    void drawInstanced(GLenum mode) {
        glBindVertexArray(vao);        
        glDrawArraysInstanced(mode, 0, nVertex, 3);
    }

    //void drawElementsInstanced(GLenum mode) {
    //    glBindVertexArray(vao);        
    //    glDrawElementsInstanced(mode, 0, nVertex, 3);
    //}

private:
    GLuint nVertex, nElemVtx = 3;
    GLuint vao, vaoBuffer;

    enum loc { vtxID = 1, normID };
};



class oglAxes : public mainProgramObj, public uniformBlocksClass
{
struct uAxesData {
    mat4 pMat;
    mat4 mvMat;
    vec3 zoomF;
} uData;

public:

    oglAxes(bool cube = false, bool fullAxes = true) : showFullAxes(fullAxes), showCube(cube) { 

        transforms = new transformsClass;

        transforms->setView(vec3(0.f, 0.f, 5.0f), vec3(0.f, 0.f, .0f)); 
        transforms->setPerspective(30.f, theApp->GetHeight() != 0 ? float(theApp->GetWidth())/float(theApp->GetHeight()) : 1.f, 0.05f, 100.f);

        startArrow = showCube ? cubeSize : cylRadius;

#ifdef GLAPP_MINIMIZE_VERTEX
        buildConeFan(coneVtx, arrowDim-coneHgt, arrowDim, coneRadius, coneSlices);
        buildCapFan(capVtx, arrowDim-coneHgt, coneRadius, coneSlices);
        buildCylStrip(cylVtx,  startArrow,arrowDim-coneHgt, .015f, cylSlices, cylRings);
        buildCube(cubeVtx, cubeSize);

        vaoCone    = new vaoAxesClass<float>(coneVtx.data(), GLuint(coneVtx.size()));
        vaoCyl     = new vaoAxesClass<float>(cylVtx.data() , GLuint(cylVtx.size() ));
        vaoCube    = new vaoAxesClass<float>(cubeVtx.data(), GLuint(cubeVtx.size()));
        vaoConeCap = new vaoAxesClass<float>(capVtx.data() , GLuint(capVtx.size() ));
#else

        buildCone(allAxes, arrowDim-coneHgt, arrowDim, coneRadius, coneSlices);
        buildCyl (allAxes,  startArrow,arrowDim-coneHgt, cylRadius, cylSlices);
        if(showFullAxes) { 
            buildCyl (allAxes,  -arrowDim,-startArrow, cylRadius, cylSlices);
            buildCap (allAxes,  -arrowDim,-startArrow, cylRadius, cylSlices);
        }
        if(showCube) {
            buildCube(allAxes, cubeSize);
        }

        vaoAxes = new vaoAxesClass<float>(allAxes.data(), GLuint(allAxes.size()));
#endif

        
        //vaoTriangle = new vaoClass<float>(value_ptr(triangle[0]), sizeof(triangle), 2);
        //vaoCyl =  new vaoClass<float>(cylVtx.data(), cylVtx.size(), 3, 1);
        //vaoConeBase = new vaoClass<float>(coneVtx[SOLID_BASE].data(), coneVtx[SOLID_BASE].size(), 3, 1);

    }



    ~oglAxes() {
#ifdef GLAPP_MINIMIZE_VERTEX
        delete vaoCone;
        delete vaoConeCap;
        delete vaoCyl;
        delete vaoCube;
#else
        delete vaoAxes ;
#endif
        delete transforms;
    }

    void initShaders(const char *vtxDefs="",  const char *fragDefs="");
    void render();
    void renderOnFB(GLuint frameBuffer) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
        render();
    }

    transformsClass *getTransforms() { return transforms; }

    void setBackgroundColor(const vec4 &bg) { bgColor = bg; }

    vec3 &getZoom() { return axesZoom; }
    void setZoom(vec3 v) { axesZoom = v; }

    vec3 &getZoomFactor() { return zoomFactor; }
    void setZoomFactor(vec3 v) { zoomFactor = v; }

    uAxesData &getUData() { return uData; }

    vg::vGizmo3D &getGizmo() { return transforms->getTrackball(); }

    vec3 &getLightPos() { return lightPos; }

    void cubeMgnitude(float mag) { cubeSize *= mag; }


private:
    float startArrow;
    // C++ 11 initializations
    const float arrowDim = 1.0f;
    const float cylRadius = 0.015f;
          float cubeSize = 0.04f;
    const float coneHgt  = .25f;
    const float coneRadius = .05f;
    const int coneSlices = 25;
    const int cylSlices = 13;
    
    transformsClass *transforms;
    GLuint _pMat, _mvMat, _zoomF, _light;
    bool showFullAxes = true, showCube = false;

    

    vec4 bgColor = vec4(0.0f);
 
    vec3 axesZoom = vec3(1.f);
    vec3 zoomFactor = vec3(1.f);
    vec3 lightPos {100.0f, 100.0f, 50.0f};

#ifdef GLAPP_MINIMIZE_VERTEX
    vaoAxesClass<float> *vaoCone, *vaoConeCap, *vaoCyl, *vaoCube;
    std::vector<vec3> coneVtx, capVtx, cylVtx, cubeVtx;
#else
    vaoAxesClass<float> *vaoAxes = nullptr;
    std::vector<vec3> allAxes;
#endif



};

