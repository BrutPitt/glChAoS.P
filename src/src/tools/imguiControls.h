//------------------------------------------------------------------------------
//  Copyright (c) 2018-2020 Michele Morrone
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
#include "../libs/imgui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../libs/imgui/imgui_internal.h"

//  
//  hslTuning
//
//      enum and defines
////////////////////////////////////////////////////////////////////////////
enum hslPaletteControl { 
    HSL_TUNING_H,
    HSL_TUNING_S,
    HSL_TUNING_L
};

#define HSL_CONTROL_PROPORTIONAL



namespace ImGui
{

//  
//  paletteButton
//
//      numCol - num colors in the buffers pallette 
//      buff   - floating point array triplettes R,G,B (size must be numCol*3)
//      sz     - widget size
////////////////////////////////////////////////////////////////////////////
IMGUI_API bool paletteButton(const char* label, int numCol, float *buff, const ImVec2& sz);

//  
//  hslTuning
//
//      selVal   - in/out value in interval [-1,1]
//      type     - type of widget, must be HSL_TUNING_H (for HUE), HSL_TUNING_S, HSL_TUNING_L
//      stepSize - increment
//      size     - widget size
//      padding  - need for adjust color line below text in case of different fonts
//               for default is your Imgui default setting
////////////////////////////////////////////////////////////////////////////
IMGUI_API bool hslTuning(const char* label, float *selVal, int type, float stepSize, const ImVec2 &size, const ImVec2 &padding=ImVec2(-1,-1));

//  
//  DragFloatEx and DragFloat#Ex
//
//      identical to DragFloat and DragFloat#: adding right justification 
//      and "return true" also on selection, to memorize position in a table 
//      of values
//      P.S. "right justification" is present internally in original 
//      DragFloat, but in not accessible via formal parameters: just adding
//      "alignment" parameter to pass value internally
////////////////////////////////////////////////////////////////////////////
IMGUI_API bool DragFloatEx (const char* label, float* v, float v_speed = 1.0f, float v_min = 0.0f, float v_max = 0.0f, const char* display_format = "%.3f", float power = 1.0f, const ImVec2 &alignment = ImVec2(0.5,0.5));     // If v_min >= v_max we have no bound
IMGUI_API bool DragFloatNEx(const char* label, float* v, int components, float v_speed, float v_min, float v_max, const char* display_format, float power, const ImVec2 &alignment = ImVec2(0.5,0.5));
IMGUI_API bool DragFloat2Ex(const char* label, float v[2], float v_speed, float v_min, float v_max, const char* display_format, float power, const ImVec2 &alignment = ImVec2(0.5,0.5));
IMGUI_API bool DragFloat3Ex(const char* label, float v[3], float v_speed, float v_min, float v_max, const char* display_format, float power, const ImVec2 &alignment = ImVec2(0.5,0.5));
IMGUI_API bool DragFloat4Ex(const char* label, float v[4], float v_speed, float v_min, float v_max, const char* display_format, float power, const ImVec2 &alignment = ImVec2(0.5,0.5));

}