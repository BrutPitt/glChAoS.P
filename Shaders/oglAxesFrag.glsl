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

out vec4 color;
uniform vec3 zoomF;
uniform vec3 light_position;


#ifdef GL_ES
in vec4 vsColor;
in vec3 vsNormal;
in vec3 vsWorldPos;
in vec3 vsPos;
#else
in vsOut {
    vec4 vsColor;
    vec3 vsNormal;
    vec3 vsWorldPos;
    vec3 vsPos;
};
#endif

const float ambientInt   = 0.3;
const float specularInt  = 0.5; // vec4(1.0, 1.0, 1.0, 1.0);\n"
const float shininessExp = 10.f;
const float near = .001;
const float far = 10.0;


float LinearizeDepth(float depth) 
{
    float z = -depth * 2.0 - (far - near);                          // back to NDC -depth/(far - near) * 2.0 - 1.0
    return (2.0 * near * far) / ((far + near) - z * (far - near));	
}

void main()
{
 
    vec3 lightDir = normalize(light_position + vsWorldPos);
    vec3 halfVec = normalize(lightDir + normalize(vsWorldPos));
    float diffuse = max(0.0, dot(vsNormal, lightDir));
    float specular = pow(max(0.0, dot(vsNormal, halfVec)), shininessExp);
    //gl_FragDepth = .z;
    
    // draw in zNDC [-1, 1] 
    float depth = LinearizeDepth(vsPos.z);

    color = vsColor * ambientInt + vsColor * diffuse + vec4(specular * specularInt) /*- distAtten*/;

    gl_FragDepth = depth; 
} 