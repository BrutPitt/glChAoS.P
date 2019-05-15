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


out vec4 outColor;

LAYUOT_BINDING(5) uniform sampler2D prevData;
LAYUOT_BINDING(6) uniform sampler2D aoTex;

float f(vec2 uv) {
    return form_01_to_m1p1(texelFetch(prevData,ivec2(uv), 0).w) ;
    //return getZ(texture(depthTexture,uv).r);
}
float f(vec2 uv, uint idx) {
    return texelFetch(prevData,ivec2(uv), 0)[idx] ;
    //return getZ(texture(depthTexture,uv).r);
}

vec4 fn(vec2 uv) {
    return texture(prevData,uv);
    //return getZ(texture(depthTexture,uv).r);
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
    float t = fract(p.x);
    float u = fract(p.y);
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
    result += fn( vec2(texPos0.x,  texPos0.y)) * w0.x * w0.y;
    result += fn( vec2(texPos12.x, texPos0.y)) * w12.x * w0.y;
    result += fn( vec2(texPos3.x,  texPos0.y)) * w3.x * w0.y;

    result += fn( vec2(texPos0.x,  texPos12.y)) * w0.x * w12.y;
    result += fn( vec2(texPos12.x, texPos12.y)) * w12.x * w12.y;
    result += fn( vec2(texPos3.x,  texPos12.y)) * w3.x * w12.y;

    result += fn( vec2(texPos0.x,  texPos3.y)) * w0.x * w3.y;
    result += fn( vec2(texPos12.x, texPos3.y)) * w12.x * w3.y;
    result += fn( vec2(texPos3.x,  texPos3.y)) * w3.x * w3.y;

    return result;
}


vec4 pixelColorLight(vec3 vtx, vec4 color, vec4 N, float AO)
{
//    if (color.a < u.alphaSkip) discard;   // kill pixels outside circle: r=1.0
    //else {
        //vec3 N;
        //N.xy = (coord - vec2(.5))*2.0; // xy = radius in 0
        //float mag = dot(N.xy, N.xy);
        //N.z = sqrt(1.0-mag);
    
        //N.xyz = normalize(N.xyz); 

        //Light @ vertex position
        vec3 light =  normalize(u.lightDir);  // 
    
        float lambertian = max(0.0, dot(light, N.xyz)); 

        vec3 V = normalize(vtx);
#if defined(GL_ES) || defined(__APPLE__)
        float specular = u.lightModel == uint(idxPHONG) ? specularPhong(V, light, N.xyz) : (u.lightModel == uint(idxBLINPHONG) ? specularBlinnPhong(V, light, N.xyz) : specularGGX(V, light, N.xyz));
#else
        float specular = lightModel(V, light, N.xyz);
#endif

        float aoD = sqrt(AO);
        vec3 lColor =  smoothstep(u.sstepColorMin, u.sstepColorMax,
                                    aoD*color.rgb * u.lightColor * lambertian * u.lightDiffInt +  //diffuse component
                                    u.lightColor * specular * u.lightSpecInt  + 
                                    AO*(color.rgb*u.lightAmbInt + vec3(u.lightAmbInt)) * .5 
                                 );

        return vec4(lColor.rgb , 1.0);
    //}
}


float getBlurredAO(ivec2 uv)
{
    float result = 0.0;
    for (int x = -2; x <= 2; ++x) 
        for (int y = -2; y <= 2; ++y) 
            result += texelFetch(aoTex,uv+ivec2(x,y), 0).w;
    return result * .04; // -> result / 25.;
}

vec3 blurredNormals(vec2 uv)
{
const float NR = 1.;

vec3 sampleCoord[9];
     sampleCoord[0] = vec3( 0.,  0., 1.);
     sampleCoord[1] = vec3(  0,  NR, .3);
     sampleCoord[2] = vec3(  0, -NR, .3);
     sampleCoord[3] = vec3( NR,   0, .3);
     sampleCoord[4] = vec3(-NR,   0, .3);
     sampleCoord[5] = vec3( NR, -NR, .3);
     sampleCoord[6] = vec3(-NR,  NR, .3);
     sampleCoord[7] = vec3( NR,  NR, .3);
     sampleCoord[8] = vec3(-NR, -NR, .3);


    const float colorDiv = .454545; //.2381; // 1/
    vec3 N = vec3(0.0);

    for (int i = 0; i < 5; i++) 
        N += texture(aoTex, (uv + vec2(sampleCoord[i].xy)) / u.scrnRes).xyz * sampleCoord[i].z;

    return normalize(2.0 * N * colorDiv - 1.0);
}


void main()
{

    //if(N.w > 1.0) discard;
    //if(u.lightActive==1) {
        vec2 uv = gl_FragCoord.xy;
        //float test = getZ(texelFetch(prevData,ivec2(uv), 0).x);
        //if(test>.5) {
//        if(zEye>=1.0) discard;

        float depth = texelFetch(prevData,ivec2(uv), 0).w;
        if(depth<=.01) { discard; outColor = vec4(0.0); return; }

        //float AO = bicubic(uv, 3); //texelFetch(aoTex,ivec2(uv), 0).w;
        float AO = getBlurredAO(ivec2(uv));
    if(u.pass >= 2) {

        //float zD = form_01_to_m1p1(depth);
        float z = getViewZ(depth);
        vec4 vtx = vec4(viewRay * z, z, 1.0);

        //vec3 N = blurredNormals(uv).xyz;
        //vec3 N = 2.0 * texelFetch(aoTex,ivec2(uv), 0).xyz - 1.0;
        vec3 N = getSelectedNormal(z, prevData);
        //vec3 N = getSimpleNormal(z, prevData);

        vec4 color = texelFetch(prevData,ivec2(uv), 0);

        
        outColor = pixelColorLight(vtx.xyz, color, vec4(N, 1.0), u.pass==3 ? AO : 1.0);

    } else {
        AO = max(.05,AO);
        float newAO = 1.-AO;
        outColor = vec4(texelFetch(prevData,ivec2(uv), 0).rgb* sqrt(AO) - 
                        smoothstep(u.sstepColorMin, u.sstepColorMax, vec3(u.lightAmbInt)* .66 * newAO * newAO), 
                        1.0);

    }

}