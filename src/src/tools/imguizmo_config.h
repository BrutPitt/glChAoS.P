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
#pragma once

//------------------------------------------------------------------------------
// imguizmo_quat configuration file
//
// Obviously all #define can be passed directly to compiler with -D option
//
//------------------------------------------------------------------------------



//------------------------------------------------------------------------------
// v3.0 and later
//
//      IMGUIZMO_USE_ONLY_ROT
//
//      Used to remove Pan & Dolly feature to imGuIZMO.quat widget and to use
//          only rotation feature (like v2.2 and above)
//
//          Pan/Dolly use virtualGizmo3DClass just a little bit complex of
//          virtualGizmoClass that uses only "quat" rotations
//          uncomment for very low resources ==> Pan & Dolly will be disabled
//
// Default ==> Pan & Dolly enabled
//------------------------------------------------------------------------------
//#define IMGUIZMO_USE_ONLY_ROT

//------------------------------------------------------------------------------
// v3.0 and later ==> from 3.1 non more strictly necessary
//                                              (maintained for compatibility)
//
//      IMGUIZMO_IMGUI_FOLDER
//
//      used to specify where ImGui include files should be searched
//          #define IMGUIZMO_IMGUI_FOLDER
//              is equivalent to use:
//                  #include <imgui.h>
//                  #include <imgui_internal.h>
//
//          #define IMGUIZMO_IMGUI_FOLDER myLibs/ImGui/
//              (final slash is REQUIRED) is equivalent to use:
//                  #include <myLib/ImGui/imgui.h>
//                  #include <myLib/ImGui/imgui_internal.h>
//
//          Default: IMGUIZMO_IMGUI_FOLDER undefined (commented)
//              is equivalent to use:
//                  #include <imgui/imgui.h>
//                  #include <imgui/imgui_internal.h>
//
// N.B. Final slash to end of path is REQUIRED!
//------------------------------------------------------------------------------
// #define IMGUIZMO_IMGUI_FOLDER ImGui/

//------------------------------------------------------------------------------
// v3.1 and later
//
//      IMGUIZMO_MANUAL_IMGUI_INCLUDE
//
//        - if DEFINED (uncommented/defined) is necessary to specify the following
//            ImGui includes:
//                  #include <imgui.h>
//                  #include <imgui_internal.h>
//            or
//                  #include <imgui_folder/imgui.h>
//                  #include <imgui_folder/imgui_internal.h>
//            or
//                  #include "imgui_folder/imgui.h"
//                  #include "imgui_folder/imgui_internal.h"
//
//            in relation to where is your imgui_folder and if is in your "INCLUDE"
//              search paths
//
//          if DEFINED (uncommented/defined) IMGUIZMO_IMGUI_FOLDER is NOT used
//            (see above)
//------------------------------------------------------------------------------
// #define IMGUIZMO_MANUAL_IMGUI_INCLUDE
// #include <imgui.h>
// #include <imgui_internal.h>

//------------------------------------------------------------------------------
// v3.1 and later
//
// Changes for:
// gizmo3D(const char* label, quat& axes, vec3& spot, float size, const int mode)
//
// Axes + spot-light with 3D vector now uses CORRECT light position:
//      NO MORE NEGATIVE POSITION
//
// If you have used it and want to maintain same behavior add follow define:
//
//      IMGUIZMO_HAS_NEGATIVE_VEC3_LIGHT
//
//------------------------------------------------------------------------------
#define IMGUIZMO_HAS_NEGATIVE_VEC3_LIGHT

//------------------------------------------------------------------------------
// v3.1 and later
//
// Show a different representation of one or more axes (only VISUAL representation)
//
//  IMGUIZMO_VMOD_AXIS_X +  ==> X grows from left  to right
//  IMGUIZMO_VMOD_AXIS_X -  ==> X grows from right to left
//  IMGUIZMO_VMOD_AXIS_Y +  ==> Y grows from down  to up
//  IMGUIZMO_VMOD_AXIS_Y -  ==> Y grows from up    to down
//  IMGUIZMO_VMOD_AXIS_Z +  ==> Z grows from far   to near  
//  IMGUIZMO_VMOD_AXIS_Z -  ==> Z grows from near  to far
//
//  Now you can also show (visually) a RIGHT_HAND system
//------------------------------------------------------------------------------
//#define IMGUIZMO_VMOD_AXIS_X +
//#define IMGUIZMO_VMOD_AXIS_Y +
//#define IMGUIZMO_VMOD_AXIS_Z +

//------------------------------------------------------------------------------
// v3.1 and later
//
// uncomment to flip the mouse movement in a specific direction / operation:
//      IMGUIZMO_FLIP_ROT_X  -  Rotation on X movement - Default: OFF = FALSE
//      IMGUIZMO_FLIP_ROT_Y  -  Rotation on Y movement - Default: ON  = TRUE
//      IMGUIZMO_FLIP_PAN_X  -  Pan on X movement      - Default: OFF = FALSE
//      IMGUIZMO_FLIP_PAN_Y  -  Pan on Y movement      - Default: ON  = TRUE
//      IMGUIZMO_FLIP_Dolly  -  Dolly on Y movement    - Default: OFF = FALSE
//
// The default values are set for use in OpenGL /WebGL
//
// Possibilty to override the default settings also inside of the code
// calling the relative function:
//    void setFlipRotX(bool b)  { isFlipRotX = b; }
//    void setFlipRotY(bool b)  { isFlipRotY = b; }
//    void setFlipPanX(bool b)  { isFlipPanX = b; }
//    void setFlipPanY(bool b)  { isFlipPanY = b; }
//    void setFlipDolly(bool b) { isFlipDolly = b; }
//
// Or acquire current status:
//    bool getFlipRotX() { return isFlipRotX; }
//    bool getFlipRotY() { return isFlipRotY; }
//    bool getFlipPanX() { return isFlipPanX; }
//    bool getFlipPanY() { return isFlipPanY; }
//    bool getFlipDolly() { return isFlipDolly; }
//
//------------------------------------------------------------------------------
//#define IMGUIZMO_FLIP_ROT_X
#define IMGUIZMO_FLIP_ROT_Y
//#define IMGUIZMO_FLIP_PAN_X
#define IMGUIZMO_FLIP_PAN_Y
//#define IMGUIZMO_FLIP_DOLLY

//------------------------------------------------------------------------------
// uncomment to use "glm" (0.9.9 or higher) library instead of vgMath
//      Need to have "glm" installed and in your INCLUDE research compiler path
//
// vgMath is a subset of "glm" and is compatible with glm types and calls
//      change only namespace from "vgm" to "glm". It's automatically set by
//      including vGizmo.h or vgMath.h or imGuIZMOquat.h
//
// note: affects only virtualGizmo3D / imGuIZMO.quat on which library to use
//      internally: vgMath | glm
//
// Default ==> use vgMath
//      If you enable GLM use, automatically is enabled also VGM_USES_TEMPLATE
//------------------------------------------------------------------------------
#define IMGIZMO_USES_GLM




// TODO for v.3.2?
//------------------------------------------------------------------------------
// v3.1 and later
//
//      IMGUIZMO_FLIP_LIGHT
//------------------------------------------------------------------------------

