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
#line 37 // #version dynamically inserted

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
LAYUOT_BINDING(0) uniform sampler2D u_colorTexture;
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
	vec3 rgbNW = textureOffset(u_colorTexture, vTexCoord, ivec2(-1, 1)).rgb;
    vec3 rgbNE = textureOffset(u_colorTexture, vTexCoord, ivec2(1, 1)).rgb;
    vec3 rgbSW = textureOffset(u_colorTexture, vTexCoord, ivec2(-1, -1)).rgb;
    vec3 rgbSE = textureOffset(u_colorTexture, vTexCoord, ivec2(1, -1)).rgb;

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
	if (lumaMax - lumaMin < lumaMax * fxaaData.x)
	{
		// ... do no AA and return.
		fragColor = vec4(rgbM, 1.0);
        //fragColor.rgb = vec3(1.0, 0.0, 0.0);    
		
		return;
	}  
	
	// Sampling is done along the gradient.
	vec2 samplingDirection;	
	samplingDirection.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    samplingDirection.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));
    
    // Sampling step distance depends on the luma: The brighter the sampled texels, the smaller the final sampling step direction.
    // This results, that brighter areas are less blurred/more sharper than dark areas.  
    float samplingDirectionReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.25 * fxaaData.y, fxaaData.z);

	// Factor for norming the sampling direction plus adding the brightness influence. 
	float minSamplingDirectionFactor = 1.0 / (min(abs(samplingDirection.x), abs(samplingDirection.y)) + samplingDirectionReduce);
    
    // Calculate final sampling direction vector by reducing, clamping to a range and finally adapting to the texture size. 
    samplingDirection = clamp(samplingDirection * minSamplingDirectionFactor, vec2(-fxaaData.w, -fxaaData.w), vec2(fxaaData.w, fxaaData.w)) * invScrnSize;
	
	// Inner samples on the tab.
	vec3 rgbSampleNeg = texture(u_colorTexture, vTexCoord + samplingDirection * (1.0/3.0 - 0.5)).rgb;
	vec3 rgbSamplePos = texture(u_colorTexture, vTexCoord + samplingDirection * (2.0/3.0 - 0.5)).rgb;

	vec3 rgbTwoTab = (rgbSamplePos + rgbSampleNeg) * 0.5;  

	// Outer samples on the tab.
	vec3 rgbSampleNegOuter = texture(u_colorTexture, vTexCoord + samplingDirection * (0.0/3.0 - 0.5)).rgb;
	vec3 rgbSamplePosOuter = texture(u_colorTexture, vTexCoord + samplingDirection * (3.0/3.0 - 0.5)).rgb;
	
	vec3 rgbFourTab = (rgbSamplePosOuter + rgbSampleNegOuter) * 0.25 + rgbTwoTab * 0.5;   
	
	// Calculate luma for checking against the minimum and maximum value.
	float lumaFourTab = dot(rgbFourTab, toLuma);
	
	// Are outer samples of the tab beyond the edge ... 
	if (lumaFourTab < lumaMin || lumaFourTab > lumaMax)
	{
		// ... yes, so use only two samples.
		fragColor = vec4(rgbTwoTab, 1.0); 
	}
	else
	{
		// ... no, so use four samples. 
		fragColor = vec4(rgbFourTab, 1.0);
	}

    //fragColor.r = 1.0;    
}

