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
#include <array>
#include <vector>
#include <ostream>
                
#include "glApp.h"
#include "glWindow.h"

#include "ShadersClasses.h"

#if !defined (__EMSCRIPTEN__)
    GLFWmonitor* getCurrentMonitor(GLFWwindow *window);
    void toggleFullscreenOnOff(GLFWwindow* window);
    bool isDoubleClick(int button, int action, double x, double y, double ms);
#endif


static void glfwErrorCallback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

void glfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    if(ImGui::GetIO().WantCaptureKeyboard) return;

    if(action == GLFW_PRESS) {

        theWnd->onSpecialKeyDown(key, 0, 0);
        switch(key) {
            case GLFW_KEY_ESCAPE : theDlg.visible(theDlg.visible()^1); break;
            case GLFW_KEY_SPACE  : attractorsList.restart();           break;

            case GLFW_KEY_F1 : theDlg.aboutDlg.toggleVisible();       break;
            case GLFW_KEY_F2 : theDlg.attractorDlg.toggleVisible();   break;
            case GLFW_KEY_F3 : theDlg.particlesDlg.toggleVisible();   break;
            case GLFW_KEY_F4 : theDlg.imGuIZMODlg.toggleVisible();    break;
            case GLFW_KEY_F5 : theDlg.viewSettingDlg.toggleVisible(); break;
            case GLFW_KEY_F6 : theDlg.infoDlg.toggleVisible();        break;
#if !defined(GLCHAOSP_LIGHTVER)
            case GLFW_KEY_F7 : theDlg.dataDlg.toggleVisible();        break;
            case GLFW_KEY_F8 : theDlg.progSettingDlg.toggleVisible(); break;
            case GLFW_KEY_I  : theApp->invertSettings();              break;  
#else 
            case GLFW_KEY_I  : 
                theDlg.setInvertSettings(theDlg.getInvertSettings()^1);
                if(theApp->getLastFile().size()>0) loadAttractorFile(false, theApp->getLastFile().c_str());
            break;  
#endif
#if !defined (__EMSCRIPTEN__)
            case GLFW_KEY_F11: toggleFullscreenOnOff(window);         break;
#endif
            case GLFW_KEY_DOWN :  
            case GLFW_KEY_D    :  {
                int idx = attractorsList.getSelection();
                attractorsList.setSelection((idx >= attractorsList.getList().size()-1) ? 0 : ++idx);
                } break;
            case GLFW_KEY_UP :  
            case GLFW_KEY_U  :  {
                int idx = attractorsList.getSelection();
                attractorsList.setSelection((idx <= 0) ? attractorsList.getList().size()-1 : --idx);
                } break;
            case GLFW_KEY_RIGHT :  
            case GLFW_KEY_R     :  {
                int idx = theApp->selectedListQuickView();
                theApp->loadQuikViewSelection((idx >= theApp->getListQuickView().size()-1) ? 0 : ++idx);
                } break;
            case GLFW_KEY_LEFT :  
            case GLFW_KEY_L    :  {
                int idx = theApp->selectedListQuickView();
                theApp->loadQuikViewSelection((idx <= 0) ? theApp->getListQuickView().size()-1 : --idx);
                } break;
            case GLFW_KEY_PRINT_SCREEN :  
                if(mods & GLFW_MOD_CONTROL) // CTRL+PrtScr -> request FileName
                    theApp->setScreenShotRequest(ScreeShotReq::ScrnSht_FILE_NAME);
                if(mods & GLFW_MOD_SHIFT) // SHIFT+PrtScr -> silent capture
                    theApp->setScreenShotRequest(ScreeShotReq::ScrnSht_SILENT_MODE);
                if((mods & GLFW_MOD_ALT) || (mods & GLFW_MOD_SUPER)) // ALT+PrtScr -> capture also GUI
                    theApp->setScreenShotRequest(ScreeShotReq::ScrnSht_CAPTURE_ALL);
                break;
            default:
                break;

        }
    }
}

void glfwMouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{   

    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    if(ImGui::GetIO().WantCaptureMouse) return;

    double x,y;
    glfwGetCursorPos(window, &x, &y);    

    if (action == GLFW_PRESS) {
#if !defined (__EMSCRIPTEN__)
        if(isDoubleClick(button, action, x , y, 300)) toggleFullscreenOnOff(window);
        else 
#endif
            theWnd->onMouseButton(button, APP_MOUSE_BUTTON_DOWN, x, y); 
        //getApp()->LeftButtonDown();
            
    } else if (action == GLFW_RELEASE) {        
#if !defined (__EMSCRIPTEN__)
        isDoubleClick(button, action, x , y, 300);
#endif
        theWnd->onMouseButton(button, APP_MOUSE_BUTTON_UP, x, y); 
        
        //getApp()->LeftButtonUp();
    }
}

static void glfwCharCallback(GLFWwindow* window, unsigned int c)
{
    ImGui_ImplGlfw_CharCallback(window, c);
    if(ImGui::GetIO().WantCaptureKeyboard) return;
}

void glfwScrollCallback(GLFWwindow* window, double x, double y)
{
#if !defined (__EMSCRIPTEN__)
    ImGui_ImplGlfw_ScrollCallback(window, x, y);
#else
    ImGui_ImplGlfw_ScrollCallback(window, x, -y);
#endif
    if(ImGui::GetIO().WantCaptureMouse) return;
    theWnd->onMouseWheel(0, 0, int(x), int(y));
}

static void glfwMousePosCallback(GLFWwindow* window, double x, double y)
{
    if((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)  == GLFW_PRESS) || 
       (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) )
        theWnd->onMotion(x, y); 
}


void glfwWindowSizeCallback(GLFWwindow* window, int width, int height)
{
    theWnd->onReshape(width,height);
}

#if !defined (__EMSCRIPTEN__)

bool isDoubleClick(int button, int action, double x, double y, double ms)
{
    static auto before = std::chrono::system_clock::now();
    static int oldButton = -1;
    static double oldx=-1, oldy=-1;    

    auto now = std::chrono::system_clock::now();
    double diff_ms = std::chrono::duration <double, std::milli> (now - before).count();

    bool retval = (diff_ms<ms && (oldx==x && oldy==y) && (action==GLFW_PRESS) && 
                  (oldButton == button && button == GLFW_MOUSE_BUTTON_LEFT));
        
    if(action==GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) before = now ;    
    oldButton = button;
    oldx=x, oldy=y;

    return retval;
}

void toggleFullscreenOnOff(GLFWwindow* window)
{
    static int windowed_xpos, windowed_ypos, windowed_width, windowed_height;
                
    if (glfwGetWindowMonitor(window)) { //Windowed
        glfwSetWindowMonitor(window, NULL, windowed_xpos,  windowed_ypos, windowed_width, windowed_height, 0);
#ifdef GLAPP_IMGUI_VIEWPORT
        theApp->fullScreen(false);
        theDlg.switchMode(-windowed_xpos, -windowed_ypos);
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; 
        ImGui::UpdatePlatformWindows();
#endif        
    } else { //FullScreen    
        GLFWmonitor* monitor = getCurrentMonitor(window);
        if (monitor)
        {
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            glfwGetWindowPos(window,  &windowed_xpos,  &windowed_ypos);
            glfwGetWindowSize(window, &windowed_width, &windowed_height);
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        }
#ifdef GLAPP_IMGUI_VIEWPORT
        theApp->fullScreen(true);
        theDlg.switchMode(windowed_xpos, windowed_ypos);
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable; 
#endif        
    }
    glfwSwapInterval(theApp->getVSync());
}

GLFWmonitor* getCurrentMonitor(GLFWwindow *window)
{
    int nmonitors, i;
    int wx, wy, ww, wh;
    int mx, my, mw, mh;
    int overlap, bestoverlap;
    GLFWmonitor *bestmonitor;
    GLFWmonitor **monitors;
    const GLFWvidmode *mode;

    bestoverlap = 0;
    bestmonitor = NULL;

    glfwGetWindowPos(window, &wx, &wy);
    glfwGetWindowSize(window, &ww, &wh);
    monitors = glfwGetMonitors(&nmonitors);

    for (i = 0; i < nmonitors; i++) {
        mode = glfwGetVideoMode(monitors[i]);
        glfwGetMonitorPos(monitors[i], &mx, &my);
        mw = mode->width;
        mh = mode->height;

        overlap =
            std::max(0, std::min(wx + ww, mx + mw) - std::max(wx, mx)) *
            std::max(0, std::min(wy + wh, my + mh) - std::max(wy, my));

        if (bestoverlap < overlap) {
            bestoverlap = overlap;
            bestmonitor = monitors[i];
        }
    }
    return bestmonitor;
}
#endif

