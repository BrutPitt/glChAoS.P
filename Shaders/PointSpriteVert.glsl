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
#line 37    // #version dynamically inserted

out vec3 mvVtxPos;

out float pointDistance;
out vec4 particleColor;
out float particleSize;

void main()
{

    vec4 vtxPos = m.mvMatrix * vec4(a_ActualPoint.xyz,1.f);
    gl_Position = m.pMatrix * vtxPos;
    mvVtxPos = vtxPos.xyz;

#if defined(GL_ES) || defined(TEST_WGL)
    particleColor = velColor();
#else
    particleColor = colorResult();
#endif

    pointDistance = gl_Position.w; //length(vtxPos.w);

    float ptAtten = exp(-0.01*sign(pointDistance)*pow(abs(pointDistance)+1.f, u.pointDistAtten*.1));
    float size = u.pointSize * ptAtten * u.ySizeRatio;
    particleSize = size/u.scrnRes.y;

    vec4 pt  = m.pMatrix * vec4(vtxPos.xy + vec2(size) * u.ptSizeRatio , vtxPos.zw);
    gl_PointSize = distance(gl_Position.xyz, pt.xyz)/max(abs(gl_Position.w),.0001);

    // NVidia & Intel do not supports gl_PointSize<1.0 -> point disappear
    // Look in Info dialog: point Range and Granularity
    if(gl_PointSize<u.pointspriteMinSize) gl_PointSize = u.pointspriteMinSize;
}
