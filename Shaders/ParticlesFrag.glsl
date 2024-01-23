//------------------------------------------------------------------------------
//  Copyright (c) 2018-2020 Michele Morrone
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

//#define LAYOUT_BINDING(X)  layout(binding = X)
//#define LAYOUT_LOCATION(X) layout(location = X)

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
in float magnitudeFrag;

//in vec4 shadowlightView;

layout (location = 0) out vec4 outColor;
layout (location = 1) out vec4 baseColor;  // color for postprocessing w/o light attribute

// multi out
//layout (location = 1) out vec4 vtxOut;
//layout (location = 2) out vec4 normOut;

//uniform float distAlphaFactor;
//uniform float alphaK;

// in OGL 4.5 "cull distance" to drop Vertex... to look in upgrade

float luminance(vec3 c) { return dot(c, vec3(0.2990, 0.5870, 0.1140)); }

float getAlpha(float alpha)
{

    float alphaAtten = exp(-0.1*sign(pointDistance)*pow(abs(pointDistance+1.f)+1.f, u.alphaDistAtten*.1));
    return clamp(alpha * alphaAtten * u.alphaK, 0.0, 1.0);

}

vec4 acquireColor(vec2 coord)
{
    vec4 color = particleColor * texture(tex, coord).r;
    float mag = clamp(magnitudeFrag, 0.0, 1.0);

    vec3 newColor = (u.slowMotion != uint(0)) ?
                    (color.rgb + vec3(u.magnitudeInt) * mag) * u.colIntensity :
                     color.rgb  * u.colIntensity;

    //vec3 newColor = mix(color.rgb, vec3(1.0), mag) * u.colIntensity;

    return vec4(newColor , getAlpha(color.a)) ;
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

    vec3 light = u.lightDir;  // already normalized light

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
    vec3 light = u.lightDir; // already normalized light

    float lambertian = max(0.0, dot(light, N.xyz));

    vec3 V = normalize(newVertex.xyz);
#if defined(GL_ES) || defined(GLCHAOSP_NO_USES_GLSL_SUBS)
    float specular = u.lightModel == uint(idxPHONG) ? specularPhong(V, light, N.xyz) : (u.lightModel == uint(idxBLINPHONG) ? specularBlinnPhong(V, light, N.xyz) : specularGGX(V, light, N.xyz));
#else
    float specular = lightModel(V, light, N.xyz);
#endif

    vec3 lColor =  color.rgb * u.lightColor * lambertian * u.lightDiffInt +  //diffuse component
                   u.lightColor * specular * u.lightSpecInt;

    return vec4(lColor, color.a);
}

LAYOUT_INDEX(idxSOLID_DR) SUBROUTINE(_pixelColor)
vec4 pixelColorDR(vec4 color, vec4 N)
{
   return color;
}

LAYOUT_INDEX(idxBLENDING) SUBROUTINE(_pixelColor)  
vec4 pixelColorBlending(vec4 color, vec4 N)
{
    if(color.a < u.alphaSkip ) { color.a = 0.0; return color; }
    else {
        color *= alphaAttenuation;
        float dist = -newVertex.z;
        if(u.slowMotion!=uint(0) && dist<u.smoothDistance)
            color *= 1.0-(u.smoothDistance-dist)/u.smoothDistance;
        //color.xyz  *= color.a;
        return color;
    }
}


vec4 mainFunc(vec2 ptCoord)
{

    vec4 N = getParticleNormal(ptCoord);
        //if(pointDistance<u.clippingDist) discard;

    newVertex = mvVtxPos + vec4(0., 0., N.z * particleSize, 0.);
    vec4 color = acquireColor(ptCoord);

    if(N.w > 1.0 || N.z < u.alphaSkip || -mvVtxPos.z<u.clippingDist || clippedPoint(newVertex))  discard;
    else {

        gl_FragDepth = getFragDepth(newVertex.z);

        // plane bound color
        vec4 bound = colorBoundary(newVertex, 0) + colorBoundary(newVertex, 1) + colorBoundary(newVertex, 2);
        color.xyz = mix(color.xyz, bound.xyz, bound.a);

        color.a = clamp(color.a, .01, .99);

        baseColor = color;

#if defined(GL_ES) || defined(GLCHAOSP_NO_USES_GLSL_SUBS)
    #ifdef GLCHAOSP_NO_AO_SHDW
        color = u.lightActive==1u ? pixelColorDirect(color, N) : pixelColorBlending(color, N);
    #else
        switch(u.renderType) {
            default:
            case uint(idxBLENDING) : color = pixelColorBlending(color, N); break;
            case uint(idxSOLID   ) : color = pixelColorDirect(color, N);   break;
            case uint(idxSOLID_AO) : color = pixelColorAO(color, N);       break;
            case uint(idxSOLID_DR) : color = pixelColorDR(color, N);       break;
        }
    #endif
#else
        color = pixelColor(color, N);
#endif
    }
    return color;
}