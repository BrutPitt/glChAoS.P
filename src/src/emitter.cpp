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
#include "emitter.h"
#include "glWindow.h"


void singleEmitterClass::storeData() {
    if(isEmitterOn()) { 
        if(!useMappedMem()) {   // ! USE_MAPPED_BUFFER
            bool bufferFull;
            if(useThread()) {    // USE_THREAD_TO_FILL 
                bufferFull = getVertexBase()->uploadSubBuffer(getEmittedParticles(), szCircularBuffer, stopFull());
            } else {
                checkRestartCircBuffer();
                GLfloat *ptrBuff = getVertexBase()->getBuffer();
                uint32_t numElem = attractorsList.get()->Step(ptrBuff, getSizeStepBuffer());
                bufferFull = getVertexBase()->uploadSubBuffer(numElem, getSizeCircularBuffer(), stopFull());
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

void singleEmitterClass::setEmitter(bool emit) 
{ 
    bEmitter = emit;
#if !defined(GLCHAOSP_LIGHTVER)
    if(emit) theWnd->getParticlesSystem()->viewObjOFF();
    attractorsList.getStepCondVar().notify_one();
#endif
}

#if !defined(GLCHAOSP_DISABLE_FEEDBACK)
//
//  transformFeedbackInterleaved
//
////////////////////////////////////////////////////////////////////////////////
void transformedEmitterClass::setEmitter(bool emit)
{
    if(!bEmitter && emit && restartCircBuff()) {
        attractorsList.getThreadStep()->restartEmitter();
        attractorsList.get()->initStep();
    }

    bEmitter = emit;
#if !defined(GLCHAOSP_LIGHTVER)
    if(emit) theWnd->getParticlesSystem()->viewObjOFF();
#endif

}

transformFeedbackInterleaved::transformFeedbackInterleaved(GLenum primitive, uint32_t stepBuffer, int attributesPerVertex)
{
    trasformVB = new transformVertexBuffer(primitive, stepBuffer, attributesPerVertex);
    trasformVB->initBufferStorage(stepBuffer, GL_ARRAY_BUFFER, GL_DYNAMIC_COPY); // stepBuffer is whole buffer
    trasformVB->buildTransformVertexAttrib();
}

void transformFeedbackInterleaved::Begin(GLuint query, GLsizeiptr sz) 
{
    if (FeedbackActive) return;

    FeedbackActive = true;
    trasformVB->BindToFeedback(0, sz);

    glBeginTransformFeedback(trasformVB->getPrimitive());

    if(bDiscard) glEnable(GL_RASTERIZER_DISCARD);

#if !defined(GLCHAOSP_LIGHTVER)
    glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);
#endif

}

GLuint transformFeedbackInterleaved::End(GLuint query, GLsizeiptr sz) 
{
    GLuint iPrimitivesWritten = 0;
    if(!FeedbackActive)  return -1;
    FeedbackActive = false;

#if !defined(GLCHAOSP_LIGHTVER)
    glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
    glGetQueryObjectuiv(query,GL_QUERY_RESULT,&iPrimitivesWritten);
#endif
    //glGetQueryiv(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, GL_CURRENT_QUERY ,&iPrimitivesWritten);
    if(bDiscard)  glDisable(GL_RASTERIZER_DISCARD);
    glEndTransformFeedback();
    trasformVB->endBindToFeedback(0);

    if(iPrimitivesWritten == 0) iPrimitivesWritten = sz;
    return iPrimitivesWritten;
}



//
//  transformedEmitter
//
////////////////////////////////////////////////////////////////////////////////
bool transformFeedbackInterleaved::FeedbackActive = false;

void transformedEmitterClass::buildEmitter() 
{
    const int numVtxAttrib = 2;

    //Feedback
    GLsizeiptr size = tfSettinsClass::getMaxTransformedEmissionFrame();

    tfbs[0] = new transformFeedbackInterleaved(GL_POINTS, getSizeAllocatedBuffer(), numVtxAttrib);
    tfbs[1] = new transformFeedbackInterleaved(GL_POINTS, getSizeAllocatedBuffer(), numVtxAttrib);

#if !defined(GLCHAOSP_LIGHTVER)
    glGenQueries(1,&query);
#endif
    InsertVbo = new transformVertexBuffer(GL_POINTS, size, numVtxAttrib);
    InsertVbo->initBufferStorage(size, GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW, true);
    InsertVbo->buildTransformVertexAttrib();

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
    uniformBlocksClass::create(GLuint(sizeof(tfSettinsClass::uTFData)), nullptr);
#else
    USE_PROGRAM

    uniformBlocksClass::create(GLuint(sizeof(tfSettinsClass::uTFData)), nullptr, getProgram(), "_TFData");
#endif
}

void transformedEmitterClass::renderOfflineFeedback()
{
    bindPipeline();
    USE_PROGRAM

    static auto start = std::chrono::high_resolution_clock::now();
    static const auto startEvent = start;

    tfSettinsClass &cPit = theWnd->getParticlesSystem()->getParticleRenderPtr()->getTFSettings();

    auto end = std::chrono::high_resolution_clock::now();

    //const GLint index = program->getAttribLocation(p->name);
    cPit.getUdata().diffTime    = std::chrono::duration<float>(end-start).count();
    cPit.getUdata().elapsedTime = std::chrono::duration<float>(end-startEvent).count();

    start = end;
    updateBufferData((void *) &cPit.getUdata());

    static float restEmiss = 0.f;
    float fEmiss = float(cPit.cockPit() ? cPit.getSlowMotionDpS() : cPit.getSlowMotionFSDpS()) * theApp->getTimer().fps()+restEmiss;
    //if(cPit.getPIPposition()!=tfSettinsClass::pip::noPIP) fEmiss*=.5; // with PiP there is double emission (dual pass)
    //fEmiss+=restEmiss;                                              // add prefious pass fraction

    int emiss = fEmiss;
    restEmiss = fEmiss-float(emiss);
    if(emiss>tfSettinsClass::getMaxTransformedEmissionFrame()) emiss = tfSettinsClass::getMaxTransformedEmissionFrame();

    vec4 *vboBuffer = (vec4 *)InsertVbo->getBuffer();
    int vtxCount = 0;
    float const speedMagnitudo = cPit.getInitialSpeed();
    const GLuint szCircular = getSizeCircularBuffer();
    const GLuint vtxStepBuff = InsertVbo->getNumVtxStepBuffer();
    const GLuint pCount = getParticlesCount() % szCircular;

    while(isEmitterOn() && emiss-- && (pCount+vtxCount<szCircular) && vtxCount<vtxStepBuff) {
        attractorsList.get()->Step();

        const vec3 oldPosAttractor(attractorsList.get()->getPrevious());
        const vec3 newPosAttractor(attractorsList.get()->getCurrent());
        const float dist = distance(newPosAttractor, oldPosAttractor);
        const vec3 vStep = (newPosAttractor-oldPosAttractor)/float(cPit.getTransformedEmission());
        vec3 vInc(0.0);

        for(int i=cPit.getTransformedEmission(); i>0 && (pCount+vtxCount<szCircular) && vtxCount<vtxStepBuff; i--) {
            const float bornTime = std::chrono::duration<float> (std::chrono::high_resolution_clock::now()-startEvent).count();
            *vboBuffer++ = vec4(newPosAttractor + vInc, dist);
            *vboBuffer++ = vec4(vec3(fastRandom.VNI(),fastRandom.VNI(),fastRandom.VNI())*speedMagnitudo, -bornTime);

            vInc += vStep;
            vtxCount++;
        }
    }

    InsertVbo->uploadData(vtxCount);
    
    transformFeedbackInterleaved *tfCurr = tfbs[activeBuffer], *tfNext = tfbs[activeBuffer^1];
    const uint64_t szI =  tfNext->getTransformSize();

    tfCurr->Begin(query, (GLsizeiptr) szCircular);
    if(vtxCount) InsertVbo->drawRange(GL_TRANSFORM_FEEDBACK_BUFFER, 0,vtxCount);


    // if GL_TRANSFORM_FEEDBACK_BUFFER get error ONLY on FireFox 71 Mobile:
    // Error: WebGL warning: drawArrays: Vertex attrib 1's buffer is bound for transform feedback.
    if(szI) tfNext->getVertexBase()->drawRange(GL_TRANSFORM_FEEDBACK_BUFFER, 0, szI<szCircular ? szI : szCircular);

    const GLuint countV = tfCurr->End(query, szI+vtxCount<szCircular ? szI+vtxCount : szCircular);
    tfCurr->setTransformSize(countV);
    addVertexCount(vtxCount);

    if(pCount+vtxCount>=szCircular) {
        if(stopFull()) { setEmitter(false); }
        else { if(restartCircBuff()) attractorsList.restart(); }
    }
}
#endif
