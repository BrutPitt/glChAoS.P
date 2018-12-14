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
#ifndef _mm_FBO_
#define _mm_FBO_

#include "glm/vec2.hpp"

//#include "GPU.h"

using namespace glm;

struct fboBuffers
{
    GLuint  m_fb;		
    GLuint  m_rb;	
    GLuint  m_tex;
};

class mmFBO 
{
public:
    
    mmFBO();
    ~mmFBO();

    void buildFBO(int num, int sizeX, int sizeY, bool zBuff=false, int levelAA=0, GLuint precision = GL_RGBA32F);
    void reBuildFBO(int num, int sizeX, int sizeY, bool zBuff=false, int levelAA=0, GLuint precision = GL_RGBA32F);
    void buildMultiDrawFBO(int num, int sizeX, int sizeY, GLuint precision = GL_RGBA32F);
    void reSizeFBO(int sizeX, int sizeY);
    void deleteFBO();

    GLuint getFB(int num)  { return num<m_NumFB ? m_fb[num] : -1; }
    GLuint getTex(int num) { return num<m_NumFB ? m_tex[num] : -1; }
    GLuint getRB(int num) { return num<m_NumFB ? m_rb[num] : -1; }

    int getSizeX() { return m_sizeX; }
    int getSizeY() { return m_sizeY; }


// static members
    static void onReshape() { onReshape(mmFBO::m_winSize.x, mmFBO::m_winSize.y); }

    static void onReshape(int w, int h);
    static void DrawQuads();
    static void Init(int w, int h);

    static ivec2 m_winSize;
    static vec2 m_winAspect;     
    static vec2 m_winInvSize;
    static float m_reduction;

static GLuint vboVertexBufferID;
static GLuint vboTexBufferID;
static GLuint vbaID;


private:
    void initFB(GLuint fbuff, GLuint iText);
    void attachRB(GLuint iRB );
    void CheckFramebufferStatus();
    void resetData();

    int m_NumFB, aaLevel = 4;
    bool isBuilded;
    bool haveRB;

    GLuint glPrecision;

    //fboBuffers *fbo;

    GLuint  *m_fb;		
    GLuint  *m_rb;	
    GLuint  *m_tex;

    int m_sizeX, m_sizeY;




};


#endif