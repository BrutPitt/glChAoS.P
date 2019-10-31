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
        setEmitterOff();
    }

    virtual ~emitterBaseClass() {}

    virtual void resetVBOindexes() {
        //setParticlesCount(0L);
        startPointSlowMotion = 0.f;
        InsertVbo->resetVertexCount();
    }

    virtual void preRenderEvents() {}
    virtual void postRenderEvents() {}
    virtual void renderEvents() {}

    void render();

    void checkRestartCircBuffer() {
        if(needRestartCircBuffer()) {
            resetVBOindexes();
            attractorsList.get()->resetEmittedParticles();
            attractorsList.get()->initStep();
            needRestartCircBuffer(false);
        }
    }

    void storeData();


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

    float getStartPointSlowMotion() { return startPointSlowMotion; }

protected:
    friend class particlesSystemClass; 

    GLuint szAllocatedBuffer ;
    GLuint szCircularBuffer;
    GLuint szStepBuffer;

    float startPointSlowMotion = 0.f;

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

        USE_PROGRAM

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
#endif