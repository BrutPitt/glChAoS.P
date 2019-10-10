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


#ifndef GL_ES
    subroutine vec4 _radialPass();
    subroutine uniform _radialPass imageResult;
#endif


LAYOUT_BINDING(2) uniform _blurData {
    vec4 sigma;
    float threshold;
    bool toneMap;
    vec2 toneMapVals; // tonemap -> col = A * pow(col, G); -> x = A and y = G

    vec4 texControls;
    vec4 videoControls; //videoControls vec4 ->  1.f/m_gamma, m_exposure, m_bright, m_contrast
    vec2 invScreenSize;

    float mixTexture;

    int blurCallType;
};

LAYOUT_BINDING(0) uniform sampler2D origTexture;
LAYOUT_BINDING(1) uniform sampler2D pass1Texture;

//LAYOUT_BINDING(3) uniform sampler2D depthTexture;
//LAYOUT_BINDING(4) uniform sampler2D vtxTex;
//LAYOUT_BINDING(5) uniform sampler2D normTex;


//in vec2 vTexCoord;
out vec4 outColor;


CONST vec2 aspect = vec2(1.0, 1.0);


//Bilateral smart smooth

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

vec4 bilateralSmartSmooth(CONST float reductFactor)
{
    CONST float bsigma = threshold*reductFactor;
    CONST float radius = float(round(sigma.y*sigma.x-1.f));
    CONST float radQ = radius * radius;
    
    CONST float invSigma = 1.f/sigma.x;
    CONST float invSigmaQx2 = .5 * invSigma * invSigma;          // 1.0 / (sigma^2 * 2.0)
    CONST float invSigmaQx2PI = INV_PI * invSigmaQx2;    // 1.0 / (sqrt(PI) * sigma)
    
    CONST float invBSigma = 1.f/bsigma;
    CONST float invBSigmaSqx2 = .5 * invBSigma * invBSigma;          // 1.0 / (sigma^2 * 2.0)
    CONST float invBSigmaxSqrt2PI = INV_SQRT_OF_2PI * invBSigma;    // 1.0 / (sqrt(2*PI) * sigma)
    
    CONST vec4 centrPx = texelFetch(origTexture,ivec2(gl_FragCoord.xy),0);
    
    float Zbuff = 0.0;
    vec4 accumBuff = vec4(0.0);
    
    vec2 d;
    for (d.x=-radius; d.x <= radius; d.x++)	{
        CONST float pt = sqrt(radQ-d.x*d.x);
        for (d.y=-pt; d.y <= pt; d.y++) {
            CONST float blurFactor = exp( -dot(d , d) * invSigmaQx2 ) * invSigmaQx2;
            
            CONST vec4 walkPx =  texelFetch(origTexture, ivec2(gl_FragCoord.xy+d),0 );
            CONST vec4 dC = walkPx-centrPx;
            CONST float deltaFactor = exp( -dot(dC, dC) * invBSigmaSqx2) * invBSigmaxSqrt2PI * blurFactor;
                                 
            Zbuff     += deltaFactor;
            accumBuff += deltaFactor*walkPx;
        }
    }
    return accumBuff/Zbuff;
}


vec4 bilateralSmartSmoothOK()
{
    return bilateralSmartSmooth(1.0);
}

vec4 gPass(sampler2D tex, vec2 direction) 
{
    //vec2 offset = wSize; 
    
    //compute the radius across the kernel
    CONST float radius = sigma.y*sigma.x-1.f;
    
    float Zbuff = 0.0;
    vec4 accumBuff = vec4(0.0);
    
    //precompute factors used every iteration
    CONST float invSigma = 1.f/sigma.x;
    CONST float invSigmaSqx2 = .5 * invSigma * invSigma;          // 1.0 / (sigma^2 * 2.0)
    CONST float invSigmaxSqrt2PI = INV_SQRT_OF_2PI * invSigma;    // 1.0 / (sqrt(PI) * sigma)
    
    // separable Gaussian
    for ( float r = -radius; r <= radius; r++) {
        float factor = exp( -(r*r) * invSigmaSqx2 ) * invSigmaxSqrt2PI;
        vec4 c = texelFetch(tex, ivec2(gl_FragCoord.xy + r * direction), 0);
        accumBuff += factor * c;
    }
    
    return accumBuff;
}



