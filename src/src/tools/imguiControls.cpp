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
#include "imguiControls.h"


namespace ImGui
{
//////////////////////////////////////////////////////////////////
bool hslTuning(const char* label, float *selVal, int barType, float stepSize, const ImVec2 &size, const ImVec2 &padd)
{

    bool value_changed = false;

    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems) return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    ImGuiIO& io = g.IO; 
    //if padding ImVec2(-1, -1) --> padding = FramePadding
    ImVec2 padding = (padd.x == -1 && padd.y == -1) ? style.FramePadding : padd;

    //window.
    const ImGuiID id = window->GetID(label);

    ImVec2 barSize = size - padding*2;
    const ImRect bb(ImVec2(window->DC.CursorPos), ImVec2(window->DC.CursorPos+size));
    const float barThickness = padding.y<3 ? 3 : padding.y;

    ImVec2 picker_pos = window->DC.CursorPos;
    ImVec2 bar_pos = window->DC.CursorPos+padding;
    bar_pos.y = window->DC.CursorPos.y+size.y-(barThickness+padding.y*.5+.5);
    barSize.y=barThickness;    

    ItemSize(bb);
    if (!ItemAdd(bb, id)) return false;

    SetCursorScreenPos(ImVec2(bar_pos.x, picker_pos.y));
    InvisibleButton(label, ImVec2(bb.GetWidth(), bb.GetHeight()));

    // Calculate value
#ifdef HSL_CONTROL_PROPORTIONAL
    //Proportional drag value (like DragFloat)
    //value_changed = DragBehavior(bb, id, selVal, .005f, -1.f, 1.f, 3, 1);
    const float min=-1.f, max = 1.f;
    value_changed = DragBehavior(id, ImGuiDataType_Float, selVal, stepSize, (void *)&min, (void *)&max, "%3f", 1, ImGuiDragFlags_None);
#else
    // ONE <-> ONE value (like slider)
    if (IsItemActive())
    {
        float mousePos = io.MousePos.x<bar_pos.x ? bar_pos.x : (io.MousePos.x>bar_pos.x+barSize.x ? bar_pos.x+barSize.x : io.MousePos.x);
        *selVal = -1.f + ImSaturate((mousePos - bar_pos.x) / (barSize.x)) * 2.f;
        value_changed = true;
    }
#endif

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    // Bars
    const int alpha = int(ImGui::GetStyle().Alpha*255.f);
    if(barType == HSL_TUNING_H) {
        const ImU32 hue_colors[6+1] = { IM_COL32(255,0,0,alpha), IM_COL32(255,255,0,alpha), IM_COL32(0,255,0,alpha), IM_COL32(0,255,255,alpha), IM_COL32(0,0,255,alpha), IM_COL32(255,0,255,alpha), IM_COL32(255,0,0,alpha) };
        for (int i = 0; i < 6; ++i)
            draw_list->AddRectFilledMultiColor(ImVec2(bar_pos.x +  i      * (barSize.x / 6.f), bar_pos.y), 
                                               ImVec2(bar_pos.x + (i + 1) * (barSize.x / 6.f), bar_pos.y + barSize.y), 
                                               hue_colors[i], hue_colors[i + 1], hue_colors[i + 1], hue_colors[i]);
    } else {
        const ImU32 endColor = (barType == HSL_TUNING_L) ?  IM_COL32(255, 255, 255, alpha) : IM_COL32(255, 0, 0, alpha);
        draw_list->AddRectFilledMultiColor(bar_pos, bar_pos+barSize, IM_COL32(0, 0, 0, alpha), endColor, endColor, IM_COL32(0, 0, 0, alpha));


    }
    // Marker
    RenderFrameBorder(bb.Min, bb.Max, 0.0f);
    float posX = padding.x + picker_pos.x + (*selVal + 1) * .5 * barSize.x;

    ImColor col(style.Colors[ImGuiCol_Text]);
    col.Value.w*=ImGui::GetStyle().Alpha;
    draw_list->AddLine(ImVec2(posX, picker_pos.y-1), ImVec2(posX, picker_pos.y+size.y-1), col); 

