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
#line 13    //#version dynamically inserted

layout(std140) uniform;

layout (location = 0) in vec4 a_ActualPoint;
layout (location = 1) in vec4 a_Vel;

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
    float elapsedTime;
    float lifeTime;
    float lifeTimeAtten;
    float smoothDistance;
    float vpReSize;
    float magnitude;
    float magnitudeInt;
    float invMagnitudeAtten;
    uint  slowMotion;

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
    mat4 vMatrix;
    mat4 pMatrix;
    mat4 invP;
    mat4 mvMatrix;
    mat4 invMV  ;
    mat4 mvpMatrix;
    mat4 mvLightM;
    mat4 mvpLightM;
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


float getMagnitudoAtten()
{
    float pointLife = u.elapsedTime-a_Vel.w;
    float pointLifeAtten = pointLife * u.invMagnitudeAtten;
    return 1.0 - pointLifeAtten;
    //(pointLifeAtten>0.0 && u.slowMotion != uint(0))  ? pointLifeAtten : 0.0
}
float getMagnitude()
{
    float magnitude = u.magnitude * getMagnitudoAtten();

    return ((magnitude>1.0 && u.slowMotion != uint(0)) ? magnitude : 1.0);
}

float getLifeTimeAtten()
{
    float alphaAtten = 1.0;
    if(u.slowMotion != uint(0) ) {
        float time = a_Vel.w;
        float life = time+u.lifeTime;
        if(life<u.elapsedTime)
            alphaAtten = max(1.0 - (u.elapsedTime-life) * u.lifeTimeAtten, 0.0);
    } 

    return alphaAtten;

}