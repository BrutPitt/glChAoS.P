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
#pragma once
#include "appDefines.h"

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
    ScrnSht_CAPTURE_ALL = 4, //0x04
    ScrnSht_SILENT_MODE_ALPHA = 0x8,
    ScrnSht_FILE_NAME_ALPHA = 0x10
};

enum normalType { ptCoR, ptPt1, ptPt1CoR };

enum enumEmitterType { emitter_singleThread_externalBuffer,    // webGL and APPLE
                       emitter_separateThread_externalBuffer,  // max performance for OGL 4.1
                       emitter_separateThread_mappedBuffer};    // max performance for OGL 4.5

enum enumEmitterEngine { emitterEngine_staticParticles,
                         emitterEngine_transformFeedback };

enum glslPrecision { low, medium, hight };




#define GLAPP_PROG_NAME "glChAoS.P"

#define PALETTE_PATH "ColorMaps/"
#define STRATT_PATH "ChaoticAttractors/"
#define CAPTURE_PATH "imgsCapture/"
#define EXPORT_PLY_PATH CAPTURE_PATH
#define RENDER_CFG_PATH "renderCfg/"

#define GLAPP_PROG_CONFIG "glChAoSP.cfg"

class glWindow;

#include "ui/uiMainDlg.h"
#ifdef __EMSCRIPTEN__
    #include "emsTouch.h"
#endif

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
    #define PARTICLES_MAX 267000000
#else
    #error "Environment not 32 or 64-bit."
#endif


#ifdef NDEBUG
    #if !defined(GLCHAOSP_LIGHTVER)
        #define EMISSION_STEP (200*1024)
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

#define GET_TIME_FUNC glfwGetTime()

enum loadSettings { ignoreNone, ignoreCircBuffer, ignoreConfig=0x4 };


void exportPLY(bool wantBinary, bool wantColors, bool alphaDist, bool wantNormals, bool bCoR, bool wantNormalized, normalType nType = normalType::ptCoR);
bool importPLY(bool wantColors, int velType);


class particlesBaseClass;


class timerClass
{
public:
    timerClass() {  
#ifdef GLAPP_TIMER_RT_AVG
        memset((void *)fpsBuff, sizeof(fpsBuff), 0);
#endif
    }

    void init() { last = prev = startAVG = glfwGetTime(); }

    void tick() { 
        prev = last;  
        last = glfwGetTime();  
        count++;

#ifdef GLAPP_TIMER_RT_AVG
        const float dt = elapsed();
        fpsAccum += dt - fpsBuff[fpsIDX];
        fpsBuff[fpsIDX] = dt;
        fpsIDX++;
        framerate = (fpsAccum > 0.0f) ? (1.0f / (fpsAccum / 256.f)) : FLT_MAX;
#endif
    }

    float elapsed() { last = glfwGetTime(); return static_cast<float>(last-prev); }
    void start() { prev = last = glfwGetTime(); }

    void resetAVG() { startAVG = glfwGetTime(); count = 0; }

    float fps() { const float elaps = static_cast<float>(last-prev); return 1.f/elaps>0 ? elaps : FLT_EPSILON; }

    float fpsAVG() { return static_cast<float>(count/(last-startAVG)); }

private:
    double last = 0.0, prev = 0.0, startAVG = 0.0;
    long count = 0;
#ifdef GLAPP_TIMER_RT_AVG
    uint8_t fpsIDX = 0;
    float fpsBuff[256];
    float fpsAccum;
#endif
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
//GLFW Utils
/////////////////////////////////////////////////
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
    void getScreenShot(GLuint tex, bool is32bit=false);

    char const *openFile(char const *startDir, char const * patterns[], int numPattern);
    char const *saveFile(char const *startDir, char const * patterns[], int numPattern);
    void saveScreenShot(unsigned char *data, int w, int h, bool is32bit=false);

    void saveSettings(const char *name);
    bool loadSettings(const char *name, const int type = loadSettings::ignoreNone);
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

    unsigned getMaxAllocatedBuffer() { return maxAllocatedBuffer; }
    void setMaxAllocatedBuffer(unsigned v) { maxAllocatedBuffer = v; }
    unsigned getEmissionStepBuffer() { return emissionStepBuffer; }
    void setEmissionStepBuffer(unsigned v) { emissionStepBuffer = v; }

    bool isParticlesSizeConstant() { return particlesSizeConstant; }
    void setParticlesSizeConstant(bool b) { particlesSizeConstant = b; }

    bool useDetailedShadows() { return detailedShadows; }
    void useDetailedShadows(bool b) { detailedShadows = b; }

