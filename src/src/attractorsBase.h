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

#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <iostream>
#include <fstream>
#include <deque>
#include <string>
#include <complex>

#include <thread>
#include <mutex>
#include <condition_variable>

#include <configuru/configuru.hpp>

#include "IFS.h"
#include "tools/glslProgramObject.h"

using namespace std;
using namespace configuru;
using namespace fastPRNG;

extern fastRand32 fastRandom;
#define RANDOM(MIN, MAX) (fastRandom.KISS_Range<float>(MIN, MAX))


#define BUFFER_DIM 200
#define STABILIZE_DIM 1500

class attractorDlgClass;
class AttractorsClass;
class emitterBaseClass;
class transformedEmitterClass;

//void drawMinMaxButtons(float wDn, float border);

//  Attractor base class
////////////////////////////////////////////////////////////////////////////
class AttractorBase
{
public:

    typedef void (AttractorBase::*stepPtrFn)(vec4 &v, vec4 &vp);

    enum { attLoadPtVal, attLoadKtVal };
    enum { attHaveKVect, attHaveKScalar }; 
    enum { attPt3D = 3, attPt4D };

    AttractorBase() { AttractorBase::resetQueue(); }
    virtual ~AttractorBase() {}

    virtual void resetQueue();

    virtual void initStep() = 0;
    virtual void newRandomValues() = 0;

    virtual void searchAttractor() {};

    virtual void saveVals(const char *name) {}
    virtual void loadVals(const char *name) {}

    virtual void saveVals(Config &cfg);
    virtual void loadVals(Config &cfg);

    virtual void saveAdditionalData(Config &cfg) {}
    virtual void loadAdditionalData(Config &cfg) {}

    virtual int additionalDataDlg();
    virtual void additionalDataCtrls();

    virtual void saveKVals(Config &cfg) = 0;
    virtual void loadKVals(Config &cfg) = 0;

    virtual void startData() = 0;

    virtual float getValue(int i, int type) = 0;
    virtual float getValue(int x, int y, int type) = 0; 
    virtual void setValue(int i, int type, float val) = 0; 
    virtual void setValue(int x, int y, int type, float val) = 0; 
    virtual int getNumElements(int type) = 0; //return # rows

    virtual int getKType() = 0;

    virtual float getDtStepInc() { return 0.0; }
    virtual void setDtStepInc(float f) { }

    virtual ifsBaseClass *getIFSParam() { return nullptr; }
    virtual ifsBaseClass *getIFSPoint() { return nullptr; }

    virtual inline float colorFunc(const vec4 &v, const vec4 &vp) { return distance((vec3)v, (vec3)vp); }

    //thread Step with shared GPU memory
    void Step(float *&ptr, vec4 &v, vec4 &vp);
    void StepAsync(float *&ptr, vec4 &v, vec4 &vp);
    //single step
    virtual void Step();
    //buffered Step
    virtual uint32_t Step(float *ptr, uint32_t numElements);
    //attractor step algorithm
    virtual void Step(vec4 &v, vec4 &vp) = 0;

    void stabilize(int samples) { for(int i = samples; i>0; i--) Step(); }

    vec4& getCurrent()  { return stepQueue.front(); }
    vec4& getPrevious() { return stepQueue[1]; }
    vec4& getAt(int i)  { return stepQueue[i]; }
    float getQueueSize() { return stepQueue.size(); }
    void Insert(const vec4 &vect)
    {
        stepQueue.push_front(vect);
        stepQueue.pop_back();
    }

    int getMagnetSize() { return vVal.size(); }

    vec3& getPOV() { return m_POV; }
    vec3& getTGT() { return m_TGT; }

    void setKMax(float v) { kMax = v; }
    void setKMin(float v) { kMin = v; }
    void setVMax(float v) { vMax = v; }
    void setVMin(float v) { vMin = v; }

    float getKMax() { return kMax; }
    float getKMin() { return kMin; }
    float getVMax() { return vMax; }
    float getVMin() { return vMin; }

    string& getDisplayName() { return displayName; }
    string& getGraphChar() { return graphChar; }
    vec4& getColorGraphChar() { return colorGraphChar; }
    void setDisplayName(string &name) { displayName = name; }

    string& getNameID() { return nameID; }

    void setBufferRendered() {  bufferRendered = true; }

    bool dlgAdditionalDataVisible() { return bDlgAdditionalDataVisible; }
    void dlgAdditionalDataVisible(bool b) { bDlgAdditionalDataVisible=b; }

    bool dtType() { return attractorType == dtTpo; }
    bool dlaType() { return attractorType == dlaTpo; }
    bool fractalType() { return attractorType == fractalTpo; }

    void setDTType() { attractorType = dtTpo; }
    void setDLAType() { attractorType = dlaTpo; }
    void setFractalType() { attractorType = fractalTpo; }

