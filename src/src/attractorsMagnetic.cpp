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
void Magnetic::Step(vec4 &v, vec4 &vp)
{
    vec3 vt(0.f);

    int i = 0;
    for(auto itPt = vVal.begin(), itK = kVal.begin(); !newItemsEnd && itPt!=vVal.end();) {
        const vec3 vo(*itPt++ - v); //vo.z=0.0;
        const float dotProd = dot(vo,vo);
        if(dotProd > FLT_EPSILON) vt+=(this->*increment)(vec3(vec3(*itK++) * (vo / dotProd)),i++);
    }

    vp = vec4(vt, 0.f);
}

const vec3 Magnetic::straight(const vec3 &vx, int i)
{
    return vx;
}

const vec3 Magnetic::rightShift(const vec3 &vx, int i)
{
    switch(i%3) {
        case 0 :  return vx;
        case 1 :  return vec3(vx.z,vx.x,vx.y);
        case 2 :  return vec3(vx.y,vx.z,vx.x);
    }

    return vx;
}

const vec3 Magnetic::leftShift(const vec3 &vx, int i)
{
    switch(i%3) {
        case 0 :  return vx;
        case 1 :  return vec3(vx.y,vx.z,vx.x);
        case 2 :  return vec3(vx.z,vx.x,vx.y);
    }
    return vx;
}
const vec3 Magnetic::fullPermutated(const vec3 &vx, int i)
{
    switch(i%6) {
        case 0 :  return vx;
        case 1 :  return vec3(vx.y,vx.z,vx.x);
        case 2 :  return vec3(vx.z,vx.x,vx.y);
        case 3 :  return vec3(vx.x,vx.z,vx.y);
        case 4 :  return vec3(vx.z,vx.y,vx.x);
        case 5 :  return vec3(vx.y,vx.x,vx.z);
    }
    return vx;
}

const vec3 Magnetic::tryed(const vec3 &vx, int i)
{
    switch(i%3) {
        case 0 :  return vx;
        case 1 :  return vec3(vx.x,sinf(vx.y/T_PI),cosf(vx.z/pi<float>()));
        case 2 :  return vec3(vx.x,cosf(vx.y/T_PI),sinf(vx.z/pi<float>()));
    }
    return vx;
}
