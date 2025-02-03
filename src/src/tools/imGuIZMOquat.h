//------------------------------------------------------------------------------
//  Copyright (c) 2018-2025 Michele Morrone
//  All rights reserved.
//
//  https://michelemorrone.eu - https://brutpitt.com
//
//  X: https://x.com/BrutPitt - GitHub: https://github.com/BrutPitt
//
//  direct mail: brutpitt(at)gmail.com - me(at)michelemorrone.eu
//
//  This software is distributed under the terms of the BSD 2-Clause license
//------------------------------------------------------------------------------
#if !defined(__IMGUIZMOQUAT_H__)
#define __IMGUIZMOQUAT_H__

#include <algorithm>
//#include <cfloat>

#include "imguizmo_config.h"    // used (also) to modify/specify ImGui include directory

#if defined(IMGUIZMO_USES_GLM) && !defined(VGIZMO_USES_GLM)
    #define VGIZMO_USES_GLM
#endif

#include "vGizmo3D.h"

#ifndef IMGUIZMO_MANUAL_IMGUI_INCLUDE
    #if !defined(IMGUIZMO_IMGUI_FOLDER)
        #define IMGUIZMO_IMGUI_FOLDER imgui/
    #endif

    #define GET_PATH(P) P
    #define INC_PATH(X) <GET_PATH(IMGUIZMO_IMGUI_FOLDER)X>

    #include INC_PATH(imgui.h)
    #include INC_PATH(imgui_internal.h)
#endif

#ifndef IMGUIZMO_VMOD_AXIS_X
    #define IMGUIZMO_VMOD_AXIS_X
#endif
#ifndef IMGUIZMO_VMOD_AXIS_Y
    #define IMGUIZMO_VMOD_AXIS_Y
#endif
#ifndef IMGUIZMO_VMOD_AXIS_Z
    #define IMGUIZMO_VMOD_AXIS_Z
#endif


#define IMGUI_DEFINE_MATH_OPERATORS

//--------------------------------------------------------------------------
//
//    NEED TO BUILD :
//
//    - if using glm (OpenGL Mathematics): need version 0.9.9 or later
//        and GLM_ENABLE_EXPERIMENTAL compile option to enable "quaternion" class
//    - Using include virtualGizmo.h (my virtualGizmoClass) to get input controls
//          and apply rotations
//    - use c++11 standard
//
//    Use preprocessor define imguiGizmo_INTERPOLATE_NORMALS, to a phong color effect
//    for solid components, but there are more vertex memorized and more operations,
//    even if the visual effect is better... the computational difference is:
//    one normal for vertex  VS  one normal for plain figure (triangle or quad)
//    
//    comment/uncomment below or add as directive to compiler
//--------------------------------------------------------------------------

#define imguiGizmo_INTERPOLATE_NORMALS
#define STARTING_ALPHA_PLANE .75f

//--------------------------------------------------------------------------
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
//--------------------------------------------------------------------------

// The data structure that holds the orientation among other things
struct imguiGizmo
{
    imguiGizmo() {
        #if defined(IMGUIZMO_FLIP_ROT_X)
            isFlipRotX = true;
        #endif
        #if defined(IMGUIZMO_FLIP_ROT_Y)
            isFlipRotY = true;
        #endif
        #if defined(IMGUIZMO_FLIP_PAN_X)
            isFlipPanX = true;
        #endif
        #if defined(IMGUIZMO_FLIP_PAN_Y)
            isFlipPanY = true;
        #endif
        #if defined(IMGUIZMO_FLIP_DOLLY)
            isFlipDolly = true;
        #endif
    }
    quat qtV  = quat(1.0f, vec3(0.0f)); // Quaternion value
    quat qtV2 = quat(1.0f, vec3(0.0f)); // Quaternion value
#ifndef IMGUIZMO_USE_ONLY_ROT
    vec3 posPanDolly = vec3(0.f);
    vgButtons buttonPanDolly = vg::evLeftButton;
#endif
    vec3 viewVecModifier{ IMGUIZMO_VMOD_AXIS_X 1, IMGUIZMO_VMOD_AXIS_Y 1, IMGUIZMO_VMOD_AXIS_Z 1 };

