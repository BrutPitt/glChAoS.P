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

in vec3 posEyeVS[];
in vec4 particleColor[];
in float pointDistance[];

out vec3 posEye;
out float pointSZ;
out vec2 texCoord;
out vec4 geomParticleColor;
out float ptDist;

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


void main(void){
  vec4 position;
  mat2 RotationMatrix;

//Get elapsed time in texcoord.y to zoom particle in 5 sec.
  
  posEye  = posEyeVS[0];
  float sz  = gl_in[0].gl_PointSize;
  ptDist = pointDistance[0];

  pointSZ = sz;


  RotationMatrix[0] = vec2(1.0, 0.0); //vec2(cos(InfoIn[0].x),-sin(InfoIn[0].x));
  RotationMatrix[1] = vec2(0.0, 1.0); //vec2(sin(InfoIn[0].x), cos(InfoIn[0].x));  
  geomParticleColor = particleColor[0];

  
  //Vertex 1
  position = gl_in[0].gl_Position;
  position.xy += RotationMatrix * (vec2(-sz,-sz));
  gl_Position = m.pMatrix * position;
  texCoord = vec2(0.0,0.0);
  EmitVertex();

  //Vertex 2
  position = gl_in[0].gl_Position;
  position.xy += RotationMatrix * (vec2(-sz, sz));
  gl_Position = m.pMatrix * position;
  texCoord = vec2(0.0,1.0);
  EmitVertex();
  
  //Vertex 3
  position = gl_in[0].gl_Position;
  position.xy += RotationMatrix * (vec2( sz,-sz));
  gl_Position = m.pMatrix * position;
  texCoord = vec2(1.0,0.0);
  EmitVertex();

  //Vertex 4
  position = gl_in[0].gl_Position;
  position.xy += RotationMatrix * (vec2( sz, sz));
  gl_Position = m.pMatrix * position;
  texCoord = vec2(1.0,1.0);
  EmitVertex();

  EndPrimitive();

}


