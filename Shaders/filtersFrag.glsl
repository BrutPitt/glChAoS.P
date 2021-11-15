////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2018-2020 Michele Morrone
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
#line 17    //#version dynamically inserted - #include ColorSpaces.glsl

///////////////////////////////////////
//
// Add file: "ColorSpaces.glsl" 
//
// Use ShaderObject::Load(char *defines, int nArgs, ...); in wrap class to include more files
//
#define SQRT_OF_2PI 2.506628274631000502415765284811
#define INV_SQRT_OF_2PI 0.39894228040143267793994605993439  // 1.0/SQRT_OF_2PI
#define COS_PI_4 .70710678118654752440084436210485
#define M_PI 3.1415926535897932384626433832795
#define INV_PI 0.31830988618379067153776752674503

///////////////////////////////////////////////////////////////////////
// Multipass Glow
///////////////////////////////////////////////////////////////////////
layout(std140) uniform;

#define idxTUNING          0
#define idxRADIAL_P1       1
#define idxRADIAL_P2       2
#define idxRADIAL_P2_BILAT 3
#define idxBILATERAL       4
#define idxFXAA            5
#define idxBLIT            6
#define idxMIXTEX          7
#define idxPIP             8

#if !defined(GL_ES) && !defined(GLCHAOSP_NO_USES_GLSL_SUBS)
    subroutine vec4 filterSub();
    subroutine uniform filterSub imageResult;
#endif


LAYOUT_BINDING(2) uniform _filterData {
    vec4 texControls;
    vec4 videoControls; //videoControls vec4 ->  1.f/m_gamma, m_exposure, m_bright, m_contrast
    vec4 fxaaData;
    vec4 pipViewport;
    vec2 invScreenSize;
    vec2 toneMapVals; // tonemap -> col = A * pow(col, G); -> x = A and y = G

    float sigmaSize;
    float sigmaRange;
    float threshold;

    float mixTexture;
    float mixBrurGlow;

    int blurCallType;
    bool toneMap;
};

LAYOUT_BINDING(0) uniform sampler2D origTexture;
LAYOUT_BINDING(1) uniform sampler2D auxTexture;

out vec4 outColor;
in vec2 vTexCoord;


CONST vec2 aspect = vec2(1.0, 1.0);


#define MSIZE 40


//  fX = exp( -(x*x) * invSigmaSqx2 ) * invSigmaxSqrt2PI; 
//  fY = exp( -(y*y) * invSigmaSqx2 ) * invSigmaxSqrt2PI; 
//  where...
//      invSigmaSqx2     = 1.0 / (sigma^2 * 2.0)
//      invSigmaxSqrt2PI = 1.0 / (sqrt(2 * PI) * sigma)
//
//  now, fX*fY can be written in unique expression...
//
//      e^(a*X) * e^(a*Y) * c*c
//
//      where:
//        a = invSigmaSqx2, X = (x*x), Y = (y*y), c = invSigmaxSqrt2PI
//
//           -[(x*x) * 1/(2 * sigma^2)]             -[(y*y) * 1/(2 * sigma^2)] 
//          e                                      e
//  fX = -------------------------------    fY = -------------------------------
//                ________                               ________
//              \/ 2 * PI  * sigma                     \/ 2 * PI  * sigma
//
//      now with... 
//        a = 1/(2 * sigma^2), 
//        X = (x*x) 
//        Y = (y*y) ________
//        c = 1 / \/ 2 * PI  * sigma
//
//      we have...
//              -[aX]              -[aY]
//        fX = e      * c;   fY = e      * c;
//
//      and...
//                 -[aX + aY]    [2]     -[a(X + Y)]    [2]
//        fX*fY = e           * c     = e            * c   
//
//      well...
//
//                    -[(x*x + y*y) * 1/(2 * sigma^2)]
//                   e                                
//        fX*fY = --------------------------------------
//                                        [2]           
//                          2 * PI * sigma           
//      
//      now with assigned constants...
//
//          invSigmaQx2   = 1/(2 * sigma^2)
//          invSigmaQx2PI = 1/(2 * PI * sigma^2) = invSigmaQx2 * INV_PI 
//
//      and the kernel vector 
//
//          k = vec2(x,y)
//
//      we can write:
//
//          fXY = exp( -dot(k,k) * invSigmaQx2) * invSigmaQx2PI

