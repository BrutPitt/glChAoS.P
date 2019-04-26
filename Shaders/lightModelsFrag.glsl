////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2018-2019 Michele Morrone
//  All rights reserved.
//
//  mailto:me@michelemorrone.eu
//  mailto:brutpitt@gmail.com
//  
//  https://github.com/BrutPitt
//
//  https://michelemorrone.eu
//  https://BrutPitt.com
//
//  This software is distributed under the terms of the BSD 2-Clause license
//  
////////////////////////////////////////////////////////////////////////////////
#line 17    //#version dynamically inserted
layout(std140) uniform;


LAYUOT_BINDING(2) uniform _particlesData {
    vec3 lightDir;          // align 0
    float lightDiffInt;
    vec3 lightColor;        // align 16
    float lightSpecInt;
    vec2 scrnRes;
    float lightAmbInt ;
    float lightShinExp;
    float sstepColorMin;
    float sstepColorMax;
    float pointSize;
    float pointDistAtten;
    float alphaDistAtten;
    float alphaSkip;
    float alphaK;
    float colIntensity;
    float clippingDist;
    float zNear;
    float zFar;
    float halfTanFOV;
    float velIntensity;
    float ySizeRatio;
    float ptSizeRatio;
    float pointspriteMinSize;
    float ggxRoughness;
    float ggxFresnel;
    uint lightModel;
    uint lightActive;
    int pass;
} u;

#define lightModelOFFSET 5
#define idxPHONG     (lightModelOFFSET    )
#define idxBLINPHONG (lightModelOFFSET + 1)
#define idxGGX       (lightModelOFFSET + 2)

#if !defined(GL_ES) && !defined(__APPLE__)
    subroutine float _lightModel(vec3 V, vec3 L, vec3 N);
    subroutine uniform _lightModel lightModel;
#endif

float getViewZ(float D)
{
    float denom = u.zFar-u.zNear;
    return (-2.0*u.zFar*u.zNear/denom) / (-(2. * D - 1.) + ((u.zFar+u.zNear)/denom));
    //return (pMatrix[3].z / (-(2.* D -1) - pMatrix[2].z));
}

float form_01_to_m1p1(float f)  { return 2. * f - 1.; }
float form_m1p1_to_01(float f)  { return  f*.5 + .5; }

#if !defined(__APPLE__)
LAYUOT_INDEX(idxPHONG) SUBROUTINE(_lightModel) 
#endif
float specularPhong(vec3 V, vec3 L, vec3 N)
{
    //vec3 V = normalize(mvVtxPos.xyz);
    vec3 R = reflect(L, N);
    float specAngle = max(dot(R, V), 0.0);

    return pow(specAngle, u.lightShinExp * .25);
}

#if !defined(__APPLE__)
LAYUOT_INDEX(idxBLINPHONG) SUBROUTINE(_lightModel) 
#endif
float specularBlinnPhong(vec3 V, vec3 L, vec3 N)
{
// point on surface of sphere in eye space
    //vec3 V = normalize(mvVtxPos.xyz);
    vec3 H = normalize(L - V);
    float specAngle = max(dot(H, N), 0.0);

    return pow(specAngle, u.lightShinExp);

}

#if !defined(__APPLE__)
LAYUOT_INDEX(idxGGX) SUBROUTINE(_lightModel) 
#endif
float specularGGX(vec3 V, vec3 L, vec3 N) 
{
    float alpha = u.ggxRoughness*u.ggxRoughness;
    float alphaSqr = alpha * alpha;

    //vec3 V = normalize(mvVtxPos.xyz);
    vec3 H = normalize(L - V); // View = -
    float dotLH = max(0.0, dot(L,H));
    float dotNH = max(0.0, dot(N,H));
    float dotNL = max(0.0, dot(N,L));

    // D (GGX normal distribution)
    float denom = dotNH * dotNH * (alphaSqr - 1.0) + 1.0;
    float D = alphaSqr / (3.141592653589793 * denom * denom);

    // F (Fresnel term)
    float F = u.ggxFresnel + (1.0 - u.ggxFresnel) * pow(1.0 - dotLH, 5.0);
    float k = 0.5 * alpha;
    float k2 = k * k;

    return dotNL * D * F / (dotLH*dotLH*(1.0-k2)+k2);
}
