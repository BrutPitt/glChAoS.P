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

////////////////////////////////////////////////////////////////////////////
void Lorenz::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.x+dtStepInc*(kVal[0]*(v.y-v.x));
    vp.y = v.y+dtStepInc*(v.x*(kVal[1]-v.z)-v.y);
    vp.z = v.z+dtStepInc*(v.x*v.y-kVal[2]*v.z);
}
////////////////////////////////////////////////////////////////////////////
void ChenCelikovsky::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.x + dtStepInc*(kVal[0]*(v.y-v.x));
    vp.y = v.y + dtStepInc*(-v.x*v.z + kVal[2]*v.y);
    vp.z = v.z + dtStepInc*( v.x*v.y - kVal[1]*v.z);
}
////////////////////////////////////////////////////////////////////////////
void ChenLee::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.x + dtStepInc*(kVal[0]*v.x - v.y*v.z);
    vp.y = v.y + dtStepInc*(kVal[1]*v.y + v.x*v.z);
    vp.z = v.z + dtStepInc*(kVal[2]*v.z + v.x*v.y/3.f);
}

////////////////////////////////////////////////////////////////////////////
void TSUCS::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.x + dtStepInc*(kVal[0]*(v.y - v.x) + kVal[3]*v.x*v.z);
    vp.y = v.y + dtStepInc*(kVal[1]*v.x + kVal[5]*v.y - v.x*v.z);
    vp.z = v.z + dtStepInc*(kVal[2]*v.z + v.x*v.y - kVal[4]*v.x*v.x);
}
////////////////////////////////////////////////////////////////////////////
void Aizawa::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.x + dtStepInc*((v.z-kVal[1])*v.x - kVal[3]*v.y);
    vp.y = v.y + dtStepInc*((v.z-kVal[1])*v.y + kVal[3]*v.x);
    const float xQ = v.x*v.x;
    vp.z = v.z + dtStepInc*(kVal[2] + kVal[0]*v.z - (v.z*v.z*v.z)/3.f - (xQ + v.y*v.y) * (1.f + kVal[4]*v.z) + kVal[5]*v.z*xQ*v.x);
}
////////////////////////////////////////////////////////////////////////////
void YuWang::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.x + dtStepInc*(kVal[0]*(v.y -v.x));
    vp.y = v.y + dtStepInc*(kVal[1]*v.x - kVal[2]*v.x*v.z);
    vp.z = v.z + dtStepInc*(exp(v.x*v.y) - kVal[3]*v.z);
}
////////////////////////////////////////////////////////////////////////////
void FourWing::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.x + dtStepInc*(kVal[0]*v.x - kVal[1]*v.y*v.z);
    vp.y = v.y + dtStepInc*(v.x*v.z - kVal[2]*v.y);
    vp.z = v.z + dtStepInc*(kVal[4]*v.x - kVal[3]*v.z + v.x*v.y);
}
////////////////////////////////////////////////////////////////////////////
void FourWing2::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.x + dtStepInc*(kVal[0]*v.x + kVal[1]*v.y + kVal[2]*v.y*v.z);
    vp.y = v.y + dtStepInc*(kVal[3]*v.y - v.x*v.z);
    vp.z = v.z + dtStepInc*(kVal[4]*v.z + kVal[5]*v.x*v.y);
}
////////////////////////////////////////////////////////////////////////////
void FourWing3::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.x + dtStepInc*(kVal[0]*v.x + kVal[1]*v.y + kVal[2]*v.y*v.z);
    vp.y = v.y + dtStepInc*(kVal[3]*v.y*v.z - kVal[4]*v.x*v.z);
    vp.z = v.z + dtStepInc*(1.f - kVal[5]*v.z - kVal[6]*v.x*v.y);
}
////////////////////////////////////////////////////////////////////////////
void Thomas::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.x + dtStepInc*(-kVal[0]*v.x + sinf(v.y));
    vp.y = v.y + dtStepInc*(-kVal[1]*v.y + sinf(v.z));
    vp.z = v.z + dtStepInc*(-kVal[2]*v.z + sinf(v.x));
}
////////////////////////////////////////////////////////////////////////////
void Halvorsen::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.x + dtStepInc*(-kVal[0]*v.x - 4.f*v.y - 4.f*v.z - v.y*v.y);
    vp.y = v.y + dtStepInc*(-kVal[1]*v.y - 4.f*v.z - 4.f*v.x - v.z*v.z);
    vp.z = v.z + dtStepInc*(-kVal[2]*v.z - 4.f*v.x - 4.f*v.y - v.x*v.x);
}
////////////////////////////////////////////////////////////////////////////
void Arneodo::Step(vec4 &v, vec4 &vp)
{ // kVal[] -> a,b,c
    vp.x = v.x + dtStepInc*v.y;
    vp.y = v.y + dtStepInc*v.z;
    vp.z = v.z + dtStepInc*(-kVal[0]*v.x - kVal[1]*v.y - v.z + kVal[2]*v.x*v.x*v.x);
}
////////////////////////////////////////////////////////////////////////////
void Bouali::Step(vec4 &v, vec4 &vp)
{ // kVal[] -> a,b,c,s,alfa,beta
    vp.x = v.x + dtStepInc*( v.x*(kVal[0] - v.y) + kVal[4]*v.z);
    vp.y = v.y + dtStepInc*(-v.y*(kVal[1] - v.x*v.x));
    vp.z = v.z + dtStepInc*(-v.x*(kVal[2] - kVal[3]*v.z) - kVal[5]*v.z);
}
////////////////////////////////////////////////////////////////////////////
void BrukeShaw::Step(vec4 &v, vec4 &vp)
{ // kVal[] -> a,b,c,s,alfa,beta
    vp.x = v.x + dtStepInc*(-kVal[0] * (v.x + v.y));
    vp.y = v.y + dtStepInc*(-v.y -kVal[0]*v.x*v.z);
    vp.z = v.z + dtStepInc*(kVal[0]*v.x*v.y + kVal[1]);
}
////////////////////////////////////////////////////////////////////////////
void Hadley::Step(vec4 &v, vec4 &vp)
{ // kVal[] -> a,b,f,g
    vp.x = v.x + dtStepInc*(-v.y*v.y -v.z*v.z -kVal[0]*v.x + kVal[0]*kVal[2]);
    vp.y = v.y + dtStepInc*(v.x*v.y - kVal[1]*v.x*v.z - v.y + kVal[3]);
    vp.z = v.z + dtStepInc*(kVal[1]*v.x*v.y + v.x*v.z - v.z);
}
////////////////////////////////////////////////////////////////////////////
void LiuChen::Step(vec4 &v, vec4 &vp)
{ // kVal[] -> a,b,c,d,e,f,g
     vp.x = v.x + dtStepInc*(kVal[0]*v.y + kVal[1]*v.x + kVal[2]*v.y*v.z);
     vp.y = v.y + dtStepInc*(kVal[3]*v.y - v.z + kVal[4]*v.x*v.z);
     vp.z = v.z + dtStepInc*(kVal[5]*v.z + kVal[6]*v.x*v.y);
}
////////////////////////////////////////////////////////////////////////////
void GenesioTesi::Step(vec4 &v, vec4 &vp)
{ // kVal[] -> a,b,c
    vp.x = v.x + dtStepInc*v.y;
    vp.y = v.y + dtStepInc*v.z;
    vp.z = v.z + dtStepInc*(-kVal[2]*v.x - kVal[1]*v.y - kVal[0]*v.z + v.x*v.x);
}
////////////////////////////////////////////////////////////////////////////
void NewtonLeipnik::Step(vec4 &v, vec4 &vp)
{ // kVal[] -> a,b
    vp.x = v.x + dtStepInc*(-kVal[0]*v.x + v.y + 10.f*v.y*v.z);
    vp.y = v.y + dtStepInc*(-v.x - 0.4f*v.y + 5.f*v.x*v.z);
    vp.z = v.z + dtStepInc*(kVal[1]*v.z - 5.f*v.x*v.y);
}
////////////////////////////////////////////////////////////////////////////
void NoseHoover::Step(vec4 &v, vec4 &vp)
{ // kVal[] -> a, b, c ...
    vp.x = v.x + dtStepInc*v.y;
    vp.y = v.y + dtStepInc*(-v.x + v.y*v.z);
    vp.z = v.z + dtStepInc*(kVal[0] - v.y*v.y);
}
////////////////////////////////////////////////////////////////////////////
void QiChen::Step(vec4 &v, vec4 &vp)
{ // kVal[] -> a, b, c ...
    vp.x = v.x + dtStepInc*(kVal[0]*(v.y-v.x)+kVal[4]*v.y*v.z);
    vp.y = v.y + dtStepInc*(kVal[2]*v.x+kVal[3]*v.y-v.x*v.z);
    vp.z = v.z + dtStepInc*(v.x*v.y-kVal[1]*v.z);
}
////////////////////////////////////////////////////////////////////////////
void RayleighBenard::Step(vec4 &v, vec4 &vp)
{ // kVal[] -> a, b, r
    vp.x = v.x + dtStepInc*(-kVal[0]*(v.x - v.y));
    vp.y = v.y + dtStepInc*(kVal[2]*v.x - v.y - v.x*v.z);
    vp.z = v.z + dtStepInc*(v.x*v.y - kVal[1]*v.z);
}
////////////////////////////////////////////////////////////////////////////
void Sakarya::Step(vec4 &v, vec4 &vp)
{ // kVal[] -> a, b
    vp.x = v.x + dtStepInc*(-v.x + v.y + v.y*v.z);
    vp.y = v.y + dtStepInc*(-v.x - v.y + kVal[0]*v.x*v.z);
    vp.z = v.z + dtStepInc*(v.z - kVal[1]*v.x*v.y);
}
////////////////////////////////////////////////////////////////////////////
void Robinson::Step(vec4 &v, vec4 &vp)
{ // kVal[] -> a, b, c, d, v
    const float x2 = v.x*v.x;
    vp.x = v.x + dtStepInc*v.y;
    vp.y = v.y + dtStepInc*(v.x - 2.f*x2*v.x - kVal[0]*v.y + kVal[1]*x2*v.y - kVal[4]*v.y*v.z);
    vp.z = v.z + dtStepInc*(-kVal[2]*v.z + kVal[3]*x2);
}
////////////////////////////////////////////////////////////////////////////
void Rossler::Step(vec4 &v, vec4 &vp)
{ // kVal[] -> a, b, c
    vp.x = v.x + dtStepInc*(-v.y - v.z);
    vp.y = v.y + dtStepInc*(v.x + kVal[0]*v.y);
    vp.z = v.z + dtStepInc*(kVal[1] + v.z*(v.x - kVal[2]));
}
////////////////////////////////////////////////////////////////////////////
void Rucklidge::Step(vec4 &v, vec4 &vp)
{ // kVal[] -> a, k
    vp.x = v.x + dtStepInc*(-kVal[1]*v.x + kVal[0]*v.y - v.y*v.z);
    vp.y = v.y + dtStepInc*v.x;
    vp.z = v.z + dtStepInc*(-v.z + v.y*v.y);
}
////////////////////////////////////////////////////////////////////////////
void DequanLi::Step(vec4 &v, vec4 &vp)
{ // kVal[] -> a, c, d, e, k, f
    vp.x = v.x + dtStepInc*(kVal[0]*(v.y-v.x) + kVal[2]*v.x*v.z);
    vp.y = v.y + dtStepInc*(kVal[4]*v.x + kVal[5]*v.y - v.x*v.z);
    vp.z = v.z + dtStepInc*(kVal[1]*v.z + v.x*v.y - kVal[3]*v.x*v.x);
}
////////////////////////////////////////////////////////////////////////////
void MultiChuaII::Step(vec4 &v, vec4 &vp)
{ // kVal[] -> a, b, m0 -> m5, c1 -> c5
    auto f = [&](const float x) -> float {
        const int startM = 2, startC = 7; float sum = 0;
        for(int i=1; i<=5; i++) sum+= (kVal[startM+(i-1)]-kVal[startM+i])*(fabs(x+kVal[startC+i]) - fabs(x-kVal[startC+i]));
        return kVal[startM+5]*x + sum*.5f;
    };

    vp.x = v.x + dtStepInc*(kVal[0]*(v.y-f(v.x)));
    vp.y = v.y + dtStepInc*(v.x - v.y + v.z);
    vp.z = v.z + dtStepInc*(-kVal[1]*v.y);
}
////////////////////////////////////////////////////////////////////////////
void ZhouChen::Step(vec4 &v, vec4 &vp)
{ // kVal[] -> a, b, c, d, e,
    vp.x = v.x + dtStepInc*(kVal[0]*v.x + kVal[1]*v.y+ v.y*v.z);
    vp.y = v.y + dtStepInc*(kVal[2]*v.y - v.x*v.z - kVal[3]*v.y*v.z);
    vp.z = v.z + dtStepInc*(kVal[4]*v.z - v.x*v.y);
}
////////////////////////////////////////////////////////////////////////////
void ShimizuMorioka::Step(vec4 &v, vec4 &vp)
{ // kVal[] -> a
    vp.x = v.x + dtStepInc*v.y;
    vp.y = v.y + dtStepInc*((1.f-v.z)*v.x - kVal[0]*v.y);
    vp.z = v.z + dtStepInc*(v.x*v.x - kVal[1]*v.z);
}
////////////////////////////////////////////////////////////////////////////
void SprottLinzF::Step(vec4 &v, vec4 &vp)
{ // kVal[] -> a
    vp.x = v.x + dtStepInc*(v.y + v.z);
    vp.y = v.y + dtStepInc*(-v.x + kVal[0]*v.y);
    vp.z = v.z + dtStepInc*(v.x*v.x - v.z);
}
////////////////////////////////////////////////////////////////////////////
void SprottLinzB::Step(vec4 &v, vec4 &vp)
{ // kVal[] -> a
    vp.x = v.x + dtStepInc* kVal[0]*v.y * v.z;
    vp.y = v.y + dtStepInc*(kVal[1]*v.x - kVal[2]*v.y);
    vp.z = v.z + dtStepInc*(1 - kVal[3]*v.x*v.y);
}
void Tamari::Step(vec4 &v, vec4 &vp)
{ // kVal[] -> a
    vp.x = v.x + dtStepInc*((v.x - kVal[0]*v.y)*cos(v.z) - kVal[1]*v.y*sin(v.z));
    vp.y = v.y + dtStepInc*((v.x + kVal[2]*v.y)*sin(v.z) + kVal[3]*v.y*cos(v.z));
    vp.z = v.z + dtStepInc*(kVal[4] + kVal[5]*v.z + kVal[6]*atan(((1-kVal[7])*v.y) / ((1-kVal[8])*v.x)));
}
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
void Coullet::Step(vec4 &v, vec4 &vp)
{ // kVal[] -> a, b, c, d
    vp.x = v.x + dtStepInc*v.y;
    vp.y = v.y + dtStepInc*v.z;
    vp.z = v.z + dtStepInc*(kVal[0]*v.x + kVal[1]*v.y + kVal[2]*v.z + kVal[3]*v.x*v.x*v.x);
}
////////////////////////////////////////////////////////////////////////////
void Dadras::Step(vec4 &v, vec4 &vp)
{ // kVal[] -> p, q, r, s, e
    vp.x = v.x + dtStepInc*(v.y - kVal[0]*v.x + kVal[1]*v.y*v.z);
    vp.y = v.y + dtStepInc*(kVal[2]*v.y - v.x*v.z + v.z);
    vp.z = v.z + dtStepInc*(kVal[3]*v.x*v.y - kVal[4]*v.z);
}
////////////////////////////////////////////////////////////////////////////
void WangSunCang::Step(vec4 &v, vec4 &vp)
{ //https://www.researchgate.net/publication/267989602_A_3-D_spherical_chaotic_attractor

    auto b = [&]() -> float {
        //const float a =kVal[1]*kVal[5];
        //return (kVal[6]*(a*a) / (32.f*kVal[2]*kVal[2]*kVal[4]*kVal[4])) / sqrt(abs((kVal[2]*kVal[4])/(kVal[0]*kVal[3])));
        return kVal[7];
    };

    auto c = [&]() -> float {
        //return (1.f/kVal[1]) * (((kVal[1]*kVal[1]*kVal[5]*kVal[5]) / 4.f*kVal[2]*kVal[4]) - (f1()/kVal[6])*(kVal[2]*kVal[3]+kVal[0]*kVal[4]));
        return kVal[8];
    };

    vp.x = v.x + dtStepInc*(kVal[0]*v.x - kVal[1]*v.y + kVal[2]*v.z);
    vp.y = v.y + dtStepInc*(-kVal[6]*v.x*v.z + b());
    vp.z = v.z + dtStepInc*(kVal[3]*v.x*v.y + kVal[4]*v.y*v.z + kVal[5]*v.z + c());
}


////////////////////////////////////////////////////////////////////////////
// http://globotoroid.com by Nick Samardzija,  http://vixra.org/pdf/1712.0419v1.pdf
void GloboToroid::Step(vec4 &v, vec4 &vp)
{ // kVal[] -> p, q, r, s, e
    const float A = kVal[0];
    const float B = kVal[1];
    const float omega = 2.f * T_PI * kVal[2];
    const float x1 = v.x+1.f;
    vp.x = v.x + dtStepInc*(-omega*v.y - A*v.z*x1);
    vp.y = v.y + dtStepInc*omega*v.x;
    vp.z = v.z + dtStepInc*(A*x1*x1 - B);
}