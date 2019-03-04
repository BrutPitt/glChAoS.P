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
#line 37 // #include ColorSpaces.glsl

layout(std140) uniform;

#if defined(GL_ES) || defined(TEST_WGL)
out vec4 color;
uniform sampler2D paletteTex;

#else
layout (location = 0) out vec4 color;

LAYUOT_BINDING(0) uniform sampler2D paletteTex;
#endif

LAYUOT_BINDING(2) uniform _cmData {
    vec3 hslData;
    float offsetPoint;
    float palRange;
    bool clmp;
    bool reverse;
} u;


vec4 pal0() { return texelFetch(paletteTex, ivec2(gl_FragCoord.x,0),0); }

vec4 pal1() 
{       
    float tCoord = u.offsetPoint+gl_FragCoord.x*u.palRange/256.0;
    
    if(u.reverse) tCoord = 1.0-tCoord; //reverse?    
    if(u.clmp)    tCoord = clamp(tCoord, 0.0, .99); //clamp?

    vec4 c = texture(paletteTex, vec2(tCoord,0.0),0.0);
    c.xyz = rgb2hsl(c.xyz) + vec3(u.hslData.x*.5, u.hslData.yz);

#if defined(GL_ES)
    c.yz = clamp(c.yz, vec2(0.0), vec2(1.0));
#else
    c.yz = clamp(c.yz, 0.0, 1.0);
#endif
    return vec4(hsl2rgb(c.xyz), 1.0);
}

void main(void)
{

    color = pal1();

    //color1 = texelFetch(paletteTex, int(256.0*vData.y)+int(gl_FragCoord.x*vData.z),0);
    //color = vec4(0.0, 0.8, 1.0, 1.0);
   
}