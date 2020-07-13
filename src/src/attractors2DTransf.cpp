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
void Mira3D::Step(vec4 &v, vec4 &vp)
{
    auto f = [] (float x, float k) -> float {
        const float x2 = x*x;
        return k*x + 2.f*(1.f-k)*x2 / (1+x2);
    };

    vp.x =  v.y + f(v.x, kVal[0]);
    const float vpp = f(vp.x, kVal[0]);
    vp.y =  vpp - v.x;
    vp.z =  f(v.z, kVal[3]) + f(vp.y, kVal[2]) + f(vpp, kVal[1]);
}
////////////////////////////////////////////////////////////////////////////
void Mira4D::Step(vec4 &v, vec4 &vp)
{
    auto f = [] (const float x, const float k) -> float {
        const float x2 = x*x;
        return k*x + 2.f*(1.f-k)*x2 / (1+x2);
    };
    vp.x =  kVal[1]*v.y + f(v.x, kVal[0]);
    vp.y =  - v.x + f(vp.x, kVal[0]);
    vp.z =  kVal[3]*v.w + f(v.z, kVal[2]);
    vp.w =  - v.z + f(vp.z, kVal[2]);

}
////////////////////////////////////////////////////////////////////////////
void PopCorn3D::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.x - kVal[0] * sin((double)v.z+tan((double)kVal[1]*v.z));
    vp.y = v.y - kVal[2] * sin((double)v.x+tan((double)kVal[3]*v.x));
    vp.z = v.z - kVal[4] * sin((double)v.y+tan((double)kVal[5]*v.y));
}
////////////////////////////////////////////////////////////////////////////
void PopCorn4Dset::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.x - kVal[0] * pfX((double)v.y+tan((double)kVal[1]*v.y));
    vp.y = v.y - kVal[2] * pfY((double)v.x+tan((double)kVal[3]*v.x));
    vp.z = v.z - kVal[4] * pfZ((double)v.w+tan((double)kVal[5]*v.w));
    vp.w = v.w - kVal[6] * pfW((double)v.z+tan((double)kVal[7]*v.z));
}
////////////////////////////////////////////////////////////////////////////
void PopCorn4Dsimple::Step(vec4 &v, vec4 &vp)
{
    vp.x = kVal[0] * pfX((double)v.y+tan((double)kVal[1]*v.y));
    vp.y = kVal[2] * pfY((double)v.x+tan((double)kVal[3]*v.x));
    vp.z = kVal[4] * pfZ((double)v.w+tan((double)kVal[5]*v.w));
    vp.w = kVal[6] * pfW((double)v.z+tan((double)kVal[7]*v.z));
}
////////////////////////////////////////////////////////////////////////////
void PopCorn4Drnd::Step(vec4 &v, vec4 &vp)
{
    vp.x = kVal[0] - kVal[1]*v.x*v.x + v.y;
    vp.y = kVal[2]*v.x;
    vp.z = kVal[3] - kVal[4]*v.z*v.z + v.w;
    vp.w = kVal[5]*v.z;
}
////////////////////////////////////////////////////////////////////////////
void Martin4DBase::Step(vec4 &v, vec4 &vp)
{
    vp.x = v.y - pfX((double)v.x);
    vp.y = kVal[0] - v.x;
    vp.z = v.w - pfZ((double)v.z);
    vp.w = kVal[1] - v.z;
}
////////////////////////////////////////////////////////////////////////////
void SymmetricIcons4D::Step(vec4 &v, vec4 &vp)
{
    //alpha, beta, gamma, lambda, omega, degree
    const float sqrXY = v.x*v.x + v.y*v.y;
    const float sqrZW = v.z*v.z + v.w*v.w;
    float pXY = kVal[0]*sqrXY + kVal[3];
    float pZW = kVal[5]*sqrZW + kVal[8];

    vec4 newP(v);

    for(int i=6; i>0; i--) {
        newP = vec4(newP.x*v.x - newP.y*v.y, newP.y*v.x + newP.x*v.y,
                    newP.z*v.z - newP.w*v.w, newP.w*v.z + newP.z*v.w);
    }

    float nXY = v.x*newP.x - v.y*newP.y;
    float nZW = v.z*newP.z - v.w*newP.w;
    pXY = pXY + kVal[1]*nXY;
    pZW = pZW + kVal[6]*nZW;

    vp = vec4(pXY*v.x + kVal[2]*newP.x - kVal[4]*v.y,
              pXY*v.y + kVal[2]*newP.y - kVal[4]*v.x,
              pZW*v.z + kVal[7]*newP.z - kVal[9]*v.w,
              pZW*v.w + kVal[7]*newP.w - kVal[9]*v.z );
}
////////////////////////////////////////////////////////////////////////////
void Hopalong4D::Step(vec4 &v, vec4 &vp)
{
    auto func = [](const float f, const float k1, const float k2) {
        return (f>0 ? 1 : -1) * sqrt(abs(k1*f-k2));
    };

    vp.x = v.y - func(v.x, kVal[1], kVal[2]);
    vp.y = kVal[0] - v.x;
    vp.z = v.w - func(v.z, kVal[4], kVal[5]);
    vp.w = kVal[3] - v.z;
}
