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
#line 17    //#version dynamically inserted

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

in vec2 viewRay;
in vec2 vTexCoord;

out vec4 outColor;

LAYOUT_BINDING(5) uniform sampler2D prevData;
LAYOUT_BINDING(6) uniform sampler2D aoTex;
LAYOUT_BINDING(7) uniform sampler2D shadowTex;
LAYOUT_BINDING(8) uniform sampler2D texBaseColor;
LAYOUT_BINDING(10) uniform sampler2D zTex;


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

float getStrongAO(float AO)
{
    return u.aoStrong<.001 ? 1.0 : pow(AO,u.aoStrong);
}

vec4 pixelColorLight(vec3 vtx, vec4 color, vec4 N, float AO, float shadow)
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
#if defined(GL_ES) || defined(GLCHAOSP_NO_USES_GLSL_SUBS)
        float specular = u.lightModel == uint(idxPHONG) ? specularPhong(V, light, N.xyz) : (u.lightModel == uint(idxBLINPHONG) ? specularBlinnPhong(V, light, N.xyz) : specularGGX(V, light, N.xyz));
#else
        float specular = lightModel(V, light, N.xyz);
#endif

        float aoD = getStrongAO(AO);
        vec3 lColor =  smoothstep(u.sstepColorMin, u.sstepColorMax,
                                    (aoD*color.rgb * u.lightColor * lambertian * u.lightDiffInt +  //diffuse component
                                    u.lightColor * specular * u.lightSpecInt) * shadow + 
                                    AO*(color.rgb*u.lightAmbInt + vec3(u.lightAmbInt)) * .5 
                                 );

        return vec4(lColor.rgb , 1.0);
    //}
}


float getBlurredAO(vec2 uv)
{
    float result = 0.0;
    for (int x = -2; x <= 2; x++) 
        for (int y = -2; y <= 2; y++) 
            result += texture(aoTex,uv+vec2(x,y)*u.invScrnRes).w;
    return result * .04; // -> result / 25.;
}

float getShadow(vec2 uv)
{
    return clamp(texture(aoTex,uv).x, 0.0, 1.0);
}

float buildShadow(vec4 frag)
{
    
    vec4 pt = vec4(viewRay*-frag.z, frag.z, 1.0);
           
    vec4 fragPosLightSpace = m.pMatrix * m.mvLightM  * pt;
    // perspective divide
    vec3 projCoords = (fragPosLightSpace.xyz)/(fragPosLightSpace.w);
    // to [0,1] range
    projCoords = projCoords * 0.5 + .5;
    // get closest depth ==> from light's perspective
    float closestDepth = restoreZ(texture(shadowTex, projCoords.xy).r); 
    // get depth ==> light's perspective
    float currentDepth = restoreZ(projCoords.z) + u.shadowBias;
    // check whether current frag pos is in shadow
    float shadow = (currentDepth+u.shadowBias) < closestDepth  ? 0.0 : 1.0;

    return shadow;
}  

float random(vec3 seed, int i){
    vec4 seed4 = vec4(seed,i);
    float dot_product = dot(seed4, vec4(12.9898,78.233,45.164,94.673));
    return fract(sin(dot_product) * 43758.5453);
}

float buildScatterShadow(vec4 frag)
{
    float shadow = 0.0;
    for(int i=0; i<8; i++) {
        int idx = int(16.0*random(gl_FragCoord.xyy, i));
        vec4 pt = vec4((viewRay*-frag.z)+poissonDisk[idx]/1000.0, frag.z, 1.0);
           
        vec4 fragPosLightSpace = m.pMatrix * m.mvLightM  * pt;
        //fragPosLightSpace = m.pMatrix * m.mvLightM * (vec4(viewLight*frag.z, frag.z, 1.0));
        // perform perspective divide
        vec3 projCoords = (fragPosLightSpace.xyz)/(fragPosLightSpace.w);
        // transform to [0,1] range
        projCoords = projCoords * 0.5 + .5;
        // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
        //float closestDepth = (SampleTextureCatmullRom(projCoords.xy).x);
        float closestDepth = restoreZ(texture(shadowTex, projCoords.xy+poissonDisk[idx]/1000.0).r); 
        // get depth of current fragment from light's perspective
        float currentDepth = restoreZ(projCoords.z) + u.shadowBias;
        // check whether current frag pos is in shadow
        shadow += currentDepth < closestDepth  ? 0.0 : 1.0/8.0;
    }

    return shadow;
}  

