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

LAYOUT_BINDING(0) uniform sampler2D billboardTex;
LAYOUT_BINDING(1) uniform sampler2D pointTex;

uniform float mixingVal;

layout (location = 0) out vec4 color;

void main ()
{

    ivec2 coord = ivec2(gl_FragCoord.xy);
    vec4 c0 = texelFetch(billboardTex, coord, 0);
    vec4 c1 = texelFetch(pointTex,     coord, 0); 

    color = mix(c0, c1, mixingVal);

}