    virtual int getPtSize() { return attPt3D; }

    float getInputKMin() { return inputKMin; }
    float getInputKMax() { return inputKMax; }
    float getInputVMin() { return inputVMin; }
    float getInputVMax() { return inputVMax; }

    vector<vec4> vVal;

// QuadRays
// http://www.grunch.net/synergetics/quadintro.html
    vec3 quad2xyz(const vec4 &quad) {
        const float r2 = 0.7071067811865475244; //1.f/sqrt(2.f);
        return vec3(r2 * (quad.x - quad.y - quad.z + quad.w),
                    r2 * (quad.x - quad.y + quad.z - quad.w),
                    r2 * (quad.x + quad.y - quad.z - quad.w));
    }
    vec4 xyz2quad(const vec3 p) {
        const float r2 = 0.7071067811865475244; //1.f/sqrt(2.f);
        vec4 quad(r2 * (p.x>=0 ? p.x :   0 + p.y>=0 ? p.y :   0 + p.z>=0 ? p.z :   0),
                  r2 * (p.x>=0 ?   0 :-p.x + p.y>=0 ?   0 :-p.y + p.z>=0 ? p.z :   0),
                  r2 * (p.x>=0 ?   0 :-p.x + p.y>=0 ? p.y :   0 + p.z>=0 ?   0 :-p.z),
                  r2 * (p.x>=0 ? p.x :   0 + p.y>=0 ?   0 :-p.y + p.z>=0 ?   0 :-p.z));
        const float minVal = std::min<float>(quad.x, std::min<float>(quad.y, std::min<float>(quad.z, quad.w)));
        for(int i=3; i>=0; i--) quad[i]-=minVal;
        return quad;
    }

protected:
    enum aType { genericTpo, dtTpo, dlaTpo, fractalTpo };
    void searchLyapunov();

    //innerThreadStepPtrFn innerThreadStepFn;
    stepPtrFn stepFn;

#ifdef GLCHAOS_USES_SEMPLIFED_QUEUE
    vec4 queueCurrent = vec4(0.f), queuePrevious  = vec4(0.f);
#else
    deque<vec4> stepQueue;
#endif

    // limit random generators
    float kMax = 1.f, kMin = -1.f, vMax = 1.f, vMin = -1.f;

    bool bDlgAdditionalDataVisible = false;
    

    vec3 m_POV = vec3(0.f, 0.f, 10.f), m_TGT = vec3(0.f);

    string displayName, fileName, nameID, graphChar;
    vec4 colorGraphChar;

    friend class attractorDlgClass;
    friend class AttractorsClass;
    bool bufferRendered = false;

    bool flagFileData = false;

    aType attractorType = genericTpo;

    float inputKMin = 0.0, inputKMax = 0.0;
    float inputVMin = 0.0, inputVMax = 0.0;
private:

    friend void drawMinMaxButtons();
};

//--------------------------------------------------------------------------
//  Attractors class with scalar K coeff
//--------------------------------------------------------------------------
class attractorScalarK : public AttractorBase
{
public:

    //  Virtual funcs... overridable for exceptions
    ///////////////////////////////////////
    virtual void initStep() {
        resetQueue();
        Insert(vVal[0]);
        stabilize(STABILIZE_DIM);
    }

    virtual void newRandomValues() 
    {
            for (int i=0; i<kVal.size(); i++) kVal[i] = RANDOM(kMin,kMax);
            vVal[0] = vec4(RANDOM(vMin,vMax),RANDOM(vMin,vMax),RANDOM(vMin,vMax),RANDOM(vMin,vMax));
    }

    void clear() { kVal.clear(); vVal.clear(); }

    float getKParam(int i) { return kVal[i]; }

    void saveKVals(Config &cfg);
    void loadKVals(Config &cfg);

    float getValue(int i, int type)                  { return type ? kVal[i] : vVal[0][i]; }
    float getValue(int x, int y, int type)           { return type ? kVal[x] : vVal[x][y]; }
    void setValue(int i, int type, float val)        { type ? kVal[i]=val : vVal[0][i]=val; }
    void setValue(int x, int y, int type, float val) { type ? kVal[x]=val : vVal[x][y]=val; }

    int getNumElements(int type) {  return (type) ? kVal.size () : vVal.size(); } //return # rows


    int getKSize() { return kVal.size(); }
    int getKType() { return attHaveKScalar; }

protected:
    vector<float> kVal;

};

//--------------------------------------------------------------------------
//  Attractors class with vector K coeff
//--------------------------------------------------------------------------
class attractorVectorK : public AttractorBase
{
public:

