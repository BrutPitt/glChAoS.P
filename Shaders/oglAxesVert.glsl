//------------------------------------------------------------------------------
//  Copyright (c) 2018-2024 Michele Morrone
//  All rights reserved.
//
//  https://michelemorrone.eu - https://brutpitt.com
//
//  X: https://x.com/BrutPitt - GitHub: https://github.com/BrutPitt
//
//  direct mail: brutpitt(at)gmail.com - me(at)michelemorrone.eu
//
//  This software is distributed under the terms of the BSD 2-Clause license
//------------------------------------------------------------------------------
#line 14
layout (location = 1) in vec3 vPos;
layout (location = 2) in vec3 normal;

// I cant use direct location... for EMS and webgl
//layout (location = 4) uniform mat4 pMat;
//layout (location = 5) uniform mat4 mvMat;
//layout (location = 6) uniform mat4 zoomF;
uniform mat4 pMat;
uniform mat4 mvMat;
uniform vec3 zoomF;

// need for use pipeline 
#ifdef GLAPP_USE_PIPELINE
out gl_PerVertex
{
	vec4 gl_Position;
};
#endif

// I cant use blocks... for EMS and webgl

#ifdef GL_ES
out vec4 vsColor;
out vec3 vsNormal;
out vec3 vsWorldPos;
out vec3 vsPos;
#else
out vsOut {
    vec4 vsColor;
    vec3 vsNormal;
    vec3 vsWorldPos;
    vec3 vsPos;
};
#endif

const vec4 blue  = vec4(0.10, 0.10, 0.70, 1.0);
const vec4 green = vec4(0.10, 0.70, 0.10, 1.0);
const vec4 red   = vec4(0.70, 0.10, 0.10, 1.0);


//fast 90' rotation around fixzed axis
vec3 rotOnX(vec3 v) { return vec3(v.x, v.z, -v.y); }
vec3 rotOnY(vec3 v) { return vec3(v.z, v.y, -v.x); }

void main(void)
{
    
    vec3 newPos, newNorm;
    vec3 pos = vPos * zoomF;
    if(gl_InstanceID==1)      { newPos = rotOnX(pos); newNorm = rotOnX(normal); vsColor = green; }
    else if(gl_InstanceID==2) { newPos = rotOnY(pos); newNorm = rotOnY(normal); vsColor = red;   }
    else                      { newPos = pos;         newNorm = normal;         vsColor = blue;  }

    vec4 position = pMat * mvMat * vec4(newPos, 1.0f);
    vsPos = normalize(mat3(mvMat) * newPos);
    vsWorldPos =  position.xyz; //(mvMat * vec4(newPos,1.0)).xyz ;
    vsNormal = normalize(mat3(mvMat) * newNorm);
    
    gl_Position = position;
    //gl_Position = 
}