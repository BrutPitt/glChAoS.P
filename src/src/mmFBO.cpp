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


void mmFBO::resetData()
{
    m_NumFB = 0;
    haveRB = false;

    isBuilded = false;
    m_sizeX = m_sizeY = 0;
    multiDrawFB = m_depth = m_rb = m_fb = m_tex = nullptr;
    numMultiDraw = 0;
}


void mmFBO::onReshape(int w, int h)
{
    mmFBO::m_winSize = ivec2(w,h); 

    mmFBO::m_winInvSize = vec2(1.0/(float)w,1.0/(float)h) / mmFBO::m_reduction;

    mmFBO::m_winAspect = (w<h) ? vec2(1.0,(float)h/(float)w) : vec2((float)w/(float)h, 1.0);
}

void mmFBO::deleteFBO()
{
    if(!isBuilded) return;

    glDeleteFramebuffers(m_NumFB,m_fb);
    if(haveColors) glDeleteTextures(m_NumFB, m_tex);
    if(numMultiDraw) glDeleteTextures(numMultiDraw, multiDrawFB);
    if(haveRB) glDeleteRenderbuffers(m_NumFB, m_rb);
    if(!isBuiltIn) glDeleteTextures(m_NumFB, m_depth);

    delete [] m_fb;
    delete [] m_tex;
    delete [] m_rb;
    delete [] m_depth;
    delete [] multiDrawFB;

    resetData();
}

void mmFBO::reBuildFBO(int num, int sizeX, int sizeY, GLenum precision, int AA)
{
    bool tmpHaveRB = haveRB, tmpIsBuiltIn = isBuiltIn;
    int tmpNumMultiDraw = numMultiDraw;
    secondaryBufferType tmpBufferType = dsBufferType;

    deleteFBO();
    if(haveColors) buildFBO(num, sizeX, sizeY, precision, AA);
    else           buildOnlyFBO(num, sizeX, sizeY, precision);
    if(tmpHaveRB) attachSecondaryBuffer(tmpIsBuiltIn, tmpBufferType);
    if(tmpNumMultiDraw) attachMultiFB(tmpNumMultiDraw); 
}

void mmFBO::reSizeFBO(int sizeX, int sizeY)
{
    if(!isBuilded) return;

    int tmpNumFB = m_NumFB;
    bool tmpHaveRB = haveRB, tmpIsBuiltIn = isBuiltIn;
    int tmpNumMultiDraw = numMultiDraw;
    GLenum tmpPrecision = glPrecision;
    int tmpAA = aaLevel;
    secondaryBufferType tmpBufferType = dsBufferType;
    

    deleteFBO();
    if(haveColors) buildFBO(tmpNumFB,sizeX,sizeY,tmpPrecision,tmpAA);
    else           buildOnlyFBO(tmpNumFB,sizeX,sizeY,tmpPrecision);
    if(tmpHaveRB) attachSecondaryBuffer(tmpIsBuiltIn, tmpBufferType);
    if(tmpNumMultiDraw) attachMultiFB(tmpNumMultiDraw); 
}

void mmFBO::initFB(GLuint fbuff, GLuint iText)
{
#ifdef GLAPP_REQUIRE_OGL45
    if(aaLevel>0) {
        glTextureStorage2DMultisample(iText, aaLevel, glPrecision, m_sizeX, m_sizeY, GL_TRUE);
        glNamedFramebufferTexture(fbuff, GL_COLOR_ATTACHMENT0, iText, 0); 
    }
    else {
        defineTexture(iText, glPrecision);
        glNamedFramebufferTexture(fbuff, GL_COLOR_ATTACHMENT0, iText, 0);
    }
#else
    glBindFramebuffer(GL_FRAMEBUFFER, fbuff);
    defineTexture(iText, glPrecision);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, iText, 0);
#endif
    CheckFramebufferStatus(glCheckFramebufferStatus(GL_FRAMEBUFFER));
}


