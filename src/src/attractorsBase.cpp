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
#include <iostream>

#include "glWindow.h"
#include <future>

AttractorsClass attractorsList; // need to resolve inlines

#define PIQ 9.86960440108935861883f
#define PIH 1.57079632679489661923f
inline float sinAprx(float x) {
    //x = mod(x,2.f*T_PI());
    return (16.f*x*(T_PI-x)) / (5.f*PIQ - 4.f*x*(T_PI - x));
}
inline float cosAprx(float x) {
    return  sinAprx(PIH-x);
}
#undef PIQ
#undef PIH

fastXS64 fractalIIMBase::fRnd64;

//  Attractor base class
////////////////////////////////////////////////////////////////////////////
//std::function<void(float *&, vec4 &, vec4 &v)> func = std::bind((void (AttractorBase::*)(float *&, vec4 &, vec4 &v))&AttractorBase::Step, get(), std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);

//  Thread endless loop
///////////////////////////////////////
void AttractorsClass::endlessStep(emitterBaseClass *emitter)
{

    auto singleStep = [&] (float *ptr) -> void
    {
        if(!emitter->isEmitterOn()) return;

        vec4 v = get()->getCurrent(), vp = v;

        if(emitter->useMappedMem()) {   // USE_MAPPED_BUFFER
            GLuint64 &inc = *emitter->getVertexBase()->getPtrVertexUploaded();
            const uint szBuffer = emitter->getSizeCircularBuffer(); 
            uint countVtx = inc%szBuffer;
            float *newPtr = ptr + (countVtx << 2);
            for(; countVtx<szBuffer && emitter->isEmitterOn() ; countVtx++, inc++) 
                get()->Step(newPtr, v, vp);
        } else {
            for(uint &countVtx = emitter->getRefEmittedParticles(); countVtx<emitter->getSizeStepBuffer() && emitter->isEmitterOn() && (!emitter->stopLoop()); countVtx++)
                get()->Step(ptr, v, vp);
        }
        get()->Insert(vp);
    };

    while(endlessLoop) {
        std::unique_lock<std::mutex> mlock(stepMutex);
        stepCondVar.wait(mlock, std::bind(&emitterBaseClass::loopCanStart, emitter));

        std::lock_guard<std::mutex> l( mtxStep );

        emitter->checkRestartCircBuffer();
        if(emitter->useMappedMem()) {   // USE_MAPPED_BUFFER
            singleStep(emitter->getVertexBase()->getBuffer()); 
            if(emitter->isEmitterOn()) {
                if(emitter->stopFull()) emitter->setEmitterOff();
                if(emitter->restartCircBuff()) emitter->needRestartCircBuffer(true);
            }
        } else {
            emitter->setThreadRunning(true);
            singleStep(emitter->getVertexBase()->getBuffer()); 
            emitter->setThreadRunning(false);
        }
    }
}

void AttractorBase::resetQueue()
{
#if !defined(GLCHAOS_USES_SEMPLIFED_QUEUE)
    stepQueue.resize(BUFFER_DIM,vec4(0.f));
#endif
}

void AttractorBase::Step() 
{
    vec4 v = getCurrent(), vp;

    (this->*stepFn)(v,vp); //attractor::Step(v,vp);

    Insert(vp);

}
std::mutex mut;

void AttractorBase::StepAsync(float *&ptr, vec4 &v, vec4 &vp)
{


    std::unique_lock<std::mutex> guard(mut);

    *((vec3 *)ptr) = *((vec3 *)&vp);

    *(ptr+3) = colorFunc(v, vp);
    ptr+=4;

    //v = vp;

}
void AttractorBase::Step(float *&ptr, vec4 &v, vec4 &vp)
{

    (this->*stepFn)(v,vp);

    *((vec3 *)ptr) = *((vec3 *)&vp);
    *(ptr+3) = colorFunc(v, vp);
    ptr+=4;

    v = vp;
}

uint32_t AttractorBase::Step(float *ptr, uint32_t numElements)
{

#ifdef MULTITHREAD_VOL_FRACTALS // works only if "1 Thread" is selected -> Settings menu
    auto f = [&]() {
        vec4 v = getCurrent(), vp;
        (this->*stepFn)(v,vp);
        StepAsync(ptr, v, vp);

    };

    static timerClass t;

    t.start();
    uint32_t computetdElems = 0;
#pragma omp parallel for
    for(int32_t elems = numElements; elems>0; elems--) {
        f();
        computetdElems++;
        if(!(computetdElems&0x3F) && t.elapsed()>0.1f) elems = 0;
    }

    //Insert(vec4(vp));
    return computetdElems;
#else
     vec4 v = getCurrent(), vp;

    static timerClass t;

    uint32_t elems = numElements;
    t.start();
    while(elems--) {
        Step(ptr, v, vp);
        if(!(elems&0x3F) && t.elapsed()>0.1f) break;
    }

    Insert(vec4(vp));
    return numElements-elems-1;
#endif
}