    enum      {                              //0b0000'0000, //C++14 notation
                mode3Axes          = 0x0001, //0b0000'0001,
                modeDirection      = 0x0002, //0b0000'0010,
                modeDirPlane       = 0x0004, //0b0000'0100,
                modeDual           = 0x0008, //0b0000'1000,
                modePanDolly       = 0x0010, //0b0001'0000,
                modeMask           = 0x00ff, 
                

                cubeAtOrigin       = 0x0100, //0b0000'0000, 
                sphereAtOrigin     = 0x0200, //0b0001'0000,
                noSolidAtOrigin    = 0x0400, //0b0010'0000,
                modeFullAxes       = 0x0800,
                axesModeMask       = 0xff00  
    };

    enum { sphereTess16, sphereTess8, sphereTess4, sphereTess2 };
    enum { CONE_SURF, CONE_CAP, CYL_SURF, CYL_CAP };
    //enum { SOLID_SURF, SOLID_CAP }
    //enum { 
    enum { axisIsX, axisIsY, axisIsZ };

    enum solidSides{ backSide, frontSide  }; // or viceversa... 
    static ImVector<vec3> sphereVtx;
    static ImVector<int>  sphereTess;
    static ImVector<vec3> cubeVtx;
    static ImVector<vec3> cubeNorm;
    static ImVector<vec3> planeVtx;
    static ImVector<vec3> planeNorm;
    static ImVector<vec3> arrowVtx[4];
    static ImVector<vec3> arrowNorm[4];
    static void buildPlane   (const float size, const float thickness = planeThickness) {
        buildPolygon(vec3(thickness,size,size), planeVtx, planeNorm);
    }
    static void buildCube    (const float size) {
        buildPolygon(vec3(size), cubeVtx, cubeNorm);
    }
    static void buildPolygon (const vec3& size,ImVector<vec3>& vtx,ImVector<vec3>& norm);
    static void buildSphere  (float radius, int tessFactor);
    static void buildCone    (float x0, float x1, float radius, int slices);
    static void buildCylinder(float x0, float x1, float radius, int slices);
    
    //-------------------------------------
    // helper functions

///
/// Resize length and thickness of widget's axis/axes
/// @param[in]  newSize  vec3(axesLen, axesThickness, coneThickness)
///
/// default value = vec3(1.0, 1.0, 1.0) : bigger > 1.0, smaller < 1.0 <br>
/// @note "axesLen" can be only reduced to avoid that axes go over widget borders
///
/// @code
///        imguiGizmo::resizeAxesOf(vec3(.5, 2, 3));    //  vec3(axesLen, axesThick, coneThick)
///        ImGui::gizmo3D("##a11", position, rotation);
///                                                     // until not restoring all widgets are draw with this sizes
///        imguiGizmo::restoreAxesSize();               // restore at default axes length
/// @endcode
/// @note There is no a stack: a new call (w/o restoring) overwrite default values with previous ones
    static void resizeAxesOf(const vec3 &newSize) { savedAxesResizeFactor = axesResizeFactor; axesResizeFactor = newSize; }

/// Restore length and thickness of widget's axis/axes to default/previous value
///
/// @code
///        imguiGizmo::resizeAxesOf(vec3(.5, 2, 3));    //  vec3(axesLen, axesThick, coneThick)
///        ImGui::gizmo3D("##a11", position, rotation);
///                                                     // until not restoring all widgets are draw with this sizes
///        imguiGizmo::restoreAxesSize();               // restore at default axes length
/// @endcode
/// @note There is no a stack: a new call (w/o restoring) overwrite default values with previous ones
    static void restoreAxesSize() {
        axesResizeFactor = savedAxesResizeFactor; }

    static void resizeSolidOf(float newSize) {
        savedSolidResizeFactor = solidResizeFactor; solidResizeFactor = newSize; }
    static void restoreSolidSize() {
        solidResizeFactor = savedSolidResizeFactor; }

