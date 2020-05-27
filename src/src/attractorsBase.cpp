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

inline float mOne_One(float x)
{
    return x>1.f ? 1.f : x<-1.f ? -1.f : x;

}

//  Attractor base class
////////////////////////////////////////////////////////////////////////////

//  Thread endless loop
///////////////////////////////////////
void AttractorsClass::endlessStep(emitterBaseClass *emitter)
{
    auto singleStep = [&] (float *ptr) -> void
    {
        //mtxStep.lock();

        if(!emitter->isEmitterOn()/* || getSelection() < 0*/) return;

        vec4 v = get()->getCurrent();
        vec4 vp = v;

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
        //mtxStep.unlock();
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
    //stepQueue.clear();
#if !defined(GLCHAOS_USES_SEMPLIFED_QUEUE)
    stepQueue.resize(BUFFER_DIM,vec4(0.f));
#endif
}

void AttractorBase::Step() 
{
    vec4 v = getCurrent();
    vec4 vp;

    //Step(v,vp);
    (this->*stepFn)(v,vp);

    Insert(vp);

}

void AttractorBase::Step(float *&ptr, vec4 &v, vec4 &vp) 
{
    (this->*stepFn)(v,vp);
    
    *(ptr++) = vp.x;
    *(ptr++) = vp.y;
    *(ptr++) = vp.z;

    *(ptr++) = distance((vec3)v, (vec3)vp);

    v = vp;
}

uint32_t AttractorBase::Step(float *ptr, uint32_t numElements) 
{
    vec4 vp;
    vec4 v=getCurrent();

    static timerClass t;

    uint32_t elems = numElements;
    t.start();
    while(elems--) {
        Step(ptr, v, vp); 
        if(!(elems&0x3F) && t.elapsed()>0.1f) break;
    }

    Insert(vec4(vp));
    return numElements-elems-1;
}

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


//  Hopalong Attractor
////////////////////////////////////////////////////////////////////////////
/*
void Hopalong::Step(float *ptr, int numElements) 
{
    vec3 vp;
    vec3 v=getCurrent();
    oldZ = v.z;

    while(numElements--) Step(ptr, v, vp); 

    Insert(vec3(vp));

}


void Hopalong::Step()
{

    vec3 v = getCurrent();
    vec3 w = getPrevious();

    const float r=.01f;
    float a = kVal[0]*r, b = kVal[1]*r, c = kVal[2]*r;

    //Insert(vec3(
    //    v.z - sqrt(abs(kVal[1] * v.x - kVal[2])) * (v.x>0. ? 1. : -1.),
    //    v.z - sqrt(abs(kVal[3] * v.y - kVal[4])) * (v.y>0. ? 1. : -1.),
    //    (kVal[0] + kVal[5])  - v.x - v.y
    //    ));
    static float zy = 0;
    static float oldZ = v.z;
    vec3 nV;

    static float x =0, y=0;
        
    float oldX = x, oldY = y;
                    x = oldY - sqrt(abs(b*r * x - c*r)) * (x > 0.f ? 1. : (x < 0.f ? -1.: 0));
                    y = a*r - oldX;
    float z = step;

    nV = vec3( step*sin(2.f*M_PI*x)*cos(2.f*M_PI*y),
                step*sin(2.f*M_PI*x)*sin(2.f*M_PI*y),
                step*cos(2.f*M_PI*x));

        
    zy = oldZ - sqrt(abs(kVal[4]*r * v.x - kVal[5]*r)) * (v.x > 0.f ? 1. : (v.x < 0.f ? -1.: 0)),

    Insert(nV);

    step += .00000001;
        

}


void Hopalong::Step(float *&ptr, vec3 &v, vec3 &vp) {
    float a = kVal[0]*_r, b = kVal[1]*_r, c = kVal[2]*_r;  
        
    float oldX = _x, oldY = _y;
                    _x = oldY - sqrt(abs(b*_r * _x - c*_r)) * (_x > 0.f ? 1. : (_x < 0.f ? -1.: 0));
                    _y = a*_r - oldX;
    float z = step;

    vp.x = *(ptr++) = step*sin(2.f*M_PI*_x)*cos(2.f*M_PI*_y);
    vp.y = *(ptr++) = step*sin(2.f*M_PI*_x)*sin(2.f*M_PI*_y);
    vp.z = *(ptr++) = step*cos(2.f*M_PI*_x)                ;
    *(ptr++) = distance(v,vp);

    _zy = oldZ - sqrt(abs(kVal[4]*_r * v.x - kVal[5]*_r)) * (v.x > 0.f ? 1. : (v.x < 0.f ? -1.: 0)),

    v = vp;            

    oldZ = v.z;
    step += kVal[6]/100000.f;
}
*/
void Hopalong::Step(vec4 &v, vec4 &vp) {
    float a = kVal[0]*_r, b = kVal[1]*_r, c = kVal[2]*_r;  
        
    float oldX = _x, oldY = _y;
                    _x = oldY - sqrt(abs(b*_r * _x - c*_r)) * (_x > 0.f ? 1. : (_x < 0.f ? -1.: 0));
                    _y = a*_r - oldX;

    vp.x = step*sin(2.f*T_PI*_x)*cos(2.f*pi<float>()*_y);
    vp.y = step*sin(2.f*T_PI*_x)*sin(2.f*pi<float>()*_y);
    vp.z = step*cos(2.f*T_PI*_x)                ;

    _zy = oldZ - sqrt(abs(kVal[4]*_r * v.x - kVal[5]*_r)) * (v.x > 0.f ? 1. : (v.x < 0.f ? -1.: 0)),

    v = vp;            

    oldZ = v.z;
    step += kVal[6]/100000.f;
}


//  PowerN3D Attractor
////////////////////////////////////////////////////////////////////////////

void PowerN3D::Step(vec4 &v, vec4 &vp) 
{
    elv[0] = vec3(1.f);
    for(int i=1; i<=order; i++) elv[i] = elv[i-1] * (vec3)v;

    auto itCf = cf.begin();
    for(int x=order-1; x>=0; x--)
        for(int y=order-1; y>=0; y--)
            for(int z=order-1; z>=0; z--)
                if(x+y+z <= order) *itCf++ = elv[x].x * elv[y].y * elv[z].z;

    *itCf++ = elv[order].x;
    *itCf++ = elv[order].y;
    *itCf++ = elv[order].z;

    vec3 vt(0.f);    

    itCf = cf.begin();
    for(auto &it : kVal) vt += (vec3)it * *itCf++;

    vp = vec4(vt, 0.f);
} 

/*
void PowerN3D::Step(vec4 &v, vec4 &vp) 
{
    elv[0] = dvec3(1.f);
    for(int i=1; i<=order; i++) elv[i] = elv[i-1] * (dvec3)v;

    std::vector<double> dcf(cf.size()); 
    auto itCf = dcf.begin();

    for(int x=order-1; x>=0; x--)
        for(int y=order-1; y>=0; y--)
            for(int z=order-1; z>=0; z--) 
                if(x+y+z <= order) *itCf++ = elv[x].x * elv[y].y * elv[z].z;

    *itCf++ = elv[order].x;
    *itCf++ = elv[order].y;
    *itCf++ = elv[order].z;

    itCf = dcf.begin();
    dvec3 vNew(0.0);
    for(auto &it : kVal) vNew += dvec3(it.x, it.y, it.z)  * *itCf++;

    vp = vec4(vNew.x, vNew.y, vNew.z, 0.f);
}
*/

/*
    static int i = 0;
    switch(i++%3) {
        case 0 :  break;
        case 1 :  vp = vec4(vp.y,vp.z,vp.x); break;
        case 2 :  vp = vec4(vp.z,vp.x,vp.y); break;
    }
 */

////////////////////////////////////////////////////////////////////////////
void PolynomialA::Step(vec4 &v, vec4 &vp)
{
    vp.x = kVal[0].x+ v.y - v.y*v.z;
    vp.y = kVal[0].y+ v.z - v.x*v.z;
    vp.z = kVal[0].z+ v.x - v.x*v.y;                                
}
////////////////////////////////////////////////////////////////////////////
void PolynomialB::Step(vec4 &v, vec4 &vp)
{
    vp.x = kVal[0].x+v.y-v.z*(kVal[1].x+v.y);
    vp.y = kVal[0].y+v.z-v.x*(kVal[1].y+v.z);
    vp.z = kVal[0].z+v.x-v.y*(kVal[1].z+v.x);                                
}
////////////////////////////////////////////////////////////////////////////
void PolynomialC::Step(vec4 &v, vec4 &vp)
{
    vp.x = kVal[0].x +v.x*(kVal[1].x +kVal[2].x *v.x+kVal[3].x *v.y)+v.y*(kVal[4].x+kVal[5].x*v.y);
    vp.y = kVal[0].y +v.y*(kVal[1].y +kVal[2].y *v.y+kVal[3].y *v.z)+v.z*(kVal[4].y+kVal[5].y*v.z);
    vp.z = kVal[0].z +v.z*(kVal[1].z +kVal[2].z *v.z+kVal[3].z *v.x)+v.x*(kVal[4].z+kVal[5].z*v.x);
}
////////////////////////////////////////////////////////////////////////////
void PolynomialABS::Step(vec4 &v, vec4 &vp)
{
    vp.x = kVal[0].x + kVal[1].x*v.x + kVal[2].x*v.y + kVal[3].x*v.z + kVal[4].x*abs(v.x) + kVal[5].x*abs(v.y) +kVal[6].x*abs(v.z);
    vp.y = kVal[0].y + kVal[1].y*v.x + kVal[2].y*v.y + kVal[3].y*v.z + kVal[4].y*abs(v.x) + kVal[5].y*abs(v.y) +kVal[6].y*abs(v.z);
    vp.z = kVal[0].z + kVal[1].z*v.x + kVal[2].z*v.y + kVal[3].z*v.z + kVal[4].z*abs(v.x) + kVal[5].z*abs(v.y) +kVal[6].z*abs(v.z);
}
////////////////////////////////////////////////////////////////////////////
void PolynomialPow::Step(vec4 &v, vec4 &vp)
{
    vp.x = kVal[0].x + kVal[1].x*v.x + kVal[2].x*v.y + kVal[3].x*v.z + kVal[4].x*abs(v.x) + kVal[5].x*abs(v.y) +kVal[6].x*pow(abs(v.z),kVal[7].x);
    vp.y = kVal[0].y + kVal[1].y*v.x + kVal[2].y*v.y + kVal[3].y*v.z + kVal[4].y*abs(v.x) + kVal[5].y*abs(v.y) +kVal[6].y*pow(abs(v.z),kVal[7].y);
    vp.z = kVal[0].z + kVal[1].z*v.x + kVal[2].z*v.y + kVal[3].z*v.z + kVal[4].z*abs(v.x) + kVal[5].z*abs(v.y) +kVal[6].z*pow(abs(v.z),kVal[7].z);

}
////////////////////////////////////////////////////////////////////////////
void PolynomialSin::Step(vec4 &v, vec4 &vp)
{
//SinCosA
//    vp.x = kVal[0].x + kVal[1].x*v.x + kVal[2].x*v.y + kVal[3].x*v.z + kVal[4].x*cos(kVal[5].x*v.x) + kVal[6].x*sin(kVal[7].x*v.y) +kVal[8].x*sin(kVal[9].x*v.z);
//    vp.y = kVal[0].y + kVal[1].y*v.x + kVal[2].y*v.y + kVal[3].y*v.z + kVal[4].y*sin(kVal[5].y*v.x) + kVal[6].y*cos(kVal[7].y*v.y) +kVal[8].y*sin(kVal[9].y*v.z);
//    vp.z = kVal[0].z + kVal[1].z*v.x + kVal[2].z*v.y + kVal[3].z*v.z + kVal[4].z*sin(kVal[5].z*v.x) + kVal[6].z*sin(kVal[7].z*v.y) +kVal[8].z*cos(kVal[9].z*v.z);
/*
    vp.x = kVal[0].x + kVal[1].x*v.x + kVal[2].x*v.y + kVal[3].x*v.z + kVal[4].x*cos(kVal[5].x*v.x) + kVal[6].x*sin(kVal[7].x*v.y) +kVal[8].x*cos(kVal[9].x*v.z);
    vp.y = kVal[0].y + kVal[1].y*v.x + kVal[2].y*v.y + kVal[3].y*v.z + kVal[4].y*sin(kVal[5].y*v.x) + kVal[6].y*cos(kVal[7].y*v.y) +kVal[8].y*sin(kVal[9].y*v.z);
    vp.z = kVal[0].z + kVal[1].z*v.x + kVal[2].z*v.y + kVal[3].z*v.z + kVal[4].z*cos(kVal[5].z*v.x) + kVal[6].z*sin(kVal[7].z*v.y) +kVal[8].z*cos(kVal[9].z*v.z);
*/

    vp.x = kVal[0].x + kVal[1].x*v.x + kVal[2].x*v.y + kVal[3].x*v.z + kVal[4].x*sin(kVal[5].x*v.x) + kVal[6].x*sin(kVal[7].x*v.y) +kVal[8].x*sin(kVal[9].x*v.z);
    vp.y = kVal[0].y + kVal[1].y*v.x + kVal[2].y*v.y + kVal[3].y*v.z + kVal[4].y*sin(kVal[5].y*v.x) + kVal[6].y*sin(kVal[7].y*v.y) +kVal[8].y*sin(kVal[9].y*v.z);
    vp.z = kVal[0].z + kVal[1].z*v.x + kVal[2].z*v.y + kVal[3].z*v.z + kVal[4].z*sin(kVal[5].z*v.x) + kVal[6].z*sin(kVal[7].z*v.y) +kVal[8].z*sin(kVal[9].z*v.z);

/*
    vp.x = kVal[0].x + kVal[1].x*v.x + kVal[2].x*v.y + kVal[3].x*v.z + kVal[4].x*sin(kVal[5].x*kVal[6].x*v.x) + kVal[7].x*sin(kVal[8].x*kVal[9].x*v.y) +kVal[10].x*sin(kVal[11].x*kVal[12].x*v.z);
    vp.y = kVal[0].y + kVal[1].y*v.x + kVal[2].y*v.y + kVal[3].y*v.z + kVal[4].y*sin(kVal[5].y*kVal[6].y*v.x) + kVal[7].y*sin(kVal[8].y*kVal[9].y*v.y) +kVal[10].y*sin(kVal[11].y*kVal[12].y*v.z);
    vp.z = kVal[0].z + kVal[1].z*v.x + kVal[2].z*v.y + kVal[3].z*v.z + kVal[4].z*sin(kVal[5].z*kVal[6].z*v.x) + kVal[7].z*sin(kVal[8].z*kVal[9].z*v.y) +kVal[10].z*sin(kVal[11].z*kVal[12].z*v.z);
*/
}
////////////////////////////////////////////////////////////////////////////
void Rampe01::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.z*sin(kVal[0].x*v.x)+cos(kVal[1].x*v.y);
    vp.y = v.x*sin(kVal[0].y*v.y)+cos(kVal[1].y*v.z);
    vp.z = v.y*sin(kVal[0].z*v.z)+cos(kVal[1].z*v.x);
}
////////////////////////////////////////////////////////////////////////////
void Rampe02::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.z*sin(kVal[0].x*v.x)+acos(mOne_One(kVal[1].x*v.y));
    vp.y = v.x*sin(kVal[0].y*v.y)+acos(mOne_One(kVal[1].y*v.z));
    vp.z = v.y*sin(kVal[0].z*v.z)+acos(mOne_One(kVal[1].z*v.x));
}
////////////////////////////////////////////////////////////////////////////
void Rampe03::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.x*v.z*sin(kVal[0].x*v.x)-cos(kVal[1].x*v.y);
    vp.y = v.y*v.x*sin(kVal[0].y*v.y)-cos(kVal[1].y*v.z);
    vp.z = v.z*v.y*sin(kVal[0].z*v.z)-cos(kVal[1].z*v.x);
}
////////////////////////////////////////////////////////////////////////////
void Rampe03A::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.z*v.z*sin(kVal[0].x*v.x)-cos(kVal[1].x*v.y);
    vp.y = v.x*v.x*sin(kVal[0].y*v.y)-cos(kVal[1].y*v.z);
    vp.z = v.y*v.y*sin(kVal[0].z*v.z)-cos(kVal[1].z*v.x);
}
////////////////////////////////////////////////////////////////////////////
void Rampe04::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.x*sin(kVal[0].x*v.x)+cos(kVal[1].x*v.y);
    vp.y = v.y*sin(kVal[0].y*v.y)+cos(kVal[1].y*v.z);
    vp.z = v.z*sin(kVal[0].z*v.z)+cos(kVal[1].z*v.x);
}
////////////////////////////////////////////////////////////////////////////
void Rampe05::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.z*sin(kVal[0].x*v.x)+cos(kVal[1].x*v.y)+sin(kVal[2].x*v.z);
    vp.y = v.x*sin(kVal[0].y*v.x)+cos(kVal[1].y*v.y)+sin(kVal[2].y*v.z);
    vp.z = v.y*sin(kVal[0].z*v.x)+cos(kVal[1].z*v.y)+sin(kVal[2].z*v.z);
}
////////////////////////////////////////////////////////////////////////////
void Rampe06::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.z*sin(kVal[0].x*v.x)-cos(kVal[1].x*v.y);
    vp.y = v.x*sin(kVal[0].y*v.y)+cos(kVal[1].y*v.z);
    vp.z = v.y*sin(kVal[0].z*v.z)-cos(kVal[1].z*v.x);
}
////////////////////////////////////////////////////////////////////////////
void Rampe07::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.z*sin(kVal[0].x*v.x)-cos(kVal[1].x*v.y);
    vp.y = v.x*cos(kVal[0].y*v.y)+sin(kVal[1].y*v.z);
    vp.z = v.y*sin(kVal[0].z*v.z)-cos(kVal[1].z*v.x);
}
////////////////////////////////////////////////////////////////////////////
void Rampe08::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.z*sin(kVal[0].x*v.x)-cos(v.y);
    vp.y = v.x*cos(kVal[0].y*v.y)+sin(v.z);
    vp.z = v.y*sin(kVal[0].z*v.z)-cos(v.x);
}
////////////////////////////////////////////////////////////////////////////
void Rampe09::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.z*sin(kVal[0].x*v.x)-acos(mOne_One(kVal[1].x*v.y))+sin(kVal[2].x*v.z);
    vp.y = v.x*sin(kVal[0].y*v.x)-acos(mOne_One(kVal[1].y*v.y))+sin(kVal[2].y*v.z);
    vp.z = v.y*sin(kVal[0].z*v.x)-acos(mOne_One(kVal[1].z*v.y))+sin(kVal[2].z*v.z);
}
////////////////////////////////////////////////////////////////////////////
void Rampe10::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.z*v.y*sin(kVal[0].x*v.x)-cos(kVal[1].x*v.y)+asin(mOne_One(kVal[2].x*v.z));
    vp.y = v.x*v.z*sin(kVal[0].y*v.x)-cos(kVal[1].y*v.y)+ sin(kVal[2].y*v.z);
    vp.z = v.y*v.x*sin(kVal[0].z*v.x)-cos(kVal[1].z*v.y)+ sin(kVal[2].z*v.z);
}
////////////////////////////////////////////////////////////////////////////
void KingsDream::Step(vec4 &v, vec4 &vp)
{
    vp.x = sin(v.z * kVal[0]) + kVal[3] * sin(v.x * kVal[0]);
    vp.y = sin(v.x * kVal[1]) + kVal[4] * sin(v.y * kVal[1]);
    vp.z = sin(v.y * kVal[2]) + kVal[5] * sin(v.z * kVal[2]);
}
////////////////////////////////////////////////////////////////////////////
void Pickover::Step(vec4 &v, vec4 &vp) 
{
    vp.x =     sin(kVal[0]*v.y) - v.z*cos(kVal[1]*v.x);
    vp.y = v.z*sin(kVal[2]*v.x) -     cos(kVal[3]*v.y);
    vp.z =     sin(v.x)                               ;
}
////////////////////////////////////////////////////////////////////////////
void SinCos::Step(vec4 &v, vec4 &vp) 
{
    vp.x =  cos(kVal[0]*v.x) + sin(kVal[1]*v.y) - sin(kVal[2]*v.z);
    vp.y =  sin(kVal[3]*v.x) - cos(kVal[4]*v.y) + sin(kVal[5]*v.z);
    vp.z = -cos(kVal[6]*v.x) + cos(kVal[7]*v.y) + cos(kVal[8]*v.z);
}
////////////////////////////////////////////////////////////////////////////
void Mira3D::Step(vec4 &v, vec4 &vp) 
{
    auto f = [] (float x, float k) -> float {
        const float x2 = x*x;
        return k*x + 2.f*(1.f-k)*x2 / (1+x2);
    };

    vp.x =  v.y + f(v.x, kVal[0]);
    const float vpp = f(vp.x, kVal[0]);
    vp.y =  vpp - v.x;
    vp.z =  f(v.z, kVal[3]) + f(vp.y, kVal[2]) + f(vpp, kVal[1]);
}
////////////////////////////////////////////////////////////////////////////
void Mira4D::Step(vec4 &v, vec4 &vp) 
{
    auto f = [] (const float x, const float k) -> float {
        const float x2 = x*x;
        return k*x + 2.f*(1.f-k)*x2 / (1+x2);
    };
    vp.x =  kVal[1]*v.y + f(v.x, kVal[0]);
    vp.y =  - v.x + f(vp.x, kVal[0]);
    vp.z =  kVal[3]*v.w + f(v.z, kVal[2]);
    vp.w =  - v.z + f(vp.z, kVal[2]);

}
////////////////////////////////////////////////////////////////////////////
void PopCorn3D::Step(vec4 &v, vec4 &vp) 
{
    vp.x = v.x - kVal[0] * sin((double)v.z+tan((double)kVal[1]*v.z));
    vp.y = v.y - kVal[2] * sin((double)v.x+tan((double)kVal[3]*v.x));
    vp.z = v.z - kVal[4] * sin((double)v.y+tan((double)kVal[5]*v.y));
}
////////////////////////////////////////////////////////////////////////////
void PopCorn4Dset::Step(vec4 &v, vec4 &vp) 
{
    vp.x = v.x - kVal[0] * pfX((double)v.y+tan((double)kVal[1]*v.y));
    vp.y = v.y - kVal[2] * pfY((double)v.x+tan((double)kVal[3]*v.x));
    vp.z = v.z - kVal[4] * pfZ((double)v.w+tan((double)kVal[5]*v.w));
    vp.w = v.w - kVal[6] * pfW((double)v.z+tan((double)kVal[7]*v.z));
}
////////////////////////////////////////////////////////////////////////////
void PopCorn4Dsimple::Step(vec4 &v, vec4 &vp)
{
    vp.x = kVal[0] * pfX((double)v.y+tan((double)kVal[1]*v.y));
    vp.y = kVal[2] * pfY((double)v.x+tan((double)kVal[3]*v.x));
    vp.z = kVal[4] * pfZ((double)v.w+tan((double)kVal[5]*v.w));
    vp.w = kVal[6] * pfW((double)v.z+tan((double)kVal[7]*v.z));
}
////////////////////////////////////////////////////////////////////////////
void PopCorn4Drnd::Step(vec4 &v, vec4 &vp) 
{
    vp.x = kVal[0] - kVal[1]*v.x*v.x + v.y;
    vp.y = kVal[2]*v.x;
    vp.z = kVal[3] - kVal[4]*v.z*v.z + v.w;
    vp.w = kVal[5]*v.z;
}
////////////////////////////////////////////////////////////////////////////
void Martin4DBase::Step(vec4 &v, vec4 &vp) 
{
    vp.x = v.y - pfX((double)v.x);
    vp.y = kVal[0] - v.x;
    vp.z = v.w - pfZ((double)v.z);
    vp.w = kVal[1] - v.z;
}
////////////////////////////////////////////////////////////////////////////
void SymmetricIcons4D::Step(vec4 &v, vec4 &vp) 
{
    //alpha, beta, gamma, lambda, omega, degree
    const float sqrXY = v.x*v.x + v.y*v.y;
    const float sqrZW = v.z*v.z + v.w*v.w; 
    float pXY = kVal[0]*sqrXY + kVal[3];
    float pZW = kVal[5]*sqrZW + kVal[8];

    vec4 newP(v);

    for(int i=6; i>0; i--) {
        newP = vec4(newP.x*v.x - newP.y*v.y, newP.y*v.x + newP.x*v.y,
                    newP.z*v.z - newP.w*v.w, newP.w*v.z + newP.z*v.w);
    }

    float nXY = v.x*newP.x - v.y*newP.y;
    float nZW = v.z*newP.z - v.w*newP.w;
    pXY = pXY + kVal[1]*nXY;
    pZW = pZW + kVal[6]*nZW;

    vp = vec4(pXY*v.x + kVal[2]*newP.x - kVal[4]*v.y,
              pXY*v.y + kVal[2]*newP.y - kVal[4]*v.x,
              pZW*v.z + kVal[7]*newP.z - kVal[9]*v.w,
              pZW*v.w + kVal[7]*newP.w - kVal[9]*v.z );
}
////////////////////////////////////////////////////////////////////////////
void Hopalong4D::Step(vec4 &v, vec4 &vp) 
{
    auto func = [](const float f, const float k1, const float k2) {
        return (f>0 ? 1 : -1) * sqrt(abs(k1*f-k2));
    };

    vp.x = v.y - func(v.x, kVal[1], kVal[2]);
    vp.y = kVal[0] - v.x;
    vp.z = v.w - func(v.z, kVal[4], kVal[5]);
    vp.w = kVal[3] - v.z;
}


