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

class attractorDtType : public attractorScalarK
{
protected:

    attractorDtType() {
        kMin = -5.0; kMax = 5.0; vMin = 0.0; vMax = 0.0;
        m_POV = vec3( 0.f, 0, 7.f);
        setDTType();
    }

    virtual void additionalDataCtrls();

    virtual void saveAdditionalData(Config &cfg);
    virtual void loadAdditionalData(Config &cfg);

    float getDtStepInc() { return dtStepInc; }
    void setDtStepInc(float f) { dtStepInc = f; }

    // dTime step
    float dtStepInc = 0.001f;


};

#define DT(A)\
class A : public attractorDtType {\
    public:\
        A() { stepFn = (stepPtrFn) &A::Step; }\
    protected:\
        void Step(vec4 &v, vec4 &vp);\
        void startData();\
};

DT(Aizawa         )
DT(Arneodo        )
DT(Bouali         )
DT(BrukeShaw      )
DT(ChenCelikovsky )
DT(ChenLee        )
DT(Coullet        )
DT(Dadras         )
DT(DequanLi       )
DT(FourWing       )
DT(FourWing2      )
DT(FourWing3      )
DT(GenesioTesi    )
DT(GloboToroid    )
DT(Halvorsen      )
DT(Hadley         )
DT(LiuChen        )
DT(Lorenz         )
DT(MultiChuaII    )
DT(NewtonLeipnik  )
DT(NoseHoover     )
DT(QiChen         )
DT(Sakarya        )
DT(RayleighBenard )
DT(Robinson       )
DT(Rossler        )
DT(Rucklidge      )
DT(ShimizuMorioka )
DT(SprottLinzB    )
DT(SprottLinzF    )
DT(Tamari         )
DT(Thomas         )
DT(WangSunCang    )
DT(TSUCS          )
DT(YuWang         )
DT(ZhouChen       )

#undef DT

