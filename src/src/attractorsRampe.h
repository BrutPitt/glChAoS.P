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