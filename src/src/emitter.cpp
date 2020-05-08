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