    //  Virtual funcs... overridable for exceptions
    ///////////////////////////////////////
    virtual void initStep() {
        resetQueue();
        Insert(vVal[0]);
        stabilize(STABILIZE_DIM);
    }
    virtual void newRandomValues() 
    {
            for (auto &i : kVal) i = vec4(RANDOM(kMin,kMax),RANDOM(kMin,kMax),RANDOM(kMin,kMax),RANDOM(vMin,vMax));
            vVal[0] = vec4(RANDOM(vMin,vMax),RANDOM(vMin,vMax),RANDOM(vMin,vMax),RANDOM(vMin,vMax));
    }

    void clear() { kVal.clear(); vVal.clear(); }

    vec4& getKParam(int i) { return kVal[i]; }

    void saveKVals(Config &cfg);
    void loadKVals(Config &cfg);

    virtual float getValue(int i, int type) { return type ? kVal[0][i] : vVal[0][i]; }
    virtual float getValue(int row, int col, int type) { return type ? kVal[row][col] : vVal[row][col]; }
    virtual void setValue(int i, int type, float val) { 
        if (type) kVal[0][i]=val;
        else      vVal[0][i]=val;
    }
    virtual void setValue(int row, int col, int type, float val) { 
        if (type) kVal[row][col]=val;
        else      vVal[row][col]=val;
    }

    int getNumElements(int type) {  return (type) ? kVal.size () : vVal.size(); } //return # rows


    int getKSize() { return kVal.size(); }
    int getKType() { return attHaveKVect; }

protected:
    vector<vec4> kVal;

};


//--------------------------------------------------------------------------
//  Volumetric Fractals
//--------------------------------------------------------------------------
class volumetricFractals : public attractorScalarK
{
//enum dotPlot { all, skipConvergent, skipDivergent };
public:
    volumetricFractals() {
        vMin = -1.5; vMax = 1.5; kMin = -1.0; kMax = 1.0;
        m_POV = vec3( 0.f, 0, 7.f);
        stepFn = (stepPtrFn) &volumetricFractals::Step;
    }

    virtual inline float colorFunc(const vec4 &v, const vec4 &vp) { return outColor; }
    virtual void additionalDataCtrls();

    inline const vec4 random3Dpoint() {
        return vec4(fastPrng64.xoshiro256p_Range(sMin.x,sMax.x),
                    fastPrng64.xoshiro256p_Range(sMin.y,sMax.y),
                    fastPrng64.xoshiro256p_Range(sMin.z,sMax.z), 0.f);
    }

    virtual float innerStep(vec4 &v, vec4 &vp, const vec4 &c) = 0;

protected:
    void Step(vec4 &v, vec4 &vp);

    int maxIter = 256;
    float upperLimit = 64.f;
    float outColor = 0;
    vec3 sMin = vec3(-1.5f), sMax = vec3(1.5);
    ivec2 plotRange = ivec2(12, 256);

};

class volBedouin : public volumetricFractals
{
public:
    inline float innerStep(vec4 &v, vec4 &vp, const vec4 &p0) {
        vp.x = v.x*v.x - v.y*v.y - v.z*v.z + sin(p0.x);
        vp.y = 2.f*v.x*v.z + sin(p0.y);
        vp.z = 2.f*v.x*v.y + sin(p0.z);
        v = vp;
        return dot(vec3(vp), vec3(vp));
    }

protected:
    void startData();
};

class volRealMandel : public volumetricFractals
{
public:
    inline float innerStep(vec4 &v, vec4 &vp, const vec4 &p0) {
        vp.x = v.x*v.x - v.y*v.y - v.z*v.z + p0.x;
        vp.y = 2.f*v.x*v.y + p0.y;
        vp.z = 2.f*v.x*v.z + p0.z;
        v = vp;
        return dot(vec3(vp), vec3(vp));
    }

protected:
    void startData();
};


class volSinRealMandel : public volumetricFractals
{
public:
    inline float innerStep(vec4 &v, vec4 &vp, const vec4 &p0) {
        vp.x = v.x*v.x + 2.f*v.y*v.z + sin(p0.x);
        vp.y = v.z*v.z + 2.f*v.x*v.y + sin(p0.y);
        vp.z = v.y*v.y + 2.f*v.x*v.z + sin(p0.z);
        v = vp;
        return dot(vec3(vp), vec3(vp));
    }

protected:
    void startData();
};

class volQuatJulia : public volumetricFractals
{
public:
    inline float innerStep(vec4 &v, vec4 &vp, const vec4 &p0) {
        vp.x = v.x*v.x - v.y*v.y - v.z*v.z - v.w*v.w;
        vp.y = 2.f*v.x*v.y;
        vp.z = 2.f*v.x*v.z;
        vp.w = 2.f*v.x*v.w;
        v = vp + *((vec4 *)kVal.data());
        return dot(v, v);
    }

    int getPtSize() { return attPt4D; }

protected:
    void startData();
private:

};

//--------------------------------------------------------------------------
//  IIM Fractals
//--------------------------------------------------------------------------
#include "attractorsFractalsIIM.h"