/*
 vec4 v = getCurrent(), vp;

    static timerClass t;

    uint32_t elems = numElements;
    t.start();
    while(elems--) {
        Step(ptr, v, vp);
        if(!(elems&0x3F) && t.elapsed()>0.1f) break;
    }

    Insert(vec4(vp));
    return numElements-elems-1;
 */

/*
// 4Dim version to test

void AttractorBase::searchLyapunov()
{
    vec3 ve;
    float d0;
    bool canExit = false, restart;
    const float exitVal = 1000.f;
    const int maxIter = 100000;
    int iter = maxIter;
    int counted = 0;
    do {
        do { //testing if diverfgent or convergent to single ponit
            newRandomValues();
            initStep();
            vec4 v0(getCurrent());
            ve = vec3((vec3)v0 + (vec3(RANDOM(kMin,kMax),RANDOM(kMin,kMax),RANDOM(kMin,kMax))/1000.f));
            d0 = distance((vec3)v0, ve);
            restart = false;
            for(int i=0; i<300; i++) {
                AttractorBase::Step();
                vec4 v(getCurrent());
                vec4 p(getPrevious());
                if (v.x > exitVal || v.y > exitVal || v.z > exitVal || 
                    v.x <-exitVal || v.y <-exitVal || v.z <-exitVal || (i>1 && distance(*((vec3 *)value_ptr(v)), *((vec3 *)value_ptr(p))) < FLT_EPSILON) ) {
                    restart = true;
                    iter--;
                    break;
                }  
            }
        } while(restart && iter>0);

        canExit = false;
        counted++;

        float lyapunov = 0.0;
        float fSpace;
        int nL = 1;

        for(int i=1; i<2000; i++) {
            AttractorBase::Step();             
            vec4 v(getCurrent()); // saving v (current value of attractor)

            // compute vepsilon insering ve on queue
            Insert(vec4(ve, 0.f));
            AttractorBase::Step();             
            vec4 veNew(getCurrent());

            Insert(v); // reinsert v on queue

            vec4 vd(v - veNew);
            const float dd = length(vd);

            if(fabs(d0)>FLT_EPSILON && fabs(dd)>FLT_EPSILON) {
                //lyapunov += .721347 * log(fabs(dd / d0));
                lyapunov += .721347 * log(dd / d0);
                nL++;
            }

            ve = v + (vd * d0) / dd;

            //if(!(i%1000)) cout << "i: " <<  i << " - LExp: " << lyapunov/nL << " - L: " <<  nL << endl;

            if( i>1500 && nL> 1350 && ((lyapunov)/nL < 1.0 && lyapunov/nL > 0.015)) {
#if !defined(NDEBUG)
                cout << "berak: " << lyapunov/nL << " - L: " <<  nL << " - i: " <<  i << endl;
#endif
                canExit = true;
                break;
            }
        }
        //lyapunov/=nL;
        //cout << "LyapExp: " << lyapunov << endl;
#if !defined(NDEBUG)
         cout << counted << " - " << maxIter - iter << endl;
#endif
    } while(!canExit && iter-->0);
}
*/

