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

LAYOUT_BINDING(1) uniform sampler2D tex;

#define pixelColorOFFSET 0
#define idxBLENDING (pixelColorOFFSET    )
#define idxSOLID    (pixelColorOFFSET + 1)
#define idxSOLID_AO (pixelColorOFFSET + 2)
#define idxSOLID_DR (pixelColorOFFSET + 3)

#if !defined(GL_ES) && !defined(GLCHAOSP_NO_USES_GLSL_SUBS)
    subroutine vec4 _pixelColor(vec4 color, vec4 N);
    LAYOUT_LOCATION(1) subroutine uniform _pixelColor pixelColor;
#endif

in vec4 mvVtxPos;

in float pointDistance;
in vec4 particleColor;
in float particleSize;
in float alphaAttenuation;

//in vec4 shadowlightView;

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

vec4 acquireColor(vec2 coord)
{
    vec4 color = particleColor * texture(tex, coord).r;

    return vec4(color.rgb * u.colIntensity, getAlpha(color.a)) ;
}


vec4 newVertex;

vec3 packing2Colors16bit(vec3 colorA, vec3 colorB)
{
    return vec3(packColor16(colorA.rg),
                packColor16(vec2(colorA.b, colorB.r)),
                packColor16(colorB.gb));
}

#if !defined(GL_ES)
vec3 packing2Colors8bit(vec3 colorA, vec3 colorB)
{
    return vec3(packColor8(vec4(0.0,colorA.rgb)),
                packColor8(vec4(0.0,colorB.rgb)), 0.0);
}
#endif

LAYOUT_INDEX(idxSOLID) SUBROUTINE(_pixelColor) 
vec4 pixelColorDirect(vec4 color, vec4 N)
{

    //Light @ vertex position
    vec3 light =  normalize(u.lightDir);  // +vtx
    
    float lambertian = max(0.0, dot(light, N.xyz)); 

    vec3 V = normalize(newVertex.xyz);
#if defined(GL_ES) || defined(GLCHAOSP_NO_USES_GLSL_SUBS)
    float specular = u.lightModel == uint(idxPHONG) ? specularPhong(V, light, N.xyz) : (u.lightModel == uint(idxBLINPHONG) ? specularBlinnPhong(V, light, N.xyz) : specularGGX(V, light, N.xyz));
#else
    float specular = lightModel(V, light, N.xyz);
#endif

    vec3 lightColor = color.rgb * u.lightColor * lambertian * u.lightDiffInt +  //diffuse component
                    u.lightColor * specular * u.lightSpecInt;

    vec3 ambColor = (color.rgb*u.lightAmbInt + vec3(u.lightAmbInt)) * .5;

    return vec4(smoothstep(u.sstepColorMin, u.sstepColorMax, lightColor + ambColor) , color.a);
}

LAYOUT_INDEX(idxSOLID_AO) SUBROUTINE(_pixelColor) 
vec4 pixelColorAO(vec4 color, vec4 N)
{
    vec3 light =  normalize(u.lightDir);  // +vtx
    
    float lambertian = max(0.0, dot(light, N.xyz)); 

    vec3 V = normalize(newVertex.xyz);
#if defined(GL_ES) || defined(GLCHAOSP_NO_USES_GLSL_SUBS)
    float specular = u.lightModel == uint(idxPHONG) ? specularPhong(V, light, N.xyz) : (u.lightModel == uint(idxBLINPHONG) ? specularBlinnPhong(V, light, N.xyz) : specularGGX(V, light, N.xyz));
#else
    float specular = lightModel(V, light, N.xyz);
#endif

    vec3 lColor =  color.rgb * u.lightColor * lambertian * u.lightDiffInt +  //diffuse component
                    u.lightColor * specular * u.lightSpecInt;

    return vec4(packing2Colors16bit(lColor, color.rgb), getFragDepth(newVertex.z));
}

LAYOUT_INDEX(idxSOLID_DR) SUBROUTINE(_pixelColor)
vec4 pixelColorDR(vec4 color, vec4 N)
{
   return vec4(color.xyz, getFragDepth(newVertex.z));
}

LAYOUT_INDEX(idxBLENDING) SUBROUTINE(_pixelColor)  
vec4 pixelColorBlending(vec4 color, vec4 N)
{
    if(color.a < u.alphaSkip ) { discard; return vec4(0.0); }  //returm need for Angle error
    
    color *= alphaAttenuation;
    float dist = -newVertex.z;
    if(u.slowMotion!=uint(0) && dist<u.smoothDistance)
        color *= 1.0-(u.smoothDistance-dist)/u.smoothDistance;
    return color;
}

vec4 mainFunc(vec2 ptCoord)
{

    vec4 N = getParticleNormal(ptCoord);
        //if(pointDistance<u.clippingDist) discard;

    newVertex = mvVtxPos + vec4(0., 0., N.z * particleSize, 0.);

    if(N.w >= 1.0 || N.z < u.alphaSkip || -newVertex.z<u.clippingDist || clippedPoint(newVertex)) { discard;   } //returm need for Angle error

    gl_FragDepth = getFragDepth(newVertex.z);

    vec4 color = acquireColor(ptCoord);

    vec4 bound = colorBoundary(newVertex, 0) + colorBoundary(newVertex, 1) + colorBoundary(newVertex, 2);
    color.xyz = mix(color.xyz, bound.xyz, bound.a);

#if defined(GL_ES) || defined(GLCHAOSP_NO_USES_GLSL_SUBS)
    #if defined(GL_ES) && !defined(GLCHAOSP_LIGHTVER_EXPERIMENTAL)
        return u.lightActive==1u ? pixelColorDirect(color, N) : pixelColorBlending(color, N);
    #else        
        switch(u.renderType) {
            default:
            case uint(idxBLENDING) : return pixelColorBlending(color, N);
            case uint(idxSOLID   ) : return pixelColorDirect(color, N);
            case uint(idxSOLID_AO) : return pixelColorAO(color, N);
            case uint(idxSOLID_DR) : return pixelColorDR(color, N);             
        }
        //return (u.pass >= uint(2)) ? vec4(color.xyz, getDepth(newVertex.z)) : (u.pass==uint(0) ? retColor : vec4(retColor.xyz, getDepth(newVertex.z))); 
    #endif
#else   
        return pixelColor(color, N); 
#endif
}