    static void setDirectionColor(ImU32 dColor, const ImU32 pColor) {
        setDirectionColor(ImGui::ColorConvertU32ToFloat4(dColor), ImGui::ColorConvertU32ToFloat4(pColor)); }
    static void setDirectionColor(const ImVec4 &dColor, const ImVec4 &pColor) {
        savedDirectionColor = directionColor; savedPlaneColor = planeColor; 
        directionColor = dColor; planeColor = pColor;
    }
    static void setDirectionColor(ImU32 color) { setDirectionColor(ImGui::ColorConvertU32ToFloat4(color)); } 
    static void setDirectionColor(const ImVec4& color) { setDirectionColor(color,ImVec4(color.x, color.y, color.z, STARTING_ALPHA_PLANE));  }
    static void restoreDirectionColor() {
        directionColor = savedDirectionColor; 
        planeColor     = savedPlaneColor;     }

    static void setSphereColors(const ImVec4& a, const ImVec4& b) {
        setSphereColors( ImGui::ColorConvertFloat4ToU32(a), ImGui::ColorConvertFloat4ToU32(b)); }    
    static void setSphereColors(ImU32 a, ImU32 b) {
        savedSphereColors[0] = sphereColors[0]; savedSphereColors[1] = sphereColors[1];
        sphereColors[0] = a; sphereColors[1] = b; }
    static void restoreSphereColors() {
        sphereColors[0] = savedSphereColors[0]; sphereColors[1] = savedSphereColors[1]; }


    //  gizmo mouse/key settings
    //--------------------------------------------------------------------------
    // Call it once, to set all widgets... or if you need it 
    static void setGizmoFeelingRot(float f) { gizmoFeelingRot = f; } // default 1.0, >1 more mouse sensitivity, <1 less mouse sensitivity
    static float getGizmoFeelingRot() { return gizmoFeelingRot; }

#ifndef IMGUIZMO_USE_ONLY_ROT
// available vgModifiers values:
//      evShiftModifier   -> Shift
//      evControlModifier -> Ctrl
//      evAltModifier     -> Alt
//      evSuperModifier   -> Super
    static void setPanModifier  (vgModifiers v) { panMod   = v; }    // Change default assignment for Pan
    static void setDollyModifier(vgModifiers v) { dollyMod = v; }  // Change default assignment for Dolly

    //  Set the mouse response for the dolly operation...  also wheel
    static void  setDollyScale(float  scale) { dollyScale = scale;  } // default 1.0, >1 more, <1 less
    static float getDollyScale() { return dollyScale;  }
    //  Set the wheel response for the dolly operation...  also wheel
    static void  setDollyWheelScale(float  scale) { dollyWheelScale = scale;  } // default 1.0, >1 more, <1 less
    static float getDollyWheelScale() { return dollyWheelScale;  }
    //  Set the mouse response for pan
    static void  setPanScale(float scale) { panScale = scale; } // default 1.0, >1 more, <1 less
    static float getPanScale() { return panScale; }
#endif

/// flipX mouse coord
///@param[in] b bool
    static void setFlipRotX(bool b) { isFlipRotX = b; }
/// flipY mouse coord
///@param[in] b bool
    static void setFlipRotY(bool b) { isFlipRotY = b; }
/// flipZ mouse coord
///@param[in] b bool
    static void setFlipDolly(bool b) { isFlipDolly = b; }
/// flipZ mouse coord
///@param[in] b bool
    static void setFlipPanX(bool b) { isFlipPanX = b; }
/// flipZ mouse coord
///@param[in] b bool
    static void setFlipPanY(bool b) { isFlipPanY = b; }

/// get flip Rot X status
/// @retval bool : current flip Rot X status
    static bool getFlipRotX() { return isFlipRotX; }
/// get flip Rot Y status
/// @retval bool : current flip Rot Y status
    static bool getFlipRotY() { return isFlipRotY; }
/// get flip Pan X status
/// @retval bool : current flip Pan X status
    static bool getFlipPanX() { return isFlipPanX; }
/// get flip Pan Y status
/// @retval bool : current flip Pan Y status
    static bool getFlipPanY() { return isFlipPanY; }
/// get flipZ mouse status
/// @retval bool : current flipZ status
    static bool getFlipDolly() { return isFlipDolly; }

    //  internals
    //--------------------------------------------------------------------------
    static bool solidAreBuilt;
    static bool dragActivate;

    int drawMode = mode3Axes;
    int axesOriginType = cubeAtOrigin;
    bool showFullAxes = false;

    bool drawFunc(const char* label, float size);

