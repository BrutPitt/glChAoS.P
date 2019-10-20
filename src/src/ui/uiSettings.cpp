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
#include "../glApp.h"
#include <IconsFontAwesome/IconsFontAwesome.h>

#include <ImGui/imgui.h>


void lightCyanYellow()
{
    ImGuiStyle& style = ImGui::GetStyle();

    style.Colors[ImGuiCol_Text]                 = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]         = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
    style.Colors[ImGuiCol_WindowBg]             = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_ChildBg]              = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
    style.Colors[ImGuiCol_PopupBg]              = ImVec4(0.92f, 0.92f, 0.92f, 1.00f);
    style.Colors[ImGuiCol_Border]               = ImVec4(0.00f, 0.00f, 0.00f, 0.80f);
    style.Colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]              = ImVec4(0.71f, 0.71f, 0.71f, 0.39f);
    style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.00f, 0.59f, 0.80f, 0.43f);
    style.Colors[ImGuiCol_FrameBgActive]        = ImVec4(0.00f, 0.47f, 0.71f, 0.67f);
    style.Colors[ImGuiCol_TitleBg]              = ImVec4(1.00f, 1.00f, 1.00f, 0.80f);
    style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.78f, 0.78f, 0.78f, 0.39f);
    style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg]            = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
    style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.59f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.00f, 0.00f, 0.00f, 0.78f);
    style.Colors[ImGuiCol_CheckMark]            = ImVec4(0.27f, 0.59f, 0.75f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]           = ImVec4(0.00f, 0.00f, 0.00f, 0.35f);
    style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.00f, 0.00f, 0.00f, 0.59f);
    style.Colors[ImGuiCol_Button]               = ImVec4(0.00f, 0.00f, 0.00f, 0.27f);
    style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(0.00f, 0.59f, 0.80f, 0.43f);
    style.Colors[ImGuiCol_ButtonActive]         = ImVec4(0.00f, 0.47f, 0.71f, 0.67f);
    style.Colors[ImGuiCol_Header]               = ImVec4(0.71f, 0.71f, 0.71f, 0.39f);
    style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(0.20f, 0.51f, 0.67f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive]         = ImVec4(0.08f, 0.39f, 0.55f, 1.00f);
    style.Colors[ImGuiCol_Separator]            = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered]     = ImVec4(0.27f, 0.59f, 0.75f, 1.00f);
    style.Colors[ImGuiCol_SeparatorActive]      = ImVec4(0.08f, 0.39f, 0.55f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(0.00f, 0.00f, 0.00f, 0.78f);
    style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(0.27f, 0.59f, 0.75f, 0.78f);
    style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4(0.08f, 0.39f, 0.55f, 0.78f);
    style.Colors[ImGuiCol_PlotLines]            = ImVec4(0.00f, 0.00f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(0.00f, 0.30f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]        = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(0.27f, 0.59f, 0.75f, 1.00f);
    style.Colors[ImGuiCol_ModalWindowDimBg]     = ImVec4(0.00f, 0.00f, 0.00f, 0.35f);

}

// Dynamic theme, freely inspired from a static theme of enemymouse: 
// https://github.com/ocornut/imgui/issues/539#issuecomment-204412632
// https://gist.github.com/enemymouse/c8aa24e247a1d7b9fc33d45091cbb8f0
void darkColorsOnBlack(const ImVec4 &color)
{
    ImGuiStyle& style = ImGui::GetStyle();

    const float kC = .25f;
    const float lum = color.x*.299f + color.y*.587f + color.z*.114f;
    const float lumK = 1 + lum * .25;

    ImVec4 wBG(color.x*kC, color.y*kC, color.z*kC, 1.0f);

    
    ImVec4 clA(wBG.x*4.3f*lumK, wBG.y*4.3f*lumK, wBG.z*4.3f*lumK, 1.00f); //0.00f, 0.55f, 0.87f
    ImVec4 clB(wBG.x*1.5f*lumK, wBG.y*1.5f*lumK, wBG.z*1.5f*lumK, 1.00f); //0.20f, 0.22f, 0.27f
    ImVec4 clC(wBG.x*3.0f*lumK, wBG.y*3.0f*lumK, wBG.z*3.0f*lumK, 1.00f); //0.20f, 0.22f, 0.27f
    ImVec4 clD(wBG.x*3.7f*lumK, wBG.y*3.7f*lumK, wBG.z*3.7f*lumK, 1.00f);

    
    ImVec4 HSV;

    const float diff = 1.f/(lumK*lumK*lumK);
    clD.x *= diff;
    clD.y *= diff;
    clD.z *= diff;

    ImVec4 txt;
    ImGui::ColorConvertRGBtoHSV(wBG.x, wBG.y, wBG.z, HSV.x, HSV.y, HSV.z);
    if(HSV.y>.25) HSV.y=.25; 
    HSV.z=.85;
    ImGui::ColorConvertHSVtoRGB(HSV.x, HSV.y, HSV.z, txt.x, txt.y, txt.z);

    ImVec4 chk((txt.x+color.x)*.5f, (txt.y+color.y)*.5f, (txt.z+color.z)*.5f, 1.0f) ;

    const ImVec4 ch1(chk.x*.60, chk.y*.60, chk.z*.60, 1.00f);
    const ImVec4 ch2(chk.x*.80, chk.y*.80, chk.z*.80, 1.00f);


    style.Colors[ImGuiCol_Text]                 = ImVec4(txt.x, txt.y, txt.z, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]         = ImVec4(txt.x, txt.y, txt.z, 1.00f);
    style.Colors[ImGuiCol_WindowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_ChildBg]              = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg]              = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_Border]               = ImVec4(clA.x, clA.y, clA.z, 0.65f);
    style.Colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]              = ImVec4(clD.x, clD.y, clD.z, 0.35f);
    style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4(clD.x, clD.y, clD.z, 0.66f);
    style.Colors[ImGuiCol_FrameBgActive]        = ImVec4(clD.x, clD.y, clD.z, 0.85f);
    style.Colors[ImGuiCol_TitleBg]              = ImVec4(clB.x, clB.y, clB.z, 0.50f);
    style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.00f, 0.00f, 0.00f, 0.75f);
    style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(clB.x, clB.y, clB.z, 0.85f);
    style.Colors[ImGuiCol_MenuBarBg]            = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
    style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(clB.x, clB.y, clB.z, 0.35f);
    style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(clA.x, clA.y, clA.z, 0.44f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(clA.x, clA.y, clA.z, 0.74f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(clA.x, clA.y, clA.z, 1.00f);
    style.Colors[ImGuiCol_CheckMark]            = ImVec4(chk.x, chk.y, chk.z, 0.80f);
    style.Colors[ImGuiCol_SliderGrab]           = ImVec4(clA.x, clA.y, clA.z, 0.36f);
    style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4(clA.x, clA.y, clA.z, 0.76f);
    style.Colors[ImGuiCol_Button]               = ImVec4(clC.x, clC.y, clC.z, 0.35f);
    style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(clA.x, clA.y, clA.z, 0.43f);
    style.Colors[ImGuiCol_ButtonActive]         = ImVec4(clA.x, clA.y, clA.z, 0.62f);
    style.Colors[ImGuiCol_Header]               = ImVec4(clA.x, clA.y, clA.z, 0.33f);
    style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(clA.x, clA.y, clA.z, 0.42f);
    style.Colors[ImGuiCol_HeaderActive]         = ImVec4(clA.x, clA.y, clA.z, 0.54f);
    style.Colors[ImGuiCol_Separator]            = ImVec4(clB.x, clB.y, clB.z, 0.50f);
    style.Colors[ImGuiCol_SeparatorHovered]     = ImVec4(clB.x, clB.y, clB.z, 0.75f);
    style.Colors[ImGuiCol_SeparatorActive]      = ImVec4(clC.x, clC.y, clC.z, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(clA.x, clA.y, clA.z, 0.54f);
    style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(clA.x, clA.y, clA.z, 0.74f);
    style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4(clA.x, clA.y, clA.z, 1.00f);
    style.Colors[ImGuiCol_PlotLines]            = ImVec4(clA.x, clA.y, clA.z, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(clA.x, clA.y, clA.z, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]        = ImVec4(chk.x, chk.y, chk.z, 0.40f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(chk.x, chk.y, chk.z, 0.50f);
    style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(clA.x, clA.y, clA.z, 0.22f);
    style.Colors[ImGuiCol_ModalWindowDimBg]     = ImVec4(clB.x, clB.y, clB.z, 0.40f);
}


//Light grey + Green
void colorTheme4()
{

    ImGuiStyle& style = ImGui::GetStyle();

    style.Colors[ImGuiCol_Text]                 = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]         = ImVec4(0.40f, 0.39f, 0.38f, 0.77f);
    style.Colors[ImGuiCol_WindowBg]             = ImVec4(0.92f, 0.91f, 0.88f, 0.70f);
    style.Colors[ImGuiCol_ChildBg]              = ImVec4(1.00f, 0.98f, 0.95f, 0.58f);
    style.Colors[ImGuiCol_PopupBg]              = ImVec4(0.92f, 0.91f, 0.88f, 0.92f);
    style.Colors[ImGuiCol_Border]               = ImVec4(0.84f, 0.83f, 0.80f, 0.65f);
    style.Colors[ImGuiCol_BorderShadow]         = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]              = ImVec4(1.00f, 0.98f, 0.95f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.99f, 1.00f, 0.40f, 0.78f);
    style.Colors[ImGuiCol_FrameBgActive]        = ImVec4(0.26f, 1.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TitleBg]              = ImVec4(1.00f, 0.98f, 0.95f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
    style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg]            = ImVec4(1.00f, 0.98f, 0.95f, 0.47f);
    style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(1.00f, 0.98f, 0.95f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.00f, 0.00f, 0.00f, 0.21f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.90f, 0.91f, 0.00f, 0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_CheckMark]            = ImVec4(0.25f, 1.00f, 0.00f, 0.80f);
    style.Colors[ImGuiCol_SliderGrab]           = ImVec4(0.00f, 0.00f, 0.00f, 0.14f);
    style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_Button]               = ImVec4(0.00f, 0.00f, 0.00f, 0.14f);
    style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(0.99f, 1.00f, 0.22f, 0.86f);
    style.Colors[ImGuiCol_ButtonActive]         = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_Header]               = ImVec4(0.25f, 1.00f, 0.00f, 0.76f);
    style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(0.25f, 1.00f, 0.00f, 0.86f);
    style.Colors[ImGuiCol_HeaderActive]         = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_Separator]            = ImVec4(0.00f, 0.00f, 0.00f, 0.32f);
    style.Colors[ImGuiCol_SeparatorHovered]     = ImVec4(0.25f, 1.00f, 0.00f, 0.78f);
    style.Colors[ImGuiCol_SeparatorActive]      = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(0.00f, 0.00f, 0.00f, 0.04f);
    style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(0.25f, 1.00f, 0.00f, 0.78f);
    style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotLines]            = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]        = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
    style.Colors[ImGuiCol_ModalWindowDimBg]     = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
}


