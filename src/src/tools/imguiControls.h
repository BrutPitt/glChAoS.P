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
#include "../libs/ImGui/imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "../libs/ImGui/imgui_internal.h"

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
//      selVal - in/out value in interval [-1,1]
//      type   - type of widget, must be HSL_TUNING_H (for HUE), HSL_TUNING_S, HSL_TUNING_L
//      size   - widget size
//      padding- need for adjust color line below text in case of different fonts
//               for default is your Imgui default setting
////////////////////////////////////////////////////////////////////////////
IMGUI_API bool hslTuning(const char* label, float *selVal,int type, const ImVec2 &size, const ImVec2 &padding=ImVec2(-1,-1));

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