////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2018-2020 Michele Morrone
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

LAYOUT_BINDING(0) uniform sampler2D sourceRendered;
LAYOUT_BINDING(1) uniform sampler2D accumMotion;

uniform float blurIntensity;

layout (location = 0) out vec4 color;

void main ()
{


    ivec2 coord = ivec2(gl_FragCoord.xy);
    vec4 c0 = texelFetch(sourceRendered, coord,0);
    vec4 c1 = texelFetch(accumMotion, coord,0);


    //gl_FragColor = vec4(mix(c1.rgb, c0.rgb, c0.rgb), c0.a*c1.a)*1.1;
    //gl_FragColor = mix(c1, c0, c0*.5)/blurK;

    color = mix(c0, vec4(mix(c1.rgb, c0.rgb, c0.rgb), c0.a) , blurIntensity) ;



}