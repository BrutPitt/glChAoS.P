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
#pragma once

#include "appDefines.h"
#include <float.h>

extern "C" {
extern void initVR();
}

class emsMDeviceClass
{
public:

#define TOUCH(I,P) (theApp->useFixedCanvas() ? float(e->touches[I].target##P) : (float(e->touches[I].target##P) * float(float(theApp->getSize##P()) / float(theApp->getCanvas##P()))))

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

};

