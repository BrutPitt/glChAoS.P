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
#pragma once
#include "appDefines.h"
//#include "glUtilities.h"

// For compilers that support precompilation, includes "wx/wx.h".
#include <string>
#include <sstream>
#include <vector>
#include <iomanip>
#include <iostream>
#include <GLFW/glfw3.h>



enum ScreeShotReq {
    ScrnSht_NO_REQUEST,     //0x00
    ScrnSht_SILENT_MODE,    //0x01
    ScrnSht_FILE_NAME,      //0x02
    ScrnSht_CAPTURE_ALL = 4 //0x04
};


#define GLAPP_PROG_NAME "glChAoS.P"

#define PALETTE_PATH "ColorMaps/"
#define STRATT_PATH "ChaoticAttractors/"
#define CAPTURE_PATH "imgsCapture/"
#define RENDER_CFG_PATH "renderCfg/"

#define GLAPP_PROG_CONFIG "glChAoSP.cfg"

class glWindow;

#include "ui/uiMainDlg.h"

bool fileExist(const char *filename);


/*
///////////////////////////////////////////
//Data Init for 32/64 bit systems
//////////////////////////////////////////

template<int> void IntDataHelper();

template<> void IntDataHelper<4>() 
{
  // do 32-bits operations
}

template<> void IntDataHelper<8>() 
{
  // do 64-bits operations
}

// helper function just to hide clumsy syntax
inline void IntData() { IntDataHelper<sizeof(size_t)>(); }
*/

#include <cstdint>
#if INTPTR_MAX == INT32_MAX
    #define PARTICLES_MAX 67000000
#elif INTPTR_MAX == INT64_MAX
    #define PARTICLES_MAX 105000000
#else
    #error "Environment not 32 or 64-bit."
#endif




#ifdef NDEBUG
    #if !defined(GLCHAOSP_LIGHTVER)
        #define EMISSION_STEP 100000
    #else
        #define EMISSION_STEP 20000
    #endif
#else
    #define EMISSION_STEP 7777
#endif

#if !defined(GLCHAOSP_LIGHTVER)
#define ALLOCATED_BUFFER 30000000
#define CIRCULAR_BUFFER  10000000
#else
#define ALLOCATED_BUFFER 10000000
#define CIRCULAR_BUFFER  5000000
#endif


/////////////////////////////////////////////////
// Initialize WX for main Dlg tools:
//    other thread  -> Not real main App


//
/////////////////////////////////////////////////

class particlesBaseClass;


class timerClass
{
public:
    timerClass() {  }

    void init() { last = prev = startAVG = glfwGetTime(); }

    void tick() { prev = last;  last = glfwGetTime();  count++; }

    void resetAVG() { startAVG = glfwGetTime(); count = 0; }

    float fps() { return static_cast<float>(1.0/(last-prev)); }

    float fpsAVG() { return static_cast<float>(count/(last-startAVG)); }

private:
    double last = 0.0, prev = 0.0, startAVG = 0.0;
    long count = 0;
};


/////////////////////////////////////////////////
// theApp -> Main App -> container
/////////////////////////////////////////////////


class mainGLApp
{
public:
    // self pointer .. static -> the only one 
    static mainGLApp* theMainApp;

    mainGLApp();
    ~mainGLApp();

    void onInit(int w = INIT_WINDOW_W, int h = INIT_WINDOW_H);
    int onExit();

    void mainLoop();
////////////////////////////////
//GLFW Utils
    GLFWwindow* getGLFWWnd()  const { return(mainGLFWwnd);  }
    

    glWindow *getEngineWnd() { return glEngineWnd; }
    void setGLFWWnd(GLFWwindow* wnd) { mainGLFWwnd = wnd; }

    //Only for initial position (save/Load)
    int getPosX() const { return xPosition; }
    int getPosY() const { return yPosition; }
    void setPosX(int v) { xPosition = v; }
    void setPosY(int v) { yPosition = v; }

	int GetWidth()     const { return width;     }
	int GetHeight()    const { return height;    }
    void SetWidth(int v)  { width  = v; }
    void SetHeight(int v) { height = v; }
	const char* getWindowTitle() const { return(windowTitle.c_str()); }

    void setWindowSize(int  w, int  h) { glfwSetWindowSize(mainGLFWwnd, w, h); }
    void getWindowSize(int *w, int *h) { glfwGetWindowSize(mainGLFWwnd, w, h); }

    //  Request for scrrenshot
    //////////////////////////////////////////////////////////////////
    void setScreenShotRequest(int val) { screenShotRequest = val; }
    void getScreenShot();

