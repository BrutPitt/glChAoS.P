//------------------------------------------------------------------------------
//  Copyright (c) 2018-2024 Michele Morrone
//  All rights reserved.
//
//  https://michelemorrone.eu - https://glchaosp.com - https://brutpitt.com
//
//  X: https://x.com/BrutPitt - GitHub: https://github.com/BrutPitt
//
//  direct mail: brutpitt(at)gmail.com - me(at)michelemorrone.eu
//
//  This software is distributed under the terms of the BSD 2-Clause license
//------------------------------------------------------------------------------
#include <array>
#include <vector>

#include "glApp.h"
#include "glWindow.h"

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

    static bool leftShift = true, leftCtrl = true;
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
            case GLFW_KEY_G:
                theApp->idleRotation(theApp->idleRotation()^1);
                break;
            case GLFW_KEY_DOWN :  
            case GLFW_KEY_D    :  {
                int idx = attractorsList.getSelection();
                attractorsList.setSelection((idx >= attractorsList.getList().size()-1) ? 0 : idx+1);
                } break;
            case GLFW_KEY_UP :  
            case GLFW_KEY_U  :  {
                int idx = attractorsList.getSelection();
                attractorsList.setSelection((idx <= 0) ? attractorsList.getList().size()-1 : idx-1);
                } break;
            case GLFW_KEY_V    :
            case GLFW_KEY_C    :
                tfSettinsClass::cockPit(tfSettinsClass::cockPit() ^ 1);
                break;
            case GLFW_KEY_PAGE_UP :
                tfSettinsClass::setPIPposition(tfSettinsClass::pip::rTop);
                break;
            case GLFW_KEY_PAGE_DOWN :
                tfSettinsClass::setPIPposition(tfSettinsClass::pip::rBottom);
                break;
            case GLFW_KEY_HOME :
                tfSettinsClass::setPIPposition(tfSettinsClass::pip::lTop);
                break;
            case GLFW_KEY_END :
                tfSettinsClass::setPIPposition(tfSettinsClass::pip::lBottom);
                break;
            case GLFW_KEY_P :
                tfSettinsClass::invertPIP(tfSettinsClass::invertPIP() ^ 1);
                break;
            case GLFW_KEY_DELETE :  
            case GLFW_KEY_INSERT :
                tfSettinsClass::setPIPposition(tfSettinsClass::pip::noPIP);
                break;
#if !defined(GLCHAOSP_TEST_RANDOM_DISTRIBUTION)
            case GLFW_KEY_RIGHT :
            case GLFW_KEY_R     :  {
                int idx = theApp->selectedListQuickView();
                theApp->loadQuikViewSelection((idx >= theApp->getListQuickView().size()-1) ? 0 : idx+1);
                } break;
            case GLFW_KEY_LEFT :
            case GLFW_KEY_L    :  {
                int idx = theApp->selectedListQuickView();
                theApp->loadQuikViewSelection((idx <= 0) ? theApp->getListQuickView().size()-1 : idx-1);
                } break;
