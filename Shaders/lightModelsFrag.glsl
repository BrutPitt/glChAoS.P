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
    float aoStrong;
    float dpAdjConvex;
    float dpNormalTune;
    uint  lightModel;
    uint  lightActive;
    uint  pass;
    uint  renderType;
} u;                    // 51*4 + APPLE

LAYUOT_BINDING(4) uniform _tMat {
    mat4 pMatrix;
    mat4 mvMatrix;
    mat4 mvpMatrix;
    mat4 mvLightM;
} m;

#if !defined(GL_ES) && !defined(__APPLE__)
    #define lightModelOFFSET 5
    subroutine float _lightModel(vec3 V, vec3 L, vec3 N);
    subroutine uniform _lightModel lightModel;
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

float getViewZ(float D)
{
    float denom = u.zFar-u.zNear;
    return (-2.0*u.zFar*u.zNear/denom) / (-(2. * D - 1.) + ((u.zFar+u.zNear)/denom));
    //return (pMatrix[3].z / (-(2.* D -1) - pMatrix[2].z));
}
float getViewZ_(float D)
{
    return -pow(2.0, (2. * D - 1.) * log2(u.zFar/u.zNear)) * u.zNear;
}

// linear depth (persective)
float depthSampleA(float Z)
{
    //float nonLinearDepth =  (zFar + zNear + 2.0 * zNear * zFar / linearDepth) / (zFar - zNear);
    float nonLinearDepth =  -(m.pMatrix[2].z + m.pMatrix[3].z / Z) * .5 +.5;
    return nonLinearDepth;
}

float getDepth(float Z) 
{
    float n = u.zNear, f = u.zFar;
    //return (( -Z * (f+n) - 2*f*n ) / (-Z * (f-n))  + 1.0) * .5; //non linear -> same of depthSample
    //return ( (- 2.f * depth - (f+n))/(f-n) + 1.0) * .5; // linearize + depthSample -> linear
    return ( ((u.zFar+u.zNear)/(u.zFar-u.zNear)) + ((2.0*u.zFar*u.zNear/(u.zFar-u.zNear)) / Z) ) * .5 + .5;

}

//https://stackoverflow.com/questions/18182139/logarithmic-depth-buffer-linearization/18187212#18187212
//https://outerra.blogspot.com/2012/11/maximizing-depth-buffer-range-and.html

float getDepth_(float Z)
{
    //float C = .001;
    //return (2.*log(C*-Z + 1.) / log(C*u.zFar + 1.) - 1.) * Z;
     //return (log2((Z+1) / u.zNear) / log2(u.zFar / u.zNear)) * .5 + .5;
    return (log2(1.+Z) * (2.0 / log2(u.zFar + 1.0)) -1.0) * Z;

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


/*
float logzbuf( float z ) {
    z = -z;
    float fc = 2.0/log2(u.zFar + 1.0);
    return (log(1. + z) * fc - 1.) * z;
}

  */
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
    float gradA = getViewZ(texelFetch(depthData,ivec2(gl_FragCoord.xy + vec2( 1., 0.)), 0).w) - z;
    float gradB = getViewZ(texelFetch(depthData,ivec2(gl_FragCoord.xy + vec2( 0., 1.)), 0).w) - z;

    vec2 m = u.invScrnRes * -z;// * vec2(u.scrnRes.x/u.scrnRes.y * u.halfTanFOV, u.halfTanFOV);
    float invTanFOV = u.dpAdjConvex/u.halfTanFOV;

    vec3 N0 = cross(vec3(vec2( 1., 0.)*m, gradA*invTanFOV), vec3(vec2( 0., 1.)*m, gradB*invTanFOV));

    return normalize (N0);
}

vec3 getSelectedNormal(float z, sampler2D depthData)
{

    float gradA = getViewZ(texelFetch(depthData,ivec2(gl_FragCoord.xy + vec2( 1., 0.)), 0).w) - z;
    float gradB = getViewZ(texelFetch(depthData,ivec2(gl_FragCoord.xy + vec2( 0., 1.)), 0).w) - z;
    float gradC = z - getViewZ(texelFetch(depthData,ivec2(gl_FragCoord.xy + vec2(-1., 0.)), 0).w);
    float gradD = z - getViewZ(texelFetch(depthData,ivec2(gl_FragCoord.xy + vec2( 0.,-1.)), 0).w);
    //float gradE = form_01_to_m1p1(texelFetch(prevData,ivec2(uv + vec2( 1., 1.)), 0).w);
    //float gradF = form_01_to_m1p1(texelFetch(prevData,ivec2(uv + vec2(-1.,-1.)), 0).w);
    //float gradG = form_01_to_m1p1(texelFetch(prevData,ivec2(uv + vec2( 1.,-1.)), 0).w);
    //float gradH = form_01_to_m1p1(texelFetch(prevData,ivec2(uv + vec2(-1., 1.)), 0).w);

    vec2 m = u.invScrnRes * -z; //vec2(u.scrnRes.x/u.scrnRes.y * u.halfTanFOV, u.halfTanFOV);
    float invTanFOV = u.dpAdjConvex/u.halfTanFOV;
/*
    const float eps = .00001;
    gradA = max(abs(gradA),eps) * sign(gradA);
    gradB = max(abs(gradB),eps) * sign(gradB);
    gradC = max(abs(gradC),eps) * sign(gradC);
    gradD = max(abs(gradD),eps) * sign(gradD);
*/
        //V1 = abs(gradA)<abs(gradC) ? vec3(vec2( 1., 0.)*m, gradA*invTanFOV) : vec3(vec2(1., 0.)*m, gradC*invTanFOV);
        //V2 = abs(gradB)<abs(gradD) ? vec3(vec2( 0., 1.)*m, gradB*invTanFOV) : vec3(vec2(0., 1.)*m, gradD*invTanFOV);
    vec3 V1 = abs(gradA-gradC)>u.dpNormalTune && abs(gradA)<abs(gradC) ? vec3(vec2( 1., 0.)*m, gradA*invTanFOV) : vec3(vec2(1., 0.)*m, gradC*invTanFOV);
    vec3 V2 = abs(gradB-gradD)>u.dpNormalTune && abs(gradB)<abs(gradD) ? vec3(vec2( 0., 1.)*m, gradB*invTanFOV) : vec3(vec2(0., 1.)*m, gradD*invTanFOV);
    vec3 N0 = cross(V1, V2);

/*
    vec3 V1 = vec3(vec2( 1., 0.)*m, gradA*invTanFOV);
    vec3 V2 = vec3(vec2( 0., 1.)*m, gradB*invTanFOV);
    vec3 N0 = cross(V1, V2);
*/
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

#define RENDER_AO uint(1)
#define RENDER_DEF uint(2)
#define RENDER_SHADOW uint(4)

