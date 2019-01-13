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

// #include ParticlesFrag.glsl

#ifdef GL_ES
    #define SUBROUTINE(X) 
#else
    subroutine vec4 _pixelColor();
    subroutine uniform _pixelColor pixelColor;
    #define SUBROUTINE(X) subroutine(X)
#endif

out vec4 outColor;

LAYUOT_INDEX(1) SUBROUTINE(_pixelColor) vec4 pixelColorLight()
{

    return getLightedColor(gl_PointCoord);

}

LAYUOT_INDEX(0) SUBROUTINE(_pixelColor)  vec4 pixelColorOnly()
{

    return getColorOnly(gl_PointCoord);

}



void main()
{

#ifdef GL_ES
    outColor = pixelColorOnly();
#else
    //gl_FragDepth = -posEye.z*u.zFar;
    //gl_FragDepth = (u.zNear - u.zFar/posEye.z);
    //outColor = vec4(vec3((1.0+gl_FragDepth)*.5), 1.0);

    gl_FragDepth = LinearizeDepth(posEye.z, u.zNear, u.zFar);
    outColor = pixelColor();
#endif
}