//--------------------------------------------------------------------------
//  Poynomial classes
//--------------------------------------------------------------------------
#include "attractorsPolynomial.h"

//--------------------------------------------------------------------------
//  Trigonometric class
//--------------------------------------------------------------------------
#include "attractorsTrigonom.h"

//--------------------------------------------------------------------------
//  2D attractors transformed in 3D/4D
//--------------------------------------------------------------------------
#include "attractors2DTransf.h"

//--------------------------------------------------------------------------
//  Rampe class
//--------------------------------------------------------------------------
#include "attractorsRampe.h"

//--------------------------------------------------------------------------
//  DLA3D class
//--------------------------------------------------------------------------
#include "attractorsDLA3D.h"

//--------------------------------------------------------------------------
//  dp/dt class
//--------------------------------------------------------------------------
#include "attractorsDiffEq.h"

//--------------------------------------------------------------------------
//  Magnetic class
//--------------------------------------------------------------------------
#include "attractorsMagnetic.h"


//--------------------------------------------------------------------------
//  IFS class
//--------------------------------------------------------------------------
class tetrahedronGaussMap : public attractorVectorK
{
public:
    tetrahedronGaussMap() {
        vMin = 0.0; vMax = 0.0; kMin = -1.0; kMax = 1.0;
        m_POV = vec3( 0.f, 0, 7.f);
        stepFn = (stepPtrFn) &tetrahedronGaussMap::Step;
    }

protected:
    void Step(vec4 &v, vec4 &vp);
    void startData();
};



//#define GLCHAOSP_TEST_RANDOM_DISTRIBUTION
#ifdef GLCHAOSP_TEST_RANDOM_DISTRIBUTION
class testPRNGbaseClass : public attractorScalarK
{
public:
    //testPRNGbaseClass() {}
    fastXS64 fRnd64;
    fastXS32 fRnd32;
    fastRand32 ff32;
    fastRand64 ff64;
};

#define PRNG(A, func)\
class A : public testPRNGbaseClass {\
    public:\
        A() { stepFn = (stepPtrFn) &A::Step; }\
    protected:\
        void Step(vec4 &v, vec4 &vp) { vp = vec4(func(), func(), func(), 0.f); }\
        void startData() { vVal.push_back(vec4(0.f)); kVal.push_back(0.f); Insert(vVal[0]); }\
};

//#define TEST_UNI_FUNC
#ifdef TEST_UNI_FUNC // test [0,1] funcs
PRNG(xoshiro256p   , fRnd64.    xoshiro256p_UNI  <float>)
PRNG(xoroshiro128p , fRnd64.    xoroshiro128p_UNI<float>)
PRNG(marsKiss64    , ff64.      kiss_UNI         <float>)
PRNG(xorShft64     , fRnd64.    xorShift_UNI     <float>)
PRNG(xoshiro256ps  , fastXS64s::xoshiro256p_UNI  <float>)
PRNG(xoroshiro128ps, fastXS64s::xoroshiro128p_UNI<float>)
PRNG(xorShft64s    , fastXS64s::xorShift_UNI     <float>)

PRNG(xoshiro128p   , fRnd32.    xoshiro128p_UNI <float>)
PRNG(xoroshiro64x  , fRnd32.    xoroshiro64x_UNI<float>)
PRNG(marsKiss32    , ff32.      kiss_UNI        <float>)
PRNG(xorShft32     , fRnd32.    xorShift_UNI    <float>)
PRNG(xoshiro128ps  , fastXS32s::xoshiro128p_UNI <float>)
PRNG(xoroshiro64xs , fastXS32s::xoroshiro64x_UNI<float>)
PRNG(xorShft32s    , fastXS32s::xorShift_UNI    <float>)
#else
PRNG(xoshiro256p   , fRnd64.    xoshiro256p_VNI  <float>)
PRNG(xoroshiro128p , fRnd64.    xoroshiro128p_VNI<float>)
PRNG(marsKiss64    , ff64.      KISS_VNI         <float>)
PRNG(xorShft64     , fRnd64.    xorShift_VNI     <float>)
PRNG(xoshiro256ps  , fastXS64s::xoshiro256p_VNI  <float>)
PRNG(xoroshiro128ps, fastXS64s::xoroshiro128p_VNI<float>)
PRNG(xorShft64s    , fastXS64s::xorShift_VNI     <float>)

PRNG(xoshiro128p   , fRnd32.    xoshiro128p_VNI <float>)
PRNG(xoroshiro64x  , fRnd32.    xoroshiro64x_VNI<float>)
PRNG(marsKiss32    , ff32.      KISS_VNI        <float>)
PRNG(xorShft32     , fRnd32.    xorShift_VNI    <float>)
PRNG(xoshiro128ps  , fastXS32s::xoshiro128p_VNI <float>)
PRNG(xoroshiro64xs , fastXS32s::xoroshiro64x_VNI<float>)
PRNG(xorShft32s    , fastXS32s::xorShift_VNI    <float>)
#endif