#if !defined(GLAPP_DISABLE_DLA)
////////////////////////////////////////////////////////////////////////////
void dla3D::Step(vec4 &v, vec4 &vp) 
{
    v = vec4(theWnd->getParticlesSystem()->getTMat()->getTrackball().getRotationCenter(), 0.f);
    vp = vec4(AddParticle(), 0.f);
}

void dla3D::buildIndex() // for loaded data
{
    uint32_t id = thisPOINT.size();

    m_JoinAttempts.resize(id);
    memset(m_JoinAttempts.data(), 0, id*sizeof(int));

#ifdef GLAPP_USE_BOOST_LIBRARY
    vec3 *p = thisPOINT.data();
    for(int i=0; i<id; i++, p++)
        m_Index.insert(std::make_pair(BoostPoint(p->x, p->y, p->z), i));
#else
    m_Index->addPoints(0, id-1);
#endif

    attractorsList.continueDLA(false); // reset the flag
    attractorsList.getThreadStep()->startThread();
}
#endif
////////////////////////////////////////////////////////////////////////////
void Lorenz::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.x+dtStepInc*(kVal[0]*(v.y-v.x));
    vp.y = v.y+dtStepInc*(v.x*(kVal[1]-v.z)-v.y);
    vp.z = v.z+dtStepInc*(v.x*v.y-kVal[2]*v.z);
}
////////////////////////////////////////////////////////////////////////////
void ChenCelikovsky::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.x + dtStepInc*(kVal[0]*(v.y-v.x));
    vp.y = v.y + dtStepInc*(-v.x*v.z + kVal[2]*v.y);
    vp.z = v.z + dtStepInc*( v.x*v.y - kVal[1]*v.z);
}
////////////////////////////////////////////////////////////////////////////
void ChenLee::Step(vec4 &v, vec4 &vp) 
{
    vp.x = v.x + dtStepInc*(kVal[0]*v.x - v.y*v.z);
    vp.y = v.y + dtStepInc*(kVal[1]*v.y + v.x*v.z);
    vp.z = v.z + dtStepInc*(kVal[2]*v.z + v.x*v.y/3.f);
}

