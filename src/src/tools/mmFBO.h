//------------------------------------------------------------------------------
//  Copyright (c) 2018-2024 Michele Morrone
//  All rights reserved.
//
//  https://michelemorrone.eu - https://brutpitt.com
//
//  X: https://x.com/BrutPitt - GitHub: https://github.com/BrutPitt
//
//  direct mail: brutpitt(at)gmail.com - me(at)michelemorrone.eu
//
//  This software is distributed under the terms of the BSD 2-Clause license
//------------------------------------------------------------------------------
#ifndef _mm_FBO_
#define _mm_FBO_

#include <vector>
#include <vgMath.h>

//
//  FixMe: OpenGL 4.5 uses glTextureStorage2D and Textures are immutable
//         OpenGl 4.1 uses glTexImage2D then i can resize w/o destroy it (usefull in webGL=>Windows, for A.N.G.L.E. limitations)
//         I need to make this management more linear (possibility to use both strategies in 4.1/ES/WebGL)
//
//  Fixed??? (tests required!!!)
//

#ifdef GLAPP_REQUIRE_OGL45
    #define GLAPP_NOT_REUSES_FBO
#endif

// for OpenGL 4.1 / ES  and WebGL to destroy and recreate FBO need follow define
//#define GLAPP_NOT_REUSES_FBO

class mmFBO 
{
public:
    enum secondaryBufferType { depthBuffer, stencilBuffer, depthStencilBuffer };
    enum depthBufferType { depthTexture, depthBuiltIn };
    enum checkFBTypes { resize, initMain, multiFB, depthRB, rebuild };

    mmFBO()  { resetData(); }
    ~mmFBO() {
#if !defined(GLAPP_NOT_REUSES_FBO)
        deleteAll_FB_TEX();
#else
        deleteFBO();
#endif
    }

    void buildOnlyFBO(int num,int sizeX,int sizeY,GLenum intFormat);
    
    void declareFBO(int num, int sizeX, int sizeY, GLenum intFormat, GLuint format = GL_RGBA, int levelAA=0);
    void buildFBO  (int num, int sizeX, int sizeY, GLenum intFormat, GLuint format = GL_RGBA, int levelAA=0);
    void reBuildFBO(int num, int sizeX, int sizeY, GLenum intFormat, GLuint format = GL_RGBA, int levelAA=0);
    void reSizeFBO(int sizeX, int sizeY);
    void deleteFBO();
    void deleteAll_FB_TEX();

    GLuint getFB(int num)  { return num<m_NumFB ? m_fb[num] : -1; }
    GLuint getTex(int num=0) { return num<m_NumFB ? m_tex[num] : -1; }
    GLuint getRB(int num)  { return num<m_NumFB ? m_rb[num] : -1; }
    GLuint getDepth(int num) { return num<m_NumFB ? m_depth[num] : -1; }
    GLuint getTexMultiFB(int fbIDX, int multiIDX) { return fbIDX<m_NumFB && multiIDX<numMultiDraw ? multiDrawFB[fbIDX*numMultiDraw+multiIDX] : -1; }

    void attachDB   (bool builtIN, GLuint interpol = GL_NEAREST, GLuint clamp = GL_REPEAT) { attachSecondaryBuffer(builtIN, depthBuffer); }
    void attachSB   (bool builtIN, GLuint interpol = GL_NEAREST, GLuint clamp = GL_REPEAT) { attachSecondaryBuffer(builtIN, stencilBuffer); }
    void attachDB_SB(bool builtIN, GLuint interpol = GL_NEAREST, GLuint clamp = GL_REPEAT) { attachSecondaryBuffer(builtIN, depthStencilBuffer); };

    int getSizeX() { return m_sizeX; }
    int getSizeY() { return m_sizeY; }

    void attachMultiFB(int num);

private:
    void initFB(GLuint fbuff, GLuint iText, GLenum intFormat);
    void attachSecondaryBuffer(bool builtIN, secondaryBufferType type, 
                               GLuint interpol = GL_NEAREST, GLuint clamp = GL_CLAMP_TO_EDGE); //depthBuffer + Stencil
    void CheckFramebufferStatus(GLenum status);
    void CheckFramebufferStatus(GLuint fb, bool isFBBuilt, enum checkFBTypes fromWhere = resize);
#if defined(GLCHAOSP_LIGHTVER) /*|| defined(GLAPP_USES_ES3)*/
    void defineTexture(GLuint iTex, GLuint intFormat, GLuint format = GL_RGBA, GLuint type = GL_FLOAT, 
                       GLuint interp = GL_NEAREST, GLuint clamp = GL_CLAMP_TO_EDGE);
#else
    void defineTexture(GLuint iTex, GLuint intFormat, GLuint format = GL_RGBA, GLuint type = GL_FLOAT, 
                       GLuint interp = GL_LINEAR, GLuint clamp = GL_CLAMP_TO_EDGE);
#endif
    void resetData();

    int m_NumFB, aaLevel = 0;
    bool isBuilt = false;
    bool haveRB = false;
    bool isBuiltIn = true;
    bool isMultiBuilt = false;
    bool isDepthBuilt = false;
    bool haveColors = false;


    GLuint glPrecision, glFormat = GL_RGBA;

    GLuint *m_fb    = nullptr;
    GLuint *m_rb    = nullptr;
    GLuint *m_tex   = nullptr;
    GLuint *m_depth = nullptr;
    GLuint *multiDrawFB = nullptr;
    secondaryBufferType dsBufferType = depthBuffer;
    int numMultiDraw = 0;

    int m_sizeX, m_sizeY;
};


#endif