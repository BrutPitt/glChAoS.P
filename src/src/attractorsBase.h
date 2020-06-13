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

//#include <fastRandom.h>

#include "IFS.h"
#include "tools/glslProgramObject.h"

using namespace std;
using namespace configuru;
using namespace fstRnd;

#define RANDOM(MIN, MAX) (fastRandom.range(MIN, MAX))
extern fFastRand32 fastRandom;


#define BUFFER_DIM 200
#define STABILIZE_DIM 1500

class attractorDlgClass;
class AttractorsClass;
class emitterBaseClass;
class transformedEmitterClass;

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

    virtual ifsBaseClass *getIFS() { return nullptr; }

    //thread Step with shared GPU memory
    void Step(float *&ptr, vec4 &v, vec4 &vp);
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

};

//  Attractors class with scalar K coeff
////////////////////////////////////////////////////////////////////////////
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

class attractorDtType : public attractorScalarK
{
protected:

    attractorDtType() {
        kMin = -5.0; kMax = 5.0; vMin = 0.0; vMax = 0.0;
        setDTType();
    }

    virtual void additionalDataCtrls();

    virtual void saveAdditionalData(Config &cfg);
    virtual void loadAdditionalData(Config &cfg);

    float getDtStepInc() { return dtStepInc; }
    void setDtStepInc(float f) { dtStepInc = f; }

    // dTime step 
    float dtStepInc = 0.001f;


};



//  Attractors class with vector K coeff
////////////////////////////////////////////////////////////////////////////
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

class fractalIIMBase : public attractorScalarK
{
public:
    bool ifsActive() { return ifs.active(); }
    ifsBaseClass *getIFS() { return &ifs; }

    vec4 &getIFSvec4() { return ifs.getTransfStruct(ifs.getCurrentTransform())->variations; }

protected:
    fractalIIMBase() {
        vMin = 0.f; vMax = 0.f; kMin = 0.f; kMax = 0.f;
        m_POV = vec3( 0.f, 0, 7.f);
        setFractalType();
    }

    void maxDepthReached() {

    }

    void testDepth(vec4 &v, vec4 &vp) {
        if(depth++>maxDepth || distance(v,vp)<minDistance) {
            depth = 0;
            v = vVal[0] + (vMin == vMax ? vec4(vMin) :
                   vec4(fastRandom.range(vMin, vMax),
                        fastRandom.range(vMin, vMax),
                        fastRandom.range(vMin, vMax),
                        fastRandom.range(vMin, vMax)));
            
            kRnd = kMin == kMax ? vec4(kMin) :
                   vec4(fastRandom.range(kMin, kMax),
                        fastRandom.range(kMin, kMax),
                        fastRandom.range(kMin, kMax),
                        fastRandom.range(kMin, kMax));
        } 
    }

    virtual void preStep(vec4 &v) {
        if(depth++>maxDepth) {
            depth = 0;
            v = vVal[0] + (vMin == vMax ? vec4(vMin) :
                   vec4(fastRandom.range(vMin, vMax),
                        fastRandom.range(vMin, vMax),
                        fastRandom.range(vMin, vMax),
                        fastRandom.range(vMin, vMax)));
            
            kRnd = kMin == kMax ? vec4(kMin) :
                   vec4(fastRandom.range(kMin, kMax),
                        fastRandom.range(kMin, kMax),
                        fastRandom.range(kMin, kMax),
                        fastRandom.range(kMin, kMax));
        } 
    }

    //  Specific attractor values
    void saveAdditionalData(Config &cfg);
    void loadAdditionalData(Config &cfg);

    virtual void additionalDataCtrls();

    vec4 kRnd = vec4(0.f);
    vec4 vIter;
    
    int maxDepth = 50;
    int degreeN = 2;
    float minDistance = .001;

    int depth = 0;

    ifsBaseClass ifs;
private:
};


class fractalIIM_Nth : public fractalIIMBase
{
public:
    //  Specific attractor values
    virtual void additionalDataCtrls();
protected:

};


class juliaBulb_IIM : public fractalIIMBase
{    
public:
    juliaBulb_IIM() { stepFn = (stepPtrFn) &juliaBulb_IIM::Step; }

protected:
    void Step(vec4 &v, vec4 &vp);
    void startData();
};

class juliaBulb4th_IIM : public fractalIIM_Nth
{    
public:
    juliaBulb4th_IIM() { stepFn = (stepPtrFn) &juliaBulb4th_IIM::Step; }

protected:
    void Step(vec4 &v, vec4 &vp);
    void startData();
};

class fractalIIM_4D : public fractalIIMBase
{
public:
    //void preStep(vec4 &v, vec4 &vp) { last4D = RANDOM(vMin, vMax); fractalIIMBase::preStep(v,vp); }
    int getPtSize() { return attPt4D; }

    virtual void initStep() {
        attractorScalarK::initStep();
    }

protected:

};


class BicomplexBase : public fractalIIM_4D
{
public:
    typedef void (BicomplexBase::*magneticPtrFn)(const vec4 &, int);
    void startData();

    void radiciBicomplex(const vec4 &pt, vec4 &vp)
    {
        const int rnd = fastRand32::xorShift();
        const float sign1 = (rnd&2) ? 1.f : -1.f, sign2 = (rnd&1) ? 1.f : -1.f;
        const vec4 c = ifs.active() ? kRnd+getIFSvec4() : kRnd; // IFS transforms
        const vec4 p(pt - ((vec4 &)*kVal.data()+c));

        const std::complex<float> z1(p.x, p.y), z2(-p.w, p.z);
        const std::complex<float> w1 = sign1 * sqrt(z1 - z2), w2 = sign2 *sqrt(z1 + z2);
        vp = vec4(w1.real()+w2.real(), w1.imag()+w2.imag(), w2.imag()-w1.imag(), w1.real()-w2.real())*.5f;
    };

};

