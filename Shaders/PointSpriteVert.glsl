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

// #version dynamically inserted
/*
#if __VERSION__ >= 420
    #define LAYUOT_BINDING(X) layout (location = X)
//    #define SHARED_BINDING(X) layout (shared, location = X)
#else
    #define LAYUOT_BINDING(X)
#endif*/


layout(std140) uniform;

layout (location = 0) in vec4 a_ActualPoint;

uniform float velIntensity;

LAYUOT_BINDING(2) uniform _particlesData {
    float lightDiffInt;
    float lightSpecInt;
    float lightAmbInt ;
    float lightShinExp;
    vec3 lightDir;
    float sstepColorMin;
    float sstepColorMax;
    float pointSize;
    float pointDistAtten;
    float alphaDistAtten;
    float alphaSkip;
    float alphaK;
    float clippingDist;
    float zFar;
    float velIntensity;
} u;

LAYUOT_BINDING(4) uniform _tMat { //shared?
    mat4 pMatrix;
    mat4 mvMatrix;
    mat4 mvpMatrix;
} m;



out vec3 posEye;
out vec4 LightRay;
out float pointSZ;

out float pointDistance;
out vec4 particleColor;

LAYUOT_BINDING(0) uniform sampler2D paletteTex;



out gl_PerVertex
{
	vec4 gl_Position;
    float gl_PointSize;
};


void main()                                                 
{              

    vec4 vtxPos = m.mvMatrix * vec4(a_ActualPoint.xyz,1.f);
    float vel = a_ActualPoint.w*u.velIntensity;

    vec4 cOut = vec4(texture(paletteTex, vec2(vel,0.f)).rgb,1.0);
    
        
        posEye = vtxPos.xyz;

        float dist = length(posEye);
        pointDistance = dist;
        
        //gl_PointSize = dist<clippingDist ? 0.0 : pointSize / max(.01,pow(dist,distSizeFactor)); 
        float ptAtten = exp(-0.01*pow(dist+1.f, u.pointDistAtten*.1));
        gl_PointSize = dist<u.clippingDist ? 0.0 : u.pointSize * ptAtten ; 

        if(gl_PointSize<1.0) gl_PointSize = 1.0;

        //1.0 / max(.01,pow(dist,u.pointDistAtten)); 


        
        //float ps = pointSize / max(.01,pow(dist,distSizeFactor));
        //dropPoint = ps < gl_Point.sizeMin ? 0.0 : 1.0;
        //gl_PointSize =  ps;

/////////////////////////////////////////////////////////////
// on glColor ci sono le coordinate del punto precedente
// per il calcolo del colore in base alla velociita' di fuga
    //vec4 oldVtxPos = vec4(a_PrevPoint.xyz,1.f);
    //float vel = distance(vtxPos.xyz,oldVtxPos.xyz)*velIntensity;

    
    LightRay = normalize(vec4(u.lightDir, 1.f)) ;
    
    gl_Position = (m.pMatrix * vtxPos);
    particleColor = cOut;  
    
    
}                                                           

