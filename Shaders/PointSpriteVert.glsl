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

out vec4 mvVtxPos;
//out vec4 mvVtxLightPos;

out float pointDistance;
out vec4 particleColor;
out float particleSize;

out vec4 shadowlightView;


void main()
{


#ifdef SHADOW_PASS
    vec4 vtxPos =  m.mvLightM  * vec4(a_ActualPoint.xyz,1.f);
    //gl_Position = m.pLightM * vtxPos;
    //gl_PointSize = 1.0;
#else
    vec4 vtxPos = m.mvMatrix * vec4(a_ActualPoint.xyz,1.f);
    vec4 shadowlightView = m.mvLightM * vec4(a_ActualPoint.xyz,1.f);
#endif

    #if defined(GL_ES) || defined(TEST_WGL)
        particleColor = velColor();
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

    // NVidia & Intel do not supports gl_PointSize<1.0 -> point disappear
    // AMD driver (some times) supports gl_PointSize from 0.1
    // Look in Info dialog: point Range and Granularity
    gl_PointSize = max(distance(gl_Position.xyz, pt.xyz)/max(abs(gl_Position.w),.0001), u.pointspriteMinSize);
}