/////////////////////////////////////////////////
class BicomplexJ_IIM : public BicomplexBase 
{    
public:
    BicomplexJ_IIM() { stepFn = (stepPtrFn) &BicomplexJ_IIM::Step; }

protected:
    //void Step(vec4 &v, vec4 &vp) { preStep(v); radiciBicomplex(vec4(v, last4D), vp); } //remove
    void Step(vec4 &v, vec4 &vp) { radiciBicomplex(v, vp); testDepth(v, vp); }
};

/////////////////////////////////////////////////
class BicomplexJMod0_IIM : public BicomplexBase
{    
public:
    BicomplexJMod0_IIM() { stepFn = (stepPtrFn) &BicomplexJMod0_IIM::Step; }

protected:
    //void Step(vec4 &v, vec4 &vp) { preStep(v); radiciBicomplex(vec4(v, dim4D), vp); } remove
    void Step(vec4 &v, vec4 &vp) { vec4 pt((vec3)v, vVal[0].w); radiciBicomplex(pt, vp); testDepth(pt, vp); }
};

/////////////////////////////////////////////////
class BicomplexJMod1_IIM : public BicomplexBase
{    
public:
    BicomplexJMod1_IIM() { stepFn = (stepPtrFn) &BicomplexJMod1_IIM::Step; }

protected:
    //void Step(vec4 &v, vec4 &vp) { preStep(v); radiciBicomplex(vec4(v.x, v.y, vVal[0].z, last4D), vp); } remove
    void Step(vec4 &v, vec4 &vp) { vec4 pt(v.x, v.y, vVal[0].z, v.w); radiciBicomplex(pt, vp); testDepth(pt, vp); }
};
/////////////////////////////////////////////////
class BicomplexJMod2_IIM : public BicomplexBase
{    
public:
    BicomplexJMod2_IIM() { stepFn = (stepPtrFn) &BicomplexJMod2_IIM::Step; }

protected:
    //void Step(vec4 &v, vec4 &vp) { preStep(v,vp); radiciBicomplex(vec4(last4D, v.x, v.z, v.x), vp); }
    //void Step(vec4 &v, vec4 &vp) { preStep(v,vp); radiciBicomplex(vec4(v.y, v.z, last4D, v.x), vp); }
    //void Step(vec4 &v, vec4 &vp) { preStep(v,vp); radiciBicomplex(vec4(v.x, v.y, v.y, v.y), vp); }

    //void Step(vec4 &v, vec4 &vp) { preStep(v); radiciBicomplex(vec4(v.x, v.y, v.z, v.y), vp); } remove
    void Step(vec4 &v, vec4 &vp) { vec4 pt(v.x, v.y, v.z, v.y); radiciBicomplex(pt, vp); testDepth(pt, vp); }
};

/////////////////////////////////////////////////
class BicomplexJMod3_IIM : public BicomplexBase
{    
public:
    BicomplexJMod3_IIM() { stepFn = (stepPtrFn) &BicomplexJMod3_IIM::Step; }

protected:
    void Step(vec4 &v, vec4 &vp) { vec4 pt(vVal[0].x, v.y, v.z, v.w);  radiciBicomplex(pt, vp); testDepth(pt, vp); }
};
/////////////////////////////////////////////////
class BicomplexJMod4_IIM : public BicomplexBase
{    
public:
    BicomplexJMod4_IIM() { stepFn = (stepPtrFn) &BicomplexJMod4_IIM::Step; }

protected:
    void Step(vec4 &v, vec4 &vp) { vec4 pt(v.x, v.y, v.x, v.w); radiciBicomplex(pt, vp); testDepth(pt, vp); }
};
/////////////////////////////////////////////////
class BicomplexJMod5_IIM : public BicomplexBase
{    
public:
    BicomplexJMod5_IIM() { stepFn = (stepPtrFn) &BicomplexJMod5_IIM::Step; }

protected:
    void Step(vec4 &v, vec4 &vp) { vec4 pt( v.y, v.x, v.w, v.z); radiciBicomplex(pt, vp); testDepth(pt, vp); }
};
/////////////////////////////////////////////////
class BicomplexJMod6_IIM : public BicomplexBase
{    
public:
    BicomplexJMod6_IIM() { stepFn = (stepPtrFn) &BicomplexJMod6_IIM::Step; }

protected:
    void Step(vec4 &v, vec4 &vp) { vec4 pt(vVal[0].x, vVal[0].y, v.z, v.w); radiciBicomplex(pt, vp); testDepth(pt, vp); }
};
/////////////////////////////////////////////////
class BicomplexJMod7_IIM : public BicomplexBase
{    
public:
    BicomplexJMod7_IIM() { stepFn = (stepPtrFn) &BicomplexJMod7_IIM::Step; }

protected:
    //void Step(vec4 &v, vec4 &vp) { radiciBicomplex(vec4( v.x, v.x, v.z, v.x), vp); }
    void Step(vec4 &v, vec4 &vp) { vec4 pt( v.x, v.x, v.z, vVal[0].w); radiciBicomplex(pt, vp); testDepth(pt, vp); }
    //void Step(vec4 &v, vec4 &vp) { radiciBicomplex(vec4( v.x, v.x, v.z, last4D), vp); }
};

/////////////////////////////////////////////////
class BicomplexJExplorer : public BicomplexBase
{    
public:
    BicomplexJExplorer() { 
        stepFn = (stepPtrFn) &BicomplexJExplorer::Step; 
    }

    void saveAdditionalData(Config &cfg);
    void loadAdditionalData(Config &cfg);

