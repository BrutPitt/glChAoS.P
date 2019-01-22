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

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec4 vertParticleColor[];
in float pointDist[];

out vec3 mvVtxPos;
out vec2 texCoord;
out vec4 particleColor;
out float pointDistance;

LAYUOT_BINDING(4) uniform _tMat {
    mat4 pMatrix;
    mat4 mvMatrix;
    mat4 mvpMatrix;
} m;

in gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;

} gl_in[];

out gl_PerVertex
{
    vec4 gl_Position;
    float gl_PointSize;
};


void main(void)
{
    pointDistance = pointDist[0];
    particleColor = vertParticleColor[0];

    float sz = gl_in[0].gl_PointSize;

    vec4 position = gl_in[0].gl_Position;
    mvVtxPos = position.xyz;
  
    //Vertex 1
    gl_Position = m.pMatrix * vec4(position.xy+vec2(-sz,-sz),position.zw);
    texCoord = vec2(0.0,0.0);
    EmitVertex();

    //Vertex 2
    gl_Position = m.pMatrix * vec4(position.xy+vec2(-sz, sz),position.zw);
    texCoord = vec2(0.0,1.0);
    EmitVertex();
  
    //Vertex 3
    gl_Position = m.pMatrix * vec4(position.xy+vec2( sz,-sz),position.zw);
    texCoord = vec2(1.0,0.0);
    EmitVertex();

    //Vertex 4
    gl_Position = m.pMatrix * vec4(position.xy+vec2( sz, sz),position.zw);
    texCoord = vec2(1.0,1.0);
    EmitVertex();

    EndPrimitive();

}