vec4 bilateralSmartSmooth(float reductFactor)
{
    float bsigma = threshold*reductFactor;
    float reductSigma = sigmaSize * reductFactor;
    float radius = float(round(sigmaRange*reductSigma-1.f));
    float radQ = radius * radius;
    
    float invSigma = 1.f/reductSigma;
    float invSigmaQx2 = .5 * invSigma * invSigma;          // 1.0 / (sigma^2 * 2.0)
    float invSigmaQx2PI = INV_PI * invSigmaQx2;    // 1.0 / (sqrt(PI) * sigma)
    
    float invBSigma = 1.f/bsigma;
    float invBSigmaSqx2 = .5 * invBSigma * invBSigma;          // 1.0 / (sigma^2 * 2.0)
    float invBSigmaxSqrt2PI = INV_SQRT_OF_2PI * invBSigma;    // 1.0 / (sqrt(2*PI) * sigma)
    
#ifdef USE_TEXEL_FETCH
    vec4 centrPx = texelFetch(origTexture,ivec2(gl_FragCoord.xy),0);    
#else    
    vec4 centrPx = texture(origTexture,vTexCoord);
#endif
    
    float Zbuff = 0.0;
    vec4 accumBuff = vec4(0.0);
    vec4 glowBuff  = vec4(0.0);
    
    vec2 d;
    for(d.x=-radius; d.x <= radius; d.x++)	{
        float pt = sqrt(radQ-d.x*d.x);
        for(d.y=-pt; d.y <= pt; d.y++) {
            float blurFactor = exp( -dot(d , d) * invSigmaQx2 ) * invSigmaQx2PI;
            
#ifdef USE_TEXEL_FETCH
            vec4 walkPx =  texelFetch(origTexture, ivec2(gl_FragCoord.xy+d),0 );
#else
            vec4 walkPx =  texture(origTexture, vTexCoord+d*invScreenSize);
#endif
            vec4 dC = walkPx-centrPx;
            float deltaFactor = exp( -dot(dC.rgb, dC.rgb) * invBSigmaSqx2) * invBSigmaxSqrt2PI * blurFactor;
            //float l = luminance(walkPx.rgb) - luminance(centrPx.rgb);
            //float deltaFactor = exp( -(l*l) * invBSigmaSqx2) * invBSigmaxSqrt2PI * blurFactor;

            Zbuff     += deltaFactor;
            accumBuff += deltaFactor*walkPx;
            //glowBuff  += deltaFactor*vec4(walkPx.rgb*walkPx.a, walkPx.a);
        }
    }
    //return mix(accumBuff, glowBuff, mixBrurGlow) /Zbuff;
    return accumBuff/Zbuff;
}


/*

            vec4 walkPx =  texture(tex,uv+d/size);
            vec4 dC;
            float deltaFactor, l;
            switch(pass) {
                case 1:
                     dC = walkPx-centrPx;
                     deltaFactor = exp( -dot(dC, dC) * invThresholdSqx2) * invThresholdSqrt2PI * blurFactor;
                break;
                case 2:
                    l = linearLum(walkPx.rgb) - linearLum(centrPx.rgb);
                    deltaFactor = exp( -(l*l) * invThresholdSqx2) * invThresholdSqrt2PI * blurFactor;
                break;
                case 3:
                    l = luminance(walkPx.rgb) - luminance(centrPx.rgb);
                    deltaFactor = exp( -(l*l) * invThresholdSqx2) * invThresholdSqrt2PI * blurFactor;
                break;
                case 4:
                     dC = walkPx-centrPx;
                     l = linearLum(dC.rgb);
                     deltaFactor = exp( -(l*l) * invThresholdSqx2) * invThresholdSqrt2PI * blurFactor;
                break;
                case 5:
                     dC = walkPx-centrPx;
                     l = luminance(dC.rgb);
                     deltaFactor = exp( -(l*l) * invThresholdSqx2) * invThresholdSqrt2PI * blurFactor;
                break;
            }



*/


