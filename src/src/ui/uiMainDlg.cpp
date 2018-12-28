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
#include <sstream>
#include "../glApp.h"
#include "../glWindow.h"
#include "../attractorsBase.h"
#include "../ShadersClasses.h"

#ifdef APP_USE_IMGUI
#include <imguiControls.h>
#include <imGuIZMO.h>

#include "uiMainDlg.h"
#include "uiHelpEng.h"

extern HLSTexture hlsTexture;

int ShowStyleSelector(const float w);
void selectTheme(int style_idx);

bool loadObjFile();

void saveSettingsFile();
void loadSettingsFile();

bool show_test_window = true;
bool show_another_window = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

// Helper to display a little (?) mark which shows a tooltip when hovered.
static void ShowHelpMarker(const char* desc)
{
    ImGui::TextDisabled("  " ICON_FA_COMMENT_O);
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

static void ShowHelpOnTitle(const char* desc)
{
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}


void paletteDlgClass::view() 
{
    int id = ID;
    if(!visible()) return;

    particlesBaseClass *particles = id == 'B' ? (particlesBaseClass *) theWnd->getParticlesSystem()->shaderBillboardClass::getPtr() :
                                                (particlesBaseClass *) theWnd->getParticlesSystem()->shaderPointClass::getPtr();
    //static bool colorMapDlg=false;
    //if(ImGui::Button("ColorMaps")) colorMapDlg=true;

    ImGui::SetNextWindowSize(ImVec2(300, 440), ImGuiCond_FirstUseEver);

    if(ImGui::Begin(getTitle(), (bool *) &isVisible, ImGuiTreeNodeFlags_DefaultOpen)) {
        const int border = DLG_BORDER_SIZE;
        ImGui::BeginChild("cm",ImVec2(0, -ImGui::GetFrameHeightWithSpacing()) ); { // Leave room for 1 line below us                            
            const float w = ImGui::GetContentRegionAvailWidth(); //4/3
            ImGui::Columns(2,"pals",false);
            //ImGui::SetColumnOffset(1, w*.25);
            ImGui::SetColumnWidth(0, w*.25);
            //ImGui::SetColumnOffset(2, w*.75);

            const float buttY = (ImGui::GetIO().Fonts->Fonts[0]->FontSize * ImGui::GetIO().FontGlobalScale) - ImGui::GetStyle().FramePadding.y;
            
                
            for(int i=0; i<particles->getColorMapContainer().elements(); i++)  {
                //ImGui::Columns(2);
                ImGui::AlignTextToFramePadding();
                //ImGui::Text(" ");
                char label[128];
                sprintf(label, "%s##w%04d%c",particles->getColorMap_name(i), i,id);

                ImGui::SetCursorPosX(border);

                ImGui::AlignTextToFramePadding();
                if(ImGui::Selectable(label, particles->getSelectedColorMap()==i, ImGuiSelectableFlags_SpanAllColumns))
                    particles->selectColorMap(i);
                ImGui::NextColumn();

                ImGui::paletteButton(label,256, particles->getColorMap_pf3(i),ImVec2(w*.75,buttY ));
                //ImGui::SameLine(); //ImGui::SetCursorPosX(ImGui::GetCursorPosX()+border);
                ImGui::NextColumn();

            }

        } ImGui::EndChild();

        const float w = ImGui::GetContentRegionAvailWidth(); //4/3
        const float wButt5 = (w - (border*6)) *.2;
        const float posB5 = border + wButt5 + border;
        const float posC5 = posB5  + wButt5 + border;
        const float posD5 = posC5  + wButt5 + border;
        const float posE5 = posD5  + wButt5 + border;

        ImGui::SetCursorPosX(border);
        if(ImGui::Button("Load",   ImVec2(wButt5,0))) {
            char const * patterns[] = { "*.pal*", "*.map", "*.json" };        
            char const * fileName = theApp->openFile(PALETTE_PATH, patterns, 3);
            if(fileName!=nullptr) theWnd->getParticlesSystem()->getColorMapContainer().loadColorMaps(fileName);
        }

        ImGui::SameLine(posB5);
        if(ImGui::Button("Import", ImVec2(wButt5,0))) {
            char const * patterns[] = { "*.pal*", "*.map", "*.json", "*.sca" };        
            char const * fileName = theApp->openFile(STRATT_PATH, patterns, 4);
            if(fileName!=nullptr) theWnd->getParticlesSystem()->getColorMapContainer().importPalette(fileName);
        }
        ImGui::SameLine(posC5);
        if(ImGui::Button("Delete", ImVec2(wButt5,0))) {
            const int i = theWnd->getParticlesSystem()->shaderBillboardClass::getSelectedColorMap();
            const int j = theWnd->getParticlesSystem()->shaderPointClass::getSelectedColorMap();
            const int sel = particles->getSelectedColorMap();

            theWnd->getParticlesSystem()->getColorMapContainer().deleteCMap(sel);

            if(sel == i) theWnd->getParticlesSystem()->shaderBillboardClass::selectColorMap(sel);
            if(sel == j) theWnd->getParticlesSystem()->shaderPointClass::selectColorMap(sel);

        }

        ImGui::SameLine(posD5);
        if(ImGui::Button("Save",   ImVec2(wButt5,0))) {
            char const * patterns[] = { "*.pal*", "*.map", "*.json" };        
            char const * fileName = theApp->saveFile(PALETTE_PATH, patterns, 3);
            if(fileName!=nullptr) { writePalette(fileName, particles->getSelectedColorMap()); }
        }

        ImGui::SameLine(posE5);
        if(ImGui::Button("SaveAll",ImVec2(wButt5,0))) {
            char const * patterns[] = { "*.pal*", "*.map", "*.json" };        
            char const * fileName = theApp->saveFile(PALETTE_PATH, patterns, 3);
            if(fileName!=nullptr) { writePalette(fileName, -1); }

        }

    } ImGui::End();

            //ImGui::OK
}

//////////////////////////////////////////////////////////////////
bool colCheckButton(bool b, const char *s, const float sz)
{
    if(b) {
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImGui::GetColorU32(ImGuiCol_PlotHistogramHovered)));
        ImGui::PushStyleColor(ImGuiCol_Button,        (ImGui::GetColorU32(ImGuiCol_PlotHistogram)));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  (ImGui::GetColorU32(ImGuiCol_CheckMark)));
    }
    const bool ret = ImGui::Button(s,  ImVec2(sz,0));

    if(b) {
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
        ImGui::PopStyleColor();
    }
    return ret;
}

