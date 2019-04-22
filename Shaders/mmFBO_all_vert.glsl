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

layout (location = 0) in vec2 vPos;

#ifdef GL_ES
#else
out gl_PerVertex
{
	vec4 gl_Position;
};
#endif

out vec2 vTexCoord;
vec2 texCoord[4] = vec2[4](vec2(0.0, 0.0), vec2(1.0, 0.0), vec2(1.0, 1.0), vec2(0.0, 1.0));

void main(void)
{
    vTexCoord = texCoord[gl_VertexID];
    gl_Position = vec4(vPos.xy,.0f,1.f);

}