//------------------------------------------------------------------------------
//  Copyright (c) 2018-2019 Michele Morrone
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
#line 12    //#version dynamically inserted

layout(std140) uniform;

layout (location = 0) in vec4 a_ActualPoint;

LAYOUT_BINDING(0) uniform sampler2D paletteTex;

LAYOUT_BINDING(2) uniform _particlesData {
    vec3  lightDir;          // align 0
    float lightDiffInt;
    vec3  lightColor;        // align 16
    float lightSpecInt;
    vec2  scrnRes;
    vec2  invScrnRes;
    vec3  rotCenter;
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
    float shadowDetail;
    float aoRadius;
    float aoBias;
    float aoDarkness;
    float aoMul;
    float aoModulate;
    float aoStrong;
    float dpAdjConvex;
    float dpNormalTune;
    uint  lightModel;
    uint  lightActive;
    uint  pass;
    uint  renderType;
    int   colorizingMethod;
} u;

LAYOUT_BINDING(9) uniform _clippingPlanes {
    vec4  clipPlane[3];
    vec4  boundaryColor[3];
    uvec4 planeActive;
    uvec4 colorActive;
    float thickness;
    bool  additiveSpace;
    bool  atLeastOneActive;
} pl;

LAYOUT_BINDING(4) uniform _tMat {
    mat4 mMatrix;
    mat4 vMatrix;
    mat4 invMV  ;
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
    //float gl_ClipDistance[1];
};
#endif


#if !defined(GL_ES) && !defined(GLCHAOSP_NO_USES_GLSL_SUBS)
    subroutine vec4 _colorResult();
    subroutine uniform _colorResult colorResult;
#endif

// Load OBJ
#if !defined (GL_ES)
LAYOUT_INDEX(1) SUBROUTINE(_colorResult) 
vec4 objColor()
{
    uint packCol = floatBitsToUint(a_ActualPoint.w);
    vec4 col = unpackUnorm4x8(packCol);

    return col;
}
#endif

LAYOUT_INDEX(0) SUBROUTINE(_colorResult) 
vec4 velColor()
{
    float vel = a_ActualPoint.w*u.velIntensity;
    return vec4(texture(paletteTex, vec2(vel,0.f)).rgb,1.0);
}
