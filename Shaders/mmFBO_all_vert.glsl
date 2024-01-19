//------------------------------------------------------------------------------
//  Copyright (c) 2018-2024 Michele Morrone
//  All rights reserved.
//
//  https://michelemorrone.eu - https://glchaosp.com - https://brutpitt.com
//
//  X: https://x.com/BrutPitt - GitHub: https://github.com/BrutPitt
//
//  direct mail: brutpitt(at)gmail.com - me(at)michelemorrone.eu
//
//  This software is distributed under the terms of the BSD 2-Clause license
//------------------------------------------------------------------------------
#line 13    //#version dynamically inserted


layout (location = 0) in vec2 vPos;

#if !defined(GL_ES)
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