//////////////////////////////////////////////////////////////////
void particlesDlgClass::viewSettings(particlesBaseClass *particles, char id) 
{
    int srcSelect, dstSelect;

    const float wPaddingY = ImGui::GetStyle().WindowPadding.y;

    const float border = DLG_BORDER_SIZE;
    char base[]="##aa_";
    char idA='a';

    const ImU32 titleCol = ImGui::GetColorU32(ImGuiCol_PlotLines); 

    const float w = ImGui::GetContentRegionAvailWidth();
    const float wHalf = w*.5;

    const float posA = border;
    const float wButt2 = (wHalf-border*1.5);
    // const float posB2  -> posC4!!
        
    const float wButt3 = (w - border*4) *.333;
    const float posB3 = posA  + wButt3 + border;
    const float posC3 = posB3 + wButt3 + border;

    const float wButt4 = (w - (border*5)) *.25;
    const float posB4 = posA  + wButt4 + border;
    const float posC4 = posB4 + wButt4 + border;
    const float posD4 = posC4 + wButt4 + border;

    const float wButt5 = (w - (border*6)) *.2;
    const float posB5 = posA  + wButt5 + border;
    const float posC5 = posB5 + wButt5 + border;
    const float posD5 = posC5 + wButt5 + border;
    const float posE5 = posD5 + wButt5 + border;


    auto modCheckBox = [&] (bool b, float sz, const char *str=nullptr) {
        ImGui::PushStyleColor(ImGuiCol_Button,ImGui::GetColorU32(ImGuiCol_ChildBg));
        char txt[32];
        if(str) sprintf(txt, b ? "%s " ICON_FA_CHECK_SQUARE_O "%s" : "%s " ICON_FA_SQUARE_O "%s", str, buildID(base, idA++, id));
        else        sprintf(txt, b ? ICON_FA_CHECK_SQUARE_O "%s" : ICON_FA_SQUARE_O "%s", buildID(base, idA++, id));

        const bool ret = sz ? ImGui::Button(txt,ImVec2(sz,0)) : ImGui::Button(txt); 
        ImGui::PopStyleColor();
        return ret;
    };

    // Rendering settings
    ////////////////////////////////////
    {
        ImGui::PushStyleColor(ImGuiCol_Text, titleCol);
        const bool isOpen = ImGui::TreeNodeEx(buildID(base, idA++, id),ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoTreePushOnOpen,ICON_FA_CUBES " Rendering"); 
        //const bool isOpen = ImGui::TreeNode((std::string(ICON_FA_CUBES " Rendering")+buildID(base, idA++, id)).c_str(),);
        //const bool isOpen = ImGui::TreeNodeEx(buildID(base, idA++, id),ImGuiTreeNodeFlags_DefaultOpen,ICON_FA_CUBES " Rendering"); 
        ImGui::PopStyleColor();

        if(isOpen) {
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
            ImGui::BeginChild(buildID(base, idA++, id), ImVec2(0,ImGui::GetFrameHeightWithSpacing()*8-ImGui::GetStyle().ItemSpacing.y*3), true); 
                //////////Linea 1//////////
                ImGui::SetCursorPosX(INDENT(posA)); ImGui::TextDisabled("Blending Src"); 
                ImGui::SameLine(INDENT(posC4));     ImGui::TextDisabled("Blending Dst");           
        
                //////////Linea 2//////////
                ImGui::PushItemWidth(wButt2);
                    ImGui::SetCursorPosX(posA);  
                    {
                        int idx = particles->srcBlendIdx();
                        if(ImGui::Combo(buildID(base, idA++, id), &idx, particles->getBlendArrayStrings().data(), particles->getBlendArrayElements())) {
                            particles->setSrcBlend(particles->getBlendArray()[idx]);
                            particles->srcBlendIdx(idx);
                        }
                    }
                    ImGui::SameLine(posC4);
                    {
                        int idx = particles->dstBlendIdx();
                        if(ImGui::Combo(buildID(base, idA++, id), &idx, particles->getBlendArrayStrings().data(), particles->getBlendArrayElements())) {
                            particles->setDstBlend(particles->getBlendArray()[idx]);
                            particles->dstBlendIdx(idx);
                        }
                    }
                ImGui::PopItemWidth();
        
                //////////Linea 3//////////        
                //ImGui::SetCursorPosX(INDENT(posA));
                //ImGui::TextDisabled("PointSize"); //ImGui::SameLine(posB3);

                //////////Linea 4//////////        
                ImGui::PushItemWidth(wButt3);        
                {
                    ImGui::SetCursorPosX(posA);
                    {
                        bool b = particles->getBlendState();
                        char txt[32];
                        sprintf(txt, b ? "Blend " ICON_FA_CHECK_SQUARE_O "%s" : "Blend " ICON_FA_SQUARE_O "%s", buildID(base, idA++, id)); 
                        if(colCheckButton(b , txt, wButt3)) particles->setBlendState(b^1);
                    }
                    ImGui::SameLine(posB3);
                    {
                        bool b = particles->getDepthState();
                        char txt[32];
                        sprintf(txt, b ? "Depth " ICON_FA_CHECK_SQUARE_O "%s" : "Depth " ICON_FA_SQUARE_O "%s", buildID(base, idA++, id)); 
                        if(colCheckButton(b , txt, wButt3)) particles->setDepthState(b^1);
                    }

                    ImGui::SameLine(posC3);
                    {
                        bool b = particles->getLightState();
                        char txt[32];
                        sprintf(txt, b ? "Light " ICON_FA_CHECK_SQUARE_O "%s" : "Light " ICON_FA_SQUARE_O "%s", buildID(base, idA++, id)); 
                        if(colCheckButton(b , txt, wButt3)) particles->setLightState(b^1);
                    }

                }
                ImGui::PopItemWidth();

                ImGui::PushItemWidth(wButt3);

                    //////////Linea 5//////////        
                    ImGui::SetCursorPosX(INDENT(posA)); ImGui::TextDisabled("pointSize"); //ImGui::SameLine(posB3);
                    ImGui::SameLine(INDENT(posB3));     ImGui::TextDisabled("distAtten");                   
                    ImGui::SameLine(INDENT(posC3));     ImGui::TextDisabled("clipDist");  

                    //////////Linea 6//////////        
                    {
                        ImGui::SetCursorPosX(posA);
                        float f = particles->getSize();                    
                        if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.01, 0.01, 2000.0, "%.3f",DRAGFLOAT_POW_2,ImVec2(.93,0.5))) particles->setSize(f);
                    }
                    {
                        ImGui::SameLine(posB3);
                        float f = particles->getPointSizeFactor();                    
                        if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.01, 0.0, 100.0,  "%.3f",DRAGFLOAT_POW_3,ImVec2(.93,0.5))) particles->setPointSizeFactor(f);
                    }
                    {
                        ImGui::SameLine(posC3);                    
                        float f = particles->getClippingDist();                   
                        if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.01, 0.01, 20.0, "%.3f",1.0f,ImVec2(.93,0.5))) particles->setClippingDist(f);
                    }
                    ImGui::SetCursorPosX(INDENT(posA)); ImGui::TextDisabled("alphaK");     
                    ImGui::SameLine(INDENT(posB3));     ImGui::TextDisabled("alphaAtten"); //ImGui::SameLine(); ImGui::TextDisabled(" ?");
                    ImGui::SameLine(INDENT(posC3));     ImGui::TextDisabled("alphaSkip");

                    {
                        ImGui::SetCursorPosX(posA);
                        float f = particles->getAlphaKFactor();                   
                        if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.01, 0.01, 10.0, "%.3f",1.0f,ImVec2(.93,0.5))) particles->setAlphaKFactor(f);
                    }
                    {
                        ImGui::SameLine(posB3);
                        float f = particles->getAlphaAtten();                   
                        if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.01,-0.0, 100.0, "%.3f",DRAGFLOAT_POW_3,ImVec2(.93,0.5))) particles->setAlphaAtten(f);
                    }
                    {
                        ImGui::SameLine(posC3);
                        float f = particles->getAlphaSkip();
                        if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.001, 0.0,  1.0, "%.3f",1.0f,ImVec2(.93,0.5))) particles->setAlphaSkip(f);
                    }
                ImGui::PopItemWidth();

                ImGui::SetCursorPosX(posA);
                    {
                        bool b = particles->getDotType();
                        char txt[32];
                        sprintf(txt, b ? "solidDot " ICON_FA_CHECK_SQUARE_O "%s" : "solidDot " ICON_FA_SQUARE_O "%s", buildID(base, idA++, id)); 
                        if(colCheckButton(b , txt, wButt3)) particles->setDotType(b^1);
                    }


            ImGui::EndChild();
            ImGui::PopStyleVar();
        }
    }
            
    // Color settings
    ////////////////////////////////////
    {
        ImGui::PushStyleColor(ImGuiCol_Text, titleCol);
        const bool isOpen = ImGui::TreeNodeEx(buildID(base, idA++, id),ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoTreePushOnOpen,ICON_FA_TINT " Colors");
        ImGui::PopStyleColor();
        if(isOpen) {
            ImGuiIO& io = ImGui::GetIO();                
            const float fontSize = io.Fonts->Fonts[0]->FontSize;

            ImGuiStyle& style = ImGui::GetStyle();
            const float buttY = style.FramePadding.y*2+(fontSize * io.FontGlobalScale);

            const float w = ImGui::GetContentRegionAvailWidth()-border*2;

            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
            ImGui::BeginChild(buildID(base, idA++, id), ImVec2(0,ImGui::GetFrameHeightWithSpacing()*7-ImGui::GetStyle().ItemSpacing.y*2), true);

                ColorMapSettingsClass *cmSet = particles->getCMSettings();



                //////////Linea 1//////////

                {
                    ImGui::SetCursorPosX(posA);
                    bool b = cmSet->getClamp();
                    char s[32];
                    sprintf(s,"%s%s", "Clamp", buildID(base, idA++, id));
                    if(ImGui::Checkbox(s, &b)) cmSet->setClamp(b);
                }


                // ColormapName
                ////////////////////////////////////
                 ImGui::SameLine(INDENT(posB4)); ImGui::TextDisabled(" Colormap: "); 
                                        ImGui::SameLine(); ImGui::Text(particles->getColorMap_name());
                //ImGui::SameLine(INDENT(posC3));     ImGui::TextDisabled("Col.Vel."); 
                {
                    ImGui::SetCursorPosX(posA);
                    bool b = cmSet->getReverse();
                    char s[32];
                    sprintf(s,"%s%s", "Invert", buildID(base, idA++, id));
                    if(ImGui::Checkbox(s, &b)) cmSet->setReverse(b);
                }

                //////////Linea 2 //////////
                
                // PushButton Colormap
                ////////////////////////////////////
                ImGui::SameLine(posB4);
                //if(ImGui::colormapButton("pippo", ImVec2(w-10,12), 256, particles->getSelectedColorMap_pf3()))
                if(ImGui::ImageButton(reinterpret_cast<ImTextureID>(cmSet->getOrigTex()), ImVec2((wButt4)*3-border,fontSize * ImGui::GetIO().FontGlobalScale)))
                    if(id=='B') theDlg.bbPaletteDlg.visible( theDlg.bbPaletteDlg.visible()^1);
                    else        theDlg.psPaletteDlg.visible( theDlg.psPaletteDlg.visible()^1);
               

                //ImGui::colormapButton("pluto", ImVec2(300,16), 256, particles->getColorMap().getRGB_pf3(1));
                
                ////////// Linea 3 e 4  - text e widgets //////////

                // Vel / start 
                ////////////////////////////////////
                const float wButt3Half = (wButt3-border)*.5;
                const float pos3H = wButt3Half + border*2;
                
                ImGui::SetCursorPosX(INDENT(posA )); ImGui::TextDisabled("Range");
                ImGui::SameLine(     INDENT(posB3)); ImGui::TextDisabled("Offset");
                ImGui::SameLine(     INDENT(posC3)); ImGui::TextDisabled("Color Vel."); 
                
                ImGui::PushItemWidth(wButt3);
                {
                    ImGui::SetCursorPosX(posA);
                    float f = cmSet->getRange(); 
                    if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.0025, 0.0, 20.0, "% .3f",1.0f,ImVec2(.07,0.5))) cmSet->setRange(f);
                }
                {
                    ImGui::SameLine(posB3);
                    float f = cmSet->getOffsetPoint();
                    if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.0025, -10.0, 10.0, "% .3f",1.0f,ImVec2(.07,0.5))) cmSet->setOffsetPoint(f);
                }

                {
                    ImGui::SameLine(posC3);
                    float f = cmSet->getVelIntensity();
                    if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.001, 0.001, 100.0, "% .3f",1.0f,ImVec2(.07,0.5))) cmSet->setVelIntensity(f);
                }
                ImGui::PopItemWidth();
                
    
                // Image Palette
                ////////////////////////////////////
                ImGui::SetCursorPosX(border);
                ImGui::Image(reinterpret_cast<ImTextureID>(cmSet->getModfTex()), ImVec2(w-border,buttY));


                // HLSL Controls
                ////////////////////////////////////
                ImGui::PushItemWidth(wButt3);        
                {
                    ImGui::SetCursorPosX(border);
                    float f = cmSet->getH();                    
                    //if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.001, -1.0, 1.0, "%.3f",1.0f,ImVec2(.93,0.5))) particles->getCMTex()->setH(f);
                    if(ImGui::hslTuning(buildID(base, idA++, id), &f, HSL_TUNING_H, ImVec2(wButt3, buttY)))  cmSet->setH(f);
                    //ImGui::SameLine(border+5); ImGui::Text("%.2f",f);
                }
                {
                    ImGui::SameLine(posB3);
                    float f = cmSet->getS();
                    if(ImGui::hslTuning(buildID(base, idA++, id), &f, HSL_TUNING_S, ImVec2(wButt3, buttY),ImVec2(3, 3))) cmSet->setS(f);
                }
                {
                    ImGui::SameLine(posC3);
                    float f = cmSet->getL(); 
                    if(ImGui::hslTuning(buildID(base, idA++, id), &f, HSL_TUNING_L, ImVec2(wButt3, buttY),ImVec2(3, 3))) cmSet->setL(f);
                }
                ImGui::PopItemWidth();
                
                ImGui::SetCursorPosX(INDENT(border)); ImGui::TextDisabled("Hue"); 
                ImGui::SameLine(INDENT(posB3));       ImGui::TextDisabled("Saturation"); 
                ImGui::SameLine(INDENT(posC3));       ImGui::TextDisabled("Lightness"); 

            ImGui::EndChild();
            ImGui::PopStyleVar();
        }
    }

    // Light settings
    ////////////////////////////////////
    {
        ImGui::PushStyleColor(ImGuiCol_Text, titleCol);
        const bool isOpen = ImGui::TreeNodeEx(buildID(base, idA++, id),ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoTreePushOnOpen,ICON_FA_LIGHTBULB_O " Light   " ICON_FA_COMMENT_O);
        ImGui::PopStyleColor();
        ShowHelpOnTitle(GLAPP_HELP_LIGHT_TREE);
        if(isOpen) {
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
            ImGui::BeginChild(buildID(base, idA++, id), ImVec2(0,ImGui::GetFrameHeightWithSpacing()*3+ImGui::GetStyle().ItemSpacing.y), true);

                //ImGui::Columns(2);               

                const float wGizmo = ImGui::GetFrameHeightWithSpacing()*3 - ImGui::GetStyle().ItemSpacing.y;
                const float wLight = w - wGizmo - border;
                const float wLightHalf = wLight*.5;
                const float wLightButt = (wLightHalf-border*1.5);
                const float wLightPos = posA + wLightButt + border;
                const float sz = wLightButt;

                const float posA = border;

                //ImGui::PushItemWidth(wButt4);
                ImGui::AlignTextToFramePadding();

                ImGui::SetCursorPosX(posA); ImGui::TextDisabled("Diff"); ImGui::SameLine();
                ImGui::PushItemWidth(sz - (ImGui::GetCursorPosX()-posA));
                ImGui::DragFloatEx(buildID(base, idA++, id),&particles->getUData().lightDiffInt,0.01, 0.0, 10.f, "% .3f",1.0f,ImVec2(.07,0.5));
                ImGui::PopItemWidth();

                ImGui::SameLine(wLightPos);     ImGui::TextDisabled("Ambt"); ImGui::SameLine();
                ImGui::PushItemWidth(sz - (ImGui::GetCursorPosX()-wLightPos));
                ImGui::DragFloatEx(buildID(base, idA++, id),&particles->getUData().lightAmbInt, 0.01, -1.0, 2.f, "% .3f",1.0f,ImVec2(.07,0.5));
                ImGui::PopItemWidth();
                
                ImVec2 pos(ImGui::GetCursorPos());

                ImGui::SameLine(wLight);
                glm::vec3 vL(-particles->getUData().lightDir);
                if(ImGui::gizmo3D("##Bht", vL ,wGizmo))  particles->getUData().lightDir = -vL;

                ImGui::SetCursorPos(pos);

                ImGui::AlignTextToFramePadding();
                
                ImGui::SetCursorPosX(posA); ImGui::TextDisabled("Spec");  ImGui::SameLine();
                ImGui::PushItemWidth(sz - (ImGui::GetCursorPosX()-posA));
                ImGui::DragFloatEx(buildID(base, idA++, id),&particles->getUData().lightSpecInt,0.01, 0.0, 10.f, "% .3f",1.0f,ImVec2(.07,0.5));
                ImGui::PopItemWidth();
                
                ImGui::SameLine(wLightPos);     ImGui::TextDisabled("sExp");  ImGui::SameLine();
                ImGui::PushItemWidth(sz - (ImGui::GetCursorPosX()-wLightPos));
                ImGui::DragFloatEx(buildID(base, idA++, id),&particles->getUData().lightShinExp, 0.01, 1.0, 300.f, "%.3f", DRAGFLOAT_POW_3,ImVec2(.07,0.5));
                ImGui::PopItemWidth();

                ImGui::AlignTextToFramePadding();

                ImGui::SetCursorPosX(posA); ImGui::TextDisabled("ccCorr"); ImGui::SameLine();
                ImGui::PushItemWidth((wLightButt+border)*2 - (ImGui::GetCursorPosX()));
                ImGui::DragFloatRange2(buildID(base, idA++, id), &particles->getUData().sstepColorMin, &particles->getUData().sstepColorMax, .01, -1.0, 2.0);
                ImGui::PopItemWidth();


                //ImGui::NextColumn();


                //ImGui::SetCursorPos(pos);
                //ImGui::SameLine();

                //bool l = particles->lightStateIDX;
                //if(ImGui::Checkbox("light on/off", &l))  particles->lightStateIDX = l ? particles->on : particles->off;

                //ImGui::Checkbox("depth on/off", &particles->depthBuffActive);
                //ImGui::Checkbox("blend on/off", &particles->blendActive);
                // reDim axes ... same lenght, 
                //const float psG = (ImGui::GetContentRegionAvailWidth() - ((ImGui::GetFrameHeightWithSpacing()*4) - (ImGui::GetStyle().ItemSpacing.y*2))) * .5;
                //ImGui::SetCursorPosX(ImGui::GetCursorPosX()+psG);

                //imguiGizmo::resizeSolidOf(.75); // sphere bigger


                //imguiGizmo::restoreSolidSize(); // restore at default

                //ImGui::SetCursorPosX(posC4); 
/*
                ImGui::PushItemWidth(wButt2);
                ImGui::DragFloat3(buildID(base, idA++, id),glm::value_ptr(particles->getUData().lightDir),0.01f);
                ImGui::PopItemWidth();
*/
                //ImGui::SliderFloat("diffInt",&particles->getUData().lightDiffInt, 0, 20.f);
                //ImGui::SliderFloat("shi",&particles->getUData().lightShinExp, 1, 2000.f, "%.1f",5.f);
                //ImGui::SliderFloat("spec",&particles->getUData().lightSpecInt, 0, 20.f);
                //ImGui::SliderFloat("amb",&particles->getUData().lightAmbInt, -1, 2.f);

                //ImGui::SliderFloat("alphSkip",&particles->getUData().alphaSkip, 0.0, 1.f);
                //ImGui::DragFloat("ssmin",&particles->getUData().sstepColorMin,0.01, -1., 2.f);
                //ImGui::DragFloat("ssmax",&particles->getUData().sstepColorMax, 0.01, 0, 3.f);

                //ImGui::Columns(1);

            ImGui::EndChild();
            ImGui::PopStyleVar();
        }
    }
    // Glow settings
    ////////////////////////////////////
    {
        ImGui::PushStyleColor(ImGuiCol_Text, titleCol);
        const bool isOpen = ImGui::TreeNodeEx(buildID(base, idA++, id),ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoTreePushOnOpen,ICON_FA_EYE " Glow Effect   "  ICON_FA_COMMENT_O);
        ImGui::PopStyleColor();
        ShowHelpOnTitle(GLAPP_HELP_GLOW_TREE);

        if(isOpen) {
        //////////Linea 7//////////        
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
            ImGui::BeginChild(buildID(base, idA++, id), ImVec2(0,ImGui::GetFrameHeightWithSpacing()*5-wPaddingY*2), true);

                radialBlurClass *glow = particles->getGlowRender();

                int mode = glow->getGlowState();

                //////////Linea 8//////////        
                {   
                    ImGui::SetCursorPosX(posA);
/*
                    bool b = glow->getGlowState();
                    char txt[32];
                    sprintf(txt, b ? "Glow " ICON_FA_CHECK_SQUARE_O "%s" : "Glow " ICON_FA_SQUARE_O "%s", buildID(base, idA++, id)); 
                    if(colCheckButton(b , txt, wButt3)) glow->setGlowState(b^1);
                    //if(modCheckBox(b, wButt3, true)) glow->setGlowState(b^1);
*/
                    

                    {    
                        bool b = glow->isGlowOn();
                        if(ImGui::Checkbox(buildID(base, idA++, id), &b)) glow->setGlowOn(b);
                    }

                    ImGui::SameLine();

                    ImGui::PushItemWidth(wButt2 - (ImGui::GetCursorPosX()-posA));

                    {
                        int idx = mode-1;
                        if (ImGui::Combo(buildID(base, idA++, id), &idx, "gaussian blur\0"\
                                                                          "gaussian + bilateral\0"\
                                                                          "bilateral threshold\0"))
                            { glow->setGlowState(idx+1); }
                    }

                    ImGui::PopItemWidth();
                }

                {
                    ImGui::SameLine(posC4);

                    bool b = bool(glow->getSigmaRadX() == 2.0);
                    char txt[32];
                    sprintf(txt, " %d" u8"\u00D7 " "%s", b ? 2 : 3, buildID(base, idA++, id)); 
                    if(ImGui::Button(txt)) (b ? glow->setSigmaRad3X() :  glow->setSigmaRad2X());

                }

                {
                    ImGui::SameLine();
                    ImGui::PushItemWidth(w-ImGui::GetCursorPosX()-border); 

                    float f = glow->getSigma();                    
                    if(ImGui::DragFloat(buildID(base, idA++, id), &f,.01, 1.00, 40.0, "sigma: %5.3f",1.0f)) glow->setSigma(f);
                    ImGui::PopItemWidth();

                }
    
                //////////Linea 9//////////        
                ImGui::SetCursorPosX(INDENT(posA )); ImGui::TextDisabled("rendrI"); 
                if(mode == glow->glowType_Blur      || mode == glow->glowType_Threshold) { ImGui::SameLine(     INDENT(posB5)); ImGui::TextDisabled("gaussI");         }
                if(mode == glow->glowType_Bilateral || mode == glow->glowType_Threshold) { ImGui::SameLine(     INDENT(posC5)); ImGui::TextDisabled("bilatI"  );      
                                                                                           ImGui::SameLine(     INDENT(posD5)); ImGui::TextDisabled("bilat thrshld"); }
                //ImGui::SameLine(     INDENT(posD5)); ImGui::TextDisabled("render " ICON_FA_ARROWS_H " glow"); 

                //////////Linea 10//////////        
                ImGui::PushItemWidth(wButt5);        
                //glowType_Blur, glowType_Threshold, glowType_Bilateral
                {
                    ImGui::SetCursorPosX(posA);
                    float f = glow->getImgTuning()->getTextComponent();                    
                    if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.001, 0.01, 10.0, "%.3f",1.0f,ImVec2(.93,0.5))) glow->getImgTuning()->setTextComponent(f);
                }
                if(mode == glow->glowType_Blur || mode == glow->glowType_Threshold) {
                    ImGui::SameLine(posB5);
                    float f = glow->getImgTuning()->getBlurComponent();                    
                    if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.001, 0.01, 20.0, "%.3f",1.0f,ImVec2(.93,0.5))) glow->getImgTuning()->setBlurComponent(f);
                }
                if(mode == glow->glowType_Bilateral || mode == glow->glowType_Threshold) {
                    ImGui::SameLine(posC5);
                    float f = glow->getImgTuning()->getBlatComponent();                    
                    if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.001, 0.01, 20.0, "%.3f",1.0f,ImVec2(.93,0.5))) glow->getImgTuning()->setBlatComponent(f);
                }
                ImGui::PopItemWidth();


                if(mode == glow->glowType_Bilateral || mode == glow->glowType_Threshold) {
                    ImGui::SameLine(posD5); 
                    ImGui::PushItemWidth(wButt5*2+border);
                    float f = glow->getThreshold(); 
                    if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.0001, 0.001, 3.0, "%.3f",DRAGFLOAT_POW_2,ImVec2(.93,0.5))) glow->setThreshold(f);

                    ImGui::PopItemWidth();
                }                             
    

                //////////Linea 9//////////        
                ImGui::SetCursorPosX(INDENT(posA)); ImGui::TextDisabled("render " ICON_FA_ARROWS_H " glow"); 
                if(mode == glow->glowType_Threshold) { ImGui::SameLine(     INDENT(posC4)); ImGui::TextDisabled("gauss " ICON_FA_ARROWS_H " bilat"); }
                ImGui::SetCursorPosX(posA); 
                const int sz = wButt4*2+border;
                ImGui::AlignTextToFramePadding();
                {
                    //ImGui::TextDisabled("threshold"); ImGui::SameLine(); 

                    ImGui::PushItemWidth(sz);         

                    float f = glow->getMixTexture();                    
                    if(ImGui::SliderFloat(buildID(base, idA++, id), &f, -1.0f, 1.0f, "% .3f")) glow->setMixTexture(f);
                    ImGui::PopItemWidth();
                }
                if(mode == glow->glowType_Threshold) {
                    //ImGui::SameLine(wHalf); ImGui::TextDisabled("thrshldMix"); ImGui::SameLine(); 
                    //ImGui::PushItemWidth(w-ImGui::GetCursorPosX()-border);         
                    ImGui::SameLine(posC4); ImGui::PushItemWidth(sz); 
                    float f = glow->getImgTuning()->getMixBilateral();
                    if(ImGui::SliderFloat(buildID(base, idA++, id), &f, -1.0f, 1.0f, "% .3f")) glow->getImgTuning()->setMixBilateral(f);
                    ImGui::PopItemWidth();
                }


            ImGui::EndChild();
            ImGui::PopStyleVar();
        }
    }
    // FXAA settings
    ////////////////////////////////////
    {
        ImGui::PushStyleColor(ImGuiCol_Text, titleCol);
        const bool isOpen = ImGui::TreeNodeEx(buildID(base, idA++, id),ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoTreePushOnOpen,ICON_FA_EYE " FXAA Filter");
        ImGui::PopStyleColor();
        if(isOpen) {
        //////////Linea 7//////////        
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
            ImGui::BeginChild(buildID(base, idA++, id), ImVec2(0,ImGui::GetFrameHeightWithSpacing()*2+wPaddingY), true);
                
            const char *cmb1 = " 1\0 1/2\0 1/4\0 1/8\0 1/16\0 1/32\0 1/64\0 1/128\0 1/256\0 1/512\0 0\0";

            fxaaClass* fxaa = particles->getFXAA();


            ImGui::SetCursorPosX(posA);
            {
                bool b = fxaa->isOn();                
                if(ImGui::Checkbox(buildID(base, idA++, id),&b)) fxaa->activate(b);
/*
                char txt[32];
                sprintf(txt, b ? "FXAA " ICON_FA_CHECK_SQUARE_O "%s" : "FXAA " ICON_FA_SQUARE_O "%s", buildID(base, idA++, id)); 
                if(colCheckButton(b , txt, wButt3)) fxaa->activate(b^1);
*/
            }

            {

                ImGui::SameLine();
                ImGui::TextDisabled(" Span:"); ImGui::SameLine();
                ImGui::PushItemWidth(wButt2-ImGui::GetCursorPosX());

                int i = int(log2(fxaa->getSpan()))-1;
                if (ImGui::Combo(buildID(base, idA++, id), &i, "  2\0  4\0  8\0 16\0 32\0"))
                    { fxaa->setSpan(int(1<<(i+1))); }

                ImGui::PopItemWidth();

            }
            {
                ImGui::SameLine(posC4);
                ImGui::TextDisabled("Thresold:"); ImGui::SameLine();
                ImGui::PushItemWidth(w-ImGui::GetCursorPosX()-border);

                float f = fxaa->getThreshold();
                if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.001, 0.0, 1.0, "%.3f",1.0f,ImVec2(.93,0.5))) fxaa->setThreshold(f);

                ImGui::PopItemWidth();

            }

            {
                ImGui::AlignTextToFramePadding();
                ImGui::TextDisabled("Mul:"); ImGui::SameLine();
                ImGui::PushItemWidth(wButt2-ImGui::GetCursorPosX());

                int i = int(log2(fxaa->getReductMul()));
                if (ImGui::Combo(buildID(base, idA++, id), &i, cmb1))
                    { fxaa->setReductMul(int(1<<(i))); }

                ImGui::PopItemWidth();
            }
            {
                ImGui::SameLine(posC4);
                ImGui::TextDisabled("Min:"); ImGui::SameLine();
                ImGui::PushItemWidth(w-ImGui::GetCursorPosX()-border);

                int i = int(log2(fxaa->getReductMin()));
                if (ImGui::Combo(buildID(base, idA++, id), &i, cmb1))
                    { fxaa->setReductMin(int(1<<(i))); }

                ImGui::PopItemWidth();
            }

                
            ImGui::EndChild();
            ImGui::PopStyleVar();
        }
    }

    // Display Adjoust
    ////////////////////////////////////
    {
        ImGui::PushStyleColor(ImGuiCol_Text, titleCol);
        const bool isOpen = ImGui::TreeNodeEx(buildID(base, idA++, id),ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoTreePushOnOpen,ICON_FA_ADJUST " Display Adjust");
        ImGui::PopStyleColor();
        if(isOpen) {
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
            ImGui::BeginChild(buildID(base, idA++, id), ImVec2(0,ImGui::GetFrameHeightWithSpacing()*3), true);
                //ImGui::TextColored(titleCol,"Display Adjust"); 

                const float buttY = ImGui::GetStyle().FramePadding.y*2+(ImGui::GetIO().Fonts->Fonts[0]->FontSize * ImGui::GetIO().FontGlobalScale);

                ImGui::SetCursorPosX(INDENT(border)); ImGui::TextDisabled("Gamma"); 
                ImGui::SameLine(INDENT(posB4));       ImGui::TextDisabled("Expos."); 
                ImGui::SameLine(INDENT(posC4));       ImGui::TextDisabled("Bright"); 
                ImGui::SameLine(INDENT(posD4));       ImGui::TextDisabled("Contrast"); 


                imgTuningClass *imT = particles->getGlowRender()->getImgTuning();

                ImGui::SetCursorPosX(border);

                ImGui::SetCursorPosX(border);
                ImGui::PushItemWidth(wButt4);        
                {
                    float f = imT->getGamma();                    
                    if(ImGui::DragFloat(buildID(base, idA++, id), &f,.001, 0.001, 20.0, "%.3f",1.0f)) imT->setGamma(f);
                }

                ImGui::SameLine(posB4);                
                {
                    float f = imT->getExposure();
                    if(ImGui::DragFloat(buildID(base, idA++, id), &f,.001, 0.001, 10.0, "%.3f",1.0f)) imT->setExposure(f);
                
                }   
                ImGui::PopItemWidth();                

                ImGui::SameLine(posC4);
                {
                    float f = imT->getBright();
                    if(ImGui::hslTuning(buildID(base, idA++, id), &f, HSL_TUNING_L, ImVec2(wButt4-2, buttY),ImVec2(0, 0))) imT->setBright(f);
                }               

                ImGui::SameLine(posD4);
                {
                    float f = imT->getContrast();
                    if(ImGui::hslTuning(buildID(base, idA++, id), &f, HSL_TUNING_L, ImVec2(wButt4-2, buttY),ImVec2(0, 0))) imT->setContrast(f);
                
                }   

                ImGui::AlignTextToFramePadding();
                ImGui::SetCursorPosX(border);
                {
                    bool b = imT->getToneMap();
/*
                    if(ImGui::Checkbox(buildID(base, idA++, id),&b)) imT->setToneMap(b);
                    ImGui::SameLine();
                    ImGui::TextDisabled("ToneMap");
*/
                    char txt[32];
                    sprintf(txt, b ? "ToneMap " ICON_FA_CHECK_SQUARE_O "%s" : "ToneMap " ICON_FA_SQUARE_O "%s", buildID(base, idA++, id)); 
                    if(colCheckButton(b , txt, wButt3)) imT->setToneMap(b^1);


                }

                if(imT->getToneMap()) {
                    ImGui::SameLine(posB3);
                    ImGui::PushItemWidth(wButt3);
                    {
                        float f = imT->getToneMap_A();
                        if(ImGui::DragFloat(buildID(base, idA++, id),&f,.001,0.05, 3.0,"val: %.3f")) imT->setToneMap_A(f);
                    }

                    ImGui::SameLine(posC3);
                    {
                        float f = imT->getToneMap_G();
                        if(ImGui::DragFloat(buildID(base, idA++, id),&f,.001, 0.01, 1.0,"exp: %.3f")) imT->setToneMap_G(f);
                    }
                    ImGui::PopItemWidth();                
                }


/*
                ImGui::SetCursorPosX(border);   
                {
                    char txt[32];
                    bool b = particles->getGlowRender()->getImgTuning()->getDynEq();
                    sprintf(txt, b ? ICON_FA_CHECK_SQUARE_O "%s" : ICON_FA_SQUARE_O "%s", buildID(base, idA++, id));
                    ImGui::SetCursorPosX(border);
                    if(ImGui::Button(txt,ImVec2(wButt4,0))) particles->getGlowRender()->getImgTuning()->setDynEq(b^1);
                }
*/
                //ImGui::PushItemWidth(wHalf-border*1.5);        
            ImGui::EndChild();
            ImGui::PopStyleVar();
        } 

    }
}

