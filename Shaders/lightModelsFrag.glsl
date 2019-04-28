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

LAYUOT_BINDING(4) uniform _tMat {
    mat4 pMatrix;
    mat4 mvMatrix;
    mat4 mvpMatrix;
    mat4 mvpLightM;
} m;


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

vec3 getSimpleNormal(float depth, float z, sampler2D depthData)
{
    float zEye  = form_01_to_m1p1(depth);
    float gradA = form_01_to_m1p1(texelFetch(depthData,ivec2(gl_FragCoord.xy + vec2( 1., 0.)), 0).w);
    float gradB = form_01_to_m1p1(texelFetch(depthData,ivec2(gl_FragCoord.xy + vec2( 0., 1.)), 0).w);

    vec2 m = (1. / u.scrnRes) * vec2(u.scrnRes.x/u.scrnRes.y * u.halfTanFOV, u.halfTanFOV);

    vec3 N0 = cross(vec3(vec2( 1., 0.)*m, (gradA-zEye)*z), vec3(vec2( 0., 1.)*m, (gradB-zEye)*z));

    return normalize (N0);
}

vec3 getSelectedNormal(float depth, float z, sampler2D depthData)
{

    float zEye  = form_01_to_m1p1(depth);
    float gradA = form_01_to_m1p1(texelFetch(depthData,ivec2(gl_FragCoord.xy + vec2( 1., 0.)), 0).w);
    float gradB = form_01_to_m1p1(texelFetch(depthData,ivec2(gl_FragCoord.xy + vec2( 0., 1.)), 0).w);
    float gradC = form_01_to_m1p1(texelFetch(depthData,ivec2(gl_FragCoord.xy + vec2(-1., 0.)), 0).w);
    float gradD = form_01_to_m1p1(texelFetch(depthData,ivec2(gl_FragCoord.xy + vec2( 0.,-1.)), 0).w);
    //float gradE = form_01_to_m1p1(texelFetch(prevData,ivec2(uv + vec2( 1., 1.)), 0).w);
    //float gradF = form_01_to_m1p1(texelFetch(prevData,ivec2(uv + vec2(-1.,-1.)), 0).w);
    //float gradG = form_01_to_m1p1(texelFetch(prevData,ivec2(uv + vec2( 1.,-1.)), 0).w);
    //float gradH = form_01_to_m1p1(texelFetch(prevData,ivec2(uv + vec2(-1., 1.)), 0).w);

    vec2 m = (1. / u.scrnRes) * vec2(u.scrnRes.x/u.scrnRes.y * u.halfTanFOV, u.halfTanFOV);
    //vec2 m = (1. / vec2(1920.,1080.)) * vec2(1920./1080. * 0.288675, 0.288675);

    vec3 V1 = gradA>gradC ? vec3(vec2( 1., 0.)*m, (gradA-zEye)*z) : -vec3(vec2(-1., 0.)*m, (gradC-zEye)*z);
    vec3 V2 = gradB>gradD ? vec3(vec2( 0., 1.)*m, (gradB-zEye)*z) : -vec3(vec2( 0.,-1.)*m, (gradD-zEye)*z);
    vec3 N0 = cross(V1, V2);

    //vec3 N0 = cross(vec3(vec2( 1., 0.)*m, (gradA-zEye)*z), vec3(vec2( 0., 1.)*m, (gradB-zEye)*z));
    //vec3 N1 = cross(vec3(vec2(-1., 0.)*m, (gradC-zEye)*z), vec3(vec2( 0.,-1.)*m, (gradD-zEye)*z));
    //vec3 N2 = cross(vec3(vec2( 0.,-1.)*m, (gradD-zEye)*z), vec3(vec2( 1., 0.)*m, (gradA-zEye)*z));
    //vec3 N3 = cross(vec3(vec2( 0., 1.)*m, (gradB-zEye)*z), vec3(vec2(-1., 0.)*m, (gradC-zEye)*z));
    //vec3 N2 = cross(vec3(vec2( 1., 1.)*m, (gradE-zEye)*z), vec3(vec2(-1., 1.)*m, (gradH-zEye)*z));
    //vec3 N3 = cross(vec3(vec2(-1.,-1.)*m, (gradF-zEye)*z), vec3(vec2( 1.,-1.)*m, (gradG-zEye)*z));

            
    //N0    = normalize(dot(N0,N0)<dot(N1,N1) ? N0 : N1);
    //N2    = normalize(dot(N2,N2)>dot(N3,N3) ? N2 : N3);
    //N.xyz = normalize(dot(N0,N0)>dot(N2,N2) ? N0 : N2);
    //N.xyz = normalize(min(N0, min(N1, min(N2,N3))));
    //N.xyz = normalize(min(N0,N1));

    return normalize (N0);


}
