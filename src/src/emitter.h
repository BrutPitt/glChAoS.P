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
#include "glApp.h"
#include "attractorsBase.h"
#include "vertexbuffer.h"

#include "ShadersClasses.h"

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
    }

    virtual ~emitterBaseClass() {}

    virtual void resetVBOindexes()  = 0;

    virtual void preRenderEvents()  = 0;
    virtual void postRenderEvents() = 0;
    virtual void renderEvents()     = 0;

    virtual void render() = 0;
    virtual void storeData() = 0;
    virtual void setEmitter(bool emit) = 0;

    virtual void buildEmitter() = 0;

    void checkRestartCircBuffer() {
        if(needRestartCircBuffer()) {
            resetVBOindexes();
            resetEmittedParticles();
            attractorsList.get()->initStep();
            needRestartCircBuffer(false);
        }
    }

    GLuint getSizeAllocatedBuffer() { return szAllocatedBuffer; }     //getLimitMaxParticlesToEmit
    void setSizeAllocatedBuffer(GLuint p) { szAllocatedBuffer = p; }

    void setSizeCircularBuffer(GLuint p) { szCircularBuffer = p>szAllocatedBuffer ? szAllocatedBuffer : p; }    //getMaxParticlesToEmit
    GLuint getSizeCircularBuffer() { return szCircularBuffer; }

    void setSizeStepBuffer(GLuint step) {  szStepBuffer =  step; }    //getEmittedParticles
    GLuint getSizeStepBuffer() { return szStepBuffer; }

    virtual GLuint getVBO() = 0;
    virtual vertexBufferBaseClass *getVertexBase() = 0;
    //vertexBufferBaseClass *getVBO() { return InsertVbo; }

    GLuint64 getParticlesCount() { return getVertexBase()->getVertexUploaded(); }
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

    void setEmitterOn()  { setEmitter(true ); }
    void setEmitterOff() { setEmitter(false); }

    bool stopFull() { return bStopFull; }
    void stopFull(bool b) { bStopFull = b; }

    bool restartCircBuff() { return bRestartCircBuff; }
    void restartCircBuff(bool b) { bRestartCircBuff = b; }

    bool needRestartCircBuffer() { return needRestartBuffer; }
    void needRestartCircBuffer(bool b) { needRestartBuffer = b; }

    void setThreadRunning(bool b) { threadRunning=b; }
    bool getThreadRunning() { return threadRunning; }
    bool isLoopRunning() { return getThreadRunning(); }
    bool isLoopStopped() { return !getThreadRunning(); }

    bool useThread() { return bUseThread; }
    void useThread(bool b) { bUseThread = b; }

    bool useMappedMem() { return bUseMappedMem; }
    void useMappedMem(bool b) { bUseMappedMem = b; }

    int getEmittedParticles() { return emittedPoints; }
    uint32_t &getRefEmittedParticles() { return emittedPoints; }
    void resetEmittedParticles() { emittedPoints = 0; }
    void incEmittedParticles() { emittedPoints++; }

    void setEmitterType(int type) {
#if !defined(GLCHAOSP_NO_TF)
        if(theApp->getEmitterEngineType() == enumEmitterEngine::emitterEngine_transformFeedback) {
            bUseThread = false; bUseMappedMem = false; 
        } else
#endif
            switch(type) {
#ifdef GLAPP_REQUIRE_OGL45
                case emitter_separateThread_mappedBuffer:
                    bUseThread = true; bUseMappedMem = true; break;
#endif
#if !defined(GLCHAOSP_NO_TH)
                case emitter_separateThread_externalBuffer:
                    bUseThread = true; bUseMappedMem = false; break;
#endif
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

    uint32_t emittedPoints = 0;

    bool bEmitter = false;
    bool bStopFull = false, bRestartCircBuff = false;

    bool bUseThread = false;
    bool bUseMappedMem = true;
    
    bool bBufferRendered = false, bStopLoop = false;

    bool threadRunning=false;
    bool needRestartBuffer = false;

};

class singleEmitterClass : public emitterBaseClass
{
public:
    singleEmitterClass() { setEmitterOff(); }

    void buildEmitter() {        
        InsertVbo = useMappedMem() ? (vertexBufferBaseClass *) new mappedVertexBuffer(GL_POINTS, getSizeStepBuffer(), 1) : 
                                     (vertexBufferBaseClass *) new vertexBuffer(GL_POINTS, getSizeStepBuffer(), 1);
        InsertVbo->initBufferStorage(getSizeAllocatedBuffer());
        InsertVbo->buildVertexAttrib();
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
                    resetEmittedParticles();
                    attractorsList.getThreadStep()->notify();
                } else 
                    storeData();
            }
            //else
            //attractorsList.getThreadStep()->notify();
        }
    }
    void renderEvents() { render(); }
    void postRenderEvents() {}

    GLuint getVBO() { return InsertVbo->getVBO(); }

    void resetVBOindexes() {
        getVertexBase()->resetVertexCount();
    }

    vertexBufferBaseClass *getVertexBase() { return InsertVbo; }

    void render() { getVertexBase()->draw(szCircularBuffer); }
    void storeData();
    void setEmitter(bool emit);