#endif
            case GLFW_KEY_LEFT_SHIFT :
                leftShift = true;
                break;
            case GLFW_KEY_RIGHT_SHIFT:
                leftShift = false;
                break;
            case GLFW_KEY_LEFT_CONTROL :
                leftCtrl = true;
                break;
            case GLFW_KEY_RIGHT_CONTROL:
                leftCtrl = false;
                break;
            case GLFW_KEY_PRINT_SCREEN :  
                if(mods & GLFW_MOD_CONTROL) // CTRL+PrtScr -> request FileName
                    theApp->setScreenShotRequest(leftCtrl ? ScreeShotReq::ScrnSht_FILE_NAME : ScreeShotReq::ScrnSht_FILE_NAME_ALPHA);
                if(mods & GLFW_MOD_SHIFT) // SHIFT+PrtScr -> silent capture
                    theApp->setScreenShotRequest(leftShift ? ScreeShotReq::ScrnSht_SILENT_MODE : ScreeShotReq::ScrnSht_SILENT_MODE_ALPHA);
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
    } else if (action == GLFW_RELEASE) {        
#if !defined (__EMSCRIPTEN__)
        isDoubleClick(button, action, x , y, 300);
#endif
        theWnd->onMouseButton(button, APP_MOUSE_BUTTON_UP, x, y); 
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
    int nmonitors, bestoverlap {0};;
    GLFWmonitor *bestmonitor = nullptr;
    GLFWmonitor **monitors = glfwGetMonitors(&nmonitors);

    int wx, wy; glfwGetWindowPos (window, &wx, &wy);
    int ww, wh; glfwGetWindowSize(window, &ww, &wh);

    for (int i = 0; i < nmonitors; i++) {
        const GLFWvidmode *mode = glfwGetVideoMode(monitors[i]);
        int mx, my; glfwGetMonitorPos(monitors[i], &mx, &my);

        const int overlap = std::max(0, std::min(wx + ww, mx + mode->width ) - std::max(wx, mx)) *
                            std::max(0, std::min(wy + wh, my + mode->height) - std::max(wy, my));

        if (bestoverlap < overlap) {
            bestoverlap = overlap;
            bestmonitor = monitors[i];
        }
    }
    return bestmonitor ? bestmonitor : throw "no monitor found!";
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
    //IMGUI_CHECKVERSION();
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
    theDlg.getAttractorDlg().resetFirstTime();
}

int mainGLApp::imguiExit()
{
// need to test exit wx ... now 0!
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    return 0;
}

