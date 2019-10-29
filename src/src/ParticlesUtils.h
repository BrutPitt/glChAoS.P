//------------------------------------------------------------------------------
//  Copyright (c) 2018-2019 Michele Morrone
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
#pragma once

#include <vector>

#include <vGizmoMath.h>

#include "glApp.h"
#include "glslProgramObject.h"
#include "glslShaderObject.h"


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

    RandomTexture() { srand( (unsigned)time( NULL ) ); }

    ~RandomTexture() {}
    void buildTex(int size);


    void incIndex() { if(++texIndex >= texSize) texIndex=0; }
    int  getIndex() { return texIndex; }
    float getCoord() { return (float)texIndex/(float)texSize; }

private:

    int texIndex = 0;
};

class HLSTexture  : public textureBaseClass
{
public:

    HLSTexture() {}
    ~HLSTexture() {}
    void buildTex(int size);
};

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

    TextureView(int x, int y, float r) : texSize(ivec2(x, y)), reduction(r) { onReshape(x, y); }

    void SetOrtho() {}    
    void End(int w, int h) {}

    void onReshape() { onReshape(winSize.x, winSize.y); }
    void onReshape(int w, int h) {
        winSize = ivec2(w,h);   
        texInvSize = vec2(1.0/(float)w,1.0/(float)h) / reduction;
        wAspect = (w<h) ? vec2(1.0,(float)h/(float)w) : 
                          vec2((float)w/(float)h, 1.0);
    }
    
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
        float Incr = 2.0f/float(size+1);
    
        Y = -1.0f+Incr;
        //float mmax = 0;
        for(int y=0; y<size; y++, Y+=Incr) {
            Y2=Y*Y;
            X = -1.0f+Incr;
            for(int x=0; x<size; x++, X+=Incr) {
                Dist = (float)sqrtf(X*X+Y2);
                //if (Dist>1) Dist=1;
                const float ev = evalHermite(hermVals,Dist); 
                *m++ = (Dist>=1.f) ? 0.f : typeSolid ? 1.f :  ev <= 0.0 ? 0.0 : ev * factor;
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
//  EvalHermite(float pA, float pB, float vA, float vB, float u)
//  Evaluates Hermite basis functions for the specified coefficients.
///////////////////////////////////////////////////////////////////////////////
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

//  Particles Texture
///////////////////////////////////////////////////////////////////////////////
class dotsTextureClass
{
public:
    enum { dotsAlpha, dotsSolid };

    dotsTextureClass()  {}

    void build(int shift, const vec4 &v, int t)
    {
#if !defined(GLCHAOSP_LIGHTVER)
        shiftBaseSize = shift;
#else
        shiftBaseSize = DOT_TEXT_SHFT;
#endif
        texSize = baseSize << shiftBaseSize; hermiteVals = v; dotType = t;
        build();
    }

    void rebuild(int shift, const vec4 &v, int t)
    {
#if !defined(GLCHAOSP_LIGHTVER)
        if(generated) {
            glDeleteTextures(1, &texID);
            generated = false;
        }
#endif
        build(shift, v, t);
    }

    void build()
    {
        gaussianMap<GLfloat> gMap(texSize);

        gMap.generateMap(hermiteVals, 1, dotType); 

        const int w = texSize, h = texSize;

        
    #ifdef GLAPP_REQUIRE_OGL45
        int level = 1;
        int sz = texSize;
        while(sz>>=1) level++;

        static int oldSZ = -1;

        if(generated && oldSZ!=texSize) { glDeleteTextures(1, &texID); generated = false; }
        if(!generated) {
            glCreateTextures(GL_TEXTURE_2D , 1, &texID);
            glTextureStorage2D(texID, level, theApp->getTexInternalPrecision(), w, h);
        }

        oldSZ = texSize;
        glTextureSubImage2D(texID, 0, 0, 0, w, h, GL_RED, GL_FLOAT, gMap.getBuffer());
        glGenerateTextureMipmap(texID);
        glTextureParameteri(texID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(texID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(texID, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(texID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    #else
        if(!generated)  glGenTextures(1, &texID);   // Generate OpenGL texture IDs
        glBindTexture(GL_TEXTURE_2D, texID);        // Bind Our Texture

        //glTexStorage2D(GL_TEXTURE_2D, 7, GL_R32F, w, h);
        //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RED, GL_FLOAT, buffer);
        glTexImage2D(GL_TEXTURE_2D, 0, theApp->getTexInternalPrecision(), w, h, 0, GL_RED, GL_FLOAT, gMap.getBuffer());

        CHECK_GL_ERROR();
        glGenerateMipmap(GL_TEXTURE_2D);
        CHECK_GL_ERROR();

        //glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
        //glBindTexture(GL_TEXTURE_2D, 0);
    #endif
        CHECK_GL_ERROR();
    
        generated = true;
    }

    int getDotsTexSize() { return texSize; }
    GLuint getTexID() { return texID; }

    void setDotType(int type) { if(dotType != type) rebuild(shiftBaseSize, hermiteVals, type); }
    int getDotType() { return dotType; }

    void setHermiteVals(const vec4 &v) { hermiteVals = v; }
    vec4& getHermiteVals() { return hermiteVals; }

    int getIndex() { return shiftBaseSize; }
    void setIndex(int shift) { if(shift!=shiftBaseSize) rebuild(shift, hermiteVals, dotType); } 

protected:
    bool generated = false;
    GLuint texID;
    int texSize = 32;
    const int baseSize = 32;
    int shiftBaseSize = 0;
    vec4 hermiteVals = vec4(.7f, 0.f, .3f, 0.f);
    int dotType;
};

