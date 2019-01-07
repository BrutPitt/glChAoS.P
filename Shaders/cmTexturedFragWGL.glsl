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
out vec4 color;

uniform bool whichPalette;
uniform vec4 vData;   //x: velIntensity, y: offsetPoint, z: lenSegment, w: rotate (bool)
uniform vec4 hslData; // w: clamp!!! (bool)

uniform sampler2D paletteTex;

void main(void)
{
    if(whichPalette) {
        float tCoord = vData.y+gl_FragCoord.x*vData.z/256.0;
    
        if(vData.w != 0.0)   tCoord = 1.0-tCoord; //reverse?    
        if(hslData.w != 0.0) tCoord = clamp(tCoord, 0.0, .999); //clamp?

        vec4 c = texture(paletteTex, vec2(tCoord,0),0);
        c.xyz = rgb2hsl(c.xyz) + hslData.xyz;

        color = vec4(hsl2rgb(vec3(c.x, saturate(c.yz))), c.a);
    } else
        color = texelFetch(paletteTex, ivec2(gl_FragCoord.x,0),0);

}