    void additionalDataCtrls();
    void initStep() {
        //fractalIIMBase::initStep();
        resetQueue();
        Insert(vVal[0]);
        //stabilize(STABILIZE_DIM);

        a1[0] = a2[0] = a3[0] = a4[0] = &vVal[0].x;
        a1[1] = a2[1] = a3[1] = a4[1] = &vVal[0].y;
        a1[2] = a2[2] = a3[2] = a4[2] = &vVal[0].z;
        a1[3] = a2[3] = a3[3] = a4[3] = &vVal[0].w;
        a1[4] = a2[4] = a3[4] = a4[4] = &vt.x;
        a1[5] = a2[5] = a3[5] = a4[5] = &vt.y;
        a1[6] = a2[6] = a3[6] = a4[6] = &vt.z;
        a1[7] = a2[7] = a3[7] = a4[7] = &vt.w;
    }

protected:
    void Step(vec4 &v, vec4 &vp) {         
        vt = v; 
        vec4 pt( *a1[idx0], *a2[idx1], *a3[idx2], *a4[idx3]);
        radiciBicomplex(pt, vp); 
        testDepth(pt, vp); 
    }
private:
    float *a1[8], *a2[8], *a3[8], *a4[8];
    int idx0 = 4, idx1 = 5, idx2 = 6, idx3 = 7 ;
    const char str[8][4] { "s.X", "s.Y", "s.Z", "s.W", "i.X", "i.Y", "i.Z", "i.W" }; 
    vec4 vt;

};

/////////////////////////////////////////////////
class quatJulia_IIM : public fractalIIM_4D
{    
public:
    quatJulia_IIM() { stepFn = (stepPtrFn) &quatJulia_IIM::Step; }

protected:
    void Step(vec4 &v, vec4 &vp);
    void startData();
};


class glynnJB_IIM : public fractalIIMBase
{    
public:
    glynnJB_IIM() { stepFn = (stepPtrFn) &glynnJB_IIM::Step;  }

protected:
    void Step(vec4 &v, vec4 &vp);
    void startData();
};


//--------------------------------------------------------------------------
//  Vector K Coeff Attractors
//--------------------------------------------------------------------------

//  Polinomial base class
////////////////////////////////////////////////////////////////////////////
class PowerN3D : public attractorVectorK
{
public:

    PowerN3D() {
        
        stepFn = (stepPtrFn) &PowerN3D::Step;    

        tmpOrder = order = 2;
        vMin = 0.0; vMax = 0.0; kMin = -1.25; kMax = 1.25;
        m_POV = vec3( 0.f, 0, 7.f);
        
        resetData();
    }

    void Step(vec4 &v, vec4 &vp);
    void startData();

    void searchAttractor()  { searchLyapunov(); }

    void initStep() {
        resetQueue();
        resetData();

        if(nCoeff!=kVal.size()) assert("mismatch loaded size!!");

        Insert(vVal[0]);
        stabilize(STABILIZE_DIM);
    }

    //  Personal vals
    ///////////////////////////////////////
    void saveAdditionalData(Config &cfg);
    void loadAdditionalData(Config &cfg);

    void additionalDataCtrls();

    //  SetOrder
    ///////////////////////////////////////
    void setOrder(const int n)
    { 
        tmpOrder = order = n;
        resetData();

        kVal.resize(nCoeff);

        newRandomValues();

        resetQueue();
        Insert(vVal[0]);

        searchAttractor();
    }


    void newRandomValues()
    {
        for(int i=0; i<nCoeff; i++)
            kVal[i] = vec4(RANDOM(kMin,kMax),RANDOM(kMin,kMax),RANDOM(kMin,kMax),0.f);

        vVal[0] = vec4(RANDOM(vMin,vMax),RANDOM(vMin,vMax),RANDOM(vMin,vMax),0.f);
    }

private:

    int getNumCoeff(int o) { return (o+1) * (o+2) * (o+3) / 6; }
    int getNumCoeff() { return getNumCoeff(order); }

    void resetData() {
        nCoeff = getNumCoeff();

        elv.resize(order+1);
        cf.resize(nCoeff);
    }

    vector<vec3> elv;
    vector<float> cf;
    int nCoeff;
    int order, tmpOrder;
};

//  Polinomial base class
////////////////////////////////////////////////////////////////////////////
class PolynomialBase : public attractorVectorK
{
public:
    PolynomialBase() {
        vMin = 0.0; vMax = 0.0; kMin = -1.0; kMax = 1.0;
        m_POV = vec3( 0.f, 0, 7.f);
    }
protected:
    virtual void searchAttractor()  { searchLyapunov(); }
};

/////////////////////////////////////////////////
class PolynomialA : public PolynomialBase
{
public:
    PolynomialA() { stepFn = (stepPtrFn) &PolynomialA::Step; }
protected:
    void Step(vec4 &v, vec4 &vp);
    void startData();
/*
    void newRandomValues ()
    {
            float val=.23+(float)rand()/(float)RAND_MAX*.1f;
            //float val=1.55f+(float)rand()/(float)RAND_MAX*.2f;

            int j=rand()%3;

            kVal[j]=val;
            kVal[(j+1)%3]=(float)rand()/(float)RAND_MAX*2.f;
            kVal[(j+2)%3]=3.f-kVal[j]-kVal[(j+1)%3];
    }*/
};

/////////////////////////////////////////////////
class PolynomialB : public PolynomialBase
{
public:
    PolynomialB() { stepFn = (stepPtrFn) &PolynomialB::Step; }
protected:
    void Step(vec4 &v, vec4 &vp);
    void startData();
};

/////////////////////////////////////////////////
class PolynomialC : public PolynomialBase
{
public:
    PolynomialC() { stepFn = (stepPtrFn) &PolynomialC::Step; }
protected:
    void Step(vec4 &v, vec4 &vp);
    void startData();
};

/////////////////////////////////////////////////
class PolynomialABS : public PolynomialBase
{
public:
    PolynomialABS() { stepFn = (stepPtrFn) &PolynomialABS::Step; }
protected:
    void Step(vec4 &v, vec4 &vp);
    void startData();
};

