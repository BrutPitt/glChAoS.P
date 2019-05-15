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

LAYUOT_BINDING(1) uniform sampler2D tex;

#define pixelColorOFFSET 0
#define idxBLENDING (pixelColorOFFSET    )
#define idxSOLID    (pixelColorOFFSET + 1)
#define idxSOLID_AO (pixelColorOFFSET + 2)
#define idxSOLID_DR (pixelColorOFFSET + 3)

#if !defined(GL_ES) && !defined(__APPLE__)
    subroutine vec4 _pixelColor(vec4 color, vec4 N);
    subroutine uniform _pixelColor pixelColor;
#endif

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

// linear depth (persective)
float depthSample(float Z)
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
    N.xyz = normalize(N.xyz); 
    
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

vec4 newVertex;

#if !defined(__APPLE__)
LAYUOT_INDEX(idxSOLID) SUBROUTINE(_pixelColor) 
#endif
vec4 pixelColorSolid(vec4 color, vec4 N)
{
    //if (color.a < u.alphaSkip) discard;   // kill pixels outside circle: r=1.0
    //else {
        //vec3 N;
        //N.xy = (coord - vec2(.5))*2.0; // xy = radius in 0
        //float mag = dot(N.xy, N.xy);
        //N.z = sqrt(1.0-mag);
    
        //N.xyz = normalize(N.xyz); 

        //Light @ vertex position
        vec3 light =  normalize(u.lightDir);  // +vtx
    
        float lambertian = max(0.0, dot(light, N.xyz)); 

        vec3 V = normalize(newVertex.xyz);
#if defined(GL_ES) || defined(__APPLE__)
        float specular = u.lightModel == uint(idxPHONG) ? specularPhong(V, light, N.xyz) : (u.lightModel == uint(idxBLINPHONG) ? specularBlinnPhong(V, light, N.xyz) : specularGGX(V, light, N.xyz));
#else
        float specular = lightModel(V, light, N.xyz);
#endif

        vec3 lColor =  smoothstep(u.sstepColorMin, u.sstepColorMax,
                                    /*color.rgb + */
                                    color.rgb * u.lightColor * lambertian * u.lightDiffInt +  //diffuse component
                                    u.lightColor * specular * u.lightSpecInt +
                                    (color.rgb*u.lightAmbInt + vec3(u.lightAmbInt)) * .5); /* (color.rgb+u.lightAmbInt*0.1) * */

        return vec4(lColor.rgb , color.a);
    //}
}

#if !defined(__APPLE__)
LAYUOT_INDEX(idxSOLID_AO) SUBROUTINE(_pixelColor)  
#endif
vec4 pixelColorAO(vec4 color, vec4 N)
{
   return vec4(pixelColorSolid(color, N).rgb, depthSample(newVertex.z)); 
}

#if !defined(__APPLE__)
LAYUOT_INDEX(idxSOLID_DR) SUBROUTINE(_pixelColor)  
#endif
vec4 pixelColorDR(vec4 color, vec4 N)
{
   return vec4(color.xyz, depthSample(newVertex.z));
}


#if !defined(__APPLE__)
LAYUOT_INDEX(idxBLENDING) SUBROUTINE(_pixelColor)  
#endif
vec4 pixelColorBlending(vec4 color, vec4 N)
{
    if(color.a < u.alphaSkip ) discard ;
    return color;
}

vec4 mainFunc(vec2 ptCoord)
{

    vec4 N = getParticleNormal(ptCoord);
    if(N.w > 1.0 || N.z < u.alphaSkip) { discard; return vec4(0.0); } //return black color and max depth

    newVertex    = mvVtxPos + vec4(0., 0., N.z * particleSize, 0.);

//linear
    gl_FragDepth = getDepth(newVertex.z);
    //gl_FragDepth = depthSample(newVertex.z); // same but with zNear & zFar
//logarithmic
    //vec4 pPos = m.pMatrix * newVertex;
    //gl_FragDepth = logDepthSample(pPos.w);

    vec4 color = acquireColor(ptCoord);

#if defined(GL_ES) || defined(__APPLE__)
    vec4 retColor = u.lightActive==uint(1) ? pixelColorSolid(color, N) : pixelColorBlending(color, N);
    #ifdef GL_ES
        return retColor;
    #else
        return (u.pass >= uint(2)) ? vec4(color.xyz, depthSample(newVertex.z)) : (u.pass==uint(0) ? retColor : vec4(retColor.xyz, depthSample(newVertex.z))); 
    #endif
#else                          

    return pixelColor(color, N); 

#endif
}