#undef PRNG
#endif

//  Attractors Thread helper class
////////////////////////////////////////////////////////////////////////////
class threadStepClass
{
public:
    threadStepClass(emitterBaseClass *e) : emitter(e) { newThread(); }
    ~threadStepClass() { deleteThread(); }

    void newThread();
    void deleteThread();
    void startThread(bool startOn=true);

    thread *getThread() { return attractorLoop; }
    emitterBaseClass *getEmitter() { return emitter; }

    void stopThread();
    void restartEmitter();

    void notify();

private:
    emitterBaseClass *emitter = nullptr;
    thread *attractorLoop = nullptr;

};


//  Attractor Class container
////////////////////////////////////////////////////////////////////////////
#define ATT_EXT ".sca"

#define MAGNETIC_COLOR vec4(0.00f, 1.00f, 0.00f, 1.00f)
#define POLINOM_COLOR  vec4(0.00f, 0.00f, 1.00f, 1.00f)
#define POWER3D_COLOR  vec4(0.00f, 0.50f, 1.00f, 1.00f)
#define RAMPE_COLOR    vec4(1.00f, 0.00f, 1.00f, 1.00f)
#define PICKOVER_COLOR vec4(0.00f, 1.00f, 1.00f, 1.00f)
#define PORTED3D_COLOR vec4(1.00f, 0.00f, 0.00f, 1.00f)
#define DLA3D_COLOR    vec4(0.00f, 0.70f, 0.00f, 1.00f)
#define DT_COLOR       vec4(1.00f, 1.00f, 0.00f, 1.00f)
#define FRACTAL_COLOR  vec4(1.00f, 0.50f, 0.00f, 1.00f)
#define VOLFRAC_COLOR  vec4(0.50f, 0.00f, 1.00f, 1.00f)
#define IFS_COLOR      vec4(1.00f, 1.00f, 1.00f, 1.00f)