float buildSmoothScattereShadow(vec4 frag)
{
    vec4 pt = vec4(viewRay*-frag.z, frag.z, 1.0);

    mat4 tMat = m.pMatrix * m.mvLightM;
           
    vec4 fragPosLightSpace = tMat * pt;
    // perform perspective divide and port to [0,1] range
    vec3 projCoords = 0.5 * (fragPosLightSpace.xyz)/fragPosLightSpace.w + .5;

    if(projCoords.z>1.0) return 0.0;
    // get depth of current fragment from light's perspective    

    float currentDepth = restoreZ(projCoords.z) + u.shadowBias;

    vec2 stepTex = u.shadowGranularity*u.invScrnRes;

    float shadow = 0.0;
    int radius = int(u.shadowSmoothRadius);
    int diam = radius * 2 + 1;
    float invDiv = 1.0/float(diam * diam);
    
    for (int x = -radius; x <= radius ; x++) 
        for (int y = -radius; y <= radius; y++) {
            float closestDepth = restoreZ(texture(shadowTex, projCoords.xy+vec2(x,y)*stepTex).r);

            shadow += currentDepth < closestDepth  ?  u.shadowDarkness*u.shadowDarkness*invDiv : invDiv ;    // 1.0/9.0

            vec4 pt = vec4((viewRay+vec2(x,y)*stepTex)*-frag.z-(vec2(x,y)*stepTex), frag.z, 1.0);
           
            vec4 fragPosLightSpace = tMat * pt;
            vec3 projCoords = .5 * (fragPosLightSpace.xyz)/fragPosLightSpace.w + .5;

            closestDepth = restoreZ(texture(shadowTex, projCoords.xy+poissonDisk[(x*diam+y)%16]/1000.0).r);
            float currentDepth = restoreZ(projCoords.z) + u.shadowBias;

            shadow += currentDepth < closestDepth  ?  u.shadowDarkness*u.shadowDarkness*invDiv : invDiv ;    // 1.0/9.0
        }

    return shadow * .5;
}


float buildSmoothShadow(vec4 frag)
{
    //vec4 pt = vec4(viewRay*-frag.z, frag.z, 1.0);
    vec4 pt = getVertexFromDepth(-viewRay, frag.z);
    //float dist = distance(pt.xyz, 

    vec4 fragPosLightSpace = m.mvpLightM * pt;

    vec3 projCoords = 0.5 * (fragPosLightSpace.xyz)/fragPosLightSpace.w + .5;

    if(projCoords.z>1.0) return 0.0;


    //float bias = 0.005 *  tan(acos(clamp(dot(normalize(u.lightDir),frag.xyz), 0., .9999)));
    float currentDepth = restoreZ(projCoords.z);
    currentDepth += -currentDepth*.0025 + u.shadowBias;

    vec2 stepTex = u.shadowGranularity*u.invScrnRes;

    float shadow = 0.0;
    int radius = int(u.shadowSmoothRadius);
    float diam = float(radius) * 2. + 1.0;
    float invDiv = 1.0/(diam * diam);
    
    for (int x = -radius; x <= radius ; x++) 
        for (int y = -radius; y <= radius; y++) {
            float closestDepth = restoreZ(texture(shadowTex, projCoords.xy+vec2(x,y)*stepTex).r);

            shadow += (currentDepth < closestDepth  ?  u.shadowDarkness*u.shadowDarkness*invDiv : invDiv) ;    // 1.0/9.0
/* Real more accuracy ???
            vec4 pt = vec4((viewRay+vec2(x,y)*stepTex)*-frag.z-(vec2(x,y)*stepTex), frag.z, 1.0);
           
            vec4 fragPosLightSpace = tMat * pt;
            vec3 projCoords = .5 * (fragPosLightSpace.xyz)/fragPosLightSpace.w + .5;

            closestDepth = restoreZ(texture(shadowTex, projCoords.xy).r);
            float currentDepth = restoreZ(projCoords.z) + u.shadowBias;

            shadow += (currentDepth < closestDepth  ?  u.shadowDarkness*u.shadowDarkness*invDiv : invDiv) * .1 ;    // 1.0/9.0
*/
        }

    return shadow;
}


