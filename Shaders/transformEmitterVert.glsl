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

layout(std140) uniform;

layout (location = 0) in vec4 posIn;
layout (location = 1) in vec4 velTIn;

out vec4 posOut;
out vec4 velTOut;

LAYOUT_BINDING(2) uniform _TFData {
    vec4 wind;
    vec4 gravity;
    float airFriction;
    float diffTime;
    float elapsedTime;
} u;

#ifndef GL_ES
out gl_PerVertex
{
    vec4 gl_Position;
};
#endif


void main(void)
{
    vec4 pos = posIn;
    vec3 vel = velTIn.xyz;
    float time = velTIn.w;
    vec4 gravPos = vec4(0.0);
  

    if(time<0.0) {
        time = -time;
        pos.xyz += vel * u.diffTime;
    } else {
        float dTime = u.elapsedTime-time;

        vel -= vel*u.airFriction*u.diffTime;

        //float drag = u.airFriction*dTime;

        float lifeW = dTime-u.wind.w;
        if(lifeW>0.0)
            pos.xyz += u.wind.xyz*u.diffTime;

        float lifeG = dTime-u.gravity.w;
        if(lifeG>0.0) 
             vel += u.gravity.xyz*u.diffTime;

        pos.xyz += vel * u.diffTime;
    }

    //if(u.elapsedTime-time>10.0) { vel+=vec3(.1); }
    
    gl_Position = pos;
    posOut = pos;
    velTOut = vec4(vel, time);

}