////////////////////////////////////////////////////////////////////////////
void TSUCS::Step(vec4 &v, vec4 &vp) 
{
    vp.x = v.x + dtStepInc*(kVal[0]*(v.y - v.x) + kVal[3]*v.x*v.z);
    vp.y = v.y + dtStepInc*(kVal[1]*v.x + kVal[5]*v.y - v.x*v.z);
    vp.z = v.z + dtStepInc*(kVal[2]*v.z + v.x*v.y - kVal[4]*v.x*v.x);
}
////////////////////////////////////////////////////////////////////////////
void Aizawa::Step(vec4 &v, vec4 &vp) 
{
    vp.x = v.x + dtStepInc*((v.z-kVal[1])*v.x - kVal[3]*v.y);
    vp.y = v.y + dtStepInc*((v.z-kVal[1])*v.y + kVal[3]*v.x);
    const float xQ = v.x*v.x;
    vp.z = v.z + dtStepInc*(kVal[2] + kVal[0]*v.z - (v.z*v.z*v.z)/3.f - (xQ + v.y*v.y) * (1.f + kVal[4]*v.z) + kVal[5]*v.z*xQ*v.x);
}
////////////////////////////////////////////////////////////////////////////
void YuWang::Step(vec4 &v, vec4 &vp) 
{
    vp.x = v.x + dtStepInc*(kVal[0]*(v.y -v.x));
    vp.y = v.y + dtStepInc*(kVal[1]*v.x - kVal[2]*v.x*v.z);
    vp.z = v.z + dtStepInc*(exp(v.x*v.y) - kVal[3]*v.z);
}
////////////////////////////////////////////////////////////////////////////
void FourWing::Step(vec4 &v, vec4 &vp) 
{
    vp.x = v.x + dtStepInc*(kVal[0]*v.x - kVal[1]*v.y*v.z);
    vp.y = v.y + dtStepInc*(v.x*v.z - kVal[2]*v.y);
    vp.z = v.z + dtStepInc*(kVal[4]*v.x - kVal[3]*v.z + v.x*v.y);
}
////////////////////////////////////////////////////////////////////////////
void FourWing2::Step(vec4 &v, vec4 &vp) 
{
    vp.x = v.x + dtStepInc*(kVal[0]*v.x + kVal[1]*v.y + kVal[2]*v.y*v.z);
    vp.y = v.y + dtStepInc*(kVal[3]*v.y - v.x*v.z);
    vp.z = v.z + dtStepInc*(kVal[4]*v.z + kVal[5]*v.x*v.y);
}
////////////////////////////////////////////////////////////////////////////
void FourWing3::Step(vec4 &v, vec4 &vp) 
{
    vp.x = v.x + dtStepInc*(kVal[0]*v.x + kVal[1]*v.y + kVal[2]*v.y*v.z);
    vp.y = v.y + dtStepInc*(kVal[3]*v.y*v.z - kVal[4]*v.x*v.z);
    vp.z = v.z + dtStepInc*(1.f - kVal[5]*v.z - kVal[6]*v.x*v.y);
}
////////////////////////////////////////////////////////////////////////////
void Thomas::Step(vec4 &v, vec4 &vp) 
{
    vp.x = v.x + dtStepInc*(-kVal[0]*v.x + sinf(v.y));
    vp.y = v.y + dtStepInc*(-kVal[1]*v.y + sinf(v.z));
    vp.z = v.z + dtStepInc*(-kVal[2]*v.z + sinf(v.x));
}
////////////////////////////////////////////////////////////////////////////
void Halvorsen::Step(vec4 &v, vec4 &vp) 
{
    vp.x = v.x + dtStepInc*(-kVal[0]*v.x - 4.f*v.y - 4.f*v.z - v.y*v.y);
    vp.y = v.y + dtStepInc*(-kVal[1]*v.y - 4.f*v.z - 4.f*v.x - v.z*v.z);
    vp.z = v.z + dtStepInc*(-kVal[2]*v.z - 4.f*v.x - 4.f*v.y - v.x*v.x);
}
////////////////////////////////////////////////////////////////////////////
void Arneodo::Step(vec4 &v, vec4 &vp) 
{ // kVal[] -> a,b,c
    vp.x = v.x + dtStepInc*v.y;
    vp.y = v.y + dtStepInc*v.z; 
    vp.z = v.z + dtStepInc*(-kVal[0]*v.x - kVal[1]*v.y - v.z + kVal[2]*v.x*v.x*v.x);
}
////////////////////////////////////////////////////////////////////////////
void Bouali::Step(vec4 &v, vec4 &vp) 
{ // kVal[] -> a,b,c,s,alfa,beta
    vp.x = v.x + dtStepInc*( v.x*(kVal[0] - v.y) + kVal[4]*v.z);
    vp.y = v.y + dtStepInc*(-v.y*(kVal[1] - v.x*v.x));
    vp.z = v.z + dtStepInc*(-v.x*(kVal[2] - kVal[3]*v.z) - kVal[5]*v.z);
}
////////////////////////////////////////////////////////////////////////////
void BrukeShaw::Step(vec4 &v, vec4 &vp)
{ // kVal[] -> a,b,c,s,alfa,beta
    vp.x = v.x + dtStepInc*(-kVal[0] * (v.x + v.y));
    vp.y = v.y + dtStepInc*(-v.y -kVal[0]*v.x*v.z);
    vp.z = v.z + dtStepInc*(kVal[0]*v.x*v.y + kVal[1]);
}
////////////////////////////////////////////////////////////////////////////
void Hadley::Step(vec4 &v, vec4 &vp) 
{ // kVal[] -> a,b,f,g
    vp.x = v.x + dtStepInc*(-v.y*v.y -v.z*v.z -kVal[0]*v.x + kVal[0]*kVal[2]);
    vp.y = v.y + dtStepInc*(v.x*v.y - kVal[1]*v.x*v.z - v.y + kVal[3]);
    vp.z = v.z + dtStepInc*(kVal[1]*v.x*v.y + v.x*v.z - v.z);
}
////////////////////////////////////////////////////////////////////////////
void LiuChen::Step(vec4 &v, vec4 &vp) 
{ // kVal[] -> a,b,c,d,e,f,g
     vp.x = v.x + dtStepInc*(kVal[0]*v.y + kVal[1]*v.x + kVal[2]*v.y*v.z);
     vp.y = v.y + dtStepInc*(kVal[3]*v.y - v.z + kVal[4]*v.x*v.z);
     vp.z = v.z + dtStepInc*(kVal[5]*v.z + kVal[6]*v.x*v.y);
}
////////////////////////////////////////////////////////////////////////////
void GenesioTesi::Step(vec4 &v, vec4 &vp) 
{ // kVal[] -> a,b,c
    vp.x = v.x + dtStepInc*v.y;
    vp.y = v.y + dtStepInc*v.z; 
    vp.z = v.z + dtStepInc*(-kVal[2]*v.x - kVal[1]*v.y - kVal[0]*v.z + v.x*v.x);
}
////////////////////////////////////////////////////////////////////////////
void NewtonLeipnik::Step(vec4 &v, vec4 &vp) 
{ // kVal[] -> a,b
    vp.x = v.x + dtStepInc*(-kVal[0]*v.x + v.y + 10.f*v.y*v.z);
    vp.y = v.y + dtStepInc*(-v.x - 0.4f*v.y + 5.f*v.x*v.z);
    vp.z = v.z + dtStepInc*(kVal[1]*v.z - 5.f*v.x*v.y);
}
////////////////////////////////////////////////////////////////////////////
void NoseHoover::Step(vec4 &v, vec4 &vp) 
{ // kVal[] -> a
    vp.x = v.x + dtStepInc*v.y;
    vp.y = v.y + dtStepInc*(-v.x + v.y*v.z); 
    vp.z = v.z + dtStepInc*(kVal[0] - v.y*v.y);
}
////////////////////////////////////////////////////////////////////////////
void RayleighBenard::Step(vec4 &v, vec4 &vp) 
{ // kVal[] -> a, b, r
    vp.x = v.x + dtStepInc*(-kVal[0]*(v.x - v.y));
    vp.y = v.y + dtStepInc*(kVal[2]*v.x - v.y - v.x*v.z); 
    vp.z = v.z + dtStepInc*(v.x*v.y - kVal[1]*v.z);
}
////////////////////////////////////////////////////////////////////////////
void Sakarya::Step(vec4 &v, vec4 &vp) 
{ // kVal[] -> a, b
    vp.x = v.x + dtStepInc*(-v.x + v.y + v.y*v.z);
    vp.y = v.y + dtStepInc*(-v.x - v.y + kVal[0]*v.x*v.z); 
    vp.z = v.z + dtStepInc*(v.z - kVal[1]*v.x*v.y);
}
////////////////////////////////////////////////////////////////////////////
void Robinson::Step(vec4 &v, vec4 &vp) 
{ // kVal[] -> a, b, c, d, v
    const float x2 = v.x*v.x;
    vp.x = v.x + dtStepInc*v.y;
    vp.y = v.y + dtStepInc*(v.x - 2.f*x2*v.x - kVal[0]*v.y + kVal[1]*x2*v.y - kVal[4]*v.y*v.z); 
    vp.z = v.z + dtStepInc*(-kVal[2]*v.z + kVal[3]*x2);
}
////////////////////////////////////////////////////////////////////////////
void Rossler::Step(vec4 &v, vec4 &vp) 
{ // kVal[] -> a, b, c
    vp.x = v.x + dtStepInc*(-v.y - v.z);
    vp.y = v.y + dtStepInc*(v.x + kVal[0]*v.y); 
    vp.z = v.z + dtStepInc*(kVal[1] + v.z*(v.x - kVal[2]));
}
////////////////////////////////////////////////////////////////////////////
void Rucklidge::Step(vec4 &v, vec4 &vp) 
{ // kVal[] -> a, k
    vp.x = v.x + dtStepInc*(-kVal[1]*v.x + kVal[0]*v.y - v.y*v.z);
    vp.y = v.y + dtStepInc*v.x; 
    vp.z = v.z + dtStepInc*(-v.z + v.y*v.y);
}
////////////////////////////////////////////////////////////////////////////
void DequanLi::Step(vec4 &v, vec4 &vp) 
{ // kVal[] -> a, c, d, e, k, f
    vp.x = v.x + dtStepInc*(kVal[0]*(v.y-v.x) + kVal[2]*v.x*v.z);
    vp.y = v.y + dtStepInc*(kVal[4]*v.x + kVal[5]*v.y - v.x*v.z); 
    vp.z = v.z + dtStepInc*(kVal[1]*v.z + v.x*v.y - kVal[3]*v.x*v.x);
}
////////////////////////////////////////////////////////////////////////////
void MultiChuaII::Step(vec4 &v, vec4 &vp) 
{ // kVal[] -> a, b, m0 -> m5, c1 -> c5
    auto f = [&](const float x) -> float {
        const int startM = 2, startC = 7; float sum = 0;
        for(int i=1; i<=5; i++) sum+= (kVal[startM+(i-1)]-kVal[startM+i])*(fabs(x+kVal[startC+i]) - fabs(x-kVal[startC+i]));
        return kVal[startM+5]*x + sum*.5f;  
    };

    vp.x = v.x + dtStepInc*(kVal[0]*(v.y-f(v.x)));
    vp.y = v.y + dtStepInc*(v.x - v.y + v.z); 
    vp.z = v.z + dtStepInc*(-kVal[1]*v.y);
}
////////////////////////////////////////////////////////////////////////////
void ZhouChen::Step(vec4 &v, vec4 &vp) 
{ // kVal[] -> a, b, c, d, e, 
    vp.x = v.x + dtStepInc*(kVal[0]*v.x + kVal[1]*v.y+ v.y*v.z);
    vp.y = v.y + dtStepInc*(kVal[2]*v.y - v.x*v.z - kVal[3]*v.y*v.z); 
    vp.z = v.z + dtStepInc*(kVal[4]*v.z - v.x*v.y);
}
////////////////////////////////////////////////////////////////////////////
void ShimizuMorioka::Step(vec4 &v, vec4 &vp)
{ // kVal[] -> a
    vp.x = v.x + dtStepInc*v.y;
    vp.y = v.y + dtStepInc*((1.f-v.z)*v.x - kVal[0]*v.y);
    vp.z = v.z + dtStepInc*(v.x*v.x - kVal[1]*v.z);
}
////////////////////////////////////////////////////////////////////////////
void SprottLinzF::Step(vec4 &v, vec4 &vp) 
{ // kVal[] -> a
    vp.x = v.x + dtStepInc*(v.y + v.z);
    vp.y = v.y + dtStepInc*(-v.x + kVal[0]*v.y);
    vp.z = v.z + dtStepInc*(v.x*v.x - v.z);
}
////////////////////////////////////////////////////////////////////////////
void SprottLinzB::Step(vec4 &v, vec4 &vp) 
{ // kVal[] -> a
    vp.x = v.x + dtStepInc* kVal[0]*v.y * v.z;
    vp.y = v.y + dtStepInc*(kVal[1]*v.x - kVal[2]*v.y); 
    vp.z = v.z + dtStepInc*(1 - kVal[3]*v.x*v.y);
}
void Tamari::Step(vec4 &v, vec4 &vp)
{ // kVal[] -> a
    vp.x = v.x + dtStepInc*((v.x - kVal[0]*v.y)*cos(v.z) - kVal[1]*v.y*sin(v.z));
    vp.y = v.y + dtStepInc*((v.x + kVal[2]*v.y)*sin(v.z) + kVal[3]*v.y*cos(v.z));
    vp.z = v.z + dtStepInc*(kVal[4] + kVal[5]*v.z + kVal[6]*atan(((1-kVal[7])*v.y) / ((1-kVal[8])*v.x)));
}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void Coullet::Step(vec4 &v, vec4 &vp) 
{ // kVal[] -> a, b, c, d
    vp.x = v.x + dtStepInc*v.y;
    vp.y = v.y + dtStepInc*v.z; 
    vp.z = v.z + dtStepInc*(kVal[0]*v.x + kVal[1]*v.y + kVal[2]*v.z + kVal[3]*v.x*v.x*v.x);
}
////////////////////////////////////////////////////////////////////////////
void Dadras::Step(vec4 &v, vec4 &vp) 
{ // kVal[] -> p, q, r, s, e
    vp.x = v.x + dtStepInc*(v.y - kVal[0]*v.x + kVal[1]*v.y*v.z);
    vp.y = v.y + dtStepInc*(kVal[2]*v.y - v.x*v.z + v.z); 
    vp.z = v.z + dtStepInc*(kVal[3]*v.x*v.y - kVal[4]*v.z);
}
////////////////////////////////////////////////////////////////////////////
void GloboToroid::Step(vec4 &v, vec4 &vp) 
{ // kVal[] -> p, q, r, s, e
    const float A = kVal[0];
    const float B = kVal[1];
    const float omega = 2.f * T_PI * kVal[2];
    const float x1 = v.x+1.f;
    vp.x = v.x + dtStepInc*(-omega*v.y - A*v.z*x1);
    vp.y = v.y + dtStepInc*omega*v.x;
    vp.z = v.z + dtStepInc*(A*x1*x1 - B);
}

