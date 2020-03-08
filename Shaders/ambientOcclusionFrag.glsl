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


in vec2 viewRay;
in vec2 vTexCoord;

out vec4 outColor;

//LAYOUT_BINDING(5) uniform sampler2D prevData;
LAYOUT_BINDING(6) uniform sampler2D noise;
LAYOUT_BINDING(7) uniform sampler2D ssaoSample;
LAYOUT_BINDING(10) uniform sampler2D zTex;
//LAYOUT_BINDING(7) uniform sampler2D shadowTex;

/*
vec4 fn(vec2 uv) {
    return vec4(getViewZ(texture(shadowTex, uv).r));
    //return getZ(texture(depthTexture,uv).r);
}

float f(vec2 uv, uint idx) {
    return getViewZ(texture(shadowTex, uv).r);
}


float bicubic (vec2 pt, uint idx) {
    vec3 dp = vec3(1., 0., -1.);
    vec2 p1 = floor(pt);
    vec2 p2 = p1 + dp.xx;
    vec4 p  = vec4(p1, p2);

    vec4 v0 = vec4( f(p.xy, idx), f(p.xw, idx), f(p.zy, idx), f(p.zw, idx));
    vec4 vX = vec4( f(p.xy + dp.xy, idx) - f(p.xy + dp.zy, idx),
                    f(p.xw + dp.xy, idx) - f(p.xw + dp.zy, idx),
                    f(p.zy + dp.xy, idx) - f(p.zy + dp.zy, idx),
                    f(p.zw + dp.xy, idx) - f(p.zw + dp.zy, idx)) * .5;
    vec4 vY = vec4( f(p.xy + dp.yx, idx) - f(p.xy + dp.yz, idx),
                    f(p.xw + dp.yx, idx) - f(p.xw + dp.yz, idx),
                    f(p.zy + dp.yx, idx) - f(p.zy + dp.yz, idx),
                    f(p.zw + dp.yx, idx) - f(p.zw + dp.yz, idx)) * .5;
    vec4 vXY = vec4(f(p.xy + dp.xx, idx) - f(p.xy + dp.xz, idx) - f(p.xy + dp.zx, idx) + f(p.xy + dp.zz, idx),
                    f(p.xw + dp.xx, idx) - f(p.xw + dp.xz, idx) - f(p.xw + dp.zx, idx) + f(p.xw + dp.zz, idx),
                    f(p.zy + dp.xx, idx) - f(p.zy + dp.xz, idx) - f(p.zy + dp.zx, idx) + f(p.zy + dp.zz, idx),
                    f(p.zw + dp.xx, idx) - f(p.zw + dp.xz, idx) - f(p.zw + dp.zx, idx) + f(p.zw + dp.zz, idx)) * .25;
    mat4 Q = mat4(vec4(v0.xz, vX.xz), vec4(v0.yw, vX.yw), vec4(vY.xz, vXY.xz), vec4(vY.yw, vXY.yw));
    mat4 S = mat4(1., 0., 0., 0., 0., 0., 1., 0., -3., 3., -2., -1., 2., -2., 1., 1.);
    mat4 T = mat4(1., 0., -3., 2., 0., 0., 3., -2., 0., 1., -2., 1., 0., 0., -1., 1.);
    mat4 A = T * Q * S;
    float t = fract(pt.x);
    float u = fract(pt.y);
    vec4 tv = vec4(1., t, t * t, t * t * t);
    vec4 uv = vec4(1., u, u * u, u * u * u);
    return dot(tv * A, uv);
}


// note: entirely stolen from https://gist.github.com/TheRealMJP/c83b8c0f46b63f3a88a5986f4fa982b1
// Samples a texture with Catmull-Rom filtering, using 9 texture fetches instead of 16.
// See http://vec3.ca/bicubic-filtering-in-fewer-taps/ for more details
vec4 SampleTextureCatmullRom( vec2 uv)
{
    // We're going to sample a a 4x4 grid of texels surrounding the target UV coordinate. We'll do this by rounding
    // down the sample location to get the exact center of our "starting" texel. The starting texel will be at
    // location [1, 1] in the grid, where [0, 0] is the top left corner.
    vec2 samplePos = uv * u.scrnRes;
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


float buildSmoothShadowB(vec4 frag)
{
    vec4 pt = vec4(viewRay*-frag.z-u.POV.xy, frag.z-u.POV.z, 1.0);

    mat4 tMat = m.pMatrix * m.mvLightM;
           
    vec4 fragPosLightSpace = tMat * pt;
    // perform perspective divide and port to [0,1] range
    vec3 projCoords = 0.5 * (fragPosLightSpace.xyz)/fragPosLightSpace.w + .5;

    if(projCoords.z>1.0) return 0.0;
    // get depth of current fragment from light's perspective    
    float closestDepth = getViewZ(texture(shadowTex, projCoords.xy).r);
    float currentDepth = getViewZ(projCoords.z) + u.shadowBias;

    vec2 stepTex = u.shadowGranularity*u.invScrnRes;

    float shadow = 0.0;
    int radius = int(u.shadowSmoothRadius);
    float diam = float(radius) * 2. + 1.0;
    float invDiv = 1.0/(diam * diam);
    
    for (int x = -radius; x <= radius ; x++) 
        for (int y = -radius; y <= radius; y++) {
            float closestDepth = getViewZ(texture(shadowTex, projCoords.xy+vec2(x,y)*stepTex).r);
            shadow += currentDepth < closestDepth  ?  0.0 : invDiv ;
        }

    return shadow;
}  
*/
float getBlurredZ(ivec2 uv)
{
    float result = 0.0;
    for (int x = -2; x <= 2; ++x) 
        for (int y = -2; y <= 2; ++y) 
            result += texelFetch(zTex,uv+ivec2(x,y), 0).x;
    return result * .04; // -> result / 25.;
}


