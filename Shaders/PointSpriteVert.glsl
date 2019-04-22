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

out float pointDistance;
out vec4 particleColor;
out float particleSize;

out vec3 viewportPixelCoord;

float logzbuf( float w ) {
    float fc = 2.0/log2(u.zFar + 1.0);
    return (log(1. + w) * fc - 1.) * w;
}


void main()
{


    vec4 vtxPos = m.mvMatrix * vec4(a_ActualPoint.xyz,1.f);
    gl_Position = m.pMatrix * vtxPos;


//    float Fcoef = 2.0 / log2(u.zFar + 1.0);
//    gl_Position.z = log2(max(1e-6, 1.0 + gl_Position.w)) * Fcoef - 1.0;
    //gl_Position.z = logzbuf(gl_Position.w);
    mvVtxPos = vec4(vtxPos.xyz,1.0 + gl_Position.w);

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