// stochastic adaptation of P.Nylander's Mathematica formula of JuliaBulb set
// http://bugman123.com/Hypercomplex/index.html
////////////////////////////////////////////////////////////////////////////
void juliaBulb_IIM::Step(vec4 &v, vec4 &vp) 
{ // kVal[] -> a, k
    auto radiciEq = [&](const vec3 &p, float sign1, float sign2) {
        const float xQ = p.x * p.x, yQ = p.y * p.y, zQ = p.z * p.z;
        const float r = sqrtf(xQ+yQ+zQ);
        const float a = sign1 * sqrtf(yQ + xQ*(2.f+zQ/(xQ+yQ)) - 2.f*p.x*r);
        const float b = sign2 * sqrtf(r - p.x - a) * .5f;
        const float c = yQ*yQ + xQ * (yQ - zQ);
        const float d = a * (p.x * (r+p.x) + yQ);
        vp = vec4(b * ((p.x*yQ-d) * (xQ+yQ) - p.x*xQ*zQ) / (p.y*c),
                  b,
                  -p.z/sqrtf(2 * (r - d *(xQ+yQ)/c)),
                  0.f);
    };

    preStep(v);
    const uint32_t rnd = fastRand32::xorShift();
    radiciEq((vec3)v-((vec3 &)*kVal.data()+(vec3)kRnd), (rnd&1) ? 1.f : -1.f, (rnd&2) ? 1.f : -1.f);
    //if(depth<10) Step(v, vp);

}
// stochastic adaptation of P.Nylander's Mathematica formula of JuliaBulb set
// http://bugman123.com/Hypercomplex/index.html
////////////////////////////////////////////////////////////////////////////
void juliaBulb4th_IIM::Step(vec4 &v, vec4 &vp) 
{ // kVal[] -> a, k
    
    auto radiciEq = [&](const vec3 &p, int kTheta, int kPhi) {
        const float r = length(p);
        const int absOrder = abs(degreeN);
        const int k1 = (absOrder - (p.z<0 ? 0 : 1)), k2 = (3*absOrder + (p.z<0 ? 4 : 2));

        const int dk = ((absOrder%2)==0 && (kPhi<<2)>k1 && (kPhi<<2)<k2) ? (sign(p.z) * ( (absOrder % 4) ? 1 : -1)) : 0;
        const float theta  = (atan2f(p.y,p.x) + (2 * kTheta + dk) * T_PI) / float(degreeN);
        const float phi    = (asinf(p.z/r)    + (2 * kPhi   - dk) * T_PI) / float(degreeN);
        const float cosphi = cosf(phi);
        vp = vec4(powf(r, 1.0f/float(degreeN)) * vec3(cosf(theta)*cosphi,sinf(theta)*cosphi,sinf(phi)), 0.f);
    };

    preStep(v);
    //const vec3 p(v.z, v.y, v.x);
    radiciEq((vec3)v-((vec3 &)*kVal.data()+(vec3)kRnd), fastRand32::xorShift() % degreeN, fastRand32::xorShift() % degreeN);

}

