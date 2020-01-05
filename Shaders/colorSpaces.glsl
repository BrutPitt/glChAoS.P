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
#line 14    //#version dynamically inserted


//This is an HEADER/INCLUDE file for RadialBlur2PassFrag.glsl and cmTextureFrag.glsl

#ifdef GL_ES
#define saturate(v) clamp(v, vec3(0.0), vec3(1.0))
#else
#ifndef saturate
#define saturate(v) clamp(v, 0.0, 1.0)
#endif
#endif

const float epsilon = 1e-10;

// luminance 
float luminance(vec3 c) { return dot(c, vec3(0.2990, 0.5870, 0.1140)); }
// linear rgb color luminance 
float linearLum(vec3 c) { return dot(c, vec3(0.2126, 0.7152, 0.0722)); }

vec3 toneMapping(vec3 c) 
{ 
    return c/(vec3(1.0) + c); 
}

// tonemap -> col = A * pow(col, G); -> x = A and y = G
// A > 0,  0 < G < 1
vec3 toneMapping(vec3 c, float A, float g) 
{ 
    return A * pow(clamp(c, vec3(0.0), vec3(1.0)), vec3(g)); 
}


vec2 contrastHSL(vec2 c, float contrast) 
{   
    return c * (vec2(1.0) + (contrast>0.0 ? vec2(contrast*2.0,contrast*.33) : vec2(contrast*.5,contrast*.5)));
}

//soft brightness
float brightnessHSL(float c, float bright)
{
   return bright>0.0 ? c * (1.0 + bright*1.5) + bright*.02 : c * (1.0 + bright*.95);
}


vec3 contrastRGB(vec3 c, float contr) 
{   
    vec3 dC = c - vec3(0.5);

    return vec3(0.5) + sign(dC)*pow(abs(dC), vec3(1.0/contr));
}

vec3 contrast2(vec3 c, float contr) 
{   
    float f = 1.01*(contr + 1.0) / (1.01-contr);
    const vec3 cHalf = vec3(.5);

    return f * (c - cHalf) + cHalf;
}


/////////////////////////////////////////////////
vec3 gammaCorrection(vec3 c, float gamma)
{
    return saturate(vec3(  c.r < 0.0031308f ? 12.92f*c.r : 1.055f * pow(c.r, gamma) - .055f,
                           c.g < 0.0031308f ? 12.92f*c.g : 1.055f * pow(c.g, gamma) - .055f,
                           c.b < 0.0031308f ? 12.92f*c.b : 1.055f * pow(c.b, gamma) - .055f  ));
}

/////////////////////////////////////////////////
vec3 rgb2yuv(vec3 c)
{
    // alias... YCrCb
    CONST float y = luminance(c);
    CONST float u = (c.b - y) * 0.564f; //= -0.169R - 0.331G + 0.500B
    CONST float v = (c.r - y) * 0.713f; //= 0.500R - 0.419G - 0.081B

    return vec3(y,u,v);
}
/////////////////////////////////////////////////
vec3 yuv2rgb(vec3 c)
{
    CONST float R = c.x + c.z * 1.402f;
    CONST float G = c.x - c.y * 0.344f - c.z * 0.714;
    CONST float B = c.x + c.y * 1.772f;

    return vec3(R,G,B);
}

/////////////////////////////////////////////////
vec3 rgb2hcv(vec3 rgb)
{
    CONST vec4 P = (rgb.g < rgb.b) ? vec4(rgb.bg, -1.0, 2.0/3.0) : vec4(rgb.gb, 0.0, -1.0/3.0);
    CONST vec4 Q = (rgb.r < P.x)   ? vec4(P.xyw, rgb.r)          : vec4(rgb.r, P.yzx);
    CONST float C = Q.x - min(Q.w, Q.y);
    CONST float H = abs((Q.w - Q.y) / (6.0 * C + epsilon) + Q.z);
    return vec3(H, C, Q.x);
}

/////////////////////////////////////////////////
vec3 hue2rgb(float hue)
{
    float H = fract(hue);
    return saturate(vec3(      abs(H * 6.0 - 3.0) - 1.0,
                         2.0 - abs(H * 6.0 - 2.0),
                         2.0 - abs(H * 6.0 - 4.0) 
                         ));
}

/////////////////////////////////////////////////
vec3 hsv2rgb(vec3 hsv)
{
    vec3 rgb = hue2rgb(hsv.x);
    return ((rgb - 1.0) * hsv.y + 1.0) * hsv.z;
}

/////////////////////////////////////////////////
vec3 hsl2rgb(vec3 hsl)
{
    vec3 rgb = hue2rgb(hsl.x);
    float C = (1.0 - abs(2.0 * hsl.z - 1.0)) * hsl.y;
    return (rgb - 0.5) * C + hsl.z;
}

/////////////////////////////////////////////////
vec3 hcy2rgb(vec3 hcy)
{
    vec3 RGB = hue2rgb(hcy.x);
    float Z = luminance(RGB);
    return (RGB - Z) * ((hcy.z < Z) ? hcy.y * hcy.z / Z : ((Z < 1.0) ? hcy.y * (1.0 - hcy.z) / (1.0 - Z) : hcy.y)) + hcy.z;
}


/////////////////////////////////////////////////
vec3 rgb2hsv(vec3 rgb)
{
    vec3 HCV = rgb2hcv(rgb);
    float S = HCV.y / (HCV.z + epsilon);
    return vec3(HCV.x, S, HCV.z);
}

/////////////////////////////////////////////////
vec3 rgb2hsl(vec3 rgb)
{
    vec3 HCV = rgb2hcv(rgb);
    float L = HCV.z - HCV.y * 0.5;
    float S = HCV.y / (1.0 - abs(L * 2.0 - 1.0) + epsilon);
    return vec3(HCV.x, S, L);
}

/////////////////////////////////////////////////
vec3 rgb2hcy(vec3 rgb)
{
    vec3 HCV = rgb2hcv(rgb);
    float Y = luminance(rgb);
    float Z = luminance(hue2rgb(HCV.x));
    HCV.y *= (Y < Z) ? Z / (epsilon + Y) : (1.0 - Z) / (epsilon + 1.0 - Y);

    return vec3(HCV.x, HCV.y, Y);
}
