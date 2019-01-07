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
#include <algorithm>


#include <ImGui/imgui.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <ImGui/imgui_internal.h>

#include <virtualGizmo.h>


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//
//    NEED TO BUILD :
//
//    - Using glm (OpenGL Mathematics) include files: need version 0.9.9 or later
//        and GLM_ENABLE_EXPERIMENTAL compile option to enable "quaternion" class
//    - Using include virtualGizmo.h (my virtualGizmoClass) to get imput controls
//          and apply rotations
//    - use c++11 standard
//
//    Use preprocessor define imguiGizmo_INTERPOLATE_NORMALS, to a phong color effect
//    for solid components, but there are more vertex memorized and more operations,
//    even if the visual effect is better... the computational difference is:
//    one normal for vertex  VS  one normal for plain figure (triangle or quad)
//    
//    comment/uncomment below or add as directive to compiler
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

#define imguiGizmo_INTERPOLATE_NORMALS

typedef float tbT;

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//
//  imguiGizmo 3D
//
//      Buttons:
//          Left:   drag to free rotate axes
//          Right:  drag to free rotate spot (only dualmode!)
//          Right & Left: drag to rotate together
//              for ImGui features, right button drag is active only INSIDE the frame
//      Modifiers:
//          Shift:  rotation only around X
//          Ctrl:   rotation only around Y
//          Alt:    rotation only around Z
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

// The data structure that holds the orientation among other things
struct imguiGizmo
{

    glm::tquat<tbT> qtV  = glm::tquat<tbT>(tbT(1.0), glm::tvec3<tbT>(0.0)); // Quaternion value
    glm::tquat<tbT> qtV2 = glm::tquat<tbT>(tbT(1.0), glm::tvec3<tbT>(0.0)); // Quaternion value

    enum      {                            //0b0000'0000, //C++14 notation
                mode3Axes          = 0x01, //0b0000'0001, 
                modeDirection      = 0x02, //0b0000'0010,
                modeDual           = 0x08, //0b0000'1000,
                modeMask           = 0x0f, //0b0000'1111,
                

                cubeAtOrigin       = 0x10, //0b0000'0000, 
                sphereAtOrigin     = 0x20, //0b0001'0000,
                noSolidAtOrigin    = 0x40, //0b0010'0000,
                modeFullAxes       = 0x80,
                axesModeMask       = 0xf0  //0b1111'0000
    };

    enum { sphereTess16, sphereTess8, sphereTess4, sphereTess2 };
    enum { CONE_SURF, CONE_CAP, CYL_SURF, CYL_CAP };
    //enum { SOLID_SURF, SOLID_CAP }
    //enum { 
    enum { axisIsX, axisIsY, axisIsZ };

    enum solidSides{ backSide, frontSide  }; // or viceversa... 
    static ImVector<glm::vec3> sphereVtx;
    static ImVector<int>       sphereTess;
    static ImVector<glm::vec3> cubeVtx;
    static ImVector<glm::vec3> cubeNorm;
    static ImVector<glm::vec3> arrowVtx[4];
    static ImVector<glm::vec3> arrowNorm[4];
    static void buildCube    (const float side);
    static void buildSphere  (const float radius, const int tessFactor);
    static void buildCone    (const float x0, const float x1, const float radius, const int slices);
    static void buildCylinder(const float x0, const float x1, const float radius, const int slices);
    
    
    // helper functions
    ///////////////////////////////////////
    static void resizeAxesOf(const glm::vec3 &newSize) {
        savedAxesResizeFactor = axesResizeFactor; axesResizeFactor = newSize; }
    static void restoreAxesSize() {
        axesResizeFactor = savedAxesResizeFactor; }

    static void resizeSolidOf(float newSize) {
        savedSolidResizeFactor = solidResizeFactor; solidResizeFactor = newSize; }
    static void restoreSolidSize() {
        solidResizeFactor = savedSolidResizeFactor; }

    static void setDirectionColor(const ImVec4 &color) {
        savedDirectionColor = directionColor; directionColor = color; }
    static void restoreDirectionColor() {
        directionColor = savedDirectionColor; }

    static void setSphereColors(ImU32 a, ImU32 b) {
        savedSphereColors[0] = sphereColors[0]; savedSphereColors[1] = sphereColors[1];
        sphereColors[0] = a; sphereColors[1] = b; }
    static void restoreSphereColors() {
        sphereColors[0] = savedSphereColors[0]; sphereColors[1] = savedSphereColors[1]; }


    //
    //  internals
    //
    ////////////////////////////////////////////////////////////////////////////
    static bool solidAreBuilded;
    static bool dragActivate;

    int drawMode = mode3Axes;
    int axesOriginType = cubeAtOrigin;
    bool showFullAxes = false;

    bool drawFunc(const char* label, float size);

    void modeSettings(int mode) {
        drawMode = int(mode & modeMask); axesOriginType = int(mode & axesModeMask); showFullAxes = bool(modeFullAxes & mode); }

    void setDualMode(const int mode) { modeSettings((imguiGizmo::modeDual | imguiGizmo::axesModeMask) & (mode | imguiGizmo::modeDual)); }

