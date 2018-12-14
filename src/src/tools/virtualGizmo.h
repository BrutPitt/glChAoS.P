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

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

typedef int vgButtons;
typedef int vgModifiers;

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
template <class T> class virtualGizmoBaseClass {

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
    void viewportSize(T w, T h) { 
        width = w; height = h; 
        minVal = T(width < height ? width*T(0.5) : height*T(0.5));
        offset = glm::tvec3<T>(T(.5 * width), T(.5 * height), 0.0);
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
            if      (xRotationModifier & mod) { tbActive = true; rotationVector = glm::tvec3<T>(1.0, 0.0, 0.0); activateMouse(x,y); }
            else if (yRotationModifier & mod) { tbActive = true; rotationVector = glm::tvec3<T>(0.0, 1.0, 0.0); activateMouse(x,y); }
            else if (zRotationModifier & mod) { tbActive = true; rotationVector = glm::tvec3<T>(0.0, 0.0, 1.0); activateMouse(x,y); }
        } else if((button == tbRotationButton) && !pressed) { 
            deactivateMouse(); rotationVector = glm::tvec3<T>(1.0); tbActive = false; 
        }
    
    }

    //    Call on Mouse motion
    ////////////////////////////////////////////////////////////////////////////
    void motion( T x, T y) {
        delta.x = x - pos.x;   delta.y = pos.y - y;
        pos.x = x;   pos.y = y;
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
            qtStep = glm::tquat<T>(1.0, 0.0, 0.0, 0.0); //no rotation
            return;
        }

        glm::tvec3<T> a(T(pos.x-delta.x), T(height - (pos.y+delta.y)), 0.0);
        glm::tvec3<T> b(T(pos.x    ),     T(height -  pos.y         ), 0.0);

        auto vecFromPos = [&] (glm::tvec3<T> &v) {
            v -= offset;
            v /= minVal;
            const T len = length(v);
            v.z = len>T(0.0) ? pow(T(2.0), -T(.5) * len) : T(1.0);
            v = normalize(v);
        };

        vecFromPos(a);
        vecFromPos(b);

        glm::tvec3<T> axis = cross(a, b);
        axis = normalize(axis);

        T AdotB = dot(a, b); 
        T angle = acos( AdotB>T(1.0) ? T(1.0) : (AdotB<-T(1.0) ? -T(1.0) : AdotB)); // clamp need!!! corss float is approximate to FLT_EPSILON

        qtStep = normalize(glm::angleAxis(angle * tbScale * fpsRatio, axis * rotationVector));
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
    inline void applyRotation(glm::tmat4x4<T> &m) { m = m * glm::mat4_cast(qtV); }                                     

    //  Set the point around which the virtualGizmo will rotate.
    //////////////////////////////////////////////////////////////////
    void setRotationCenter( const glm::tvec3<T>& c) { rotationCenter = c; }
    glm::tvec3<T>& getRotationCenter() { return rotationCenter; }

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
	glm::tquat<T> &getRotation() { return qtV; }

	//  get the rotation increment
    //////////////////////////////////////////////////////////////////
    glm::tquat<T> &getStepRotation() { return qtStep; }

	//  get the rotation quaternion
    //////////////////////////////////////////////////////////////////
	void setRotation(const glm::tquat<T> &q) { qtV = q; }

	//  get the rotation increment
    //////////////////////////////////////////////////////////////////
	void setStepRotation(const glm::tquat<T> &q) { qtStep = q; }

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
        if      (xRotationModifier & mod) { rotationVector = glm::tvec3<T>(1.0, 0.0, 0.0); }
        else if (yRotationModifier & mod) { rotationVector = glm::tvec3<T>(0.0, 1.0, 0.0); }
        else if (zRotationModifier & mod) { rotationVector = glm::tvec3<T>(0.0, 0.0, 1.0); }
        update();
    }

    //  return current transformatin as 4x4 matrix.
    ////////////////////////////////////////////////////////////////////////////
    virtual glm::tmat4x4<T> getTransform() = 0;
    ////////////////////////////////////////////////////////////////////////////
    virtual void applyTransform(glm::tmat4x4<T> &model) = 0;

