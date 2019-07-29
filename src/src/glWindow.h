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

class particlesSystemClass;

class emitterBaseClass
{
public:
    emitterBaseClass() {
        setEmitterType(theApp->getEmitterType());
        setSizeCircularBuffer(CIRCULAR_BUFFER);
        szAllocatedBuffer = theApp->getMaxAllocatedBuffer();
        //setParticlesCount(0L);
        setSizeStepBuffer(theApp->getEmissionStepBuffer());
        setEmitterOff();
    }

    virtual ~emitterBaseClass() {}

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

    void checkRestartCircBuffer() {
        if(needRestartCircBuffer()) {
            resetVBOindexes();
            attractorsList.get()->resetEmittedParticles();
            attractorsList.get()->initStep();
            needRestartCircBuffer(false);
        }
    }

    void storeData() {
        if(isEmitterOn()) { 
            if(!useMappedMem()) {   // ! USE_MAPPED_BUFFER
                bool bufferFull;
                if(useThread()) {    // USE_THREAD_TO_FILL 
                    bufferFull = InsertVbo->uploadSubBuffer(attractorsList.get()->getEmittedParticles(), szCircularBuffer);
                } else {
                    checkRestartCircBuffer();
                    GLfloat *ptrBuff = InsertVbo->getBuffer();
                    uint32_t numElem = attractorsList.get()->Step(ptrBuff, getSizeStepBuffer());
                    bufferFull = InsertVbo->uploadSubBuffer(numElem, getSizeCircularBuffer());
                    //attractorsList.get()->Step(ptrBuff, 1);
                    //bufferFull = InsertVbo->uploadSubBuffer(1, getSizeCircularBuffer());
                }
                if(bufferFull && stopFull())        setEmitterOff();
                if(bufferFull && restartCircBuff()) needRestartCircBuffer(true);                    
            } 
            //else //Alredy GL_MAP_COHERENT_BIT
                //glMemoryBarrier(GL_CLIENT_MAPPED_BUFFER_BARRIER_BIT);
            
            //
        }
    }


    GLuint getSizeAllocatedBuffer() { return szAllocatedBuffer; }     //getLimitMaxParticlesToEmit
    void setSizeAllocatedBuffer(GLuint p) { szAllocatedBuffer = p; }

    void setSizeCircularBuffer(GLuint p) { szCircularBuffer = p>szAllocatedBuffer ? szAllocatedBuffer : p; }    //getMaxParticlesToEmit
    GLuint getSizeCircularBuffer() { return szCircularBuffer; }

    void setSizeStepBuffer(GLuint step) {  szStepBuffer =  step; }    //getEmittedParticles
    GLuint getSizeStepBuffer() { return szStepBuffer; }

    GLuint64 getParticlesCount() { return InsertVbo->getVertexUploaded(); }
    //inline void setParticlesCount(GLuint val) { ParticlesCount=val; }               

    bool loopCanStart() {
        bool retVal;
        if(useMappedMem())
            retVal = isEmitterOn() || !attractorsList.getEndlessLoop();
        else {
            retVal = (bBufferRendered && isEmitterOn() || !attractorsList.getEndlessLoop());
            bufferRendered(false);
        }
        return retVal;
    }

    void bufferRendered(bool b=true) { bBufferRendered = b; }
    bool isBufferRendered() { return bBufferRendered; }

    bool stopLoop() { return bStopLoop; }
    void stopLoop(bool b) { bStopLoop = b; }


    bool isEmitterOn() { return bEmitter; }
    void setEmitter(bool emit);
    void setEmitterOn();
    void setEmitterOff() { setEmitter(false); }

    bool stopFull() { return bStopFull; }
    void stopFull(bool b) { bStopFull = b; }

    bool restartCircBuff() { return bRestartCircBuff; }
    void restartCircBuff(bool b) { bRestartCircBuff = b; }

    bool needRestartCircBuffer() { return needRestartBuffer; }
    void needRestartCircBuffer(bool b) { needRestartBuffer = b; }

    vertexBufferBaseClass *getVBO() { return InsertVbo; }

    void setThreadRunning(bool b) { threadRunning=b; }
    bool getThreadRunning() { return threadRunning; }
    bool isLoopRunning() { return getThreadRunning(); }
    bool isLoopStopped() { return !getThreadRunning(); }

    bool useThread() { return bUseThread; }
    void useThread(bool b) { bUseThread = b; }

    bool useMappedMem() { return bUseMappedMem; }
    void useMappedMem(bool b) { bUseMappedMem = b; }

