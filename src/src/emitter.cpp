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
    if(emit) startPointSlowMotion = 0;
#if !defined(GLCHAOSP_LIGHTVER)
    if(emit) theWnd->getParticlesSystem()->viewObjOFF();
    attractorsList.getStepCondVar().notify_one();
#endif
}

void transformedEmitterClass::setEmitter(bool emit) 
{ 
    bEmitter = emit;
#if !defined(GLCHAOSP_LIGHTVER)
    if(emit) theWnd->getParticlesSystem()->viewObjOFF();
#endif
}

    #if !defined(GLCHAOSP_DISABLE_FEEDBACK)

//
//  transformedEmitter
//
////////////////////////////////////////////////////////////////////////////////
bool transformFeedbackInterleaved::FeedbackActive = false;

float stdRandom(float lo, float hi)  {
    static thread_local std::mt19937 gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    std::uniform_real_distribution<tPrec> dist(lo, hi); return dist(gen);
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
    if(emiss>cPit.getMaxEmissionFrame()) emiss = cPit.getMaxEmissionFrame();

    vec4 *vboBuffer = (vec4 *)InsertVbo->getBuffer();
    int vtxCount = 0;
    float const speedMagnitudo = cPit.getInitialSpeed();
    const GLuint szCircular = getSizeCircularBuffer();
    const GLuint64 pCount = getParticlesCount();
    while(isEmitterOn() && emiss-- && (pCount<szCircular)) {
        attractorsList.get()->Step();

        const vec3 oldPosAttractor(attractorsList.get()->getPrevious());
        const vec3 newPosAttractor(attractorsList.get()->getCurrent());
        const float dist = distance(newPosAttractor, oldPosAttractor);
        const vec3 vStep = (newPosAttractor-oldPosAttractor)/float(cPit.getTransformedEmission());
        vec3 vInc(0.0);

        //vec3 speed(fastRandom.VNI(), fastRandom.VNI(), fastRandom.VNI());
        //speed = normalize(speed) * cPit.getInitialSpeed();
        for(int i=cPit.getTransformedEmission(); i>0; i--) {
            const float bornTime = std::chrono::duration<float> (std::chrono::high_resolution_clock::now()-startEvent).count();
            *vboBuffer++ = vec4(newPosAttractor + vInc, dist);
            *vboBuffer++ = vec4(vec3(fastRandom.VNI(),fastRandom.VNI(),fastRandom.VNI())*speedMagnitudo, -bornTime);

            vInc += vStep;
            vtxCount++;
        }
    }

    InsertVbo->uploadData(vtxCount);
    tfbs[activeBuffer]->Begin(query, getSizeCircularBuffer() * InsertVbo->getBytesPerVertex());
    if(vtxCount) InsertVbo->drawRange(GL_ARRAY_BUFFER, 0,vtxCount);

    CHECK_GL_ERROR()

    const long szI =  tfbs[activeBuffer^1]->getTransformSize();

    // if GL_TRANSFORM_FEEDBACK_BUFFER get error ONLY on FireFox 71 Mobile:
    // Error: WebGL warning: drawArrays: Vertex attrib 1's buffer is bound for transform feedback.
    if(szI) tfbs[activeBuffer^1]->getVertexBase()->drawRange(GL_ARRAY_BUFFER, 0, szI<szCircular ? szI : szCircular);
    CHECK_GL_ERROR()

    const GLuint countV = tfbs[activeBuffer]->End(query, szI+vtxCount);
    tfbs[activeBuffer]->setTransformSize(countV);
    tfbs[activeBuffer^1]->getVertexBase()->setVertexCount(countV);
}
#endif
