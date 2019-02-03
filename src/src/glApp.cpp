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
#include <array>
#include <vector>
#include <ostream>
                
#include "glApp.h"
#include "glWindow.h"

#include "ShadersClasses.h"

#include "ui/uiSettings.h"


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
        //if(key == GLFW_KEY_F11) toggleFullscreenOnOff(window);

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
#else


enum touchAct {
    tCancel, tStart, tEnd, tMove
};

static touchAct actualTouchEvent = touchAct::tCancel;

bool pinchStart = false;
bool dblTouch = false;
float prevDist;

bool g_JustTouched[5] = { false, false, false, false, false };
float g_touch_x = -FLT_MAX, g_touch_y = -FLT_MAX;
float oldTouchX = -FLT_MAX, oldTouchY = -FLT_MAX;
bool touched = false;



glm::vec2 pinchPoints[2]; 
/*
EM_JS(int, screent_width, (), {
  return screen.width;
});
EM_JS(int, screent_height, (), {
  return screen.height;
});
*/

EM_JS(int, get_orientation, (), {
    var orientation = screen.msOrientation || (screen.orientation || screen.mozOrientation || {}).type;
    if      (orientation === "portrait-primary"   ) return  1;
    else if (orientation === "portrait-secondary" ) return  2;
    else if (orientation === "landscape-primary"  ) return  4;
    else if (orientation === "landscape-secondary") return  8;
    else return -1;
});