vec4 bilateralSmartSmoothOK()
{
    return bilateralSmartSmooth(1.0);
}

vec4 gPass(sampler2D tex, vec2 direction) 
{
    //vec2 offset = wSize; 
    
    //compute the radius across the kernel
    float radius = sigmaRange*sigmaSize-1.f;
    
    float Zbuff = 0.0;
    vec4 accumBuffGauss = vec4(0.0);
    vec4 accumBuffGlow  = vec4(0.0);
    
    //precompute factors used every iteration
    float invSigma = 1.f/sigmaSize;
    float invSigmaSqx2 = .5 * invSigma * invSigma;          // 1.0 / (sigma^2 * 2.0)
    float invSigmaxSqrt2PI = INV_SQRT_OF_2PI * invSigma;    // 1.0 / (sqrt(PI) * sigma)
    
    // separable Gaussian
    for( float r = -radius; r <= radius; r++) {
        float factor = exp( -(r*r) * invSigmaSqx2 ) * invSigmaxSqrt2PI;
#ifdef USE_TEXEL_FETCH
        vec4 c = texelFetch(tex, ivec2(gl_FragCoord.xy + r * direction), 0);
#else
        vec4 c = texture(tex,  vTexCoord + r * direction * invScreenSize);
#endif
        accumBuffGauss += factor * c;
        accumBuffGlow  += factor * vec4((c.rgb*c.a), c.a);
    }
    
    return mix(accumBuffGauss, accumBuffGlow, mixBrurGlow);
}


vec4 qualitySetting(vec4 col)
{
    // Gamma
    col.rgb = pow(col.rgb, vec3(videoControls.x));  //approx gamma -> 1/gamma

    //Exposure
    col.rgb = saturate(vec3(1.f) - exp(-col.rgb * videoControls.y)); //

    //ToneMapping
    if(toneMap) col.rgb = toneMapping(col.rgb, toneMapVals.x, toneMapVals.y);


    //Brightness/Contrast 
    col.xyz = rgb2hsl(col.rgb);

    col.yz = contrastHSL( col.yz, videoControls.w);
    col.z  = brightnessHSL(col.z, videoControls.z);

    col.yz = clamp(col.yz,vec2(0.0), vec2(1.0));
    col.rgb = hsl2rgb(vec3(col.xyz));
        
    if(videoControls.w>0.0) col.rgb = contrastRGB(col.rgb, videoControls.w + (1.0+epsilon));
        //if(videoControls.w>0.0) col.rgb = contrast2(col.rgb, (videoControls.w ));

    return col;
}

// about FXAA
// http://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf
// http://iryoku.com/aacourse/downloads/09-FXAA-3.11-in-15-Slides.pdf
// http://horde3d.org/wiki/index.php5?title=Shading_Technique_-_FXAA
//
//
//  FXAA
//
//  Modified code from original source of Norbert Nopper:
//  https://github.com/McNopper/OpenGL/blob/master/Example42/shader/fxaa.frag.glsl