#undef INDENT

void particlesDlgClass::view() 
{
    if(!visible()) return;

    bool bbSelected = theWnd->getParticlesSystem()->whichRenderMode==RENDER_USE_BILLBOARD || theWnd->getParticlesSystem()->getRenderMode() == RENDER_USE_BOTH;
    bool psSelected = theWnd->getParticlesSystem()->whichRenderMode==RENDER_USE_POINTS    || theWnd->getParticlesSystem()->getRenderMode() == RENDER_USE_BOTH;


    
    const float border = DLG_BORDER_SIZE;    
    particlesSystemClass *pSys = theWnd->getParticlesSystem();

    ImGui::SetNextWindowSize(ImVec2(300, 900), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
        
    const char *particlesWndName = getTitle();
    if(ImGui::Begin(particlesWndName, &isVisible)) {

        const float szMin = 270.f;
        if( ImGui::GetWindowWidth() < szMin) ImGui::SetWindowSize(particlesWndName,ImVec2(szMin,0));

        ImGui::BeginGroup(); 
            ImGui::BeginChild("Settings", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()*5-border)); 
                //static bool bBB=false, bPS=false;
                ImGui::SetNextTreeNodeOpen(psSelected || psTreeVisible);
                if(psTreeVisible = ImGui::CollapsingHeader(ICON_FA_SLIDERS " Pointsprite", ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
                    viewSettings(pSys->shaderPointClass::getPtr(), 'P');   
                }
                ImGui::SetNextTreeNodeOpen(bbSelected || bbTreeVisible);
                if(bbTreeVisible = ImGui::CollapsingHeader(ICON_FA_SLIDERS " Billboard", ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
                    viewSettings(pSys->shaderBillboardClass::getPtr(),'B');
                }
            ImGui::EndChild();  

            ImGui::BeginChild("Commons"); 
                const float w = ImGui::GetContentRegionAvailWidth();
                const float wHalf = w*.5;
                const float wButt = (w - border*3) *.33;
                const float pos2 = border*2+wButt;
                const float pos3 = border*3+wButt*2;
                    
                //////////Linea 0//////////
                // Title
                //ImGui::SetCursorPosX(wHalf - (7.5)*7); //strlen/2 * charWdth
                ImGui::SetCursorPosX(border);
                //ImGui::TextColored(ImVec4(.75f, .75f, 1.0f, 1.f), ICON_FA_SLIDERS " Common Settings");
                ImGui::Text( ICON_FA_SLIDERS " Common Settings   " ICON_FA_COMMENT_O);
                ShowHelpOnTitle(GLAPP_HELP_COMMON_TREE);

                //////////Linea 1//////////
                // Pointsprite
                ImGui::SetCursorPosX(border);
                {
                    const bool b = pSys->whichRenderMode==RENDER_USE_POINTS;
                    if(colCheckButton(b, b ? "Points " ICON_FA_CHECK : "Points", wButt)) {
                        pSys->setRenderMode(RENDER_USE_POINTS); 
                        bbTreeVisible = false; //Hide Billboard
                    }
                }

                { //Motionblur
                    ImGui::SameLine(pos2);
                    const bool b = pSys->getMotionBlur()->Active();
                    if(colCheckButton(b, b ? "M.Blur " ICON_FA_TOGGLE_ON : "M.Blur " ICON_FA_TOGGLE_OFF, wButt)) {pSys->getMotionBlur()->Active(b^1); }
                }

                { //Emitter
                    ImGui::SameLine(pos3); 
                    const bool b = pSys->getEmitter()->isEmitterOn();
                    if(colCheckButton(b, b ? "Emitter " ICON_FA_TOGGLE_ON : "Emitter " ICON_FA_TOGGLE_OFF, wButt)) {pSys->getEmitter()->setEmitter(b^1); }
                }

                //////////Linea 2//////////
                // Billboard
                ImGui::SetCursorPosX(border);
                {
                    const bool b = pSys->whichRenderMode==RENDER_USE_BILLBOARD;
                    if(colCheckButton(b, b ? "Billboard " ICON_FA_CHECK : "Billboard", wButt)) {
                        pSys->setRenderMode(RENDER_USE_BILLBOARD); 
                        psTreeVisible = false; //Hide pointSprite
                    }
                }
                    
                ImGui::SameLine(pos2);
                    
                //Motionblur slider
                if(pSys->getMotionBlur()->Active()) {
                    ImGui::PushItemWidth(wButt);
                    float f=pSys->getMotionBlur()->getBlurIntensity();                         
                    if(ImGui::SliderFloat("##blr", &f, 0.0f, 1.0f, "effect: %.3f")) pSys->getMotionBlur()->setBlurIntensity(f);
                    ImGui::PopItemWidth();
                    ImGui::SameLine(); 
                }

                ImGui::SetCursorPosX(pos3); 
                {
                    const bool b = pSys->getEmitter()->stopFull();
                    //if(colCheckButton(!b, b ? "StopFull" : "CircBuffer", wButt)) { pSys->getEmitter()->stopFull(b ? false : true); }
                    if(ImGui::Button(b ? ICON_FA_STEP_FORWARD " fullStop" : ICON_FA_FORWARD " Continue", ImVec2(wButt,0))) { pSys->getEmitter()->stopFull(b ? false : true); }
                }


                //////////Linea 3//////////
                // Both
                {
                    ImGui::SetCursorPosX(border);
                    const bool b = pSys->whichRenderMode==RENDER_USE_BOTH;
                    if(colCheckButton(b, b ? "Both " ICON_FA_CHECK : "Both", wButt)) pSys->setRenderMode(RENDER_USE_BOTH); 
                }
                
                ImGui::SameLine(pos2);
                ImGui::TextDisabled(ICON_FA_ARROW_DOWN " Buffer "  ICON_FA_ARROW_RIGHT);
                    
                ImGui::SameLine(pos3); 
                {
                    const bool b = pSys->getEmitter()->restartCircBuff();
                    if(ImGui::Button(b  ? ICON_FA_REPEAT " Restart" : ICON_FA_CIRCLE_O_NOTCH " Endless", ImVec2(wButt,0))) { pSys->getEmitter()->restartCircBuff(b ? false : true); }
                }

                             
                //////////Linea 4//////////
                // Mix slider
                if(pSys->whichRenderMode==RENDER_USE_BOTH) {                        
                    ImGui::SetCursorPosX(border);
                    ImGui::PushItemWidth(wButt);
                    float f=pSys->getMergedRendering()->getMixingVal();                        
                    if(ImGui::SliderFloat("##mix", &f, -1.0f, 1.0f, "mix: %.3f")) pSys->getMergedRendering()->setMixingVal(f);
                    ImGui::PopItemWidth();
                    ImGui::SameLine(); 
                }
                    
                ImGui::AlignTextToFramePadding();
                //ImGui::SetCursorPosX(pos3-border-9*(ImGui::GetFontSize()*.5+.5)); 
                
                ImGui::SetCursorPosX(pos2);
                { //max particles
                    ImGui::PushItemWidth(wButt);
                    float f = float(pSys->getEmitter()->getSizeCircularBuffer())*.000001f;
                    if(ImGui::DragFloat("##max", &f, .01f, .1, 
                                                     pSys->getEmitter()->getSizeAllocatedBuffer()*.000001f,"%.3f M")) {
                        int maxBuff = int(f*1000000.f);
                        pSys->getEmitter()->setSizeCircularBuffer(maxBuff>pSys->getEmitter()->getSizeAllocatedBuffer() ? pSys->getEmitter()->getSizeAllocatedBuffer() : maxBuff);
                        pSys->getEmitter()->getVBO()->resetVertexCount();
                    }
                    ImGui::PopItemWidth();
                }

                //ImGui::TextDisabled("CircBuff:");
                    
                //text particles
                ImGui::SameLine(); 
                ImGui::SetCursorPosX(pos3);
                ImGui::PushItemWidth(wButt);


                const unsigned int buffSize = pSys->getEmitter()->getSizeCircularBuffer();
                const unsigned int progBuff = pSys->getEmitter()->getParticlesCount() % buffSize;
                static unsigned int oldProgBuff = progBuff;

                static int cycle = 0;
                if(progBuff< oldProgBuff) cycle^=1; 

                oldProgBuff = progBuff;

                ImGui::LabelText("##lab", "%8dK", progBuff>>10);

                ImGui::PopItemWidth();

                //const ImU32 gauge[2] = { ImGui::GetColorU32(ImGuiCol_PlotHistogram), ImGui::GetColorU32(ImGuiCol_FrameBg) };
                const ImU32 gauge[2] = { 0xffff8000, 0xff008fff };

                ImGui::PushStyleColor(ImGuiCol_PlotHistogram, gauge[cycle]);
                ImGui::PushStyleColor(ImGuiCol_FrameBg,   gauge[cycle^1]);

                //ImU32 savedCol = ImGui::GetColorU32(ImGuiCol_FrameBg);
                {
                    //char s[30];
                    //sprintf(s, "%dK/%dM", progBuff>>10, buffSize>>20);
                    ImGui::SetCursorPosX(pos3);
                    ImGui::PushItemWidth(wButt);

                    ImGui::ProgressBar(float(progBuff)/float(buffSize), ImVec2(0.f,3.f), "##prg01");
                }
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();

                //ImGui::LabelText("##lab", "%-20d", );
                ImGui::PopItemWidth();

            ImGui::EndChild();  
        ImGui::EndGroup();
    }
    ImGui::End();

    //ColorMaps Windows
    //colorMapsSelectDlg(theWnd->getParticlesSystem()->shaderBillboardClass::getPtr(),'B');
    //colorMapsSelectDlg(theWnd->getParticlesSystem()->shaderPointClass::getPtr(),'P');
}

void comboWindowRes(const float width)
{

    ImGui::AlignTextToFramePadding();
    ImGui::TextDisabled("FrameBuffer Size:");
    ImGui::SameLine();
    ImGui::PushItemWidth(width-ImGui::GetCursorPosX()-DLG_BORDER_SIZE);

    const char *items[20] = { "", "1024x1024", "1280x720", "1920x1080", "1920x1200", "2048x2048", "2560x1440", "3440x1440" };
    int w, h;
    glfwGetWindowSize(theApp->getGLFWWnd(), &w, &h);
    char s[30];
    sprintf(s, "%dx%d (current)", w, h);
    items[0]=s;
            
    static int i;
    if (ImGui::Combo("##wSize", &i, items, 8)) {  
        sscanf(items[i], "%dx%d", &w, &h);
        theApp->setWindowSize(w,h);
        i=0;
    }
}
        
void viewSettingDlgClass::view()
{
    ImGui::SetNextWindowSize(ImVec2(300, 440), ImGuiCond_FirstUseEver);

    if(!isVisible) return;

    //bool wndVisible;
    if(ImGui::Begin(getTitle(), &isVisible)) { 
        
        particlesSystemClass *pSys = theWnd->getParticlesSystem();
        vfGizmo3DClass &tBall = pSys->getTMat()->getTrackball();

        const float w = ImGui::GetContentRegionAvailWidth();
        const float wHalf = w*.5;
        const float wButt2 = (wHalf-DLG_BORDER_SIZE*1.5);

        comboWindowRes(w);

        //  Gizmo
        ///////////////////////////////////////////////////////////////////////
        ImGui::NewLine();
        ImGui::Text(" Camera parameters");
        vec3 persp(pSys->getTMat()->getPerspAngle(),
                   pSys->getTMat()->getPerspNear() ,
                   pSys->getTMat()->getPerspFar() ); 
        if(ImGui::DragFloat3("Angle/Near/Far",value_ptr(persp),.01,0.0,0.0)) 
            pSys->getTMat()->setPerspective(persp.x, persp.y, persp.z);
        {
            vec3 v = pSys->getTMat()->getPOV();
            if(ImGui::DragFloat3("PointOfView",value_ptr(v),.01,0.0,0.0)) pSys->getTMat()->setPOV(v);
        }
        {
            vec3 v = pSys->getTMat()->getTGT();
            if(ImGui::DragFloat3("TargetView",value_ptr(v),.01,0.0,0.0)) pSys->getTMat()->setTGT(v);
        }
        {
            vec3 v(tBall.getPanPosition().x, tBall.getPanPosition().y, tBall.getDollyPosition().z);
            if(ImGui::DragFloat3("Pan/Dolly",value_ptr(v),.01,0.0,0.0)) {
                tBall.setPanPosition(vec3(v.x, v.y, 0.f));
                tBall.setDollyPosition(vec3(0.f, 0.f, v.z));
            }
        }

        //  Gizmo
        ///////////////////////////////////////////////////////////////////////
        ImGui::NewLine();
        ImGui::Text(" Object/Light Transformations");
        {
            //pSys->shaderPointClass::getPtr() whichRenderMode==RENDER_USE_BILLBOARD

            quat q(tBall.getRotation());
            vec3 l(pSys->getWhitchRenderMode()==RENDER_USE_BILLBOARD ? 
                   -pSys->shaderBillboardClass::getPtr()->getUData().lightDir :
                   -pSys->shaderPointClass::getPtr()->getUData().lightDir);

            //imguiGizmo::resizeSolidOf(.75); // sphere resize
            if(ImGui::gizmo3D("##RotB1", q, wButt2, imguiGizmo::sphereAtOrigin))  { tBall.setRotation(q); } 
            //imguiGizmo::restoreSolidSize(); // restore at default
            ImGui::SameLine();
            imguiGizmo::resizeAxesOf(vec3(.75));
            if( ImGui::gizmo3D("##RotA", l,wButt2,0)) { 
                if(pSys->getWhitchRenderMode()==RENDER_USE_BILLBOARD)
                     pSys->shaderBillboardClass::getPtr()->getUData().lightDir = -l;
                else
                     pSys->shaderPointClass::getPtr()->getUData().lightDir = -l;                
            }   
            imguiGizmo::restoreAxesSize();

            ImGui::TextDisabled(" Rotation");
            ImGui::SameLine(wHalf+DLG_BORDER_SIZE);
            ImGui::TextDisabled(" Light Pos");

            ImGui::PushItemWidth(wButt2);
            ImGui::DragFloat4("##Rot",value_ptr(q),.01,0.0,0.0);
            ImGui::SameLine();
            if(ImGui::DragFloat3("##Lux",value_ptr(l),.01,0.0,0.0)) {
                if(pSys->getWhitchRenderMode()==RENDER_USE_BILLBOARD)
                     pSys->shaderBillboardClass::getPtr()->getUData().lightDir = l;
                else
                     pSys->shaderPointClass::getPtr()->getUData().lightDir = l;                
            }
            ImGui::PopItemWidth();
            //if(ImGui::DragFloat4("Rot",value_ptr(q),.01,0.0,0.0)) tBall.setRotation(q);
        }

        //  Axes
        ///////////////////////////////////////////////////////////////////////
        ImGui::NewLine(); ImGui::Text(" Center of Rotation");
        ImGui::SameLine();
        ShowHelpMarker(GLAPP_HELP_AXES_COR);

        int axes = pSys->showAxes();
        //if() {pSys->getMotionBlur()->Active(b^1); }       
        //ImGui::Button("Show CoR",ImVec2(wButt2,0))

        {
            bool b = pSys->showAxes() == renderBaseClass::showAxesToViewCoR;
            if(colCheckButton(b , b ? ICON_FA_CHECK_SQUARE_O " Show CoR" : ICON_FA_SQUARE_O " Show CoR" , wButt2)) {
                pSys->showAxes(pSys->showAxes() == renderBaseClass::showAxesToViewCoR ? renderBaseClass::noShowAxes : renderBaseClass::showAxesToViewCoR);
            }
        }
        ImGui::SameLine();
        {
            bool b = pSys->showAxes() == renderBaseClass::showAxesToSetCoR;
            if(colCheckButton(b , b ? ICON_FA_CHECK_SQUARE_O " Modify CoR" : ICON_FA_SQUARE_O " Modify CoR", wButt2)) {
                pSys->showAxes(pSys->showAxes() == renderBaseClass::showAxesToSetCoR ? renderBaseClass::noShowAxes : renderBaseClass::showAxesToSetCoR);
            }
        }
        if(pSys->showAxes() != renderBaseClass::noShowAxes) {
            ImGui::AlignTextToFramePadding();

            if(pSys->showAxes() == renderBaseClass::showAxesToSetCoR) ImGui::TextDisabled("Modify CoR:");
            else                                                      ImGui::TextDisabled("Show CoR:  ");

            ImGui::GetStyleColorVec4(ImGuiCol_Text);

            const ImU32 alphaBG = 0x80000000, alphaHoverBG = 0xa0000000, alphaActiveBG = 0xff000000;
            const ImU32 red = 0x80, green = 0xa000, blue = 0x800000; 

            auto pushDragColor = [=] (ImU32 col) {
                if(pSys->showAxes() == renderBaseClass::showAxesToSetCoR) {
                    ImGui::PushStyleColor(ImGuiCol_FrameBg       , col + alphaBG);
                    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, col + alphaHoverBG);
                    ImGui::PushStyleColor(ImGuiCol_FrameBgActive , col + alphaActiveBG);
                }
            };

            auto popDragColor = [=] () {
                if(pSys->showAxes() == renderBaseClass::showAxesToSetCoR) {
                    ImGui::PopStyleColor();
                    ImGui::PopStyleColor();
                    ImGui::PopStyleColor();
                }
            };


            ImGui::SameLine();
            ImGui::PushItemWidth((w-ImGui::GetCursorPosX()-DLG_BORDER_SIZE)*.333);

            //ImGui::PushItemWidth(wButt2);
            //ImGui::SameLine(wHalf+DLG_BORDER_SIZE);

            ImGui::PushStyleColor(ImGuiCol_Text, 0xffffffff);

            vec3 v = tBall.getRotationCenter();

            pushDragColor(red);            
            if(ImGui::DragFloat("##RtC_X",&v.x,.01) && pSys->showAxes() == renderBaseClass::showAxesToSetCoR) 
                tBall.setRotationCenter(v);
            popDragColor();

            ImGui::SameLine();
            pushDragColor(green);
            if(ImGui::DragFloat("##RtC_Y",&v.y,.01) && pSys->showAxes() == renderBaseClass::showAxesToSetCoR) 
                tBall.setRotationCenter(v);
            popDragColor();

            ImGui::SameLine();
            pushDragColor(blue);
            if(ImGui::DragFloat("##RtC_Z",&v.z,.01) && pSys->showAxes() == renderBaseClass::showAxesToSetCoR) 
                tBall.setRotationCenter(v);
            popDragColor();

            ImGui::PopStyleColor();

            ImGui::PopItemWidth();

            {
                vec3 v = pSys->getAxes()->getZoom();

                ImGui::NewLine();                
                ImGui::Text(" Axes size:");

                ImGui::AlignTextToFramePadding();

                ImGui::TextDisabled("Thick.");
                ImGui::SameLine();
                ImGui::PushItemWidth(wHalf-ImGui::GetCursorPosX()-DLG_BORDER_SIZE);
                if(ImGui::DragFloat("##ax_xy",&v.x,.01, 0.0f, 30.0f)) pSys->getAxes()->setZoom(vec3(v.x, v.x, v.z));
                ImGui::PopItemWidth();

                ImGui::SameLine(wHalf + DLG_BORDER_SIZE);

                ImGui::TextDisabled("Len.");
                ImGui::SameLine();
                ImGui::PushItemWidth(w-ImGui::GetCursorPosX()-DLG_BORDER_SIZE);
                if(ImGui::DragFloat("##ax_z" ,&v.z,.01, 0.0f, 50.0f)) pSys->getAxes()->setZoom(v);
                ImGui::PopItemWidth();
            }

        }
    }
    ImGui::End();
}

void progSettingDlgClass::view()
{
    ImGui::SetNextWindowSize(ImVec2(300, 440), ImGuiCond_FirstUseEver);

    if(!isVisible) return;

    //bool wndVisible;
    if(ImGui::Begin(getTitle(), &isVisible)) { 

        const float w = ImGui::GetContentRegionAvailWidth();
        const float border = DLG_BORDER_SIZE;
        const float wButt = ImGui::GetContentRegionAvailWidth();
        ImGuiStyle& style = ImGui::GetStyle();

        ImGui::Text(" Size and position");

        comboWindowRes(w);

        ImGui::AlignTextToFramePadding();
        ImGui::TextDisabled("Position: ");

        //ImGui::SameLine();
        //static bool savePos;
        //if(ImGui::Checkbox("##pos", &savePos)) {    }

        ImGui::SameLine(); 
        int x,y;
        glfwGetWindowPos(theApp->getGLFWWnd(),&x,&y);
        ImGui::Text("[ %4d, %4d ]", x, y);


        bool b = bool(theApp->getVSync());
        if(ImGui::Checkbox("vSync", &b)) {
            theApp->setVSync(b ? 1 : 0);
            glfwSwapInterval(theApp->getVSync());
        }
/*
        ImGui::NewLine();

        ImGui::Text(" Glow effect override");

        ImGui::AlignTextToFramePadding();
        bool b = bool(theApp->getVSync());
        if(ImGui::Checkbox("Start always off", &b)) {
            theApp->setVSync(b ? 1 : 0);
            glfwSwapInterval(theApp->getVSync());
        }
*/
        ImGui::NewLine();

        ImGui::Text(" GUI apparence");

        ImGui::AlignTextToFramePadding();
        ImGui::TextDisabled("Theme:");
        ImGui::SameLine();
        const int idxTheme = ShowStyleSelector(wButt-ImGui::GetCursorPosX());

        ImGui::Text(" Fonts");
        ImGui::SameLine();
        ShowHelpMarker(GLAPP_HELP_FONTS);

        ImGui::AlignTextToFramePadding();
        ImGui::TextDisabled("FontSize:");
        ImGui::SameLine();
        ImGui::PushItemWidth(wButt*.5 -ImGui::GetCursorPosX() - border);
        static float ratio = theDlg.fontSize;
        {
            if(ImGui::DragFloat("##fontPx", &theDlg.fontSize,.05, 7.0, 24.0,"%.0fpx")) {
                ImGui::GetIO().FontGlobalScale = theDlg.fontSize/ratio * theDlg.fontZoom;
            }
            
        }
        ImGui::PopItemWidth();
        ImGui::SameLine(wButt*.5 + border); 
        ImGui::TextDisabled("FontScale:"); 
        ImGui::SameLine(); 
        ImGui::PushItemWidth(wButt -ImGui::GetCursorPosX());
        {
            if(ImGui::DragFloat("##fontZoom", &theDlg.fontZoom,.01, .3, 3.0,"%.2f")) {
                ImGui::GetIO().FontGlobalScale = theDlg.fontSize/ratio * theDlg.fontZoom;
            }
        }
        ImGui::PopItemWidth();




/*
        ImGui::AlignTextToFramePadding();
        ImGui::TextDisabled("Thick:");
        ImGui::SameLine();
        ImGui::PushItemWidth(wButt*.5 -ImGui::GetCursorPosX() - border);
        {
            
            if(ImGui::DragFloat("##raster", &theDlg.RasterizerMultiply,.01, .1, 2.0,"%.2f")) {
                theDlg.fontChanged = true;
            }
                          
            
        }
        ImGui::PopItemWidth();
        ImGui::SameLine(wButt*.5 + border); 
        ImGui::TextDisabled("Spacing:"); 
        ImGui::SameLine(); 
        ImGui::PushItemWidth(wButt -ImGui::GetCursorPosX());
        {
            float f = theDlg.getFontZoom();
            if(ImGui::DragFloat2("##spacing", &f,.01, .3, 3.0,"%.2f")) {
                theDlg.setFontZoom(f);
                ImGui::GetIO().FontGlobalScale=f;
            }
        }
        ImGui::PopItemWidth();

        ImGui::PushFont(theDlg.testFont);
        ImGui::Text("ABCDEFG abcdefg 0123456789");
        ImGui::PopFont();
*/

        ImGui::NewLine();

        ImGui::Text(" MAX # of particles");
        ImGui::SameLine();
        ShowHelpMarker(GLAPP_HELP_MAX_PART);

        ImGui::AlignTextToFramePadding();
        ImGui::TextDisabled("#:"); 
        ImGui::SameLine(); 
        
        ImGui::PushItemWidth(wButt*.5 -ImGui::GetCursorPosX() - border);
        static float maxBuff= theApp->getMaxAllocatedBuffer()*(.000001);
        ImGui::DragFloat("##partNum",&maxBuff,0.1, 0, PARTICLES_MAX*(.000001),"%.3f M");

        ImGui::PopItemWidth();

        ImGui::SameLine(wButt*.5 + border); 
        ImGui::TextDisabled("Mem req.: "); 
        ImGui::SameLine(); 
        ImGui::PushItemWidth(wButt -ImGui::GetCursorPosX());
        ImGui::Text("%.3f GB", (maxBuff*16.f)/(1024));

        ImGui::PopItemWidth();




        ImGui::NewLine();

        ImGui::Text(" Images capture");

        ImGui::AlignTextToFramePadding();
        if(ImGui::Button("...")) theApp->selectCaptureFolder();
        ImGui::SameLine();

        ImGui::TextDisabled(" Path: ");
        ImGui::SameLine();
        ImGui::Text(theApp->getCapturePath().c_str());




        //const float kC = .2f;
        //ImVec4 c = ImVec4(mainColorTheme.x/kC, mainColorTheme.y/kC, mainColorTheme.z/kC, 1.0f);

/*
        ImGui::SetCursorPosX(border);
        ImGui::PushItemWidth(wButt);
        
        static float f=1.0;
        ImGui::SliderFloat("##alp", &f, 0.0f, 1.0f, "alpha: %.2f");
        style.Colors[ImGuiCol_WindowBg].w = f;
        style.Alpha = f < .15 ? .15 : f;
        ImGui::PopItemWidth();
*/
        ImGui::NewLine();
        if(ImGui::Button("Save Prog.Settings",ImVec2(wButt,0))) {
            theApp->setPosX(x);
            theApp->setPosY(y);
            theApp->setMaxAllocatedBuffer(maxBuff * 1000000.f);
            theApp->saveProgConfig();
        }

    }
    ImGui::End();
}


void imGuIZMODlgClass::view()
{
    if(!isVisible) return;
    ImGuiStyle& style = ImGui::GetStyle();

    //quaternionf qt = theWnd->getTrackball().getRotation();
    glm::quat qt = theWnd->getParticlesSystem()->getTMat()->getTrackball().getRotation();

    float sz=180+ ImGui::GetStyle().ItemSpacing.x*2;
    ImGui::SetNextWindowSize(ImVec2(sz, (sz+2*ImGui::GetFrameHeightWithSpacing())), ImGuiCond_Always);

#ifdef GLAPP_IMGUI_VIEWPORT
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetMainViewport()->Pos.x+ImGui::GetMainViewport()->Size.x-sz,ImGui::GetMainViewport()->Pos.y), ImGuiCond_Always);
#else
    ImGui::SetNextWindowPos(ImVec2(theApp->GetWidth()-sz,0 ), ImGuiCond_Always);
