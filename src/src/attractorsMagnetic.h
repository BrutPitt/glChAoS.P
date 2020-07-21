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