protected:

    glm::tvec2<T> pos, delta;

    // UI commands that this virtualGizmo responds to (defaults to left mouse button with no modifier key)
    vgButtons   tbControlButton, tbRotationButton;   
    vgModifiers tbControlModifiers, xRotationModifier, yRotationModifier, zRotationModifier;

    glm::tvec3<T> rotationVector = glm::tvec3<T>(1.0);

    glm::tquat<T> qtV    = glm::tquat<T>(1.0, 0.0, 0.0, 0.0);
    glm::tquat<T> qtStep = glm::tquat<T>(1.0, 0.0, 0.0, 0.0);

    glm::tvec3<T> rotationCenter = glm::tvec3<T>(0.0);

    //  settings for the sensitivity
    //////////////////////////////////////////////////////////////////
    T tbScale = T(1.0);   //base scale sensibility
    T fpsRatio = T(1.0);  //auto adjust by FPS (call idle with current FPS)
    
    T minVal;
    glm::tvec3<T> offset;

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
template <class T> class virtualGizmoClass : public virtualGizmoBaseClass<T> {

public:

    virtualGizmoClass()  { }

    //////////////////////////////////////////////////////////////////
    void motion( T x, T y) { if(this->tbActive) virtualGizmoBaseClass<T>::motion(x,y); }

    //////////////////////////////////////////////////////////////////
    void update() { this->updateGizmo(); }

    //////////////////////////////////////////////////////////////////
    void applyTransform(glm::tmat4x4<T> &model) {
        model = glm::translate(model, -this->rotationCenter);
        virtualGizmoBaseClass<T>::applyRotation(model);
        model = glm::translate(model, this->rotationCenter);
    }

    //////////////////////////////////////////////////////////////////
    glm::tmat4x4<T> getTransform() {
        glm::tmat4x4<T> trans, invTrans, rotation;
        rotation = glm::mat4_cast(this->qtV);

        trans = glm::translate(glm::tmat4x4<T>(T(1.0)),this->rotationCenter);
        invTrans = glm::translate(glm::tmat4x4<T>(T(1.0)),-this->rotationCenter);
        
        return invTrans * rotation * trans;
    }

    //  Set the speed for the virtualGizmo.
    //////////////////////////////////////////////////////////////////
    void setGizmoScale( T scale) { scale = scale; }

	// get the rotation quaternion
	glm::tquat<T> &getRotation() { return this->qtV; }
};

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//
// virtualGizmo3DClass
//  3D trackball: rotation interface with pan and dolly operations
//
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
template <class T> class virtualGizmo3DClass : public virtualGizmoBaseClass<T> {

using virtualGizmoBaseClass<T>::delta;
using virtualGizmoBaseClass<T>::qtV; 

public:
    //////////////////////////////////////////////////////////////////
    virtualGizmo3DClass() : dollyControlButton(evRightButton),   panControlButton(evMiddleButton),  dollyActive(false),
                         dollyControlModifiers(evNoModifier), panControlModifiers(evNoModifier), panActive(false) { }

    //////////////////////////////////////////////////////////////////
    void mouse( vgButtons button, vgModifiers mod, bool pressed, T x, T y) 
    {
        virtualGizmoBaseClass<T>::mouse(button, mod, pressed,  x,  y);
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

    //////////////////////////////////////////////////////////////////
    void motion( T x, T y) {
        if( this->tbActive || dollyActive || panActive) virtualGizmoBaseClass<T>::motion(x,y);
    }

    //////////////////////////////////////////////////////////////////
    void updatePan() {
        glm::tvec3<T> v(delta.x, delta.y, 0.0);	  
        pan += v * panScale;
	}

    //////////////////////////////////////////////////////////////////
    void updateDolly() {
        glm::tvec3<T> v(0.0, 0.0, delta.y);
        dolly -= v * dollyScale;
	}

    //////////////////////////////////////////////////////////////////
    void update() {
        if (this->tbActive) virtualGizmoBaseClass<T>::updateGizmo();
        if (dollyActive) updateDolly();
        if (panActive)   updatePan();
    }

    //////////////////////////////////////////////////////////////////
    void applyTransform(glm::tmat4x4<T> &m) {
        m = glm::translate(m, pan);
        m = glm::translate(m, dolly);
        m = glm::translate(m, -this->rotationCenter);
        virtualGizmoBaseClass<T>::applyRotation(m);
        m = glm::translate(m, this->rotationCenter);
    }

    //////////////////////////////////////////////////////////////////
    glm::tmat4x4<T> getTransform() {
        glm::tmat4x4<T> trans, invTrans, rotation;
        glm::tmat4x4<T> panMat, dollyMat;

        //create pan and dolly translations
        panMat   = glm::translate(glm::tmat4x4<T>(T(1.0)),pan  );
        dollyMat = glm::translate(glm::tmat4x4<T>(T(1.0)),dolly);

        //create the virtualGizmo rotation
        rotation = glm::mat4_cast(qtV);

        //create the translations to move the center of rotation to the origin and back
        trans    = glm::translate(glm::tmat4x4<T>(T(1.0)), this->rotationCenter);
        invTrans = glm::translate(glm::tmat4x4<T>(T(1.0)),-this->rotationCenter);

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

    //  Set the speed for the dolly operation.
    //////////////////////////////////////////////////////////////////
    void setDollyScale( T scale) { dollyScale = scale;  }
    //  Set the speed for pan
    //////////////////////////////////////////////////////////////////
    void setPanScale( T scale) { panScale = scale; }

    //  Set the Dolly to a specified distance.
    //////////////////////////////////////////////////////////////////
    void setDollyPosition(T pos)             { dolly.z = pos; }
    void setDollyPosition(glm::tvec3<T> pos) { dolly.z = pos.z; }

    //  Set the Dolly to a specified distance.
    //////////////////////////////////////////////////////////////////
    void setPanPosition(glm::tvec3<T> pos) { pan.x = pos.x; pan.y = pos.y;}

    //  Get dolly pos... use as Zoom factor
    //////////////////////////////////////////////////////////////////
    glm::tvec3<T> &getDollyPosition() { return dolly; }

    //  Get dolly pos... use as Zoom factor
    //////////////////////////////////////////////////////////////////
    glm::tvec3<T> &getPanPosition() { return pan; }

private:
    // UI commands that this virtualGizmo responds to (defaults to left mouse button with no modifier key)
    vgButtons   dollyControlButton,    panControlButton;
    vgModifiers dollyControlModifiers, panControlModifiers;

    // Variable used to determine if the manipulator is presently tracking the mouse
    bool dollyActive;
    bool panActive;      

    glm::tvec3<T> pan   = glm::tvec3<T>(0.0);
    glm::tvec3<T> dolly = glm::tvec3<T>(0.0);
    
    T dollyScale = T(0.01);  //dolly scale
    T panScale   = T(0.01);  //pan scale
};

typedef virtualGizmoClass<float> vfGizmoClass;
typedef virtualGizmo3DClass<float> vfGizmo3DClass;

typedef virtualGizmoClass<double> vdGizmoClass;
typedef virtualGizmo3DClass<double> vdGizmo3DClass;
