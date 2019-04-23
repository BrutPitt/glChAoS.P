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

LAYUOT_BINDING(1) uniform sampler2D tex;

//uniform vec2 WinAspect;

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

#define idxPHONG 3
#define idxBLINPHONG 4
#define idxGGX 5

#if !defined(GL_ES) && !defined(__APPLE__)
    subroutine vec4 _pixelColor(vec4 color, vec4 N);
    subroutine uniform _pixelColor pixelColor;

    subroutine float _lightModel(vec3 V, vec3 L, vec3 N);
    subroutine uniform _lightModel lightModel;
#endif

LAYUOT_BINDING(4) uniform _tMat {
    mat4 pMatrix;
    mat4 mvMatrix;
    mat4 mvpMatrix;
} m;


in vec4 mvVtxPos;

in float pointDistance;
in vec4 particleColor;
in float particleSize;

layout (location = 0) out vec4 outColor;
// multi out
//layout (location = 1) out vec4 vtxOut;
//layout (location = 2) out vec4 normOut;

//uniform float distAlphaFactor;
//uniform float alphaK;

// in OGL 4.5 "cull distance" to drop Vertex... to look in upgrade

float luminance(vec3 c) { return dot(c, vec3(0.2990, 0.5870, 0.1140)); }

float getAlpha(float alpha)
{

    CONST float alphaAtten = exp(-0.1*sign(pointDistance)*pow(abs(pointDistance+1.f)+1.f, u.alphaDistAtten*.1));
    return clamp(alpha * alphaAtten * u.alphaK, 0.0, 1.0);

}

float linearizeDepth(float Z, float near, float far) 
{
    //if(-depth>far) discard;
    float z = (-Z * 2.0 - (far+near))/(far-near); //-depth * 2.0 - (far - near);  // back to NDC -> -depth/(far - near) * 2.0 - 1.0
    //float z = -depth/(far - near) * 2.0 - 1.0;
    return ((2.0 * near * far) / ((far + near) - z * (far - near)));	
    //return far*near/(far+near+depth);

    //return z;
}

// not linear depth (persective)
float depthSample(float Z)
{
    //float nonLinearDepth =  (zFar + zNear + 2.0 * zNear * zFar / linearDepth) / (zFar - zNear);
    float nonLinearDepth =  -(m.pMatrix[2].z + m.pMatrix[3].z / Z) * .5 +.5;
    return nonLinearDepth;
}

float zSample(float D)
{

    return 1.0;
}


float getDepth(float Z) 
{
    float n = u.zNear, f = u.zFar;
    //return (( -Z * (f+n) - 2*f*n ) / (-Z * (f-n))  + 1.0) * .5; //non linear -> same of depthSample
    //return ( (- 2.f * depth - (f+n))/(f-n) + 1.0) * .5; // linearize + depthSample -> linear
    return ( ((u.zFar+u.zNear)/(u.zFar-u.zNear)) + ((2.0*u.zFar*u.zNear/(u.zFar-u.zNear)) / Z) ) * .5 + .5;

}

float logzbuf( float w ) {
    float fc = 2.0/log2(u.zFar + 1.0);
    return (log(1. + w) * fc - 1.) * w;
}

float logDepthSample(float Z)
{
    return log2(1.+Z) / log2(u.zFar + 1.0);
}

vec4 acquireColor(vec2 coord)
{
    if(pointDistance<u.clippingDist) discard;    

    vec4 color = particleColor * texture(tex, coord).r;

    return vec4(color.rgb * u.colIntensity, getAlpha(color.a)) ;
}

vec4 getParticleNormal(vec2 coord)
{
    vec4 N;
    N.xy = (coord - vec2(.5))*2.0;  // diameter ZERO centred -> radius
    N.w = dot(N.xy, N.xy);          // magnitudo
    N.z = sqrt(1.0-N.w);            // Z convexity 
    return N;
}

#if !defined(GL_ES)
float packColor(vec4 color)
{
    return uintBitsToFloat( packUnorm4x8(color) );
}

vec4 unPackColor(uint pkColor)
{
    return unpackUnorm4x8(floatBitsToUint(pkColor));
}
#endif

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