private:
    vertexBufferBaseClass *InsertVbo = nullptr;
};

#if !defined(GLCHAOSP_NO_TF)

class transformFeedbackInterleaved 
{
public:
    transformFeedbackInterleaved(GLenum primitive, uint32_t stepBuffer, int attributesPerVertex);
    ~transformFeedbackInterleaved() { delete trasformVB; }

    void Pause()  { glPauseTransformFeedback();  }
    void Resume() { glResumeTransformFeedback(); }

    vertexBufferBaseClass *getVertexBase() { return trasformVB; }
    vertexBufferBaseClass *getTrasformVB() { return trasformVB; }
    uint64_t getTransformSize() { return transformSize; }
    void setTransformSize(uint64_t v = 0L) { transformSize = v; }

    void Begin(GLuint query, GLsizeiptr sz);
    GLuint End(GLuint query, GLsizeiptr sz);
    
    void SetDiscard(bool value=true){ bDiscard = value; }

private:
    vertexBufferBaseClass *trasformVB;
    static bool FeedbackActive;
    bool bDiscard = true;
    uint64_t transformSize = 0L;
};


class transformedEmitterClass : public mainProgramObj, public emitterBaseClass, public uniformBlocksClass
{
public:      
    transformedEmitterClass() { setEmitterOff(); }

    void buildEmitter();

    ~transformedEmitterClass() {
        delete tfbs[0];
        delete tfbs[1];
#if !defined(GLCHAOSP_NO_TF_QUERY)
        glDeleteQueries(1,&query);
#endif
        delete InsertVbo;
    }

    void renderFeedbackData() {
        // if GL_TRANSFORM_FEEDBACK_BUFFER get error ONLY on FireFox 71 Mobile: 
        // Error: WebGL warning: drawArrays: Vertex attrib 1's buffer is bound for transform feedback.
        const uint64_t sz = tfbs[activeBuffer]->getTransformSize();
        tfbs[activeBuffer]->getTrasformVB()->drawRange(GL_ARRAY_BUFFER, 0, sz<szCircularBuffer ? sz : szCircularBuffer);
    }

    void preRenderEvents() { renderOfflineFeedback(); }
    void renderEvents() {  render(); }
    void postRenderEvents() { rotateActiveBuffer(); }

    void render() { renderFeedbackData(); }
    void storeData() {}
    void setEmitter(bool emit);

    GLuint getVBO() { return tfbs[0]->getVertexBase()->getVBO(); }
    vertexBufferBaseClass *getVertexBase() { return tfbs[0]->getVertexBase(); }
    vertexBufferBaseClass *getVertexBase(int i) { return tfbs[i]->getVertexBase(); }

    void resetVBOindexes()
    {
        activeBuffer = 0;
        tfbs[0]->getVertexBase()->resetVertexCount();
        tfbs[0]->setTransformSize();

        tfbs[1]->getVertexBase()->resetVertexCount();
        tfbs[1]->setTransformSize();
    }

    void addVertexCount(GLuint64 i) {
        tfbs[0]->getVertexBase()->addVertexCount(i);
        tfbs[1]->getVertexBase()->addVertexCount(i);
    }

    void rotateActiveBuffer() { activeBuffer ^= 1; }

    void renderOfflineFeedback();

protected:
    int activeBuffer;
    GLuint query = 0;

    transformFeedbackInterleaved *tfbs[2];
    vertexBufferBaseClass *InsertVbo = nullptr;

    //float stdRandom(float lo, float hi)  {
    //    static thread_local std::mt19937 gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    //    std::uniform_real_distribution<tPrec> dist(lo, hi); return dist(gen);
    //}
};
#endif