/////////////////////////////////////////////////
class PolynomialPow : public PolynomialBase
{
public:
    PolynomialPow() { stepFn = (stepPtrFn) &PolynomialPow::Step; }
protected:
    void Step(vec4 &v, vec4 &vp);
    void startData();
};

/////////////////////////////////////////////////
class PolynomialSin : public PolynomialBase
{
public:
    PolynomialSin() { stepFn = (stepPtrFn) &PolynomialSin::Step; }
protected:
    void Step(vec4 &v, vec4 &vp);
    void startData();
};

//  Rampe base class
////////////////////////////////////////////////////////////////////////////
class RampeBase : public attractorVectorK
{
public:
    RampeBase() {
        vMin = 0.0; vMax = 0.0; kMin = -1.0; kMax = 1.0;
        m_POV = vec3( 0.f, 0, 10.f);
    }
protected:
    void searchAttractor()  { searchLyapunov(); }
};

#define RAMPE(A)\
class Rampe##A : public RampeBase {\
public:\
    Rampe##A() { stepFn = (stepPtrFn) &Rampe##A::Step; }\
protected:\
    void Step(vec4 &v, vec4 &vp);\
    void startData(); };

RAMPE(01)
RAMPE(02)
RAMPE(03)
RAMPE(03A)
RAMPE(04)
RAMPE(05)
RAMPE(06)
RAMPE(07)
RAMPE(08)
RAMPE(09)
RAMPE(10)

#undef RAMPE


//--------------------------------------------------------------------------
//  Scalar K Coeff Attractors
//--------------------------------------------------------------------------

//  KingsDream base class
////////////////////////////////////////////////////////////////////////////
class KingsDream : public attractorScalarK
{
public:
    KingsDream() { 
        stepFn = (stepPtrFn) &KingsDream::Step;
        vMin = -0.5; vMax = 0.5; kMin = -2.0; kMax = 2.0;
        m_POV = vec3( 0.f, 0, 10.f);
    }
protected:
    void Step(vec4 &v, vec4 &vp);
    void startData();
    void searchAttractor()  { searchLyapunov(); }
};

//  Pickover base class
////////////////////////////////////////////////////////////////////////////
class Pickover : public attractorScalarK
{
public:

    Pickover() {
        stepFn = (stepPtrFn) &Pickover::Step;

        vMin = -1.0; vMax = 1.0; kMin = -3.0; kMax = 3.0;

        m_POV = vec3( 0.f, 0, 7.f);
    }

protected:
    void Step(vec4 &v, vec4 &vp);
    void startData();
    void searchAttractor()  { searchLyapunov(); }
};

//  SinCos base class
////////////////////////////////////////////////////////////////////////////
class SinCos : public attractorScalarK
{
public:

    SinCos() {
        stepFn = (stepPtrFn) &SinCos::Step;

        kMin = -T_PI;
        kMax =  T_PI;
        vMin = -1.0; vMax = 1.0;

        m_POV = vec3( 0.f, 0, 12.f);
    }

protected:
    void Step(vec4 &v, vec4 &vp);
    void startData();
    void searchAttractor()  { searchLyapunov(); }
};

//  Mira3D
////////////////////////////////////////////////////////////////////////////
class Mira3D : public attractorScalarK
{
public:

    Mira3D() {
        stepFn = (stepPtrFn) &Mira3D::Step;

        vMin = -10.0; vMax = 10.0; kMin = -1.0; kMax = 1.0;

        m_POV = vec3( 0.f, 0, 12.f);
    }

protected:
    void Step(vec4 &v, vec4 &vp);
    void startData();
    void searchAttractor()  { searchLyapunov(); }
};
//  Mira4D
////////////////////////////////////////////////////////////////////////////
class Mira4D : public attractorScalarK
{
public:

    Mira4D() {
        stepFn = (stepPtrFn) &Mira4D::Step;
        vMin = -10.0; vMax = 10.0; kMin = -1.0; kMax = 1.0;

        m_POV = vec3( 0.f, 0, 50.f);
    }

    int getPtSize() { return attPt4D; }
protected:
    void Step(vec4 &v, vec4 &vp);
    void startData();
    void searchAttractor()  { searchLyapunov(); }
};

//  PopCorn 4D BaseClasses
////////////////////////////////////////////////////////////////////////////
class PopCorn4DType : public attractorScalarK
{
public:
    
    PopCorn4DType() {
        vMin = -1.0; vMax = 1.0; kMin = -1.0; kMax = 1.0;
        m_POV = vec3( 0.f, 0, 12.f);
    }

    int getPtSize() { return attPt4D; }
protected:
    virtual void startData();
    //virtual void additionalDataCtrls();
};
class PopCorn4Dset : public PopCorn4DType
{
public:
    void Step(vec4 &v, vec4 &vp);

protected:
    double (*pfX)(double), (*pfY)(double), (*pfZ)(double), (*pfW)(double);
};

//  PopCorn 4D
////////////////////////////////////////////////////////////////////////////
class PopCorn4D : public PopCorn4Dset
{
public:
    PopCorn4D() { stepFn = (stepPtrFn) &PopCorn4Dset::Step; 
                  pfX = pfY = pfZ = pfW = sin; }
};
////////////////////////////////////////////////////////////////////////////
class PopCorn4Dscss : public PopCorn4Dset
{
public:
    PopCorn4Dscss() { stepFn = (stepPtrFn) &PopCorn4Dset::Step; 
                      pfX = pfZ = pfW = sin;  pfY = cos; }
};
////////////////////////////////////////////////////////////////////////////
class PopCorn4Dscsc : public PopCorn4Dset
{
public:
    PopCorn4Dscsc() { stepFn = (stepPtrFn) &PopCorn4Dset::Step; 
                      pfX = pfZ = sin;  pfY = pfW = cos; }
};
////////////////////////////////////////////////////////////////////////////
class PopCorn4Dsscc : public PopCorn4Dset
{
public:
    PopCorn4Dsscc() { stepFn = (stepPtrFn) &PopCorn4Dset::Step; 
                      pfX = pfY = sin;  pfZ = pfW = cos; }
};

