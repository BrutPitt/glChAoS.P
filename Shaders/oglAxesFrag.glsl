//------------------------------------------------------------------------------
//  Copyright (c) 2018-2019 Michele Morrone
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

out vec4 color;
uniform vec3 zoomF;


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
vec3 light_position = vec3(10.0f, 10.0f, 5.0f);
float far = 10.0;
float near = .0001;

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
    
    // draw in [-1, 1] 
    gl_FragDepth = LinearizeDepth(vsPos.z);

    color = vsColor * ambientInt + vsColor * diffuse + vec4(specular * specularInt) /*- distAtten*/;

} 