// stochastic adaptation of P.Nylander's Mathematica formula of quaternion Julia set
// http://bugman123.com/Hypercomplex/index.html
////////////////////////////////////////////////////////////////////////////
void quatJulia_IIM::Step(vec4 &v, vec4 &vp) 
{ // kVal[] -> a, k

    auto radiciEq = [&](const vec4 &p, float sign)
    {
        const float xQ = p.x * p.x, yQ = p.y * p.y, zQ = p.z * p.z, wQ = p.w * p.w;
        const float r = sqrtf(xQ + yQ + zQ + wQ);
        const float a = sqrtf((p.x+r)*.5);
        const float b = (r-p.x) * a / (yQ + zQ + wQ);
        vp = sign * vec4(a, b*p.y, b*p.z, b*p.w);
    };

    preStep(v);
    const uint32_t rnd = fastRand32::xorShift();
    radiciEq(v-((vec4 &)*kVal.data()+kRnd), (rnd&1) ? 1.f : -1.f);

}

/*
void quatJulia_IIM::Step(vec3 &v, vec3 &vp) 
{ // kVal[] -> a, k

    auto radiciEq = [&](const vec4 &p, float sign)
    {
        const float xQ = p.x * p.x, yQ = p.y * p.y, zQ = p.z * p.z, wQ = p.w * p.w;
        const float r = sqrtf(xQ + yQ + zQ + wQ);
        const float a = sqrtf((p.x+r)*.5);
        const float b = (r-p.x) * a / (yQ + zQ + wQ);
        vIter = sign * vec4(a, b*p.y, b*p.z, b*p.w);
    };

    preStep(v,vp);
    vIter = vec4(v, last4D);

    while(depth++<maxDepth) {
        const int rnd = Random::get<int>(int(0),int(INT_MAX));
        radiciEq(vIter-((vec4 &)*kVal.data()+kRnd), (rnd&1) ? 1.f : -1.f);
    };
    
    vp = vec3(vIter); last4D = vIter.w;

}
*/

