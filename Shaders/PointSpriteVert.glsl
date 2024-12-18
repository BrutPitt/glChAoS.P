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

out vec4 mvVtxPos;
#ifdef SHADOW_PASS
out vec4 solidVtx;
#endif
out float pointDistance;
out vec4 particleColor;
out float particleSize;
out float alphaAttenuation;
out float magnitudeFrag;


void main()
{

#ifdef SHADOW_PASS
    vec4 vtxPos =  m.mvLightM  * vec4(a_ActualPoint.xyz,1.0);
    solidVtx = m.mvMatrix * vec4(a_ActualPoint.xyz,1.0);
#else
    vec4 vtxPos = m.mvMatrix * vec4(a_ActualPoint.xyz,1.0);
#endif

#if defined(GL_ES)
    particleColor = velColor();
#elif defined(GLCHAOSP_NO_USES_GLSL_SUBS)
    particleColor = u.colorizingMethod == 0 ? velColor() : objColor();
#else
    particleColor = colorResult();
#endif
    gl_Position = m.pMatrix * vtxPos;

    mvVtxPos = vec4(vtxPos.xyz,1.0);

    pointDistance = gl_Position.w; //length(vtxPos.w);

    float ptAtten = exp(-0.01*sign(pointDistance)*pow(abs(pointDistance)+1.0, u.pointDistAtten*.1));
    float size = u.pointSize * ptAtten * u.ySizeRatio;

    particleSize = size*u.invScrnRes.y;

    vec4 pt  = m.pMatrix * vec4(vtxPos.xy + vec2(size) * u.ptSizeRatio , vtxPos.zw);

    // TF time-dependent values to pass to fragment
    alphaAttenuation = getLifeTimeAtten();
    magnitudeFrag = getMagnitudoAtten();

    // NVidia & Intel do not supports gl_PointSize<1.0 -> point disappear
    // AMD driver (some times) supports gl_PointSize from 0.1
    // Look in Info dialog: point Range and Granularity
    gl_PointSize = max(distance(gl_Position.xyz, pt.xyz)/max(abs(gl_Position.w),.0001), u.pointspriteMinSize) * getMagnitude();
#ifdef SHADOW_PASS
     gl_PointSize *= u.shadowDetail;
#endif
}