    // Text
    char value_buf[64];
    const char* value_buf_end = value_buf + ImFormatString(value_buf, IM_ARRAYSIZE(value_buf), "%.3f", *selVal);
    RenderTextClipped(bb.Min, bb.Max-ImVec2(0,barThickness), value_buf, value_buf_end, NULL, ImVec2(.5,.5));

    return value_changed;
}

//////////////////////////////////////////////////////////////////
bool paletteButton(const char* label, int numCol, float *buff, const ImVec2 &size)
{


    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;

    // Default to using texture ID as ID. User can still push string/integer prefixes.
    // We could hash the size/uv to create a unique ID but that would prevent the user from animating UV.
    //PushID(label);
    const ImGuiID id = window->GetID(label);
    const float lineH = window->DC.CurrLineSize.y<=0 ? (window->DC.PrevLineSize.y <=0 ? size.y : window->DC.PrevLineSize.y) : window->DC.CurrLineSize.y;
    const ImVec2 padding = ImVec2(0,0);//(frame_padding >= 0) ? ImVec2((float)frame_padding, (float)frame_padding) : style.FramePadding;
    const ImRect bb(ImVec2(window->DC.CursorPos.x+ style.FramePadding.x,window->DC.CursorPos.y), ImVec2(window->DC.CursorPos.x + size.x - style.FramePadding.x*2,window->DC.CursorPos.y+lineH) );
    ItemSize(bb);
    if (!ItemAdd(bb, id)) return false;

    //RenderFrame(bb.Min, bb.Max, col, true, ImClamp((float)ImMin(padding.x, padding.y), 0.0f, style.FrameRounding));

    float borderY =lineH<size.y ? 0 : (lineH - size.y)*.5;

    const ImVec2 posMin = bb.Min+ImVec2(0,borderY), posMax = bb.Max-ImVec2(0,borderY);
    int width = posMax.x-posMin.x;
    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    auto RenderLines = [&] (const ImU32 alpha) {
        float step = (float) numCol/(float) width;
        for(int i=0; i<width; i++) {        
            const int idx = (int)((float)i*step) * 3;
            draw_list->AddLine(ImVec2(posMin.x+i, posMin.y), ImVec2(posMin.x+i, posMax.y), 
                               IM_COL32(buff[idx]*255.f, buff[idx+1]*255.f, buff[idx+2]*255.f, alpha));
        }
    };

    auto RenderFilledRects = [&] (const ImU32 alpha) {
        float pos=0, step = (float) width / (float) numCol;
        while(--numCol) {
            draw_list->AddRectFilled(ImVec2(posMin.x+pos, posMin.y), ImVec2(posMin.x+pos+step, posMax.y),
                                     IM_COL32(buff[0]*255.f, buff[1]*255.f, buff[2]*255.f, alpha)); 
            pos+=step; buff+=3;
        }

    };

    if((numCol>>1)>=width) RenderLines(ImU32(ImGui::GetStyle().Alpha*255.f));
    else                   RenderFilledRects(ImU32(ImGui::GetStyle().Alpha*255.f));


    return false;
}

