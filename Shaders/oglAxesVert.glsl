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

layout (location = 1) in vec3 vPos;
layout (location = 2) in vec3 normal;

// I cant use direct location... for EMS and webgl
//layout (location = 4) uniform mat4 pMat;
//layout (location = 5) uniform mat4 mvMat;
//layout (location = 6) uniform mat4 zoomF;
uniform mat4 pMat;
uniform mat4 mvMat;
uniform vec3 zoomF;

// need for use pipeline 
#ifdef GLAPP_USE_PIPELINE
out gl_PerVertex
{
	vec4 gl_Position;
};
#endif

// I cant use blocks... for EMS and webgl

#ifdef GL_ES
out vec4 vsColor;
out vec3 vsNormal;
out vec3 vsWorldPos;
out vec3 vsPos;
#else
out vsOut {
    vec4 vsColor;
    vec3 vsNormal;
    vec3 vsWorldPos;
    vec3 vsPos;
};
#endif

const vec4 blue  = vec4(0.10, 0.10, 0.70, 1.0);
const vec4 green = vec4(0.10, 0.70, 0.10, 1.0);
const vec4 red   = vec4(0.70, 0.10, 0.10, 1.0);


//fast 90' rotation around fixzed axis
vec3 rotOnX(vec3 v) { return vec3(v.x, v.z, -v.y); }
vec3 rotOnY(vec3 v) { return vec3(v.z, v.y, -v.x); }

void main(void)
{
    
    vec3 newPos, newNorm;
    vec3 pos = vPos * zoomF;
    if(gl_InstanceID==1)      { newPos = rotOnX(pos); newNorm = rotOnX(normal); vsColor = green; }
    else if(gl_InstanceID==2) { newPos = rotOnY(pos); newNorm = rotOnY(normal); vsColor = red;   }
    else                      { newPos = pos;         newNorm = normal;         vsColor = blue;  }

    vec4 position = pMat * mvMat * vec4(newPos, 1.0f);
    vsPos = normalize(mat3(mvMat) * newPos);
    vsWorldPos =  position.xyz; //(mvMat * vec4(newPos,1.0)).xyz ;
    vsNormal = normalize(mat3(mvMat) * newNorm);
    
    gl_Position = position;
    //gl_Position = 
}