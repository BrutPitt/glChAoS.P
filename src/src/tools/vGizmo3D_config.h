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

////////////////////////////////////////////////////////////////////////////////
//  v G i z m o 3 D   C O N F I G   start

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
//#define VGIZMO_USES_GLM

//------------------------------------------------------------------------------
// uncomment to flip the mouse movement in a specific direction / operation:
//      VGIZMO3D_FLIP_ROT_X  -  Rotation on X movement - Default: OFF = FALSE
//      VGIZMO3D_FLIP_ROT_Y  -  Rotation on Y movement - Default: ON  = TRUE
//      VGIZMO3D_FLIP_PAN_X  -  Pan on X movement      - Default: OFF = FALSE
//      VGIZMO3D_FLIP_PAN_Y  -  Pan on Y movement      - Default: ON  = TRUE
//      VGIZMO3D_FLIP_Dolly  -  Dolly on Y movement    - Default: OFF = FALSE
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
//#define VGIZMO3D_FLIP_ROT_X
#define VGIZMO3D_FLIP_ROT_Y
//#define VGIZMO3D_FLIP_PAN_X
#define VGIZMO3D_FLIP_PAN_Y
//#define VGIZMO3D_FLIP_DOLLY

//  v G i z m o 3 D   C O N F I G   end
////////////////////////////////////////////////////////////////////////////////
