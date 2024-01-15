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


out float pointDist;
out vec4 vertParticleColor;
out vec4 vertShadowlightView;
out float alphaAttenuationVert;
out float magnitudeVert;



void main(void)
{

    gl_Position = m.mvMatrix * vec4(a_ActualPoint.xyz,1.f);

    pointDist = length(gl_Position.xyz);



    float ptAtten = exp(-0.01*sign(pointDist)*pow(abs(pointDist)+1.f, u.pointDistAtten*.1));
    gl_PointSize = u.pointSize*u.invScrnRes.y * ptAtten * u.ySizeRatio * getMagnitude();

// TF time-dependent values to pass to fragment
    alphaAttenuationVert = getLifeTimeAtten();
    magnitudeVert = getMagnitudoAtten();

#if defined(GL_ES)
    vertParticleColor = velColor();
#elif defined(GLCHAOSP_NO_USES_GLSL_SUBS)
    vertParticleColor = u.colorizingMethod == 0 ? velColor() : objColor();
#else
    vertParticleColor = colorResult();
#endif


}