//#define NVIDIA_GAUSS 
#ifdef NVIDIA_GAUSS 
 
// The inverse of the texture dimensions along X and Y
 
float blurSize = 1.0*wSize.x;       
                           // The sigma value for the gaussian function: higher value means more blur
                            // A good value for 9x9 is around 3 to 5
                            // A good value for 7x7 is around 2.5 to 4
                            // A good value for 5x5 is around 2 to 3.5
                            // ... play around with this based on what you need 
 
const float pi = 3.14159265;
float numBlurPixelsPerSide = (3.0*sigma);
 
vec4 glow(sampler2D tex, vec2 direction) {  
 
 
  // Incremental Gaussian Coefficent Calculation (See GPU Gems 3 pp. 877 - 889)
  vec3 incrementalGaussian;
  incrementalGaussian.x = 1.0 / (sqrt(2.0 * pi) * sigma.x);
  incrementalGaussian.y = exp(-0.5 / (sigma.x * sigma.x));
  incrementalGaussian.z = incrementalGaussian.y * incrementalGaussian.y;
 
  vec4 avgValue = vec4(0.0, 0.0, 0.0, 0.0);
  float coefficientSum = 0.0;
 
  // Take the central sample first...
  avgValue += texelFetch(tex, ivec2(gl_FragCoord.xy), 0) * incrementalGaussian.x;
  coefficientSum += incrementalGaussian.x;
  incrementalGaussian.xy *= incrementalGaussian.yz;
 
  // Go through the remaining 8 vertical samples (4 on each side of the center)
  for (float i = 1.0; i <= numBlurPixelsPerSide; i++) { 
    avgValue += texelFetch(tex, ivec2(gl_FragCoord.xy - i * blurSize * direction), 0) * incrementalGaussian.x;         
    avgValue += texelFetch(tex, ivec2(gl_FragCoord.xy + i * blurSize * direction), 0) * incrementalGaussian.x;         
    coefficientSum += 2.0 * incrementalGaussian.x;
    incrementalGaussian.xy *= incrementalGaussian.yz;
  }
 
  return avgValue / coefficientSum;
}

#endif


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

#ifdef GL_ES
    col.yz = clamp(col.yz,vec2(0.0), vec2(1.0));
    col.rgb = hsl2rgb(vec3(col.xyz));
#else
    col.rgb = hsl2rgb(vec3(col.x,saturate(col.yz)));
#endif        
        
    if(videoControls.w>0.0) col.rgb = contrastRGB(col.rgb, videoControls.w + (1.0+epsilon));
        //if(videoControls.w>0.0) col.rgb = contrast2(col.rgb, (videoControls.w ));

    return col;

}

//  Pass1 Gauss Blur
////////////////////////////////////////////////////////////////////////////
LAYOUT_INDEX(1) SUBROUTINE(_radialPass) vec4 radialPass1()
{
    return gPass(origTexture, vec2(1.0, 0.0));
}

//  Pass2 Gauss Blur
////////////////////////////////////////////////////////////////////////////
LAYOUT_INDEX(2) SUBROUTINE(_radialPass) vec4 radialPass2()
{
    vec4 original = texelFetch(origTexture,ivec2(gl_FragCoord.xy),0) * texControls.y; //origTex * intensity
    vec4 blurred = gPass(pass1Texture, vec2(0.0, 1.0))* texControls.x;                //blur * intensity

    return qualitySetting(min(mix(original,blurred,mixTexture), 1.0f));

    //return qualitySetting(mix(original,clamp(mix(blurred,newBlur,clamp(dotP+.5, 0.0, 1.0)), 0.0, 1.0),mixTexture));

}

//  Pass2 Gauss Blur + threshold -> reduced (1/4) bilateral
////////////////////////////////////////////////////////////////////////////
LAYOUT_INDEX(3) SUBROUTINE(_radialPass) vec4 radialPass2withBilateral()
{
    vec4 original = texelFetch(origTexture,ivec2(gl_FragCoord.xy),0) * texControls.y;
    vec4 blurred = gPass(pass1Texture, vec2(0.0, 1.0))* texControls.x;
    vec4 newBlur = bilateralSmartSmooth(.2) * texControls.z;

    float dotP = clamp(linearLum(newBlur.rgb)+texControls.w, 0.0f, 1.0f);

    return qualitySetting(min(mix(original,mix(blurred,newBlur, dotP),mixTexture), 1.0));
    // mix(blurred,newBlur, dotP)

}