#if !defined(__APPLE__)
LAYUOT_INDEX(1) SUBROUTINE(_pixelColor) 
#endif
vec4 pixelColorLight(vec4 color, vec4 N)
{
    if (color.a < u.alphaSkip) discard;   // kill pixels outside circle: r=1.0
    //else {
        //vec3 N;
        //N.xy = (coord - vec2(.5))*2.0; // xy = radius in 0
        //float mag = dot(N.xy, N.xy);
        //N.z = sqrt(1.0-mag);
    
        N.xyz = normalize(N.xyz); 

        //Light @ vertex position
        vec3 vtx = mvVtxPos.xyz;
        vtx.z += (N.w > 1.0 ? 0.0 : N.z) * particleSize;
        vec3 light =  normalize(u.lightDir);  // +vtx
    
        float lambertian = max(0.0, dot(light, N.xyz)); 

        vec3 V = normalize(vtx);
#if defined(GL_ES) || defined(__APPLE__)
        float specular = u.lightModel == uint(idxPHONG) ? specularPhong(V, light, N.xyz) : (u.lightModel == uint(idxBLINPHONG) ? specularBlinnPhong(V, light, N.xyz) : specularGGX(V, light, N.xyz));
#else
        float specular = lightModel(V, light, N.xyz);
#endif

        vec3 lColor =  smoothstep(u.sstepColorMin, u.sstepColorMax,
                                    /*color.rgb + */
                                    color.rgb * u.lightColor * lambertian * u.lightDiffInt +  //diffuse component
                                    u.lightColor * specular * u.lightSpecInt  + 
                                    (color.rgb*u.lightAmbInt + vec3(u.lightAmbInt)) * .5); /* (color.rgb+u.lightAmbInt*0.1) * */

        return vec4(lColor.rgb , color.a);
    //}
}



#if !defined(__APPLE__)
LAYUOT_INDEX(0) SUBROUTINE(_pixelColor)  
#endif
vec4 pixelColorOnly(vec4 color, vec4 N)
{

    if(color.a < u.alphaSkip ) discard ;
    return color;
}

vec4 mainFunc(vec2 ptCoord)
{

    vec4 N = getParticleNormal(ptCoord);
    if(N.w > 1.0 || N.z < u.alphaSkip) { discard; /*return vec4(vec3(0.0), 1.0);*/ } //return black color and max depth

    float incVtx = (N.w > 1.0 ? 0.0 : N.z) * particleSize;

//linear
    //gl_FragDepth = getDepth(mvVtxPos.z + incVtx);// depthSample(mvVtxPos.z + incVtx);
//logarithmic
    vec4 pPos = m.pMatrix * (mvVtxPos + vec4(0., 0., incVtx, 0.));
    gl_FragDepth = logDepthSample(pPos.w);
    //gl_FragDepth = depthSample(mvVtxPos.z + incVtx);

    vec4 color = acquireColor(ptCoord);

#if defined(GL_ES) || defined(__APPLE__)
    vec4 retColor = u.lightActive==uint(1) ? pixelColorLight(color, N) : pixelColorOnly(color, N);
    #ifdef GL_ES
        return retColor;
    #else
        return (u.pass >= uint(2)) ? vec4(color.xyz, depthSample(mvVtxPos.z + incVtx)) : (u.pass==uint(0) ? retColor : vec4(retColor.xyz, depthSample(mvVtxPos.z + incVtx))); 
    #endif
#else                            
/*
    vec2 uv = ptCoord;
    float depth = mvVtxPos.z + incVtx;// 2. * gl_FragDepth - 1.; //depthSample(gl_FragDepth, u.zNear, u.zFar);
    vec3 dx = dFdx(vec3(uv,depth));
    vec3 dy = dFdy(vec3(uv,depth));
    vec3 vN = normalize(cross(dx, dy));
*/
    //return vec4(vN,1.0);

    //return vec4(color.xyz, depthSample(mvVtxPos.z + incVtx));
    return (u.pass >= uint(2)) ? vec4(color.xyz, depthSample(mvVtxPos.z + incVtx)) : (u.pass==uint(0) ? pixelColor(color, N) : vec4(pixelColor(color, N).rgb, depthSample(mvVtxPos.z + incVtx))); 


    //return vec4(vec3(depthSample(gl_FragDepth, u.zNear, u.zFar)),1.0); 
    //return vec4(vec3(1.-(1.+gl_FragDepth)*.5),1.0);
    //return vec4(vec3(gl_FragDepth),1.0);
#endif
}