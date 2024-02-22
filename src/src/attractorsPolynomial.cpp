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
#include "glWindow.h"

//  Polynomial power of N
////////////////////////////////////////////////////////////////////////////
void PowerN3D::Step(vec4 &v, vec4 &vp)
{
    elv[0] = vec3(1.f);
    for(int i=1; i<=order; i++) elv[i] = elv[i-1] * (vec3)v;

    auto itCf = cf.begin();
    for(int x=order-1; x>=0; x--)
        for(int y=order-1; y>=0; y--)
            for(int z=order-1; z>=0; z--)
                if(x+y+z <= order) *itCf++ = elv[x].x * elv[y].y * elv[z].z;

    *itCf++ = elv[order].x;
    *itCf++ = elv[order].y;
    *itCf++ = elv[order].z;

    vec3 vt(0.f);

    itCf = cf.begin();
    for(auto &it : kVal) vt += (vec3)it * *itCf++;

    vp = vec4(vt, 0.f);
}

void tetrahedronGaussMap::Step(vec4 &v, vec4 &vp)
{
    const float rnd = fastPrng64.xoroshiro128p_VNI<float>();
    if(rnd<.2) {
        const float x2 = v.x*v.x, y2 = v.y*v.y;
        const float coeff = 1.f / (1.f+x2+y2);
        vp = vec3(2.f*v.x, 2.f*v.y, 1.f-x2-y2) * coeff + (vec3)kVal[0];
    } else if(rnd<.4) {
        const float z2 = v.z*v.z, y2 = v.y*v.y;
        const float coeff = 1.f / (1.f+z2+y2);
        vp = vec3(1.f-z2-y2, 2.f*v.y, 2.f*v.z) * coeff + (vec3)kVal[1];
    } else if(rnd<.6) {
        const float x2 = v.x*v.x, z2 = v.z*v.z;
        const float coeff = 1.f / (1.f+x2+z2);
        vp = vec3(2.f*v.x, 1.f-x2-z2, 2.f*v.z) * coeff + (vec3)kVal[2];
    } else if(rnd<.8) {
        vp = v * .5;
    } else {
        vp = v * .5 + kVal[3];
    }
}

//  Polynomial attractors
////////////////////////////////////////////////////////////////////////////
void PolynomialA::Step(vec4 &v, vec4 &vp)
{   vp = vec3(v.y - v.y*v.z, v.z - v.x*v.z, v.x - v.x*v.y) + (vec3) kVal[0]; }
////////////////////////////////////////////////////////////////////////////
void PolynomialB::Step(vec4 &v, vec4 &vp)
{   vp = vec3(v.y-v.z*(kVal[1].x+v.y), v.z-v.x*(kVal[1].y+v.z), v.x-v.y*(kVal[1].z+v.x)) + (vec3) kVal[0]; }
////////////////////////////////////////////////////////////////////////////
void PolynomialC::Step(vec4 &v, vec4 &vp)
{
    vp.x = kVal[0].x +v.x*(kVal[1].x +kVal[2].x *v.x+kVal[3].x *v.y)+v.y*(kVal[4].x+kVal[5].x*v.y);
    vp.y = kVal[0].y +v.y*(kVal[1].y +kVal[2].y *v.y+kVal[3].y *v.z)+v.z*(kVal[4].y+kVal[5].y*v.z);
    vp.z = kVal[0].z +v.z*(kVal[1].z +kVal[2].z *v.z+kVal[3].z *v.x)+v.x*(kVal[4].z+kVal[5].z*v.x);
}
////////////////////////////////////////////////////////////////////////////
void PolynomialABS::Step(vec4 &v, vec4 &vp)
{
    vp.x = kVal[0].x + kVal[1].x*v.x + kVal[2].x*v.y + kVal[3].x*v.z + kVal[4].x*abs(v.x) + kVal[5].x*abs(v.y) +kVal[6].x*abs(v.z);
    vp.y = kVal[0].y + kVal[1].y*v.x + kVal[2].y*v.y + kVal[3].y*v.z + kVal[4].y*abs(v.x) + kVal[5].y*abs(v.y) +kVal[6].y*abs(v.z);
    vp.z = kVal[0].z + kVal[1].z*v.x + kVal[2].z*v.y + kVal[3].z*v.z + kVal[4].z*abs(v.x) + kVal[5].z*abs(v.y) +kVal[6].z*abs(v.z);
}
////////////////////////////////////////////////////////////////////////////
void PolynomialPow::Step(vec4 &v, vec4 &vp)
{
    vp.x = kVal[0].x + kVal[1].x*v.x + kVal[2].x*v.y + kVal[3].x*v.z + kVal[4].x*abs(v.x) + kVal[5].x*abs(v.y) +kVal[6].x*pow(abs(v.z),kVal[7].x);
    vp.y = kVal[0].y + kVal[1].y*v.x + kVal[2].y*v.y + kVal[3].y*v.z + kVal[4].y*abs(v.x) + kVal[5].y*abs(v.y) +kVal[6].y*pow(abs(v.z),kVal[7].y);
    vp.z = kVal[0].z + kVal[1].z*v.x + kVal[2].z*v.y + kVal[3].z*v.z + kVal[4].z*abs(v.x) + kVal[5].z*abs(v.y) +kVal[6].z*pow(abs(v.z),kVal[7].z);
}
////////////////////////////////////////////////////////////////////////////
void PolynomialSin::Step(vec4 &v, vec4 &vp)
{
    vp.x = kVal[0].x + kVal[1].x*v.x + kVal[2].x*v.y + kVal[3].x*v.z + kVal[4].x*sin(kVal[5].x*v.x) + kVal[6].x*sin(kVal[7].x*v.y) +kVal[8].x*sin(kVal[9].x*v.z);
    vp.y = kVal[0].y + kVal[1].y*v.x + kVal[2].y*v.y + kVal[3].y*v.z + kVal[4].y*sin(kVal[5].y*v.x) + kVal[6].y*sin(kVal[7].y*v.y) +kVal[8].y*sin(kVal[9].y*v.z);
    vp.z = kVal[0].z + kVal[1].z*v.x + kVal[2].z*v.y + kVal[3].z*v.z + kVal[4].z*sin(kVal[5].z*v.x) + kVal[6].z*sin(kVal[7].z*v.y) +kVal[8].z*sin(kVal[9].z*v.z);
}