void AttractorBase::searchLyapunov()
{
    vec3 ve;
    float d0;
    bool canExit = false, restart;
    const float exitVal = 1000.f;
    const int maxIter = 100000;
    int iter = maxIter;
    int counted = 0;
    do {
        do { //testing if diverfgent or convergent to single ponit
            newRandomValues();
            initStep();
            vec3 v0(getCurrent());
            ve = vec3(v0 + (vec3(RANDOM(kMin,kMax),RANDOM(kMin,kMax),RANDOM(kMin,kMax))/1000.f));
            d0 = distance((vec3)v0, ve);
            restart = false;
            for(int i=0; i<300; i++) {
                AttractorBase::Step();
                vec3 v(getCurrent());
                vec3 p(getPrevious());
                if (v.x > exitVal || v.y > exitVal || v.z > exitVal || 
                    v.x <-exitVal || v.y <-exitVal || v.z <-exitVal || (i>1 && distance(vec3(v), vec3(p)) < FLT_EPSILON) /*|| distance(v,a) <.001 */) {
                    restart = true;
                    iter--;
                    break;
                }  
            }
        } while(restart && iter>0);

        canExit = false;
        counted++;

        float lyapunov = 0.0;
        int nL = 1;

        for(int i=1; i<2000; i++) {
            AttractorBase::Step();             
            vec3 v(getCurrent()); // saving v (current value of attractor)

            // compute vepsilon insering ve on queue
            Insert(vec4(ve, 0.f));
            AttractorBase::Step();             
            vec3 veNew(getCurrent());

            Insert(vec4(v, 1.0)); // reinsert v on queue

            vec3 vd(v - veNew);
            const float dd = length(vd);

            if(fabs(d0)>FLT_EPSILON && fabs(dd)>FLT_EPSILON) {
                //lyapunov += .721347 * log(fabs(dd / d0));
                lyapunov += .721347 * log(dd / d0);
                nL++;
            }

            ve = v + (vd * d0) / dd;

            //if(!(i%1000)) cout << "i: " <<  i << " - LExp: " << lyapunov/nL << " - L: " <<  nL << endl;

            if( i>1500 && nL> 1350 && ((lyapunov)/nL < 1.0 && lyapunov/nL > 0.015)) {
#if !defined(NDEBUG)
                cout << "berak: " << lyapunov/nL << " - L: " <<  nL << " - i: " <<  i << endl;
#endif
                canExit = true;
                break;
            }
        }
        //lyapunov/=nL;
        //cout << "LyapExp: " << lyapunov << endl;
#if !defined(NDEBUG)
         cout << counted << " - " << maxIter - iter << endl;
#endif
    } while(!canExit && iter-->0);
}


void attractorScalarIterateZ::initStep()
{
    const float gap = abs(zMax-zMin);
    const float buffSize = float(theWnd->getParticlesSystem()->getEmitter()->getSizeCircularBuffer());
    stepZ =  gap*float(zIter)/buffSize;

    zVal = min(zMin, zMax);

    attractorScalarK::initStep();
}


void volumetricFractals::initStep()
{
    stepInc = 2;
    xP = min(sMin.x,sMax.x); yP = min(sMin.y,sMax.y); zP = min(sMin.z,sMax.z);
    //xP = (sMin.x+sMax.x)*.5f; yP = (sMin.y+sMax.y)*.5f; zP = (sMin.z+sMax.z)*.5f;
    const float gapX = abs(sMax.x-sMin.x), gapY = abs(sMax.y-sMin.y), gapZ = abs(sMax.z-sMin.z);
    const float buffSize = float(theWnd->getParticlesSystem()->getEmitter()->getSizeCircularBuffer());
    step = cbrt(cbrt(gapX*gapY*gapZ)/buffSize);

    attractorScalarK::initStep();

}

////////////////////////////////////////////////////////////////////////////
const vec4 volumetricFractals::sequential3Dpoint()
{

    if(stepInc>4) return random3Dpoint();
    if((xP+=step)>sMax.x) {       xP = sMin.x;
        if((yP+=step)>sMax.y) {   yP = sMin.y;
            if((zP+=step)>sMax.z) {
                sMin.x += step/float(stepInc);
                sMin.y += step/float(stepInc);
                sMin.z += step/float(stepInc);
                stepInc<<=1;
                zP = sMin.z;
            }
        }
    }
    return vec4(xP, yP, zP, 0.f);
}

void volumetricFractals::Step(vec4 &v, vec4 &vp)
{
    int i;
    do {
        i = 0;
        vec4 p0(random3Dpoint());
        vec4 vx = p0;// vec4(0.f);

        for(float val = 0.f; i<maxIter && val<upperLimit; i++) {
            val = innerStep(vx, vp, p0);
        }
        vp = p0;
    } while(i<plotRange.x || i>plotRange.y);
    //while(i<skipTop || (whatPlot == skipConvergent && i>=maxIter) || (whatPlot == skipDivergent && i<(maxIter-150)));

    outColor = float(i) / float(maxIter);
}


//  Attractors Thread helper class
////////////////////////////////////////////////////////////////////////////
void threadStepClass::newThread()
{
#if !defined(GLCHAOSP_NO_TH)
    if(getEmitter()->useThread()) {
        attractorsList.queryStartThread();  //endlessLoop = true
        emitter->setEmitterOff();
        if(attractorLoop == nullptr) {
            attractorLoop = new thread(&AttractorsClass::endlessStep, &attractorsList, emitter);
            //attractorLoop->detach();
        }
    }
#endif
}
void threadStepClass::startThread(bool stratOn)
{
    
    emitter->setEmitter(stratOn);
}

