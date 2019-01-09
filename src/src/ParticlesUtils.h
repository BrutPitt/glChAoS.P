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

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include <emscripten/html5.h>
    #include <GLES3/gl3.h>
#else
#include <glad/glad.h>
#endif

#include <glm/glm.hpp>
#include <vector>

#include "glslProgramObject.h"
#include "glslShaderObject.h"


using namespace glm;


float* createGaussianMap(int N, const vec4 & hermVals, int components, int solid);

class textureBaseClass 
{
public:
    
    textureBaseClass() { }

    GLuint getTexID()   { return texID;   }
    GLuint getTexSize() { return texSize; }

    GLuint texID  ;
    GLuint texSize =  0;

protected:
    bool generated = false;
    void genTex();
    void assignAttribs(GLint filterMin, GLint filterMag, GLint wrap);

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



 
template <class T> class gaussianMap
{
public:
    
    gaussianMap(int s, float f = 1.0) 
    {
        size = s; totSize = s*s;
        factor = f;
        M = new T[totSize];
    }

    void generateMap(const vec4 & hermVals, int components, int typeSolid)
    {
        T *m = M;

        float X,Y,Y2,Dist;
        float Incr = 2.0f/float(size-1);
    
        Y = -1.0f;
        //float mmax = 0;
        for(int y=0; y<size; y++, Y+=Incr) {
            Y2=Y*Y;
            X = -1.0f;
            for(int x=0; x<size; x++, X+=Incr) {
                Dist = (float)sqrtf(X*X+Y2);
                //if (Dist>1) Dist=1;
                *m++ = (Dist>1) ? 0.f : (typeSolid ? 1.f : evalHermite(hermVals,Dist)) * factor;
            }
        }

        if(components>1) {
            T *B = new T[components*totSize];
            T *b = B;
                m = M;
                for(int k=components*totSize; k>0; k--)
                    *b++ = *m++; //(unsigned char)(M[i] * 255);
            delete [] M;
            M = B;
        };
    }

    ~gaussianMap() { delete [] M; }

    T* getBuffer() { return M; }

private:
//------------------------------------------------------------------------------
//  EvalHermite(float pA, float pB, float vA, float vB, float u)
//  Evaluates Hermite basis functions for the specified coefficients.
//------------------------------------------------------------------------------
    float evalHermite(const vec4 &v, float u)
    {
        const float u2=(u*u), u3=u2*u;

        const vec4 b( 2*u3 - 3*u2 + 1,
                     -2*u3 + 3*u2,
                        u3 - 2*u2 + u,
                        u3        - u);

        return dot(b,v);
    }

    T *M;
    int size, totSize;
    T factor;


};

class dotsTextureClass
{
public:
    enum { dotsAlpha, dotsSolid };

    dotsTextureClass()  
    {


    }

    // 
    //  Particles Texture
    ////////////////////////////////////////////////////////////////////////////
    void build(int size, const vec4 &v, int t)
    {
        texSize = size; hermiteVals = v; dotType = t;
        build();
    }

    void build()
    {

        gaussianMap<GLfloat> gMap(texSize);

    #if !defined(GLCHAOSP_USE_LOWPRECISION)
        const GLint texInternal = GL_R32F;
    #else
        const GLint texInternal = GL_R16F;
    #endif
    
        gMap.generateMap(hermiteVals, 1, dotType); 

        const int w = texSize, h = texSize;

        if(!generated)
    #ifdef GLAPP_REQUIRE_OGL45
            glCreateTextures(GL_TEXTURE_2D , 1, &texID);
        glTextureStorage2D(texID, 7, GL_R32F, w, h);
        glTextureSubImage2D(texID, 0, 0, 0, w, h, GL_RED, GL_FLOAT, gMap.getBuffer());
        glGenerateTextureMipmap(texID);
        glTextureParameteri(texID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(texID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(texID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(texID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    #else
            glGenTextures(1, &texID);					// Generate OpenGL texture IDs
        glBindTexture(GL_TEXTURE_2D, texID);			// Bind Our Texture

        //glTexStorage2D(GL_TEXTURE_2D, 7, GL_R32F, w, h);
        //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RED, GL_FLOAT, buffer);
        glTexImage2D(GL_TEXTURE_2D, 0, texInternal, w, h, 0, GL_RED, GL_FLOAT, gMap.getBuffer());
        glGenerateMipmap(GL_TEXTURE_2D);

        //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
        glBindTexture(GL_TEXTURE_2D, 0);
    #endif
        CHECK_GL_ERROR();
    
    }

    int getDotsTexSize() { return texSize; }
    GLuint getTexID() { return texID; }

    void setDotType(int type) { dotType = type; }
    int getDotType() { return dotType; }

    void setHermiteVals(const vec4 &v) { hermiteVals = v; }
    vec4& getHermiteVals() { return hermiteVals; }

protected:
    bool generated = false;
    GLuint texID;
    int texSize = 64;
    vec4 hermiteVals = vec4(.7f, 0.f, .3f, 0.f);
    int dotType;


};



#endif //PARTICLES_UTILS_H