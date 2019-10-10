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

////////////////////////////////////////////////////////////////////////////////
//  
//  Modified code from original source of Norbert Nopper:
//  https://github.com/McNopper/OpenGL/blob/master/Example42/shader/fxaa.frag.glsl
//  
////////////////////////////////////////////////////////////////////////////////


// #version dynamically inserted
#ifdef GL_ES
uniform sampler2D u_colorTexture;
#else
LAYOUT_BINDING(0) uniform sampler2D u_colorTexture;
#endif

uniform vec2 invScrnSize; 
uniform vec4 fxaaData;

out vec4 fragColor;
in vec2 vTexCoord;

// see FXAA
// http://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf
// http://iryoku.com/aacourse/downloads/09-FXAA-3.11-in-15-Slides.pdf
// http://horde3d.org/wiki/index.php5?title=Shading_Technique_-_FXAA

void main(void)
{       
    vec3 rgbM = texture(u_colorTexture, vTexCoord).rgb;

    // Sampling neighbour texels. Offsets are adapted to OpenGL texture coordinates. 
    vec3 rgbNW = textureOffset(u_colorTexture, vTexCoord, ivec2(-1,  1)).rgb;
    vec3 rgbNE = textureOffset(u_colorTexture, vTexCoord, ivec2( 1,  1)).rgb;
    vec3 rgbSW = textureOffset(u_colorTexture, vTexCoord, ivec2(-1, -1)).rgb;
    vec3 rgbSE = textureOffset(u_colorTexture, vTexCoord, ivec2( 1, -1)).rgb;

    //luminance
    const vec3 toLuma = vec3(0.299, 0.587, 0.114);

    // Convert from RGB to luma.
    float lumaNW = dot(rgbNW, toLuma);
    float lumaNE = dot(rgbNE, toLuma);
    float lumaSW = dot(rgbSW, toLuma);
    float lumaSE = dot(rgbSE, toLuma);
    float lumaM = dot(rgbM, toLuma);

    // Gather minimum and maximum luma.
    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    // If contrast is lower than a maximum threshold ...
    if (lumaMax - lumaMin < lumaMax * fxaaData.x) { fragColor = vec4(rgbM, 1.0); return; }

    // Sampling is done along the gradient.
    vec2 dir = vec2(-((lumaNW + lumaNE) - (lumaSW + lumaSE)), 
                     ((lumaNW + lumaSW) - (lumaNE + lumaSE)));
    
    // Sampling step distance depends on the luma: The brighter the sampled texels, the smaller the final sampling step direction.
    // This results, that brighter areas are less blurred/more sharper than dark areas.  
    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.25 * fxaaData.y, fxaaData.z);

    // Factor for norming the sampling direction plus adding the brightness influence. 
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    
    // Calculate final sampling direction vector by reducing, clamping to a range and finally adapting to the texture size. 
    dir = min(vec2(fxaaData.w, fxaaData.w), max(vec2(-fxaaData.w, -fxaaData.w), dir * rcpDirMin)) * invScrnSize;

    vec3 rgbA = 0.5 * (
        texture(u_colorTexture, vTexCoord + dir * (1.0/3.0 - 0.5)).rgb +
        texture(u_colorTexture, vTexCoord + dir * (2.0/3.0 - 0.5)).rgb);    
    vec3 rgbB = rgbA * 0.5 + 0.25 * (
        texture(u_colorTexture, vTexCoord + dir * (0.0/3.0 - 0.5)).rgb +
        texture(u_colorTexture, vTexCoord + dir * (3.0/3.0 - 0.5)).rgb);

    // Calculate luma for checking against the minimum and maximum value.
    float lumaB = dot(rgbB, toLuma);

    // Are outer samples of the tab beyond the edge ... 
    fragColor = vec4(lumaB  < lumaMin || lumaB  > lumaMax ? rgbA : rgbB, 1.0); 
}