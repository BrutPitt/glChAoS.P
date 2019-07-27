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

layout (location = 0) in vec4 a_ActualPoint;

LAYUOT_BINDING(0) uniform sampler2D paletteTex;

LAYUOT_BINDING(2) uniform _particlesData {
    vec3  lightDir;          // align 0
    float lightDiffInt;
    vec3  lightColor;        // align 16
    float lightSpecInt;
    vec4  POV;
    vec2  scrnRes;
    vec2  invScrnRes;
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
    float shadowSmoothRadius;
    float shadowGranularity;
    float shadowBias;
    float shadowDarkness;
    float aoRadius;
    float aoBias;
    float aoDarkness;
    float aoMul;
    float aoModulate;
    float dpAdjConvex;
    float dpNormalTune;
    uint  lightModel;
    uint  lightActive;
    int   pass;
} u;

LAYUOT_BINDING(4) uniform _tMat {
    mat4 pMatrix;
    mat4 mvMatrix;
    mat4 mvpMatrix;
    mat4 mvLightM;
} m;

#ifndef GL_ES
out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
};
#endif


#ifndef GL_ES
    subroutine vec4 _colorResult();
    subroutine uniform _colorResult colorResult;
#endif

// Load OBJ
#ifndef GL_ES
LAYUOT_INDEX(1) SUBROUTINE(_colorResult) vec4 objColor()
{
    uint packCol = floatBitsToUint(a_ActualPoint.w);
    vec4 col = unpackUnorm4x8(packCol);

    return col;
}
#endif

LAYUOT_INDEX(0) SUBROUTINE(_colorResult) vec4 velColor()
{
    float vel = a_ActualPoint.w*u.velIntensity;
    return vec4(texture(paletteTex, vec2(vel,0.f)).rgb,1.0);
}
