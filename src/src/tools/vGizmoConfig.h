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

// EXPERIMENTAL ==> NOT FULL TESTED (YET)
//
// uncomment to use DOUBLE precision
//      It automatically enable also VGIZMO_USES_TEMPLATE (read below)
// Default ==> SINGLE precision: float
//------------------------------------------------------------------------------
//#define VGIZMO_USES_DOUBLE_PRECISION

// uncomment to use TEMPLATE internal vGizmoMath classes/types
//
// This is if you need to extend the use of different math types in your code
//      or for your purposes, there are predefined alias:
//          float  ==>  vec2 /  vec3 /  vec4 /  quat /  mat3|mat3x3  /  mat4|mat4x4
//      and more TEMPLATE (only!) alias:
//          double ==> dvec2 / dvec3 / dvec4 / dquat / dmat3|dmat3x3 / dmat4|dmat4x4
//          int    ==> ivec2 / ivec3 / ivec4
//          uint   ==> uvec2 / uvec3 / uvec4
// If you select TEMPLATE classes the widget too will use internally them 
//      with single precision (float)
//
// Default ==> NO template
//------------------------------------------------------------------------------
//#define VGIZMO_USES_TEMPLATE

// uncomment to use "glm" (0.9.9 or higher) library instead of vGizmoMath
//      Need to have "glm" installed and in your INCLUDE research compiler path
//
// vGizmoMath is a subset of "glm" and is compatible with glm types and calls
//      change only namespace from "vgm" to "glm". It's automatically set by
//      including vGizmo.h or vGizmoMath.h or imGuIZMOquat.h
//
// Default ==> use vGizmoMath
//      If you enable GLM use, automatically is enabled also VGIZMO_USES_TEMPLATE
//          if you can, I recommend to use GLM
//------------------------------------------------------------------------------
//#define VGIZMO_USES_GLM

// uncomment to use LeftHanded 
//
// This is used only in: lookAt / perspective / ortho / frustrum - functions
//      DX is LeftHanded, OpenGL is RightHanded
//
// Default ==> RightHanded
//------------------------------------------------------------------------------
//#define VGIZMO_USES_LEFT_HAND_AXES


#ifdef VGIZMO_USES_DOUBLE_PRECISION
    #define VG_T_TYPE double
    #define VGIZMO_USES_TEMPLATE
#else
    #define VG_T_TYPE float
#endif
