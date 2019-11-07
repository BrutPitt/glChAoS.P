//------------------------------------------------------------------------------
//  Copyright (c) 2018-2019 Michele Morrone
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

#include <vector>
#include <iostream>
#include <thread>


#include "glApp.h"
#include "attractorsBase.h"
#include "emitter.h"


// Includes all the files for the library

#include "mmFBO.h"


using namespace std;

class glApp;

class shaderPointClass;
class shaderBillboardClass;


class vaoClass {
public:
    vaoClass() {
        float vtx[] = {-1.0f,-1.0f,
                        1.0f,-1.0f,
                        1.0f, 1.0f,
                       -1.0f, 1.0f };


        size = sizeof(vtx);
#ifdef GLAPP_REQUIRE_OGL45
        glCreateVertexArrays(1, &vao);
        glCreateBuffers(1, &vaoBuffer);
        glNamedBufferStorage(vaoBuffer, size, vtx, 0); 

        glVertexArrayAttribBinding(vao,vPosition, 0);
        glVertexArrayAttribFormat(vao, vPosition, 2, GL_FLOAT, GL_FALSE, 0);
        glEnableVertexArrayAttrib(vao, vPosition);        

        glVertexArrayVertexBuffer(vao, 0, vaoBuffer, 0, 8);

#else
        glGenVertexArrays(1, &vao); 
        glGenBuffers(1, &vaoBuffer);
        glBindBuffer(GL_ARRAY_BUFFER,vaoBuffer);
        glBufferData(GL_ARRAY_BUFFER,size, vtx, GL_STATIC_DRAW);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vaoBuffer);
        glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, 0L);
        glEnableVertexAttribArray(vPosition);
        CHECK_GL_ERROR();
#endif
    }

    ~vaoClass() {
        glDeleteBuffers(1, &vaoBuffer);
        glDeleteVertexArrays(1, &vao);
    }

    void draw() {
        glBindVertexArray(vao);        
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        CHECK_GL_ERROR();
    }
private:
    GLuint vao, vaoBuffer;
    enum Attrib_IDs { vPosition = 0 };
    int size;
};