    void modeSettings(uint32_t mode) {
        drawMode = uint32_t(mode & modeMask); axesOriginType = uint32_t(mode & axesModeMask); showFullAxes = bool(modeFullAxes & mode); }

    void setDualMode(const uint32_t mode) { modeSettings((imguiGizmo::modeDual | imguiGizmo::axesModeMask) & (mode | imguiGizmo::modeDual)); }

    // vec3 -> quat -> trackbalTransforms -> quat -> vec3
    //--------------------------------------------------------------------------
    bool getTransforms(quat& q, const char* label, vec3& dir, float size) {
        float len = length(dir);

        if(len<1.0 && len>= FLT_EPSILON) { normalize(dir); len = 1.0; }
        else if(len< FLT_EPSILON) { dir = vec3(1.f, 0.f, 0.f); len = 1.0; }

#ifdef IMGUIZMO_HAS_NEGATIVE_VEC3_LIGHT
        q = angleAxis(acosf( dir.x/len), normalize(vec3(FLT_EPSILON, -dir.z,  dir.y)));
        bool ret = drawFunc(label, size);
        if (ret) dir = (q * vec3( 1.0f, 0.0f, 0.0f)) * len ; //return vector with original lenght
#else
        q = angleAxis(acosf(-dir.x/len), normalize(vec3(FLT_EPSILON, dir.z, -dir.y)));
        bool ret = drawFunc(label, size);
        if (ret) dir = (q * vec3(-1.0f, 0.0f, 0.0f)) * len ; //return vector with original lenght
#endif


        return ret;
    }
    // Vec4 (xyz axis, w angle) -> quat -> trackbalTransforms -> quat -> vec4
    //--------------------------------------------------------------------------
    bool getTransforms(quat& q, const char* label, vec4& axis_angle, float size) {
        q = angleAxis(axis_angle.w,normalize(vec3(axis_angle))); //g.ConvertFromAxisAngle();
   
        bool ret = drawFunc(label, size);
        if (ret) axis_angle = vec4(normalize(axis(q)),angle(normalize(q)));

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
    //          imguiGizmo::resizeAxesOf(vec3(imguiGizmo::axesResizeFactor.x, 2.0, 2.0)); 
    //          imguiGizmo::resizeSolidOf(1.25); // sphere bigger
    //          ImGui::gizmo3D("##RotB", b,sz);   
    //          imguiGizmo::restoreSolidSize(); // restore at default
    //          imguiGizmo::restoreAxesSize();
    //--------------------------------------------------------------------------

    //
    //  Build solid components
    //
    //--------------------------------------------------------------------------

    // arrow/axes components
    //-------------------------------------
    static int   coneSlices ;
    static float coneRadius;
    static float coneLength;

    static int   cylSlices  ;
    static float cylRadius ;  // sizeCylLength ... defined in base to control size    

    // Sphere components
    //-------------------------------------
    static float sphereRadius;
    static int sphereTessFactor;

    // Cube components
    //-------------------------------------
    static float cubeSize;

    // Plane components
    //-------------------------------------
    static float planeSize;
    static float planeThickness;


    //
    //  Resizing and color settings
    //
    //--------------------------------------------------------------------------


    // Axes reduction
    //-------------------------------------
    static vec3 axesResizeFactor;
    static vec3 savedAxesResizeFactor;

    // solid reduction
    //-------------------------------------
    static float solidResizeFactor;
    static float savedSolidResizeFactor;


    static ImU32 sphereColors[2]; // Tessellation colors
    static ImU32 savedSphereColors[2];
    //ImU32 spherecolorA=0xff005cc0, spherecolorB=0xffc05c00;
    
    static ImVec4 directionColor;
    static ImVec4 savedDirectionColor;

    static ImVec4 planeColor;
    static ImVec4 savedPlaneColor;

    // Gizmo mouse settings
    //-------------------------------------
    static float gizmoFeelingRot; // >1 more mouse sensibility, <1 less mouse sensibility
#ifndef IMGUIZMO_USE_ONLY_ROT
    static float panScale, dollyScale, dollyWheelScale, dollyWheelMulFactor;
    static vgModifiers panMod, dollyMod;
#endif


    // Flipping mouse control
    //-------------------------------------
    static bool isFlipRotX , isFlipRotY;
    static bool isFlipPanX , isFlipPanY, isFlipDolly;


    static const int imguiGizmoDefaultSize;

};

#define IMGUIZMO_DEF_SIZE (ImGui::GetFrameHeightWithSpacing()*4 - (ImGui::GetStyle().ItemSpacing.y*2))

namespace ImGui
{
/// <b>Widget 3 axes</b><br>
/// <b>quat</b> (quaternion) axes rotation<br>
///
/// @param[in]     t     <b> const char *</b> - widget text: put "##" before the text to hide it
/// @param[in,out] q     <b> quat &      </b> - quaternion with axes rotation
/// @param[in]     sz    <b> float       </b> - widget size: default size = FrameHeightWithSpacing*4 - ItemSpacing.y*2
/// @param[in]     flag  <b> uint32_t    </b> - masked flags to set modes (lower 16bit) and aspect (higher 16bit)
/// @code
/// #include <imguizmo_quat/imguizmo_quat.h>
/// ...
/// static quat rotation(1,0,0,0);
///
/// // inside ImGui a frame
/// ImGui::gizmo3D("##widget", rotation, /* size */ 240);
/// @endcode
IMGUI_API bool gizmo3D(const char* t, quat& q, float sz=IMGUIZMO_DEF_SIZE, uint32_t flag=imguiGizmo::mode3Axes|imguiGizmo::cubeAtOrigin);

/// <b>Widget 3 axes</b><br>
/// <b>vec4</b> p(xyz), w angle in radians for the spot<br>
///
/// @param[in]     t     <b> const char *</b> - widget text: put "##" before the text to hide it
/// @param[in,out] v     <b> vec4 &      </b> - vec4 p(xyz), w angle in radians
/// @param[in]     sz    <b> float       </b> - widget size: default size = FrameHeightWithSpacing*4 - ItemSpacing.y*2
/// @param[in]     flag  <b> uint32_t    </b> - masked flags to set modes (lower 16bit) and aspect (higher 16bit)
/// @code
/// #include <imguizmo_quat/imguizmo_quat.h>
/// ...
/// static vec4 direction(1,0,0,1);
///
/// // inside ImGui a frame
/// ImGui::gizmo3D("##widget", direction, /* size */ 240);
/// @endcode
IMGUI_API bool gizmo3D(const char* t, vec4& v, float sz=IMGUIZMO_DEF_SIZE, uint32_t flag=imguiGizmo::mode3Axes|imguiGizmo::cubeAtOrigin);

/// <b>Widget directional arrow</b><br>
/// <b>vec3</b> vec3 p(xyz) (center oriented)  
///
/// @param[in]     t     <b> const char *</b> - widget text: put "##" before the text to hide it
/// @param[in,out] v     <b> vec3 &      </b> - p(xyz) position/direction (center oriented)
/// @param[in]     sz    <b> float       </b> - widget size: default size = FrameHeightWithSpacing*4 - ItemSpacing.y*2
/// @param[in]     flag  <b> uint32_t    </b> - masked flags to set modes (lower 16bit) and aspect (higher 16bit)
/// @code
/// #include <imguizmo_quat/imguizmo_quat.h>
/// ...
/// static vec4 direction(1,0,0);
///
/// // inside ImGui a frame
/// ImGui::gizmo3D("##widget", direction, /* size */ 240);
/// @endcode
IMGUI_API bool gizmo3D(const char* t, vec3& v, float sz=IMGUIZMO_DEF_SIZE, uint32_t flag=imguiGizmo::modeDirection);

/// <b>Widget 3 axes + spot light</b><br>
/// <b>quat</b> (quaternion) axes rotation<br>
/// <b>quat</b> rotation for the spot<br>
///
/// @param[in]     t     <b> const char *</b> - widget text: put "##" before the text to hide it
/// @param[in,out] q     <b> quat &      </b> - quaternion with axes rotation
/// @param[in,out] ql    <b> vec4 &      </b> - quaternion with spot rotation
/// @param[in]     sz    <b> float       </b> - widget size: default size = FrameHeightWithSpacing*4 - ItemSpacing.y*2
/// @param[in]     flag  <b> uint32_t    </b> - masked flags to set modes (lower 16bit) and aspect (higher 16bit)
/// @code
/// #include <imguizmo_quat/imguizmo_quat.h>
/// ...
/// static quat rotation(1,0,0,0);
/// static quat lightRot(1,0,0,0);
///
/// // inside ImGui a frame
/// ImGui::gizmo3D("##widget", rotation, lightRot, /* size */ 240);
/// @endcode
IMGUI_API bool gizmo3D(const char* t, quat& q, quat& ql, float sz=IMGUIZMO_DEF_SIZE, uint32_t flag=imguiGizmo::modeDual|imguiGizmo::cubeAtOrigin);

/// <b>Widget 3 axes + spot light</b><br>
/// <b>quat</b> (quaternion) axes rotation<br>
/// <b>vec4</b> p(xyz), w angle in radians for the spot
///
/// @param[in]     t     <b> const char *</b> - widget text: put "##" before the text to hide it
/// @param[in,out] q     <b> quat &      </b> - quaternion with axes rotation
/// @param[in,out] v     <b> vec4 &      </b> - vec4 for the spot: p(xyz), w angle in radians
/// @param[in]     sz    <b> float       </b> - widget size: default size = FrameHeightWithSpacing*4 - ItemSpacing.y*2
/// @param[in]     flag  <b> uint32_t    </b> - masked flags to set modes (lower 16bit) and aspect (higher 16bit)
/// @code
/// #include <imguizmo_quat/imguizmo_quat.h>
/// ...
/// static quat rotation(1,0,0,0);
/// static vec4 lightDir(1,0,0,1);
///
/// // inside ImGui a frame
/// ImGui::gizmo3D("##widget", rotation, lightDir, /* size */ 240);
/// @endcode
IMGUI_API bool gizmo3D(const char* t, quat& q, vec4& v , float sz=IMGUIZMO_DEF_SIZE, uint32_t flag=imguiGizmo::modeDual|imguiGizmo::cubeAtOrigin);

/// <b>Widget 3 axes + spot light</b><br>
/// <b>quat</b> (quaternion) axes rotation<br>
/// <b>vec3</b> vec3 p(xyz) for the spot 
///
/// @param[in]     t     <b> const char *</b> - widget text: put "##" before the text to hide it
/// @param[in,out] q     <b> quat &      </b> - quaternion with axes rotation
/// @param[in,out] v     <b> vec3 &      </b> - vec3 for the spot: p(xyz)
/// @param[in]     sz    <b> float       </b> - widget size: default size = FrameHeightWithSpacing*4 - ItemSpacing.y*2
/// @param[in]     flag  <b> uint32_t    </b> - masked flags to set modes (lower 16bit) and aspect (higher 16bit)
/// @code
/// #include <imguizmo_quat/imguizmo_quat.h>
/// ...
/// static quat rotation(1,0,0,0);
/// static vec3 lightDir(1,0,0,1);
///
/// // inside ImGui a frame
/// ImGui::gizmo3D("##widget", rotation, lightDir, /* size */ 240);
/// @endcode
IMGUI_API bool gizmo3D(const char* t, quat& q, vec3& v , float sz=IMGUIZMO_DEF_SIZE, uint32_t flag=imguiGizmo::modeDual|imguiGizmo::cubeAtOrigin);

#ifndef IMGUIZMO_USE_ONLY_ROT

//with Pan & Dolly feature
/// <b>Widget 3 axes + Pan & Dolly/Zoom capability</b><br>
/// <b>vec3</b> Pan/Dolly: (x, y) Pan position, (z) Dolly/Zoom position<br>
/// <b>quat</b> (quaternion) axes rotation<br>
///
/// @param[in]     t     <b> const char *</b> - widget text: put "##" before the text to hide it
/// @param[in,out] vm    <b> vec3 &      </b> - Pan/Dolly position: (x, y) Pan position, (z) Dolly/Zoom position
/// @param[in,out] q     <b> quat &      </b> - quaternion with axes rotation
/// @param[in]     sz    <b> float       </b> - widget size: default size = FrameHeightWithSpacing*4 - ItemSpacing.y*2
/// @param[in]     flag  <b> uint32_t    </b> - masked flags to set modes (lower 16bit) and aspect (higher 16bit)
/// @code
/// #include <imguizmo_quat/imguizmo_quat.h>
/// ...
/// static quat rotation(1,0,0,0);
/// static vec3 position;
///
/// // inside ImGui a frame
/// ImGui::gizmo3D("##widget", position, rotation, /* size */ 240);
/// @endcode
IMGUI_API bool gizmo3D(const char* t, vec3& vm, quat& q, float sz=IMGUIZMO_DEF_SIZE, uint32_t flag=imguiGizmo::mode3Axes|imguiGizmo::cubeAtOrigin);

/// <b>Widget 3 axes + Pan & Dolly/Zoom capability</b><br>
/// <b>vec3</b> Pan/Dolly: (x, y) Pan position, (z) Dolly/Zoom position<br>
/// <b>vec4</b> p(xyz), w angle in radians for the spot<br>
///
/// @param[in]     t     <b> const char *</b> - widget text: put "##" before the text to hide it
/// @param[in,out] vm    <b> vec3 &      </b> - Pan/Dolly position: (x, y) Pan position, (z) Dolly/Zoom position
/// @param[in,out] v     <b> vec4 &      </b> - vec4 for direction: p(xyz), w angle in radians
/// @param[in]     sz    <b> float       </b> - widget size: default size = FrameHeightWithSpacing*4 - ItemSpacing.y*2
/// @param[in]     flag  <b> uint32_t    </b> - masked flags to set modes (lower 16bit) and aspect (higher 16bit)
/// @code
/// #include <imguizmo_quat/imguizmo_quat.h>
/// ...
/// static vec4 direction(1,0,0,1);
/// static vec3 position;
///
/// // inside ImGui a frame
/// ImGui::gizmo3D("##widget", position, direction, /* size */ 240);
/// @endcode
IMGUI_API bool gizmo3D(const char* t, vec3& vm, vec4& v, float sz=IMGUIZMO_DEF_SIZE, uint32_t flag=imguiGizmo::mode3Axes|imguiGizmo::cubeAtOrigin);

/// <b>Widget directional arrow + Pan & Dolly/Zoom capability</b><br>
/// <b>vec3</b> Pan/Dolly: (x, y) Pan position, (z) Dolly/Zoom position<br>
/// <b>vec3</b> vec3 p(xyz) (center oriented)  
///
/// @param[in]     t     <b> const char *</b> - widget text: put "##" before the text to hide it
/// @param[in,out] vm    <b> vec3 &      </b> - Pan/Dolly position: (x, y) Pan position, (z) Dolly/Zoom position
/// @param[in,out] v     <b> vec3 &      </b> - p(xyz) position/direction (center oriented)
/// @param[in]     sz    <b> float       </b> - widget size: default size = FrameHeightWithSpacing*4 - ItemSpacing.y*2
/// @param[in]     flag  <b> uint32_t    </b> - masked flags to set modes (lower 16bit) and aspect (higher 16bit)
/// @code
/// #include <imguizmo_quat/imguizmo_quat.h>
/// ...
/// static vec4 direction(1,0,0);
/// static vec3 position;
///
/// // inside ImGui a frame
/// ImGui::gizmo3D("##widget", position, direction, /* size */ 240);
/// @endcode
IMGUI_API bool gizmo3D(const char* t, vec3& vm, vec3& v, float sz=IMGUIZMO_DEF_SIZE, uint32_t flag=imguiGizmo::modeDirection);

/// <b>Widget 3 axes + spot light + Pan & Dolly/Zoom capability</b><br>
/// <b>vec3</b> Pan/Dolly: (x, y) Pan position, (z) Dolly/Zoom position<br>
/// <b>quat</b> (quaternion) axes rotation<br>
/// <b>quat</b> rotation for the spot<br>
///
/// @param[in]     t     <b> const char *</b> - widget text: put "##" before the text to hide it
/// @param[in,out] vm    <b> vec3 &      </b> - Pan/Dolly position: (x, y) Pan position, (z) Dolly/Zoom position
/// @param[in,out] q     <b> quat &      </b> - quaternion with axes rotation
/// @param[in,out] ql    <b> quat &      </b> - quaternion with spot rotation
/// @param[in]     sz    <b> float       </b> - widget size: default size = FrameHeightWithSpacing*4 - ItemSpacing.y*2
/// @param[in]     flag  <b> uint32_t    </b> - masked flags to set modes (lower 16bit) and aspect (higher 16bit)
/// @code
/// #include <imguizmo_quat/imguizmo_quat.h>
/// ...
/// static quat rotation(1,0,0,0);
/// static quat lightRot(1,0,0,0);
/// static vec3 position;
///
/// // inside ImGui a frame
/// ImGui::gizmo3D("##widget", position, rotation, lightRot, /* size */ 240);
/// @endcode
IMGUI_API bool gizmo3D(const char* t, vec3& vm, quat& q, quat& ql, float sz=IMGUIZMO_DEF_SIZE, uint32_t flag=imguiGizmo::modeDual|imguiGizmo::cubeAtOrigin);

/// <b>Widget 3 axes + spot light + Pan & Dolly/Zoom capability</b><br>
/// <b>vec3</b> Pan/Dolly: (x, y) Pan position, (z) Dolly/Zoom position<br>
/// <b>quat</b> (quaternion) axes rotation<br>
/// <b>vec4</b> p(xyz), w angle in radians for the spot<br>
///
/// Builds a ImGuIZMO_quat 3 axes + spot light from/with a "q" (quaternion) rotations of axes and a vec4 (xyz axis, w angle in radians) for the spot + Pan & Dolly/Zoom capability
///
/// @param[in]     t     <b> const char *</b> - widget text: put "##" before the text to hide it
/// @param[in,out] vm    <b> vec3 &      </b> - Pan/Dolly position: (x, y) Pan position, (z) Dolly/Zoom position
/// @param[in,out] q     <b> quat &      </b> - quaternion with axes rotation
/// @param[in,out] v     <b> vec4 &      </b> - vec4 for the spot: p(xyz), w angle in radians
/// @param[in]     sz    <b> float       </b> - widget size: default size = FrameHeightWithSpacing*4 - ItemSpacing.y*2
/// @param[in]     flag  <b> uint32_t    </b> - masked flags to set modes (lower 16bit) and aspect (higher 16bit)
/// @code
/// #include <imguizmo_quat/imguizmo_quat.h>
/// ...
/// static quat rotation(1,0,0,0);
/// static vec4 lightDir(1,0,0,1);
/// static vec3 position;
///
/// // inside ImGui a frame
/// ImGui::gizmo3D("##widget", position, rotation, lightDir, /* size */ 240);
/// @endcode
IMGUI_API bool gizmo3D(const char* t, vec3& vm, quat& q, vec4& v , float sz=IMGUIZMO_DEF_SIZE, uint32_t flag=imguiGizmo::modeDual|imguiGizmo::cubeAtOrigin);

/// <b>Widget 3 axes + spot light + Pan & Dolly/Zoom capability</b><br>
/// <b>vec3</b> Pan/Dolly: (x, y) Pan position, (z) Dolly/Zoom position<br>
/// <b>quat</b> (quaternion) axes rotation<br>
/// <b>vec3</b> vec3 p(xyz) for the spot 
///
/// @param[in]     t     <b> const char *</b> - widget text: put "##" before the text to hide it
/// @param[in,out] vm    <b> vec3 &      </b> - Pan/Dolly position: (x, y) Pan position, (z) Dolly/Zoom position
/// @param[in,out] q     <b> quat &      </b> - quaternion with axes rotation
/// @param[in,out] v     <b> vec3 &      </b> - vec3 for the spot: p(xyz)
/// @param[in]     sz    <b> float       </b> - widget size: default size = FrameHeightWithSpacing*4 - ItemSpacing.y*2
/// @param[in]     flag  <b> uint32_t    </b> - masked flags to set modes (lower 16bit) and aspect (higher 16bit)
/// @code
/// #include <imguizmo_quat/imguizmo_quat.h>
/// ...
/// static quat rotation(1,0,0,0);
/// static vec3 lightDir(1,0,0,1);
/// static vec3 position;
///
/// // inside ImGui a frame
/// ImGui::gizmo3D("##widget", position, rotation, lightDir, /* size */ 240);
/// @endcode
IMGUI_API bool gizmo3D(const char* t, vec3& vm, quat& q, vec3& v , float sz=IMGUIZMO_DEF_SIZE, uint32_t flag=imguiGizmo::modeDual|imguiGizmo::cubeAtOrigin);

#endif
}

//#undef imguiGizmo_DEF_SIZE

#endif