    std::string& getStartWithAttractorName() { return startWithAttractorName; }
    void setStartWithAttractorName(const std::string& s) { startWithAttractorName = s; }


    void setVSync(int v) { vSync = v; }
    int getVSync() { return vSync; }

    bool fullScreen() { return isFullScreen; }
    void fullScreen(bool b) { isFullScreen = b; }

    std::string &getCapturePath() { return capturePath; }
    void setCapturePath(const char * const s) { capturePath = s; }

    std::string &getPlyPath() { return exportPlyPath; }
    void setPlyPath(const char * const s) { exportPlyPath = s; }

    std::string &getRenderCfgPath() { return renderCfgPath; }
    void setRenderCfgPath(const char * const s) { renderCfgPath = s; }

    void   setPalInternalPrecision(GLenum e) { palInternalPrecision = e; }
    GLenum getPalInternalPrecision() { return  palInternalPrecision; }
    void   setTexInternalPrecision(GLenum e) { texInternalPrecision = e; }
    GLenum getTexInternalPrecision() { return  texInternalPrecision; }
    void   setFBOInternalPrecision(GLenum e) { fboInternalPrecision = e; }
    GLenum getFBOInternalPrecision() { return  fboInternalPrecision; }
    void   setDBInterpolation(GLenum e)      { dbInterpolation = e; }
    GLuint getDBInterpolation()      { return  dbInterpolation; }
    void   setFBOInterpolation(GLenum e)     { fboInterpolation = e; }
    GLuint getFBOInterpolation()     { return  fboInterpolation; }
    void   setShaderFloatPrecision(glslPrecision e)     { shaderFloatPrecision = e; }
    glslPrecision getShaderFloatPrecision()     { return  shaderFloatPrecision; }
    void   setShaderIntPrecision(glslPrecision e)       { shaderIntPrecision = e; }
    glslPrecision getShaderIntPrecision()       { return  shaderIntPrecision; }

    int getMultisamplingIdx() { return multisamplingIdx; }
    void setMultisamplingIdx(int v) { multisamplingIdx = v; }
    int getMultisamplingValue() { return fbMultisampling[multisamplingIdx]; }

    GLuint getClampToBorder() { return clampToBorder; }


    void setLowPrecision() {
        useLowPrecision(true);
        setTexInternalPrecision(GL_R16F);
        setPalInternalPrecision(GL_RGB16F);
        setFBOInternalPrecision(GL_RGBA16F);
        setDBInterpolation(GL_NEAREST);
        setFBOInterpolation(GL_LINEAR);
        setShaderFloatPrecision(glslPrecision::low);
        setShaderIntPrecision(glslPrecision::medium);
        multisamplingIdx = 3;
    }
    void setHighPrecision() {
        useLowPrecision(false);
        setTexInternalPrecision(GL_R32F);
        setPalInternalPrecision(GL_RGB32F);
        setFBOInternalPrecision(GL_RGBA32F);
        setDBInterpolation(GL_NEAREST);
        setFBOInterpolation(GL_LINEAR);
        setShaderFloatPrecision(glslPrecision::hight);
        setShaderIntPrecision(glslPrecision::hight);
        multisamplingIdx = 3;
    }

    // wgl command line settings
    bool isTabletMode() { return tabletMode; }
    void setTabletMode(bool b) { tabletMode=b; }
    bool useLowPrecision() { return lowPrecision; }
    void useLowPrecision(bool b) { lowPrecision = b; }
    bool useLightGUI() { return lightGUI; }
    void useLightGUI(bool b) {  lightGUI = b; }
    bool useFixedCanvas() { return fixedCanvas; }
    void useFixedCanvas(bool b) {  fixedCanvas = b; }

    bool startWithGlowOFF() { return initialGlowOFF; }
    void startWithGlowOFF(bool b) {  initialGlowOFF = b; }
    bool slowGPU() { return isSlowGPU; }
    void slowGPU(bool b) {  isSlowGPU = b; }
    bool needRestart() { return appNeedRestart; }
    void needRestart(bool b) {  appNeedRestart = b; }

    bool useSyncOGL() { return syncOGL; }
    void useSyncOGL(bool b) { syncOGL = b; }


    void selectFolder(std::string &s);

    
    mainImGuiDlgClass &getMainDlg() { return mainImGuiDlg; }

    std::vector<std::string> & getListQuickView() { return listQuickView; }
    void getQuickViewDirList();
    void loadQuikViewSelection(int idx);

