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

class fractalIIMBase : public attractorScalarK
{
public:
    bool ifsActive() { return ifsParam.active() || ifsPoint.active(); }
    ifsBaseClass *getIFSParam() { return &ifsParam; }
    ifsBaseClass *getIFSPoint() { return &ifsPoint; }

    vec4 getIFSvec4(ifsBaseClass &ifs) {
        if(ifs.getCurrentTransform()<0) return vec4(ifs.getInitValue()); // no active transforms: neutral value
        else {
            ifsDataStruct *ifsStruct = ifs.getTransfStruct(ifs.getCurrentTransform());
            return ifsStruct->variationFunc(ifsStruct->variations) * ifsStruct->variationFactor;
        }
    }
    static fastXS64 fRnd64;

protected:
    fractalIIMBase() {
        vMin = 0.f; vMax = 0.f; kMin = 0.f; kMax = 0.f;
        m_POV = vec3( 0.f, 0, 7.f);
        setFractalType();
    }

    inline void refreshRandoms(vec4 &v) {
            depth = 0;
            v = vVal[0] + (vMin == vMax ? vec4(vMin) :
                   vec4(fRnd64.xoroshiro128p_Range(vMin, vMax),
                        fRnd64.xoroshiro128p_Range(vMin, vMax),
                        fRnd64.xoroshiro128p_Range(vMin, vMax),
                        0.f /*fRnd64.xoroshiro128p_Range(vMin, vMax)*/));

            kRnd = kMin == kMax ? vec4(kMin) :
                   vec4(fRnd64.xoroshiro128p_Range(kMin, kMax),
                        fRnd64.xoroshiro128p_Range(kMin, kMax),
                        fRnd64.xoroshiro128p_Range(kMin, kMax),
                        0.f /*fRnd64.xoroshiro128p_Range(kMin, kMax)*/);
    }

    inline void preStep(vec4 &v) { if(depth++>maxDepth) refreshRandoms(v); }

    //  Specific attractor values
    void saveAdditionalData(Config &cfg);
    void loadAdditionalData(Config &cfg);

    virtual void additionalDataCtrls();
    void drawDepthSkipButtons();

    vec4 kRnd = vec4(0.f);
    vec4 vIter;

    int maxDepth = 50;
    int degreeN = 2;
    int skipTop = 10;

    int depth = 0;

    ifsBaseClass ifsPoint = ifsBaseClass(1.f), ifsParam = ifsBaseClass(0.f);
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

    inline void radiciBicomplex(const vec4 &pt, vec4 &vp)
    {
        //static fastXS64_mt fff;
        //fff.jump_xoroshiro128p();
        const uint32_t rnd = fRnd64.xoroshiro128p();
        const float sign1 = (rnd&2) ? 1.f : -1.f, sign2 = (rnd&1) ? 1.f : -1.f;
        const vec4 c = ifsParam.active() ? kRnd+getIFSvec4(ifsParam) : kRnd; // IFS param transforms
        const vec4 p(pt - ((vec4 &)*kVal.data()+c));

        const std::complex<float> z1(p.x, p.y), z2(-p.w, p.z);
        const std::complex<float> w1 = sign1 * sqrt(z1 - z2), w2 = sign2 *sqrt(z1 + z2);
        vp = vec4(w1.real()+w2.real(), w1.imag()+w2.imag(), w2.imag()-w1.imag(), w1.real()-w2.real())*.5f;
        if(ifsPoint.active()) vp *= getIFSvec4(ifsPoint); // IFS point transforms
    };

    void Step(vec4 &v, vec4 &vp) { vec4 pt = getVec(v); mainFunc(pt, vp); }
    inline void mainFunc(vec4 &v, vec4 &vp) {
        preStep(v);
        while(depth++<skipTop) {
            radiciBicomplex(getVec(v), vp);
            v = vp;
        }
        radiciBicomplex(getVec(v),vp);
    }