    char const *openFile(char const *startDir, char const * patterns[], int numPattern);
    char const *saveFile(char const *startDir, char const * patterns[], int numPattern);
    void saveScreenShot(unsigned char *data, int w, int h);

    void saveSettings(const char *name);
    bool loadSettings(const char *name);
    void saveAttractor(const char *name);
    bool loadAttractor(const char *name);
    void saveProgConfig();
    bool loadProgConfig();
    void invertSettings();

    void setLastFile(const char *s) { lastAttractor = s; }
    void setLastFile(const std::string &s) { lastAttractor = s; }
    std::string &getLastFile() { return lastAttractor; }

    std::string &get_glslVer() {return glslVersion; }
    std::string &get_glslDef() {return glslDefines; }

    int getMaxAllocatedBuffer() { return maxAllocatedBuffer; }
    void setMaxAllocatedBuffer(int v) { maxAllocatedBuffer = v; }

    bool isParticlesSizeConstant() { return particlesSizeConstant; }
    void isParticlesSizeConstant(bool b) { particlesSizeConstant = b; }

    void setVSync(int v) { vSync = v; }
    int getVSync() { return vSync; }

    bool fullScreen() { return isFullScreen; }
    void fullScreen(bool b) { isFullScreen = b; }

    std::string &getCapturePath() { return capturePath; }
    void setCapturePath(const char * const s) { capturePath = s; }

    void setPalInternalPrecision(GLenum e) { palInternalPrecision = e; }
    GLenum getPalInternalPrecision() { return palInternalPrecision; }
    void setTexInternalPrecision(GLenum e) { texInternalPrecision = e; }
    GLenum getTexInternalPrecision() { return texInternalPrecision; }
    void setFBOInternalPrecision(GLenum e) { fboInternalPrecision = e; }
    GLenum getFBOInternalPrecision() { return fboInternalPrecision; }

    bool useLowPrecision() { return lowPrecision; }
    void useLowPrecision(bool b) { lowPrecision = b; }

    void setLowPrecision() {
        useLowPrecision(true);
        theApp->setTexInternalPrecision(GL_R16F);
        theApp->setPalInternalPrecision(GL_RGB16F);
        theApp->setFBOInternalPrecision(GL_RGBA16F);
    }
    void setHighPrecision() {
        useLowPrecision(false);
        theApp->setTexInternalPrecision(GL_R32F);
        theApp->setPalInternalPrecision(GL_RGB32F);
        theApp->setFBOInternalPrecision(GL_RGBA32F);
    }



    void selectCaptureFolder();

    
    mainImGuiDlgClass &getMainDlg() { return mainImGuiDlg; }

    std::vector<std::string> & getListQuickView() { return listQuickView; }
    void getQuickViewDirList();
    void loadQuikViewSelection(int idx);

    void selectedListQuickView(int i) { idxListQuickView = i; }
    int selectedListQuickView() { return idxListQuickView; }

    timerClass& getTimer() { return timer; }


protected:

	// The Position of the window
	int xPosition, yPosition;
	int width, height;
	/** The title of the window */

    bool exitFullScreen;
    bool particlesSizeConstant = false;

	// The title of the window
	std::string windowTitle;
	std::string glslVersion;
    std::string glslDefines;

    std::vector<std::string> listQuickView;
    int idxListQuickView = 0;
    
    
private:
/////////////////////////////////////////////////
// imGui utils
    void imguiInit();
    int imguiExit();
    mainImGuiDlgClass mainImGuiDlg;

/////////////////////////////////////////////////
// glfw utils
    void glfwInit();
    int glfwExit();
    int getModifier();

    int maxAllocatedBuffer = ALLOCATED_BUFFER;

    int screenShotRequest;
    int vSync = 0;
    bool isFullScreen = false;

#if !defined(GLCHAOSP_USE_LOWPRECISION)
    bool lowPrecision = false;
    GLenum fboInternalPrecision = GL_RGBA32F;
    GLenum palInternalPrecision = GL_RGB32F;
    GLenum texInternalPrecision = GL_R32F;
#else
    bool lowPrecision = true;
    GLenum fboInternalPrecision = GL_RGBA16F;
    GLenum palInternalPrecision = GL_RGB16F;
    GLenum texInternalPrecision = GL_R16F;
#endif

    std::string lastAttractor = std::string("");
    std::string capturePath = std::string(CAPTURE_PATH);
    
    GLFWwindow* mainGLFWwnd = nullptr;
    glWindow *glEngineWnd = nullptr;

    timerClass timer;
    

friend class glWindow;

};



