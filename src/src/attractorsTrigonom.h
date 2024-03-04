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
