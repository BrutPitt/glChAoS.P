////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2018 Michele Morrone
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
//  This software is distributed under the terms of the BSD 2-Clause license:
//  
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//      * Redistributions of source code must retain the above copyright
//        notice, this list of conditions and the following disclaimer.
//      * Redistributions in binary form must reproduce the above copyright
//        notice, this list of conditions and the following disclaimer in the
//        documentation and/or other materials provided with the distribution.
//   
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
//  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
//  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
////////////////////////////////////////////////////////////////////////////////
#line 37    //#version dynamically inserted


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
    float velIntensity;
    float ySizeRatio;
    float ptSizeRatio;
    float pointspriteMinSize;
    float ggxRoughness;
    float ggxFresnel;
    int lightModel;
    bool lightActive;
} u;

#define idxPHONG 3
#define idxBLINPHONG 4
#define idxGGX 5

#ifndef GL_ES
    subroutine float _lightModel(vec3 L, vec3 N);
    subroutine uniform _lightModel lightModel;
#endif

in vec3 mvVtxPos;


in float pointDistance;
in vec4 particleColor;
in float particleSize;


//uniform float distAlphaFactor;
//uniform float alphaK;

// in OGL 4.5 "cull distance" to drop Vertex... to look in upgrade

float luminance(vec3 c) { return dot(c, vec3(0.2990, 0.5870, 0.1140)); }

float getAlpha(float alpha)
{

    CONST float alphaAtten = exp(-0.1*sign(pointDistance)*pow(abs(pointDistance+1.f)+1.f, u.alphaDistAtten*.1));
    return clamp(alpha * alphaAtten * u.alphaK, 0.0, 1.0);

}


LAYUOT_INDEX(idxPHONG) SUBROUTINE(_lightModel) float specularPhong(vec3 L, vec3 N)
{
    vec3 viewDir = normalize(-mvVtxPos);
    vec3 reflectDir = reflect(-L, N);
    float specAngle = max(dot(reflectDir, viewDir), 0.0);

    return pow(specAngle, u.lightShinExp * .25);
}

LAYUOT_INDEX(idxBLINPHONG) SUBROUTINE(_lightModel) float specularBlinnPhong(vec3 L, vec3 N)
{
// point on surface of sphere in eye space
    vec3 viewDir = normalize(-mvVtxPos);
    vec3 halfDir = normalize(L + viewDir);
    float specAngle = max(dot(halfDir, N), 0.0);

    return pow(specAngle, u.lightShinExp);

}

LAYUOT_INDEX(idxGGX) SUBROUTINE(_lightModel) float specularGGX(vec3 L, vec3 N) 
{
    float alpha = u.ggxRoughness*u.ggxRoughness;
    vec3 V = normalize(mvVtxPos);
    vec3 H = normalize(L - V); // View = -
    float dotLH = max(0.0, dot(L,H));
    float dotNH = max(0.0, dot(N,H));
    float dotNL = max(0.0, dot(N,L));
    float alphaSqr = alpha * alpha;

    // D (GGX normal distribution)
    float denom = dotNH * dotNH * (alphaSqr - 1.0) + 1.0;
    float D = alphaSqr / (3.141592653589793 * denom * denom);

    // F (Fresnel term)
    float F = u.ggxFresnel + (1.0 - u.ggxFresnel) * pow(1.0 - dotLH, 5.0);
    float k = 0.5 * alpha;
    float k2 = k * k;

    return dotNL * D * F / (dotLH*dotLH*(1.0-k2)+k2);
}

float linearizeDepth(float depth, float near, float far) 
{
    float z = -depth * 2.0 - (far - near);                          // back to NDC -depth/(far - near) * 2.0 - 1.0
    return (2.0 * near * far) / ((far + near) - z * (far - near));	
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


vec4 getLightedColor(vec2 coord, vec4 N)
{

    vec4 color = acquireColor(coord);

    //vec3 N;
    //N.xy = (coord - vec2(.5))*2.0; // xy = radius in 0
    //float mag = dot(N.xy, N.xy);
    //N.z = sqrt(1.0-mag);
    if (N.w > 1.0f || color.a < u.alphaSkip) discard;   // kill pixels outside circle: r=1.0
    
    N.xyz = normalize(N.xyz); 

    //Light @ vertex position
    vec3 light = normalize(u.lightDir+mvVtxPos);  // 
    
    float lambertian = max(0.0, dot(light, N.xyz)); 

#ifdef GL_ES
    float specular = u.lightModel == idxPHONG ? specularPhong(light, N.xyz) : (u.lightModel == idxBLINPHONG ? specularBlinnPhong(light, N.xyz) : specularGGX(light, N.xyz));
#else
    float specular = lightModel(light, N.xyz);
#endif


    vec3 lColor =  smoothstep(u.sstepColorMin, u.sstepColorMax,
                                /*color.rgb + */
                                color.rgb * u.lightColor * lambertian * u.lightDiffInt +  //diffuse component
                                u.lightColor * specular * u.lightSpecInt  + 
                                (color.rgb*u.lightAmbInt + vec3(u.lightAmbInt)) * .5); /* (color.rgb+u.lightAmbInt*0.1) * */


 
    return vec4(lColor.rgb , color.a);        
}


vec4 getColorOnly(vec2 coord)
{
    vec4 color = acquireColor(coord);

    if(color.a < u.alphaSkip ) discard ;

    return color;
}
