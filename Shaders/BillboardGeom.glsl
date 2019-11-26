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

layout(std140) uniform;

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec4 vertParticleColor[];
in vec4 vertShadowlightView[];

in float pointDist[];

out vec4 mvVtxPos;
out vec2 texCoord;

out vec4 particleColor;
out float pointDistance;
out float particleSize;


LAYOUT_BINDING(4) uniform _tMat {
    mat4 vMatrix;
    mat4 pMatrix;
    mat4 invP;
    mat4 mvMatrix;
    mat4 invMV  ;
    mat4 mvpMatrix;
    mat4 mvLightM;
    mat4 mvpLightM;
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
    particleSize = sz;

    vec4 position = gl_in[0].gl_Position;
    mvVtxPos = position;
  
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