#endif

    sz -= ImGui::GetStyle().ItemSpacing.x*2;
    
    ImGui::PushStyleColor(ImGuiCol_WindowBg,ImVec4(0.f,0.f,0.f,0.f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg,ImVec4(0.f,0.f,0.f,0.f));
    
    const float prevWindowBorderSize = style.WindowBorderSize;
    style.WindowBorderSize = .0f;

    if(ImGui::Begin(getTitle(), &isVisible, ImGuiWindowFlags_NoTitleBar|
                                         ImGuiWindowFlags_NoResize|
                                       //ImGuiWindowFlags_NoInputs|
                                         ImGuiWindowFlags_NoScrollbar)) {

    
        ImGui::SetCursorPos(ImVec2(0,0));
        ImGui::PushItemWidth(sz*.25-2);    
        ImVec4 oldTex(style.Colors[ImGuiCol_Text]);
        bool quatChanged=false;
        style.Colors[ImGuiCol_Text].x = 1.0, style.Colors[ImGuiCol_Text].y = style.Colors[ImGuiCol_Text].z =0.f;
        if(ImGui::DragFloat("##u0",(float *)&qt.x,0.01f, -1.0, 1.0, "%.2f",1.f)) quatChanged=true; ImGui::SameLine();
        style.Colors[ImGuiCol_Text].y = 1.0, style.Colors[ImGuiCol_Text].x = style.Colors[ImGuiCol_Text].z =0.f;
        if(ImGui::DragFloat("##u1",(float *)&qt.y,0.01f, -1.0, 1.0, "%.2f",1.f)) quatChanged=true;  ImGui::SameLine();
        style.Colors[ImGuiCol_Text].z = 1.0, style.Colors[ImGuiCol_Text].x = style.Colors[ImGuiCol_Text].y =0.f;
        if(ImGui::DragFloat("##u2",(float *)&qt.z,0.01f, -1.0, 1.0, "%.2f",1.f)) quatChanged=true;  ImGui::SameLine();
        style.Colors[ImGuiCol_Text] = oldTex;
        if(ImGui::DragFloat("##u3",(float *)&qt.w,0.01f, -1.0, 1.0, "%.2f",1.f)) quatChanged=true;
        ImGui::PopItemWidth();

        vec3 &ligh = theWnd->getParticlesSystem()->getRenderMode()==RENDER_USE_POINTS ? theWnd->getParticlesSystem()->shaderPointClass::getUData().lightDir : theWnd->getParticlesSystem()->shaderBillboardClass::getUData().lightDir;
        glm::vec3 lL(-ligh);
        if(ImGui::gizmo3D("##aaa", qt, lL, sz))  { 
            ligh = -lL;
            theWnd->getParticlesSystem()->getTMat()->getTrackball().setRotation(qt);
        }

        ImGui::PushItemWidth(sz);
        style.Colors[ImGuiCol_Text].x = style.Colors[ImGuiCol_Text].y = 1.0, style.Colors[ImGuiCol_Text].z =0.f;
        ImGui::DragFloat3("##uL3",glm::value_ptr(ligh),0.01f);
        style.Colors[ImGuiCol_Text] = oldTex;
        ImGui::PopItemWidth();

    }
    ImGui::End();

    style.WindowBorderSize = prevWindowBorderSize;
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();


}