//  Bilateral onePass
////////////////////////////////////////////////////////////////////////////
LAYOUT_INDEX(4) SUBROUTINE(_radialPass) vec4 bilateralSmooth()
{
    vec4 original = texelFetch(origTexture,ivec2(gl_FragCoord.xy),0) * texControls.y; //origTex intensity
    vec4 newBlur = bilateralSmartSmoothOK() * texControls.z;
    return qualitySetting(min(mix(original,newBlur,mixTexture), 1.0f));
}

// result suitable for assigning to gl_FragDepth
float depthSample(float linearDepth, float zNear, float zFar)
{
    float nonLinearDepth = (zFar + zNear - 2.0 * zNear * zFar / linearDepth) / (zFar - zNear);
    nonLinearDepth = (nonLinearDepth + 1.0) / 2.0;
    return nonLinearDepth;
}

float specularGGX(vec3 vtx, vec3 L, vec3 N) 
{

    float ggxRoughness = .703;
    float ggxFresnel = .348;

    float alpha = ggxRoughness*ggxRoughness;
    float alphaSqr = alpha * alpha;

    vec3 V = normalize(vtx);
    vec3 H = normalize(L - V); // View = -
    float dotLH = max(0.0, dot(L,H));
    float dotNH = max(0.0, dot(N,H));
    float dotNL = max(0.0, dot(N,L));

    // D (GGX normal distribution)
    float denom = dotNH * dotNH * (alphaSqr - 1.0) + 1.0;
    float D = alphaSqr / (3.141592653589793 * denom * denom);

    // F (Fresnel term)
    float F = ggxFresnel + (1.0 - ggxFresnel) * pow(1.0 - dotLH, 5.0);
    float k = 0.5 * alpha;
    float k2 = k * k;

    return dotNL * D * F / (dotLH*dotLH*(1.0-k2)+k2);
}

float getZ(float depth)
{
    float f = 100., n = .1;

    float zNDC = 2.1 * depth - 1.05;



    //if(zNDC>1.0) discard;
/*
    float zEye = 2.0 * n * f / (f + n - zNDC * (f - n));
    return zEye;
*/
    return zNDC;
}
/*
float getZ(vec2 uv)
{
    vec3 sampleCoord[9];
    const float NR = 1.;
    sampleCoord[0] = vec3( 0.,  0., 1.);
    sampleCoord[1] = vec3( NR, -NR, .3);
    sampleCoord[2] = vec3(-NR,  NR, .3);
    sampleCoord[3] = vec3( NR,  NR, .3);
    sampleCoord[4] = vec3(-NR, -NR, .3);
    sampleCoord[5] = vec3(  0,  NR, .5);
    sampleCoord[6] = vec3(  0, -NR, .5);
    sampleCoord[7] = vec3( NR,   0, .5);
    sampleCoord[8] = vec3(-NR,   0, .5);
    
    float colorDiv = .2381;

    float z = 0;
    for (int i = 0; i < 9; i++) 
        z+=texelFetch(depthTexture,ivec2(uv+sampleCoord[i].xy), 0).r * sampleCoord[i].z;
        //z+=texture(depthTexture,(uv+sampleCoord[i].xy)*invScreenSize).r * sampleCoord[i].z;

    return getZ(z*colorDiv);
}
*/

vec2 scale (vec2 p, float s) {
    return p * s;
}


//  bypass, only image adjust
////////////////////////////////////////////////////////////////////////////
LAYOUT_INDEX(0) SUBROUTINE(_radialPass) vec4 byPass()
{
    return qualitySetting(min(texelFetch(origTexture,ivec2(gl_FragCoord.xy),0) * texControls.y, 1.0f));
}

void main ()
{
#if defined(GL_ES) || defined(TEST_WGL)
    outColor = blurCallType==0 ? byPass() : bilateralSmooth();
#else
    outColor = imageResult();
#endif
}