void mainGLApp::getScreenShot(bool is32bit)
{
    const int w = glEngineWnd->getParticlesSystem()->getWidth(), h = glEngineWnd->getParticlesSystem()->getHeight();
    const int rowDim = w*(is32bit ? 4 : 3);
    unsigned char *buffer = new unsigned char [rowDim*h], *flipped = new unsigned char [rowDim*h];
    unsigned char *tmpBuff = buffer, *tmpFlip = flipped+rowDim*(h-1);


    glReadPixels(0, 0, w, h, (is32bit ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, buffer);
    while(tmpFlip >= flipped) {
        memcpy(tmpFlip, tmpBuff, rowDim);
        tmpFlip-=rowDim; 
        tmpBuff+=rowDim;
    }
    theApp->saveScreenShot(flipped, w, h, is32bit);

    delete[] buffer;
    delete[] flipped;
}

#ifdef __EMSCRIPTEN__
//EM_JS(void, jsActiveTexture, (int tex),  { _glActiveTexture(tex); } );
//EM_JS(void, jsActiveTexture, (int tex),  { console.log(GL.textures[tex]); console.log(GL.textures[tex].name); _glActiveTexture(tex); } );

EM_JS(int, get_canvas_width, (), { return canvas.width; });
EM_JS(int, get_canvas_height, (), { return canvas.height; });
EM_JS(bool, isAngleWithChromium, (), {
    var lineWidthRange = Module.ctx.getParameter(Module.ctx.ALIASED_LINE_WIDTH_RANGE);
    var isLineOneOne = (lineWidthRange[0] == 1) && (lineWidthRange[1] == 1);
    var isNotFF = navigator.userAgent.toLowerCase().indexOf('firefox') == -1;
    let platform = navigator.platform; //navigator.userAgentData.platform

    var angle = ((platform === 'Win32') || (platform === 'Win64')) && isNotFF && isLineOneOne;
    //alert(angle);
    return angle;
});
#endif

/////////////////////////////////////////////////
// glfw utils
void mainGLApp::glfwInit()
{
    glfwSetErrorCallback(glfwErrorCallback);

    if (!::glfwInit()) exit(EXIT_FAILURE);

    const std::string fPrecision(std::string("precision ") + std::string(shaderFloatPrecision == glslPrecision::low ? "lowp" : (shaderFloatPrecision == glslPrecision::medium ? "mediump" : "highp")) + " float;\n");
    const std::string iPrecision(std::string("precision ") + std::string(shaderIntPrecision   == glslPrecision::low ? "lowp" : (shaderIntPrecision   == glslPrecision::medium ? "mediump" : "highp")) + " int;\n");
//    const std::string fPrecision(std::string("precision highp float;\n"));
//    const std::string iPrecision(std::string("precision highp int;\n"));

#if !defined (__EMSCRIPTEN__)
    #ifdef GLAPP_REQUIRE_OGL45
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
        glslVersion = "#version 450\n";
        glslDefines = "#define LAYOUT_BINDING(X) layout (binding = X)\n"
                      "#define LAYOUT_INDEX(X) layout(index = X)\n"
                      "#define LAYOUT_LOCATION(X) layout(location = X)\n"
                      "#define SUBROUTINE(X) subroutine(X)\n"
                      "#define CONST const\n";
    #else
        #ifdef GLAPP_USES_ES3
            glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
            glslVersion = "#version 300 es\n";
            glslDefines = fPrecision + iPrecision;
            glslDefines+= "#define LAYOUT_BINDING(X)\n"
                          "#define LAYOUT_INDEX(X)\n"
                          "#define LAYOUT_LOCATION(X)\n"
                          "#define GLCHAOSP_NO_USES_GLSL_SUBS\n"
                          "#define SUBROUTINE(X)\n"
                          "#define CONST\n";
        #else
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        #ifdef NDEBUG
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        #else
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3); //debug to 4.3: need glDebugMessageCallback
        #endif
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
    #endif
    glfwWindowHint(GLFW_SAMPLES, getMultisamplingValue());
#else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glslVersion = "#version 300 es\n";
        glslDefines = fPrecision + iPrecision;
        glslDefines+= "#define LAYOUT_BINDING(X)\n"
                      "#define LAYOUT_INDEX(X)\n"
                      "#define LAYOUT_LOCATION(X)\n"
                      "#define SUBROUTINE(X)\n"
                      "#define CONST\n";
        glfwWindowHint(GLFW_SAMPLES, 0);
#endif

#if defined(GLAPP_USES_ES3) || defined(__EMSCRIPTEN__)
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#endif

#if defined(GLAPP_USES_ES3)
    // From GLFW:
    //      X11: On some Linux systems, creating contexts via both the native and EGL APIs in a single process
    //           will cause the application to segfault. Stick to one API or the other on Linux for now.
    glfwWindowHint(GLFW_CONTEXT_CREATION_API, GLFW_EGL_CONTEXT_API); //GLFW_NATIVE_CONTEXT_API
#endif
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // from GLFW: If OpenGL ES is requested, this hint is ignored.
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // from GLFW: If OpenGL ES is requested, this hint is ignored.

#ifdef NDEBUG
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_CONTEXT_NO_ERROR);
#else
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 0); // rendering is on FBO, so disable DEPTH buffer of context

    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    //glfwWindowHint(GLFW_SAMPLES,4);

    setGLFWWnd(glfwCreateWindow(GetWidth(), GetHeight(), getWindowTitle(), NULL, NULL));
    if(!getGLFWWnd()) {  glfwTerminate(); exit(EXIT_FAILURE);  }

//list  video modes
//    GLFWmonitor* monitor = getCurrentMonitor(getGLFWWnd());
//    if (monitor)
//    {
//        int numModes;
//        const GLFWvidmode* modes = glfwGetVideoModes(monitor, &numModes);
//
//        for(int i=0; i<numModes; i++) {
//            cout << modes[i].width   << ' ' << modes[i].height << ' ';
//            cout << modes[i].redBits << ' ' << modes[i].greenBits << ' ' << modes[i].blueBits << ' ' <<  modes[i].refreshRate << '\n';
//        }
//    }
//
    glfwPollEvents();
    
    if(getPosX()>=0 && getPosY()>=0) glfwSetWindowPos(getGLFWWnd(), getPosX(), getPosY());
    glfwSetWindowSize(getGLFWWnd(), width, height);

    glfwMakeContextCurrent(getGLFWWnd());

