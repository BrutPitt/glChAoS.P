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

//  Polynomial power of N
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


//  Polynomial attractors
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
