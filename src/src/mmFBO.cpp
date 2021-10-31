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
#include <stdio.h>

#include "glApp.h"
#include "mmFBO.h"


void mmFBO::resetData()
{
    m_NumFB = 0;
    haveRB = false;

    isBuilt = false;
    m_sizeX = m_sizeY = 0;
    //multiDrawFB = m_depth = m_rb = m_fb = m_tex = nullptr;
    //m_rb = m_fb = nullptr;
    numMultiDraw = 0;
}

void mmFBO::deleteFBO()
{
    if(!isBuilt) return;

    //glDeleteFramebuffers(m_NumFB,m_fb);
    //if(haveColors) glDeleteTextures(m_NumFB, m_tex);
    //if(numMultiDraw) glDeleteTextures(numMultiDraw*m_NumFB, multiDrawFB);
    //if(haveRB) glDeleteRenderbuffers(m_NumFB, m_rb);
    //if(!isBuiltIn) glDeleteTextures(m_NumFB, m_depth);

    //delete [] m_fb;
    //delete [] m_tex;
    //delete [] m_rb;
    //delete [] m_depth;
    //delete [] multiDrawFB;

    resetData();
}

void mmFBO::reBuildFBO(int num, int sizeX, int sizeY, GLenum intFormat, GLuint format, int AA)
{
    bool tmpHaveRB = haveRB, tmpIsBuiltIn = isBuiltIn;
    int tmpNumMultiDraw = numMultiDraw;
    secondaryBufferType tmpBufferType = dsBufferType;

    deleteFBO();
    if(haveColors) buildFBO(num, sizeX, sizeY, intFormat, format, AA);
    else           buildOnlyFBO(num, sizeX, sizeY, intFormat);
    if(tmpHaveRB) attachSecondaryBuffer(tmpIsBuiltIn, tmpBufferType);
    if(tmpNumMultiDraw) attachMultiFB(tmpNumMultiDraw); 
}

void mmFBO::reSizeFBO(int sizeX, int sizeY)
{
    if(!isBuilt) return;

    int tmpNumFB = m_NumFB;
    bool tmpHaveRB = haveRB, tmpIsBuiltIn = isBuiltIn;
    int tmpNumMultiDraw = numMultiDraw;
    GLenum tmpPrecision = glPrecision;
    GLuint tmpFormat = glFormat;
    int tmpAA = aaLevel;
    secondaryBufferType tmpBufferType = dsBufferType;
    

    deleteFBO();
    if(haveColors) buildFBO(tmpNumFB,sizeX,sizeY,tmpPrecision,tmpFormat,tmpAA);
    else           buildOnlyFBO(tmpNumFB,sizeX,sizeY,tmpPrecision);
    if(tmpHaveRB) attachSecondaryBuffer(tmpIsBuiltIn, tmpBufferType);
    if(tmpNumMultiDraw) attachMultiFB(tmpNumMultiDraw); 
}

void mmFBO::initFB(GLuint fbuff, GLuint iText)
{
#ifdef GLAPP_REQUIRE_OGL45
    if(aaLevel>0) {
        glTextureStorage2DMultisample(iText, aaLevel, theApp->getFBOInternalPrecision(), m_sizeX, m_sizeY, GL_TRUE);
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
#ifdef GL_CLAMP_TO_BORDER
    if(clamp == GL_CLAMP_TO_BORDER) {
        float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTextureParameterfv(iTex, GL_TEXTURE_BORDER_COLOR, borderColor);
    }
#endif
#else
    //glActiveTexture(GL_TEXTURE0+iTex);
    glBindTexture(GL_TEXTURE_2D, iTex);
    glTexImage2D(GL_TEXTURE_2D, 0,  intFormat , m_sizeX, m_sizeY, 0, format, type, NULL);
    //glTexStorage2D(GL_TEXTURE_2D,1,intFormat,m_sizeX, m_sizeY);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interp);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interp);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, clamp);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, clamp);
#ifdef GL_CLAMP_TO_BORDER
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
    int totalMultiAttached = num*m_NumFB; // attaching "num" textures to any FB

    numMultiDraw = num;
    num++;
    
    GLuint *drawBuffers = new GLuint[num];

#ifdef GLAPP_REQUIRE_OGL45
    if(multiDrawFB == nullptr) {
        multiDrawFB = new GLuint[totalMultiAttached];
        glCreateTextures(GL_TEXTURE_2D ,totalMultiAttached, multiDrawFB);
    }
#else
    if(multiDrawFB == nullptr) {
        multiDrawFB = new GLuint[totalMultiAttached];
        glGenTextures(totalMultiAttached, multiDrawFB);
    }
#endif
    for (int i = 0; i < totalMultiAttached; i++) defineTexture(multiDrawFB[i], glPrecision);

    for(int countFB = 0; countFB<m_NumFB; countFB++) {
        for (int i = 0; i < num; i++) {
            drawBuffers[i] = GL_COLOR_ATTACHMENT0+i;

#ifdef GLAPP_REQUIRE_OGL45
            glNamedFramebufferTexture(m_fb[countFB], GL_COLOR_ATTACHMENT0+i, i==0 ? m_tex[countFB] : multiDrawFB[(countFB*numMultiDraw)+(i-1)], 0);
        }
        glNamedFramebufferDrawBuffers(m_fb[countFB], num, drawBuffers);
#else
            glBindFramebuffer(GL_FRAMEBUFFER, m_fb[countFB]);
    #if !defined(GLCHAOSP_LIGHTVER) && !defined(GLAPP_USES_ES3)
            glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i,i==0 ? m_tex[countFB] : multiDrawFB[(countFB*numMultiDraw)+(i-1)], 0); // for OpenGLES -> defined only in ES 3.2
    #else
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, GL_TEXTURE_2D, i==0 ? m_tex[countFB] : multiDrawFB[(countFB*numMultiDraw)+(i-1)], 0);
    #endif
        }
        glDrawBuffers(num, drawBuffers);