#if !defined (__EMSCRIPTEN__)
    //Init OpenGL
    #ifdef GLAPP_USES_ES3
        gladLoadGLES2Loader((GLADloadproc)glfwGetProcAddress); //get OpenGL ES extensions
    #else
        gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);    //get OpenGL extensions
    #endif

#ifndef NDEBUG
/*
    glEnable              ( GL_DEBUG_OUTPUT );
    //glDebugMessageCallback( MessageCallback, nullptr );
    glDebugMessageCallback( openglCallbackFunction, nullptr);
*/
    if(glDebugMessageCallback) {
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(openglCallbackFunction, nullptr);
        GLuint unusedIds = 0;
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, &unusedIds, true);
    }
    else
        cout << "glDebugMessageCallback not available: need OpenGL ES 3.2+ or OpenGL 4.3+" << endl;
#endif

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

    const int x = EM_ASM_INT({ return window.innerWidth;  }), y = EM_ASM_INT({ return window.innerHeight; });
    width  = canvasX = x;
    height = canvasY = y;

    glfwSetWindowSize(theApp->getGLFWWnd(), x, y);

    //EM_ASM(console.log(Module.ctx.getParameter(Module.ctx.UNPACK_PREMULTIPLY_ALPHA_WEBGL) ? "unpak VERO!!" : "unpack FALSO!!"););
    //EM_ASM(console.log(Module.ctx.getContextAttributes().alpha ? "alpha VERO!!" : "alpha FALSO!!"); );
    //EM_ASM(console.log(Module.ctx.getContextAttributes().premultipliedAlpha ? "pre VERO!!" :"pre FALSO!!"); );
    //EM_ASM(Module.ctx.pixelStorei(Module.ctx.UNPACK_PREMULTIPLY_ALPHA_WEBGL, true););

    //detect ANGLE
    //EM_ASM( var angleInfo = Module.ctx.getExtension('ANGLE_instanced_arrays'); if(angleInfo!=null) alert("Angle detected"););
    //EM_ASM( var debInfo = Module.ctx.getExtension('WEBGL_debug_renderer_info'); alert(Module.ctx.getParameter(debInfo.UNMASKED_RENDERER_WEBGL)););

    isAngleBug = false; //isAngleWithChromium();

#ifndef NDEBUG
    EM_ASM(if(!Module.ctx.getExtension('EXT_color_buffer_float')) alert("wglChAoS.P need EXT_color_buffer_float"););
    EM_ASM(if(!Module.ctx.getExtension('EXT_float_blend'))        alert("wglChAoS.P need EXT_float_blend"););
#endif

    //emscripten_set_deviceorientation_callback(getEmsDevice(), true, emsMDeviceClass::devOrientation);
    //emscripten_set_orientationchange_callback(getEmsDevice(), true, emsMDeviceClass::devOrientChange);
    //emscripten_set_devicemotion_callback(getEmsDevice(), true, emsMDeviceClass::devMotion);
#endif

#ifdef GLCHAOSP_NO_AO_SHDW
       glslDefines+= "#define GLCHAOSP_NO_AO_SHDW\n";
#else
       if(!canUseShadow()) glslDefines+= "#define GLCHAOSP_NO_AO_SHDW\n";
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
#if !defined(GLCHAOSP_USE_LOWPRECISION)
    setHighPrecision();
#else
    setLowPrecision();
#endif
    mainGLApp::theMainApp = this;

    glEngineWnd = new glWindow; 
    screenShotRequest = false;
    getQuickViewDirList();
}