float getBlurredShadow(vec2 uv)
{
    
    float result = 0.0;
    const int radius = 1;//int(u.shadowSmoothRadius*.5);
    const float diam = float(radius) * 2. + 1.0;
    const float div = diam * diam;
    for (int x = -radius; x <= radius ; x++) 
        for (int y = -radius; y <= radius; y++) 
            result += getShadow(uv+vec2(x,y)*u.invScrnRes);

    result /= div;
    return result;
    //return result>.99 ? result : min(result*result*result, texture(aoTex,uv).x) ; // -> result / 25.;
    //return result;
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
        N += texture(aoTex, (uv + vec2(sampleCoord[i].xy)) * u.invScrnRes).xyz * sampleCoord[i].z;

    return normalize(2.0 * N * colorDiv - 1.0);
}


void main()
{

    //if(N.w > 1.0) discard;
    //if(u.lightActive==1) {
    vec2 uv = gl_FragCoord.xy*u.invScrnRes;
    //float test = getZ(texelFetch(prevData,ivec2(uv), 0).x);
    //if(test>.5) {
//        if(zEye>=1.0) discard;

    float depth = texture(zTex,uv).x;
    if(depth>.9999) { discard; outColor = vec4(0.0); return; }

    float z = restoreZ(depth);
    vec4 vtx = getVertexFromDepth(viewRay, z);

    //float AO = bicubic(uv, 3); //texelFetch(aoTex,ivec2(uv), 0).w;
    float AO = bool(u.pass&RENDER_AO) ? getBlurredAO(uv) : 1.0;
    float shadow = bool(u.pass &  RENDER_SHADOW) ?  buildSmoothShadow(vtx) : 1.0;


    if(bool(u.pass &  RENDER_DEF)) {

        //float zD = form_01_to_m1p1(depth);

        //vec3 N = blurredNormals(uv).xyz;
        //vec3 N = 2.0 * texelFetch(aoTex,ivec2(uv), 0).xyz - 1.0;

        //vec4 N = vec4(getSelectedNormal(z, prevData), 1.0);      

        vec4 N = vec4(getSelectedNormal(vtx, zTex), 1.0);
        //vec4 N = vec4(getSimpleNormal(vtx, prevData), 1.0);

        vec4 color = vec4(texture(prevData,uv).rgb, texture(texBaseColor,uv).a);

        vtx = vec4(viewRay, z, 1.0);
        vtx = m.invP * vtx;
        vtx /= vtx.w;
        
        outColor = pixelColorLight(vtx.xyz, color, N, AO, shadow);

        //outColor = vec4(AO);

    } else {
        //vec3 lightColor = texture(prevData,uv).rgb;

        //vec3 packedColor = texelFetch(prevData,ivec2(gl_FragCoord.xy),0).rgb;
        //vec2 c = unPackColor16(packedColor.y);
        //vec3 lightColor = vec3(unPackColor16(packedColor.x), c.x);
        //vec3 baseColor  = vec3(c.y, unPackColor16(packedColor.z));

        vec3 lightColor = texelFetch(prevData,ivec2(gl_FragCoord.xy),0).rgb;
        vec4 baseColor =  texelFetch(texBaseColor,ivec2(gl_FragCoord.xy), 0);

        float aoD = getStrongAO(AO);
        lightColor = smoothstep(u.sstepColorMin, u.sstepColorMax, 
                                 aoD *lightColor * shadow + 
                                AO * (baseColor.rgb*u.lightAmbInt + vec3(u.lightAmbInt)) * .5);
        outColor = vec4(lightColor, baseColor.a) ;
        //outColor = vec4( 1.0) ;

    }

}