// Interface
/////////////////////////////////////////////////

// Set the application to null for the linker
mainGLApp* mainGLApp::theMainApp = 0;


/////////////////////////////////////////////////
// ImGui utils
void mainGLApp::imguiInit()
{
    // Setup ImGui binding
    ImGui::CreateContext();

#ifdef GLAPP_IMGUI_VIEWPORT
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    //io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcon;
#endif
    //ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(mainGLFWwnd, false);
    ImGui_ImplOpenGL3_Init(get_glslVer().c_str());
    //ImGui::StyleColorsDark();
    setGUIStyle();
}

int mainGLApp::imguiExit()
{
// need to test exit wx ... now 0!
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}

void mainGLApp::getScreenShot() 
{
    const int w = glEngineWnd->getParticlesSystem()->getWidth(), h = glEngineWnd->getParticlesSystem()->getHeight();
    const int rowDim = w*3;
    unsigned char *buffer = new unsigned char [rowDim*h], *flipped = new unsigned char [rowDim*h];
    unsigned char *tmpBuff = buffer, *tmpFlip = flipped+rowDim*(h-1);

        
    glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, buffer);
    while(tmpFlip >= flipped) {
        memcpy(tmpFlip, tmpBuff, rowDim);
        tmpFlip-=rowDim; 
        tmpBuff+=rowDim;
    }
    theApp->saveScreenShot(flipped, w, h);

    delete[] buffer, flipped;
}


GLFWwindow *secondary;
/////////////////////////////////////////////////
// glfw utils
void mainGLApp::glfwInit()
{
    glfwSetErrorCallback(glfwErrorCallback);

    if (!::glfwInit()) exit(EXIT_FAILURE);
       
#if !defined (__EMSCRIPTEN__)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    #ifdef GLAPP_REQUIRE_OGL45
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glslVersion = "#version 450\n";
        glslDefines = "#define LAYOUT_BINDING(X) layout (binding = X)\n"
                      "#define LAYOUT_INDEX(X) layout(index = X)\n"
                      "#define LAYOUT_LOCATION(X) layout(location = X)\n"
                      "#define SUBROUTINE(X) subroutine(X)\n"
                      "#define CONST const\n";
    #else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glslVersion = "#version 410\n";
        glslDefines = "#define LAYOUT_BINDING(X)\n"
                      "#define LAYOUT_LOCATION(X)\n"
                      "#define LAYOUT_INDEX(X)\n"
        #ifdef GLCHAOSP_NO_USES_GLSL_SUBS // troubles on MAC with multiple subroutines
                      "#define GLCHAOSP_NO_USES_GLSL_SUBS\n"
                      "#define SUBROUTINE(X)\n"
        #else
                      "#define SUBROUTINE(X) subroutine(X)\n"
        #endif
                      "#define CONST\n";
    #endif
#else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glslVersion = "#version 300 es\n";
        if(useLowPrecision()) glslDefines = "precision mediump float;\n" "precision highp int;\n";
        else                  glslDefines = "precision highp float;\n"   "precision highp int;\n";
        glslDefines+= "#define LAYOUT_BINDING(X)\n"
                      "#define LAYOUT_INDEX(X)\n"
                      "#define LAYOUT_LOCATION(X)\n"
                      "#define SUBROUTINE(X)\n"
#ifdef GLCHAOSP_LIGHTVER_EXPERIMENTAL
                      "#define GLCHAOSP_LIGHTVER_EXPERIMENTAL\n"
#endif
                      "#define CONST\n";
        glfwWindowHint(GLFW_SAMPLES,0);
#endif
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //GLFW_OPENGL_ANY_PROFILE
    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS,0);

    //glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    //glfwWindowHint(GLFW_SAMPLES,4);

    setGLFWWnd(glfwCreateWindow(GetWidth(), GetHeight(), getWindowTitle(), NULL, NULL));
    if (!getGLFWWnd())
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    if(getPosX()>=0 && getPosY()>=0) glfwSetWindowPos(getGLFWWnd(), getPosX(), getPosY());

    //secondary = glfwCreateWindow(512, 512, "My Engine", NULL, getGLFWWnd());

    glfwMakeContextCurrent(getGLFWWnd());

    //glfwSetWindowOpacity(getGLFWWnd(),.5);