//Dark + Blu
void colorTheme5b()
{
    ImGuiStyle& style = ImGui::GetStyle();
    //Like Solar
    //ImVec4 wBG(0.00f, 0.045f, 0.0525f, 1.00f);
    ImVec4 wBG(0.00f, 0.06f, 0.075f, 1.00f);
    //ImVec4 wBG(0.00f, 0.12f, 0.15f, 1.00f);
    
    //Blu
    //ImVec4 wBG(0.00f, 0.03f, 0.09f, 1.00f);
    //ImVec4 wBG(0.00f, 0.045f, 0.075f, 1.00f);
    //ImVec4 wBG(0.00f, 0.06f, 0.09f, 1.00f);
    //ImVec4 wBG(0.07f, 0.07f, 0.00f, 1.00f);
    
    //0.00,0.09,0.16

    //ImVec4 clA(0.17f, 0.32f, 0.35f, 1.00f); //0.00f, 0.55f, 0.87f
    
    ImVec4 clA(wBG.x*3.5f, wBG.y*3.5f, wBG.z*3.5f, 1.00f); //0.00f, 0.55f, 0.87f
    ImVec4 clB(wBG.x*2.f, wBG.y*2.f, wBG.z*2.f, 1.00f); //0.20f, 0.22f, 0.27f
    ImVec4 clC(wBG.x*5.f, wBG.y*5.f, wBG.z*5.f, 1.00f); //0.20f, 0.22f, 0.27f
    
    //ImVec4 txt(0.55f, 0.70f, 0.70f, 1.00f); //0.86f, 0.93f, 0.89f       
    ImVec4 act(0.00f, 0.33f, 0.66f, 1.00f); //0.86f, 0.93f, 0.89f
    //ImVec4 act(wBG.x*7.f, wBG.y*7.f, wBG.z*7.f, 1.00f);

    ImVec4 txt;
    ImVec4 HSV;
    ImGui::ColorConvertRGBtoHSV(wBG.x, wBG.y, wBG.z, HSV.x, HSV.y, HSV.z);
    if(HSV.y>.25) HSV.y=.25; 
    HSV.z=.75;
    ImGui::ColorConvertHSVtoRGB(HSV.x, HSV.y, HSV.z, txt.x, txt.y, txt.z);

    


    style.Colors[ImGuiCol_Text]                 = ImVec4(txt.x, txt.y, txt.z, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]         = ImVec4(txt.x, txt.y, txt.z, 0.45f);
    style.Colors[ImGuiCol_WindowBg]             = ImVec4(wBG.x, wBG.y, wBG.z, 1.00f);
    style.Colors[ImGuiCol_ChildBg]              = ImVec4(clB.x, clB.y, clB.z, 0.30f);
    style.Colors[ImGuiCol_Border]               = ImVec4(clB.x, clB.y, clB.z, 0.75f);
    style.Colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]              = ImVec4(clC.x, clC.y, clC.z, 0.25f);
    style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4(clC.x, clC.y, clC.z, 0.70f);
    style.Colors[ImGuiCol_FrameBgActive]        = ImVec4(act.x, act.y, act.z, 1.00f);
    style.Colors[ImGuiCol_TitleBg]              = ImVec4(clB.x, clB.y, clB.z, 0.70f);
    style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(wBG.x, wBG.y, wBG.z, 0.50f);
    style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(act.x, act.y, act.z, 0.90f);
    style.Colors[ImGuiCol_MenuBarBg]            = ImVec4(clB.x, clB.y, clB.z, 0.47f);
    style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(clB.x, clB.y, clB.z, 0.25f);
    style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(clA.x, clA.y, clA.z, 0.66f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(clA.x, clA.y, clA.z, 0.85f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(clA.x, clA.y, clA.z, 1.00f);
    style.Colors[ImGuiCol_CheckMark]            = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]           = ImVec4(clC.x, clC.y, clC.z, 0.55f);
    style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4(clC.x, clC.y, clC.z, 0.70f);
    style.Colors[ImGuiCol_Button]               = ImVec4(clC.x, clC.y, clC.z, 0.40f);
    style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(clC.x, clC.y, clC.z, 0.50f);
    style.Colors[ImGuiCol_ButtonActive]         = ImVec4(act.x, act.y, act.z, 1.00f);
    style.Colors[ImGuiCol_Header]               = ImVec4(act.x, act.y, act.z, 0.50f);
    style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(act.x, act.y, act.z, 0.75f);
    style.Colors[ImGuiCol_HeaderActive]         = ImVec4(act.x, act.y, act.z, 1.00f);
    style.Colors[ImGuiCol_Separator]            = ImVec4(wBG.x, wBG.y, wBG.z, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered]     = ImVec4(clA.x, clA.y, clA.z, 0.78f);
    style.Colors[ImGuiCol_SeparatorActive]      = ImVec4(clA.x, clA.y, clA.z, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(clB.x, clB.y, clB.z, 0.50f);
    style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(clA.x, clA.y, clA.z, 0.78f);
    style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4(clA.x, clA.y, clA.z, 1.00f);
    style.Colors[ImGuiCol_PlotLines]            = ImVec4(txt.x, txt.y, txt.z, 0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(clA.x, clA.y, clA.z, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]        = ImVec4(txt.x, txt.y, txt.z, 0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(clA.x, clA.y, clA.z, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(clA.x, clA.y, clA.z, 0.43f);
    style.Colors[ImGuiCol_PopupBg]				= ImVec4(clB.x, clB.y, clB.z, 0.9f);
    style.Colors[ImGuiCol_ModalWindowDimBg]     = ImVec4(clB.x, clB.y, clB.z, 0.73f);
}


void darkDefaultTheme(const ImVec4 &color, bool wantAlwaysRedButtons=false)
{
    ImGuiStyle& style = ImGui::GetStyle();
    //Like Solar
    //ImVec4 wBG(0.00f, 0.045f, 0.0525f, 1.00f);
    //ImVec4 wBG(0.00f, 0.06f, 0.075f, 1.00f);
    //ImVec4 wBG(0.00f, 0.12f, 0.15f, 1.00f);
    
    //Blu
    const float kC = .2f;
    const float lum = color.x*.299f + color.y*.587f + color.z*.114f;
    const float lumK = 1 + lum * .25;

    ImVec4 wBG(color.x*kC, color.y*kC, color.z*kC, 1.0f);

    ImVec4 act(wBG.x*6.0f*lumK, wBG.y*6.0f*lumK, wBG.z*6.0f*lumK, 1.00f);

    ImVec4 clA(wBG.x*3.0f*lumK, wBG.y*3.0f*lumK, wBG.z*3.0f*lumK, 1.00f); //0.00f, 0.55f, 0.87f
    ImVec4 clB(wBG.x*1.5f*lumK, wBG.y*1.5f*lumK, wBG.z*1.5f*lumK, 1.00f); //0.20f, 0.22f, 0.27f
    ImVec4 clC(wBG.x*4.0f*lumK, wBG.y*4.0f*lumK, wBG.z*4.0f*lumK, 1.00f); //0.20f, 0.22f, 0.27f

    
    ImVec4 HSV;

    const float diff = 1.f/(lumK*lumK*lumK);
    act.x *= diff;
    act.y *= diff;
    act.z *= diff;

    ImVec4 txt;

    ImGui::ColorConvertRGBtoHSV(wBG.x, wBG.y, wBG.z, HSV.x, HSV.y, HSV.z);
    if(HSV.y>.25) HSV.y=.25; 
    HSV.z=.85;
    static float baseH = HSV.x;
    float tmpH = HSV.x;
    

    if(theApp->isTabletMode())  // high contrast on tablet & smartphones
        txt = ImVec4(1.f, 1.f, 1.f, 1.f);
    else
        ImGui::ColorConvertHSVtoRGB(HSV.x, HSV.y, HSV.z, txt.x, txt.y, txt.z);

    ImVec4 lin;
    ImGui::ColorConvertRGBtoHSV(1.0f, 1.0f, .75f, HSV.x, HSV.y, HSV.z);
    HSV.x += baseH - tmpH;
    if(HSV.x>1.0)      HSV.x-=1.0; 
    else if(HSV.x<0.0) HSV.x+=1.0; 
    ImGui::ColorConvertHSVtoRGB(HSV.x, HSV.y, HSV.z, lin.x, lin.y, lin.z);

    ImVec4 chk;
    if(wantAlwaysRedButtons) {
        chk = ImVec4(0.60f, 0.00f, 0.00f, 1.0);
    } else {
        ImGui::ColorConvertRGBtoHSV(0.60f, 0.00f, 0.00f, HSV.x, HSV.y, HSV.z);
        HSV.x += baseH - tmpH;
        if(HSV.x>1.0)      HSV.x-=1.0; 
        else if(HSV.x<0.0) HSV.x+=1.0; 
        ImGui::ColorConvertHSVtoRGB(HSV.x, HSV.y, HSV.z, chk.x, chk.y, chk.z);
    }

    chk.x *= lumK;
    chk.y *= lumK;
    chk.z *= lumK;

    const float l1 = theApp->isTabletMode() ? .80f : .60f;
    const float l2 = theApp->isTabletMode() ? .93f : .80f;
    const ImVec4 ch1(chk.x*l1, chk.y*l1, chk.z*l1, 1.00f);
    const ImVec4 ch2(chk.x*l2, chk.y*l2, chk.z*l2, 1.00f);


    style.Colors[ImGuiCol_Text]                 = ImVec4(txt.x, txt.y, txt.z, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]         = ImVec4(txt.x, txt.y, txt.z, theApp->isTabletMode() ? 0.60f : 0.45f);
    style.Colors[ImGuiCol_WindowBg]             = ImVec4(wBG.x, wBG.y, wBG.z, 1.00f);
    style.Colors[ImGuiCol_ChildBg]              = ImVec4(clB.x, clB.y, clB.z, 0.30f);
    style.Colors[ImGuiCol_PopupBg]              = ImVec4(wBG.x, wBG.y, wBG.z, 1.00f);
    style.Colors[ImGuiCol_Border]               = ImVec4(clA.x, clA.y, clA.z, 0.66f);
    style.Colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]              = ImVec4(clA.x, clA.y, clA.z, 0.60f);
    style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4(act.x, act.y, act.z, 0.75f);
    style.Colors[ImGuiCol_FrameBgActive]        = ImVec4(act.x, act.y, act.z, 1.00f);
    style.Colors[ImGuiCol_TitleBg]              = ImVec4(clB.x, clB.y, clB.z, 0.85f);
    style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(wBG.x, wBG.y, wBG.z, 0.66f);
    style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(act.x, act.y, act.z, 0.85f);
    style.Colors[ImGuiCol_MenuBarBg]            = ImVec4(clB.x, clB.y, clB.z, 0.47f);
    style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(clB.x, clB.y, clB.z, theApp->isTabletMode() ? 0.40f : 0.25f);
    style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(clA.x, clA.y, clA.z, theApp->isTabletMode() ? 0.75f : 0.50f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ch1;
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ch2;
    style.Colors[ImGuiCol_CheckMark]            = ImVec4(chk.x, chk.y, chk.z, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]           = ch1;
    style.Colors[ImGuiCol_SliderGrabActive]     = ch2;
    style.Colors[ImGuiCol_Button]               = ImVec4(clC.x, clC.y, clC.z, 0.40f);
    style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(act.x, act.y, act.z, 0.50f);
    style.Colors[ImGuiCol_ButtonActive]         = ImVec4(act.x, act.y, act.z, 1.00f);
    style.Colors[ImGuiCol_Header]               = ImVec4(act.x, act.y, act.z, 0.50f);
    style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(act.x, act.y, act.z, 0.75f);
    style.Colors[ImGuiCol_HeaderActive]         = ImVec4(act.x, act.y, act.z, 1.00f);
    style.Colors[ImGuiCol_Separator]            = ImVec4(wBG.x, wBG.y, wBG.z, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered]     = ImVec4(clA.x, clA.y, clA.z, 0.78f);
    style.Colors[ImGuiCol_SeparatorActive]      = ImVec4(clA.x, clA.y, clA.z, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(clA.x, clA.y, clA.z, 0.75f);
    style.Colors[ImGuiCol_ResizeGripHovered]    = ch1;
    style.Colors[ImGuiCol_ResizeGripActive]     = ch2;
    style.Colors[ImGuiCol_PlotLines]            = ImVec4(lin.x, lin.y, lin.z, 0.75f);
    style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(lin.x, lin.y, lin.z, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]        = ch1;
    style.Colors[ImGuiCol_PlotHistogramHovered] = ch2;
    style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(clA.x, clA.y, clA.z, 0.43f);
    style.Colors[ImGuiCol_PopupBg]				= ImVec4(clB.x, clB.y, clB.z, 0.9f);
    style.Colors[ImGuiCol_ModalWindowDimBg]     = ImVec4(clB.x, clB.y, clB.z, 0.73f);
}



//Dark + Blu
void darkDefaultGrayButt(const ImVec4 &color)
{
    ImGuiStyle& style = ImGui::GetStyle();


    const float kC = .2f;
    const float lum = color.x*.299f + color.y*.587f + color.z*.114f;
    const float lumK = 1 + lum * .25;

    ImVec4 wBG(color.x*kC, color.y*kC, color.z*kC, 1.0f);

    //ImVec4 clA(0.00f, 0.31f, 0.40f, 1.00f);
    //ImVec4 clB(0.00f, 0.21f, 0.27f, 1.00f);

    ImVec4 clA(wBG.x*3.0f*lumK, wBG.y*3.0f*lumK, wBG.z*3.0f*lumK, 1.00f); //0.00f, 0.55f, 0.87f
    ImVec4 clB(wBG.x*1.5f*lumK, wBG.y*1.5f*lumK, wBG.z*1.5f*lumK, 1.00f); //0.20f, 0.22f, 0.27f
    ImVec4 clC(wBG.x*4.0f*lumK, wBG.y*4.0f*lumK, wBG.z*4.0f*lumK, 1.00f); //0.20f, 0.22f, 0.27f


    ImVec4 txt(0.90f, 0.90f, 0.90f, 1.00f); //0.86f, 0.93f, 0.89f    

    //ImVec4 act(0.00f, 0.25f, 0.57f, 1.00f); //0.86f, 0.93f, 0.89f
    ImVec4 act(wBG.x*3.75f, wBG.y*3.75f, wBG.z*3.75f, 1.00f);
    const float diff = 1.f/(lumK*lumK);
    act.x *= diff;
    act.y *= diff;
    act.z *= diff;

    ImVec4 lin(1.0f, 1.0f, 1.0f, 1.0f);
    ImVec4 chk(0.60f*lumK, 0.60f*lumK, 0.60f*lumK, 1.0f);

    const ImVec4 ch1(chk.x*.60, chk.y*.60, chk.z*.60, 1.00f);
    const ImVec4 ch2(chk.x*.80, chk.y*.80, chk.z*.80, 1.00f);



    //ImVec4 chk(0.00f, 0.25f, 0.57f, 1.00f);
    
    style.Colors[ImGuiCol_Text]                 = ImVec4(txt.x, txt.y, txt.z, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]         = ImVec4(txt.x, txt.y, txt.z, 0.45f);
    style.Colors[ImGuiCol_WindowBg]             = ImVec4(wBG.x, wBG.y, wBG.z, 1.00f);
    style.Colors[ImGuiCol_ChildBg]              = ImVec4(clB.x, clB.y, clB.z, 0.30f);
    style.Colors[ImGuiCol_PopupBg]              = ImVec4(wBG.x, wBG.y, wBG.z, 1.00f);
    style.Colors[ImGuiCol_Border]               = ImVec4(clA.x, clA.y, clA.z, 0.66f);
    style.Colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]              = ImVec4(clA.x, clA.y, clA.z, 0.60f);
    style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4(act.x, act.y, act.z, 0.75f);
    style.Colors[ImGuiCol_FrameBgActive]        = ImVec4(act.x, act.y, act.z, 1.00f);
    style.Colors[ImGuiCol_TitleBg]              = ImVec4(clB.x, clB.y, clB.z, 0.85f);
    style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(wBG.x, wBG.y, wBG.z, 0.66f);
    style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(act.x, act.y, act.z, 0.85f);
    style.Colors[ImGuiCol_MenuBarBg]            = ImVec4(clB.x, clB.y, clB.z, 0.47f);
    style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(clB.x, clB.y, clB.z, 0.25f);
    style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(clA.x, clA.y, clA.z, 0.50f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ch1;
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ch2;
    style.Colors[ImGuiCol_CheckMark]            = ImVec4(chk.x, chk.y, chk.z, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]           = ch1;
    style.Colors[ImGuiCol_SliderGrabActive]     = ch2;
    style.Colors[ImGuiCol_Button]               = ImVec4(clC.x, clC.y, clC.z, 0.40f);
    style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(act.x, act.y, act.z, 0.50f);
    style.Colors[ImGuiCol_ButtonActive]         = ImVec4(act.x, act.y, act.z, 1.00f);
    style.Colors[ImGuiCol_Header]               = ImVec4(act.x, act.y, act.z, 0.50f);
    style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(act.x, act.y, act.z, 0.75f);
    style.Colors[ImGuiCol_HeaderActive]         = ImVec4(act.x, act.y, act.z, 1.00f);
    style.Colors[ImGuiCol_Separator]            = ImVec4(wBG.x, wBG.y, wBG.z, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered]     = ImVec4(clA.x, clA.y, clA.z, 0.78f);
    style.Colors[ImGuiCol_SeparatorActive]      = ImVec4(clA.x, clA.y, clA.z, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(clA.x, clA.y, clA.z, 0.75f);
    style.Colors[ImGuiCol_ResizeGripHovered]    = ch1;
    style.Colors[ImGuiCol_ResizeGripActive]     = ch2;
    style.Colors[ImGuiCol_PlotLines]            = ImVec4(lin.x, lin.y, lin.z, 0.75f);
    style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(lin.x, lin.y, lin.z, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]        = ch1;
    style.Colors[ImGuiCol_PlotHistogramHovered] = ch2;
    style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(clA.x, clA.y, clA.z, 0.43f);
    style.Colors[ImGuiCol_PopupBg]				= ImVec4(clB.x, clB.y, clB.z, 0.9f);
    style.Colors[ImGuiCol_ModalWindowDimBg]     = ImVec4(clB.x, clB.y, clB.z, 0.73f);

}


void darkSolarized()
{
    ImGuiStyle& style = ImGui::GetStyle();

    ImVec4 wBG(0.00f, 0.10f, 0.125f, 1.00f);

    //ImVec4 clA(0.00f, 0.31f, 0.40f, 1.00f);
    //ImVec4 clB(0.00f, 0.21f, 0.27f, 1.00f);

    ImVec4 clA(wBG.x*2.50f, wBG.y*2.50f, wBG.z*2.50f, 1.00f);
    ImVec4 clB(wBG.x*1.75f, wBG.y*1.75f, wBG.z*1.75f, 1.00f);


    ImVec4 txt(0.66f, 0.81f, 0.78f, 1.00f); //0.86f, 0.93f, 0.89f    
//    ImVec4 act(0.00f, 0.25f, 0.57f, 1.00f); //0.86f, 0.93f, 0.89f
    ImVec4 act(wBG.x*3.75f, wBG.y*3.75f, wBG.z*3.75f, 1.00f);


    //ImVec4 chk(0.00f, 0.25f, 0.57f, 1.00f);
    

    style.Colors[ImGuiCol_Text]                 = ImVec4(txt.x, txt.y, txt.z, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]         = ImVec4(txt.x, txt.y, txt.z, 0.28f);
    style.Colors[ImGuiCol_WindowBg]             = ImVec4(wBG.x, wBG.y, wBG.z, 1.00f);
    style.Colors[ImGuiCol_ChildBg]              = ImVec4(clB.x, clB.y, clB.z, 0.40f);
    style.Colors[ImGuiCol_PopupBg]              = ImVec4(wBG.x, wBG.y, wBG.z, 1.00f);
    style.Colors[ImGuiCol_Border]               = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
    style.Colors[ImGuiCol_BorderShadow]         = ImVec4(0.99f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]              = ImVec4(clB.x, clB.y, clB.z, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4(clA.x, clA.y, clA.z, 0.78f);
    style.Colors[ImGuiCol_FrameBgActive]        = ImVec4(txt.x, txt.y, txt.z, 0.60f);
    style.Colors[ImGuiCol_TitleBg]              = ImVec4(wBG.x, wBG.y, wBG.z, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(wBG.x, wBG.y, wBG.z, 0.60f);
    style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(act.x, act.y, act.z, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg]            = ImVec4(clB.x, clB.y, clB.z, 0.47f);
    style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(wBG.x, wBG.y, wBG.z, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(clB.x, clB.y, clB.z, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(clA.x, clA.y, clA.z, 0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(clA.x, clA.y, clA.z, 1.00f);
    style.Colors[ImGuiCol_CheckMark]            = ImVec4(txt.x, txt.y, txt.z, 0.65f);
    style.Colors[ImGuiCol_SliderGrab]           = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4(clA.x, clA.y, clA.z, 1.00f);
    style.Colors[ImGuiCol_Button]               = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(clA.x, clA.y, clA.z, 0.86f);
    style.Colors[ImGuiCol_ButtonActive]         = ImVec4(txt.x, txt.y, txt.z, 0.65f);
    style.Colors[ImGuiCol_Header]               = ImVec4(clA.x, clA.y, clA.z, 0.76f);
    style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(clA.x, clA.y, clA.z, 0.86f);
    style.Colors[ImGuiCol_HeaderActive]         = ImVec4(act.x, act.y, act.z, 1.00f);
    style.Colors[ImGuiCol_Separator]            = ImVec4(wBG.x, wBG.y, wBG.z, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered]     = ImVec4(clA.x, clA.y, clA.z, 0.78f);
    style.Colors[ImGuiCol_SeparatorActive]      = ImVec4(clA.x, clA.y, clA.z, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
    style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(clA.x, clA.y, clA.z, 0.78f);
    style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4(clA.x, clA.y, clA.z, 1.00f);
    style.Colors[ImGuiCol_PlotLines]            = ImVec4(1.00f, 1.00f, 0.75f, 0.80f);
    style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(clA.x, clA.y, clA.z, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]        = ImVec4(txt.x, txt.y, txt.z, 0.40f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(txt.x, txt.y, txt.z, 0.50f);
    style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(clA.x, clA.y, clA.z, 0.43f);
    style.Colors[ImGuiCol_PopupBg]				= ImVec4(clB.x, clB.y, clB.z, 0.9f);
    style.Colors[ImGuiCol_ModalWindowDimBg]     = ImVec4(clB.x, clB.y, clB.z, 0.73f);
}

//Dark + red
void colorTheme6()
{
    ImGuiStyle& style = ImGui::GetStyle();

    style.Colors[ImGuiCol_Text]                 = ImVec4(0.86f, 0.93f, 0.89f, 0.78f);
    style.Colors[ImGuiCol_TextDisabled]         = ImVec4(0.86f, 0.93f, 0.89f, 0.28f);
    style.Colors[ImGuiCol_WindowBg]             = ImVec4(0.13f, 0.14f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_ChildBg]              = ImVec4(0.20f, 0.22f, 0.27f, 0.58f);
    style.Colors[ImGuiCol_Border]               = ImVec4(0.31f, 0.31f, 1.00f, 0.00f);
    style.Colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]              = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.10f, 0.67f, 1.00f, 0.78f);
    style.Colors[ImGuiCol_FrameBgActive]        = ImVec4(0.10f, 0.67f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TitleBg]              = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.20f, 0.22f, 0.27f, 0.75f);
    style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(0.10f, 0.67f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg]            = ImVec4(0.20f, 0.22f, 0.27f, 0.47f);
    style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.20f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.09f, 0.15f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.10f, 0.67f, 1.00f, 0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.10f, 0.67f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_CheckMark]            = ImVec4(0.71f, 0.22f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]           = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.10f, 0.67f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_Button]               = ImVec4(0.47f, 0.77f, 0.83f, 0.14f);
    style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(0.10f, 0.67f, 1.00f, 0.86f);
    style.Colors[ImGuiCol_ButtonActive]         = ImVec4(0.10f, 0.67f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_Header]               = ImVec4(0.10f, 0.67f, 1.00f, 0.76f);
    style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(0.10f, 0.67f, 1.00f, 0.86f);
    style.Colors[ImGuiCol_HeaderActive]         = ImVec4(0.10f, 0.67f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_Separator]            = ImVec4(0.14f, 0.16f, 0.19f, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered]     = ImVec4(0.10f, 0.67f, 1.00f, 0.78f);
    style.Colors[ImGuiCol_SeparatorActive]      = ImVec4(0.10f, 0.67f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(0.47f, 0.77f, 0.83f, 0.04f);
    style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(0.10f, 0.67f, 1.00f, 0.78f);
    style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4(0.10f, 0.67f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_PlotLines]            = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(0.10f, 0.67f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]        = ImVec4(0.86f, 0.93f, 0.89f, 0.63f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.10f, 0.67f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(0.10f, 0.67f, 1.00f, 0.43f);
    style.Colors[ImGuiCol_PopupBg]				= ImVec4(0.20f, 0.22f, 0.27f, 0.9f);
    style.Colors[ImGuiCol_ModalWindowDimBg]     = ImVec4(0.20f, 0.22f, 0.27f, 0.73f);
}

void darkCyanYellow()
{
    ImGuiStyle& style = ImGui::GetStyle();

    style.Colors[ImGuiCol_Text]                 = ImVec4(0.90f, 0.90f, 0.90f, 0.90f);
    style.Colors[ImGuiCol_TextDisabled]         = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_WindowBg]             = ImVec4(0.09f, 0.09f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_ChildBg]              = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg]              = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_Border]               = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
    style.Colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]              = ImVec4(0.00f, 0.00f, 0.01f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.60f, 0.60f, 0.60f, 0.15f);
    style.Colors[ImGuiCol_FrameBgActive]        = ImVec4(0.60f, 0.60f, 0.60f, 0.30f);
    style.Colors[ImGuiCol_TitleBg]              = ImVec4(0.00f, 0.00f, 0.00f, 0.83f);
    style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(0.40f, 0.40f, 0.80f, 0.20f);
    style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(0.00f, 0.00f, 0.00f, 0.87f);
    style.Colors[ImGuiCol_MenuBarBg]            = ImVec4(0.01f, 0.01f, 0.02f, 0.80f);
    style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
    style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.55f, 0.53f, 0.55f, 0.51f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.56f, 0.56f, 0.56f, 0.91f);
    style.Colors[ImGuiCol_CheckMark]            = ImVec4(0.90f, 0.90f, 0.90f, 0.83f);
    style.Colors[ImGuiCol_SliderGrab]           = ImVec4(0.70f, 0.70f, 0.70f, 0.62f);
    style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.30f, 0.30f, 0.30f, 0.84f);
    style.Colors[ImGuiCol_Button]               = ImVec4(0.48f, 0.72f, 0.89f, 0.49f);
    style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(0.50f, 0.69f, 0.99f, 0.68f);
    style.Colors[ImGuiCol_ButtonActive]         = ImVec4(0.80f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_Header]               = ImVec4(0.30f, 0.69f, 1.00f, 0.53f);
    style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(0.44f, 0.61f, 0.86f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive]         = ImVec4(0.38f, 0.62f, 0.83f, 1.00f);
    style.Colors[ImGuiCol_Separator]            = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered]     = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_SeparatorActive]      = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(1.00f, 1.00f, 1.00f, 0.85f);
    style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(1.00f, 1.00f, 1.00f, 0.60f);
    style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4(1.00f, 1.00f, 1.00f, 0.90f);
    style.Colors[ImGuiCol_PlotLines]            = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]        = ImVec4(0.70f, 0.50f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.90f, 0.50f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
    style.Colors[ImGuiCol_ModalWindowDimBg]     = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}


void darkBlackGrayRed()
{
    ImGuiStyle& style = ImGui::GetStyle();

    const ImVec4 chk(.6, 0.0, 0.0, 1.00f);
    const ImVec4 ch1(.4, 0.0, 0.0, 1.00f);
    const ImVec4 ch2(.5, 0.0, 0.0, 1.00f);
    const ImVec4 but(0.40f, 0.50f, 0.75f, 1.00f);


    style.Colors[ImGuiCol_Text]                 = ImVec4(0.90f, 0.90f, 0.90f, 0.90f);
    style.Colors[ImGuiCol_TextDisabled]         = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_WindowBg]             = ImVec4(0.09f, 0.09f, 0.15f, 1.00f);
    style.Colors[ImGuiCol_ChildBg]              = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg]              = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_Border]               = ImVec4(0.70f, 0.70f, 0.70f, 0.65f);
    style.Colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]              = ImVec4(0.00f, 0.00f, 0.01f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.70f, 0.70f, 0.70f, 0.20f);
    style.Colors[ImGuiCol_FrameBgActive]        = ImVec4(0.90f, 0.00f, 0.00f, 0.75f);
    style.Colors[ImGuiCol_TitleBg]              = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(but.x, but.y, but.z, 0.35f);
    style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(0.30f, 0.30f, 0.37f, 0.70f);
    style.Colors[ImGuiCol_MenuBarBg]            = ImVec4(0.01f, 0.01f, 0.02f, 0.80f);
    style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.20f, 0.25f, 0.30f, 0.60f);
    style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.55f, 0.53f, 0.55f, 0.51f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ch1;
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ch2;
    style.Colors[ImGuiCol_CheckMark]            = ImVec4(chk.x, chk.y, chk.z, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]           = ch1;
    style.Colors[ImGuiCol_SliderGrabActive]     = ch2;
    style.Colors[ImGuiCol_Button]               = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
    style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(but.x, but.y, but.z, 1.00f);
    style.Colors[ImGuiCol_ButtonActive]         = ImVec4(0.60f, 0.00f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_Header]               = ImVec4(but.x, but.y, but.z, 0.30f);
    style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(but.x, but.y, but.z, 0.75f);
    style.Colors[ImGuiCol_HeaderActive]         = ImVec4(but.x, but.y, but.z, 0.50f);
    style.Colors[ImGuiCol_Separator]            = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered]     = ImVec4(0.70f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_SeparatorActive]      = ImVec4(0.90f, 0.70f, 0.70f, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(0.50f, 0.50f, 0.50f, 0.50f);
    style.Colors[ImGuiCol_ResizeGripHovered]    = ch1;
    style.Colors[ImGuiCol_ResizeGripActive]     = ch2;
    style.Colors[ImGuiCol_PlotLines]            = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]        = ch1;
    style.Colors[ImGuiCol_PlotHistogramHovered] = ch2;
    style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(0.00f, 0.00f, 1.00f, 0.35f);
    style.Colors[ImGuiCol_ModalWindowDimBg]     = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}


