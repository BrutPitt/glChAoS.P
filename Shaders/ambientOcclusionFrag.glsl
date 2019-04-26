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


in vec2 viewRay;
in vec2 vTexCoord;

uniform vec4 aoVals; // x = Bias, y = Radius, z = darkness


LAYUOT_BINDING(4) uniform _tMat {
    mat4 pMatrix;
    mat4 mvMatrix;
    mat4 mvpMatrix;
};

uniform vec3 ssaoSamples[64];


out vec4 outColor;

LAYUOT_BINDING(5) uniform sampler2D prevData;
LAYUOT_BINDING(6) uniform sampler2D noise;


vec4 fn(vec2 uv) {
    return texture(prevData,uv);
    //return getZ(texture(depthTexture,uv).r);
}


// note: entirely stolen from https://gist.github.com/TheRealMJP/c83b8c0f46b63f3a88a5986f4fa982b1
// Samples a texture with Catmull-Rom filtering, using 9 texture fetches instead of 16.
// See http://vec3.ca/bicubic-filtering-in-fewer-taps/ for more details
vec4 SampleTextureCatmullRom( vec2 uv)
{
    // We're going to sample a a 4x4 grid of texels surrounding the target UV coordinate. We'll do this by rounding
    // down the sample location to get the exact center of our "starting" texel. The starting texel will be at
    // location [1, 1] in the grid, where [0, 0] is the top left corner.
    vec2 samplePos = uv;
    vec2 texPos1 = floor(samplePos - 0.5) + 0.5;

    // Compute the fractional offset from our starting texel to our original sample location, which we'll
    // feed into the Catmull-Rom spline function to get our filter weights.
    vec2 f = samplePos - texPos1;

    // Compute the Catmull-Rom weights using the fractional offset that we calculated earlier.
    // These equations are pre-expanded based on our knowledge of where the texels will be located,
    // which lets us avoid having to evaluate a piece-wise function.
    vec2 w0 = f * ( -0.5 + f * (1.0 - 0.5*f));
    vec2 w1 = 1.0 + f * f * (-2.5 + 1.5*f);
    vec2 w2 = f * ( 0.5 + f * (2.0 - 1.5*f) );
    vec2 w3 = f * f * (-0.5 + 0.5 * f);
    
    // Work out weighting factors and sampling offsets that will let us use bilinear filtering to
    // simultaneously evaluate the middle 2 samples from the 4x4 grid.
    vec2 w12 = w1 + w2;
    vec2 offset12 = w2 / (w1 + w2);

    // Compute the final UV coordinates we'll use for sampling the texture
    vec2 texPos0 = texPos1 - vec2(1.0);
    vec2 texPos3 = texPos1 + vec2(2.0);
    vec2 texPos12 = texPos1 + offset12;

    texPos0 /= u.scrnRes;
    texPos3 /= u.scrnRes;
    texPos12 /= u.scrnRes;

    vec4 result = vec4(0.0);
    result += fn( vec2(texPos0.x,  texPos0.y )) * w0.x  * w0.y;
    result += fn( vec2(texPos12.x, texPos0.y )) * w12.x * w0.y;
    result += fn( vec2(texPos3.x,  texPos0.y )) * w3.x  * w0.y;

    result += fn( vec2(texPos0.x,  texPos12.y)) * w0.x  * w12.y;
    result += fn( vec2(texPos12.x, texPos12.y)) * w12.x * w12.y;
    result += fn( vec2(texPos3.x,  texPos12.y)) * w3.x  * w12.y;

    result += fn( vec2(texPos0.x,  texPos3.y )) * w0.x  * w3.y;
    result += fn( vec2(texPos12.x, texPos3.y )) * w12.x * w3.y;
    result += fn( vec2(texPos3.x,  texPos3.y )) * w3.x  * w3.y;

    return result;
}

float getBlurredZ(ivec2 uv)
{
    float result = 0.0;
    for (int x = -2; x <= 2; ++x) 
        for (int y = -2; y <= 2; ++y) 
            result += texelFetch(prevData,uv+ivec2(x,y), 0).w;
    return result * .04; // -> result / 25.;
}