void mmFBO::defineTexture(GLuint iTex, GLuint intFormat, GLuint format, GLuint type, GLuint interp, GLuint clamp)
{
#ifdef GLAPP_REQUIRE_OGL45
    glTextureStorage2D(iTex,1,intFormat,m_sizeX, m_sizeY);    
    glTextureParameteri(iTex, GL_TEXTURE_MIN_FILTER, interp);
    glTextureParameteri(iTex, GL_TEXTURE_MAG_FILTER, interp);
    glTextureParameteri(iTex, GL_TEXTURE_WRAP_S, clamp);
    glTextureParameteri(iTex, GL_TEXTURE_WRAP_T, clamp);
    if(clamp == GL_CLAMP_TO_BORDER) {
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTextureParameterfv(iTex, GL_TEXTURE_BORDER_COLOR, borderColor);
    }
#else
    //glActiveTexture(GL_TEXTURE0+iTex);
    glBindTexture(GL_TEXTURE_2D, iTex);
    glTexImage2D(GL_TEXTURE_2D, 0,  intFormat , m_sizeX, m_sizeY, 0, format, type, NULL);
    //glTexStorage2D(GL_TEXTURE_2D,1,intFormat,m_sizeX, m_sizeY);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interp);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interp);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp);
#if !defined(GLCHAOSP_LIGHTVER)
    if(clamp == GL_CLAMP_TO_BORDER) {
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    }
#endif
    glBindTexture(GL_TEXTURE_2D, 0);

#endif
}

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
/*
    //glGenRenderbuffersEXT(1, m_rb[index]);
    glBindRenderbuffer(GL_RENDERBUFFER, iRB);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, m_sizeX, m_sizeY);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, iRB);
    CheckFramebufferStatus(glCheckFramebufferStatus(GL_FRAMEBUFFER));
*/
/*
    glBindTexture(GL_TEXTURE_2D, m_depthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, m_sizeX, m_sizeY, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthTex, 0);
    glDrawBuffers(1, &drawBuffers);
*/

    //GLuint drawBuffers = GL_COLOR_ATTACHMENT0;


void mmFBO::attachMultiFB(int num)
{
    multiDrawFB = new GLuint[num];
    numMultiDraw = num;
    
    num++;
    GLuint *drawBuffers = new GLuint[num];

#ifdef GLAPP_REQUIRE_OGL45
    glCreateTextures(GL_TEXTURE_2D ,numMultiDraw, multiDrawFB);
    for (int i = 0; i < numMultiDraw; i++) defineTexture(multiDrawFB[i], glPrecision);

    for (int i = 0; i < num; i++) {
        drawBuffers[i] = GL_COLOR_ATTACHMENT0+i;
        glNamedFramebufferTexture(m_fb[0], GL_COLOR_ATTACHMENT0+i, i==0 ? m_tex[0] : multiDrawFB[i-1], 0);
    }
    glNamedFramebufferDrawBuffers(m_fb[0], num, drawBuffers);
#else
    glBindFramebuffer(GL_FRAMEBUFFER, m_fb[0]);
    glGenTextures(num, multiDrawFB);
    // Bind and allocate storage for it   
    for (int i = 0; i < numMultiDraw; i++) defineTexture(multiDrawFB[i], glPrecision);
    for (int i = 0; i < num; i++) {
        // Attach it to our framebuffer object as color attachments
        drawBuffers[i] = GL_COLOR_ATTACHMENT0+i;
#if !defined(GLCHAOSP_LIGHTVER)
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i,i==0 ? m_tex[0] : multiDrawFB[i-1], 0);
#else
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, i==0 ? m_tex[0] : multiDrawFB[i-1], 0);
#endif
    }
    glDrawBuffers(num, drawBuffers);
#endif
    delete[] drawBuffers;

}