void lightGrayScale()
{
    ImGuiStyle& style = ImGui::GetStyle();

    style.Colors[ImGuiCol_Text]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.85f);
    style.Colors[ImGuiCol_TextDisabled]         = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_WindowBg]             = ImVec4(0.94f, 0.94f, 0.94f, 1.00f);
    style.Colors[ImGuiCol_ChildBg]              = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg]              = ImVec4(1.00f, 1.00f, 1.00f, 0.98f);
    style.Colors[ImGuiCol_Border]               = ImVec4(0.00f, 0.00f, 0.00f, 0.44f);
    style.Colors[ImGuiCol_BorderShadow]         = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_FrameBg]              = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4(0.64f, 0.65f, 0.66f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive]        = ImVec4(0.64f, 0.65f, 0.66f, 0.40f);
    style.Colors[ImGuiCol_TitleBg]              = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(0.71f, 0.70f, 0.70f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
    style.Colors[ImGuiCol_MenuBarBg]            = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
    style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(0.69f, 0.69f, 0.69f, 0.80f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.49f, 0.49f, 0.49f, 0.80f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
    style.Colors[ImGuiCol_CheckMark]            = ImVec4(0.43f, 0.43f, 0.43f, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]           = ImVec4(0.63f, 0.63f, 0.63f, 0.78f);
    style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4(0.43f, 0.44f, 0.46f, 0.78f);
    style.Colors[ImGuiCol_Button]               = ImVec4(0.61f, 0.61f, 0.62f, 0.40f);
    style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(0.57f, 0.57f, 0.57f, 0.52f);
    style.Colors[ImGuiCol_ButtonActive]         = ImVec4(0.61f, 0.63f, 0.64f, 1.00f);
    style.Colors[ImGuiCol_Header]               = ImVec4(0.64f, 0.64f, 0.65f, 0.31f);
    style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(0.58f, 0.58f, 0.59f, 0.55f);
    style.Colors[ImGuiCol_HeaderActive]         = ImVec4(0.52f, 0.52f, 0.52f, 0.55f);
    style.Colors[ImGuiCol_Separator]            = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered]     = ImVec4(0.17f, 0.17f, 0.17f, 0.89f);
    style.Colors[ImGuiCol_SeparatorActive]      = ImVec4(0.17f, 0.17f, 0.17f, 0.89f);
    style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(0.80f, 0.80f, 0.80f, 0.56f);
    style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(0.39f, 0.39f, 0.40f, 0.67f);
    style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4(0.39f, 0.39f, 0.40f, 0.67f);
    style.Colors[ImGuiCol_PlotLines]            = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]        = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
    style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(0.71f, 0.72f, 0.73f, 0.57f);
    style.Colors[ImGuiCol_ModalWindowDimBg]     = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
    style.Colors[ImGuiCol_DragDropTarget]       = ImVec4(0.16f, 0.16f, 0.17f, 0.95f);
}