// stochastic adaptation of P.Nylander's Mathematica formula of quaternion Julia set
// http://bugman123.com/Hypercomplex/index.html
////////////////////////////////////////////////////////////////////////////
void glynnJB_IIM::Step(vec4 &v, vec4 &vp) 
{
//Glynn roots: {x,y,z}^(1/1.5) = {x,y,z}^(2/3)

    const vec3 p = vec3(v)-(vec3(kVal[0], kVal[1], kVal[2])+vec3(kRnd)); //    ((vec3 &)*kVal.data()+(vec3)kRnd);
    const float xQ = p.x * p.x, yQ = p.y * p.y, zQ = p.z * p.z;
    const float r = sqrtf(xQ+yQ+zQ);
    const uint32_t rnd = fastRandom.KISS();
    const uint32_t rndR = fastRandom.KISS();
    const int px = rnd&0x10;
    const int py = rnd&0x20;
    const int pz = rnd&0x40;
    const int pw = rnd&0x80;
    //const bool isCone = bool(zQ > xQ + yQ);
    const int isCone = rnd&1;
/*
    auto powN = [&] (vec3 &p, float n) -> vec3 {
        const float theta = n * atan2f(p.x, p.y);
        const float phi = n * asinf(p.z/r);
        return pow(vec3(r), r*vec3(cosf(theta) * cosf(phi), sinf(theta) * cosf(phi), sinf(phi)));
    };
*/
    auto numRadici = [&] () -> int {
        return (isCone) ? rndR%3 : (px ? rndR%2 : 0);
    };

    auto radiciEq = [&] (int k) {
        const float n = kVal[3];

        float ktheta, kphi;
        const float uno = kVal[4], due = kVal[5], zeroCinque = kVal[6], dueCinque = kVal[7];

        if(!k) { ktheta = kphi = 0.0; }
        else { 
            if(isCone) { ktheta=(py ? due:uno); kphi=0.0; } 
            else {
                if(k==1) {
                    if(pw) {ktheta=due; kphi=0.0;} else {ktheta=zeroCinque; kphi=(pz?zeroCinque:dueCinque);}
                }
                else {
                    if(pw) {ktheta=uno; kphi=0.0;} else {ktheta=dueCinque; kphi=(pz?zeroCinque:dueCinque);}
                }
            }
        }

        const float theta  = (atan2f(p.y,p.x)+ ktheta * T_PI)/n;
        const float phi    = (asinf(p.z/(r == 0.f ? FLT_EPSILON : r))   + kphi   * T_PI)/n;

        const float cosphi = cosf(phi);
        vp = vec4(powf(r,1.0f/n) * vec3(cosf(theta)*cosphi,sinf(theta)*cosphi,sinf(phi)), 0.f);
    };
    


    preStep(v);
    int nRad = numRadici();
    radiciEq( nRad);

}