class PopCorn4Dsimple : public PopCorn4DType
{
public:
    PopCorn4Dsimple() { stepFn = (stepPtrFn) &PopCorn4Dsimple::Step;
                      pfX = pfY = pfZ = pfW = sin; }
    void Step(vec4 &v, vec4 &vp);

protected:
    double (*pfX)(double), (*pfY)(double), (*pfZ)(double), (*pfW)(double);
};

////////////////////////////////////////////////////////////////////////////
class PopCorn4Drnd : public PopCorn4DType
{
public:
    void startData();
    PopCorn4Drnd() { stepFn = (stepPtrFn) &PopCorn4DType::Step; }
protected:
    void Step(vec4 &v, vec4 &vp);
};

//  PopCorn 3D
////////////////////////////////////////////////////////////////////////////
class PopCorn3D : public attractorScalarK
{
public:

    PopCorn3D() {
        stepFn = (stepPtrFn) &PopCorn3D::Step;
        vMin = -1.0; vMax = 1.0; kMin = -1.0; kMax = 1.0;

        m_POV = vec3( 0.f, 0, 12.f);
    }

protected:
    void Step(vec4 &v, vec4 &vp);
    void startData();
};
//  Martin 4D
////////////////////////////////////////////////////////////////////////////
class Martin4DBase : public attractorScalarK
{
public:
    Martin4DBase() {
        vMin = .0; vMax = .5; kMin = 2.7; kMax = 3.0;

        m_POV = vec3( 0.f, 0, 50.f);
    }

    int getPtSize() { return attPt4D; }

    void Step(vec4 &v, vec4 &vp);
protected:
    double (*pfX)(double), (*pfZ)(double);
    void startData();
};

class Martin4D : public Martin4DBase
{
public:
    Martin4D() { stepFn = (stepPtrFn) &Martin4DBase::Step;  pfX = pfZ = sin; }
};
class Martin4Dsc : public Martin4DBase
{
public:
    Martin4Dsc() { stepFn = (stepPtrFn) &Martin4DBase::Step;  pfX = sin; pfZ = cos; }
};
class Martin4Dcc : public Martin4DBase
{
public:
    Martin4Dcc() { stepFn = (stepPtrFn) &Martin4DBase::Step;  pfX = pfZ = cos; }
};

//  Symmetric Icons
////////////////////////////////////////////////////////////////////////////
class SymmetricIcons4D : public attractorScalarK
{
public:

    SymmetricIcons4D() {
        stepFn = (stepPtrFn) &SymmetricIcons4D::Step;
        vMin = -1.0; vMax = 1.0; kMin = -1.0; kMax = 1.0;

        m_POV = vec3( 0.f, 0, 12.f);
    }

    int getPtSize() { return attPt4D; }
protected:
    void Step(vec4 &v, vec4 &vp);
    void startData();
};


//  Hopalong 4D
////////////////////////////////////////////////////////////////////////////
class Hopalong4D : public attractorScalarK
{
public:

    Hopalong4D() {
        stepFn = (stepPtrFn) &Hopalong4D::Step;

        vMin = -1.0; vMax = 1.0; kMin = -1.0; kMax = 1.0;
        m_POV = vec3( 0.f, 0, 50.f);
    }

    int getPtSize() { return attPt4D; }
protected:
    void Step(vec4 &v, vec4 &vp);
    void startData();
    //void searchAttractor()  { searchLyapunov(); }
};

#if !defined(GLAPP_DISABLE_DLA)

using tPrec = VG_T_TYPE;

//#define GLAPP_USE_BOOST_LIBRARY

// number of dimensions (must be 2 or 3)
#define DLA_DIM 3

#define DLA_USE_FAST_RANDOM
#ifdef DLA_USE_FAST_RANDOM
    #define DLA_RANDOM_NORM fastRandom.VNI()
    #define DLA_RANDOM_01   fastRandom.UNI()
#else
    #define DLA_RANDOM_NORM stdRandom(-1.f, 1.f)
    #define DLA_RANDOM_01   stdRandom( 0.f, 1.f)
#endif

// boost is used for its spatial index

#ifdef GLAPP_USE_BOOST_LIBRARY
    #include <boost/function_output_iterator.hpp>
    #include <boost/geometry/geometry.hpp>

    using BoostPoint = boost::geometry::model::point<float, DLA_DIM, boost::geometry::cs::cartesian>;
    using boostIndexValue = std::pair<BoostPoint, uint32_t>;
    using boostIndex = boost::geometry::index::rtree<boostIndexValue, boost::geometry::index::linear<4>>;

    #define parentPOINT(PARENT) m_Points[PARENT]
    #define thisPOINT m_Points
#else
    #include <nanoflann/nanoflann.hpp>

    #define parentPOINT(PARENT) m_Points.pts[PARENT]
    #define thisPOINT m_Points.pts


TEMPLATE_TYPENAME_T struct pointCloud
{
    
    std::vector<vec3> pts;
    
    // Must return the number of data points
    inline size_t kdtree_get_point_count() const { return pts.size(); }
    
    // Returns the dim'th component of the idx'th point in the class:
    // Since this is inlined and the "dim" argument is typically an immediate value, the
    //  "if/else's" are actually solved at compile time.
    inline tPrec kdtree_get_pt(const size_t idx, const size_t dim) const
    { return pts[idx][dim]; }
    
    // Optional bounding-box computation: return false to default to a standard bbox computation loop.
    //   Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
    //   Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
    template <class BBOX>
    bool kdtree_get_bbox(BBOX& /* bb */) const { return false; }
    
};

using tPointCloud =  pointCloud<tPrec>;

using tKDTreeDistanceFunc = nanoflann::L2_Simple_Adaptor<tPrec, tPointCloud>;
using tKDTree = nanoflann::KDTreeSingleIndexDynamicAdaptor<tKDTreeDistanceFunc, tPointCloud, 3>;
#endif

