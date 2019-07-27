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

in vec4 mvVtxPos;
in float particleSize;

void main()
{

    vec2 ptCoord = vec2(gl_PointCoord.x,1.0-gl_PointCoord.y);
    vec4 N = getParticleNormal(ptCoord);
    if(N.w > 1.0 || N.z < u.alphaSkip) { discard; return; } //return black color and max depth

//linear
    float z = mvVtxPos.z + N.z * particleSize;
    //z = (z + u.zNear) / (u.zFar - u.zNear);
    gl_FragDepth = getDepth(z);

    //gl_FragDepth = mvVtxPos.z + N.z * particleSize;

} 