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

// stochastic adaptation of P.Nylander's Mathematica formula of JuliaBulb set
// http://bugman123.com/Hypercomplex/index.html
////////////////////////////////////////////////////////////////////////////
void juliaBulb_IIM::Step(vec4 &v, vec4 &vp)
{
    auto radiciEq = [&](const vec3 &p, float sign1, float sign2) {
        const float xQ = p.x * p.x, yQ = p.y * p.y, zQ = p.z * p.z;
        const float r = sqrtf(xQ+yQ+zQ);
        const float a = sign1 * sqrtf(yQ + xQ*(2.f+zQ/(xQ+yQ)) - 2.f*p.x*r);
        const float b = sign2 * sqrtf(r - p.x - a) * .5f;
        const float c = yQ*yQ + xQ * (yQ - zQ);
        const float d = a * (p.x * (r+p.x) + yQ);
        vp = vec4(b * ((p.x*yQ-d) * (xQ+yQ) - p.x*xQ*zQ) / (p.y*c),
                  b,
                  -p.z/sqrtf(2 * (r - d *(xQ+yQ)/c)),
                  0.f);
        if(ifsPoint.active()) vp *= getIFSvec4(ifsPoint); // IFS point transforms
    };

    auto mainFunc = [&](vec4 &v, vec4 &vp) {
        const uint32_t rnd = fRnd64.xorShift();
        const vec4 c = ifsParam.active() ? kRnd+getIFSvec4(ifsParam) : kRnd; // IFS param transforms
        vec4 tmpV((vec3)v-((vec3 )*kVal.data()+(vec3)c));

        radiciEq(tmpV, (rnd&1) ? 1.f : -1.f, (rnd&2) ? 1.f : -1.f);
    };

    preStep(v);
    while(depth++<skipTop) { mainFunc(v,vp); v = vp; } // skip first "skipTop" points

    mainFunc(v,vp);

}
// stochastic adaptation of P.Nylander's Mathematica formula of JuliaBulb set
// http://bugman123.com/Hypercomplex/index.html
////////////////////////////////////////////////////////////////////////////
void juliaBulb4th_IIM::Step(vec4 &v, vec4 &vp)
{
    auto radiciEq = [&](const vec3 &p, int kTheta, int kPhi) {
        const float r = length(p);
        const int absOrder = abs(degreeN);
        const int k1 = (absOrder - (p.z<0 ? 0 : 1)), k2 = (3*absOrder + (p.z<0 ? 4 : 2));

        const int dk = ((absOrder%2)==0 && (kPhi<<2)>k1 && (kPhi<<2)<k2) ? (sign(p.z) * ( (absOrder % 4) ? 1 : -1)) : 0;
        const float theta  = (atan2f(p.y,p.x) + (2 * kTheta + dk) * T_PI) / float(degreeN);
        const float phi    = (asinf(p.z/r)    + (2 * kPhi   - dk) * T_PI) / float(degreeN);
        const float cosphi = cosf(phi);
        vp = vec4(powf(r, 1.0f/float(degreeN)) * vec3(cosf(theta)*cosphi,sinf(theta)*cosphi,sinf(phi)), 0.f);
        if(ifsPoint.active()) vp *= getIFSvec4(ifsPoint);
    };

    auto mainFunc = [&](vec4 &v, vec4 &vp) {
        const vec4 c = ifsParam.active() ? kRnd+getIFSvec4(ifsParam) : kRnd; // IFS param transforms
        radiciEq((vec3)v-((vec3 &)*kVal.data()+(vec3)c), fRnd64.xorShift() % degreeN, fRnd64.xorShift() % degreeN);
    };

    preStep(v);
    while(depth++<skipTop) { mainFunc(v,vp); v = vp; } // skip first "skipTop" points

    mainFunc(v,vp);

}

// stochastic adaptation of P.Nylander's Mathematica formula of quaternion Julia set
// http://bugman123.com/Hypercomplex/index.html
////////////////////////////////////////////////////////////////////////////

void quatJulia_IIM::Step(vec4 &v, vec4 &vp)
{
    auto radiciEq = [&](const vec4 &p, float sign) {
        const float xQ = p.x * p.x, yQ = p.y * p.y, zQ = p.z * p.z, wQ = p.w * p.w;
        const float r = sqrtf(xQ + yQ + zQ + wQ);
        const float a = sqrtf((p.x+r)*.5);
        const float b = (r-p.x) * a / (yQ + zQ + wQ);
        vp = sign * vec4(a, b*p.y, b*p.z, b*p.w);
        if(ifsPoint.active()) vp *= getIFSvec4(ifsPoint); // IFS point transforms
    };

    auto mainFunc = [&](vec4 &v, vec4 &vp) {
        const vec4 c = ifsParam.active() ? kRnd+getIFSvec4(ifsParam) : kRnd; // IFS param transforms
        vec4 tmpV(v-((vec4 &)*kVal.data()+c));
        radiciEq(tmpV, (fRnd64.xoroshiro128xx()&1) ? 1.f : -1.f);
    };

    preStep(v);
    while(depth++<skipTop) { mainFunc(v,vp); v = vp; } // skip first "skipTop" points

    mainFunc(v,vp);
}