//  DLA3D base class
////////////////////////////////////////////////////////////////////////////
// Adaptation and optimization from original project of Michael Fogleman
// https://github.com/fogleman/dlaf
//
// Original Parameters   ==>    substitution
// -------------------------------------------------------------------------
// ParticleSpacing       ==>    kVal[0]
// AttractionDistance    ==>    kVal[1]
// MinMoveDistance       ==>    kVal[2]
//
// Description from author
// -------------------------------------------------------------------------
// ParticleSpacing     The distance between particles when they become joined together.
// AttractionDistance  How close together particles must be in order to join together.
// MinMoveDistance     The minimum distance that a particle will move in an iteration during its random walk.
// Stubbornness        How many join attempts must occur before a particle will allow another particle to join to it.
// Stickiness          The probability that a particle will allow another particle to join to it.
class dla3D : public attractorScalarK {
public:
    dla3D() {
        stepFn = (stepPtrFn) &dla3D::Step;

        vMin = -1.0; vMax = 1.0; kMin = -1.0; kMax = 1.0;

        m_POV = vec3( 0.f, 0, 12.f);
        inputKMin = 0.0001, inputKMax = 10000.0;
        setDLAType();
    }
#if !defined(GLAPP_USE_BOOST_LIBRARY)
    ~dla3D() { delete m_Index; }
#endif

    void buildIndex();

    inline void addLoadedPoint(const vec3 &p) {
        thisPOINT.push_back(p);
        boundingRadius = std::max(boundingRadius, length(p) + kVal[1]);
    }


    void resetIndexData() {
        if(thisPOINT.size()) thisPOINT.clear();
#ifdef GLAPP_USE_BOOST_LIBRARY
        if(m_Index.size()) m_Index.clear();
#else             
        delete m_Index;
        m_Index = new tKDTree(DLA_DIM, m_Points, nanoflann::KDTreeSingleIndexAdaptorParams(10 /* max leaf */) );
#endif        
        m_JoinAttempts.clear();
        boundingRadius = 0.f;
    }
protected:
    void initStep() {
        resetIndexData();
        resetQueue();
        Insert(vec4(0.f));
        Add(vec3(0.0));
        //addedPoints.clear();
        //startThreads(4);
    }

    void additionalDataCtrls();
    void saveAdditionalData(Config &cfg);
    void loadAdditionalData(Config &cfg);

#ifdef GLAPP_USE_BOOST_LIBRARY
    // Add adds a new particle with the specified parent particle
    void Add(const vec3 &p) {
        const uint32_t id = m_Points.size();
        m_Index.insert(std::make_pair(BoostPoint(p.x, p.y, p.z), id));
        m_Points.push_back(p);
        m_JoinAttempts.push_back(0);
        boundingRadius = max(boundingRadius, length(p) + kVal[1]);
    }
    // Nearest returns the index of the particle nearest the specified point
    uint32_t Nearest(const vec3 &point) const {
        uint32_t result = -1;
        m_Index.query(
            boost::geometry::index::nearest(BoostPoint(point.x,point.y,point.z), 1),
            boost::make_function_output_iterator([&result](const auto &value) {
                result = value.second;
            }));
        return result;
    }
#else
/*
// Multipoints
    std::vector<vec3> tmp;
    void Add(const vec3 &p) {
        //my_kd_tree_t index(3 , m_Points, KDTreeSingleIndexAdaptorParams(10 max leaf ) );
        //static uint32_t count = 0;
        //const int nPT = 16;

        size_t id = m_Points.pts.size();
        m_Points.pts.push_back(p);        
        m_JoinAttempts.push_back(0);

        static int count =0;
        static int next = 1;
        static int limit = 1024;
        static int block = 0xff;
        if(next<limit) {
            if(!(id%next)) { m_Index->addPoints(id-count, id); count = 0; next = (1+(id>>4)); }
            else count++;
            //m_Index->addPoints(id, id);
        } else {
            if(!(id&block)) { 
                m_Index->addPoints(id-count, id); count = 0; int blk = 512 / (1+(id>>16)); next = blk<1 ? 1 : blk; 
            }
            else count++;
        } 
        //m_Index->addPoints(id,id);
        
        boundingRadius = std::max(boundingRadius, length(p) + kVal[1]);
    }
*/
    void Add(const vec3 &p) {
        size_t id = m_Points.pts.size();
        m_Points.pts.push_back(p);        
        m_JoinAttempts.push_back(0);

        m_Index->addPoints(id, id);
        boundingRadius = std::max(boundingRadius, length(p) + kVal[1]);
    }

    uint32_t Nearest(const vec3 &point) const {
        size_t ret_index;
        tPrec out_dist_sqr = kVal[1]*.5;
        nanoflann::KNNResultSet<tPrec> resultSet(1);
        resultSet.init(&ret_index, &out_dist_sqr );
        m_Index->findNeighbors(resultSet, (const tPrec *) &point, nanoflann::SearchParams(1, boundingRadius /*kVal[1]*/));
        return ret_index;
    }

#endif
        //std::cout << id << "," << p.x << "," << p.y << "," << p.z << std::endl;

    // PlaceParticle computes the final placement of the particle.
    vec3 PlaceParticle(const vec3 &p, const uint32_t parent) const {
        return lerp(parentPOINT(parent), p, kVal[0]);
    }

    // RandomStartingPosition returns a random point to start a new particle
    vec3 RandomStartingPosition() const {
        return normalizedRandomVector() * boundingRadius;
    }

    // ShouldReset returns true if the particle has gone too far away and
    // should be reset to a new random starting position
    bool ShouldReset(const vec3 &p) const {
        return length(p) > boundingRadius * 2;
    }

