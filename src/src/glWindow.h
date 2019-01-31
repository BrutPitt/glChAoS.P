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

#include <vector>
#include <iostream>
#include <thread>


#include "glApp.h"
#include "attractorsBase.h"
#include "vertexbuffer.h"

#include "ShadersClasses.h"


// Includes all the files for the library

#include "mmFBO.h"


using namespace std;

class glApp;

class shaderPointClass;
class shaderBillboardClass;


void setViewOrtho();

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
        glBufferData(GL_ARRAY_BUFFER,size, vtx, GL_STATIC_READ);

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
    enum Attrib_IDs { vPosition = 2 };
    int size;
};

class particlesSystemClass;

#ifdef USE_MAPPED_BUFFER
    #define vtxBUFFER mappedVertexBuffer
#else
    #define vtxBUFFER vertexBuffer
#endif

class emitterBaseClass
{
public:
    emitterBaseClass() {
        setSizeCircularBuffer(CIRCULAR_BUFFER);
        szAllocatedBuffer = theApp->getMaxAllocatedBuffer();
        //setParticlesCount(0L);
        setSizeStepBuffer(theApp->getEmissionStepBuffer());    
        setEmitterOff();
    }

    virtual void resetVBOindexes() {
        //setParticlesCount(0L);
        InsertVbo->resetVertexCount();
    }

    virtual void preRenderEvents() {}
    virtual void postRenderEvents() {}
    virtual void renderEvents() {}

    void render() {
        InsertVbo->draw(szCircularBuffer);
    }

    void storeData() {
        if(isEmitterOn()) { 

#if !defined(USE_MAPPED_BUFFER) 
    #ifdef USE_THREAD_TO_FILL 
            bool bufferFull = InsertVbo->uploadSubBuffer(attractorsList.get()->getEmittedParticles(), szCircularBuffer);
    #else
            GLfloat *ptrBuff = InsertVbo->getBuffer();
            attractorsList.get()->Step(ptrBuff, getSizeStepBuffer());
            bool bufferFull = InsertVbo->uploadSubBuffer(szStepBuffer, szCircularBuffer);
    #endif
            if(bufferFull && stopFull()) {
                setEmitterOff();
            }
            if(bufferFull && restartCircBuff()) {
                needRestartCircBuffer(true);
            } 
#else
            //Alredy GL_MAP_COHERENT_BIT
            //glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
#endif
            
        }
    }


    GLuint getSizeAllocatedBuffer() { return szAllocatedBuffer; }     //getLimitMaxParticlesToEmit
    void setSizeAllocatedBuffer(GLuint p) { szAllocatedBuffer = p; }

    void setSizeCircularBuffer(GLuint p) { szCircularBuffer = p; }    //getMaxParticlesToEmit
    GLuint getSizeCircularBuffer() { return szCircularBuffer; }

    void setSizeStepBuffer(GLuint step) {  szStepBuffer =  step; }    //getEmittedParticles
    GLuint getSizeStepBuffer() { return szStepBuffer; }

    GLuint64 getParticlesCount() { return InsertVbo->getVertexUploaded(); }
    //inline void setParticlesCount(GLuint val) { ParticlesCount=val; }               

    bool loopCanStart() { 
#ifdef USE_MAPPED_BUFFER
        const bool retVal = isEmitterOn() || !attractorsList.getEndlessLoop();
#else
        const bool retVal = (bBufferRendered && isEmitterOn() || !attractorsList.getEndlessLoop());
        bufferRendered(false);
#endif
        return retVal;
    }

    void bufferRendered(bool b=true) { bBufferRendered = b; }
    bool isBufferRendered() { return bBufferRendered; }

    bool stopLoop() { return bStopLoop; }
    void stopLoop(bool b) { bStopLoop = b; }


    bool isEmitterOn() { return bEmitter; }
    void setEmitter(bool emit) 
    { 
        bEmitter = emit;
#ifdef USE_THREAD_TO_FILL
        attractorsList.getThreadStep()->notify();
#endif
    }
    void setEmitterOn() { setEmitter(true); }
    void setEmitterOff() { setEmitter(false); }

    bool stopFull() { return bStopFull; }
    void stopFull(bool b) { bStopFull = b; }

    bool restartCircBuff() { return bRestartCircBuff; }
    void restartCircBuff(bool b) { bRestartCircBuff = b; }

    bool needRestartCircBuffer() { return needRestartBuffer; }
    void needRestartCircBuffer(bool b) { needRestartBuffer = b; }

    vtxBUFFER *getVBO() { return InsertVbo; }

    void setThreadRunning(bool b) { threadRunning=b; }
    bool getThreadRunning() { return threadRunning; }
    bool isLoopRunning() { return getThreadRunning(); }
    bool isLoopStopped() { return !getThreadRunning(); }



protected:
    friend class particlesSystemClass; 

