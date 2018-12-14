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
#ifndef PARTICLES_UTILS_H
#define PARTICLES_UTILS_H

#ifndef M_PI
    #define M_PI       3.14159265358979323846
#endif //M_PI

#ifdef APP_USE_GLEW
    #include <GL/glew.h>
#else
    #include "libs/glad/glad.h"
#endif
#include "glm/glm.hpp"
#include <vector>

using namespace glm;


float* createGaussianMap(int N, int components);

class textureBaseClass 
{
public:
    
    textureBaseClass() { }

    GLuint getTexID()   { return texID;   }
    GLuint getTexSize() { return texSize; }

    GLuint texID   = -1;
    GLuint texSize =  0;

protected:
    void buildTex1D();

};



class RandomTexture : public textureBaseClass
{
public:

    RandomTexture();
    ~RandomTexture();
    void buildTex(int size);


    void incIndex() { if(++texIndex >= texSize) texIndex=0; }
    int  getIndex() { return texIndex; }
    float getCoord() { return (float)texIndex/(float)texSize; }

private:

    int texIndex;

};

class HLSTexture  : public textureBaseClass
{
public:

    HLSTexture();
    ~HLSTexture();
    void buildTex(int size);


private:

};
extern HLSTexture hlsTexture;



class sigmaTextureClass  : public textureBaseClass
{
public:

    sigmaTextureClass();
    ~sigmaTextureClass();
    void buildTex(int size,float sigma);
    void rebuild(float sigma);


private:

};
//extern sigmaTextureClass hlsTexture;



class paletteTexClass : public textureBaseClass
{
public:

    paletteTexClass() {  /*texID=hlsTexture.getTexID();*/ }
    void buildTex(unsigned char *buffer, int size);
    void buildTex(float *buffer, int size);

};



class TextureView
{
public:

    TextureView(int x, int y, float r);
    void SetOrtho();    
    void End(int w, int h);

    void onReshape(int w, int h);
    void onReshape() { onReshape(winSize.x, winSize.y); }
    
    void setReduction(float r) { reduction = r; }
    float getReduction() { return reduction; }

    vec2& getWAspect() { return wAspect; }
    float getWAspectX() { return wAspect.x; }
    float getWAspectY() { return wAspect.y; }

    vec2& getTexInvSize() { return texInvSize; }
    float getTexInvSizeX() { return texInvSize.x; }
    float getTexInvSizeY() { return texInvSize.y; }

    ivec2& getTexSize() { return texSize; }
    int getTexSizeX() { return texSize.x; }
    int getTexSizeY() { return texSize.y; }

    void setTexSize(int x, int y) { texSize = ivec2(x,y); }




private:
    vec2 wAspect;     
    vec2 texInvSize;

    ivec2 winSize;
    ivec2 texSize;

    float reduction;

};



#endif //PARTICLES_UTILS_H