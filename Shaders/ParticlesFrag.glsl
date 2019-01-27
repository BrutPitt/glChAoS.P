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

// #version dynamically inserted

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
    bool lightActive;
} u;

//vec3 lightColor = vec3(.5,.9,0.0);

//uniform vec3 light;
in vec3 mvVtxPos;


in float pointDistance;
in vec4 particleColor;

//uniform float distAlphaFactor;
//uniform float alphaK;

// in OGL 4.5 "cull distance" to drop Vertex... to look in upgrade

float luminance(vec3 c) { return dot(c, vec3(0.2990, 0.5870, 0.1140)); }

float getAlpha(float alpha)
{

    CONST float alphaAtten = exp(-0.1*sign(pointDistance)*pow(abs(pointDistance+1.f)+1.f, u.alphaDistAtten*.1));
    return clamp(alpha * alphaAtten * u.alphaK, 0.0, 1.0);

}

float getSpecularPhong(vec3 L, vec3 N)
{
    vec3 viewDir = normalize(-mvVtxPos);
    vec3 reflectDir = reflect(-L, N);
    float specAngle = max(dot(reflectDir, viewDir), 0.0);

    return pow(specAngle, u.lightShinExp * .25);
}

float getSpecularBlinnPhong(vec3 L, vec3 N)
{
// point on surface of sphere in eye space
    vec3 viewDir = normalize(-mvVtxPos);
    vec3 halfDir = normalize(L + viewDir);
    float specAngle = max(dot(halfDir, N), 0.0);

    return pow(specAngle, u.lightShinExp);

}

float ggx (vec3 N, vec3 V, vec3 L, float roughness, float F0) 
{
    float alpha = roughness*roughness;
    vec3 H = normalize(L - V);
    float dotLH = max(0.0, dot(L,H));
    float dotNH = max(0.0, dot(N,H));
    float dotNL = max(0.0, dot(N,L));
    float alphaSqr = alpha * alpha;
    float denom = dotNH * dotNH * (alphaSqr - 1.0) + 1.0;
    float D = alphaSqr / (3.141592653589793 * denom * denom);
    float F = F0 + (1.0 - F0) * pow(1.0 - dotLH, 5.0);
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


vec4 getLightedColor(vec2 coord)
{

    vec4 color = acquireColor(coord);

    vec3 N;
    N.xy = (coord - vec2(.5))*2.0; // xy = radius in 0

    float mag = dot(N.xy, N.xy);
    if (mag > 1.0f || color.a < u.alphaSkip) discard;   // kill pixels outside circle: r=1.0
    
    N.z = sqrt(1.0-mag);
    N = normalize(N);

    vec3 light = normalize(u.lightDir+mvVtxPos);  // 
    
    float lambertian = max(0.0, dot(light, N)); 

    float specular = getSpecularBlinnPhong(light, N);
    //specular = ggx(N, normalize(mvVtxPos), normalize(u.lightDir),  u.lightShinExp * .1, .12);
    //float lightDist = distance(u.lightDir, mvVtxPos);
    //float lightAtten = 1.0 / (lightDist * lightDist);


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
