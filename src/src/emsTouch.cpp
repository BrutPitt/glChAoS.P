#include "emsTouch.h"

#include "glApp.h"
#include "glWindow.h"


EM_JS(int, screent_width, (), {
  return screen.width;
});
EM_JS(int, screent_height, (), {
  return screen.height;
});


EM_JS(int, get_orientation, (), {
    var orientation = screen.msOrientation || (screen.orientation || screen.mozOrientation || {}).type;
    if      (orientation === "portrait-primary"   ) return  1;
    else if (orientation === "portrait-secondary" ) return  2;
    else if (orientation === "landscape-primary"  ) return  4;
    else if (orientation === "landscape-secondary") return  8;
    else return -1;
});


bool emsMDeviceClass::isDoubleTap(float x, float y, double ms)
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

//ImGui Docking/ViewPorts
void emsMDeviceClass::imGuiUpdateTouch()
{
    ImGuiIO& io = ImGui::GetIO();
    const ImVec2 mouse_pos_backup = io.MousePos;
    io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
    io.MouseHoveredViewport = 0;

    // Update buttons
    for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
    {
        // If a touch event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
        io.MouseDown[i] = imguiJustTouched[i];
    }

    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    for (int n = 0; n < platform_io.Viewports.Size; n++)
    {
        ImGuiViewport* viewport = platform_io.Viewports[n];
        GLFWwindow* window = (GLFWwindow*)viewport->PlatformHandle;
        IM_ASSERT(window != NULL);
        const bool focused = true;
        IM_ASSERT(platform_io.Viewports.Size == 1);

        if (focused)
        {
            std::cout << "Tx: " << touchX << "Ty: " << touchY << "Px: " << viewport->Pos.x << "Py: " << viewport->Pos.y << std::endl;
            io.MousePos = ImVec2((float)touchX + viewport->Pos.x, (float)touchY + viewport->Pos.y);

            for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) {
                io.MouseDown[i] |=  imguiJustTouched[i];
            }
        }
    }
}
/*
//ImGui ViewPorts master
void emsMDeviceClass::imGuiUpdateTouch()
{
    ImGuiIO& io = ImGui::GetIO();
    const ImVec2 mouse_pos_backup = io.MousePos;
    io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
    //io.MouseHoveredViewport = 0;

    // Update buttons
    for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
    {
        // If a touch event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
        io.MouseDown[i] = imguiJustTouched[i];
    }

    {
        io.MousePos = ImVec2((float)touchX + mouse_pos_backup.x, (float)touchY + mouse_pos_backup.y);

        for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) {
            io.MouseDown[i] |=  imguiJustTouched[i];
        }
    }
}
*/
EM_BOOL emsMDeviceClass::touchStart(int eventType, const EmscriptenTouchEvent *e, void *userData)
{
    emsMDeviceClass &t = *((emsMDeviceClass *) userData);
    t.imguiJustTouched[0] = true;
    t.actualTouchEvent = touchAct::tStart;

    t.oldTouchX = t.touchX;
    t.oldTouchY = t.touchY;
    t.touchX = e->touches[0].targetX; 
    t.touchY = e->touches[0].targetY;

    t.touched = true;

    std::cout << "TouchStart: " <<  std::endl;
    std::cout << "Tx: " << t.touchX << "Ty: " << t.touchY <<  std::endl;

    if(e->numTouches==1 && t.isDoubleTap(t.touchX, t.touchY, 300)) {
        const float sz = 150.f;
        if(t.touchX<sz || t.touchY<sz || t.touchX>theApp->GetWidth()-sz || t.touchY>theApp->GetHeight()-sz) {
            theDlg.visible(theDlg.visible()^1);
        }
        else {
            t.dblTouch = true;
        }
        return true;
    }

    if(e->numTouches==2) {
        t.prevDist = glm::distance(glm::vec2(e->touches[0].targetX, e->touches[0].targetY),
                                   glm::vec2(e->touches[1].targetX, e->touches[1].targetY));
            
        t.pinchStart = true;
        return true;
    }

    if(ImGui::GetIO().WantCaptureMouse) return true;
        
    theWnd->onMouseButton(0, APP_MOUSE_BUTTON_DOWN, e->touches[0].targetX, e->touches[0].targetY); 

    return true;
}