void main()
{
    vec2 uv = gl_FragCoord.xy;
    float depth = texelFetch(prevData,ivec2(uv), 0).w;
    if(depth<=.01) { discard; outColor = vec4(0.0); return; }

    float z = getViewZ(depth);
    vec4 vtx = vec4(viewRay * z, z, 1.0);

    float zEye  = form_01_to_m1p1(depth);
    float gradA = form_01_to_m1p1(texelFetch(prevData,ivec2(uv + vec2( 1., 0.)), 0).w);
    float gradB = form_01_to_m1p1(texelFetch(prevData,ivec2(uv + vec2( 0., 1.)), 0).w);

    //float gradC = form_01_to_m1p1(texelFetch(prevData,ivec2(uv + vec2(-1., 0.)), 0).w);
    //float gradD = form_01_to_m1p1(texelFetch(prevData,ivec2(uv + vec2( 0.,-1.)), 0).w);
    //float gradE = form_01_to_m1p1(texelFetch(prevData,ivec2(uv + vec2( 1., 1.)), 0).w);
    //float gradF = form_01_to_m1p1(texelFetch(prevData,ivec2(uv + vec2(-1.,-1.)), 0).w);
    //float gradG = form_01_to_m1p1(texelFetch(prevData,ivec2(uv + vec2( 1.,-1.)), 0).w);
    //float gradH = form_01_to_m1p1(texelFetch(prevData,ivec2(uv + vec2(-1., 1.)), 0).w);

    vec2 m = (1. / u.scrnRes) * vec2(u.scrnRes.x/u.scrnRes.y * u.halfTanFOV, u.halfTanFOV);
    //vec2 m = (1. / vec2(1920.,1080.)) * vec2(1920./1080. * 0.288675, 0.288675);

    vec3 N0 = cross(vec3(vec2( 1., 0.)*m, (gradA-zEye)*z), vec3(vec2( 0., 1.)*m, (gradB-zEye)*z));
    //vec3 N1 = cross(vec3(vec2(-1., 0.)*m, (gradC-zEye)*z), vec3(vec2( 0.,-1.)*m, (gradD-zEye)*z));
    //vec3 N2 = cross(vec3(vec2( 1., 1.)*m, (gradE-zEye)*z), vec3(vec2(-1., 1.)*m, (gradH-zEye)*z));
    //vec3 N3 = cross(vec3(vec2(-1.,-1.)*m, (gradF-zEye)*z), vec3(vec2( 1.,-1.)*m, (gradG-zEye)*z));

            
    vec4 N;
    //N0    = normalize(dot(N0,N0)>dot(N1,N1) ? N0 : N1);
    //N2    = normalize(dot(N2,N2)>dot(N3,N3) ? N2 : N3);
    //N.xyz = normalize(dot(N0,N0)>dot(N2,N2) ? N0 : N2);
    //N.xyz = normalize(min(N0, min(N1, min(N2,N3))));
    N.xyz = normalize (N0);

    
    float AO = 0.0;
    const int RAD = 64;
    float bias = aoVals.x;
    float radius = aoVals.y;
    float darkness = aoVals.z;

    vec2 noiseScale = u.scrnRes * .25; // -> u.scrnRes/4.0
    vec3 randomVec = texture(noise, vTexCoord * noiseScale).xyz;
    vec3 tangent   = normalize(randomVec - N.xyz * dot(randomVec, N.xyz));
    vec3 bitangent = cross(N.xyz, tangent);
    mat3 TBN       = mat3(tangent, bitangent, N.xyz);

        for (int i = 0 ; i < RAD ; i++) {
            vec3 sampleP = TBN * ssaoSamples[i];
            sampleP = vtx.xyz + sampleP * radius; 
            vec4 offset = vec4(sampleP, 1.0);
            offset = pMatrix * offset;
            offset.xy /= -offset.w;
            offset.xy = offset.xy * 0.5 + 0.5;

            float sampleDepth = getViewZ(texture(prevData,offset.xy).w);
            //float sampleDepth = CalcViewZ(offset.xy);

            AO += (sampleDepth >= sampleP.z + bias ? 1.0-darkness : 0.0);
        }

    AO = 1.0 - AO/float(RAD);

    outColor = vec4(N.xyz*.5 + .5 , AO);
    //outColor = vec4(N.xyz*.5 + .5 , 1.0);
    //outColor = vec4( vec3(texelFetch(prevData,ivec2(uv), 0).w) , 1.0);
    //outColor = vec4(texelFetch(prevData,ivec2(uv               ) , 0).xyz , 1.0);
}
