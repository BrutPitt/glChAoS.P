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
            attractorsList.get()->resetEmittedParticles();
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


    //void setMaxTransformedEmission(int i)  {  maxTransformedEmission =  i; }    //Max 

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

    void setEmitterType(int type) {
        if(theApp->getEmitterEngineType() == enumEmitterEngine::emitterEngine_transformFeedback) {
            bUseThread = false; bUseMappedMem = false; 
        } else {
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
                    attractorsList.get()->resetEmittedParticles();
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
        startPointSlowMotion = 0.f;
        getVertexBase()->resetVertexCount();
    }

    vertexBufferBaseClass *getVertexBase() { return InsertVbo; }

    void render();
    void storeData();
    void setEmitter(bool emit);


private:
    vertexBufferBaseClass *InsertVbo = nullptr;
};

#if !defined(GLCHAOSP_DISABLE_FEEDBACK)

class transformFeedbackInterleaved {
public:
    transformFeedbackInterleaved(GLenum primitive, uint32_t stepBuffer, int attributesPerVertex)
    {
        trasformVB = new transformVertexBuffer(primitive, stepBuffer, attributesPerVertex);
        trasformVB->initBufferStorage(stepBuffer, GL_TRANSFORM_FEEDBACK_BUFFER, GL_DYNAMIC_COPY); // stepBuffer is whole buffer
        trasformVB->buildTransformVertexAttrib();
    }
    ~transformFeedbackInterleaved() {
        delete trasformVB;
    }



    void Pause() {
        glPauseTransformFeedback();

    }

    void Resume() {
        glResumeTransformFeedback();
    }

    vertexBufferBaseClass *getVertexBase() { return trasformVB; }
    vertexBufferBaseClass *getTrasformVB() { return trasformVB; }
    uint64_t getTransformSize() { return transformSize; }
    void setTransformSize(uint64_t v = 0L) { transformSize = v; }


    void Begin(GLuint query, GLsizeiptr sz) {
        if (FeedbackActive) return;

        FeedbackActive = true;
        trasformVB->BindToFeedback(0, sz);

        glBeginTransformFeedback(trasformVB->getPrimitive());

        if (bDiscard) glEnable(GL_RASTERIZER_DISCARD);

#if !defined(GLCHAOSP_LIGHTVER)
      glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);
#endif
    }

    GLuint End(GLuint query, GLsizeiptr sz) {
        GLuint iPrimitivesWritten = 0;
        if(!FeedbackActive)  return -1;
        FeedbackActive = false;

        //glGetQueryiv(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, GL_CURRENT_QUERY ,&iPrimitivesWritten);
        if(bDiscard)  glDisable(GL_RASTERIZER_DISCARD);
        glEndTransformFeedback();
        trasformVB->endBindToFeedback(0);

#if !defined(GLCHAOSP_LIGHTVER)
        glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
        glGetQueryObjectuiv(query,GL_QUERY_RESULT,&iPrimitivesWritten);
#endif
/*
#ifdef __EMSCRIPTEN__
        iPrimitivesWritten = EM_ASM_INT({ 
            return Module.ctx.getQueryParameter(GL.queries[$0], Module.ctx.QUERY_RESULT);
        }, query);
#endif
*/
        if(iPrimitivesWritten == 0) iPrimitivesWritten = sz;

        return iPrimitivesWritten;
    }
    
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

    void buildEmitter() {
        const int numVtxAttrib = 2;

        //Feedback
        GLsizeiptr size = 1000 * attractorsList.getCockpit().getMaxTransformedEmission();

        tfbs[0] = new transformFeedbackInterleaved(GL_POINTS, getSizeAllocatedBuffer(), numVtxAttrib);
        tfbs[1] = new transformFeedbackInterleaved(GL_POINTS, getSizeAllocatedBuffer(), numVtxAttrib);

#if !defined(GLCHAOSP_LIGHTVER)
        glGenQueries(1,&query);
#endif

        //InsertVbo = new vertexBuffer(GL_POINTS, 1, 2);
        InsertVbo = new transformVertexBuffer(GL_POINTS, size, numVtxAttrib);
        InsertVbo->initBufferStorage(size, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW, true);
        InsertVbo->buildTransformVertexAttrib();

        //InsertVbo->storeSpace(2);

        activeBuffer = 0;

        //build shader

        const GLchar *namesParticlesLoc[] {"posOut", "velTOut", "TexCoord0Out", "TexCoord1Out", "TexCoord2Out"};
        useVertex();
        useFragment();

        getVertex()->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 1, SHADER_PATH "transformEmitterVert.glsl");
        fragObj->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 1, SHADER_PATH "transformEmitterFrag.glsl");

        addVertex();
        addFragment();

        glTransformFeedbackVaryings(getHandle(), numVtxAttrib, namesParticlesLoc, GL_INTERLEAVED_ATTRIBS);


        link();

        removeAllShaders(true);

#ifdef GLAPP_REQUIRE_OGL45
        uniformBlocksClass::create(GLuint(sizeof(cockpitClass::uTFData)), (void *) &attractorsList.getCockpit().getUdata());
#else
        USE_PROGRAM

        uniformBlocksClass::create(GLuint(sizeof(cockpitClass::uTFData)), (void *) &attractorsList.getCockpit().getUdata(), getProgram(), "_TFData");
#endif

        //USE_PROGRAM

        //for(int i=0; i<InsertVbo->getNumComponents(); i++) InsertVbo->getBuffer()[i] = 0.f;

        //InsertVbo->uploadData(1);

        //mainProgramObj::reset();
    }


    ~transformedEmitterClass() {
        delete tfbs[0];
        delete tfbs[1];
#if !defined(GLCHAOSP_LIGHTVER)
        glDeleteQueries(1,&query);
#endif
        delete InsertVbo;
    }


    void renderFeedbackData() {
        // if GL_TRANSFORM_FEEDBACK_BUFFER get error ONLY on FireFox 71 Mobile: 
        // Error: WebGL warning: drawArrays: Vertex attrib 1's buffer is bound for transform feedback.
        const uint64_t sz = tfbs[activeBuffer]->getTransformSize();
        tfbs[activeBuffer]->getTrasformVB()->drawRange(GL_ARRAY_BUFFER, 0, sz<szCircularBuffer ? sz : szCircularBuffer,0);
        CHECK_GL_ERROR();
    }

    void preRenderEvents() { renderOfflineFeedback(attractorsList.get()); }
    void renderEvents() {  render(); }
    void postRenderEvents() { rotateActiveBuffer(); }

    void render() { renderFeedbackData(); }
    void storeData() {}
    void setEmitter(bool emit);

    GLuint getVBO() { return tfbs[activeBuffer]->getVertexBase()->getVBO(); }
    vertexBufferBaseClass *getVertexBase() { return tfbs[activeBuffer]->getVertexBase(); }

    void resetVBOindexes()
    {
        activeBuffer = 0;
        getVertexBase()->resetVertexCount();
        tfbs[0]->setTransformSize();
        tfbs[1]->setTransformSize();
    }

    void rotateActiveBuffer() { activeBuffer ^= 1; }

    void renderOfflineFeedback(AttractorBase *att);

protected:
    int activeBuffer;
    GLuint query = 0;


    transformFeedbackInterleaved *tfbs[2];
    vertexBufferBaseClass *InsertVbo = nullptr;

};
#endif