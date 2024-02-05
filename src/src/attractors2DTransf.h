//------------------------------------------------------------------------------
//  Copyright (c) 2018-2024 Michele Morrone
//  All rights reserved.
//
//  https://michelemorrone.eu - https://glchaosp.com - https://brutpitt.com
//
//  X: https://x.com/BrutPitt - GitHub: https://github.com/BrutPitt
//
//  direct mail: brutpitt(at)gmail.com - me(at)michelemorrone.eu
//
//  This software is distributed under the terms of the BSD 2-Clause license
//------------------------------------------------------------------------------
#pragma once

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

class Hopalong3D : public attractorScalarIterateZ
{
public:

    Hopalong3D() {
        stepFn = (stepPtrFn) &Hopalong3D::Step;

        vMin = -1.0; vMax = 1.0; kMin = -1.0; kMax = 1.0;
        m_POV = vec3( 0.f, 0, 50.f);
    }

    void initStep() { zMin = kVal[2]; zMax = kVal[3]; zIter=100000; attractorScalarIterateZ::initStep(); }

    int getPtSize() { return attPt4D; }
protected:
    void Step(vec4 &v, vec4 &vp);
    void startData();
    //void searchAttractor()  { searchLyapunov(); }
};

//  Kaneko3D
////////////////////////////////////////////////////////////////////////////
class Kaneko3D : public attractorScalarIterateZ
{
public:

    Kaneko3D() {
        stepFn = (stepPtrFn) &Kaneko3D::Step;

        vMin = -10.0; vMax = 10.0; kMin = -1.0; kMax = 1.0;

        m_POV = vec3( 0.f, 0, 12.f);
    }

    void initStep() { zMin = kVal[1]; zMax = kVal[2]; zIter=1000; attractorScalarIterateZ::initStep(); }

protected:
    void Step(vec4 &v, vec4 &vp);
    void startData();
};

//  Henon3D
////////////////////////////////////////////////////////////////////////////
class Henon3D : public attractorScalarIterateZ
{
public:

    Henon3D() {
        stepFn = (stepPtrFn) &Henon3D::Step;

        vMin = -10.0; vMax = 10.0; kMin = -1.0; kMax = 1.0;

        m_POV = vec3( 0.f, 0, 12.f);
    }

    void initStep() { zMin = kVal[1]; zMax = kVal[2]; zIter=1000; attractorScalarIterateZ::initStep(); }

protected:
    void Step(vec4 &v, vec4 &vp);
    void startData();
};