    GLuint szAllocatedBuffer ;
    GLuint szCircularBuffer;
    GLuint szStepBuffer;
    bool bEmitter = false; 
    bool bStopFull = false, bRestartCircBuff = false;
    
    bool bBufferRendered = false, bStopLoop = false;

    vtxBUFFER *InsertVbo;

    bool threadRunning=false;
    bool needRestartBuffer = false;

};

class singleEmitterClass : public emitterBaseClass
{
public:
    singleEmitterClass() {
        InsertVbo = new vtxBUFFER(GL_POINTS, getSizeStepBuffer(), 1);
        InsertVbo->initBufferStorage(getSizeAllocatedBuffer());
    }

    ~singleEmitterClass() { delete InsertVbo; }

    void preRenderEvents() 
    { 
        if(isEmitterOn()) 
        {
#if !defined(USE_MAPPED_BUFFER)
    #ifdef USE_THREAD_TO_FILL
            stopLoop(true);
            while(isLoopRunning()) attractorsList.getThreadStep()->notify();

            storeData();
            stopLoop(false);
            bufferRendered();
            attractorsList.get()->resetEmittedParticles();
            attractorsList.getThreadStep()->notify();
    #else
            storeData();
    #endif
#else
            //attractorsList.getThreadStep()->notify();
#endif
        }

    }

    void renderEvents() {
        render();
    }

    virtual void postRenderEvents() 
    {
        
    }
 
};

#if !defined(GLCHAOSP_LIGHTVER)
class transformedEmitterClass : public mainProgramObj, public emitterBaseClass
{
public:      

    transformedEmitterClass(int num, GLuint bitMap) {
        

        attribBitMap = bitMap;
        Sizes[0] = Sizes[1] =0L;

        //Feedback
        vbos[0] = new vertexBuffer(GL_POINTS, 1, 2);
        vbos[0]->initBufferStorage(getSizeAllocatedBuffer());
        vbos[1] = new vertexBuffer(GL_POINTS, 1, 2);
        vbos[1]->initBufferStorage(getSizeAllocatedBuffer());

        tfbs[0] = new transformFeedbackInterleaved(vbos[0]);
        tfbs[1] = new transformFeedbackInterleaved(vbos[1]);

        //InsertVbo = new vertexBuffer(GL_POINTS, 1, 2);
        InsertVbo = new vtxBUFFER(GL_POINTS, 1, 2);
        //InsertVbo->storeSpace(2);

        activeBuffer = 0;

        //build shader

        const GLchar *namesParticlesLoc[] {"PositionOut", "ColorOut", "TexCoord0Out", "TexCoord1Out", "TexCoord2Out"};
        useVertex();
        //useAll();
        //useFragment();

	    getVertex()->Load(SHADER_PATH "particlesVShader.glsl");
        //geomObj->Load(SHADER_PATH "BillboardGeom.glsl");
	    //fragObj->Load(SHADER_PATH "BillboardFrag.glsl");

        addVertex();

        glTransformFeedbackVaryings(getHandle(), num+1, namesParticlesLoc, GL_INTERLEAVED_ATTRIBS);


	    link();

        bindPipeline();

	    useProgram();

        vec4 v(0.0f);
        for(int i=0; i<InsertVbo->getNumComponents(); i++) InsertVbo->getBuffer()[i] = 0.f;

        InsertVbo->uploadData(1);

        mainProgramObj::reset();


    }

    ~transformedEmitterClass() {
        delete vbos[0];
        delete vbos[1];

        delete tfbs[0];
        delete tfbs[1];

        delete InsertVbo;
    }

    void renderFeedbackData() {
        vbos[activeBuffer]->drawRange(0, Sizes[activeBuffer]<szCircularBuffer ? Sizes[activeBuffer] : szCircularBuffer);
    }

    void preRenderEvents() { renderOfflineFeedback(attractorsList.get()); }
    void renderEvents() { renderFeedbackData();  }
    void postRenderEvents() { rotateActiveBuffer(); }


    void resetVBOindexes()
    {
        activeBuffer = 0;
        Sizes[0] =  Sizes[1] = 0L;

    }

    void rotateActiveBuffer() { activeBuffer ^= 1; }

    void renderOfflineFeedback(AttractorBase *att);

protected:
    GLuint attribBitMap;
    int activeBuffer;
    long Sizes[2];

    vertexBuffer *vbos[2];
    transformFeedbackInterleaved *tfbs[2];

};