    virtual vec4 getVec(const vec4 &v) { return v; }


};

/////////////////////////////////////////////////
class BicomplexJ_IIM : public BicomplexBase
{
public:
    BicomplexJ_IIM() { stepFn = (stepPtrFn) &BicomplexJ_IIM::Step; }

protected:
    //void Step(vec4 &v, vec4 &vp) { preStep(v); radiciBicomplex(vec4(v, last4D), vp); } //remove
};

/////////////////////////////////////////////////
class BicomplexJMod0_IIM : public BicomplexBase
{
public:
    BicomplexJMod0_IIM() { stepFn = (stepPtrFn) &BicomplexJMod0_IIM::Step; }

protected:
    //void Step(vec4 &v, vec4 &vp) { preStep(v); radiciBicomplex(vec4(v, dim4D), vp); } remove
    vec4 getVec(const vec4 &v) { return vec4((vec3)v, vVal[0].w); }
};

/////////////////////////////////////////////////
class BicomplexJMod1_IIM : public BicomplexBase
{
public:
    BicomplexJMod1_IIM() { stepFn = (stepPtrFn) &BicomplexJMod1_IIM::Step; }

protected:
    //void Step(vec4 &v, vec4 &vp) { preStep(v); radiciBicomplex(vec4(v.x, v.y, vVal[0].z, last4D), vp); } remove
    vec4 getVec(const vec4 &v) { return vec4(v.x, v.y, vVal[0].z, v.w); }
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
    vec4 getVec(const vec4 &v) { return vec4(v.x, v.y, v.z, v.y); }
};

/////////////////////////////////////////////////
class BicomplexJMod3_IIM : public BicomplexBase
{
public:
    BicomplexJMod3_IIM() { stepFn = (stepPtrFn) &BicomplexJMod3_IIM::Step; }

protected:
    vec4 getVec(const vec4 &v) { return vec4(vVal[0].x, v.y, v.z, v.w); }
};
/////////////////////////////////////////////////
class BicomplexJMod4_IIM : public BicomplexBase
{
public:
    BicomplexJMod4_IIM() { stepFn = (stepPtrFn) &BicomplexJMod4_IIM::Step; }

protected:
    vec4 getVec(const vec4 &v) { return vec4(v.x, v.y, v.x, v.w); }
};
/////////////////////////////////////////////////
class BicomplexJMod5_IIM : public BicomplexBase
{
public:
    BicomplexJMod5_IIM() { stepFn = (stepPtrFn) &BicomplexJMod5_IIM::Step; }

protected:
    vec4 getVec(const vec4 &v) { return vec4( v.y, v.x, v.w, v.z); }
};
/////////////////////////////////////////////////
class BicomplexJMod6_IIM : public BicomplexBase
{
public:
    BicomplexJMod6_IIM() { stepFn = (stepPtrFn) &BicomplexJMod6_IIM::Step; }

protected:
    vec4 getVec(const vec4 &v) { return vec4(vVal[0].x, vVal[0].y, v.z, v.w); }
};
/////////////////////////////////////////////////
class BicomplexJMod7_IIM : public BicomplexBase
{
public:
    BicomplexJMod7_IIM() { stepFn = (stepPtrFn) &BicomplexJMod7_IIM::Step; }

protected:
    //void Step(vec4 &v, vec4 &vp) { radiciBicomplex(vec4( v.x, v.x, v.z, v.x), vp); }
    vec4 getVec(const vec4 &v) { return vec4( v.x, v.x, v.z, vVal[0].w); }
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
        preStep(v);
        while(depth++<skipTop) {
            vt = v;
            radiciBicomplex(getVec(v), vp);
            v = vp;
        }
        vt = v;
        radiciBicomplex(getVec(v), vp);
    }
    vec4 getVec(const vec4 &v) { return vec4( *a1[idx0], *a2[idx1], *a3[idx2], *a4[idx3]); }
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