// stochastic adaptation of P.Nylander's Mathematica formula of quaternion Julia set
// http://bugman123.com/Hypercomplex/index.html
////////////////////////////////////////////////////////////////////////////
void glynnJB_IIM::Step(vec4 &v, vec4 &vp)
{
//Glynn roots: {x,y,z}^(1/1.5) = {x,y,z}^(2/3)

    const vec3 p = vec3(v)-(vec3(kVal[0], kVal[1], kVal[2])+vec3(kRnd)); //    ((vec3 &)*kVal.data()+(vec3)kRnd);
    const float xQ = p.x * p.x, yQ = p.y * p.y, zQ = p.z * p.z;
    const float r = sqrtf(xQ+yQ+zQ);
    const uint32_t rnd = fastRandom.KISS();
    const uint32_t rndR = fastRandom.KISS();
    const int px = rnd&0x10;
    const int py = rnd&0x20;
    const int pz = rnd&0x40;
    const int pw = rnd&0x80;
    //const bool isCone = bool(zQ > xQ + yQ);
    const int isCone = rnd&1;
/*
    auto powN = [&] (vec3 &p, float n) -> vec3 {
        const float theta = n * atan2f(p.x, p.y);
        const float phi = n * asinf(p.z/r);
        return pow(vec3(r), r*vec3(cosf(theta) * cosf(phi), sinf(theta) * cosf(phi), sinf(phi)));
    };
*/
    auto numRadici = [&] () -> int {
        return (isCone) ? rndR%3 : (px ? rndR%2 : 0);
    };

    auto radiciEq = [&] (int k) {
        const float n = kVal[3];

        float ktheta, kphi;
        const float uno = kVal[4], due = kVal[5], zeroCinque = kVal[6], dueCinque = kVal[7];

        if(!k) { ktheta = kphi = 0.0; }
        else {
            if(isCone) { ktheta=(py ? due:uno); kphi=0.0; }
            else {
                if(k==1) {
                    if(pw) {ktheta=due; kphi=0.0;} else {ktheta=zeroCinque; kphi=(pz?zeroCinque:dueCinque);}
                }
                else {
                    if(pw) {ktheta=uno; kphi=0.0;} else {ktheta=dueCinque; kphi=(pz?zeroCinque:dueCinque);}
                }
            }
        }

        const float theta  = (atan2f(p.y,p.x)+ ktheta * T_PI)/n;
        const float phi    = (asinf(p.z/(r == 0.f ? FLT_EPSILON : r))   + kphi   * T_PI)/n;

        const float cosphi = cosf(phi);
        vp = vec4(powf(r,1.0f/n) * vec3(cosf(theta)*cosphi,sinf(theta)*cosphi,sinf(phi)), 0.f);
    };



    preStep(v);
    int nRad = numRadici();
    radiciEq( nRad);
}


/*
void glynnJB_IIM::Step(vec4 &v, vec4 &vp)
{
//Glynn roots: {x,y,z}^(1/1.5) = {x,y,z}^(2/3)

    const vec3 p = vec3(v)-(vec3(kVal[0], kVal[1], kVal[2])+vec3(kRnd)); //    ((vec3 &)*kVal.data()+(vec3)kRnd);
    const float xQ = p.x * p.x, yQ = p.y * p.y, zQ = p.z * p.z;
    const float r = sqrtf(xQ+yQ+zQ);
    const bool isCone = bool(zQ > xQ + yQ);

    auto powN = [&] (vec3 &p, float n) -> vec3 {
        const float theta = n * atan2f(p.x, p.y);
        const float phi = n * asinf(p.z/r);
        return pow(vec3(r), r*vec3(cosf(theta) * cosf(phi), sinf(theta) * cosf(phi), sinf(phi)));
    };

    auto numRadici = [&] () -> int {
        const uint32_t rnd = fastRandom.KISS();
        return (isCone) ? rnd%3 : (p.x > 0 ? rnd%2 : 0);
    };

    auto radiciEq = [&] (int k) {
        const float n = 1.5f;
        const uint32_t rnd = fastRandom.KISS();

        float ktheta, kphi;

        if(!k) { ktheta = kphi = 0.0; }
        else {
            if(!isCone) { ktheta=(p.y<0 ? 2.0:1.0); kphi=0.0; }
            else {
                if(k==1) {
                    if(p.x<0 && p.y<0) {ktheta=2.0; kphi=0.0;} else {ktheta=0.5; kphi=(p.z>0?0.5:2.5);}
                }
                else {
                    if(p.x<0 && p.y>0) {ktheta=1.0; kphi=0.0;} else {ktheta=2.5; kphi=(p.z>0?0.5:2.5);}
                }
            }
        }

        const float theta  = (atan2f(p.y,p.x)+ ktheta * T_PI)/n;
        const float phi    = (asinf(p.z/(r == 0.f ? FLT_EPSILON : r))   + kphi   * T_PI)/n;

        const float cosphi = cosf(phi);
        vp = vec4(powf(r,1.0f/n) * vec3(cosf(theta)*cosphi,sinf(theta)*cosphi,sinf(phi)), 0.f);
    };


    preStep(v);
    int nRad = numRadici();
    const uint32_t rnd = fastRandom.KISS();
    radiciEq( nRad);

}
*/