class AttractorsClass 
{
public:
    AttractorsClass() {
#ifdef GLCHAOSP_TEST_RANDOM_DISTRIBUTION
#define PB(ATT, GRAPH_CHAR, COLOR_GRAPH_CHAR, DISPLAY_NAME)\
    ptr.push_back(new ATT());\
    ptr.back()->fileName = ("rndData/" "randomTest.sca");\
    ptr.back()->nameID = (#ATT);\
    ptr.back()->graphChar = (GRAPH_CHAR);\
    ptr.back()->colorGraphChar = (COLOR_GRAPH_CHAR);\
    ptr.back()->displayName = (DISPLAY_NAME);

        PB(xoshiro256p       , u8"\uf0da", IFS_COLOR     , "64bit xoshiro256+"       )
        PB(xoroshiro128p     , u8"\uf0da", IFS_COLOR     , "64bit xoroshiro128+"     )
        PB(xorShft64         , u8"\uf0da", IFS_COLOR     , "64bit xorShift"          )
        PB(xoshiro256ps      , u8"\uf0da", DT_COLOR      , "64bit static xoshiro256+")
        PB(xoroshiro128ps    , u8"\uf0da", DT_COLOR      , "64bit static xoroshiro128+")
        PB(xorShft64s        , u8"\uf0da", DT_COLOR      , "64bit static xorShift"   )
        PB(marsKiss64        , u8"\uf0da", POLINOM_COLOR , "64bit Marsaglia Kiss"    )

        PB(xoshiro128p       , u8"\uf0da", PORTED3D_COLOR, "32bit xoshiro128+"       )
        PB(xoroshiro64x      , u8"\uf0da", PORTED3D_COLOR, "32bit xoroshiro64x"      )
        PB(xorShft32         , u8"\uf0da", PORTED3D_COLOR, "32bit xorShift"          )
        PB(xoshiro128ps      , u8"\uf0da", FRACTAL_COLOR , "32bit static xoshiro128p")
        PB(xoroshiro64xs     , u8"\uf0da", FRACTAL_COLOR , "32bit static xoroshiro64x")
        PB(xorShft32s        , u8"\uf0da", FRACTAL_COLOR , "32bit static xorShift"   )
        PB(marsKiss32        , u8"\uf0da", PICKOVER_COLOR, "32bit Marsaglia Kiss"    )
#else
#define ATT_PATH "startData/"
#define PB(ATT, GRAPH_CHAR, COLOR_GRAPH_CHAR, DISPLAY_NAME)\
    ptr.push_back(new ATT());\
    ptr.back()->fileName = (ATT_PATH #ATT ATT_EXT);\
    ptr.back()->nameID = (#ATT);\
    ptr.back()->graphChar = (GRAPH_CHAR);\
    ptr.back()->colorGraphChar = (COLOR_GRAPH_CHAR);\
    ptr.back()->displayName = (DISPLAY_NAME);

        PB(MagneticRight      , u8"\uf0da", MAGNETIC_COLOR, "MagneticRight"      )
        PB(MagneticLeft       , u8"\uf0da", MAGNETIC_COLOR, "MagneticLeft"       )
        PB(MagneticFull       , u8"\uf0da", MAGNETIC_COLOR, "MagneticFull"       )
        PB(MagneticStraight   , u8"\uf0da", MAGNETIC_COLOR, "MagneticStraight"   )
        PB(PolynomialA        , u8"\uf0da", POLINOM_COLOR , "Polynom A"          )
        PB(PolynomialB        , u8"\uf0da", POLINOM_COLOR , "Polynom B"          )
        PB(PolynomialC        , u8"\uf0da", POLINOM_COLOR , "Polynom C"          )
        PB(PolynomialABS      , u8"\uf0da", POLINOM_COLOR , "Polynom Abs"        )
        PB(PolynomialPow      , u8"\uf0da", POLINOM_COLOR , "Polynom Pow"        )
        PB(PolynomialSin      , u8"\uf0da", POLINOM_COLOR , "Polynom Sin"        )
        PB(PowerN3D           , u8"\uf0da", POWER3D_COLOR , "Polynom N-order"    )
        PB(Rampe01            , u8"\uf0da", RAMPE_COLOR   , "Rampe01"            )
        PB(Rampe02            , u8"\uf0da", RAMPE_COLOR   , "Rampe02"            )
        PB(Rampe03            , u8"\uf0da", RAMPE_COLOR   , "Rampe03"            )
        PB(Rampe03A           , u8"\uf0da", RAMPE_COLOR   , "Rampe03 mod."       )
        PB(Rampe04            , u8"\uf0da", RAMPE_COLOR   , "Rampe04"            )
        PB(Rampe05            , u8"\uf0da", RAMPE_COLOR   , "Rampe05"            )
        PB(Rampe06            , u8"\uf0da", RAMPE_COLOR   , "Rampe06"            )
        PB(Rampe07            , u8"\uf0da", RAMPE_COLOR   , "Rampe07"            )
        PB(Rampe08            , u8"\uf0da", RAMPE_COLOR   , "Rampe08"            )
        PB(Rampe09            , u8"\uf0da", RAMPE_COLOR   , "Rampe09"            )
        PB(Rampe10            , u8"\uf0da", RAMPE_COLOR   , "Rampe10"            )
        PB(KingsDream         , u8"\uf0da", PICKOVER_COLOR, "King's Dream"       )
        PB(Pickover           , u8"\uf0da", PICKOVER_COLOR, "Pickover"           )
        PB(SinCos             , u8"\uf0da", PICKOVER_COLOR, "Sin Cos"            )
        PB(Hopalong4D         , u8"\uf0da", PORTED3D_COLOR, "Hopalong4D"         )
        PB(Martin4D           , u8"\uf0da", PORTED3D_COLOR, "Martin4D ss"        )
        PB(Martin4Dsc         , u8"\uf0da", PORTED3D_COLOR, "Martin4D sc"        )
        PB(Martin4Dcc         , u8"\uf0da", PORTED3D_COLOR, "Martin4D cc"        )
        PB(Mira3D             , u8"\uf0da", PORTED3D_COLOR, "Mira3D"             )
        PB(Mira4D             , u8"\uf0da", PORTED3D_COLOR, "Mira4D"             )
        PB(PopCorn3D          , u8"\uf0da", PORTED3D_COLOR, "PopCorn3D"          )
        PB(PopCorn4D          , u8"\uf0da", PORTED3D_COLOR, "PopCorn4D ssss"     )
        PB(PopCorn4Dscss      , u8"\uf0da", PORTED3D_COLOR, "PopCorn4D scss"     )
        PB(PopCorn4Dscsc      , u8"\uf0da", PORTED3D_COLOR, "PopCorn4D scsc"     )
        PB(PopCorn4Dsscc      , u8"\uf0da", PORTED3D_COLOR, "PopCorn4D sscc"     )
        PB(PopCorn4Dsimple    , u8"\uf0da", PORTED3D_COLOR, "PopCorn4D simple"   )
        PB(Kaneko3D           , u8"\uf0da", PORTED3D_COLOR, "Koneko3D"           )

//        PB(PopCorn4Drnd       , u8"\uf006" " PopCorn4D rnd"     )
//        PB(SymmetricIcons4D   , u8"\uf006" " SymmetricIcons4D"   )
#if !defined(GLAPP_DISABLE_DLA)
        PB(dla3D              , u8"\uf0da", DLA3D_COLOR   , "DLA 3D"             )
#endif
        PB(Aizawa             , u8"\uf0da", DT_COLOR      , "Aizawa"             )
        PB(Arneodo            , u8"\uf0da", DT_COLOR      , "Arneodo"            )
        PB(Bouali             , u8"\uf0da", DT_COLOR      , "Bouali"             )
        PB(BrukeShaw          , u8"\uf0da", DT_COLOR      , "Bruke-Shaw"         )
        PB(ChenCelikovsky     , u8"\uf0da", DT_COLOR      , "Chen-Celikovsky"    )
        PB(ChenLee            , u8"\uf0da", DT_COLOR      , "Chen-Lee"           )
        PB(Coullet            , u8"\uf0da", DT_COLOR      , "Coullet"            )
        PB(Dadras             , u8"\uf0da", DT_COLOR      , "Dadras"             )
        PB(DequanLi           , u8"\uf0da", DT_COLOR      , "Dequan-Li"          )
        PB(FourWing           , u8"\uf0da", DT_COLOR      , "FourWing"           )
        PB(FourWing2          , u8"\uf0da", DT_COLOR      , "FourWing 2"         )
        PB(FourWing3          , u8"\uf0da", DT_COLOR      , "FourWing 3"         )
        PB(GenesioTesi        , u8"\uf0da", DT_COLOR      , "Genesio-Tesi"       )
        PB(GloboToroid        , u8"\uf0da", DT_COLOR      , "GloboToroid"        )
        PB(Hadley             , u8"\uf0da", DT_COLOR      , "Hadley"             )
        PB(Halvorsen          , u8"\uf0da", DT_COLOR      , "Halvorsen"          )
        PB(LiuChen            , u8"\uf0da", DT_COLOR      , "Liu-Chen"           )
        PB(Lorenz             , u8"\uf0da", DT_COLOR      , "Lorenz"             )
        PB(MultiChuaII        , u8"\uf0da", DT_COLOR      , "Multi-Chua II"      )
        PB(NewtonLeipnik      , u8"\uf0da", DT_COLOR      , "Newton-Leipnik"     )
        PB(NoseHoover         , u8"\uf0da", DT_COLOR      , "Nose-Hoover"        )
        PB(QiChen             , u8"\uf0da", DT_COLOR      , "Qi3D / Qi-Chen"     )
        PB(RayleighBenard     , u8"\uf0da", DT_COLOR      , "Rayleigh-Benard"    )
        PB(Rossler            , u8"\uf0da", DT_COLOR      , "Rossler"            )
        PB(Rucklidge          , u8"\uf0da", DT_COLOR      , "Rucklidge"          )
        PB(Sakarya            , u8"\uf0da", DT_COLOR      , "Sakarya"            )
        PB(ShimizuMorioka     , u8"\uf0da", DT_COLOR      , "Shimizu-Morioka"    )
        PB(SprottLinzB        , u8"\uf0da", DT_COLOR      , "Sprott-Linz B"      )
        PB(SprottLinzF        , u8"\uf0da", DT_COLOR      , "Sprott-Linz F"      )
        PB(Tamari             , u8"\uf0da", DT_COLOR      , "Tamari"             )
        PB(Thomas             , u8"\uf0da", DT_COLOR      , "Thomas"             )
        PB(TSUCS              , u8"\uf0da", DT_COLOR      , "TSUCS1 / TSUCS2"    )
        PB(YuWang             , u8"\uf0da", DT_COLOR      , "Yu-Wang"            )
        PB(ZhouChen           , u8"\uf0da", DT_COLOR      , "Zhou-Chen"          )
//        PB(Robinson           , u8"\uf192" " Robinson"          )
        PB(juliaBulb_IIM      , u8"\uf0da", FRACTAL_COLOR , "JuliaBulb"          )
        PB(juliaBulb4th_IIM   , u8"\uf0da", FRACTAL_COLOR , "JuliaBulb Nth"      )
        PB(BicomplexJ_IIM     , u8"\uf0da", FRACTAL_COLOR , "biComplex Julia"    )
        PB(BicomplexJMod0_IIM , u8"\uf0da", FRACTAL_COLOR , "biCplxJ m.0"        )
        PB(BicomplexJMod1_IIM , u8"\uf0da", FRACTAL_COLOR , "biCplxJ m.1"        )
        PB(BicomplexJMod2_IIM , u8"\uf0da", FRACTAL_COLOR , "biCplxJ m.2"        )
        PB(BicomplexJMod3_IIM , u8"\uf0da", FRACTAL_COLOR , "biCplxJ m.3"        )
        PB(BicomplexJMod4_IIM , u8"\uf0da", FRACTAL_COLOR , "biCplxJ m.4"        )
        PB(BicomplexJMod5_IIM , u8"\uf0da", FRACTAL_COLOR , "biCplxJ m.5"        )
        PB(BicomplexJMod6_IIM , u8"\uf0da", FRACTAL_COLOR , "biCplxJ m.6"        )
        PB(BicomplexJMod7_IIM , u8"\uf0da", FRACTAL_COLOR , "biCplxJ m.7"        )
        PB(quatJulia_IIM      , u8"\uf0da", FRACTAL_COLOR , "quatJulia"          )
        PB(BicomplexJExplorer , u8"\uf0da", FRACTAL_COLOR , "biComplexJExplorer" )
        PB(glynnJB_IIM        , u8"\uf0da", FRACTAL_COLOR , "Glynn JuliaBulb"    )

        PB(volBedouin         , u8"\uf0da", VOLFRAC_COLOR , "Bedouin"            )
        PB(volRealMandel      , u8"\uf0da", VOLFRAC_COLOR , "RealMandel"         )
        PB(volSinRealMandel   , u8"\uf0da", VOLFRAC_COLOR , "SinRealMandel"      )
        PB(volQuatJulia       , u8"\uf0da", VOLFRAC_COLOR , "quatJulia"          )

        PB(tetrahedronGaussMap, u8"\uf0da", IFS_COLOR     , "tetrahedronGaussMap")
//        PB(Hopalong        , "Hopalong"         )
#endif
        selected = 0;

        loadStartData();
    }
#undef PB
#undef ATT_PATH
#undef ATT_EXT 
#undef MAGNETIC_COLOR
#undef POLINOM_COLOR 
#undef POWER3D_COLOR 
#undef RAMPE_COLOR   
#undef PICKOVER_COLOR
#undef PORTED3D_COLOR
#undef DLA3D_COLOR   
#undef DT_COLOR      
#undef FRACTAL_COLOR 

    void loadStartData() { for(auto i : ptr) i->startData(); }

    AttractorBase *get()      { return ptr.at(selected); }
    AttractorBase *get(int i) { return ptr.at(i); }
    vector<AttractorBase *>& getList()  { return ptr; }

    vec4& getColorGraphChar(int i)   { return ptr.at(i)->getColorGraphChar(); }
    string& getGraphChar(int i)      { return ptr.at(i)->getGraphChar(); }    
    string& getDisplayName(int i)    { return ptr.at(i)->getDisplayName(); }
    string& getDisplayName() { return ptr.at(selected)->getDisplayName(); }

    string& getNameID(int i)    { return ptr.at(i)->getNameID(); }
    string& getNameID() { return ptr.at(selected)->getNameID(); }

    string& getFileName(int i)    { return ptr.at(i)->fileName; }
    string& getFileName() { return ptr.at(selected)->fileName; }
    void setFileName(const std::string &s) { ptr.at(selected)->fileName = s; }
    //

    void saveFile(const char *name);
    bool loadFile(const char *name);

    void saveVals(Config &cfg);
    bool loadVals(Config &cfg);

    int  getSelection()      { return selected; }
    void setSelection(int i) { newSelection(i);  }

    void resetSelection() { selected = -1; }

    void checkCorrectEmitter();

    void setSelectionByName(const string &s) {
        for (int i = 0; i < ptr.size(); i++) 
            if(getNameID(i) == s) {
                newSelection(i);
                break;
            }
    }

    int getSelectionByName(const string &s) {
        for (int i = 0; i < ptr.size(); i++) 
            if(getNameID(i) == s) return i;

        return -1;
    }

    void restart();

    ~AttractorsClass() {
        while(!ptr.empty()) {
            delete ptr.back();
            ptr.pop_back();
        }
    }

    threadStepClass *getThreadStep() { return threadStep; }
    thread *getThread() { return threadStep->getThread(); }

    //thread func
    void endlessStep(emitterBaseClass *emitter);

    void queryStopThread() { endlessLoop=false; }
    void queryStartThread() { endlessLoop=true; }
    bool isStopThread() { return endlessLoop; }

    void newStepThread(emitterBaseClass *e);
    void deleteStepThread();
    void generateNewRandom();

    std::mutex &getStepMutex() { return stepMutex; }
    std::condition_variable & getStepCondVar() {return stepCondVar; }

    bool getEndlessLoop() { return endlessLoop; }

    bool continueDLA() { return wantContinueDLA; }
    void continueDLA(bool b) { wantContinueDLA = b; }
    void selectToContinueDLA(int i);


private:

    void newSelection(int i);
    void selection(int i);


    bool endlessLoop = true;
    bool wantContinueDLA = false;

    std::mutex stepMutex;
    std::condition_variable stepCondVar;

    std::mutex mtxStep;

    threadStepClass *threadStep = nullptr;

    vector<AttractorBase *> ptr;
    int selected;

// SlowMotion & cockPit

    friend AttractorBase;
    friend threadStepClass;

};

extern AttractorsClass attractorsList;

