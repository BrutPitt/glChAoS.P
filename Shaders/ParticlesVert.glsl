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
#line 37    // #version dynamically inserted

layout(std140) uniform;

layout (location = 0) in vec4 a_ActualPoint;

LAYUOT_BINDING(0) uniform sampler2D paletteTex;

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

LAYUOT_BINDING(4) uniform _tMat {
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


#ifndef GL_ES
    subroutine vec4 _colorResult();
    subroutine uniform _colorResult colorResult;
#endif

// Load OBJ
#ifndef GL_ES
LAYUOT_INDEX(1) SUBROUTINE(_colorResult) vec4 objColor()
{
    uint packCol = floatBitsToUint(a_ActualPoint.w);
    vec4 col = unpackUnorm4x8(packCol);

    return col;
}
#endif

LAYUOT_INDEX(0) SUBROUTINE(_colorResult) vec4 velColor()
{
    float vel = a_ActualPoint.w*u.velIntensity;
    return vec4(texture(paletteTex, vec2(vel,0.f)).rgb,1.0);
}
