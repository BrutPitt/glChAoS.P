#pragma once

#include "appDefines.h"
#include <glm/glm.hpp>

class emsMDeviceClass
{
public:

enum touchAct {
    tCancel, tStart, tEnd, tMove
};


    void imGuiUpdateTouch();
    bool isDoubleTap(float x, float y, double ms);
    static EM_BOOL touchStart(int eventType, const EmscriptenTouchEvent *e, void *userData);
    static EM_BOOL touchEnd(int eventType, const EmscriptenTouchEvent *e, void *userData);
    static EM_BOOL touchMove(int eventType, const EmscriptenTouchEvent *e, void *userData);
    static EM_BOOL touchCancel(int eventType, const EmscriptenTouchEvent *e, void *userData);
    static EM_BOOL devOrientation(int eventType, const EmscriptenDeviceOrientationEvent *e, void *userData);
    static EM_BOOL devOrientChange(int eventType, const EmscriptenOrientationChangeEvent *e, void *userData);
    static EM_BOOL devMotion(int eventType, const EmscriptenDeviceMotionEvent *e, void *userData);

    touchAct actualTouchEvent = touchAct::tCancel;

    bool pinchStart = false;
    bool dblTouch = false;
    bool touched = false;
    float prevDist;

    bool imguiJustTouched[5] = { false, false, false, false, false };
    float touchX = -FLT_MAX, touchY = -FLT_MAX;
    float oldTouchX = -FLT_MAX, oldTouchY = -FLT_MAX;

    glm::vec2 pinchPoints[2]; 
};