#endif
    }

    delete[] drawBuffers;

}

void mmFBO::attachSecondaryBuffer(bool builtIN, secondaryBufferType bufferType, GLuint interpol, GLuint clamp)
{
    haveRB = true;
    isBuiltIn = builtIN;
    dsBufferType = bufferType;

#ifdef GLAPP_REQUIRE_OGL45
        if(!isBuiltIn) {
            if(m_depth==nullptr)) {
                m_depth = new GLuint[m_NumFB];
                glCreateTextures(GL_TEXTURE_2D ,m_NumFB, m_depth);
            }
        }
        if(m_rb == nullptr) { m_rb = new GLuint[m_NumFB]; glCreateRenderbuffers(m_NumFB, m_rb); }
#else
        if(!isBuiltIn) {
            if(m_depth==nullptr) {
                m_depth = new GLuint[m_NumFB];
                glGenTextures(m_NumFB, m_depth);
            }
        }
        if(m_rb == nullptr) { m_rb = new GLuint[m_NumFB]; glGenRenderbuffers(m_NumFB, m_rb); }
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
#if !defined(GLCHAOSP_LIGHTVER) && !defined(GLAPP_USES_ES3)
            glFramebufferTexture(GL_FRAMEBUFFER, attach, m_depth[i], 0);  // for OpenGLES -> defined only in ES 3.2
#else
            glFramebufferTexture2D(GL_FRAMEBUFFER, attach, GL_TEXTURE_2D, m_depth[i], 0);
#endif
        }
#endif
    }
}

void mmFBO::buildOnlyFBO(int num, int sizeX, int sizeY, GLenum intFormat)
{
    glPrecision = intFormat;
    m_NumFB = num;
    m_sizeX = sizeX;
    m_sizeY = sizeY;
    haveColors = false;

    if(m_fb == nullptr) {
        m_fb    = new GLuint[num];
#ifdef GLAPP_REQUIRE_OGL45
        glCreateFramebuffers(num, m_fb);
#else
        glGenFramebuffers(num, m_fb);
#endif
    }
    isBuilt = true;
}

void mmFBO::declareFBO(int num, int sizeX, int sizeY, GLenum intFormat, GLuint format, int AA)
{
    glPrecision = intFormat;
    glFormat = format;
    m_NumFB = num;
    m_sizeX = sizeX;
    m_sizeY = sizeY;
    aaLevel = AA;
    haveColors = true;

}
void mmFBO::buildFBO(int num, int sizeX, int sizeY, GLenum intFormat, GLuint format, int AA)
{

    declareFBO(num,sizeX,sizeY,intFormat,AA);


    if(m_tex == nullptr) {
        m_tex = new GLuint[num];
#ifdef GLAPP_REQUIRE_OGL45
        glCreateTextures((aaLevel>0) ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D ,num, m_tex);
#else
        glGenTextures(num, m_tex);
#endif
    }

    if(m_fb ==nullptr) {
        m_fb = new GLuint[num];
#ifdef GLAPP_REQUIRE_OGL45
        glCreateFramebuffers(num, m_fb);
#else
        glGenFramebuffers(num, m_fb);
#endif
    }
    
    for(int i=0; i<num; i++) {
        initFB( m_fb[i], m_tex[i]);

#ifdef GLAPP_REQUIRE_OGL45
        CheckFramebufferStatus(glCheckNamedFramebufferStatus(m_fb[i], GL_FRAMEBUFFER));
#else
        CheckFramebufferStatus(glCheckFramebufferStatus(GL_FRAMEBUFFER));
#endif
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    isBuilt = true;
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
#ifdef GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER
        case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
            printf("Framebuffer incomplete, missing draw buffer\n");
            fprintf(stderr, "Framebuffer incomplete, missing draw buffer");
            break;
#endif
#ifdef GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER
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