EM_BOOL emsMDeviceClass::touchEnd(int eventType, const EmscriptenTouchEvent *e, void *userData)
{
    emsMDeviceClass &t = *((emsMDeviceClass *) userData);
    t.imguiJustTouched[0] = false;
    t.dblTouch = false;
    t.actualTouchEvent = touchAct::tEnd;

    t.touchX = e->touches[0].targetX; 
    t.touchY = e->touches[0].targetY;

    if(e->numTouches!=2) t.pinchStart = false;

    t.touched = false;

    if(ImGui::GetIO().WantCaptureMouse) return true;
        
    theWnd->onMouseButton(0, APP_MOUSE_BUTTON_UP, e->touches[0].targetX, e->touches[0].targetY); 
    return true;
}

EM_BOOL emsMDeviceClass::touchMove(int eventType, const EmscriptenTouchEvent *e, void *userData)
{
    emsMDeviceClass &t = *((emsMDeviceClass *) userData);

    t.actualTouchEvent = touchAct::tMove;
    if(t.touched) {
        t.touchX = e->touches[0].targetX; 
        t.touchY = e->touches[0].targetY;

        if(t.dblTouch) {
            const float scl = .25;
            vfGizmo3DClass &T = theWnd->getParticlesSystem()->getTMat()->getTrackball();

            glm::vec3 pan = T.getPanPosition();
            pan.x += (t.touchX-t.oldTouchX) * T.getPanScale() * scl;
            pan.y -= (t.touchY-t.oldTouchY) * T.getPanScale() * scl;
            T.setPanPosition(pan);
            t.oldTouchX = t.touchX; t.oldTouchY = t.touchY;
            return true;
        }
            
        if(e->numTouches==1) {
            if(ImGui::GetIO().WantCaptureMouse) return true;
            theWnd->onMotion(e->touches[0].targetX, e->touches[0].targetY);
        } else if(t.pinchStart && e->numTouches==2) { // pinch

            float d = glm::distance(glm::vec2(e->touches[0].targetX, e->touches[0].targetY), 
                                    glm::vec2(e->touches[1].targetX, e->touches[1].targetY));

            vfGizmo3DClass &T = theWnd->getParticlesSystem()->getTMat()->getTrackball();
            if(abs(t.prevDist-d)>4) {
                T.setDollyPosition(T.getDollyPosition().z + T.getDollyScale() * (d-t.prevDist));
                t.prevDist = d;
            }

        } 
    }
    return true;
}

EM_BOOL emsMDeviceClass::touchCancel(int eventType, const EmscriptenTouchEvent *e, void *userData)
{
    emsMDeviceClass &t = *((emsMDeviceClass *) userData);
    t.actualTouchEvent = touchAct::tCancel;
    return true;
}

EM_BOOL emsMDeviceClass::devOrientation(int eventType, const EmscriptenDeviceOrientationEvent *e, void *userData) 
{
    return true;
}

EM_BOOL emsMDeviceClass::devOrientChange(int eventType, const EmscriptenOrientationChangeEvent *e, void *userData) 
{
    return true;
}

EM_BOOL emsMDeviceClass::devMotion(int eventType, const EmscriptenDeviceMotionEvent *e, void *userData) 
{
    const float tollerance = 3.5;
    emsMDeviceClass &t = *((emsMDeviceClass *) userData);
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


    } else { //portrait
        accX = e->accelerationIncludingGravityX;
        accY = e->accelerationIncludingGravityY;

    }

    glm::vec3 pan = T.getPanPosition();

    if(abs(accX)>tollerance) pan.x += getAccel(accX, signX);        
    if(abs(accY)>tollerance) pan.y += getAccel(accY, signY);

    T.setPanPosition(pan);
    return true;
}