#if !defined (__EMSCRIPTEN__)
    //Init OpenGL
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
#else
    bool b = emscripten_webgl_enable_extension(emscripten_webgl_get_current_context(),"EXT_color_buffer_float");
    if (!b) {
        cout << "sorry, can't render to floating point textures\n";
        //return;
    }

    emscripten_set_touchstart_callback("#canvas", &getEmsDevice(), true, emsMDeviceClass::touchStart);
    emscripten_set_touchend_callback("#canvas", &getEmsDevice(), true, emsMDeviceClass::touchEnd);
    emscripten_set_touchmove_callback("#canvas", &getEmsDevice(), true, emsMDeviceClass::touchMove);
    emscripten_set_touchcancel_callback("#canvas", &getEmsDevice(), true, emsMDeviceClass::touchCancel);
    //emscripten_set_deviceorientation_callback(getEmsDevice(), true, emsMDeviceClass::devOrientation);
    //emscripten_set_orientationchange_callback(getEmsDevice(), true, emsMDeviceClass::devOrientChange);
    //emscripten_set_devicemotion_callback(getEmsDevice(), true, emsMDeviceClass::devMotion);
#endif

    

    glfwSetMouseButtonCallback(getGLFWWnd(), glfwMouseButtonCallback);
    glfwSetKeyCallback(getGLFWWnd(), glfwKeyCallback);
    glfwSetCharCallback(getGLFWWnd(), glfwCharCallback);
    glfwSetCursorPosCallback(getGLFWWnd(), glfwMousePosCallback);
    glfwSetWindowSizeCallback(getGLFWWnd(), glfwWindowSizeCallback);
    glfwSetScrollCallback(getGLFWWnd(), glfwScrollCallback);


//#define APP_DEBUG_GUI_INTERFACE
#ifdef APP_DEBUG_GUI_INTERFACE
    glfwSwapInterval(1);
#else
    glfwSwapInterval(vSync);
#endif

    // init glfwTimer
    timer.init();
}

int mainGLApp::glfwExit()
{
    glfwDestroyWindow(getGLFWWnd());
    glfwTerminate();

// need to test exit glfw ... now 0!
    return 0;
}