/*
void glynnJB_IIM::Step(vec4 &v, vec4 &vp) 
{
//Glynn roots: {x,y,z}^(1/1.5) = {x,y,z}^(2/3)

    const vec3 p = vec3(v)-(vec3(kVal[0], kVal[1], kVal[2])+vec3(kRnd)); //    ((vec3 &)*kVal.data()+(vec3)kRnd);
    const float xQ = p.x * p.x, yQ = p.y * p.y, zQ = p.z * p.z;
    const float r = sqrtf(xQ+yQ+zQ);
    const bool isCone = bool(zQ > xQ + yQ);

    auto powN = [&] (vec3 &p, float n) -> vec3 {
        const float theta = n * atan2f(p.x, p.y);
        const float phi = n * asinf(p.z/r);
        return pow(vec3(r), r*vec3(cosf(theta) * cosf(phi), sinf(theta) * cosf(phi), sinf(phi)));
    };

    auto numRadici = [&] () -> int {
        const uint32_t rnd = fastRandom.KISS();
        return (isCone) ? rnd%3 : (p.x > 0 ? rnd%2 : 0);
    };

    auto radiciEq = [&] (int k) {
        const float n = 1.5f;
        const uint32_t rnd = fastRandom.KISS();

        float ktheta, kphi;

        if(!k) { ktheta = kphi = 0.0; }
        else { 
            if(!isCone) { ktheta=(p.y<0 ? 2.0:1.0); kphi=0.0; } 
            else {
                if(k==1) {
                    if(p.x<0 && p.y<0) {ktheta=2.0; kphi=0.0;} else {ktheta=0.5; kphi=(p.z>0?0.5:2.5);}
                }
                else {
                    if(p.x<0 && p.y>0) {ktheta=1.0; kphi=0.0;} else {ktheta=2.5; kphi=(p.z>0?0.5:2.5);}
                }
            }
        }

        const float theta  = (atan2f(p.y,p.x)+ ktheta * T_PI)/n;
        const float phi    = (asinf(p.z/(r == 0.f ? FLT_EPSILON : r))   + kphi   * T_PI)/n;

        const float cosphi = cosf(phi);
        vp = vec4(powf(r,1.0f/n) * vec3(cosf(theta)*cosphi,sinf(theta)*cosphi,sinf(phi)), 0.f);
    };
    

    preStep(v);
    int nRad = numRadici();
    const uint32_t rnd = fastRandom.KISS();
    radiciEq( nRad);

}
*/