void main()
{
    float depth = texture(zTex,gl_FragCoord.xy*u.invScrnRes).x;
    if(depth>.9999) { discard; outColor = vec4(0.0); return; }

    float z = restoreZ(depth);
    vec4 vtx = getVertexFromDepth(viewRay,z);

    vec3 N = getSimpleNormal(vtx, zTex);
    
    float AO = 0.0;
    const int RAD = 64;
    float radius = u.aoRadius;

    vec2 noiseScale = u.scrnRes * .25; // -> u.scrnRes/4.0
    vec3 randomVec = texture(noise, vTexCoord * noiseScale).xyz * 2.0 - 1.0;
    vec3 tangent   = normalize(randomVec - N * dot(randomVec, N));
    vec3 bitangent = cross(N, tangent);
    mat3 TBN       = mat3(tangent, bitangent, N);

        for (int i = 0 ; i < RAD ; i++) {
            vec3 sampleP = TBN * texelFetch(ssaoSample, ivec2(i,0), 0).xyz;  // ssaoSamples[i];
            //vec3 sampleP = TBN * ssaoSamples[i];
            sampleP = vtx.xyz + sampleP * radius; 
            vec4 offset = vec4(sampleP, 1.0);
            offset = m.pMatrix * offset;
            offset.xy /= -offset.w;
            offset.xy = offset.xy * 0.5 + 0.5;

            float sampleDepth = restoreZ(texture(zTex,offset.xy).x);
            //float sampleDepth = CalcViewZ(offset.xy);

            AO += sampleDepth >= sampleP.z + u.aoBias ? (1.0-u.aoDarkness) * u.aoMul : 0.0;
        }

    AO = u.aoModulate -  clamp(AO/float(RAD), 0.0, 1.0);

    //float shadow = bool(u.pass &  RENDER_SHADOW) ? buildSmoothShadow(vtx): 1.0;

    outColor = vec4(vec3(1.0)/*N*.5 + .5 */, AO);
    //outColor = vec4(N.xyz*.5 + .5 , 1.0);
    //outColor = vec4( vec3(texelFetch(prevData,ivec2(uv), 0).w) , 1.0);
    //outColor = vec4(texelFetch(prevData,ivec2(uv               ) , 0).xyz , 1.0);
}