    void setEmitterType(int type) {
        switch(type) {
#ifdef GLAPP_REQUIRE_OGL45
            case emitter_separateThread_mappedBuffer:
                bUseThread = true; bUseMappedMem = true; break;
#endif
            case emitter_separateThread_externalBuffer:
                bUseThread = true; bUseMappedMem = false; break;
            case emitter_singleThread_externalBuffer:
            default:
                bUseThread = false; bUseMappedMem = false; break;
        }
    }


protected:
    friend class particlesSystemClass; 

    GLuint szAllocatedBuffer ;
    GLuint szCircularBuffer;
    GLuint szStepBuffer;
    bool bEmitter = false; 
    bool bStopFull = false, bRestartCircBuff = false;

    bool bUseThread = true;
    bool bUseMappedMem = true;
    
    bool bBufferRendered = false, bStopLoop = false;

    vertexBufferBaseClass *InsertVbo;

    bool threadRunning=false;
    bool needRestartBuffer = false;

};

class singleEmitterClass : public emitterBaseClass
{
public:
    singleEmitterClass() {        
        InsertVbo = useMappedMem() ? (vertexBufferBaseClass *) new mappedVertexBuffer(GL_POINTS, getSizeStepBuffer(), 1) : 
                                     (vertexBufferBaseClass *) new vertexBuffer(GL_POINTS, getSizeStepBuffer(), 1);
        InsertVbo->initBufferStorage(getSizeAllocatedBuffer());
    }

    ~singleEmitterClass() {
        delete InsertVbo;
    }

    void preRenderEvents()
    { 
        if(isEmitterOn()) {
            if(!useMappedMem()) {   // ! USE_MAPPED_BUFFER
                if(useThread()) {   // USE_THREAD_TO_FILL 
                    stopLoop(true);
                    while(isLoopRunning()) attractorsList.getThreadStep()->notify();

                    storeData();
                    stopLoop(false);
                    bufferRendered();
                    attractorsList.get()->resetEmittedParticles();
                    attractorsList.getThreadStep()->notify();
                } else 
                    storeData();
            }
            //else
            //attractorsList.getThreadStep()->notify();
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
        InsertVbo = new vertexBuffer(GL_POINTS, 1, 2);
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

    ~particlesSystemClass() { delete emitter; }

    void onReshape(int w, int h) {
        if(w==0 || h==0) return; //Is in iconic (GLFW do not intercept on Window)
        
        mmFBO::onReshape(w,h);

        getTMat()->setPerspective(float(w)/float(h));

        getRenderFBO().reSizeFBO(w, h);
        getPostRendering()->getFBO().reSizeFBO(w, h);
        getShadow()->getFBO().reSizeFBO(w, h);
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

        //glViewport(0,0, getWidth(), getHeight());

        //getShader()->renderOfflineFeedback(attractorsList.get());
/*
        glm::vec3 head = attractorsList.get()->getCurrent();

        glm::vec3 vecA = (attractorsList.get()->getAt(10)-head)*.5f;
        glm::vec3 vecB = (attractorsList.get()->getAt(40)-head)*.5f;
        getTMat()->setView(head+vecB, head+vecA);

        static int sel = attractorsList.getSelection(), oldSel = -1;

        sel = attractorsList.getSelection();

        if(oldSel!=sel) {
            getTMat()->setPerspective(90.f, float(theApp->GetWidth())/float(theApp->GetHeight()), 0.f, 100.f);
            getTMat()->getTrackball().setRotation(glm::quat(1.0f,0.0f, 0.0f, 0.0f));
            getTMat()->getTrackball().setDollyPosition(glm::vec3(0.f));
            getTMat()->getTrackball().setPanPosition(glm::vec3(0.f));
            getTMat()->getTrackball().setRotationCenter(glm::vec3(0.f));
            getTMat()->applyTransforms();
        }
        oldSel = sel;
*/
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

inline void emitterBaseClass::setEmitter(bool emit) 
{ 
    bEmitter = emit;
#if !defined(GLCHAOSP_LIGHTVER)
    if(emit) theWnd->getParticlesSystem()->viewObjOFF();
    attractorsList.getStepCondVar().notify_one();
#endif
}

inline void emitterBaseClass::setEmitterOn() 
{ 
    setEmitter(true); 
#if !defined(GLCHAOSP_LIGHTVER)
    theWnd->getParticlesSystem()->viewObjOFF(); 
#endif
}