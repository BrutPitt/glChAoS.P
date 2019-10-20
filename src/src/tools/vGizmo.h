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
#pragma once

#include "vGizmoConfig.h"
#include "vGizmoMath.h"

#ifdef VGIZMO_USES_TEMPLATE
    #define VGIZMO_BASE_CLASS virtualGizmoBaseClass<T>
#else
    #define VGIZMO_BASE_CLASS virtualGizmoBaseClass
    #define T VG_T_TYPE
#endif

typedef int vgButtons;
typedef int vgModifiers;

namespace vg {
//  Default values for button and modifiers.
//      This values are aligned with GLFW defines (for my comfort),
//      but they are loose from any platform library: simply initialize
//      the virtualGizmo with your values: 
//          look at "onInit" in glWindow.cpp example.
//////////////////////////////////////////////////////////////////////
    enum {
        evLeftButton  ,
        evRightButton ,
        evMiddleButton
    };

    enum {
        evNoModifier      =  0,
        evShiftModifier   =  1   ,
        evControlModifier =  1<<1,
        evAltModifier     =  1<<2,
        evSuperModifier   =  1<<3  
    };

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//
//  Base manipulator class
//
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
TEMPLATE_TYPENAME_T class virtualGizmoBaseClass {

public:
    virtualGizmoBaseClass() : tbActive(false), pos(0), delta(0),
                           tbControlButton(evLeftButton), tbControlModifiers(evNoModifier),
                           tbRotationButton(evRightButton), xRotationModifier(evShiftModifier),
                           yRotationModifier(evControlModifier),  zRotationModifier(evAltModifier|evSuperModifier)
    { 
        viewportSize(T(256), T(256));  //initial dummy value
    }
    virtual ~virtualGizmoBaseClass() {}

    //    Call to initialize and on reshape
    ////////////////////////////////////////////////////////////////////////////
    void viewportSize(int w, int h) { viewportSize(T(w), T(h)); }
    void viewportSize(T w, T h) { 
        width = w; height = h; 
        minVal = T(width < height ? width*T(0.5) : height*T(0.5));
        offset = tVec3(T(.5 * width), T(.5 * height), T(0));
    }

    void inline activateMouse(T x, T y) {
        pos.x = x;
        pos.y = y;
        delta.x = delta.y = 0;
    }
    void inline deactivateMouse() {
        if(delta.x == 0 && delta.y == 0) update();
        delta.x = delta.y = 0;
    }
    void inline testRotModifier(int x, int y, vgModifiers mod) { }
    
    //    Call on mouse button event
    //      button:  your mouse button
    //      mod:     your modifier key -> CTRL, SHIFT, ALT, SUPER
    //      pressed: if Button is pressed (TRUE) or released (FALSE)
    //      x, y:    mouse coordinates
    ////////////////////////////////////////////////////////////////////////////
    void mouse( vgButtons button, vgModifiers mod, bool pressed, T x, T y) 
    {
        if ( (button == tbControlButton) && pressed && (tbControlModifiers ? tbControlModifiers & mod : tbControlModifiers == mod) ) {
            tbActive = true;
            activateMouse(x,y);
        }
        else if ( button == tbControlButton && !pressed) {
            deactivateMouse();
            tbActive = false;
        }

        if((button == tbRotationButton) && pressed) {
            if      (xRotationModifier & mod) { tbActive = true; rotationVector = tVec3(T(1), T(0), T(0)); activateMouse(x,y); }
            else if (yRotationModifier & mod) { tbActive = true; rotationVector = tVec3(T(0), T(1), T(0)); activateMouse(x,y); }
            else if (zRotationModifier & mod) { tbActive = true; rotationVector = tVec3(T(0), T(0), T(1)); activateMouse(x,y); }
        } else if((button == tbRotationButton) && !pressed) { 
            deactivateMouse(); rotationVector = tVec3(T(1)); tbActive = false; 
        }
    
    }

    //    Call on Mouse motion
    ////////////////////////////////////////////////////////////////////////////
    void motion( T x, T y) {
        delta.x = x - pos.x;   delta.y = pos.y - y;
        pos.x = x;   pos.y = y;
        update();
    }
    //    Call on Pinching
    ////////////////////////////////////////////////////////////////////////////
    void pinching( T d ) {
        delta.y = d;
        update();
    }

    //    Call every rendering to implement continue spin rotation 
    ////////////////////////////////////////////////////////////////////////////
    void idle() { qtV = qtStep*qtV;  }

    //    Call after changed settings
    ////////////////////////////////////////////////////////////////////////////
    virtual void update() = 0;
    void updateGizmo() 
    {

        if(!delta.x && !delta.y) {
            qtStep = tQuat(T(1), T(0), T(0), T(0)); //no rotation
            return;
        }

        tVec3 a(T(pos.x-delta.x), T(height - (pos.y+delta.y)), T(0));
        tVec3 b(T(pos.x    ),     T(height -  pos.y         ), T(0));

        auto vecFromPos = [&] (tVec3 &v) {
            v -= offset;
            v /= minVal;
            const T len = length(v);
            v.z = len>T(0) ? pow(T(2), -T(.5) * len) : T(1);
            v = normalize(v);
        };

        vecFromPos(a);
        vecFromPos(b);

        tVec3 axis = cross(a, b);
        axis = normalize(axis);

        T AdotB = dot(a, b); 
        T angle = acos( AdotB>T(1) ? T(1) : (AdotB<-T(1) ? -T(1) : AdotB)); // clamp need!!! corss float is approximate to FLT_EPSILON

        qtStep = normalize(angleAxis(angle * tbScale * fpsRatio, axis * rotationVector));
        qtV = qtStep*qtV;

    }

    //  Set the sensitivity for the virtualGizmo.
    //////////////////////////////////////////////////////////////////
    void setGizmoFeeling( T scale) { tbScale = scale; }
    //  Call with current fps (every rendering) to adjust "auto" sensitivity
    //////////////////////////////////////////////////////////////////
    void setGizmoFPS(T fps) { fpsRatio = T(60.0)/fps;}

    //  Apply rotation
    //////////////////////////////////////////////////////////////////
    inline void applyRotation(tMat4 &m) { m = m * mat4_cast(qtV); }                                     

    //  Set the point around which the virtualGizmo will rotate.
    //////////////////////////////////////////////////////////////////
    void setRotationCenter( const tVec3& c) { rotationCenter = c; }
    tVec3& getRotationCenter() { return rotationCenter; }

    //  Set the mouse button and modifier for rotation 
    //////////////////////////////////////////////////////////////////
    void setGizmoRotControl( vgButtons b, vgModifiers m = evNoModifier) {
        tbControlButton = b;
        tbControlModifiers = m;
    }
    //  Set the mouse button and modifier for rotation around X axis
    //////////////////////////////////////////////////////////////////
    void setGizmoRotXControl( vgButtons b, vgModifiers m = evNoModifier) {
        tbRotationButton = b;
        xRotationModifier = m;
    }
    //  Set the mouse button and modifier for rotation around Y axis
    //////////////////////////////////////////////////////////////////
    void setGizmoRotYControl( vgButtons b, vgModifiers m = evNoModifier) {
        tbRotationButton = b;
        yRotationModifier = m;
    }
    //  Set the mouse button and modifier for rotation around Z axis
    //////////////////////////////////////////////////////////////////
    void setGizmoRotZControl( vgButtons b, vgModifiers m = evNoModifier) {
        tbRotationButton = b;
        zRotationModifier = m;
    }

    //  get the rotation quaternion
    //////////////////////////////////////////////////////////////////
    tQuat &getRotation() { return qtV; }

    //  get the rotation increment
    //////////////////////////////////////////////////////////////////
    tQuat &getStepRotation() { return qtStep; }

    //  get the rotation quaternion
    //////////////////////////////////////////////////////////////////
    void setRotation(const tQuat &q) { qtV = q; }

    //  get the rotation increment
    //////////////////////////////////////////////////////////////////
    void setStepRotation(const tQuat &q) { qtStep = q; }

// Immediate mode helpers
//////////////////////////////////////////////////////////////////////

    // for imGuIZMO or immediate mode control
    //////////////////////////////////////////////////////////////////
    void motionImmediateLeftButton( T x, T y, T dx, T dy) {
        tbActive = true;
        delta.x = dx; delta.y = -dy;
        pos.x = x;   pos.y = y;
        update();
    }
    //  for imGuIZMO or immediate mode control
    //////////////////////////////////////////////////////////////////
    void motionImmediateMode( T x, T y, T dx, T dy,  vgModifiers mod) {
        tbActive = true;
        delta.x = dx; delta.y = -dy;
        pos.x = x;   pos.y = y;
        if      (xRotationModifier & mod) { rotationVector = tVec3(T(1), T(0), T(0)); }
        else if (yRotationModifier & mod) { rotationVector = tVec3(T(0), T(1), T(0)); }
        else if (zRotationModifier & mod) { rotationVector = tVec3(T(0), T(0), T(1)); }
        update();
    }

    //  return current transformatin as 4x4 matrix.
    ////////////////////////////////////////////////////////////////////////////
    virtual tMat4 getTransform() = 0;
    ////////////////////////////////////////////////////////////////////////////
    virtual void applyTransform(tMat4 &model) = 0;

protected:

    tVec2 pos, delta;

    // UI commands that this virtualGizmo responds to (defaults to left mouse button with no modifier key)
    vgButtons   tbControlButton, tbRotationButton;   
    vgModifiers tbControlModifiers, xRotationModifier, yRotationModifier, zRotationModifier;

    tVec3 rotationVector = tVec3(T(1));

    tQuat qtV    = tQuat(T(1), T(0), T(0), T(0));
    tQuat qtStep = tQuat(T(1), T(0), T(0), T(0));

    tVec3 rotationCenter = tVec3(T(0));

    //  settings for the sensitivity
    //////////////////////////////////////////////////////////////////
    T tbScale = T(1);   //base scale sensibility
    T fpsRatio = T(1);  //auto adjust by FPS (call idle with current FPS)
    
    T minVal;
    tVec3 offset;

    bool tbActive;  // trackbal activated via mouse

    T width, height;
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//
// virtualGizmoClass
//  trackball: simple mouse rotation control 
//
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
TEMPLATE_TYPENAME_T class virtualGizmoClass : public VGIZMO_BASE_CLASS {

public:

    virtualGizmoClass()  { }

    //////////////////////////////////////////////////////////////////
    void motion( T x, T y) { if(this->tbActive) VGIZMO_BASE_CLASS::motion(x,y); }

    //////////////////////////////////////////////////////////////////
    void update() { this->updateGizmo(); }

    //////////////////////////////////////////////////////////////////
    void applyTransform(tMat4 &model) {
        model = translate(model, -this->rotationCenter);
        VGIZMO_BASE_CLASS::applyRotation(model);
        model = translate(model, this->rotationCenter);
    }

    //////////////////////////////////////////////////////////////////
    tMat4 getTransform() {
        tMat4 trans, invTrans, rotation;
        rotation = mat4_cast(this->qtV);

        trans = translate(tMat4(T(1)),this->rotationCenter);
        invTrans = translate(tMat4(T(1)),-this->rotationCenter);
        
        return invTrans * rotation * trans;
    }

    //  Set the speed for the virtualGizmo.
    //////////////////////////////////////////////////////////////////
    void setGizmoScale( T scale) { scale = scale; }

    // get the rotation quaternion
    tQuat &getRotation() { return this->qtV; }
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//
// virtualGizmo3DClass
//  3D trackball: rotation interface with pan and dolly operations
//
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
TEMPLATE_TYPENAME_T class virtualGizmo3DClass : public VGIZMO_BASE_CLASS {

using VGIZMO_BASE_CLASS::delta;
using VGIZMO_BASE_CLASS::qtV; 

public:
    //////////////////////////////////////////////////////////////////
    virtualGizmo3DClass() : dollyControlButton(evRightButton),   panControlButton(evMiddleButton),  dollyActive(false),
                         dollyControlModifiers(evNoModifier), panControlModifiers(evNoModifier), panActive(false) { }

    //////////////////////////////////////////////////////////////////
    void mouse( vgButtons button, vgModifiers mod, bool pressed, int x, int y) { mouse(button, mod, pressed, T(x), T(y)); }
    void mouse( vgButtons button, vgModifiers mod, bool pressed, T x, T y) 
    {
        VGIZMO_BASE_CLASS::mouse(button, mod, pressed,  x,  y);
        if ( button == dollyControlButton && pressed && (dollyControlModifiers ? dollyControlModifiers & mod : dollyControlModifiers == mod) ) {
            dollyActive = true;
            this->activateMouse(x,y);
        }
        else if ( button == dollyControlButton && !pressed) {
            this->deactivateMouse();
            dollyActive = false;
        }
        
        if ( button == panControlButton && pressed && (panControlModifiers ? panControlModifiers & mod : panControlModifiers == mod) ) {
            panActive = true;
            this->activateMouse(x,y);
        }
        else if ( button == panControlButton && !pressed) {
            this->deactivateMouse();
            panActive = false;
        }
    }

    //    Call on wheel (only for Dolly/Zoom)
    ////////////////////////////////////////////////////////////////////////////
    void wheel( T x, T y) {
        dolly.z += y * dollyScale * T(5);
    }

    //////////////////////////////////////////////////////////////////
    void motion( int x, int y) { motion( T(x), T(y)); }
    void motion( T x, T y) {
        if( this->tbActive || dollyActive || panActive) VGIZMO_BASE_CLASS::motion(x,y);
    }

    //////////////////////////////////////////////////////////////////
    void updatePan() {
        tVec3 v(delta.x, delta.y, T(0));
        pan += v * panScale;
    }

    //////////////////////////////////////////////////////////////////
    void updateDolly() {
        tVec3 v(T(0), T(0), delta.y);
        dolly -= v * dollyScale;
    }

    //////////////////////////////////////////////////////////////////
    void update() {
        if (this->tbActive) VGIZMO_BASE_CLASS::updateGizmo();
        if (dollyActive) updateDolly();
        if (panActive)   updatePan();
    }

    //////////////////////////////////////////////////////////////////
    void applyTransform(tMat4 &m) {
        m = translate(m, pan);
        m = translate(m, dolly);
        m = translate(m, -this->rotationCenter);
        VGIZMO_BASE_CLASS::applyRotation(m);
        m = translate(m, this->rotationCenter);
    }

    //////////////////////////////////////////////////////////////////
    tMat4 getTransform() {
        tMat4 trans, invTrans, rotation;
        tMat4 panMat, dollyMat;

        //create pan and dolly translations
        panMat   = translate(tMat4(T(1)),pan  );
        dollyMat = translate(tMat4(T(1)),dolly);

        //create the virtualGizmo rotation
        rotation = mat4_cast(qtV);

        //create the translations to move the center of rotation to the origin and back
        trans    = translate(tMat4(T(1)), this->rotationCenter);
        invTrans = translate(tMat4(T(1)),-this->rotationCenter);

        //concatenate all the tranforms
        return panMat * dollyMat * invTrans * rotation * trans;
    }

    //  Set the mouse button and mods for dolly operation.
    //////////////////////////////////////////////////////////////////
    void setDollyControl( vgButtons b, vgModifiers m = evNoModifier) {
        dollyControlButton = b;
        dollyControlModifiers = m;
    }
    //  Set the mouse button and optional mods for pan
    //////////////////////////////////////////////////////////////////
    void setPanControl( vgButtons b, vgModifiers m = evNoModifier) {
        panControlButton = b;
        panControlModifiers = m;
    }
    int getPanControlButton() { return panControlButton; }
    int getPanControlModifier() { return panControlModifiers; }

    //  Set the speed for the dolly operation.
    //////////////////////////////////////////////////////////////////
    void setWheelScale( T scale) { wheelScale = scale;  }
    T getWheelScale() { return wheelScale;  }
    //  Set the speed for the dolly operation.
    //////////////////////////////////////////////////////////////////
    void setDollyScale( T scale) { dollyScale = scale;  }
    T getDollyScale() { return dollyScale;  }
    //  Set the speed for pan
    //////////////////////////////////////////////////////////////////
    void setPanScale( T scale) { panScale = scale; }
    T getPanScale() { return panScale; }

    //  Set the Dolly to a specified distance.
    //////////////////////////////////////////////////////////////////
    void setDollyPosition(T pos)             { dolly.z = pos; }
    void setDollyPosition(const tVec3 &pos) { dolly.z = pos.z; }

    //  Set the Dolly to a specified distance.
    //////////////////////////////////////////////////////////////////
    void setPanPosition(const tVec3 &pos) { pan.x = pos.x; pan.y = pos.y;}

    //  Get dolly pos... use as Zoom factor
    //////////////////////////////////////////////////////////////////
    tVec3 &getDollyPosition() { return dolly; }

    //  Get Pan pos... use as Zoom factor
    //////////////////////////////////////////////////////////////////
    tVec3 &getPanPosition() { return pan; }

    //  Get Pan (xy) & Dolly (z) position
    //////////////////////////////////////////////////////////////////
    tVec3 getPosition() const { return tVec3(pan.x, pan.y, dolly.z); }
private:
    // UI commands that this virtualGizmo responds to (defaults to left mouse button with no modifier key)
    vgButtons   dollyControlButton,    panControlButton;
    vgModifiers dollyControlModifiers, panControlModifiers;

    // Variable used to determine if the manipulator is presently tracking the mouse
    bool dollyActive;
    bool panActive;      

    tVec3 pan   = tVec3(T(0));
    tVec3 dolly = tVec3(T(0));
    
    T dollyScale = T(.01);  //dolly scale
    T panScale   = T(.01);  //pan scale
    T wheelScale = T(5);   //dolly multiply for wheel
};

#ifdef VGIZMO_USES_TEMPLATE
    #ifdef VGIZMO_USES_DOUBLE_PRECISION
        using vGizmo   = virtualGizmoClass<double>;
        using vGizmo3D = virtualGizmo3DClass<double>;
    #else
        using vGizmo   = virtualGizmoClass<float>;
        using vGizmo3D = virtualGizmo3DClass<float>;
    #endif
    using vImGuIZMO = virtualGizmoClass<float>;
#else
    using vImGuIZMO = virtualGizmoClass;

    using vGizmo    = virtualGizmoClass;
    using vGizmo3D  = virtualGizmo3DClass;
#endif

} // end namespace vg::

#undef T  // if used T as #define, undef it