// From https://github.com/procedural/gpulib/blob/master/gpulib_imgui.h
struct ImVec3 { float x, y, z; ImVec3(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f) { x = _x; y = _y; z = _z; } };

void imgui_easy_theming(ImVec3 color_for_text, ImVec3 color_for_head, ImVec3 color_for_area, ImVec3 color_for_body)
{
    ImGuiStyle& style = ImGui::GetStyle();


    style.Colors[ImGuiCol_Text]                 = ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 1.00f);
    style.Colors[ImGuiCol_TextDisabled]         = ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 0.58f);
    style.Colors[ImGuiCol_WindowBg]             = ImVec4(color_for_body.x, color_for_body.y, color_for_body.z, 1.00f);
    style.Colors[ImGuiCol_ChildBg]              = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 0.58f);
    style.Colors[ImGuiCol_PopupBg]              = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 1.00f);
    //  style.Colors[ImGuiCol_PopupBg]              = ImVec4(color_for_pops.x, color_for_pops.y, color_for_pops.z, 0.92f);
    style.Colors[ImGuiCol_Border]               = ImVec4(color_for_body.x, color_for_body.y, color_for_body.z, 0.00f);
    style.Colors[ImGuiCol_BorderShadow]         = ImVec4(color_for_body.x, color_for_body.y, color_for_body.z, 0.00f);
    style.Colors[ImGuiCol_FrameBg]              = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]       = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.78f);
    style.Colors[ImGuiCol_FrameBgActive]        = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    style.Colors[ImGuiCol_TitleBg]              = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed]     = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 0.75f);
    style.Colors[ImGuiCol_TitleBgActive]        = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    style.Colors[ImGuiCol_MenuBarBg]            = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 1.00f);
    style.Colors[ImGuiCol_ScrollbarBg]          = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrab]        = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.50f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.78f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    style.Colors[ImGuiCol_CheckMark]            = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    style.Colors[ImGuiCol_SliderGrab]           = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.50f);
    style.Colors[ImGuiCol_SliderGrabActive]     = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    style.Colors[ImGuiCol_Button]               = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.50f);
    style.Colors[ImGuiCol_ButtonHovered]        = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.86f);
    style.Colors[ImGuiCol_ButtonActive]         = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    style.Colors[ImGuiCol_Header]               = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.76f);
    style.Colors[ImGuiCol_HeaderHovered]        = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.86f);
    style.Colors[ImGuiCol_HeaderActive]         = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    style.Colors[ImGuiCol_Separator]            = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.32f);
    style.Colors[ImGuiCol_SeparatorHovered]     = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.78f);
    style.Colors[ImGuiCol_SeparatorActive]      = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    style.Colors[ImGuiCol_ResizeGrip]           = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.15f);
    style.Colors[ImGuiCol_ResizeGripHovered]    = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.78f);
    style.Colors[ImGuiCol_ResizeGripActive]     = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    style.Colors[ImGuiCol_PlotLines]            = ImVec4(color_for_text.x, color_for_text.y, color_for_text.z, 0.63f);
    style.Colors[ImGuiCol_PlotLinesHovered]     = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram]        = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.50f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.75f);
    style.Colors[ImGuiCol_TextSelectedBg]       = ImVec4(color_for_head.x, color_for_head.y, color_for_head.z, 0.43f);
    style.Colors[ImGuiCol_ModalWindowDimBg]     = ImVec4(color_for_area.x, color_for_area.y, color_for_area.z, 0.73f);
}

