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
#include <stdlib.h>
#include <stdio.h>

#include "appDefines.h"
#include "mmFBO.h"



#define POSITION 0
#define TEXCOORD 4


ivec2 mmFBO::m_winSize = ivec2(0,0);
vec2 mmFBO::m_winAspect = vec2(0.f,0.f);
vec2 mmFBO::m_winInvSize = vec2(0.f,0.f);
float mmFBO::m_reduction = 1.f;


GLuint mmFBO::vboVertexBufferID = 0;
GLuint mmFBO::vboTexBufferID = 0;
GLuint mmFBO::vbaID = 0;


typedef struct { 
  vec2 vtxCoor;  
  vec2 texCoor;  
} Vertex;  


void mmFBO::Init(int w, int h)
{







   


/*
Vertex vtx[4] = { vec2(-1.0f,-1.0f), vec2(0.0f, 0.0f),
                  vec2( 1.0f,-1.0f), vec2(1.0f, 0.0f),
                  vec2( 1.0f, 1.0f), vec2(1.0f, 1.0f),
                  vec2(-1.0f, 1.0f), vec2(0.0f, 1.0f)
};

GLubyte index[4] = { 0, 1, 2, 3 };


    glGenBuffers( 1, &vboVertexBufferID );
	glBindBuffer( GL_ARRAY_BUFFER, vboVertexBufferID );
	glBufferData( GL_ARRAY_BUFFER, 4 * sizeof( Vertex ), vtx, GL_STATIC_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

    glGenBuffers(1, &vboTexBufferID);  
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboTexBufferID);  
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLubyte)*4, index, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
*/
    mmFBO::onReshape(w, h);
/*
    glGenVertexArrays(1, &vbaID);
    glBindVertexArray(vbaID);

    glBindBuffer(GL_ARRAY_BUFFER, vboVertexBufferID);
    //GLuint pos=glGetAttribLocation(programObject, "position");
    glVertexAttribPointer(POSITION, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char *)NULL+0);
    glVertexAttribPointer(TEXCOORD, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (char *)NULL+sizeof(vec2));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //glEnableVertexAttribArray(POSITION);
	//glEnableVertexAttribArray(TEXCOORD);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboTexBufferID);

    glBindVertexArray(0);
*/
}


mmFBO::mmFBO()
{


    resetData();

}


mmFBO::~mmFBO()
{

    deleteFBO();


}

void mmFBO::resetData()
{

    m_NumFB = 0;
    haveRB = false;

    isBuilded = false;
    m_sizeX = m_sizeY = 0;
    m_rb = m_fb = m_tex = NULL;

}


void mmFBO::onReshape(int w, int h)
{
    mmFBO::m_winSize = ivec2(w,h); 

    mmFBO::m_winInvSize = vec2(1.0/(float)w,1.0/(float)h) / mmFBO::m_reduction;

    mmFBO::m_winAspect = (w<h) ? vec2(1.0,(float)h/(float)w) : vec2((float)w/(float)h, 1.0);

    //mmFBO::m_winAspect = vec2(1.0, 1.0);
   
}




void mmFBO::deleteFBO()
{

    if(!isBuilded) return;

    glDeleteFramebuffers(m_NumFB,m_fb);
    glDeleteTextures(m_NumFB, m_tex);
    
    if(haveRB) glDeleteRenderbuffers(m_NumFB, m_rb);

    delete m_fb;
    delete m_tex;
    delete m_rb;

    resetData();
}

void mmFBO::reBuildFBO(int num, int sizeX, int sizeY, GLenum precision, bool rb, int AA)
{
    deleteFBO();
    buildFBO(num, sizeX, sizeY, precision, rb, AA);
}

void mmFBO::reSizeFBO(int sizeX, int sizeY)
{

    if(!isBuilded) return;

    int tmpNumFB = m_NumFB;
    bool tmpHaveRB = haveRB;
    GLenum tmpPrecision = glPrecision;
    int tmpAA = aaLevel;

    deleteFBO();
    buildFBO(tmpNumFB, sizeX, sizeY, tmpPrecision, tmpHaveRB, tmpAA);
}