#if !defined(GLCHAOSP_LIGHTVER)
class particlesSystemClass : public shaderPointClass, public shaderBillboardClass
#else
class particlesSystemClass : public shaderPointClass
#endif
{
public:
    particlesSystemClass(emitterBaseClass *sh) : emitter(sh) 
    {
    }

    ~particlesSystemClass() { delete emitter; }

    void onReshape(int w, int h) {
        if(w==0 || h==0) return; //Is in iconic (GLFW do not intercept on Window)
        
        mmFBO::onReshape(w,h);

        getTMat()->setPerspective(float(w)/float(h));

        getRenderFBO().reSizeFBO(w, h);
        getPostRendering()->getFBO().reSizeFBO(w, h);
        getShadow()->resize(w, h);
        getAO()->getFBO().reSizeFBO(w, h);
        shaderPointClass::getGlowRender()->getFBO().reSizeFBO(w, h);
#if !defined(GLCHAOSP_NO_FXAA)
        shaderPointClass::getFXAA()->getFBO().reSizeFBO(w, h);
#endif
#if !defined(GLCHAOSP_LIGHTVER)
        shaderBillboardClass::getGlowRender()->getFBO().reSizeFBO(w, h);
        shaderBillboardClass::getFXAA()->getFBO().reSizeFBO(w, h);
        getMotionBlur()->getFBO().reSizeFBO(w, h);
        getMergedRendering()->getFBO().reSizeFBO(w, h);
#endif

        //shaderPointClass::getGlowRender()->isToUpdate(true);
        setFlagUpdate();
    }

    GLuint render() {

        //

        //getShader()->renderOfflineFeedback(attractorsList.get());


        GLuint texRendered;        

        mat4 m;

    //particlesSystem->getAxes()->getTransforms()->tM.mvMatrix = particlesSystem->getTMat()->tM.mvMatrix;

            //shaderPointClass::render(getMSAAFBO().getFB(0), getEmitter());
            //glBlitNamedFramebuffer(getMSAAFBO().getFB(0),
            //                   getRenderFBO().getFB(0),
            //                   0,0,getMSAAFBO().getSizeX(), getMSAAFBO().getSizeY(),
            //                   0,0,getRenderFBO().getSizeX(), getRenderFBO().getSizeY(),
            //                   GL_COLOR_BUFFER_BIT, GL_NEAREST );


        emitter->preRenderEvents();

#if !defined(GLCHAOSP_LIGHTVER)
        auto renderSelection = [&](particlesBaseClass *particles) {
            getAxes()->getTransforms()->applyTransforms();
            if(showAxes()) {
                getAxes()->renderOnFB(getRenderFBO().getFB(0));
                float zoomK = particles->getTMat()->getPOV().z - particles->getTMat()->getTrackball().getDollyPosition().z;
                getAxes()->setZoomFactor(vec3(vec2(zoomK/10.f), zoomK/7.f) * particles->getTMat()->getPerspAngle()/30.f);
            }            
            texRendered = particles->render(0, getEmitter());

            const GLuint fbo = (getMotionBlur()->Active() || particles->getFXAA()->isOn()) ? particles->getGlowRender()->getFBO().getFB(1) : 0;
            particles->getGlowRender()->render(texRendered, fbo); 
            texRendered = particles->getGlowRender()->getFBO().getTex(1);  // used only if Motionblur

            if(particles->getFXAA()->isOn()) 
                texRendered = particles->getFXAA()->render(texRendered);
        };

        if(getRenderMode() == RENDER_USE_POINTS) {
            renderSelection(shaderPointClass::getPtr());
        }  else if(getRenderMode() == RENDER_USE_BILLBOARD) {
            renderSelection(shaderBillboardClass::getPtr());
        } else {
            GLuint tex1 = shaderBillboardClass::render(0, getEmitter());
            shaderBillboardClass::getGlowRender()->render(tex1, shaderBillboardClass::getGlowRender()->getFBO().getFB(1));  
            GLuint tex2 = shaderPointClass::render(1, getEmitter());
            emitter->bufferRendered();
            shaderPointClass::getGlowRender()->render(tex2, shaderPointClass::getGlowRender()->getFBO().getFB(1));  
            texRendered = getMergedRendering()->render(shaderBillboardClass::getGlowRender()->getFBO().getTex(1), shaderPointClass::getGlowRender()->getFBO().getTex(1));  // only if Motionblur
        }
#else
        particlesBaseClass *particles = shaderPointClass::getPtr();
        texRendered = particles->render(0, getEmitter());
        //particles->render(0, getEmitter());
        const GLuint fbo = 0;
        particles->getGlowRender()->render(texRendered, fbo); 

    #if !defined(GLCHAOSP_NO_FXAA)
        if(particles->getFXAA()->isOn()) 
            texRendered = particles->getFXAA()->render(texRendered);
    #endif
#endif

        emitter->postRenderEvents();
        return texRendered;

    }

    emitterBaseClass *getEmitter() { return emitter; }
    
    //emitterBaseClass *getTransformInterlieve() { return emitter; }

private:
    emitterBaseClass* emitter;
};

class glWindow 
{
public:
    glWindow() {}
    virtual ~glWindow() {}

    void Create(const char* wTitle="glApp", int w=1024, int h=1024, int xPos=100, int yPos=50) {}

    // Called when start Window engine
    virtual void onInit();
    // Called when Window engine is closed.
    virtual void onExit();

    virtual void onIdle();
    virtual void onRender();
    virtual void onReshape(GLint w, GLint h);

    virtual void onMouseButton(int button, int upOrDown, int x, int y);
    virtual void onMouseWheel(int wheel, int direction, int x, int y);
    virtual void onMotion(int x, int y);
    virtual void onPassiveMotion(int x, int y);

    virtual void onKeyDown(unsigned char key, int x, int y);
    virtual void onKeyUp(unsigned char key, int x, int y);
    virtual void onSpecialKeyUp(int key, int x, int y);
    virtual void onSpecialKeyDown(int key, int x, int y);

    GLuint renderAttractor();


    int GetWidth()  { return theApp->GetWidth();  }
    int GetHeight() { return theApp->GetHeight(); }

    particlesSystemClass *getParticlesSystem() { return particlesSystem; }

    vaoClass *getVAO() { return vao; }
    
private:
    vaoClass *vao;

    bool isInitialized;

    friend class particlesSystemClass;
    particlesSystemClass *particlesSystem = nullptr;
};