////////////////////////////////////////////////////////////////////////////
void Magnetic::Step(vec4 &v, vec4 &vp) 
{
    vec3 vt(0.f);

    int i = 0;
    for(auto itPt = vVal.begin(), itK = kVal.begin(); !newItemsEnd && itPt!=vVal.end();) {
        const vec3 vo(*itPt++ - v); //vo.z=0.0;                
        const float dotProd = dot(vo,vo);
        if(dotProd > FLT_EPSILON) vt+=(this->*increment)(vec3(vec3(*itK++) * (vo / dotProd)),i++);
    }

    vp = vec4(vt, 0.f);
}

const vec3 Magnetic::straight(const vec3 &vx, int i) 
{
    return vx;
}

const vec3 Magnetic::rightShift(const vec3 &vx, int i) 
{
    switch(i%3) {
        case 0 :  return vx;
        case 1 :  return vec3(vx.z,vx.x,vx.y);
        case 2 :  return vec3(vx.y,vx.z,vx.x); 
    }

    return vx;
}

const vec3 Magnetic::leftShift(const vec3 &vx, int i) 
{
    switch(i%3) {
        case 0 :  return vx;
        case 1 :  return vec3(vx.y,vx.z,vx.x);
        case 2 :  return vec3(vx.z,vx.x,vx.y); 
    }
    return vx;
}
const vec3 Magnetic::fullPermutated(const vec3 &vx, int i) 
{
    switch(i%6) {
        case 0 :  return vx;
        case 1 :  return vec3(vx.y,vx.z,vx.x);
        case 2 :  return vec3(vx.z,vx.x,vx.y); 
        case 3 :  return vec3(vx.x,vx.z,vx.y);
        case 4 :  return vec3(vx.z,vx.y,vx.x);
        case 5 :  return vec3(vx.y,vx.x,vx.z); 
    }
    return vx;
}

const vec3 Magnetic::tryed(const vec3 &vx, int i) 
{
    switch(i%3) {
        case 0 :  return vx;
        case 1 :  return vec3(vx.x,sinf(vx.y/T_PI),cosf(vx.z/pi<float>()));
        case 2 :  return vec3(vx.x,cosf(vx.y/T_PI),sinf(vx.z/pi<float>())); 
    }
    return vx;
}



//  Attractors Thread helper class
////////////////////////////////////////////////////////////////////////////
void threadStepClass::newThread()
{
#if !defined(GLCHAOSP_LIGHTVER)
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
#if !defined(GLCHAOSP_LIGHTVER)
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
#if !defined(GLCHAOSP_LIGHTVER)
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
#if !defined(GLCHAOSP_LIGHTVER)
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
    theApp->loadAttractor(getFileName().c_str());
#ifdef GLCHAOSP_LIGHTVER
    theApp->setLastFile(getFileName()); //to reload invert settings
#endif
    checkCorrectEmitter();
    getThreadStep()->restartEmitter();
    get()->initStep();
    getThreadStep()->startThread();

}

void AttractorsClass::checkCorrectEmitter()
{
    if(get()->dtType()) {
        if(tfSettinsClass::tfMode() && theApp->getEmitterEngineType() == enumEmitterEngine::emitterEngine_staticParticles)
            theWnd->getParticlesSystem()->changeEmitter(enumEmitterEngine::emitterEngine_transformFeedback);
        else if(!tfSettinsClass::tfMode() && theApp->getEmitterEngineType() == enumEmitterEngine::emitterEngine_transformFeedback)
            theWnd->getParticlesSystem()->changeEmitter(enumEmitterEngine::emitterEngine_staticParticles);

    } else {
        if(theApp->getEmitterEngineType()!=enumEmitterEngine::emitterEngine_staticParticles)
            theWnd->getParticlesSystem()->changeEmitter(enumEmitterEngine::emitterEngine_staticParticles);
    }

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
