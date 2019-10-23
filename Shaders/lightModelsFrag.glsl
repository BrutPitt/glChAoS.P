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
#line 14    //#version dynamically inserted
layout(std140) uniform;

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
} u;                    // 54*4

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

#if !defined(GL_ES) && defined(GLCHAOSP_USES_LIGHTMODELS_SUBS)
    subroutine float _lightModel(vec3 V, vec3 L, vec3 N);
    LAYOUT_LOCATION(0) subroutine uniform _lightModel lightModel;
#else
    #define lightModelOFFSET 0
#endif

#define idxPHONG     (lightModelOFFSET    )
#define idxBLINPHONG (lightModelOFFSET + 1)
#define idxGGX       (lightModelOFFSET + 2)

float packColor16(vec2 color)
{
    return uintBitsToFloat( packUnorm2x16(color) );
}
vec2 unPackColor16(float pkColor)
{
    return unpackUnorm2x16(floatBitsToUint(pkColor));
}

#if !defined(GL_ES)
/*
uint packUnorm4x8(vec4 v) 
{
    uint x = round(clamp(v.x, 0.0, 1.0) * 255.0);
    uint y = round(clamp(v.y, 0.0, 1.0) * 255.0) * 0x100;
    uint z = round(clamp(v.y, 0.0, 1.0) * 255.0) * 0x10000;
    uint w = round(clamp(v.y, 0.0, 1.0) * 255.0) * 0x1000000;
    return x+y+z+w; 
}

vec4 unpackUnorm4x8(uint i)
{
    return vec4(float(i         & 0xff) / 255.0,
                float(i/0x100   & 0xff) / 255.0,
                float(i/0x10000 & 0xff) / 255.0,
                float(i/0x1000000     ) / 255.0);
}
*/

float packColor8(vec4 color)
{
    return uintBitsToFloat( packUnorm4x8(color) );
}
vec4 unPackColor8(float pkColor)
{
    return unpackUnorm4x8(floatBitsToUint(pkColor));
}
#endif

// Clipping planes
///////////////////////////////////////
mat4 translate(mat4 mt, vec3 v) 
{
    mt[3] = mt[0] * v.x + mt[1] * v.y + mt[2] * v.z + mt[3]; 
    return mt; 
}

float returnClipDistance(vec4 vtx, int plane)
{
    mat4 mt=mat4(1.f);
    vec4 pt = m.invMV * vec4(vtx.xyz, 1.0); // if modelViewd vertex

    mt = translate(mt, u.rotCenter);

    return -(dot( mt * pt, pl.clipPlane[plane]));
}

vec4 colorBoundary(vec4 vtx, int plane)
{
    vec4 retColor = vec4(0.0);
    if(bool(pl.planeActive[plane]) && bool(pl.colorActive[plane])) { 
        float clipDist = returnClipDistance(vtx, plane);
        if(pl.additiveSpace) { 
            if(clipDist<=0.0 && clipDist>-pl.thickness) retColor = pl.boundaryColor[plane];
        } else {
            if(clipDist>=0.0 && clipDist< pl.thickness) retColor = pl.boundaryColor[plane];
        }
    }
    return retColor;
}
bool clippedPlane(vec4 vtx, int plane)
{
    return bool(pl.planeActive[plane]) ? 
                (pl.additiveSpace ? bool(returnClipDistance(vtx, plane)>0.0) : bool(returnClipDistance(vtx, plane)<0.0)) : 
                (pl.additiveSpace ? true : false);

}

bool clippedPoint(vec4 vtx)
{
    return pl.atLeastOneActive ? 
               (pl.additiveSpace ?
                    clippedPlane(vtx, 0) && clippedPlane(vtx, 1) && clippedPlane(vtx, 2)  :
                    clippedPlane(vtx, 0) || clippedPlane(vtx, 1) || clippedPlane(vtx, 2)) :
               false;
}

// Z Buffer
///////////////////////////////////////
float getLinearZ(float D)
{
    float denom = u.zFar-u.zNear;
    return (-2.0*u.zFar*u.zNear/denom) / (-(2. * D - 1.) + ((u.zFar+u.zNear)/denom));
}

float getLogZ(float D)
{
    //return -pow(2.0, (2. * D - 1.) * log2(u.zFar/u.zNear)) * u.zNear;
    
    float C = u.zNear/u.zFar;
    //return (exp(-(2. * D - 1.)*log(C*u.zFar+1.)) - 1.)/C;

    //return (-exp((2. * D - 1.)*log(u.zFar / u.zNear)))*u.zNear;

    float FC = 1.0/log(u.zFar*C + 1.);
    return -(exp((2. * D - 1.)/FC)-1.0)/C;

}

// linear depth (persective)
float getLinearDepth_(float Z)
{
    //float nonLinearDepth =  (zFar + zNear + 2.0 * zNear * zFar / linearDepth) / (zFar - zNear);
    float nonLinearDepth =  -(m.pMatrix[2].z + m.pMatrix[3].z / Z) * .5 +.5;
    return nonLinearDepth;
}

float getLinearDepth(float Z) 
{
    return ( ((u.zFar+u.zNear)/(u.zFar-u.zNear)) + ((2.0*u.zFar*u.zNear/(u.zFar-u.zNear)) / Z) ) * .5 + .5;
}

