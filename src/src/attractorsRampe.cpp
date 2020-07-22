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

inline float clampNormalized(float x) { return x>1.f ? 1.f : x<-1.f ? -1.f : x; }

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
    vp.x = v.z*sin(kVal[0].x*v.x)+acos(clampNormalized(kVal[1].x*v.y));
    vp.y = v.x*sin(kVal[0].y*v.y)+acos(clampNormalized(kVal[1].y*v.z));
    vp.z = v.y*sin(kVal[0].z*v.z)+acos(clampNormalized(kVal[1].z*v.x));
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
    vp.x = v.z*sin(kVal[0].x*v.x)-acos(clampNormalized(kVal[1].x*v.y))+sin(kVal[2].x*v.z);
    vp.y = v.x*sin(kVal[0].y*v.x)-acos(clampNormalized(kVal[1].y*v.y))+sin(kVal[2].y*v.z);
    vp.z = v.y*sin(kVal[0].z*v.x)-acos(clampNormalized(kVal[1].z*v.y))+sin(kVal[2].z*v.z);
}
////////////////////////////////////////////////////////////////////////////
void Rampe10::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.z*v.y*sin(kVal[0].x*v.x)-cos(kVal[1].x*v.y)+asin(clampNormalized(kVal[2].x*v.z));
    vp.y = v.x*v.z*sin(kVal[0].y*v.x)-cos(kVal[1].y*v.y)+ sin(kVal[2].y*v.z);
    vp.z = v.y*v.x*sin(kVal[0].z*v.x)-cos(kVal[1].z*v.y)+ sin(kVal[2].z*v.z);
}
