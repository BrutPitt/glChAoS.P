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

layout (location = 0) in vec2 vPos;

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
    uvec4 planeActive;       //AMD => bvec* != uvec* .... NVidia => bvec* == uvec*
    uvec4 colorActive;
    float thickness;
    bool  additiveSpace;
    bool  atLeastOneActive;
} pl;

out vec2 viewRay;

#if !defined(GL_ES)
out gl_PerVertex
{
	vec4 gl_Position;
};
#endif

out vec2 vTexCoord;
vec2 texCoord[4] = vec2[4](vec2(0.0, 0.0), vec2(1.0, 0.0), vec2(1.0, 1.0), vec2(0.0, 1.0));

void main(void)
{
    vTexCoord = texCoord[gl_VertexID];
    gl_Position = vec4(vPos.xy,.0f,1.f);
    viewRay = vPos.xy * vec2(u.scrnRes.x*u.invScrnRes.y * u.halfTanFOV, u.halfTanFOV);
}