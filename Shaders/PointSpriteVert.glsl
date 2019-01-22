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
/*
#if __VERSION__ >= 420
    #define LAYUOT_BINDING(X) layout (location = X)
//    #define SHARED_BINDING(X) layout (shared, location = X)
#else
    #define LAYUOT_BINDING(X)
#endif*/


layout(std140) uniform;

uniform float velIntensity;

LAYUOT_BINDING(2) uniform _particlesData {
    vec3 lightDir;
    float lightDiffInt;
    vec3 lightColor; 
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
    bool lightActive;
} u;

LAYUOT_BINDING(4) uniform _tMat { //shared?
    mat4 pMatrix;
    mat4 mvMatrix;
    mat4 mvpMatrix;
} m;

#ifndef GL_ES
out gl_PerVertex
{
	vec4 gl_Position;
    float gl_PointSize;
};
#endif


out vec3 mvVtxPos;

out float pointDistance;
out vec4 particleColor;

LAYUOT_BINDING(0) uniform sampler2D paletteTex;

layout (location = 0) in vec4 a_ActualPoint;


void main()                                                 
{              

    vec4 vtxPos = m.mvMatrix * vec4(a_ActualPoint.xyz,1.f);
    gl_Position = m.pMatrix * vtxPos;
    mvVtxPos = vtxPos.xyz;

    float vel = a_ActualPoint.w*u.velIntensity;
    particleColor = vec4(texture(paletteTex, vec2(vel,0.f)).rgb,1.0);

    pointDistance = gl_Position.w; //length(vtxPos.w);


    float ptAtten = exp(-0.01*sign(pointDistance)*pow(abs(pointDistance+1.f), u.pointDistAtten*.1));
    float size = u.pointSize * ptAtten * u.ySizeRatio;

    vec4 pt  = m.pMatrix * vec4(vtxPos.xy + vec2(size) * u.ptSizeRatio , vtxPos.zw);
    gl_PointSize = abs(gl_Position.w)>0.00001 ? distance(gl_Position.xy, pt.xy)/gl_Position.w  : 0.0;

    //if(gl_PointSize<1.0) gl_PointSize = 1.0;
}                                                           

