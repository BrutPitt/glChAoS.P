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

out float pointDist;
out vec4 vertParticleColor;

void main(void)
{

    gl_Position = m.mvMatrix * vec4(a_ActualPoint.xyz,1.f);

    pointDist = length(gl_Position.xyz); 

    float ptAtten = exp(-0.01*sign(pointDist)*pow(abs(pointDist)+1.f, u.pointDistAtten*.1));
    gl_PointSize = u.pointSize/u.scrnRes.y * ptAtten * u.ySizeRatio;

#if defined(GL_ES) || defined(TEST_WGL)
    vertParticleColor = velColor();
#else
    vertParticleColor = colorResult();
#endif


}