mainGLApp::~mainGLApp() 
{
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

    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxCombTexImgUnits);
}

int mainGLApp::onExit()  
{
    glfwExit();

// need to test returns code... now 0!        
    return 0;
}

void newFrame()
{
#ifdef __EMSCRIPTEN__    
    //theApp->setCanvasX(EM_ASM_INT({ return Module.canvas.width; }));
    //theApp->setCanvasY(EM_ASM_INT({ return Module.canvas.height; }));
    const int x = EM_ASM_INT({ return window.innerWidth;  }), y = EM_ASM_INT({ return window.innerHeight; });

    if(x != theApp->getCanvasX() || y != theApp->getCanvasY()) {
        //theWnd->onReshape(x, y);
        glfwSetWindowSize(theApp->getGLFWWnd(), x, y);
        theApp->setCanvasX(x);
        theApp->setCanvasY(y);
    }

    theApp->getTimer().tick();
    glfwPollEvents();

    theWnd->onIdle();

    theApp->getMainDlg().renderImGui();
    theWnd->onRender();
    theApp->getMainDlg().postRenderImGui();
    glfwMakeContextCurrent(theApp->getGLFWWnd());
    
    glfwSwapBuffers(theApp->getGLFWWnd());
#endif
}

void mainGLApp::mainLoop() 
{
    //glfwSetWindowSize(getGLFWWnd(), width, height);
    //glfwSetWindowPos(getGLFWWnd(), getPosX(), getPosY());

    while(!glfwWindowShouldClose(getGLFWWnd()) && !appNeedRestart) {
        
        glfwPollEvents();
        glfwGetFramebufferSize(getGLFWWnd(), &width, &height);

        if(!glfwGetWindowAttrib(getGLFWWnd(), GLFW_ICONIFIED))
            getTimer().tick();
#if !defined(GLCHAOSP_LIGHTVER)
            theWnd->onIdle();
            getMainDlg().renderImGui();
            theApp->needRestart();

            // debug interface
            //glClearColor(0.0, 0.0, 0.0, 0.1);
            //glClear(GL_COLOR_BUFFER_BIT);
            theWnd->onRender();

            if(screenShotRequest) {
                if(screenShotRequest == ScreeShotReq::ScrnSht_CAPTURE_ALL) {
                    getMainDlg().postRenderImGui();
                //glfwMakeContextCurrent(getGLFWWnd());
                    getScreenShot(0);
                } else {
                    getScreenShot(screenShotRequest == ScreeShotReq::ScrnSht_SILENT_MODE_ALPHA || screenShotRequest == ScreeShotReq::ScrnSht_FILE_NAME_ALPHA);
                    getMainDlg().postRenderImGui();
                }
            } else  getMainDlg().postRenderImGui();
            //glfwMakeContextCurrent(getGLFWWnd());
            glfwSwapBuffers(getGLFWWnd());
#else
            newFrame();
#endif
    }
}