void mmFBO::attachSecondaryBuffer(bool builtIN, secondaryBufferType bufferType, GLuint interpol, GLuint clamp)
{
    haveRB = true;
    isBuiltIn = builtIN;
    m_rb    = new GLuint[m_NumFB];
    dsBufferType = bufferType;

#ifdef GLAPP_REQUIRE_OGL45
    glCreateRenderbuffers(m_NumFB, m_rb);
    if(!isBuiltIn) { m_depth = new GLuint[m_NumFB]; glCreateTextures(GL_TEXTURE_2D ,m_NumFB, m_depth); }
#else
    glGenRenderbuffers(m_NumFB, m_rb);
    if(!isBuiltIn) { m_depth = new GLuint[m_NumFB]; glGenTextures(m_NumFB, m_depth); }
#endif

    GLuint intFormat, attach, format, type;
    if(bufferType == secondaryBufferType::depthBuffer) {
        intFormat = GL_DEPTH_COMPONENT32F; attach = GL_DEPTH_ATTACHMENT;
        format = GL_DEPTH_COMPONENT; type = GL_FLOAT;
    } else if(bufferType == secondaryBufferType::stencilBuffer) {
        intFormat = GL_DEPTH24_STENCIL8; attach = GL_DEPTH_STENCIL_ATTACHMENT;
        format = GL_DEPTH_STENCIL; type = GL_UNSIGNED_INT_24_8;
    } else { // depthStencilBuffer
        intFormat = GL_DEPTH32F_STENCIL8; attach = GL_DEPTH_STENCIL_ATTACHMENT;
        format = GL_DEPTH_STENCIL; type = GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
    }

    for(int i=0; i<m_NumFB; i++) {
#ifdef GLAPP_REQUIRE_OGL45
        if(isBuiltIn) {
            if(aaLevel>0) glNamedRenderbufferStorageMultisample(m_rb[i], aaLevel, intFormat, m_sizeX, m_sizeY);
            else          glNamedRenderbufferStorage(m_rb[i], intFormat, m_sizeX, m_sizeY);

            glNamedFramebufferRenderbuffer(m_fb[i], attach, GL_RENDERBUFFER, m_rb[i]);
        } else {
            defineTexture(m_depth[i], intFormat, format, type, interpol, clamp);
            glNamedFramebufferTexture(m_fb[i], attach, m_depth[i], 0);
        }
#else
        glBindFramebuffer(GL_FRAMEBUFFER, m_fb[i]);
        if(isBuiltIn) {
            glBindRenderbuffer(GL_RENDERBUFFER, m_rb[i]);
            glRenderbufferStorage(GL_RENDERBUFFER, intFormat, m_sizeX, m_sizeY);
            glFramebufferRenderbuffer( GL_FRAMEBUFFER, attach, GL_RENDERBUFFER, m_rb[i]);
        } else {
            defineTexture(m_depth[i], intFormat, format, type, interpol, clamp);
#if !defined(GLCHAOSP_LIGHTVER)
            glFramebufferTexture(GL_FRAMEBUFFER, attach, m_depth[i], 0);
#else
            glFramebufferTexture2D(GL_FRAMEBUFFER, attach, GL_TEXTURE_2D, m_depth[i], 0);
#endif
        }
#endif
    }
}

void mmFBO::buildOnlyFBO(int num, int sizeX, int sizeY, GLenum precision)
{
    glPrecision = precision;
    m_NumFB = num;
    m_sizeX = sizeX;
    m_sizeY = sizeY;
    haveColors = false;

    m_fb    = new GLuint[num];

#ifdef GLAPP_REQUIRE_OGL45
    glCreateFramebuffers(num, m_fb);
#else
    glGenFramebuffers(num, m_fb);
#endif
    isBuilded = true;
}

void mmFBO::declareFBO(int num, int sizeX, int sizeY, GLenum precision, int AA)
{
    glPrecision = precision;
    m_NumFB = num;
    m_sizeX = sizeX;
    m_sizeY = sizeY;
    aaLevel = AA;
    haveColors = true;

}
void mmFBO::buildFBO(int num, int sizeX, int sizeY, GLenum precision, int AA)
{

    declareFBO(num,sizeX,sizeY,precision,AA);

    m_fb    = new GLuint[num];
    m_tex   = new GLuint[num];

#ifdef GLAPP_REQUIRE_OGL45
    glCreateFramebuffers(num, m_fb);
    glCreateTextures((aaLevel>0) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D ,num, m_tex);
#else
    glGenFramebuffers(num, m_fb);
    glGenTextures(num, m_tex);
#endif
    
    for(int i=0; i<num; i++) {
        initFB( m_fb[i], m_tex[i]);

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