//////////////////////////////////////////////////////////////////
bool DragFloatEx(const char* label, float *v, float v_speed, float v_min, float v_max, const char* format, float power, const ImVec2 &alignment)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;
/*
    if (power != 1.0f)
        IM_ASSERT(v_min != NULL && v_max != NULL); // When using a power curve the drag needs to have known bounds
*/
    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);
    const float w = CalcItemWidth();
    const ImVec2 label_size = CalcTextSize(label, NULL, true);
    const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y*2.0f));
    const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

    ItemSize(total_bb, style.FramePadding.y);
    if (!ItemAdd(total_bb, id, &frame_bb))
        return false;

    // Default format string when passing NULL
    if (format == NULL) format = "%.3f";

    // Tabbing or CTRL-clicking on Drag turns it into an input box
    const bool hovered = ItemHoverable(frame_bb, id);
    bool temp_input_is_active = TempInputIsActive(id);
    bool temp_input_start = false;
    if (!temp_input_is_active)
    {
        const bool focus_requested = FocusableItemRegister(window, id);
        const bool clicked = (hovered && g.IO.MouseClicked[0]);
        const bool double_clicked = (hovered && g.IO.MouseDoubleClicked[0]);
        if (focus_requested || clicked || double_clicked || g.NavActivateId == id || g.NavInputId == id)
        {
            SetActiveID(id, window);
            SetFocusID(id, window);
            FocusWindow(window);
            g.ActiveIdUsingNavDirMask = (1 << ImGuiDir_Left) | (1 << ImGuiDir_Right);
            if (focus_requested || (clicked && g.IO.KeyCtrl) || double_clicked || g.NavInputId == id)
            {
                temp_input_start = true;
                FocusableItemUnregister(window);
            }
        }
    }
    if (temp_input_is_active || temp_input_start)
        return TempInputScalar(frame_bb, id, label, ImGuiDataType_Float, v, format);


    // Drag behavior
    const bool value_changed = DragBehavior(id, ImGuiDataType_Float, v, v_speed, &v_min, &v_max, format, power, ImGuiDragFlags_None);
    if (value_changed)
        MarkItemEdited(id);

    // Draw frame
    const ImU32 frame_col = GetColorU32(g.ActiveId == id ? ImGuiCol_FrameBgActive : g.HoveredId == id ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg);
    RenderNavHighlight(frame_bb, id);
    RenderFrame(frame_bb.Min, frame_bb.Max, frame_col, true, style.FrameRounding);

    // Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
    char value_buf[64];
    const char* value_buf_end = value_buf + ImFormatString(value_buf, IM_ARRAYSIZE(value_buf), format, *v);
    RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, alignment);

    if (label_size.x > 0.0f)
        RenderText(ImVec2(frame_bb.Max.x + style.ItemInnerSpacing.x, frame_bb.Min.y + style.FramePadding.y), label);

    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, window->DC.ItemFlags);
    return value_changed;
}

//////////////////////////////////////////////////////////////////
bool DragFloatNEx(const char* label, float* v, int components, float v_speed, float v_min, float v_max, const char* format, float power, const ImVec2 &alignment)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    bool value_changed = false;
    BeginGroup();
    PushID(label);
    PushMultiItemsWidths(components, CalcItemWidth());
    size_t type_size = sizeof(float);
    for (int i = 0; i < components; i++)
    {
        PushID(i);
        if (i > 0)
            SameLine(0, g.Style.ItemInnerSpacing.x);
        value_changed |= DragFloatEx("", v, v_speed, v_min, v_max, format, power, alignment);
        PopID();
        PopItemWidth();
        v = (float*)((char*)v + type_size);
    }
    PopID();

    const char* label_end = FindRenderedTextEnd(label);
    if (label != label_end)
    {
        SameLine(0, g.Style.ItemInnerSpacing.x);
        TextEx(label, label_end);
    }

    EndGroup();
    return value_changed;
}


//////////////////////////////////////////////////////////////////
bool DragFloat2Ex(const char* label, float v[2], float v_speed, float v_min, float v_max, const char* format, float power, const ImVec2 &alignment)
{
    return DragFloatNEx(label, v, 2, v_speed, v_min, v_max, format, power, alignment);
}

//////////////////////////////////////////////////////////////////
bool DragFloat3Ex(const char* label, float v[3], float v_speed, float v_min, float v_max, const char* format, float power, const ImVec2 &alignment)
{
    return DragFloatNEx(label, v, 3, v_speed, v_min, v_max, format, power, alignment);
}

//////////////////////////////////////////////////////////////////
bool DragFloat4Ex(const char* label, float v[4], float v_speed, float v_min, float v_max, const char* format, float power, const ImVec2 &alignment)
{
    return DragFloatNEx(label, v, 4, v_speed, v_min, v_max, format, power, alignment);
}
}