void SetupImGuiStyle2()
{

    static ImVec3 color_for_text = ImVec3(236.f / 255.f, 240.f / 255.f, 241.f / 255.f);
    static ImVec3 color_for_head = ImVec3(41.f / 255.f, 128.f / 255.f, 185.f / 255.f);
    static ImVec3 color_for_area = ImVec3(57.f / 255.f, 79.f / 255.f, 105.f / 255.f);
    static ImVec3 color_for_body = ImVec3(44.f / 255.f, 62.f / 255.f, 80.f / 255.f);
    static ImVec3 color_for_pops = ImVec3(33.f / 255.f, 46.f / 255.f, 60.f / 255.f);
    
/*
    Original post
    static ImVec3 color_for_text = ImVec3( .80f,  .80f,  .80f);
    static ImVec3 color_for_head = ImVec3(0.10f, 0.67f, 1.00f);
    static ImVec3 color_for_area = ImVec3(0.20f, 0.22f, 0.27f);
    static ImVec3 color_for_body = ImVec3(0.13f, 0.14f, 0.17f);
    //static ImVec3 color_for_pops = ImVec3(33.f / 255.f, 46.f / 255.f, 60.f / 255.f);
  */
    imgui_easy_theming(color_for_text, color_for_head, color_for_area, color_for_body);
}

