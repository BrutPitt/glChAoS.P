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
#line 12    //#version dynamically inserted

out vec4 mvVtxPos;
#ifdef SHADOW_PASS
out vec4 solidVtx;
#endif
out float pointDistance;
out vec4 particleColor;
out float particleSize;
out float alphaAttenuation;


void main()
{

#ifdef SHADOW_PASS
    vec4 vtxPos =  m.mvLightM  * vec4(a_ActualPoint.xyz,1.f);
    solidVtx = m.mvMatrix * vec4(a_ActualPoint.xyz,1.f);
#else
    vec4 vtxPos = m.mvMatrix * vec4(a_ActualPoint.xyz,1.f);
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

    float ptAtten = exp(-0.01*sign(pointDistance)*pow(abs(pointDistance)+1.f, u.pointDistAtten*.1));
    float size = u.pointSize * ptAtten * u.ySizeRatio;

    particleSize = size*u.invScrnRes.y;

    vec4 pt  = m.pMatrix * vec4(vtxPos.xy + vec2(size) * u.ptSizeRatio , vtxPos.zw);

    alphaAttenuation = getLifeTimeAtten();


    // NVidia & Intel do not supports gl_PointSize<1.0 -> point disappear
    // AMD driver (some times) supports gl_PointSize from 0.1
    // Look in Info dialog: point Range and Granularity
    
#ifdef SHADOW_PASS
    gl_PointSize = max(distance(gl_Position.xyz, pt.xyz)/max(abs(gl_Position.w),.0001), u.pointspriteMinSize) * u.shadowDetail;
#else
    gl_PointSize = max(distance(gl_Position.xyz, pt.xyz)/max(abs(gl_Position.w),.0001), u.pointspriteMinSize);
#endif
}