void threadStepClass::deleteThread()
{
#if !defined(GLCHAOSP_NO_TH)
    if(getEmitter()->useThread() && getThread() && (getThread()->get_id() != std::thread::id())) {
        attractorsList.queryStopThread();
        stopThread();
        getThread()->join();
        delete getThread();
        //attractorLoop = nullptr;
    }
#endif
}

void threadStepClass::stopThread() {
#if !defined(GLCHAOSP_NO_TH)
    if(getEmitter()->useThread()) {
        emitter->setEmitterOff();
        while(emitter->isLoopRunning()) {
            //emitter->setEmitterOff();
            std::this_thread::sleep_for(200ms);
        }
    }
#endif
}

void threadStepClass::notify() {
#if !defined(GLCHAOSP_NO_TH)
    if(getEmitter()->useThread()) attractorsList.stepCondVar.notify_one();
#endif
}

void threadStepClass::restartEmitter() { 
    emitter->resetVBOindexes(); 
    getEmitter()->resetEmittedParticles();
}
   
//  Attractor Class container
////////////////////////////////////////////////////////////////////////////
void AttractorsClass::newSelection(int i) {
    //if(i==getSelection()) return;
    getThreadStep()->stopThread();
    selection(i);
    theApp->getMainDlg().getParticlesDlgClass().resetTreeParticlesFlags();
#ifdef GLCHAOSP_TEST_RANDOM_DISTRIBUTION // all settings are same
    static bool isFirstTime = true;
    if(isFirstTime) {
        theApp->loadAttractor(getFileName().c_str());
        isFirstTime = false;
    }
#else
    theApp->loadAttractor(getFileName().c_str());
#endif
#ifdef GLCHAOSP_NO_BB
    theApp->setLastFile(getFileName()); //to reload invert settings
#endif
    checkCorrectEmitter();
    getThreadStep()->restartEmitter();
    get()->initStep();
    getThreadStep()->startThread();

}

void AttractorsClass::checkCorrectEmitter()
{
#if !defined(GLCHAOSP_NO_TF)
    if(get()->dtType()) {
        if(tfSettinsClass::tfMode() && theApp->getEmitterEngineType() == enumEmitterEngine::emitterEngine_staticParticles)
            theWnd->getParticlesSystem()->changeEmitter(enumEmitterEngine::emitterEngine_transformFeedback);
        else if(!tfSettinsClass::tfMode() && theApp->getEmitterEngineType() == enumEmitterEngine::emitterEngine_transformFeedback)
            theWnd->getParticlesSystem()->changeEmitter(enumEmitterEngine::emitterEngine_staticParticles);

    } else {
        if(theApp->getEmitterEngineType()!=enumEmitterEngine::emitterEngine_staticParticles)
            theWnd->getParticlesSystem()->changeEmitter(enumEmitterEngine::emitterEngine_staticParticles);
    }
#else
    if(theApp->getEmitterEngineType()!=enumEmitterEngine::emitterEngine_staticParticles)
        theWnd->getParticlesSystem()->changeEmitter(enumEmitterEngine::emitterEngine_staticParticles);
#endif

}
void AttractorsClass::selection(int i) {
    selected = i; 
    theWnd->getParticlesSystem()->getTMat()->setView(get()->getPOV(),get()->getTGT());
    //restart();    
}

void AttractorsClass::selectToContinueDLA(int i) {
    getThreadStep()->stopThread();
    selection(i);
    theApp->getMainDlg().getParticlesDlgClass().resetTreeParticlesFlags();
    theApp->loadAttractor(getFileName().c_str());
//    get()->initStep();

    ((dla3D *)get())->resetIndexData();
    get()->resetQueue();
    get()->Insert(vec4(0.f));

}

void AttractorsClass::newStepThread(emitterBaseClass *e) 
{
    threadStep = new threadStepClass(e);
}

void AttractorsClass::deleteStepThread() 
{
    delete threadStep;
    threadStep = nullptr;
}

void AttractorsClass::generateNewRandom() {
    getThreadStep()->stopThread();

    get()->newRandomValues(); 

    getThreadStep()->restartEmitter();
    get()->initStep();

    get()->searchAttractor();

    getThreadStep()->startThread();
}


void AttractorsClass::restart()
{
    getThreadStep()->stopThread();
    getThreadStep()->restartEmitter();
    get()->initStep();
    getThreadStep()->startThread();

//    if(!theWnd->getParticlesSystem()->getEmitter()->isEmitterOn())
        //theWnd->getParticlesSystem()->getEmitter()->setEmitterOn();

}