    void selectedListQuickView(int i) { idxListQuickView = i; }
    int selectedListQuickView() { return idxListQuickView; }

    bool idleRotation() { return isIdleRotation; }
    void idleRotation(bool b) { isIdleRotation = b; }
// imGui utils
/////////////////////////////////////////////////
    void imguiInit();
    int imguiExit();

    void selectEmitterType(int type) {
#if !defined(GLCHAOSP_LIGHTVER)
    #ifdef GLAPP_REQUIRE_OGL45
        emitterType = type;
    #else
#if !defined(GLCHAOSP_NO_MB)
        emitterType = type == emitter_separateThread_mappedBuffer ? emitter_separateThread_externalBuffer : type;
#else
        emitterType = emitter_singleThread_externalBuffer;
#endif
    #endif
#endif
    }

    int getEmitterType() { return emitterType; }

    void setEmitterEngineType(int i) {
#if !defined(GLCHAOSP_NO_TF)
        emitterEngineType = i;
#else
        emitterEngineType = emitterEngine_staticParticles;
#endif
    }
    int  getEmitterEngineType() { return emitterEngineType; }

    timerClass& getTimer() { return timer; }

    int getMaxCombTexImgUnits() { return maxCombTexImgUnits; }
    bool checkMaxCombTexImgUnits() { return (maxCombTexImgUnits>32); }

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

    int maxCombTexImgUnits = 0;

private:
// imGui Dlg
/////////////////////////////////////////////////
    mainImGuiDlgClass mainImGuiDlg;

// glfw utils
/////////////////////////////////////////////////
    void glfwInit();
    int glfwExit();
    int getModifier();

    unsigned maxAllocatedBuffer = ALLOCATED_BUFFER;
    unsigned emissionStepBuffer = EMISSION_STEP;
    bool tabletMode = false;
    bool lightGUI = false;
    bool fixedCanvas = false;
    bool initialGlowOFF = false;
    bool detailedShadows = false;
    bool isIdleRotation = true;
    bool isSlowGPU = false;
    bool appNeedRestart = false;
#if defined(__APPLE__) || defined(GLCHAOSP_LIGHTVER) || defined(GLAPP_USES_ES3)
    bool syncOGL = true;
#else
    bool syncOGL = false;
#endif

    int emitterEngineType = emitterEngine_staticParticles;

    std::string startWithAttractorName = std::string("random"); // -1 Random start

    int screenShotRequest;
    int vSync = 0;
    bool isFullScreen = false;
#if defined(__EMSCRIPTEN__) || defined(GLCHAOSP_NO_TH)
    int emitterType = emitter_singleThread_externalBuffer;
#else
    int emitterType = emitter_separateThread_externalBuffer;
#endif

#if !defined(GL_CLAMP_TO_BORDER)
    const GLuint clampToBorder = GL_CLAMP_TO_EDGE;
#else
    const GLuint clampToBorder = GL_CLAMP_TO_BORDER;
#endif

    int fbMultisampling[6] = { GLFW_DONT_CARE, 0, 2, 4, 8, 16 };

//Values of this block can be overwritten from #define in constructor
    bool lowPrecision = false;
    GLenum fboInternalPrecision = GL_RGBA32F;
    GLenum palInternalPrecision = GL_RGB32F;
    GLenum texInternalPrecision = GL_R32F;
    GLuint dbInterpolation      = GL_NEAREST;
    GLuint fboInterpolation     = GL_LINEAR;
    glslPrecision shaderFloatPrecision = glslPrecision::hight;
    glslPrecision shaderIntPrecision   = glslPrecision::hight;
    int multisamplingIdx = 3;
////////////////////////////////

    std::string lastAttractor = std::string("");
    std::string capturePath = std::string(CAPTURE_PATH);
    std::string exportPlyPath = std::string(EXPORT_PLY_PATH);
    std::string renderCfgPath = std::string(RENDER_CFG_PATH);
    
    GLFWwindow* mainGLFWwnd = nullptr;
    glWindow *glEngineWnd = nullptr;

    timerClass timer;

#ifdef __EMSCRIPTEN__
public:
    emsMDeviceClass &getEmsDevice() { return emsDevice; }
    int getCanvasX() { return canvasX; }
    int getCanvasY() { return canvasY; }
    int getSizeX() { return GetWidth(); }
    int getSizeY() { return GetHeight(); }
    void setCanvasX(int i) { canvasX = i; }
    void setCanvasY(int i) { canvasY = i; }
private:
    emsMDeviceClass emsDevice;
    int canvasX=0, canvasY=0;
#endif
    

friend class glWindow;

};