bool isDoubleTap(float x, float y, double ms)
{
    static auto before = std::chrono::system_clock::now();
    static touchAct oldEvent = touchAct::tCancel;
    static float oldx=-1, oldy=-1;
    const float R = 20.f;

    auto now = std::chrono::system_clock::now();
    double diff_ms = std::chrono::duration <double, std::milli> (now - before).count();

    bool retval = (diff_ms<ms && (oldx-R<x && oldx+R>x && oldy-R<y && oldy+R>y));
    before = now ;
        
    //if(action==GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) before = now ;    
    //oldButton = button;
    oldx=x, oldy=y;

    return retval;
}

    static EM_BOOL touchStart(int eventType, const EmscriptenTouchEvent *e, void *userData)
    {
        g_JustTouched[0] = true;
        actualTouchEvent = touchAct::tStart;

        oldTouchX = g_touch_x;
        oldTouchY = g_touch_y;
        g_touch_x = e->touches[0].canvasX; 
        g_touch_y = e->touches[0].canvasY;

        touched = true;

        if(e->numTouches==1 && isDoubleTap(g_touch_x, g_touch_y, 300)) {
            const float sz = 150.f;
            if(g_touch_x<sz || g_touch_y<sz || g_touch_x>theApp->GetWidth()-sz || g_touch_y>theApp->GetHeight()-sz) {
                theDlg.visible(theDlg.visible()^1);
            }
            else {
                dblTouch = true;
            }
            return true;
        }

        if(e->numTouches==2) {
            prevDist = glm::distance(glm::vec2(e->touches[0].canvasX, e->touches[0].canvasY),
                                     glm::vec2(e->touches[1].canvasX, e->touches[1].canvasY));
            
            pinchStart = true;
            return true;
        }

        //cout << "Canvas: " << e->touches[0].canvasX << " " << e->touches[0].canvasY << endl;
        //glfwMouseButtonCallback(theApp->getGLFWWnd(), 0, GLFW_PRESS, 0);

        if(ImGui::GetIO().WantCaptureMouse) return true;
        
        theWnd->onMouseButton(0, APP_MOUSE_BUTTON_DOWN, e->touches[0].canvasX, e->touches[0].canvasY); 


        return true;
    }
    static EM_BOOL touchEnd(int eventType, const EmscriptenTouchEvent *e, void *userData)
    {
        g_JustTouched[0] = false;
        dblTouch = false;
        actualTouchEvent = touchAct::tEnd;

        g_touch_x = e->touches[0].canvasX; 
        g_touch_y = e->touches[0].canvasY;

        if(e->numTouches!=2) pinchStart = false;


        if(ImGui::GetIO().WantCaptureMouse) return true;
        //glfwMouseButtonCallback(theApp->getGLFWWnd(), 0, GLFW_RELEASE, 0);
        
        theWnd->onMouseButton(0, APP_MOUSE_BUTTON_UP, e->touches[0].canvasX, e->touches[0].canvasY); 
        touched = false;
        return true;
    }

    static EM_BOOL touchMove(int eventType, const EmscriptenTouchEvent *e, void *userData)
    {
        actualTouchEvent = touchAct::tMove;
        if(touched) {
            g_touch_x = e->touches[0].canvasX; 
            g_touch_y = e->touches[0].canvasY;

            if(dblTouch) {
                const float scl = .25;
                vfGizmo3DClass &T = theWnd->getParticlesSystem()->getTMat()->getTrackball();
                //T.mouse(T.getPanControlButton(),T.getPanControlModifier(), true, g_touch_x, g_touch_y);

                glm::vec3 pan = T.getPanPosition();
                pan.x += (g_touch_x-oldTouchX) * T.getPanScale() * scl;
                pan.y -= (g_touch_y-oldTouchY) * T.getPanScale() * scl;
                T.setPanPosition(pan);
                oldTouchX = g_touch_x; oldTouchY = g_touch_y;

                return true;
            }
            
            if(e->numTouches==1) {
                if(ImGui::GetIO().WantCaptureMouse) return true;
                theWnd->onMotion(e->touches[0].canvasX, e->touches[0].canvasY);
            } else if(pinchStart && e->numTouches==2) { // pinch

                float d = glm::distance(glm::vec2(e->touches[0].canvasX, e->touches[0].canvasY), 
                                        glm::vec2(e->touches[1].canvasX, e->touches[1].canvasY));

                vfGizmo3DClass &T = theWnd->getParticlesSystem()->getTMat()->getTrackball();
                if(abs(prevDist-d)>4) {
                    T.setDollyPosition(T.getDollyPosition().z + T.getDollyScale() * (d-prevDist));
                    prevDist = d;
                }

            } 
            //cout << "Move : "<< e->touches[0].canvasX<< " " << e->touches[0].canvasY << endl;
        }
        return true;
    }

    static EM_BOOL touchCancel(int eventType, const EmscriptenTouchEvent *e, void *userData)
    {
        actualTouchEvent = touchAct::tCancel;
        return true;
    }

    static EM_BOOL devOrientation(int eventType, const EmscriptenDeviceOrientationEvent *e, void *userData) 
    {
        return true;
    }

    static EM_BOOL devOrientChange(int eventType, const EmscriptenOrientationChangeEvent *e, void *userData) 
    {
        cout << "change" << endl;
        return true;
    }

    static EM_BOOL devMotion(int eventType, const EmscriptenDeviceMotionEvent *e, void *userData) 
    {
        const float tollerance = 3.5;
        vfGizmo3DClass &T = theWnd->getParticlesSystem()->getTMat()->getTrackball();

        auto getAccel = [&] (double accel, float sign) -> double {
            return pow((abs(accel)-tollerance)*1.5, 2.0) * sign * (accel>0 ? -T.getPanScale() : T.getPanScale());
        };

        int orient = get_orientation();
        float signX = 1.0, signY = 1.0;
        double accX = e->accelerationIncludingGravityX, accY = e->accelerationIncludingGravityY;

        
        if((orient & EMSCRIPTEN_ORIENTATION_LANDSCAPE_PRIMARY) || (orient & EMSCRIPTEN_ORIENTATION_LANDSCAPE_SECONDARY)) {
            accX = e->accelerationIncludingGravityY;
            accY = e->accelerationIncludingGravityX;
/*
            if(orient == orientType::landscape_I) signY = -1.f;
            else                                  { 
                signY = -1.f;
                accX = -accX;
                accY = -accY;
            }
*/
            cout << "land : " << orient << "  "  << accX << "  " << accY << endl;

        } else { //portrait
            accX = e->accelerationIncludingGravityX;
            accY = e->accelerationIncludingGravityY;
/*
            if(orient == orientType::portrait_II) { 
                signX = -1.f; signY = -1.f; 
                accX = -accX;
                accY = -accY;
            }
*/
            cout << "port : " << orient << " - " << accX << " - " << accY << endl;
        }

        glm::vec3 pan = T.getPanPosition();

        if(abs(accX)>tollerance) pan.x += getAccel(accX, signX);        
        if(abs(accY)>tollerance) pan.y += getAccel(accY, signY);

        T.setPanPosition(pan);
        return true;
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
        glslDefines = "#define LAYUOT_BINDING(X) layout (binding = X)\n"
                      "#define LAYUOT_INDEX(X) layout(index = X)\n"
                      "#define CONST const\n";
    #else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glslVersion = "#version 410\n";
        glslDefines = "#define LAYUOT_BINDING(X)\n"
                      "#define LAYUOT_INDEX(X)\n"
    #ifdef GLCHAOSP_LIGHTVER
                      "#define TEST_WGL\n"
    #endif
                      "#define CONST\n";
    #endif
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //GLFW_OPENGL_ANY_PROFILE
        glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
#else
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
        glslVersion = "#version 300 es\n";
        if(useLowPrecision()) glslDefines = "precision lowp float;\n";
        else                  glslDefines = "precision highp float;\n";
        glslDefines+= "#define LAYUOT_BINDING(X)\n"
                      "#define LAYUOT_INDEX(X)\n"
                      "#define CONST\n";
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //GLFW_OPENGL_ANY_PROFILE
        glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
        glfwWindowHint(GLFW_SAMPLES,0);
#endif
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

#ifdef GLCHAOSP_USE_TOUCHSCREEN
    emscripten_set_mousedown_callback("#canvas", nullptr, true, 
        [](int, const EmscriptenMouseEvent* e, void*)->EMSCRIPTEN_RESULT {
            glfwMouseButtonCallback(theApp->getGLFWWnd(), e->button, GLFW_PRESS, 0);
            return true;
        });
    emscripten_set_mouseup_callback("#canvas", nullptr, true, 
        [](int, const EmscriptenMouseEvent* e, void*)->EMSCRIPTEN_RESULT {
            glfwMouseButtonCallback(theApp->getGLFWWnd(), e->button, GLFW_RELEASE, 0);          
            return true;
        });
#endif


    emscripten_set_touchstart_callback("#canvas", nullptr, true, touchStart);
    emscripten_set_touchend_callback("#canvas", nullptr, true, touchEnd);
    emscripten_set_touchmove_callback("#canvas", nullptr, true, touchMove);
    emscripten_set_touchcancel_callback("#canvas", nullptr, true, touchCancel);
    //emscripten_set_deviceorientation_callback(nullptr, true, devOrientation);
    //emscripten_set_orientationchange_callback(nullptr, true, devOrientChange);
    //emscripten_set_devicemotion_callback(nullptr, true, devMotion);

#endif

    

    glfwSetMouseButtonCallback(getGLFWWnd(), glfwMouseButtonCallback);
    glfwSetKeyCallback(getGLFWWnd(), glfwKeyCallback);
    glfwSetCharCallback(getGLFWWnd(), glfwCharCallback);
    glfwSetCursorPosCallback(getGLFWWnd(), glfwMousePosCallback);
    glfwSetWindowSizeCallback(getGLFWWnd(), glfwWindowSizeCallback);
    glfwSetScrollCallback(getGLFWWnd(), glfwScrollCallback);

    //theWnd->onReshape(GetWidth(),GetHeight());
    //gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

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

void newFrame()
{
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
            timer.tick();

#if !defined(GLCHAOSP_LIGHTVER)
            theWnd->onIdle();

            // debug interface
            //glClearColor(0.0, 0.0, 0.0, 0.1);
            //glClear(GL_COLOR_BUFFER_BIT);
            theWnd->onRender();

            if(screenShotRequest) {
                if(screenShotRequest == ScreeShotReq::ScrnSht_CAPTURE_ALL) getMainDlg().renderImGui();
                getScreenShot();
            }
            getMainDlg().renderImGui();

            //glfwMakeContextCurrent(getGLFWWnd());
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
    if(argc>1 && argc<=7) {
        int w = atoi(argv[1]);
        int h = atoi(argv[2]);
        if(argc >= 4) {
            int sz = atoi(argv[3]);
            theApp->setMaxAllocatedBuffer((sz<0 ? 10 : (sz>50) ? 50 : sz) * 1000 * 1000);
        }       
        if(argc >= 5) {
            if(atoi(argv[4])==1) theApp->setLowPrecision();
            else                 theApp->setHighPrecision();
        }
        if(argc >= 6) {
            int sz = atoi(argv[5]);
            theApp->setEmissionStepBuffer((sz<0 ? 20 : (sz>200) ? 200 : sz) * 1000);
        }
        if(argc >= 7) {
            theApp->setTabletMode(atoi(argv[6])==1 ? true : false);
        }
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