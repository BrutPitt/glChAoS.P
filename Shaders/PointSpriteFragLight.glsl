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

void main()
{

    vec2 ptCoord = vec2(gl_PointCoord.x,1.0-gl_PointCoord.y); //upsideDown: revert point default
    outColor = mainFunc(ptCoord);
/*
void mainImage( out vec4 fragColor, in vec2 fragCoord )
{
    vec2 uv = fragCoord.xy / iResolution.xy;
    vec4 color =  texture(iChannel0, uv);
    float gray = length(color.rgb);
    vec3 dx = dFdx(vec3(uv,gray));
    vec3 dy = dFdy(vec3(uv,gray));
    vec3 N = normalize(cross(dx, dy));
    fragColor = vec4(N,1.0);
}*/
}