void selectTheme(int style_idx);

void setGUIStyle()
{
#ifdef __EMSCRIPTEN__
    if(theApp->isTabletMode()) theDlg.setFontSize(15);
#endif

    ImGuiStyle& style = ImGui::GetStyle();

    //SetupImGuiStyle2();
    //darkDefaultTheme(theDlg.getGuiThemeBaseColor());
    selectTheme(theDlg.getSelectedGuiTheme());

    style.Colors[ImGuiCol_WindowBg].w = theDlg.getGuiThemeBaseColor().w;
    style.Alpha = theDlg.getGuiThemeBaseColor().w < .2 ? .2 : theDlg.getGuiThemeBaseColor().w;



    style.ItemSpacing = ImVec2(2,3); 
    style.ItemInnerSpacing = ImVec2(2,3); 

    style.FrameRounding = 3;
    style.WindowRounding = 5;
    style.ScrollbarSize = theApp->isTabletMode() ? 16 : 11;
    style.GrabRounding = 3;
    style.WindowPadding = ImVec2(3,3);
    style.ChildRounding = 3;
    style.ScrollbarRounding = 5;
    style.FramePadding = ImVec2(3,3);

    style.AntiAliasedLines = true;
    style.AntiAliasedFill = true;

    style.ScaleAllSizes(1.3);
    //style.FrameBorderSize = .0f;
    
    //style.WindowTitleAlign = ImVec2(.03f,.5f);
    


    ImFontConfig cfg;
    //memset(&cfg, 0, sizeof(ImFontConfig));
    cfg.OversampleH = 3.;
    cfg.OversampleV = 1.;
    cfg.PixelSnapH = true;
    cfg.GlyphExtraSpacing.x = 0.0f;
    cfg.GlyphExtraSpacing.y = 0.0f;
    cfg.RasterizerMultiply = 1.25f;

    //ImVec4 color(style.Colors[ImGuiCol_FrameBg]);

    //ImVec4 v(style.Colors[ImGuiCol_FrameBg]);
//    theDlg.setTableAlterbateColor1(ImVec4(v.x*1.1,v.y*1.1,v.z*1.1,v.w));
    //theDlg.setTableAlterbateColor2(ImVec4(v.x*.9,v.y*.9,v.z*.9,v.w));

    ImGuiIO& io = ImGui::GetIO();
    //io.FontAllowUserScaling = true;
    io.Fonts->Clear();
    theDlg.mainFont = io.Fonts->AddFontFromFileTTF("Fonts/Cousine-Regular.ttf", theDlg.getFontSize(), &cfg);

    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
    ImFontConfig icons_config; 
    icons_config.MergeMode = true; 
    icons_config.GlyphOffset=ImVec2(0,1); 
    //icons_config.PixelSnapH = true;

    //icons_config.OversampleH = 3.;
    //icons_config.OversampleV = 1.;
    //icons_config.GlyphExtraSpacing.x = 0.0f;
    //icons_config.GlyphExtraSpacing.y = -2.0f;

    theDlg.iconFont = io.Fonts->AddFontFromFileTTF( "Fonts/fontawesome-webfont.ttf", theDlg.getFontSize()+1, &icons_config, icons_ranges );

#ifdef GLCHAOSP_USE_MARKDOWN
    theDlg.mdConfig.headingFormats[ 0 ].font = io.Fonts->AddFontFromFileTTF("Fonts/Cousine-Regular.ttf", theDlg.getFontSize() * 2.0);
    theDlg.mdConfig.headingFormats[ 1 ].font = io.Fonts->AddFontFromFileTTF("Fonts/Cousine-Regular.ttf", theDlg.getFontSize() * 1.5);
    theDlg.mdConfig.headingFormats[ 2 ].font = io.Fonts->AddFontFromFileTTF("Fonts/Cousine-Regular.ttf", theDlg.getFontSize() * 1.2);
#endif
    //theDlg.testFont = ImGui::GetIO().Fonts->AddFontFromFileTTF("Fonts/Cousine-Regular.ttf", theDlg.getFontSize(), &theDlg.fontCFG);


}


