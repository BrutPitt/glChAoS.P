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
#include "emsTouch.h"
#include <emscripten/val.h>

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

//emscripten::val pincopallino=null;
//emscripten::val xrSession;
/*
EM_JS(void, onSessionStarted, (int session) {
  // Store the session for use later.
  var xrSession = session;

  xrSession.requestReferenceSpace('local')
  .then((referenceSpace) => {
    xrReferenceSpace = referenceSpace;
  })
  .then(setupWebGLLayer) // Create a compatible XRWebGLLayer
  .then(() => {
    // Start the render loop
    xrSession.requestAnimationFrame(onDrawFrame);
  });
});

EM_JS(void, beginXRSession, (), {
  // requestSession must be called within a user gesture event
  // like click or touch when requesting an immersive session.
  navigator.xr.requestSession('immersive-vr')
      .then(_onSessionStarted)
      .catch(err => {
        // May fail for a variety of reasons. Probably just want to
        // render the scene normally without any tracking at this point.
        window.requestAnimationFrame(onDrawFrame);
      });
});

EM_JS(void, initVR, (), {
    console.log("TEST: ");

    navigator.xr.supportsSession('immersive-vr').then(() => {
        var enterXrBtn = document.createElement("button");
        enterXrBtn.innerHTML = "Enter VR";
        enterXrBtn.addEventListener("click", _beginXRSession);
        document.body.appendChild(enterXrBtn);        
        console.log("Immersive VR is supported: ");
    }).catch((err) => {
        console.log("Immersive VR is not supported: " + err);
    });
});

*/
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
            //std::cout << "Tx: " << touchX << "Ty: " << touchY << "Px: " << viewport->Pos.x << "Py: " << viewport->Pos.y << std::endl;
            io.MousePos = ImVec2((float)touchX + viewport->Pos.x, (float)touchY + viewport->Pos.y);

            for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++) {
                io.MouseDown[i] |=  imguiJustTouched[i];
            }
        }
    }
}
/*
//ImGui master
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
    t.touchX = TOUCH(0,X);
    t.touchY = TOUCH(0,Y);

    t.touched = true;

    //std::cout << "TouchStart: " <<  std::endl;
    //std::cout << "Tx: " << t.touchX << "Ty: " << t.touchY <<  std::endl;

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
        t.prevDist = distance(vec2(TOUCH(0,X), TOUCH(0,Y)),
                              vec2(TOUCH(1,X), TOUCH(1,Y)));
            
        t.pinchStart = true;
        return true;
    }

    if(ImGui::GetIO().WantCaptureMouse) return true;
        
    theWnd->onMouseButton(0, APP_MOUSE_BUTTON_DOWN, TOUCH(0,X), TOUCH(0,Y)); 

    return true;
}

EM_BOOL emsMDeviceClass::touchEnd(int eventType, const EmscriptenTouchEvent *e, void *userData)
{
    emsMDeviceClass &t = *((emsMDeviceClass *) userData);
    t.imguiJustTouched[0] = false;
    t.dblTouch = false;
    t.actualTouchEvent = touchAct::tEnd;

    t.touchX = TOUCH(0,X); 
    t.touchY = TOUCH(0,Y);

    if(e->numTouches!=2) t.pinchStart = false;

    t.touched = false;

    if(ImGui::GetIO().WantCaptureMouse) return true;
        
    theWnd->onMouseButton(0, APP_MOUSE_BUTTON_UP, TOUCH(0,X), TOUCH(0,Y)); 
    return true;
}

EM_BOOL emsMDeviceClass::touchMove(int eventType, const EmscriptenTouchEvent *e, void *userData)
{
    emsMDeviceClass &t = *((emsMDeviceClass *) userData);

    t.actualTouchEvent = touchAct::tMove;
    if(t.touched) {
        t.touchX = TOUCH(0,X); 
        t.touchY = TOUCH(0,Y);

        if(t.dblTouch) { // pan

            transformsClass *Tm = theWnd->getParticlesSystem()->getTMat();
            vg::vGizmo3D &Tb = Tm->getTrackball();

            //const float scl = .25;
            const float scl = Tm->getOverallDistance() * Tb.getDistScale() * .5;

            vec3 pan = Tb.getPanPosition();
            pan.x += (t.touchX-t.oldTouchX) * Tb.getPanScale() * scl;
            pan.y -= (t.touchY-t.oldTouchY) * Tb.getPanScale() * scl;
            Tb.setPanPosition(pan);
            t.oldTouchX = t.touchX; t.oldTouchY = t.touchY;
            return true;
        }
            
        if(e->numTouches==1) { // rotate
            if(!ImGui::GetIO().WantCaptureMouse) theWnd->onMotion(TOUCH(0,X), TOUCH(0,Y));
            else return true;
        } else if(t.pinchStart && e->numTouches==2) { // pinch

            float d = distance(vec2(TOUCH(0,X), TOUCH(0,Y)),
                               vec2(TOUCH(1,X), TOUCH(1,Y)));

            if(abs(t.prevDist-d)>4) {
                transformsClass *Tm = theWnd->getParticlesSystem()->getTMat();
                vg::vGizmo3D &Tb = Tm->getTrackball();

                const float z = Tm->getOverallDistance() * Tb.getDistScale();
                Tb.setDollyPosition(Tb.getDollyPosition().z + Tb.getDollyScale() * (d-t.prevDist) * (z>0.0 ? z : 1.0));
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
    vg::vGizmo3D &T = theWnd->getParticlesSystem()->getTMat()->getTrackball();

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

    vec3 pan = T.getPanPosition();

    if(abs(accX)>tollerance) pan.x += getAccel(accX, signX);        
    if(abs(accY)>tollerance) pan.y += getAccel(accY, signY);

    T.setPanPosition(pan);
    return true;
}