#if defined(WIN32) || defined(_WIN32)
extern "C" {
    __declspec(dllexport) uint32_t NvOptimusEnablement = 1;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif

// classic entry point
/////////////////////////////////////////////////
int main(int argc, char **argv)
{
//Initialize class e self pointer
    theApp = new mainGLApp;
#ifdef __EMSCRIPTEN__
    if(argc>1 && argc!=8) {
        int w = atoi(argv[1]);
        int h = atoi(argv[2]);
        if(w<=1024)        theDlg.startMinimized(mainImGuiDlgClass::webRes::minus1024);
        else { if(w<=1280) theDlg.startMinimized(mainImGuiDlgClass::webRes::minus1280);
               else {
                   if(w<1600)  theDlg.startMinimized(mainImGuiDlgClass::webRes::minus1440);
                   else        theDlg.startMinimized(mainImGuiDlgClass::webRes::noRestriction);
               }
        }
        {// 3
            int sz = atoi(argv[3]);
            theApp->setMaxAllocatedBuffer((sz<0 ? 10 : (sz>50) ? 50 : sz) * 1000 * 1000); 
        }
        // 4
    #if defined(GLCHAOSP_LIGHTVER_EXPERIMENTAL)
            if(atoi(argv[4])==1) theApp->setLowPrecision();
            else                 theApp->setHighPrecision();
    #else
            theApp->setLowPrecision();
    #endif
        {// 5
            int sz = atoi(argv[5]);
            theApp->setEmissionStepBuffer((sz<0 ? 20 : (sz>200) ? 200 : sz) * 1000); 
        }
        // 6
            theApp->setTabletMode(atoi(argv[6])==1 ? true : false);
        // 7
            theApp->startWithGlowOFF(atoi(argv[7])==1 ? true : false);
        // 8
// 0000000001 0x001 LightGui
// 0000000010 0x002 FixedCanvas
// 0000000100 0x004 FullRestriction
// 0000001000 0x008 All Menu minimized
// 0000010000 0x010 Rendering mode
// 0000100000 0x020 TF FullScrren
// 0001000000 0x040 TF Cockpit
// 0010000000 0x080 Force BB
// 0100000000 0x100 Force PS
            const uint32_t val = atoi(argv[8]);
            theApp->useLightGUI(val&1 ? true : false);
            theApp->useFixedCanvas(val&2 ? true : false);
            if(val&0x004) theDlg.startMinimized(mainImGuiDlgClass::webRes::fullRestriction); // start all standard Wnd minimized
            if(val&0x008) theDlg.startMinimized(mainImGuiDlgClass::webRes::onlyMainMenuMinimized);

            // singlePixel / TF fullscreen / TF cockpit
            theApp->setScoutMode(mainGLApp::scoutTypes::normalSelectionMode);                // select default
            if(val&0x010)      { theApp->setScoutMode(mainGLApp::scoutTypes::renderingMode);   // singlePixel
                theDlg.setAttractorDlgVisible(false);
            }
            else if(val&0x020) { theApp->setScoutMode(mainGLApp::scoutTypes::evolutionMode);   // TF fullscreen
                theDlg.startMinimized(mainImGuiDlgClass::webRes::minus1280);
                theDlg.setCockpitDlgVisible();
                theDlg.setCockpitDlgCollapsed();
            }
            else if(val&0x040) { theApp->setScoutMode(mainGLApp::scoutTypes::flyMode);         // TF cockpit
                theDlg.setCockpitDlgVisible();
                theDlg.setCockpitDlgCollapsed();
            }

            theApp->setSelectionMode(mainGLApp::selectionTypes::noSelection);                   // no preference
            if(val&0x080)      theApp->setSelectionMode(mainGLApp::selectionTypes::forceSelBB); // force slection btween BB/PS
            else if(val&0x100) theApp->setSelectionMode(mainGLApp::selectionTypes::forceSelPS); // force slection btween BB/PS
        // 9
            std::string s(argv[9]);
            theApp->setStartWithAttractorName(s.empty() ? "random" : s);
        
        theApp->onInit(w<256 ? 256 : (w>3840 ? 3840 : w), h<256 ? 256 : (h>2160 ? 2160 : h));
    } else theApp->onInit();

//Init OpenGL & engine
    theWnd->onInit();
    theApp->imguiInit();

    emscripten_set_main_loop(newFrame,0,true);

// Exit from both FrameWorks
    theWnd->onExit();
    theApp->imguiExit();
    theApp->onExit();
#else
    theApp->onInit();
    do { // test if app need restart
        theApp->needRestart(false);
//Init OpenGL & engine
        theWnd->onInit();
        theApp->imguiInit();
   
        theApp->mainLoop();

// Exit from both FrameWorks
        theWnd->onExit();
        theApp->imguiExit();

    } while(theApp->needRestart());
    theApp->onExit();
#endif

    delete theApp;
    return 0;
}