// Demo helper function to select among default colors. See ShowStyleEditor() for more advanced options.
// Here we use the simplified Combo() api that packs items into a single literal string. Useful for quick combo boxes where the choices are known locally.


void selectTheme(int style_idx)
{

#ifdef __EMSCRIPTEN__
        if(theApp->useLightGUI()) lightGrayScale();
        else                      darkBlackGrayRed(); //darkDefaultTheme(theDlg.getGuiThemeBaseColor());
#else
        switch (style_idx)
        {
            case  0: darkDefaultTheme(theDlg.getGuiThemeBaseColor()); break;
            case  1: darkDefaultTheme(theDlg.getGuiThemeBaseColor(), true); break;
            case  2: darkDefaultGrayButt(theDlg.getGuiThemeBaseColor()); break;
            case  3: darkColorsOnBlack(theDlg.getGuiThemeBaseColor()); break;
            case  4: darkSolarized(); break;
                  
            case  5: SetupImGuiStyle2(); break;
            case  6: colorTheme6(); break;
                  
            case  7: darkCyanYellow(); break;
            case  8: darkBlackGrayRed(); break;
            case  9: lightCyanYellow(); break;
            case 10: lightGrayScale(); break;

            case 11: ImGui::StyleColorsClassic(); break;
            case 12: ImGui::StyleColorsDark(); break;
            case 13: ImGui::StyleColorsLight(); break;
        }
#endif
}