//https://stackoverflow.com/questions/18182139/logarithmic-depth-buffer-linearization/18187212#18187212
//https://outerra.blogspot.com/2012/11/maximizing-depth-buffer-range-and.html

float getLogDepth(float z)
{
    //float C = .001;

    float C = u.zNear/u.zFar;
    //return (log(C * z + 1.) / log(C * u.zFar + 1.) * w) * .5 + .5;

    float FC = 1.0/log(u.zFar*C + 1.);
    float logz = log(z*C + 1.)*FC  * .5 + .5;
    return logz;

}
//#define LOG_ZBUFF
float getFragDepth(float z)
{
#ifdef LOG_ZBUFF
    //vec4 pPos = m.pMatrix * vtx;
    return getLogDepth(-z);
#else
    return getLinearDepth(z);
#endif
}

float restoreZ(float D)
{
#ifdef LOG_ZBUFF
    return getLogZ(D);
#else
    return getLinearZ(D);
#endif
}


float form_01_to_m1p1(float f)  { return 2. * f - 1.; }
float form_m1p1_to_01(float f)  { return  f*.5 + .5; }

#if defined(GLCHAOSP_USES_LIGHTMODELS_SUBS)
LAYOUT_INDEX(idxPHONG) SUBROUTINE(_lightModel) 
#endif
float specularPhong(vec3 V, vec3 L, vec3 N)
{
    vec3 R = reflect(L, N);
    float specAngle = max(dot(R, V), 0.0);

    return pow(specAngle, u.lightShinExp * .25);
}

#if defined(GLCHAOSP_USES_LIGHTMODELS_SUBS)
LAYOUT_INDEX(idxBLINPHONG) SUBROUTINE(_lightModel) 
#endif
float specularBlinnPhong(vec3 V, vec3 L, vec3 N)
{
// point on surface of sphere in eye space
    vec3 H = normalize(L - V);
    float specAngle = max(dot(H, N), 0.0);

    return pow(specAngle, u.lightShinExp);

}

#if defined(GLCHAOSP_USES_LIGHTMODELS_SUBS)
LAYOUT_INDEX(idxGGX) SUBROUTINE(_lightModel) 
#endif
float specularGGX(vec3 V, vec3 L, vec3 N) 
{
    float alpha = u.ggxRoughness*u.ggxRoughness;
    float alphaSqr = alpha * alpha;

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


vec4 getParticleNormal(vec2 coord)
{
    vec4 N;
    N.xy = (coord - vec2(.5))*2.0;  // diameter ZERO centred -> radius
    N.w = dot(N.xy, N.xy);          // magnitudo
    N.z = sqrt(1.0-N.w);            // Z convexity
    N.xyz = normalize(N.xyz); 
    
    return N;
}

vec3 getSimpleNormal(float z, sampler2D depthData)
{
    float gradA = restoreZ(texelFetch(depthData,ivec2(gl_FragCoord.xy + vec2( 1., 0.)), 0).w) - z;
    float gradB = restoreZ(texelFetch(depthData,ivec2(gl_FragCoord.xy + vec2( 0., 1.)), 0).w) - z;

    vec2 m = u.invScrnRes * -z;// * vec2(u.scrnRes.x/u.scrnRes.y * u.halfTanFOV, u.halfTanFOV);
    float invTanFOV = u.dpAdjConvex/u.halfTanFOV;

    vec3 N0 = cross(vec3(vec2( 1., 0.)*m, gradA*invTanFOV), vec3(vec2( 0., 1.)*m, gradB*invTanFOV));

    return normalize (N0);
}

vec3 getSelectedNormal(float z, sampler2D depthData)
{

    float gradA = restoreZ(texelFetch(depthData,ivec2(gl_FragCoord.xy + vec2( 1., 0.)), 0).w) - z;
    float gradB = restoreZ(texelFetch(depthData,ivec2(gl_FragCoord.xy + vec2( 0., 1.)), 0).w) - z;
    float gradC = z - restoreZ(texelFetch(depthData,ivec2(gl_FragCoord.xy + vec2(-1., 0.)), 0).w);
    float gradD = z - restoreZ(texelFetch(depthData,ivec2(gl_FragCoord.xy + vec2( 0.,-1.)), 0).w);

    vec2 m = u.invScrnRes * -z; //vec2(u.scrnRes.x/u.scrnRes.y * u.halfTanFOV, u.halfTanFOV);
    float invTanFOV = u.dpAdjConvex/u.halfTanFOV;

    vec3 V1 = abs(gradA-gradC)>u.dpNormalTune && abs(gradA)<abs(gradC) ? vec3(vec2( 1., 0.)*m, gradA*invTanFOV) : vec3(vec2(1., 0.)*m, gradC*invTanFOV);
    vec3 V2 = abs(gradB-gradD)>u.dpNormalTune && abs(gradB)<abs(gradD) ? vec3(vec2( 0., 1.)*m, gradB*invTanFOV) : vec3(vec2(0., 1.)*m, gradD*invTanFOV);
    vec3 N0 = cross(V1, V2);


    return normalize (N0);
}

#define RENDER_AO uint(1)
#define RENDER_DEF uint(2)
#define RENDER_SHADOW uint(4)