LAYOUT_INDEX(idxFXAA) SUBROUTINE(filterSub) vec4 fxaaFilter()
{
    //vec2 vTexCrd = gl_FragCoord.xy * invScreenSize;
    vec4 centerPix = texture(origTexture, vTexCoord);
    vec3 rgbM = centerPix.rgb;

    // Sampling neighbour texels. Offsets are adapted to OpenGL texture coordinates. 
    vec3 rgbNW = textureOffset(origTexture, vTexCoord, ivec2(-1,  1)).rgb;
    vec3 rgbNE = textureOffset(origTexture, vTexCoord, ivec2( 1,  1)).rgb;
    vec3 rgbSW = textureOffset(origTexture, vTexCoord, ivec2(-1, -1)).rgb;
    vec3 rgbSE = textureOffset(origTexture, vTexCoord, ivec2( 1, -1)).rgb;

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
    if (lumaMax - lumaMin < lumaMax * fxaaData.x) { return vec4(rgbM, centerPix.a); }

    // Sampling is done along the gradient.
    vec2 dir = vec2(-((lumaNW + lumaNE) - (lumaSW + lumaSE)), 
                     ((lumaNW + lumaSW) - (lumaNE + lumaSE)));
    
    // Sampling step distance depends on the luma: The brighter the sampled texels, the smaller the final sampling step direction.
    // This results, that brighter areas are less blurred/more sharper than dark areas.  
    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.25 * fxaaData.y, fxaaData.z);

    // Factor for norming the sampling direction plus adding the brightness influence. 
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    
    // Calculate final sampling direction vector by reducing, clamping to a range and finally adapting to the texture size. 
    dir = min(vec2(fxaaData.w, fxaaData.w), max(vec2(-fxaaData.w, -fxaaData.w), dir * rcpDirMin)) * invScreenSize;

    vec4 rgbA = 0.5 * (
        texture(origTexture, vTexCoord + dir * (1.0/3.0 - 0.5)) +
        texture(origTexture, vTexCoord + dir * (2.0/3.0 - 0.5)));    
    vec4 rgbB = rgbA * 0.5 + 0.25 * (
        texture(origTexture, vTexCoord + dir * (0.0/3.0 - 0.5)) +
        texture(origTexture, vTexCoord + dir * (3.0/3.0 - 0.5)));

    // Calculate luma for checking against the minimum and maximum value.
    float lumaB = dot(rgbB.rgb, toLuma);

    // Are outer samples of the tab beyond the edge ... 
    return (lumaB  < lumaMin || lumaB  > lumaMax) ? rgbA : rgbB; 
}

//  Pass1 Gauss Blur
////////////////////////////////////////////////////////////////////////////
LAYOUT_INDEX(idxRADIAL_P1) SUBROUTINE(filterSub) vec4 radialPass1()
{
    //vec4 c = texelFetch(origTexture, ivec2(gl_FragCoord.xy), 0);
    return gPass(origTexture, vec2(1.0, 0.0));
}

//  Pass2 Gauss Blur
////////////////////////////////////////////////////////////////////////////
LAYOUT_INDEX(idxRADIAL_P2) SUBROUTINE(filterSub) vec4 radialPass2()
{
#ifdef USE_TEXEL_FETCH
    vec4 original = texelFetch(origTexture,ivec2(gl_FragCoord.xy),0); //origTex * intensity
#else
    vec4 original = texture(origTexture,vTexCoord); //origTex * intensity
#endif    
    vec4 blurred = gPass(auxTexture, vec2(0.0, 1.0));                //blur * intensity

    return (min(mix(original * texControls.y,vec4(blurred.rgb*texControls.x,original.a),mixTexture), 1.0f));

    //return qualitySetting(mix(original,clamp(mix(blurred,newBlur,clamp(dotP+.5, 0.0, 1.0)), 0.0, 1.0),mixTexture));

}

//  Pass2 Gauss Blur + threshold -> reduced (1/4) bilateral
////////////////////////////////////////////////////////////////////////////
LAYOUT_INDEX(idxRADIAL_P2_BILAT) SUBROUTINE(filterSub) vec4 radialPass2withBilateral()
{
#ifdef USE_TEXEL_FETCH
    vec4 original = texelFetch(origTexture,ivec2(gl_FragCoord.xy),0) * texControls.y;
#else
    vec4 original = texture(origTexture,vTexCoord) * texControls.y;
#endif    
    vec4 blurred = gPass(auxTexture, vec2(0.0, 1.0))* texControls.x;
    vec4 newBlur = bilateralSmartSmooth(.5) * texControls.z;

    return (min(mix(original,mix(blurred,newBlur, texControls.w),mixTexture), 1.0));
    // mix(blurred,newBlur, dotP)

}