int ShowStyleSelector(const float wB)
{
    const char* label = "##cmbThem";
    int style_idx = theDlg.getSelectedGuiTheme();

    const float wButt = style_idx < 3.f ? 
                        wB - ImGui::GetFrameHeightWithSpacing() :
                        wB - ImGui::GetFrameHeightWithSpacing() * 2;

    ImGui::PushItemWidth(wButt);
    if (ImGui::Combo(label, &style_idx,  "Default (dark)\0"\
                                         "Default always RED buttons (dark)\0"\
                                         "Default gray buttons (dark)\0"\
                                         "Colors on black (dark)\0"\
                                         "Solarized (dark)\0"\
                                         "SetupStyle\0"\
                                         "Cyan/White on Grey (dark)\0"\
                                         "Cyan/Yellow on Gray/Black (dark)\0"\
                                         "Red on Gray/Black (dark)\0"\
                                         "Cyan/Yellow on White (light)\0"\
                                         "Grey scale on White (light)\0"\
                                         "ImGui Classic\0"\
                                         "ImGui Dark\0"\
                                         "ImGui Light\0"         
                    ))
    {
        selectTheme(style_idx);
        theDlg.setSelectedGuiTheme(style_idx);
    }
    ImGui::PopItemWidth();

    ImGui::SameLine();

    auto setAlpha = [] ()
    {
            ImGuiStyle& style = ImGui::GetStyle();

            style.Colors[ImGuiCol_WindowBg].w = theDlg.getGuiThemeBaseColor().w;
            style.Alpha = theDlg.getGuiThemeBaseColor().w < .2 ? .2 : theDlg.getGuiThemeBaseColor().w;
    };

    if(style_idx < 3.f) { //Show Color picker
        ImVec4 col = theDlg.getGuiThemeBaseColor();
        if(ImGui::ColorEdit4("##baseCol", (float *) &col,ImGuiColorEditFlags_NoInputs)) {
            theDlg.setGuiThemeBaseColor(col);
            selectTheme(style_idx);

            setAlpha();


        }
    } else { //Show only alpha
        ImGui::PushItemWidth(ImGui::GetFrameHeightWithSpacing() * 2);
        float f = theDlg.getGuiThemeBaseColor().w;
        if(ImGui::DragFloat("##alphThem", &f, 0.01f, 0.f, 1.0f,"%.2f")) {
            theDlg.getGuiThemeBaseColor().w = f;
            setAlpha();
        }
        ImGui::PopItemWidth();
    }


    return style_idx;
}