    // ShouldJoin returns true if the point should attach to the specified
    // parent particle. This is only called when the point is already within
    // the required attraction distance.
    bool ShouldJoin(const vec3 &p, const uint32_t parent) {
        return (m_JoinAttempts[parent]++ < m_Stubbornness) ? false : DLA_RANDOM_01 <= m_Stickiness;
    }
/*
    // MotionVector returns a vector specifying the direction that the
    // particle should move for one iteration. The distance that it will move
    // is determined by the algorithm.
    vec3 MotionVector(const vec3 &p) const {
        return RandomInUnitSphere();
    }

    vec3 RandomInUnitSphere() const {
        vec3 p;
        do {
            p = vec3(DLA_RANDOM_NORM, DLA_RANDOM_NORM, DLA_RANDOM_NORM);
        } while(length(p) >= 1.f);
        return p;
    } 
*/
    vec3 normalizedRandomVector() const { return normalize(vec3(DLA_RANDOM_NORM, DLA_RANDOM_NORM, DLA_RANDOM_NORM)); }


    // AddParticle diffuses one new particle and adds it to the model
    vec3 &AddParticle_() {
        // compute particle starting location
        vec3 p = RandomStartingPosition();

        // do the random walk
        while (true) {
            // get distance to nearest other particle
            const uint32_t parent = Nearest(p);
            const tPrec d = distance(p, parentPOINT(parent));

            // check if close enough to join
            if (d < kVal[1]) {
                if (!ShouldJoin(p, parent)) {
                    // push particle away a bit
                    //p = lerp(parentPOINT(parent), p, kVal[1]+kVal[2]);
                    p = lerp(parentPOINT(parent), p, kVal[1]);
                    continue;
                } 
                // adjust particle pos in relation to its parent and add the point
                Add(PlaceParticle(p, parent));
                return thisPOINT.back();
            }

            // move randomly
            p += normalizedRandomVector() * DLA_RANDOM_01 * std::max(kVal[2], d - kVal[1]);

            // check if particle is too far away, reset if so
            if (ShouldReset(p)) p = RandomStartingPosition();
        }
    }

    vec3 &AddParticle() {
        vec3 p = RandomStartingPosition();

        uint32_t parent;
        tPrec d;
        const tPrec val1 = kVal[1], val2 = kVal[2];
        do {
            parent = Nearest(p);
            d = distance(p, parentPOINT(parent)); // get distance to nearest other particle

            if (ShouldReset(p)) p = RandomStartingPosition(); // check if particle is too far away, reset if so
            else                p += normalizedRandomVector() * DLA_RANDOM_01 * std::max(val2, d - val1); // move randomly

        } while (d > val1); // repeat until is close enough to join

        while(!ShouldJoin(p, parent)) { p = lerp(parentPOINT(parent), p, val1+val2); } // push particle away a bit

        Add(PlaceParticle(p, parent)); // adjust particle pos in relation to its parent and add the point
        return thisPOINT.back();
    }

    void Step(vec4 &v, vec4 &vp);
    void startData();

private:
    tPrec stdRandom(tPrec lo, tPrec hi) const {
        static thread_local std::mt19937 gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        std::uniform_real_distribution<tPrec> dist(lo, hi); return dist(gen);
    }

    vec3 lerp(const vec3 &a, const vec3 &b, tPrec d) const {
        return a + normalize(b-a) * d;
    }

    //The probability that a particle will allow another particle to join to it.
    tPrec m_Stickiness = 1.0;
    // m_Stubbornness defines how many interactions must occur before a
    // particle will allow another particle to join to it.
    int m_Stubbornness = 0;

    // m_BoundingRadius defines the radius of the bounding sphere that bounds
    // all of the particles
    tPrec boundingRadius = 0;

    // m_JoinAttempts tracks how many times other particles have attempted to
    // join with each finalized particle
    std::vector<uint32_t> m_JoinAttempts;

#ifdef GLAPP_USE_BOOST_LIBRARY
    // m_Index is the spatial index used to accelerate nearest neighbor queries
    boostIndex m_Index;
    // m_Points stores the final particle positions
    std::vector<vec3> m_Points;
#else
    tPointCloud m_Points;
    tKDTree *m_Index = nullptr;
#endif
};
#endif

//--------------------------------------------------------------------------
//  d(x,y,z)/dt Attractors
//--------------------------------------------------------------------------
#define DT(A)\
class A : public attractorDtType {\
    public:\
        A() { stepFn = (stepPtrFn) &A::Step; }\
    protected:\
        void Step(vec4 &v, vec4 &vp);\
        void startData();\
};

DT(Aizawa         )
DT(Arneodo        )
DT(Bouali         )
DT(BrukeShaw      )
DT(ChenCelikovsky )
DT(ChenLee        )
DT(Coullet        )
DT(Dadras         )
DT(DequanLi       )
DT(FourWing       )
DT(FourWing2      )
DT(FourWing3      )
DT(GenesioTesi    )
DT(GloboToroid    )
DT(Halvorsen      )
DT(Hadley         )
DT(LiuChen        )
DT(Lorenz         )
DT(MultiChuaII    )
DT(NewtonLeipnik  )
DT(NoseHoover     )
DT(QiChen         )
DT(Sakarya        )
DT(RayleighBenard )
DT(Robinson       )
DT(Rossler        )
DT(Rucklidge      )
DT(ShimizuMorioka )
DT(SprottLinzB    )
DT(SprottLinzF    )
DT(Tamari         )
DT(Thomas         )
DT(TSUCS          )
DT(YuWang         )
DT(ZhouChen       )

#undef DT

//  Magnetic base class
////////////////////////////////////////////////////////////////////////////
class Magnetic : public attractorVectorK
{
public:

    typedef  const vec3 (Magnetic::*magneticPtrFn)(const vec3 &, int);
    Magnetic() 
    {
        stepFn = (stepPtrFn) &Magnetic::Step;
        kMax = 5.0; kMin = -5.0; vMax = 1.0; vMin = -1.0;

        m_POV = vec3( 0.f, 0.f, 3.f);
    }