//  Bilateral onePass
////////////////////////////////////////////////////////////////////////////
LAYOUT_INDEX(idxBILATERAL) SUBROUTINE(filterSub) vec4 bilateralSmooth()
{
#ifdef USE_TEXEL_FETCH
    vec4 original = texelFetch(origTexture,ivec2(gl_FragCoord.xy),0) * texControls.y; //origTex intensity
#else
    vec4 original = texture(origTexture,vTexCoord) * texControls.y; //origTex intensity
#endif    
    vec4 newBlur = bilateralSmartSmoothOK() * texControls.z;
    return min(mix(original,newBlur,mixTexture),1.0);
}

//  bypass, only image adjust
////////////////////////////////////////////////////////////////////////////
LAYOUT_INDEX(idxTUNING) SUBROUTINE(filterSub) vec4 imgTuning()
{
#ifdef USE_TEXEL_FETCH
    return qualitySetting(min(texelFetch(origTexture,ivec2(gl_FragCoord.xy), 0) * texControls.y, 1.0f));
#else
    return qualitySetting(min(texture(origTexture,vTexCoord) * texControls.y, 1.0f));
#endif    
}

LAYOUT_INDEX(idxBLIT) SUBROUTINE(filterSub) vec4 blitFilter()
{
#ifdef USE_TEXEL_FETCH
    return texelFetch(origTexture, gl_FragCoord.xy, 0);
#else
    return texture(origTexture, vTexCoord);
#endif    
}

LAYOUT_INDEX(idxMIXTEX) SUBROUTINE(filterSub) vec4 mixTwoTex()
{
    //   return texture(origTexture, pipViewport.xy+vTexCoord * pipViewport*zw);
#ifdef USE_TEXEL_FETCH
    ivec2 coord = ivec2(gl_FragCoord.xy);
    vec4 c0 = texelFetch(origTexture, coord, 0);
    vec4 c1 = texelFetch(auxTexture,  coord, 0);
#else
    vec4 c0 = texture(origTexture, vTexCoord);    
    vec4 c1 = texture(auxTexture,  vTexCoord);
#endif
    return mix(c0, c1, mixTexture);
}


LAYOUT_INDEX(idxPIP) SUBROUTINE(filterSub) vec4 pipRender()
{
    //   return texture(origTexture, pipViewport.xy+vTexCoord * pipViewport*zw);
#ifdef USE_TEXEL_FETCH
    #error not still defined
#else
    vec4 c0 = texture(auxTexture, vTexCoord);
    if(vTexCoord.x>pipViewport.x && vTexCoord.y>pipViewport.y && 
       vTexCoord.x<pipViewport.z && vTexCoord.y<pipViewport.w) {
        vec4 c1 = vec4(0.0);
        vec2 invSize = toneMapVals;
        if(toneMap) {// border Active
            vec4 wndBorder = pipViewport + vec4(invScreenSize,-invScreenSize); // 1-pixel border 1/screen size

            if(vTexCoord.x<wndBorder.x || vTexCoord.y<wndBorder.y || 
               vTexCoord.x>wndBorder.z || vTexCoord.y>wndBorder.w) return fxaaData; //borderColor
        }  
        c1 = texture(origTexture, (vTexCoord-pipViewport.xy)*invSize);
        return mix(c0, c1*threshold, mixTexture);
    } else return c0;
#endif
}

void main ()
{
#if defined(GL_ES) || defined(GLCHAOSP_NO_USES_GLSL_SUBS)
    //outColor = blurCallType==0 ? byPass() : bilateralSmooth();
    switch(blurCallType) {
        default:
        case idxTUNING          : outColor = imgTuning();                break;
        case idxRADIAL_P1       : outColor = radialPass1();              break;
        case idxRADIAL_P2       : outColor = radialPass2();              break;
        case idxRADIAL_P2_BILAT : outColor = radialPass2withBilateral(); break;
        case idxBILATERAL       : outColor = bilateralSmooth();          break;
        case idxFXAA            : outColor = fxaaFilter();               break;
        case idxBLIT            : outColor = blitFilter();               break;
        case idxMIXTEX          : outColor = mixTwoTex();                break;
        case idxPIP             : outColor = pipRender();                break;
    }
#else
    outColor = imageResult();
#endif
}