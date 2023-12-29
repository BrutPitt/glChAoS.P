////////////////////////////////////////////////////////////////////////////////
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
#line 17    //#version dynamically inserted

layout(std140) uniform;

LAYOUT_LOCATION(0) out vec4 color;
LAYOUT_BINDING(0) uniform sampler2D paletteTex;

LAYOUT_BINDING(2) uniform _cmData {
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
    if(u.clmp) tCoord = clamp(tCoord, 0.01, .99); //clamp?

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