    //  innerSteps functions
    ///////////////////////////////////////
    const vec3 straight(const vec3 &vx, int i);
    const vec3 rightShift(const vec3 &vx, int i);
    const vec3 leftShift(const vec3 &vx, int i);
    const vec3 fullPermutated(const vec3 &vx, int i);
    const vec3 tryed(const vec3 &vx, int i);

protected:
    void initStep() {
        resetQueue();
        Insert(vec4(0.f));
        tmpElements = vVal.size();
        stabilize(STABILIZE_DIM);
    }

    void Step(vec4 &v, vec4 &vp);
    void startData();

    //  Additional save vals
    ///////////////////////////////////////
    void saveAdditionalData(Config &cfg);
    void loadAdditionalData(Config &cfg);

    //  Additional Controls
    ///////////////////////////////////////
    void additionalDataCtrls();
    int additionalDataDlg();

    void setElements(const int n)
    { 
        newItemsEnd = true;
        //clear();
        vVal.resize(n); kVal.resize(n); 

        newRandomValues();

        newItemsEnd = false;
    }

    void setElementsA(const int n)
    { 
        newItemsEnd = true;
        const int nMagnets = vVal.size();

        const float _kUP =  kMax/nMagnets;
        const float _kDW =  kMin/nMagnets;

        const float _dUP =  vMax;
        const float _dDW =  vMin;

        int elements=nMagnets;
        while(n > elements) {
            vVal.push_back(vec4(RANDOM(_dDW,_dUP), RANDOM(_dDW,_dUP), RANDOM(_dDW,_dUP), 0.f));
            kVal.push_back(vec4(RANDOM(_kDW,_kUP), RANDOM(_kDW,_kUP), RANDOM(_kDW,_kUP), 0.f)); 
            elements++;
        }
        while(n < elements) {
            vVal.pop_back();
            kVal.pop_back(); 
            elements--;
        }
        //ResizeVectors(); 
        resetQueue(); 
        newItemsEnd = false;
    }

    int getElements() { return vVal.size(); }


    void newRandomValues() 
    {
        for(int j =0; j++<100;) {
            initParams();

            for(int i=0; i++<100;) AttractorBase::Step();

            vec4 v0 = getCurrent();
            vec4 v1 = getPrevious();

            //if(fabs(v0.x-v1.x)<.01 && fabs(v0.y-v1.y)<.01 && fabs(v0.z-v1.z)<.01) 
            if(fabs(v0.x-v1.x)>.01 || fabs(v0.y-v1.y)>.01 || fabs(v0.z-v1.z)>.01) break;
        }
    }
    
    void searchAttractor()  { searchLyapunov(); }

    //thread *th[4];
    //vec3 vth[4],vcurr;

    magneticPtrFn increment;
    bool newItemsEnd = false;

    int tmpElements, nElements;


    void initParams() {

        const int nMagnets = vVal.size();

        const float _kUP =  kMax/nMagnets;
        const float _kDW =  kMin/nMagnets;

        const float _dUP =  vMax;
        const float _dDW =  vMin;

        for(auto &i : vVal) i = vec4(RANDOM(_dDW,_dUP), RANDOM(_dDW,_dUP), RANDOM(_dDW,_dUP), 0.f);
        for(auto &i : kVal) i = vec4(RANDOM(_kDW,_kUP), RANDOM(_kDW,_kUP), RANDOM(_kDW,_kUP), 0.f);

        initStep();
    }
};
//  Magnetic LeftShift
///////////////////////////////////////
class MagneticLeft : public Magnetic {
public:
    MagneticLeft() { increment = &Magnetic::leftShift; }
};
//  Magnetic RightShift
///////////////////////////////////////
class MagneticRight : public Magnetic {
public:
    MagneticRight() { increment = &Magnetic::rightShift; }
};
//  Magnetic Full permuted
///////////////////////////////////////
class MagneticFull : public Magnetic {
public:
    MagneticFull() { increment = &Magnetic::fullPermutated; }
};
//  Magnetic Full permuted
///////////////////////////////////////
class MagneticStraight : public Magnetic {
public:
    MagneticStraight() { increment = &Magnetic::straight; }
};

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
#define ATT_PATH "startData/"
#define ATT_EXT ".sca"

#define PB(ATT, GRAPH_CHAR, COLOR_GRAPH_CHAR, DISPLAY_NAME)\
    ptr.push_back(new ATT());\
    ptr.back()->fileName = (ATT_PATH #ATT ATT_EXT);\
    ptr.back()->nameID = (#ATT);\
    ptr.back()->graphChar = (GRAPH_CHAR);\
    ptr.back()->colorGraphChar = (COLOR_GRAPH_CHAR);\
    ptr.back()->displayName = (DISPLAY_NAME);

#define MAGNETIC_COLOR vec4(0.00f, 1.00f, 0.00f, 1.00f)
#define POLINOM_COLOR  vec4(0.00f, 0.00f, 1.00f, 1.00f)
#define POWER3D_COLOR  vec4(0.00f, 0.50f, 1.00f, 1.00f)
#define RAMPE_COLOR    vec4(1.00f, 0.00f, 1.00f, 1.00f)
#define PICKOVER_COLOR vec4(0.00f, 1.00f, 1.00f, 1.00f)
#define PORTED3D_COLOR vec4(1.00f, 0.00f, 0.00f, 1.00f)
#define DLA3D_COLOR    vec4(0.00f, 0.70f, 0.00f, 1.00f)
#define DT_COLOR       vec4(1.00f, 1.00f, 0.00f, 1.00f)
#define FRACTAL_COLOR  vec4(1.00f, 0.50f, 0.00f, 1.00f)

class AttractorsClass 
{
public:
    AttractorsClass() {
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
//        PB(Hopalong        , "Hopalong"         )

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