    // vec3 -> quat -> trackbalTransforms -> quat -> vec3
    ////////////////////////////////////////////////////////////////////////////
    bool getTransforms(glm::quat& q, const char* label, glm::vec3& dir, float size) {
        const float len = glm::length(dir);
        q = glm::angleAxis(acosf(dir.x/len), glm::normalize(glm::vec3(.0, -dir.z, dir.y)));

        bool ret = drawFunc(label, size);
        if (ret) dir = (q * glm::vec3(1, 0, 0)) * len ; //return vector with original lenght

        return ret;
    }
    // Vec4 (xyz axis, w angle) -> quat -> trackbalTransforms -> quat -> vec4
    ////////////////////////////////////////////////////////////////////////////
    bool getTransforms(glm::quat& q, const char* label, glm::vec4& axis_angle, float size) {
        q = angleAxis(axis_angle.w,glm::vec3(axis_angle)); //g.ConvertFromAxisAngle();
   
        bool ret = drawFunc(label, size);
        if (ret) axis_angle = glm::vec4(glm::vec3(glm::axis(q)),glm::angle(q));

        return ret; 
    }
 
    //
    //  Settings
    //
    //      axes/arrow are composed of cone (or pyramid) and cylinder 
    //      (or parallelepiped): this solid are builded at first instance
    //      and will have same slices/radius/length for all controls in your 
    //      applications but can be  resized proportionally with a reductin
    //      factor: resizeAxesOf and resizeSolidOf (below).
    //      Also the colors of sphere tessellation are set at buil time, 
    //      while colors of axes and cube are fixed
    //
    //      if you want change solids attributes, change the static variables.
    //      If you need to resize solid and axes use resizeAxesOf and 
    //      resizeSolidOf, they works like push/pop stack (without buffer!) 
    //      with respective restoreAxesSize and restoreSolidSize.
    //      for example:
    //          // reDim axes ... same lenght, 
    //          imguiGizmo::resizeAxesOf(glm::vec3(imguiGizmo::axesResizeFactor.x, 2.0, 2.0)); 
    //          imguiGizmo::resizeSolidOf(1.25); // sphere bigger
    //          ImGui::gizmo3D("##RotB", b,sz);   
    //          imguiGizmo::restoreSolidSize(); // restore at default
    //          imguiGizmo::restoreAxesSize();
    ////////////////////////////////////////////////////////////////////////////

    //
    //  Build solid components
    //
    ////////////////////////////////////////////////////////////////////////////

    // arrow/axes components
    ///////////////////////////////////////
    static int   coneSlices ;
    static float coneRadius;
    static float coneLength;

    static int   cylSlices  ;
    static float cylRadius ;  // sizeCylLength ... defined in base to control size    

    // Sphere components
    ///////////////////////////////////////
    static float sphereRadius;
    static int sphereTessFactor;

    // Cube components
    ///////////////////////////////////////
    static float cubeSide    ;

    //
    //  Resizing and color settings
    //
    ////////////////////////////////////////////////////////////////////////////


    // Axes reduction
    ///////////////////////////////////////
    static glm::vec3 axesResizeFactor;
    static glm::vec3 savedAxesResizeFactor;

    // solid reduction
    ///////////////////////////////////////
    static float solidResizeFactor;
    static float savedSolidResizeFactor;


    static ImU32 sphereColors[2]; // Tessellation colors
    static ImU32 savedSphereColors[2];
    //ImU32 spherecolorA=0xff005cc0, spherecolorB=0xffc05c00;
    
    static ImVec4 directionColor;
    static ImVec4 savedDirectionColor;

    static const int imguiGizmoDefaultSize;
};




#define IMGUIZMO_DEF_SIZE (ImGui::GetFrameHeightWithSpacing()*4 - ImGui::GetStyle().ItemSpacing.y*2)

namespace ImGui
{

IMGUI_API bool gizmo3D(const char*, glm::quat&, float=IMGUIZMO_DEF_SIZE, const int=imguiGizmo::mode3Axes|imguiGizmo::cubeAtOrigin);
IMGUI_API bool gizmo3D(const char*, glm::vec4&, float=IMGUIZMO_DEF_SIZE, const int=imguiGizmo::mode3Axes|imguiGizmo::cubeAtOrigin);
IMGUI_API bool gizmo3D(const char*, glm::vec3&, float=IMGUIZMO_DEF_SIZE, const int=imguiGizmo::modeDirection);

IMGUI_API bool gizmo3D(const char*, glm::quat&, glm::quat&, float=IMGUIZMO_DEF_SIZE, const int=imguiGizmo::modeDual|imguiGizmo::cubeAtOrigin);
IMGUI_API bool gizmo3D(const char*, glm::quat&, glm::vec4&, float=IMGUIZMO_DEF_SIZE, const int=imguiGizmo::modeDual|imguiGizmo::cubeAtOrigin);
IMGUI_API bool gizmo3D(const char*, glm::quat&, glm::vec3&, float=IMGUIZMO_DEF_SIZE, const int=imguiGizmo::modeDual|imguiGizmo::cubeAtOrigin);

};

//#undef imguiGizmo_DEF_SIZE