void mmFBO::initFB(GLuint fbuff, GLuint iText)
{
/*
    glBindFramebuffer(GL_FRAMEBUFFER, fbuff);
    //
    // init texture
    //
    glActiveTexture(GL_TEXTURE0+iText);
    glBindTexture(GL_TEXTURE_2D, iText);
    glTexImage2D(GL_TEXTURE_2D, 0,  glPrecision , m_sizeX, m_sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    CHECK_GL_ERROR();
    //
    // SEEMS VERY IMPORTANT for the FBO to be valid. ARGL.
    //
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, iText, 0);
    CheckFramebufferStatus();
*/
    //glTexImage2D(GL_TEXTURE_2D, 0,  glPrecision , m_sizeX, m_sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
#ifdef GLAPP_REQUIRE_OGL45
    if(aaLevel>0) {
        glTextureStorage2DMultisample(iText, aaLevel, glPrecision, m_sizeX, m_sizeY, GL_TRUE);
        glNamedFramebufferTexture(fbuff, GL_COLOR_ATTACHMENT0, iText, 0); 
    }
    else {
        glTextureStorage2D(iText,1,glPrecision,m_sizeX, m_sizeY);    
        glTextureParameteri(iText, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(iText, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(iText, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(iText, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glNamedFramebufferTexture(fbuff, GL_COLOR_ATTACHMENT0, iText, 0);
    }
#else
    glBindFramebuffer(GL_FRAMEBUFFER, fbuff);
    glActiveTexture(GL_TEXTURE0+iText);
    glBindTexture(GL_TEXTURE_2D, iText);
    glTexImage2D(GL_TEXTURE_2D, 0,  glPrecision , m_sizeX, m_sizeY, 0, GL_RGBA, GL_FLOAT, NULL);
    //glTexStorage2D(GL_TEXTURE_2D,1,glPrecision,m_sizeX, m_sizeY);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, iText, 0);
#endif
    CheckFramebufferStatus(glCheckFramebufferStatus(GL_FRAMEBUFFER));
}

void mmFBO::attachRB(GLuint iRB )
{
            ////////////////////////////////////////
            // initialize depth renderbuffer
  
/*
            static int drawbuffer=GL_COLOR_ATTACHMENT0;
			glGenTextures(1, &m_rb);
			glBindTexture(GL_TEXTURE_2D, m_rb);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, sizeX, sizeY, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
            glFramebufferTexture2DEXT(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_rb, 0);
            glDrawBuffers(1, (const GLenum *)&drawbuffer);
*/

    //glGenRenderbuffersEXT(1, m_rb[index]);
    glBindRenderbuffer(GL_RENDERBUFFER, iRB);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, m_sizeX, m_sizeY);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, iRB);
    CheckFramebufferStatus(glCheckFramebufferStatus(GL_FRAMEBUFFER));
    
}


void mmFBO::buildMultiDrawFBO(int num, int sizeX, int sizeY, GLuint precision)
{
#if !defined(GLCHAOSP_LIGHTVER)
    glPrecision = precision;
    m_NumFB = num;
    m_sizeX = sizeX;
    m_sizeY = sizeY;

    m_fb  = new GLuint[num];
    m_tex = new GLuint[num];
    m_rb  = new GLuint[num];

    GLuint *drawBuffers = new GLuint[num];


    haveRB = false;

#ifdef GLAPP_REQUIRE_OGL45
    glCreateFramebuffers(1, m_fb);
    glCreateTextures(GL_TEXTURE_2D ,num, m_tex);
    for (int i = 0; i < num; i++) {
        glTextureStorage2D( m_tex[i],  1, glPrecision, m_sizeX, m_sizeY);
        glTextureParameteri(m_tex[i], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_tex[i], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(m_tex[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(m_tex[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        drawBuffers[i] = GL_COLOR_ATTACHMENT0+i;
        glNamedFramebufferTexture(m_fb[0], GL_COLOR_ATTACHMENT0+i, m_tex[i], 0);
    }
    glNamedFramebufferDrawBuffers(m_fb[0], num, drawBuffers);
    
#else
    glGenFramebuffers(1, m_fb);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fb[0]);
    glGenTextures(num, m_tex);
    for (int i = 0; i < num; i++) {
        // Bind and allocate storage for it   
        glBindTexture(GL_TEXTURE_2D, m_tex[i]);
        //glTexStorage2D(GL_TEXTURE_2D, 1, glPrecision, m_sizeX, m_sizeY);
        glTexImage2D(GL_TEXTURE_2D, 0,  glPrecision , m_sizeX, m_sizeY, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
        // Set its default filter parameters
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // Attach it to our framebuffer object as color attachments
        drawBuffers[i] = GL_COLOR_ATTACHMENT0+i;
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, m_tex[i], 0);
    }
    glDrawBuffers(num, drawBuffers);
#endif
    
    delete[] drawBuffers;
    isBuilded = true;
#endif
}


void mmFBO::buildFBO(int num, int sizeX, int sizeY, GLenum precision, bool zBuff, int AA)
{

    glPrecision = precision;
    m_NumFB = num;
    m_sizeX = sizeX;
    m_sizeY = sizeY;
    aaLevel = AA;

    m_fb  = new GLuint[num];
    m_tex = new GLuint[num];
    m_rb  = new GLuint[num];

    haveRB = zBuff;
    //
    // FBO
    //
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //glGenFramebuffers(num, m_fb);
    //glGenTextures(num, m_tex);
#ifdef GLAPP_REQUIRE_OGL45
    glCreateFramebuffers(num, m_fb);
    glCreateTextures((aaLevel>0) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D ,num, m_tex);
    if(haveRB) glCreateRenderbuffers(num, m_rb);
#else
    glGenFramebuffers(num, m_fb);
    glGenTextures(num, m_tex);
    if(haveRB) glGenRenderbuffers(num, m_rb);
#endif

    for(int i=0; i<num; i++) {
        initFB( m_fb[i], m_tex[i]);

        if(haveRB) {
            //attachRB(m_rb[i]);
#ifdef GLAPP_REQUIRE_OGL45
            if(aaLevel>0) glNamedRenderbufferStorageMultisample(m_rb[i], aaLevel, GL_DEPTH_COMPONENT32F, m_sizeX, m_sizeY);
            else          glNamedRenderbufferStorage(m_rb[i], GL_DEPTH_COMPONENT32F, m_sizeX, m_sizeY);

            glNamedFramebufferRenderbuffer(m_fb[i], GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rb[i]);
#else
            glBindRenderbuffer(GL_RENDERBUFFER, m_rb[i]);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, m_sizeX, m_sizeY);
            glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rb[i]);
#endif


            //glTextureStorage2D(m_rb[i],1,GL_DEPTH_COMPONENT32F,m_sizeX, m_sizeY);    
            //glNamedFramebufferTexture(m_fb[i], GL_DEPTH_ATTACHMENT, m_rb[i], 0);


        }
#ifdef GLAPP_REQUIRE_OGL45
        CheckFramebufferStatus(glCheckNamedFramebufferStatus(m_fb[i], GL_FRAMEBUFFER));
#else
        CheckFramebufferStatus(glCheckFramebufferStatus(GL_FRAMEBUFFER));
#endif

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    isBuilded = true;
}


void mmFBO::CheckFramebufferStatus(GLenum status)
{
#if !defined(NDEBUG)
    switch(status) {
        case GL_FRAMEBUFFER_COMPLETE:
            break;
        case GL_FRAMEBUFFER_UNSUPPORTED:
            printf("Unsupported framebuffer format\n");
            fprintf(stderr, "Unsupported framebuffer format");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            printf("Framebuffer incomplete, missing attachment\n");
            fprintf(stderr, "Framebuffer incomplete, missing attachment");
            break;

#if !defined(GLCHAOSP_LIGHTVER)
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            printf("Framebuffer incomplete, missing draw buffer\n");
            fprintf(stderr, "Framebuffer incomplete, missing draw buffer");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
            printf("Framebuffer incomplete, missing read buffer\n");
            fprintf(stderr, "Framebuffer incomplete, missing read buffer");
            break;
#endif
        default:
            printf("Framebuffer Not Complete, error %x\n", status);
            fprintf(stderr, "Framebuffer Not Complete, error %x\n", status);
			break;
    }
#endif
}
