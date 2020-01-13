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

in vec4 mvVtxPos;
in vec4 solidVtx;
in float particleSize;
in float alphaAttenuation;


void main()
{

    vec2 ptCoord = vec2(gl_PointCoord.x,1.0-gl_PointCoord.y);
    vec4 N = getParticleNormal(ptCoord);

    if(N.w > 1.0 || N.z < u.alphaSkip || -mvVtxPos.z<u.clippingDist || clippedPoint(solidVtx) || alphaAttenuation<.25) { discard; } //return black color and max depth
    
    gl_FragDepth = getFragDepth((mvVtxPos.z + N.z * particleSize*u.shadowDetail));

} 