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

in vec4 mvVtxPos;
in vec4 solidVtx;
in float particleSize;
in float alphaAttenuation;


void main()
{

    vec2 ptCoord = vec2(gl_PointCoord.x,1.0-gl_PointCoord.y);
    vec4 N = getParticleNormal(ptCoord);

    vec4 newVertex = solidVtx + vec4(0., 0., N.z * particleSize, 0.);

    if(N.w > 1.0 || N.z < u.alphaSkip || -mvVtxPos.z<u.clippingDist || clippedPoint(newVertex) || alphaAttenuation<.25) { discard; } //return black color and max depth
    
    gl_FragDepth = getFragDepth((mvVtxPos.z + N.z * particleSize*u.shadowDetail));

} 