void dataDlgClass::view()
{

    if(!isVisible) return;

    ImGui::SetNextWindowSize(ImVec2(300, 440), ImGuiCond_FirstUseEver);

    if(ImGui::Begin(getTitle(), &isVisible)) {
    
        const float w = ImGui::GetContentRegionAvailWidth();
        const float border = DLG_BORDER_SIZE;
        const float wButt = ImGui::GetContentRegionAvailWidth()-border*2;

        if(ImGui::Button("Load Data", ImVec2(wButt,0))) loadObjFile();

        if(ImGui::Button("Save CFG", ImVec2(wButt/2,0))) saveSettingsFile();
        ImGui::SameLine();
        if(ImGui::Button("Load CFG", ImVec2(wButt/2,0))) loadSettingsFile();

        ImGui::NewLine();
        ImGui::Text("Work in progress...");


    }
    ImGui::End();

}

void aboutDlgClass::view()
{

    if(!isVisible) return;

    ImGui::SetNextWindowSize(ImVec2(300, 440), ImGuiCond_FirstUseEver);

    if(ImGui::Begin(getTitle(), &isVisible)) {
    
        const float w = ImGui::GetContentRegionAvailWidth();
        const float border = DLG_BORDER_SIZE;
        const float wButt = ImGui::GetContentRegionAvailWidth()-border*2;

        ImGui::SetCursorPosX(4*theDlg.getFontSize()*theDlg.getFontZoom()*.5);
        ImGui::TextUnformatted(GLAPP_HELP_ABOUT);

        ImGui::NewLine();

        ImGui::Text("Mouse controls and shortcut keys");
        
        ImGui::SetCursorPosX(4*theDlg.getFontSize()*theDlg.getFontZoom()*.5);
        ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
        ImGui::TextUnformatted(GLAPP_HELP_MOUSE_KEY);
        ImGui::PopStyleColor();


        ImGui::NewLine();


    }
    ImGui::End();

}


