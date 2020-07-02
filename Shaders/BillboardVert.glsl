//------------------------------------------------------------------------------
//  Copyright (c) 2018-2020 Michele Morrone
//  All rights reserved.
//
//  https://michelemorrone.eu - https://BrutPitt.com
//
//  twitter: https://twitter.com/BrutPitt - github: https://github.com/BrutPitt
//
//  mailto:brutpitt@gmail.com - mailto:me@michelemorrone.eu
//  
//  This software is distributed under the terms of the BSD 2-Clause license
//------------------------------------------------------------------------------
#line 14    //#version dynamically inserted


out float pointDist;
out vec4 vertParticleColor;
out vec4 vertShadowlightView;
out float alphaAttenuationVert;
out float magnitudeVert;



void main(void)
{

    gl_Position = m.mvMatrix * vec4(a_ActualPoint.xyz,1.f);

    pointDist = length(gl_Position.xyz);

    magnitudeVert = getMagnitudoAtten();


    float ptAtten = exp(-0.01*sign(pointDist)*pow(abs(pointDist)+1.f, u.pointDistAtten*.1));
    gl_PointSize = u.pointSize*u.invScrnRes.y * ptAtten * u.ySizeRatio * getMagnitude();

    alphaAttenuationVert = getLifeTimeAtten();

#if defined(GL_ES)
    vertParticleColor = velColor();
#elif defined(GLCHAOSP_NO_USES_GLSL_SUBS)
    vertParticleColor = u.colorizingMethod == 0 ? velColor() : objColor();
#else
    vertParticleColor = colorResult();
#endif


}