int mainGLApp::getModifier() {
    GLFWwindow* window = getGLFWWnd();
    if((glfwGetKey(window,GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) || (glfwGetKey(window,GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS))
            return APP_MOD_CONTROL;
    else if((glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) || (glfwGetKey(window,GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS))
            return APP_MOD_SHIFT;
    else if((glfwGetKey(window,GLFW_KEY_LEFT_ALT) == GLFW_PRESS) || (glfwGetKey(window,GLFW_KEY_RIGHT_ALT) == GLFW_PRESS))
            return APP_MOD_ALT;
    else return APP_NO_MOD;
}

mainGLApp::mainGLApp() 
{    
    // Allocation in main(...)
    mainGLApp::theMainApp = this;
    glEngineWnd = new glWindow; 
    screenShotRequest = false;
    getQuickViewDirList();

}

mainGLApp::~mainGLApp() 
{
    onExit();

    delete glEngineWnd;
}

void mainGLApp::onInit(int w, int h) 
{

    xPosition = yPosition = -1;
    width = w, height = h;
    windowTitle = GLAPP_PROG_NAME;

#if !defined(GLCHAOSP_LIGHTVER)
    loadProgConfig();
#endif

    glfwInit();
#ifdef __EMSCRIPTEN__
    //initVR();
#endif

//Init OpenGL & engine
    glEngineWnd->onInit();

    imguiInit();

}

int mainGLApp::onExit()  
{
    glEngineWnd->onExit();
// Exit from both FrameWorks
    imguiExit();

    glfwExit();

// need to test returns code... now 0!        
    return 0;
}

void mainGLApp::resetParticlesSystem() { 
    //int sel = attractorsList.getSelection();
    theWnd->onExit(); 
    //attractorsList.resetSelection();
    theWnd->onInit(); 
    //attractorsList.setSelection(sel);
}

void newFrame()
{
    theApp->getTimer().tick();
    glfwPollEvents();

    theWnd->onIdle();
    theWnd->onRender();

    theApp->getMainDlg().renderImGui();

    glfwMakeContextCurrent(theApp->getGLFWWnd());
    glfwSwapBuffers(theApp->getGLFWWnd());
}

void mainGLApp::mainLoop() 
{
    while (!glfwWindowShouldClose(getGLFWWnd())) {
        
        glfwPollEvents();
        glfwGetFramebufferSize(getGLFWWnd(), &width, &height);

        if (!glfwGetWindowAttrib(getGLFWWnd(), GLFW_ICONIFIED)) 
            getTimer().tick();
#if !defined(GLCHAOSP_LIGHTVER)
            theWnd->onIdle();

            // debug interface
            //glClearColor(0.0, 0.0, 0.0, 0.1);
            //glClear(GL_COLOR_BUFFER_BIT);
            theWnd->onRender();

            if(screenShotRequest) {
                if(screenShotRequest == ScreeShotReq::ScrnSht_CAPTURE_ALL) getMainDlg().renderImGui();
                glfwMakeContextCurrent(getGLFWWnd());
                getScreenShot();
            }
            getMainDlg().renderImGui();

            glfwMakeContextCurrent(getGLFWWnd());
            glfwSwapBuffers(getGLFWWnd());
#else
            newFrame();
#endif
    }
}

// classic entry point
/////////////////////////////////////////////////
int main(int argc, char **argv)
{
//Initialize class e self pointer
    theApp = new mainGLApp;          

#ifdef GLCHAOSP_LIGHTVER
    if(argc>1 && argc!=8 ) {
        int w = atoi(argv[1]);
        int h = atoi(argv[2]);
        {// 3
            int sz = atoi(argv[3]);
            theApp->setMaxAllocatedBuffer((sz<0 ? 10 : (sz>50) ? 50 : sz) * 1000 * 1000); 
        }
        // 4
            if(atoi(argv[4])==1) theApp->setLowPrecision();
            else                 theApp->setHighPrecision();
        {// 5
            int sz = atoi(argv[5]);
            theApp->setEmissionStepBuffer((sz<0 ? 20 : (sz>200) ? 200 : sz) * 1000); 
        }
        // 6
            theApp->setTabletMode(atoi(argv[6])==1 ? true : false);
        // 7
            theApp->startWithGlowOFF(atoi(argv[7])==1 ? true : false);
        // 8
            theApp->useLightGUI(atoi(argv[8])==1 ? true : false);
        // 9        
            std::string s(argv[9]);
            //printf("%s\n",argv[9]);

            int index;
            int listSize = attractorsList.getList().size()-1;

            auto getRandomIDX = [&]() {
                fstRnd::fFastRand32 fastRandom;
                return int(fastRandom.UNI() * float(listSize));
            };

            if(s=="random") index = getRandomIDX();
            else            index = attractorsList.getSelectionByName(s);

            if(index<0 || index>listSize) index = getRandomIDX();            
            theApp->setStartWithAttractorIdx(index);        
        
        theApp->onInit(w<256 ? 256 : (w>3840 ? 3840 : w), h<256 ? 256 : (h>2160 ? 2160 : h));
    } else
#endif        
        theApp->onInit();
   
// Enter in GL main loop
/////////////////////////////////////////////////
#if !defined (__EMSCRIPTEN__)
    theApp->mainLoop();
#else
    emscripten_set_main_loop(newFrame,0,true);
#endif

// Exit procedures called from theApp destructor
/////////////////////////////////////////////////
    delete theApp;
   
    return 0;
}