#define GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX          0x9047
#define GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX    0x9048
#define GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX  0x9049
#define GPU_MEMORY_INFO_EVICTION_COUNT_NVX            0x904A
#define GPU_MEMORY_INFO_EVICTED_MEMORY_NVX            0x904B

#define VBO_FREE_MEMORY_ATI                     0x87FB
#define TEXTURE_FREE_MEMORY_ATI                 0x87FC
#define RENDERBUFFER_FREE_MEMORY_ATI            0x87FD


void infoDlgClass::view()
{

    if(!isVisible) return;

    ImGui::SetNextWindowSize(ImVec2(400, ImGui::GetFrameHeightWithSpacing()*15), ImGuiCond_FirstUseEver);

    //char s[64];
    //sprintf(s, "Info: %.1f FPS##zza", ImGui::GetIO().Framerate);
    bool visible = isVisible && theDlg.baseDlgClass::visible();
    if(ImGui::Begin(getTitle(), &visible)) { 

        //isVisible = visible;
    

        const GLubyte *vendor = glGetString(GL_VENDOR);
        const GLubyte *render = glGetString(GL_RENDERER);
        const GLubyte *glVer  = glGetString(GL_VERSION);
        const GLubyte *glslVer= glGetString(GL_SHADING_LANGUAGE_VERSION);

        ImGui::TextDisabled("Timings");
        ImGui::Text("Avg %.3f ms/f (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::NewLine();

        ImGui::TextDisabled("Vendor : "); ImGui::SameLine();
        ImGui::TextUnformatted((const char *)vendor);

        ImGui::TextDisabled("Render : "); ImGui::SameLine();
        ImGui::TextUnformatted((const char *)render);

        ImGui::TextDisabled("glVer  : "); ImGui::SameLine();
        ImGui::TextUnformatted((const char *)glVer);

        ImGui::TextDisabled("glslVer: "); ImGui::SameLine();
        ImGui::TextUnformatted((const char *)glslVer);


        ImGui::NewLine();
        ImGui::TextDisabled("Memory info");

        auto strNIcmp = [] (const char *a, const char *b, int sz) -> int {
          int ca, cb;
          do {
             ca = (unsigned char) *a++;
             cb = (unsigned char) *b++;
             ca = tolower(toupper(ca));
             cb = tolower(toupper(cb));
           } while (ca == cb && ca != '\0' && --sz>0);
           return ca - cb;
        };

        if(!strNIcmp((const char *)vendor, "NVIDIA", 6)) {
            GLint retVal[4];
            glGetIntegerv(GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX        , &retVal[0]);
            glGetIntegerv(GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX  , &retVal[1]);
            glGetIntegerv(GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &retVal[2]);

            char str[256];
            sprintf(str, "Dedicate VidMem    : %dK\n"\
                         "Max Tot availbl Mem: %dK\n"\
                         "Curr availbl vidMem: %dK\n"
                         ,retVal[0], retVal[1], retVal[2]);

            ImGui::TextUnformatted(str);

        } else if(strNIcmp((const char *)vendor, "Intel", 5)) { // Not Intel -> ergo AMD

            GLint retVal[4];
            char str[256];
            glGetIntegerv(VBO_FREE_MEMORY_ATI, retVal);
            sprintf(str, "Tot freeMem in pool: %dK\n"\
                            "Largst avail freeBlk:%dK\n"\
                            "Tot aux freeMem:     %dK\n"\
                            "Largst aux freeBlk:  %dK\n"
                            ,retVal[0], retVal[1], retVal[2], retVal[3]);

            ImGui::TextUnformatted(str);
        }


        ImGui::TextDisabled("PointSize");
        {
            GLfloat retVal[4];
            glGetFloatv(GL_POINT_SIZE_RANGE, &retVal[0]);
            glGetFloatv(GL_POINT_SIZE_GRANULARITY, &retVal[2]);
                char str[256];
                sprintf(str, "Range      : [%.4f, %.4f]\n"\
                             "Granularity:  %.4f"
                             ,retVal[0], retVal[1], retVal[2]);

                ImGui::TextUnformatted(str);
        }

        ImGui::NewLine();
        if(ImGui::Button(" - show metrics - ")) metricW ^=1;   ImGui::SameLine();

        if(metricW) ImGui::ShowMetricsWindow(&metricW);

            
    }
    ImGui::End();

}


//void colorTheme5c(const ImVec4 &color);
//void colorTheme3(const ImVec4 &color);
void mainImGuiDlgClass::view()
{
    ImGuiStyle& style = ImGui::GetStyle();

    const float wndSizeX = fontSize * fontZoom * 12.f; // 26 char * .5 (fontsize/2);

    ImGui::SetNextWindowSize(ImVec2(wndSizeX, ImGui::GetFrameHeightWithSpacing()*9), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(300, 0), ImGuiCond_FirstUseEver);


        
/*
    static ImGuiID ID = 0;
    if(theApp->fullScreen()) {
        ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
    } 
    else {
        //ImGui::SetNextWindowViewport(ImGui::GetMainViewport()->ID);
    }
*/

    if(ImGui::Begin(getTitle(),  NULL ,ImGuiWindowFlags_NoResize)) {

        const float w = ImGui::GetContentRegionAvailWidth();
        const float border = DLG_BORDER_SIZE;
        const float wButt = w;

        //ImGui::SetCursorPosX(border);

        {
            const bool b = aboutDlg.visible();
            if(colCheckButton(b, b ? ICON_FA_CHECK " Help/About (F1)"  : "   Help/About (F1)", wButt)) {
                aboutDlg.visible(b^true);
            }
        }
        {
            const bool b = attractorDlg.visible();
            if(colCheckButton(b, b ? ICON_FA_CHECK " Attractors (F2)"  : "   Attractors (F2)", wButt)) {
                attractorDlg.visible(b^true);
            }
        }

        {
            const bool b = particlesDlg.visible();
            if(colCheckButton(b, b ? ICON_FA_CHECK " Particles  (F3)"  : "   Particles  (F3)", wButt)) {
                particlesDlg.visible(b^true);
            }
        }

        {
            const bool b = imGuIZMODlg.visible();
            if(colCheckButton(b, b ? ICON_FA_CHECK " gui GIZMO  (F4)"  : "   gui GIZMO  (F4)", wButt)) {
                imGuIZMODlg.visible(b^true);
            }
        }

        {
            const bool b = viewSettingDlg.visible();
            if(colCheckButton(b, b ? ICON_FA_CHECK " View tools (F5)"  : "   View tools (F5)", wButt)) {
                viewSettingDlg.visible(b^true);
            }
        }

        {
            const bool b = infoDlg.visible();
            if(colCheckButton(b, b ? ICON_FA_CHECK "    Info    (F6)"  : "      Info    (F6)", wButt)) {
                infoDlg.visible(b^true);
            }
        }

        {
            const bool b = dataDlg.visible();
            if(colCheckButton(b, b ? ICON_FA_CHECK "    Data    (F7)"  : "      Data    (F7)", wButt)) {
                dataDlg.visible(b^true);
            }
        }

        {
            const bool b = progSettingDlg.visible();
            if(colCheckButton(b, b ? ICON_FA_CHECK "  Settings  (F8)"  : "    Settings  (F8)", wButt)) {
                progSettingDlg.visible(b^true);
            }
        }
        //if(ImGui::Button("Attractor(s)",  ImVec2(wButt,0))) attractorDlg.visible(true);
        //if(ImGui::Button("Particles",     ImVec2(wButt,0))) particlesDlg.visible(true); 

        //static int isViewOpen = 0;

        //if(ImGui::Button("View", ImVec2(wButt,0))) viewWindowSetting.visible(true);
    }

    ImGui::End();

//  statistics
///////////////////////////////////////////
    //static bool metricW = true;
    //ImGui::ShowMetricsWindow(&metricW);

}



void baseDlgClass::rePosWndByMode(int x, int y) 
{ 
    ImGuiWindow *wnd = ImGui::FindWindowByName(getTitle());
    ImVec2 pos(0,0);
    if(wnd) pos = wnd->Pos;
    ImGui::SetWindowPos(getTitle(), ImVec2(pos.x+x,pos.y+y)); 

}


void mainImGuiDlgClass::switchMode(int x, int y)
{
    aboutDlg.rePosWndByMode(x, y);
    attractorDlg.rePosWndByMode(x, y);
    particlesDlg.rePosWndByMode(x, y);
    bbPaletteDlg.rePosWndByMode(x, y);
    psPaletteDlg.rePosWndByMode(x, y);
    viewSettingDlg.rePosWndByMode(x, y);
    progSettingDlg.rePosWndByMode(x, y);
    infoDlg.rePosWndByMode(x, y);
    dataDlg.rePosWndByMode(x, y);
    rePosWndByMode(x, y);
}


void mainImGuiDlgClass::renderImGui()
{

/*
    if(fontChanged) {
        //delete testFont;
        fontCFG.RasterizerMultiply = RasterizerMultiply;
        fontCFG.FontDataOwnedByAtlas=false;
        //    theDlg.testFont = ImGui::GetIO().Fonts->AddFontFromFileTTF("Fonts/Cousine-Regular.ttf", theDlg.getFontSize(), &theDlg.fontCFG);

        //testFont = ImGui::GetIO().Fonts->AddFontFromFileTTF("Fonts/Cousine-Regular.ttf", getFontSize(), &fontCFG);
        //theDlg.testFont = ImGui::GetIO().Fonts->AddFontFromFileTTF("Fonts/Cousine-Regular.ttf", theDlg.getFontSize(), &theDlg.fontCFG);
        theDlg.mainFont->ConfigData->RasterizerMultiply = RasterizerMultiply;
        ImGui_ImplOpenGL3_DestroyFontsTexture();
        ImGui_ImplOpenGL3_CreateFontsTexture();
        //ImGui::GetIO().Fonts->Fonts[0];
        fontChanged = false;
    }
*/



    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    //if(theApp->fullScreen()) ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_ViewportsEnable; 
    //else                     ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; 


    ImGui::NewFrame();


#ifdef GLAPP_IMGUI_VIEWPORT

/*
    static bool prevMode = false;
    bool switchedFullScreen = prevMode != theApp->fullScreen();

    prevMode=theApp->fullScreen();

    if(switchedFullScreen) {
        if(theApp->fullScreen()) switchFullscreen();
        else                     switchWindow();
    }
*/
#endif

    if(visible()) {
        aboutDlg.view();
        attractorDlg.view();
        particlesDlg.view();
        if(particlesDlg.visible()) {
            bbPaletteDlg.view();
            psPaletteDlg.view();
        }
        imGuIZMODlg.view();
        viewSettingDlg.view();
        progSettingDlg.view();
        infoDlg.view();
        dataDlg.view();
        view();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


#ifdef GLAPP_IMGUI_VIEWPORT
    // Update and Render additional Platform Windows
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
#endif

}

#endif