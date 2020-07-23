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
#include "glWindow.h"

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

