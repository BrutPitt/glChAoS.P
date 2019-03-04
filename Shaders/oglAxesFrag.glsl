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
#line 37    //#version dynamically inserted

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