class particlesSystemClass : public shaderPointClass, public shaderBillboardClass
#else
class particlesSystemClass : public shaderPointClass
#endif
{
public:
    particlesSystemClass(emitterBaseClass *sh) : emitter(sh) 
    {
    }

    ~particlesSystemClass() { 
        delete emitter; 
    }

    void onReshape(int w, int h) {
        if(w==0 || h==0) return; //Is in iconic (GLFW do not intercept on Window)
        
        mmFBO::onReshape(w,h);

        getTMat()->setPerspective(float(w)/float(h));

        getRenderFBO().reSizeFBO(w, h);
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

        //getShader()->renderOfflineFeedback(attractorsList.get());

        glViewport(0,0, getWidth(), getHeight());
        
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
            particles->render(getRenderFBO().getFB(0), getEmitter());
            texRendered = getRenderFBO().getTex(0);

            if(particles->getFXAA()->isOn()) 
                texRendered = particles->getFXAA()->render(getRenderFBO().getTex(0));                

            const GLuint fbo = getMotionBlur()->Active() ? particles->getGlowRender()->getFBO().getFB(1) : 0;
            particles->getGlowRender()->render(texRendered, fbo); 
            texRendered = particles->getGlowRender()->getFBO().getTex(1);  // used only if Motionblur
        };

        if(getRenderMode() == RENDER_USE_POINTS) {
            renderSelection(shaderPointClass::getPtr());
        }  else if(getRenderMode() == RENDER_USE_BILLBOARD) {
            renderSelection(shaderBillboardClass::getPtr());
        } else {
            shaderBillboardClass::render(getRenderFBO().getFB(0), getEmitter());
            shaderPointClass::render(getRenderFBO().getFB(1), getEmitter());
            emitter->bufferRendered();
            //setViewOrtho();
            shaderBillboardClass::getGlowRender()->render(getRenderFBO().getTex(0), shaderBillboardClass::getGlowRender()->getFBO().getFB(1));  
            shaderPointClass::getGlowRender()->render(getRenderFBO().getTex(1), shaderPointClass::getGlowRender()->getFBO().getFB(1));  
            texRendered = getMergedRendering()->render(shaderBillboardClass::getGlowRender()->getFBO().getTex(1), shaderPointClass::getGlowRender()->getFBO().getTex(1));  // only if Motionblur
        }
#else
        particlesBaseClass *particles = shaderPointClass::getPtr();
        particles->render(getRenderFBO().getFB(0), getEmitter());
        //particles->render(0, getEmitter());
        texRendered = getRenderFBO().getTex(0);
    #if !defined(GLCHAOSP_NO_FXAA)
            if(particles->getFXAA()->isOn()) 
                texRendered = particles->getFXAA()->render(getRenderFBO().getTex(0));                
    #endif
        const GLuint fbo = 0;
        particles->getGlowRender()->render(texRendered, fbo); 
#endif

        emitter->postRenderEvents();
        return texRendered;

    }

    emitterBaseClass *getEmitter() { return emitter; }
    
    //emitterBaseClass *getTransformInterlieve() { return emitter; }

private:    
    emitterBaseClass* emitter;
};




class glWindow /*: public glApp*/
{
    //mainGLApp *GetApp()    { return mainGLApp::theMainApp; } 
    mainGLApp *GetFrame()  { return theApp; }
    mainGLApp *GetCanvas() { return theApp; }   


public:		

    glWindow() {}
    virtual ~glWindow() {}

    void Create(const char* wTitle="glApp", int w=1024, int h=1024, int xPos=100, int yPos=50) {}

    // Called when start Window engine
	virtual void onInit();
    // Called when Window engine is closed.
	virtual void onExit();

	// The onIdle and onDisplay methods should also be overloaded.
	// Within the onIdle method put the logic of the application.
	// The onDisplay method is for any drawing code.
	virtual void onIdle();
	virtual void onRender();
	virtual void onReshape(GLint w, GLint h);

	virtual void onMouseButton(int button, int upOrDown, int x, int y);
	virtual void onMouseWheel(int wheel, int direction, int x, int y);
	virtual void onMotion(int x, int y);
	virtual void onPassiveMotion(int x, int y);

	// The onKeyDown method handles keyboard input that are standard ASCII keys
	virtual void onKeyDown(unsigned char key, int x, int y);
    virtual void onKeyUp(unsigned char key, int x, int y);
	virtual void onSpecialKeyUp(int key, int x, int y);
	virtual void onSpecialKeyDown(int key, int x, int y);


    int GetWidth()  { return GetCanvas()->GetWidth();  }
    int GetHeight() { return GetCanvas()->GetHeight(); }


    particlesSystemClass *getParticlesSystem() { return particlesSystem; }


    vaoClass *getVAO() { return vao; }
    
    
private:
    void DrawOnTexture();


private:

    
    vaoClass *vao;

    bool isInitialized;

    friend class particlesSystemClass;

    particlesSystemClass *particlesSystem = nullptr;


};




