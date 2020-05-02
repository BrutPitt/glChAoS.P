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
#include <sstream>
#include "../glApp.h"
#include "../glWindow.h"

#include <imguiControls.h>
#include <imGuIZMOquat.h>

#include "uiHelpEng.h"

extern HLSTexture hlsTexture;

int ShowStyleSelector(const float w);
void selectTheme(int style_idx);

bool loadObjFile();

void saveSettingsFile();
void loadSettingsFile(bool isImportConfig=false);



bool show_test_window = true;
bool show_another_window = false;
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

// Helper to display a little (?) mark which shows a tooltip when hovered.
static void ShowHelpMarker(const char* desc, bool disabled = true)
{
    if(disabled) ImGui::TextDisabled("  " ICON_FA_COMMENT_O);
    else         ImGui::Text("  " ICON_FA_COMMENT_O);
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
    
    if(!visible()) return;

    particlesBaseClass *particles;
    int id;
    
#ifdef GLCHAOSP_LIGHTVER
    id = 'P';
    particles = (particlesBaseClass *) theWnd->getParticlesSystem()->shaderPointClass::getPtr();
    const int hSz = 0;
#else
    if(theWnd->getParticlesSystem()->getRenderMode() == particlsRenderMethod::RENDER_USE_BILLBOARD) {
        particles = (particlesBaseClass *) theWnd->getParticlesSystem()->shaderBillboardClass::getPtr();
        id = 'B';
    } else {
        particles = (particlesBaseClass *) theWnd->getParticlesSystem()->shaderPointClass::getPtr();
        id = 'P';
    }

    const int hSz = -ImGui::GetFrameHeightWithSpacing();
#endif
    //static bool colorMapDlg=false;
    //if(ImGui::Button("ColorMaps")) colorMapDlg=true;

    ImGui::SetNextWindowSize(ImVec2(300, 440), ImGuiCond_FirstUseEver);

    if(ImGui::Begin(getTitle(), (bool *) &isVisible, ImGuiTreeNodeFlags_DefaultOpen)) {
        const int border = DLG_BORDER_SIZE;
        ImGui::BeginChild("cm",ImVec2(0, hSz) ); { // Leave room for 1 line below us                            
            const float w = ImGui::GetContentRegionAvail().x; //4/3
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
#if !defined(GLCHAOSP_LIGHTVER)
        const float w = ImGui::GetContentRegionAvail().x; //4/3
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
#endif
    } ImGui::End();

            //ImGui::OK
}

void pushColorButton()
{
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImGui::GetColorU32(ImGuiCol_PlotHistogramHovered)));
    ImGui::PushStyleColor(ImGuiCol_Button,        (ImGui::GetColorU32(ImGuiCol_PlotHistogram)));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  (ImGui::GetColorU32(ImGuiCol_CheckMark)));
}

void popColorButton()
{
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
}
//////////////////////////////////////////////////////////////////
bool colCheckButton(bool b, const char *s, const float sz=0)
{
    if(b) pushColorButton();
    const bool ret = ImGui::Button(s,  ImVec2(sz,0));
    if(b) popColorButton();
    return ret;
}

//////////////////////////////////////////////////////////////////
void particlesDlgClass::viewSettings(particlesBaseClass *particles, char id) 
{
    const float wPaddingY = ImGui::GetStyle().WindowPadding.y;

    const float border = DLG_BORDER_SIZE;
    char base[]="##aa_";
    char idA='a';

    const ImU32 titleCol = ImGui::GetColorU32(ImGuiCol_PlotLines); 

    const float w = ImGui::GetContentRegionAvail().x;
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
    ImGuiIO& io = ImGui::GetIO();                
    const float fontSize = io.Fonts->Fonts[0]->FontSize;


//    auto modCheckBox = [&] (bool b, float sz, const char *str=nullptr) {
//        ImGui::PushStyleColor(ImGuiCol_Button,ImGui::GetColorU32(ImGuiCol_ChildBg));
//        char txt[32];
//        if(str) sprintf(txt, b ? "%s " ICON_FA_CHECK_SQUARE_O "%s" : "%s " ICON_FA_SQUARE_O "%s", str, buildID(base, idA++, id));
//        else        sprintf(txt, b ? ICON_FA_CHECK_SQUARE_O "%s" : ICON_FA_SQUARE_O "%s", buildID(base, idA++, id));
//
//        const bool ret = sz ? ImGui::Button(txt,ImVec2(sz,0)) : ImGui::Button(txt); 
//        ImGui::PopStyleColor();
//        return ret;
//    };

    // Rendering settings
    ////////////////////////////////////
    {
        ImGui::PushStyleColor(ImGuiCol_Text, titleCol);
        const bool isOpen = ImGui::TreeNodeEx(buildID(base, idA++, id),ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoTreePushOnOpen,ICON_FA_CUBES " Rendering"); 
        //const bool isOpen = ImGui::TreeNode((std::string(ICON_FA_CUBES " Rendering")+buildID(base, idA++, id)).c_str(),);
        //const bool isOpen = ImGui::TreeNodeEx(buildID(base, idA++, id),ImGuiTreeNodeFlags_DefaultOpen,ICON_FA_CUBES " Rendering"); 
        ImGui::PopStyleColor();

        if(isOpen) {
#if defined(GLCHAOSP_LIGHTVER) && !defined(GLCHAOSP_LIGHTVER_EXPERIMENTAL)
            const int numItems = 7;
#else
            const int numItems = 8;
#endif


            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
            ImGui::BeginChild(buildID(base, idA++, id), ImVec2(0,ImGui::GetFrameHeightWithSpacing()*numItems-ImGui::GetStyle().ItemSpacing.y*3), true); 
                //////////Linea 1//////////
                ImGui::SetCursorPosX(ImGui::GetFrameHeightWithSpacing());
                const float nextPos = ImGui::GetContentRegionAvail().x*.5;
                ImGui::TextDisabled(" Blending Src"); 
                ImGui::SameLine(ImGui::GetFrameHeightWithSpacing()+nextPos);     ImGui::TextDisabled(" Blending Dst");
        
                //////////Linea 2//////////
                vec4 bkgColor(particles->backgroundColor());
                if(ImGui::ColorEdit4(buildID(base, idA++, id),value_ptr(bkgColor),ImGuiColorEditFlags_NoInputs)) particles->backgroundColor(bkgColor); 

                ImGui::SameLine();
                ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x*.5);
                    {
                        int idx = particles->srcBlendIdx();
                        if(ImGui::Combo(buildID(base, idA++, id), &idx, particles->getBlendArrayStrings().data(), particles->getBlendArrayElements())) {
                            particles->setSrcBlend(particles->getBlendArray()[idx]);
                            particles->srcBlendIdx(idx);
                        }
                    }
                    ImGui::SameLine();
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

                {
                    const float sz = wButt3-(ImGui::GetFrameHeightWithSpacing()+border)*.3333;
                    char txt[32];
                    ImGui::SetCursorPosX(posA);
                    {
                        bool b = particles->getBlendState();
                        sprintf(txt, b ? "Blend " ICON_FA_CHECK_SQUARE_O "%s" : "Blend " ICON_FA_SQUARE_O "%s", buildID(base, idA++, id)); 
                        if(colCheckButton(b , txt, sz)) particles->setBlendState(b^1);
                    }
                    ImGui::SameLine();
                    {
                        bool b = particles->getDepthState();
                        sprintf(txt, b ? "Depth " ICON_FA_CHECK_SQUARE_O "%s" : "Depth " ICON_FA_SQUARE_O "%s", buildID(base, idA++, id)); 
                        if(colCheckButton(b , txt, sz)) particles->setDepthState(b^1);
                    }

                    ImGui::SameLine();
                    {
                        bool b = particles->getLightState();
                        sprintf(txt, b ? "Light " ICON_FA_CHECK_SQUARE_O "%s" : "Light " ICON_FA_SQUARE_O "%s", buildID(base, idA++, id)); 
                        if(colCheckButton(b , txt, sz)) particles->setLightState(b^1);
                    }
                    ImGui::SameLine();
                    {
                        bool b = theDlg.particleEditDlg.visible();
                        sprintf(txt, b ? ICON_FA_CIRCLE "%s" : ICON_FA_CIRCLE "%s", buildID(base, idA++, id)); 
                        if(colCheckButton(b , txt, ImGui::GetFrameHeightWithSpacing())) theDlg.particleEditDlg.visible(b^1);
                    }
                }

                ImGui::PushItemWidth(wButt3);

                    //////////Linea 5//////////        
                    ImGui::SetCursorPosX(INDENT(posA)); ImGui::TextDisabled("pointSize"); //ImGui::SameLine(posB3);
                    ImGui::SameLine(INDENT(posB3));     ImGui::TextDisabled("distAtten");                   
                    ImGui::SameLine(INDENT(posC3));     ImGui::TextDisabled("clipDist");  

                    //////////Linea 6//////////        
                    {
                        ImGui::SetCursorPosX(posA);
                        float f = particles->getSize();                    
                        if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.01, 0.01, 5000.0, "%.3f",DRAGFLOAT_POW_2,ImVec2(.93,0.5))) particles->setSize(f);
                    }
                    {
                        ImGui::SameLine(posB3);
                        float f = particles->getPointSizeFactor();                    
                        if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.01, 0.0, 1000.0,  "%.3f",DRAGFLOAT_POW_3,ImVec2(.93,0.5))) particles->setPointSizeFactor(f);
                    }
                    {
                        ImGui::SameLine(posC3);                    
                        float f = particles->getClippingDist();                   
                        if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.01, 0.001, 1000.0, "%.3f",1.0f,ImVec2(.93,0.5))) particles->setClippingDist(f);
                    }
                    ImGui::SetCursorPosX(INDENT(posA)); ImGui::TextDisabled("alphaK");     
                    ImGui::SameLine(INDENT(posB3));     ImGui::TextDisabled("alphaAtten"); //ImGui::SameLine(); ImGui::TextDisabled(" ?");
                    ImGui::SameLine(INDENT(posC3));     ImGui::TextDisabled("alphaSkip");

                    {
                        ImGui::SetCursorPosX(posA);
                        float f = particles->getAlphaKFactor();                   
                        if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.01, 0.01, 100.0, "%.3f",1.0f,ImVec2(.93,0.5))) particles->setAlphaKFactor(f);
                    }
                    {
                        ImGui::SameLine(posB3);
                        float f = particles->getAlphaAtten();                   
                        if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.01,0.0, 1000.0, "%.3f",DRAGFLOAT_POW_3,ImVec2(.93,0.5))) particles->setAlphaAtten(f);
                    }
                    {
                        ImGui::SameLine(posC3);
                        float f = particles->getAlphaSkip();
                        if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.001, 0.0,  1.0, "%.3f",1.0f,ImVec2(.93,0.5))) particles->setAlphaSkip(f);
                    }
                ImGui::PopItemWidth();
#if !defined(GLCHAOSP_LIGHTVER) || defined(GLCHAOSP_LIGHTVER_EXPERIMENTAL)
                if(!particles->getBlendState() && (particles->getDepthState() || particles->getLightState())) {

                    ImGui::SetCursorPosX(posA);
                    {
                        char txt[32];
                        bool b = particles->postRenderingActive();
                        sprintf(txt, b ? "DualPass " ICON_FA_CHECK_SQUARE_O "%s" : "DualPass " ICON_FA_SQUARE_O "%s", buildID(base, idA++, id)); 
                        if(colCheckButton(b , txt, wButt3)) particles->postRenderingActive(b^1);
                    }

                    if(particles->postRenderingActive()) {
                        ImGui::PushItemWidth(wButt3);
                        ImGui::SameLine();
                        {
                            float f = particles->dpAdjConvex();
                            if(ImGui::DragFloat(buildID(base, idA++, id), &f, .001, .01, 3.0, "Cnvx: %.3f",1.f)) particles->dpAdjConvex(f);
                        }

                        ImGui::SameLine();
                        {
                            float f = particles->dpNormalTune();
                            if(ImGui::DragFloat(buildID(base, idA++, id), &f, .00025, 0.0, 5.0, "Norm: %.4f",1.f)) particles->dpNormalTune(f);
                        }
                        ImGui::PopItemWidth();
                    }
                }
#endif
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
            ImGuiStyle& style = ImGui::GetStyle();
            const float buttY = style.FramePadding.y*2+(fontSize * io.FontGlobalScale);

            const float w = ImGui::GetContentRegionAvail().x-border*2;

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
                    if(ImGui::Checkbox(s, &b)) 
                        cmSet->setReverse(b);
                }

                //////////Linea 2 //////////
                
                // PushButton Colormap
                ////////////////////////////////////
                ImGui::SameLine(posB4);
                //if(ImGui::colormapButton("pippo", ImVec2(w-10,12), 256, particles->getSelectedColorMap_pf3()))
                if(ImGui::ImageButton(reinterpret_cast<ImTextureID>((int64) cmSet->getOrigTex()), ImVec2((wButt4)*3-border,fontSize * ImGui::GetIO().FontGlobalScale))) {
                //if(ImGui::Button("paletteAAA", ImVec2((wButt4)*3-border,fontSize * ImGui::GetIO().FontGlobalScale))) {
                    theDlg.paletteDlg.visible( theDlg.paletteDlg.visible()^1);
                }
               

                //ImGui::colormapButton("pluto", ImVec2(300,16), 256, particles->getColorMap().getRGB_pf3(1));
                
                ////////// Linea 3 e 4  - text e widgets //////////

                // Vel / start 
                ////////////////////////////////////
                //const float wButt3Half = (wButt4-border)*.5;
                //const float pos3H = wButt3Half + border*2;
                
                ImGui::SetCursorPosX(INDENT(posA )); ImGui::TextDisabled("Range");
                ImGui::SameLine(     INDENT(posB4)); ImGui::TextDisabled("Offset");
                ImGui::SameLine(     INDENT(posC4)); ImGui::TextDisabled("Intens.");
                ImGui::SameLine(     INDENT(posD4)); ImGui::TextDisabled("colorVel"); 
                
                ImGui::PushItemWidth(wButt4);
                {
                    ImGui::SetCursorPosX(posA);
                    float f = cmSet->getRange(); 
                    if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.0025, 0.0, 200.0, "% .3f",1.0f,ImVec2(.93,0.5))) cmSet->setRange(f);
                }
                {
                    ImGui::SameLine(posB4);
                    float f = cmSet->getOffsetPoint();
                    if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.0025, -100.0, 100.0, "% .3f",1.0f,ImVec2(.93,0.5))) cmSet->setOffsetPoint(f);
                }
                {
                    ImGui::SameLine(posC4);
                    float f = particles->getColIntensity();
                    if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.0025, 0.0, 10.0, "% .3f",1.0f,ImVec2(.93,0.5))) particles->setColIntensity(f);
                }

                {
                    ImGui::SameLine(posD4);
                    float f = cmSet->getVelIntensity();
                    if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.001, 0.001, 10000.0, "% .3f",1.0f,ImVec2(.93,0.5))) cmSet->setVelIntensity(f);
                }
                ImGui::PopItemWidth();
                
    
                // Image Palette
                ////////////////////////////////////
                ImGui::SetCursorPosX(border);
                ImGui::Image(reinterpret_cast<ImTextureID>((int64) cmSet->getModfTex()), ImVec2(w,buttY));


                // HLSL Controls
                ////////////////////////////////////
                ImGui::PushItemWidth(wButt3);        
                {
                    ImGui::SetCursorPosX(border);
                    float f = cmSet->getH();                    
                    //if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.001, -1.0, 1.0, "%.3f",1.0f,ImVec2(.93,0.5))) particles->getCMTex()->setH(f);
                    if(ImGui::hslTuning(buildID(base, idA++, id), &f, HSL_TUNING_H, 0.0025f, ImVec2(wButt3, buttY)))  cmSet->setH(f);
                    //ImGui::SameLine(border+5); ImGui::Text("%.2f",f);
                }
                {
                    ImGui::SameLine(posB3);
                    float f = cmSet->getS();
                    if(ImGui::hslTuning(buildID(base, idA++, id), &f, HSL_TUNING_S, 0.0025f, ImVec2(wButt3, buttY),ImVec2(3, 3))) cmSet->setS(f);
                }
                {
                    ImGui::SameLine(posC3);
                    float f = cmSet->getL(); 
                    if(ImGui::hslTuning(buildID(base, idA++, id), &f, HSL_TUNING_L, 0.0025f, ImVec2(wButt3, buttY),ImVec2(3, 3))) cmSet->setL(f);
                }
                ImGui::PopItemWidth();
                
                ImGui::SetCursorPosX(INDENT(border)); ImGui::TextDisabled("Hue"); 
                ImGui::SameLine(INDENT(posB3));       ImGui::TextDisabled("Saturation"); 
                ImGui::SameLine(INDENT(posC3));       ImGui::TextDisabled("Lightness"); 

            ImGui::EndChild();
            ImGui::PopStyleVar();
        }
    }
#if !defined(GLCHAOSP_NO_DeL)
    // Light settings
    ////////////////////////////////////
    {
        ImGui::PushStyleColor(ImGuiCol_Text, titleCol);
        const bool isOpen = ImGui::TreeNodeEx(buildID(base, idA++, id),ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoTreePushOnOpen,ICON_FA_LIGHTBULB_O " Light   " ICON_FA_COMMENT_O);
        ImGui::PopStyleColor();
        ShowHelpOnTitle(GLAPP_HELP_LIGHT_TREE);
        if(isOpen) {
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
            ImGui::BeginChild(buildID(base, idA++, id), ImVec2(0,ImGui::GetFrameHeightWithSpacing()*4+ImGui::GetStyle().ItemSpacing.y), true);

                //ImGui::Columns(2);               

                const float wGizmo = ImGui::GetFrameHeightWithSpacing()*3 - ImGui::GetStyle().ItemSpacing.y;
                const float wLight = w - wGizmo - border;
                const float wLightHalf = wLight*.5;
                const float wLightButt = (wLightHalf-border*1.5);
                const float wLightPos = posA + wLightButt + border;
                const float sz = wLightButt;

                const float posA = border;

                //ImGui::PushItemWidth(wButt4);
                //ImGui::AlignTextToFramePadding();
                vec3 lCol = particles->getLightColor();
                vec3 vL(-particles->getLightDir());

                ImGui::SetCursorPosX(posA); 
                if(ImGui::ColorEdit3(buildID(base, idA++, id),value_ptr(lCol),ImGuiColorEditFlags_NoInputs)) particles->setLightColor(lCol); 
                ImGui::SameLine(); 
                ImGui::TextDisabled(" Model"); ImGui::SameLine();
                ImGui::PushItemWidth(wLightButt*1.67 - (ImGui::GetCursorPosX()-posA));
                {
                    int idx = particles->getLightModel();
                    if (ImGui::Combo(buildID(base, idA++, id), &idx, "Phong\0"\
                                                                     "Blinn-Phong\0"\
                                                                     "GGX\0"))
                        {  particles->setLightModel(idx); }
                }
                ImGui::PopItemWidth();
                ImGui::SameLine(); 
                ImGui::TextDisabled("Dist"); ImGui::SameLine();
                ImGui::PushItemWidth(w - (ImGui::GetCursorPosX() +border));
                float dist = length(vL);
                if(ImGui::DragFloatEx(buildID(base, idA++, id), &dist ,0.01, 0.01f, 10000.f, "% .3f",1.0f,ImVec2(.93,0.5))) {
                    particles->setLightDir(normalize(particles->getLightDir()) * dist);
                }

                ImGui::AlignTextToFramePadding();

                ImGui::SetCursorPosX(posA); ImGui::TextDisabled("Diff"); ImGui::SameLine();
                ImGui::PushItemWidth(sz - (ImGui::GetCursorPosX()-posA));
                ImGui::DragFloatEx(buildID(base, idA++, id),&particles->getUData().lightDiffInt,0.001, 0.0, 1000.f, "% .3f",1.0f,ImVec2(.93,0.5));
                ImGui::PopItemWidth();

                ImGui::SameLine(wLightPos);     ImGui::TextDisabled("Ambt"); ImGui::SameLine();
                ImGui::PushItemWidth(sz - (ImGui::GetCursorPosX()-wLightPos));
                ImGui::DragFloatEx(buildID(base, idA++, id),&particles->getUData().lightAmbInt, 0.001, -1.0, 2.f, "% .3f",1.0f,ImVec2(.93,0.5));
                ImGui::PopItemWidth();
                
                ImVec2 pos(ImGui::GetCursorPos());

                ImGui::SameLine(wLight);
                
                imguiGizmo::setDirectionColor(ImVec4(.4f+lCol.x*.6f, .4f+lCol.y*.6f, .4f+lCol.z*.6f, 1.0));
                if(ImGui::gizmo3D("##Bht", vL ,wGizmo))  particles->setLightDir(-vL);
                imguiGizmo::restoreDirectionColor();

                ImGui::SetCursorPos(pos);

                ImGui::AlignTextToFramePadding();
                
                int idxGGX = particles->modelGGX-particles->modelOffset;
                const float wSZ = particles->getLightModel()==idxGGX ? sz*.8f : sz;

                ImGui::SetCursorPosX(posA); ImGui::TextDisabled("Spec");  ImGui::SameLine();
                ImGui::PushItemWidth(wSZ - (ImGui::GetCursorPosX()-posA));
                ImGui::DragFloatEx(buildID(base, idA++, id),&particles->getUData().lightSpecInt,0.001, 0.0, 1000.f, "% .3f",1.0f,ImVec2(.93,0.5));
                ImGui::PopItemWidth();
                
                if(particles->getLightModel()==idxGGX) {
                    ImGui::SameLine();     
                    const float sPos = ImGui::GetCursorPosX();
                    ImGui::TextDisabled("F");  ImGui::SameLine();
                    const float ePos = ImGui::GetCursorPosX();
                    const float buttGGX = (wLight - sPos)*.5 - border - (ePos-sPos);
                    ImGui::PushItemWidth(buttGGX);
                    ImGui::DragFloatEx(buildID(base, idA++, id),&particles->getUData().ggxFresnel, 0.001, .001, 1.f, "%.3f", DRAGFLOAT_POW_3,ImVec2(.93,0.5));
                    ImGui::PopItemWidth();

                    ImGui::SameLine();     ImGui::TextDisabled("R");  ImGui::SameLine();
                    ImGui::PushItemWidth(buttGGX);
                    ImGui::DragFloatEx(buildID(base, idA++, id),&particles->getUData().ggxRoughness, 0.001, .001, 1.f, "%.3f", DRAGFLOAT_POW_3,ImVec2(.93,0.5));
                    ImGui::PopItemWidth();

                } else {
                    ImGui::SameLine(wLightPos);     ImGui::TextDisabled("sExp");  ImGui::SameLine();
                    ImGui::PushItemWidth(sz - (ImGui::GetCursorPosX()-wLightPos));
                    ImGui::DragFloatEx(buildID(base, idA++, id),&particles->getUData().lightShinExp, 0.001, .1, 1000.f, "%.3f", DRAGFLOAT_POW_3,ImVec2(.93,0.5));
                    ImGui::PopItemWidth();
                }

                ImGui::AlignTextToFramePadding();

                ImGui::SetCursorPosX(posA); 
                ImGui::TextDisabled("Modulate"); ImGui::SameLine();
                ImGui::PushItemWidth((wLightButt+border)*2 - (ImGui::GetCursorPosX()));
                ImGui::DragFloatRange2(buildID(base, idA++, id), &particles->getUData().sstepColorMin, &particles->getUData().sstepColorMax, .001, -1.0, 2.0);
                ImGui::PopItemWidth();


            ImGui::EndChild();
            ImGui::PopStyleVar();
        }
    }
#endif

#if !defined(GLCHAOSP_LIGHTVER) || defined(GLCHAOSP_LIGHTVER_EXPERIMENTAL)
    if(!particles->getBlendState() && (particles->getDepthState() || particles->getLightState())) {
// Shadow settings
////////////////////////////////////
        {
            ImGui::PushStyleColor(ImGuiCol_Text, titleCol);
            const bool isOpen = ImGui::TreeNodeEx(buildID(base, idA++, id),ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoTreePushOnOpen,ICON_FA_SIGN_LANGUAGE  " Shadows   " ICON_FA_COMMENT_O);
            ImGui::PopStyleColor();
            ShowHelpOnTitle(GLAPP_HELP_SHADOWS_TREE);
            if(isOpen) {
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
                ImGui::BeginChild(buildID(base, idA++, id), ImVec2(0,ImGui::GetFrameHeightWithSpacing()*2+ImGui::GetStyle().ItemSpacing.y), true);

                ImGui::SetCursorPosX(posA); 
                {
                    bool b = particles->useShadow();
                    if(ImGui::Checkbox(buildID(base, idA++, id), &b)) particles->useShadow(b);
                }

                //if(particles->useShadow()) {
                    ImGui::SameLine();

                    ImGui::AlignTextToFramePadding();
                    ImGui::TextDisabled("Ratio");
                    ImGui::SameLine();

                    ImGui::PushItemWidth(border + (wButt2 - ImGui::GetCursorPosX()));
                    {
                        int idx = int(1.f/particles->getShadowGranularity()+.5)-1;
                        if (ImGui::Combo(buildID(base, idA++, id), &idx, "rad:1\0"\
                                                                         "rad:2\0"\
                                                                         "rad:3\0"\
                                                                         "rad:4\0"\
                                                                         "rad:5\0"))
                            {  particles->setShadowGranularity(1.f/float(idx+1)); }
                    }
                    ImGui::PopItemWidth();

                    ImGui::SameLine();

                    {
                        char txt[48];
                        bool b = particles->autoLightDist();
                        sprintf(txt, b ? "AutoLightDist " ICON_FA_CHECK_SQUARE_O "%s" : "AutoLightDist " ICON_FA_SQUARE_O "%s", buildID(base, idA++, id)); 
                        if(colCheckButton(b , txt, w - (ImGui::GetCursorPosX() +border))) particles->autoLightDist(b^1);
                    }

                    ImGui::AlignTextToFramePadding();
                    ImGui::SetCursorPosX(posA);
                    ImGui::TextDisabled("Rad");
                    ImGui::SameLine();
                    ImGui::PushItemWidth(border + (wButt3 - ImGui::GetCursorPosX()));
                    {
                        int i = int(particles->getShadowRadius());
                        if(ImGui::DragInt(buildID(base, idA++, id), &i, .1, 0, 20, "%d")) particles->setShadowRadius(i);
                    }
                    ImGui::PopItemWidth();

                    ImGui::SameLine();
                    ImGui::TextDisabled("Bias");
                    ImGui::SameLine();
                    ImGui::PushItemWidth(border + (wButt3*2+border - ImGui::GetCursorPosX()));
                    {
                        float f = particles->getShadowBias();
                        if(ImGui::DragFloatEx(buildID(base, idA++, id), &f, .001, -100.0, 100.0, "%.3f",1.f,ImVec2(.93,0.5))) particles->setShadowBias(f);
                    }
                    ImGui::PopItemWidth();

                    ImGui::SameLine();
                    ImGui::TextDisabled("Dark");
                    ImGui::SameLine();
                    ImGui::PushItemWidth(w - (ImGui::GetCursorPosX() +border));
                    {
                        float f = particles->getShadowDarkness();
                        if(ImGui::DragFloatEx(buildID(base, idA++, id), &f, .001, 0.0, 1.0, "%.3f",1.f,ImVec2(.93,0.5))) particles->setShadowDarkness(f);
                    }

                    ImGui::PopItemWidth();

                //}



                ImGui::EndChild();
                ImGui::PopStyleVar();
            }
        }
// AO settings
////////////////////////////////////
        {
            ImGui::PushStyleColor(ImGuiCol_Text, titleCol);
            const bool isOpen = ImGui::TreeNodeEx(buildID(base, idA++, id),ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoTreePushOnOpen,ICON_FA_SHIELD  " Ambient Occlusion   " ICON_FA_COMMENT_O);
            ImGui::PopStyleColor();
            ShowHelpOnTitle(GLAPP_HELP_AO_TREE);
            if(isOpen) {
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
                ImGui::BeginChild(buildID(base, idA++, id), ImVec2(0,ImGui::GetFrameHeightWithSpacing()*2+ImGui::GetStyle().ItemSpacing.y), true);

                    ImGui::SetCursorPosX(posA); 
                    {
                        bool b = particles->useAO();
                        if(ImGui::Checkbox(buildID(base, idA++, id), &b)) particles->useAO(b);
                    }

                    ImGui::SameLine();

                    ImGui::AlignTextToFramePadding();
                    ImGui::TextDisabled("Mul");
                    ImGui::SameLine();
                    ImGui::PushItemWidth(border + (wButt3 - ImGui::GetCursorPosX()));
                    {
                        float f = particles->getAOMul();
                        if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.01, 0.1, 2.0, "%.3f",1.0f,ImVec2(.93,0.5))) particles->setAOMul(f);
                    }
                    ImGui::PopItemWidth();

                    ImGui::SameLine();
                    ImGui::TextDisabled("Diff");
                    ImGui::SameLine();
                    ImGui::PushItemWidth(border + (wButt3*2+border - ImGui::GetCursorPosX()));
                    ImGui::SameLine();
                    {
                        float f = particles->getAOModulate();
                        if(ImGui::DragFloatEx(buildID(base, idA++, id), &f,.01, 0.1, 3.0, "%.3f",1.0f,ImVec2(.93,0.5))) particles->setAOModulate(f);
                    }
                    ImGui::PopItemWidth();
                    
                    ImGui::SameLine();
                    ImGui::TextDisabled("More");
                    ImGui::SameLine();
                    ImGui::PushItemWidth(w - (ImGui::GetCursorPosX() +border));
                    {
                        float f = particles->getAOStrong();
                        if(ImGui::DragFloatEx(buildID(base, idA++, id), &f, .001, 0.0, 1.0,  "%.3f",1.f,ImVec2(.93,0.5))) particles->setAOStrong(f);
                    }
                    ImGui::PopItemWidth();


                    ImGui::AlignTextToFramePadding();
                    ImGui::SetCursorPosX(posA);
                    ImGui::TextDisabled("Rad");
                    ImGui::SameLine();
                    ImGui::PushItemWidth(border + (wButt3 - ImGui::GetCursorPosX()));
                    {
                        float f = particles->getAORadius();
                        if(ImGui::DragFloatEx(buildID(base, idA++, id), &f, .01, 0.01, 20.0,  "%.2f",1.f,ImVec2(.93,0.5))) particles->setAORadius(f);
                    }
                    ImGui::PopItemWidth();

                    ImGui::SameLine();
                    ImGui::TextDisabled("Bias");
                    ImGui::SameLine();
                    ImGui::PushItemWidth(border + (wButt3*2+border - ImGui::GetCursorPosX()));
                    ImGui::SameLine();
                    {
                        float f = particles->getAOBias();
                        if(ImGui::DragFloatEx(buildID(base, idA++, id), &f, .001, 0.0, 1.0, "%.3f",1.f,ImVec2(.93,0.5))) particles->setAOBias(f);
                    }
                    ImGui::PopItemWidth();

                    ImGui::SameLine();
                    ImGui::TextDisabled("Dark");
                    ImGui::SameLine();
                    ImGui::PushItemWidth(w - (ImGui::GetCursorPosX() +border));
                    {
                        float f = particles->getAODarkness();
                        if(ImGui::DragFloatEx(buildID(base, idA++, id), &f, .0025, 0.0, 1.0,  "%.3f",1.f,ImVec2(.93,0.5))) particles->setAODarkness(f);
                    }

                    ImGui::PopItemWidth();


                ImGui::EndChild();
                ImGui::PopStyleVar();
            }
        }
    }
#endif
    // Glow settings
    ////////////////////////////////////
    {
        ImGui::PushStyleColor(ImGuiCol_Text, titleCol);
        const bool isOpen = ImGui::TreeNodeEx(buildID(base, idA++, id),ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoTreePushOnOpen,ICON_FA_LOW_VISION " Glow Effect   "  ICON_FA_COMMENT_O);
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
                    
#if !defined(GLCHAOSP_LIGHTVER)
                    {    
                        bool b = glow->isGlowOn();
                        if(ImGui::Checkbox(buildID(base, idA++, id), &b)) glow->setGlowOn(b);
                    }
                    ImGui::SameLine();

                    ImGui::PushItemWidth(wButt2 - (ImGui::GetCursorPosX()-posA));

                    {
                        int idx = mode-1;
                        if (ImGui::Combo(buildID(base, idA++, id), &idx, "Gaussian blur\0"\
                                                                          "Gaussian + bilateral\0"\
                                                                          "bilateral threshold\0"))
                            { glow->setGlowState(idx+1); }
                    }

                    ImGui::PopItemWidth();
#else 
                    {    
                        bool b = glow->isGlowOn();
                        if(ImGui::Checkbox(buildID(base, idA++, id), &b)) {
                            glow->setGlowOn(b);
                            if(glow->isGlowOn()) glow->setGlowState(BlurBaseClass::glowType_Bilateral);
                            else                 glow->setGlowState(BlurBaseClass::glowType_ByPass);
                        }
                    }
#endif

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
#if !defined(GLCHAOSP_NO_FXAA)
    // FXAA settings
    ////////////////////////////////////
    {
        ImGui::PushStyleColor(ImGuiCol_Text, titleCol);
        const bool isOpen = ImGui::TreeNodeEx(buildID(base, idA++, id),ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoTreePushOnOpen,ICON_FA_MAGIC " FXAA Filter");
        ImGui::PopStyleColor();
        if(isOpen) {
        //////////Linea 7//////////        
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
            ImGui::BeginChild(buildID(base, idA++, id), ImVec2(0,ImGui::GetFrameHeightWithSpacing()*2+wPaddingY), true);
                

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
                if (ImGui::Combo(buildID(base, idA++, id), &i, "  2\0  4\0  8\0"))
                    { fxaa->setSpan(int(1<<(i+1))); }

                ImGui::PopItemWidth();

            }
            {
                ImGui::SameLine(posC4);
                ImGui::TextDisabled("Threshold:"); ImGui::SameLine();
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
                if (ImGui::Combo(buildID(base, idA++, id), &i, " 1\0 1/2\0 1/4\0 1/8\0 1/16\0 1/32\0 1/64\0"))
                    { fxaa->setReductMul(int(1<<(i))); }

                ImGui::PopItemWidth();
            }
            {
                ImGui::SameLine(posC4);
                ImGui::TextDisabled("Min:"); ImGui::SameLine();
                ImGui::PushItemWidth(w-ImGui::GetCursorPosX()-border);            

                int i = int(log2(fxaa->getReductMin()));
                if (ImGui::Combo(buildID(base, idA++, id), &i, " 1\0 1/2\0 1/4\0 1/8\0 1/16\0 1/32\0 1/64\0 1/128\0 1/256\0 1/512\0"))
                    { fxaa->setReductMin(int(1<<(i))); }

                ImGui::PopItemWidth();
            }

                
            ImGui::EndChild();
            ImGui::PopStyleVar();
        }
    }
#endif
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
                    if(ImGui::DragFloat(buildID(base, idA++, id), &f,.001, 0.001, 20.0, "%.3f",1.0f)) 
                        imT->setGamma(f);
                }

                ImGui::SameLine(posB4);                
                {
                    float f = imT->getExposure();
                    if(ImGui::DragFloat(buildID(base, idA++, id), &f,.001, 0.001, 10.0, "%.3f",1.0f)) 
                        imT->setExposure(f);
                
                }   
                ImGui::PopItemWidth();                

                ImGui::SameLine(posC4);
                {
                    float f = imT->getBright();
                    if(ImGui::hslTuning(buildID(base, idA++, id), &f, HSL_TUNING_L, .001f, ImVec2(wButt4-2, buttY),ImVec2(0, 0))) 
                        imT->setBright(f);
                }               

                ImGui::SameLine(posD4);
                {
                    float f = imT->getContrast();
                    if(ImGui::hslTuning(buildID(base, idA++, id), &f, HSL_TUNING_L, .001f, ImVec2(wButt4-2, buttY),ImVec2(0, 0))) 
                        imT->setContrast(f);
                
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

    particlesSystemClass *pSys = theWnd->getParticlesSystem();
    bool bbSelected = pSys->whichRenderMode==RENDER_USE_BILLBOARD || pSys->getRenderMode() == RENDER_USE_BOTH;
    bool psSelected = pSys->whichRenderMode==RENDER_USE_POINTS    || pSys->getRenderMode() == RENDER_USE_BOTH;

#if !defined(GLCHAOSP_LIGHTVER) || defined(GLCHAOSP_LIGHTVER_EXPERIMENTAL) 
    #if defined(GLCHAOSP_LIGHTVER_EXPERIMENTAL) 
        const int wSZ = 270, chSZ = 1020;
        const int hSZ = chSZ > pSys->getHeight() ? pSys->getHeight() : chSZ;
    #else
        const int wSZ = 270, hSZ = 1020;
    #endif
#else
    const int wSZ = theApp->isTabletMode() ? 300 : 270, chSZ = theApp->isTabletMode() ? 900 : 820;
    const int hSZ = chSZ > pSys->getHeight() ? pSys->getHeight() : chSZ;
#endif
    const float border = DLG_BORDER_SIZE;    

    ImGui::SetNextWindowSize(ImVec2(wSZ, hSZ), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
    const float szMin = 270.f;
    ImGui::SetNextWindowSizeConstraints(ImVec2(szMin, 0), ImVec2(FLT_MAX, FLT_MAX));
        
    const char *particlesWndName = getTitle();
    if(ImGui::Begin(particlesWndName, &isVisible)) {

        //if( ImGui::GetWindowWidth() < szMin) ImGui::SetWindowSize(particlesWndName,ImVec2(szMin,0));

        ImGui::BeginGroup(); 
            ImGui::BeginChild("Settings", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()*5-border)); 
                //static bool bBB=false, bPS=false;
#if !defined(GLCHAOSP_LIGHTVER)
                ImGui::SetNextItemOpen(psSelected || psTreeVisible);
                if(psTreeVisible = ImGui::CollapsingHeader(ICON_FA_SLIDERS " Pointsprite", ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
                    viewSettings(pSys->shaderPointClass::getPtr(), 'P');   
                }
                ImGui::SetNextItemOpen(bbSelected || bbTreeVisible);
                if(bbTreeVisible = ImGui::CollapsingHeader(ICON_FA_SLIDERS " Billboard", ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
                    viewSettings(pSys->shaderBillboardClass::getPtr(),'B');
                }
#else
                ImGui::SetNextItemOpen(true);
                if(psTreeVisible = ImGui::CollapsingHeader(ICON_FA_SLIDERS " Pointsprite", ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
                    viewSettings(pSys->shaderPointClass::getPtr(), 'P');   
                }
#endif

            ImGui::EndChild();  

            ImGui::BeginChild("Commons"); 
                const float w = ImGui::GetContentRegionAvail().x;
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

#if !defined(GLCHAOSP_LIGHTVER)
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
#else

                ImGui::NewLine();
#endif
                { //Emitter
                    ImGui::SameLine(pos3); 
                    const bool b = pSys->getEmitter()->isEmitterOn();
                    if(colCheckButton(b, b ? "Emitter " ICON_FA_TOGGLE_ON : "Emitter " ICON_FA_TOGGLE_OFF, wButt)) {pSys->getEmitter()->setEmitter(b^1); }
                }

                //////////Linea 2//////////
                // Billboard
#if !defined(GLCHAOSP_LIGHTVER)
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
#else
                bool b = theDlg.getInvertSettings();
                if(colCheckButton(b, b ? ICON_FA_CHECK " Alternative settings"  : "   Alternative settings", (wButt+border)*2)) {
                    theDlg.setInvertSettings(b^1);
                    if(theApp->getLastFile().size()>0) loadAttractorFile(false, theApp->getLastFile().c_str());
                }
                ImGui::SameLine();

#endif                    

                ImGui::SetCursorPosX(pos3); 
                {
                    const bool b = pSys->getEmitter()->stopFull();
                    //if(colCheckButton(!b, b ? "StopFull" : "CircBuffer", wButt)) { pSys->getEmitter()->stopFull(b ? false : true); }
                    if(ImGui::Button(b ? ICON_FA_STEP_FORWARD " fullStop" : ICON_FA_FORWARD " Continue", ImVec2(wButt,0))) { pSys->getEmitter()->stopFull(b ? false : true); }
                }


                //////////Linea 3//////////
                // Both
#if !defined(GLCHAOSP_LIGHTVER)
                {
                    ImGui::SetCursorPosX(border);
                    const bool b = pSys->whichRenderMode==RENDER_USE_BOTH;
                    if(colCheckButton(b, b ? "Both " ICON_FA_CHECK : "Both", wButt)) pSys->setRenderMode(RENDER_USE_BOTH); 
                }
#else 
                ImGui::NewLine();
#endif                
                ImGui::SameLine(pos2);
                ImGui::TextDisabled(ICON_FA_ARROW_DOWN " Buffer "  ICON_FA_ARROW_RIGHT);
                    
                ImGui::SameLine(pos3); 
                {
                    const bool b = pSys->getEmitter()->restartCircBuff();
                    if(ImGui::Button(b  ? ICON_FA_REPEAT " Restart" : ICON_FA_CIRCLE_O_NOTCH " Endless", ImVec2(wButt,0))) { pSys->getEmitter()->restartCircBuff(b ? false : true); }
                }

#if !defined(GLCHAOSP_LIGHTVER)
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
#endif                
                    
                ImGui::AlignTextToFramePadding();
                //ImGui::SetCursorPosX(pos3-border-9*(ImGui::GetFontSize()*.5+.5)); 
                
                ImGui::SetCursorPosX(pos2);
                { //max particles
                    emitterBaseClass *emit = pSys->getEmitter();
                    ImGui::PushItemWidth(wButt);
                    float f = float(emit->getSizeCircularBuffer())*.000001f;
                    if(ImGui::DragFloat("##max", &f, .01f, .1, 
                                                     emit->getSizeAllocatedBuffer()*.000001f,"%.3f M")) {
                        uint maxBuff = uint(f*1000000.f);
                        emit->setSizeCircularBuffer(maxBuff>emit->getSizeAllocatedBuffer() ? emit->getSizeAllocatedBuffer() : maxBuff);
                        emit->getVertexBase()->resetVertexCount();
                        if(!emit->isEmitterOn()) emit->setEmitterOn();

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


#if !defined(GLCHAOSP_LIGHTVER)

void comboWindowRes(const float width)
{

    const int buttSize = 0;
    ImGui::AlignTextToFramePadding();
    ImGui::TextDisabled("FrameBuffer Size:");
    ImGui::SameLine();
    ImGui::PushItemWidth(width-ImGui::GetCursorPosX()-DLG_BORDER_SIZE-buttSize);

    const char *items[16] = { "", "1024x1024", "1280x720", "1280x1024", "1920x1080", "1920x1200", "2048x2048", "2560x1440", "3440x1440" };
    
    int w, h;
    const int fbHeight = theApp->GetHeight(), fbWidth = theApp->GetWidth();

    char s[30];
    sprintf(s, "%dx%d (current)", fbWidth, fbHeight);
    items[0]=s;
            
    static int i;
    if (ImGui::Combo("##wSize", &i, items, 8)) {  
        sscanf(items[i], "%dx%d", &w, &h);        
        //std::istringstream iss(items[i]); 
        //char x; iss >> w >> x >> h;

        theApp->setWindowSize(w,h);
        i=0;
    }
// FIXME: wait for Begin/End ImGui inconsistence with BeginPopup/EndPopup is resolved
//
//    ImGui::SameLine(); 
//    static bool openDialog = false;
//    if(ImGui::Button("Set",ImVec2(buttSize-DLG_BORDER_SIZE, 0))) { ImGui::OpenPopup("Set resolution"); openDialog = true; }
//
//        if(ImGui::BeginPopupModal("Set resolution"), NULL, ImGuiWindowFlags_AlwaysAutoResize) {
//            ImGui::PopItemWidth();
//
//            ImGui::TextDisabled("FrameBuffer:");
//            ImGui::PushItemWidth(40);
//            ImGui::SameLine(); ImGui::DragInt("##ww", &oldW, .25); 
//            ImGui::SameLine(); ImGui::Text(" x ");
//            ImGui::SameLine(); ImGui::DragInt("##hh", &oldH, .25); 
//            ImGui::PopItemWidth();
//            if (ImGui::Button("OK", ImVec2(120, 0))) { theApp->setWindowSize(oldW,oldH); ImGui::CloseCurrentPopup(); openDialog = false; }
//            ImGui::SetItemDefaultFocus();
//            ImGui::SameLine();
//            if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); openDialog = false; }
//        }
}


void progSettingDlgClass::view()
{
    ImGui::SetNextWindowSize(ImVec2(300, 440), ImGuiCond_FirstUseEver);

    if(!isVisible) return;

    //bool wndVisible;
    if(ImGui::Begin(getTitle(), &isVisible)) { 

        const float w = ImGui::GetContentRegionAvail().x;
        const float border = DLG_BORDER_SIZE;
        const float wButt = ImGui::GetContentRegionAvail().x;

        ImGui::Text(" Size and position");

        comboWindowRes(w);

        //ImGui::AlignTextToFramePadding();
        static bool wantSavePos = false;
        ImGui::Checkbox("Save position ", &wantSavePos);

        //ImGui::SameLine();
        //static bool savePos;
        //if(ImGui::Checkbox("##pos", &savePos)) {    }

        ImGui::SameLine(); 
        int x,y;
        glfwGetWindowPos(theApp->getGLFWWnd(),&x,&y);
        ImGui::Text("[ %4d, %4d ]", x, y);

        {
            bool b = bool(theApp->getVSync());
            if(ImGui::Checkbox("vSync  ", &b)) {
                theApp->setVSync(b ? 1 : 0);
                glfwSwapInterval(theApp->getVSync());
            }
        }
        ImGui::SameLine(); 
        {
            bool b = bool(theApp->slowGPU());
            if(ImGui::Checkbox("slow GPU", &b)) theApp->slowGPU(b);
            ImGui::SameLine();
            ShowHelpMarker(GLAPP_HELP_SLOW_GPU);

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

        ImGui::Text(" GUI apparence / Fonts");
        ImGui::SameLine();
        ShowHelpMarker(GLAPP_HELP_FONTS);

        ImGui::AlignTextToFramePadding();
        ImGui::TextDisabled("Theme:");
        ImGui::SameLine();
        const int idxTheme = ShowStyleSelector(wButt-ImGui::GetCursorPosX());

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

        static bool emitterChanges = false;

        static float maxBuff= theApp->getMaxAllocatedBuffer()*(.000001);
        ImGui::Text(" Emitter type & properties ");
        ImGui::SameLine();
        ShowHelpMarker(GLAPP_HELP_EMIT_TYPE);

        ImGui::PushItemWidth(wButt*.5-border);
#ifdef GLCHAOSP_DISABLE_MACOS_MT
        static int idxEmitt = emitter_singleThread_externalBuffer;
        ImGui::AlignTextToFramePadding();
        ImGui::TextDisabled("Single thread");
#else
        static int idxEmitt = theApp->getEmitterType();
    #ifdef GLAPP_REQUIRE_OGL45
        if(ImGui::Combo("##EmitterType", &idxEmitt, "1 thread  & CPU buffer\0"\
                                                    "2 threads & CPU buffer\0"\
                                                    "2 threads & GPU mapped buffer\0")) {
    #else
        if(ImGui::Combo("##EmitterType", &idxEmitt, "Single thread & Aux buffer\0"\
                                                    "Aux thread & Aux buffer\0")) {
    #endif
            emitterChanges|=true;
        }
#endif

        static int emitStep = theApp->getEmissionStepBuffer()>>10;
        if(idxEmitt!=emitter_separateThread_mappedBuffer) {
            ImGui::SameLine(wButt*.5 + border*2);            
            if(ImGui::DragInt("##emStep", &emitStep,1, 0, 2000, "Aux buff: %dK")) emitterChanges|=true; 
        }
        ImGui::PopItemWidth();

        ImGui::AlignTextToFramePadding();
        ImGui::TextDisabled("Max particles:"); 
        ImGui::SameLine();
        //ImGui::PushItemWidth(wButt*.5 -ImGui::GetCursorPosX() - border);
        ImGui::PushItemWidth(wButt*.3 - border);
        if(ImGui::DragFloat("##partNum",&maxBuff,0.05, 0, PARTICLES_MAX*(.000001),"%.2fM")) emitterChanges|=true;
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::TextDisabled(" Mem:"); 
        ImGui::SameLine(); 
        //ImGui::PushItemWidth(wButt -ImGui::GetCursorPosX());
        ImGui::Text("%.2fGB", (maxBuff*16000000.f)/(1024*1024*1024));
#if !defined(__APPLE__) && !defined(GLCHAOSP_LIGHTVER)
        {
            bool b = theApp->useSyncOGL();
            if(ImGui::Checkbox("Synchronous OpenGL mode", &b)) theApp->useSyncOGL(b);
            ImGui::SameLine();
            ShowHelpMarker(GLAPP_HELP_SYNCOGL);
        }
#endif

        if(emitterChanges) { pushColorButton();
            if(ImGui::Button("Apply Emitter Changes", ImVec2(w,0))) {
                attractorsList.getThreadStep()->stopThread();
                theApp->setMaxAllocatedBuffer(maxBuff * 1000000.f);
                theApp->setEmissionStepBuffer(emitStep<<10);
                theApp->selectEmitterType(idxEmitt);
                theApp->needRestart(true);
                if(emitterChanges) popColorButton();
                emitterChanges = false;
            }
            if(emitterChanges) popColorButton();
        } else { ImGui::AlignTextToFramePadding(); ImGui::NewLine(); }



        ImGui::NewLine();

        static bool optionChanges = false;
        {
            ImGui::AlignTextToFramePadding();
            bool b = bool(theApp->isParticlesSizeConstant());
            if(ImGui::Checkbox(" Particles size constant", &b)) {
                theApp->setParticlesSizeConstant(b);
                theWnd->getParticlesSystem()->setFlagUpdate();
            }
            ImGui::SameLine();
            ShowHelpMarker(GLAPP_HELP_PART_SZ_CONST);
        }
        {
            ImGui::AlignTextToFramePadding();
            bool b = theApp->useDetailedShadows();
            if(ImGui::Checkbox(" Detailed shadow", &b)) {
                particlesSystemClass *sys = theWnd->getParticlesSystem();
                theApp->useDetailedShadows(b);
                const int detail = theApp->useDetailedShadows() ? 2 : 1;
                sys->getShadow()->getFBO().reBuildFBO(1,sys->getWidth()*detail,sys->getHeight()*detail,GL_RGBA32F);
            }
            ImGui::SameLine();
            ShowHelpMarker(GLAPP_HELP_DETAILED_SHADOW);
        }
        {
            ImGui::AlignTextToFramePadding();
            bool b = bool(theApp->useLowPrecision());
            if(ImGui::Checkbox(" Use LOW precision", &b)) {                
                if(b) theApp->setLowPrecision();
                else  theApp->setHighPrecision();
                //theApp->needRestart(true);
                optionChanges = true;
            }
            ImGui::SameLine();
            ShowHelpMarker(GLAPP_HELP_PRECISION);
        }
        {
            ImGui::AlignTextToFramePadding();
            bool b = bool(theApp->getStartWithAttractorName()!="random");
            if(ImGui::Checkbox(" Start with current attractor", &b)) {
                if(b) theApp->setStartWithAttractorName(attractorsList.getNameID());
                else  theApp->setStartWithAttractorName("random");
                optionChanges = true;
            }
            ImGui::SameLine();
            ShowHelpMarker(GLAPP_HELP_START_ATTRACTOR);
        }
/*
        {
            ImGui::AlignTextToFramePadding();
            bool b = bool(theApp->isParticlesSizeConstant());
            if(ImGui::Checkbox(" Force min PointSprite size to 1.0", &b)) {
                theApp->isParticlesSizeConstant(b);
                theWnd->getParticlesSystem()->setFlagUpdate();

            }
            ImGui::SameLine();
            ShowHelpMarker(GLAPP_HELP_FORCE_PNTSPRT_SZ);
        }
*/
        ImGui::NewLine();

        ImGui::Text(" Preferred paths");
        ImGui::SameLine();
        ShowHelpMarker(GLAPP_HELP_CAPTURE);

        ImGui::AlignTextToFramePadding();
        if(ImGui::Button("...##02")) theApp->selectFolder(theApp->getPlyPath());
        ImGui::SameLine();

        ImGui::TextDisabled(" Imp/Exp PLY: ");
        ImGui::SameLine();
        ImGui::Text(theApp->getPlyPath().c_str());

        ImGui::AlignTextToFramePadding();
        if(ImGui::Button("...##03")) theApp->selectFolder(theApp->getRenderCfgPath());
        ImGui::SameLine();

        ImGui::TextDisabled(" Render CFGs: ");
        ImGui::SameLine();
        ImGui::Text(theApp->getRenderCfgPath().c_str());

        ImGui::AlignTextToFramePadding();
        if(ImGui::Button("...##01")) theApp->selectFolder(theApp->getCapturePath());
        ImGui::SameLine();

        ImGui::TextDisabled(" ScreenShots: ");
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
        if(optionChanges) pushColorButton();
        if(ImGui::Button("Save Prog.Settings",ImVec2(wButt,0))) {
            attractorsList.getThreadStep()->stopThread();
            theApp->setPosX(wantSavePos ? x : -1);
            theApp->setPosY(wantSavePos ? y : -1);
            theApp->setMaxAllocatedBuffer(maxBuff * 1000000.f);
            theApp->setEmissionStepBuffer(emitStep<<10);
            theApp->selectEmitterType(idxEmitt);
            theApp->saveProgConfig();
            //theApp->resetParticlesSystem();
            theApp->needRestart(true);
            if(optionChanges) popColorButton();
            emitterChanges = false;
            optionChanges = false;
        }
        if(optionChanges) popColorButton();

    }
        ImGui::End();
    
}

void dataDlgClass::view()
{

    if(!isVisible) return;

    ImGui::SetNextWindowSize(ImVec2(300, 440), ImGuiCond_FirstUseEver);

    if(ImGui::Begin(getTitle(), &isVisible)) {
    
        const float border = DLG_BORDER_SIZE;
        const float w = ImGui::GetContentRegionAvail().x;
        const float wH = w*.5f+border*2;
        const float wButt = w-border*2;
        const float wButtH = wButt * .5f;

        static bool bBinary = true;
        static bool bColors = true;
        static bool bAlphaDist = false;
        static bool bNormals = true;
        static bool bNormalized = true;
        static bool bCoR = true;
        static int idxNorm = 0;
        static int idxDistType = 0;

        const ImU32 titleCol = ImGui::GetColorU32(ImGuiCol_PlotLines); 

        ImGui::PushStyleColor(ImGuiCol_Text, titleCol);
        ImGui::Text(" " ICON_FA_FLOPPY_O "  Export PLY vertex");
        ImGui::SameLine();
        ImGui::Text("  " ICON_FA_COMMENT_O);
        ImGui::PopStyleColor();
        ShowHelpOnTitle(GLAPP_HELP_EXPORT_PLY);

        ImGui::Checkbox("Binary fileType", &bBinary);
        ImGui::Checkbox("Colors##E", &bColors);
        if(bColors) {
            ImGui::SameLine(wH);
            ImGui::Checkbox("alpha have dist##", &bAlphaDist);        
        }
        ImGui::Checkbox("Normals", &bNormals);
            ImGui::SameLine(wH);
        ImGui::Checkbox("Use CoR", &bCoR);
        if(bNormals) {
            ImGui::TextDisabled(" Normals type");
            ImGui::PushItemWidth(wButtH);
            ImGui::Combo("##NormaType", &idxNorm, "pt(i) + CoR\0"\
                                                   "pt(i) + pt(i+1)\0"\
                                                   "pt(i) + pt(i+1) + CoR\0");
            ImGui::SameLine(wH);
            ImGui::Checkbox("Normalized", &bNormalized);
            ImGui::PopItemWidth();
        } else {
            ImGui::NewLine();
            ImGui::AlignTextToFramePadding();
            ImGui::NewLine();
        }


        if(ImGui::Button(ICON_FA_FLOPPY_O " Export PLY", ImVec2(w,0))) exportPLY(bBinary, bColors, bAlphaDist, bNormals, bCoR, bNormalized, (normalType) idxNorm);

        ImGui::NewLine();
        ImGui::PushStyleColor(ImGuiCol_Text, titleCol);
        ImGui::Text(" " ICON_FA_FOLDER_OPEN_O "  Import PLY vertex");
        ImGui::SameLine();
        ImGui::Text("  " ICON_FA_COMMENT_O);
        ImGui::PopStyleColor();
        ShowHelpOnTitle(GLAPP_HELP_IMPORT_PLY);
        bool b = theWnd->getParticlesSystem()->wantPlyObjColor();
        if(ImGui::Checkbox("PLY Colors##I", &b)) theWnd->getParticlesSystem()->wantPlyObjColor(b);
        if(!b) {            
            ImGui::PushItemWidth(wButtH);
            ImGui::Combo("##DistType", &idxDistType, "points distance\0"\
                                                     "radial distance\0"\
                                                     "alpha have dist\0");
            ImGui::SameLine(wH);
            bool bl = attractorsList.continueDLA();
            if(ImGui::Checkbox("continue DLA", &bl)) attractorsList.continueDLA(bl);
            ImGui::PopItemWidth();
        } 

        if(ImGui::Button(ICON_FA_FOLDER_OPEN_O " Import PLY", ImVec2(w,0))) {
            if(attractorsList.continueDLA()) { 
                const int i = attractorsList.getSelectionByName("dla3D");
                if(i>0) attractorsList.selectToContinueDLA(i);
            }
            if(importPLY(b, idxDistType)) theWnd->getParticlesSystem()->viewObjON();
        }
//        ImGui::SameLine(wH);


        ImGui::AlignTextToFramePadding();
        ImGui::NewLine();
        ImGui::PushStyleColor(ImGuiCol_Text, titleCol);
        ImGui::Text(" " ICON_FA_FLOPPY_O " Export render settings");
        ImGui::SameLine();
        ImGui::Text("  " ICON_FA_COMMENT_O);
        ImGui::PopStyleColor();
        ShowHelpOnTitle(GLAPP_HELP_EXP_RENDR_CFG);
        if(ImGui::Button(ICON_FA_FLOPPY_O " Export to CFG", ImVec2(w,0))) saveSettingsFile();
        ImGui::NewLine();

        ImGui::PushStyleColor(ImGuiCol_Text, titleCol);
        ImGui::Text(" " ICON_FA_FOLDER_OPEN_O " Import render settings");
        ImGui::SameLine();
        ImGui::Text("  " ICON_FA_COMMENT_O);
        ImGui::PopStyleColor();
        ShowHelpOnTitle(GLAPP_HELP_IMP_RENDR_CFG);
        ImGui::TextDisabled("Select particles type");

        ImGui::Checkbox("PointSprite", &psSettings);  ImGui::SameLine(wH);
        ImGui::Checkbox("Billboard", &bbSettings);

        ImGui::TextDisabled("Select data groups");

        ImGui::Checkbox("Rendering", &rendering);  ImGui::SameLine(wH);
        ImGui::Checkbox("Colors", &color);
        ImGui::Checkbox("Light", &light);  ImGui::SameLine(wH);
        ImGui::Checkbox("Glow", &glow);
        ImGui::Checkbox("Shadow", &shadow);  ImGui::SameLine(wH);
        ImGui::Checkbox("AO", &ao);
        ImGui::Checkbox("FXAA", &fxaa);  ImGui::SameLine(wH);
        ImGui::Checkbox("Adjust", &adjust);

        ImGui::Checkbox("View/Model settings", &viewSettings);
        if(attractorsList.get()->dtType()) 
            ImGui::Checkbox("SlowMotion/CockPit (if available)", &adjust);
        if(ImGui::Button(ICON_FA_FOLDER_OPEN_O " Import from CFG/SCA", ImVec2(w,0))) loadSettingsFile(true);
/*
    bool psSettings = true, bbSettings = true;
    bool particles = true, palette = true, lightModel = true;
    bool shadow = true, ao = true, adjoust = true;
    bool glow = true, fxaa = true;
*/

    }
    ImGui::End();

}

#endif

void particleEditDlgClass::view()
{

    if(!isVisible) return;

#if !defined(GLCHAOSP_LIGHTVER)
    ImGui::SetNextWindowSize(ImVec2(350, 440), ImGuiCond_FirstUseEver);

    particlesBaseClass *pSys = theWnd->getParticlesSystem()->getWhitchRenderMode()==RENDER_USE_BILLBOARD ? 
        (particlesBaseClass *) theWnd->getParticlesSystem()->shaderBillboardClass::getPtr() : 
        (particlesBaseClass *) theWnd->getParticlesSystem()->shaderPointClass::getPtr();
#else
    ImGui::SetNextWindowPos(ImVec2(theApp->GetWidth()-385,0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(200, 280), ImGuiCond_FirstUseEver);
    particlesBaseClass *pSys = (particlesBaseClass *) theWnd->getParticlesSystem()->shaderPointClass::getPtr();
#endif


    if(ImGui::Begin(getTitle(), &isVisible)) {
        const float w = ImGui::GetContentRegionAvail().x;
        const float wButt = w;
        const float wButt3 = w*.3333;

        ImGui::Text("Hermite coefficients");
        ImGui::PushItemWidth(wButt);

        if(ImGui::DragFloat4("##parED",value_ptr(pSys->getHermiteVals()), .01, 0.0, 1.0,"%.2f")) {
            pSys->getDotTex().build();
        }
        ImGui::PopItemWidth();

        dotsTextureClass *dots = &pSys->getDotTex();

        //ImGui::SetCursorPosX(border);
        ImGui::Image(reinterpret_cast<ImTextureID>((int64) dots->getTexID()), ImVec2(wButt,wButt));
        char txt[32];


#if !defined(GLCHAOSP_LIGHTVER)
        ImGui::PushItemWidth(wButt3);
        {
            int idx = dots->getIndex();
            sprintf(txt, "pixel size##" "%s", theWnd->getParticlesSystem()->getWhitchRenderMode()==RENDER_USE_BILLBOARD ? "B" : "P"); 
            if (ImGui::Combo(txt, &idx, " 32\0"\
                                        " 64\0"\
                                        "128 (default)\0"\
                                        "256\0"\
                                        "512\0"))
                { dots->setIndex(idx); }
        }
        ImGui::PopItemWidth();

        ImGui::SameLine(w-(wButt3-DLG_BORDER_SIZE));
        const float wSZ = wButt3;
#else
        const float wSZ = w;
#endif
        {
            bool b = dots->getDotType();
            sprintf(txt, b ? ICON_FA_CHECK_SQUARE_O " solidDot##" "%s" : ICON_FA_SQUARE_O " solidDot##"  "%s", theWnd->getParticlesSystem()->getWhitchRenderMode()==RENDER_USE_BILLBOARD ? "B" : "P"); 
            if(colCheckButton(b , txt, wSZ)) dots->setDotType(b^1);
        }


    }
    ImGui::End();

}

void cockpitDlgClass::view()
{

    if(!isVisible) return;

    const int posH = 225;
    const int szH = 400+7*ImGui::GetFrameHeightWithSpacing();
    const int posW = 0;
    const int szW = 250;
/*
    particlesSystemClass *pSys = theWnd->getParticlesSystem();
    bool bbSelected = pSys->whichRenderMode==RENDER_USE_BILLBOARD || pSys->getRenderMode() == RENDER_USE_BOTH;
    bool psSelected = pSys->whichRenderMode==RENDER_USE_POINTS  
  */

    ImGui::SetNextWindowPos(ImVec2(theApp->GetWidth()-szW-posW,posH ), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(szW, szH), ImGuiCond_FirstUseEver);

    //bool wndVisible;
    if(ImGui::Begin(getTitle(), &isVisible, ImGuiWindowFlags_NoScrollbar)) { 
        particlesSystemClass *pSys = theWnd->getParticlesSystem();

#if !defined(GLCHAOSP_LIGHTVER)
        particlesBaseClass *particles =  pSys->getRenderMode()==RENDER_USE_BILLBOARD ? (particlesBaseClass *) pSys->shaderBillboardClass::getPtr() : 
                                                                                       (particlesBaseClass *) pSys->shaderPointClass::getPtr();
#else
        particlesBaseClass *particles =  (particlesBaseClass *) pSys->shaderPointClass::getPtr();
#endif
        cockpitClass &cPit = attractorsList.getCockpit();

        const ImU32 titleCol = ImGui::GetColorU32(ImGuiCol_PlotLines); 

        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
        {
            ImGui::AlignTextToFramePadding();
            ImGui::TextDisabled("FixedView dots/s "); ImGui::SameLine();
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
            int DpS = attractorsList.getSlowMotionDpS();
            if(ImGui::DragInt("##DpS__", &DpS, 1.0f, 0, INT32_MAX, "%d")) attractorsList.setSlowMotionDpS(DpS);
            ImGui::PopItemWidth();
        }
        ImGui::PopItemWidth();

        const float w = ImGui::GetContentRegionAvail().x;
        const float w2 = w*.5;
        const float w3 = w*.3333;
        const float w6 = w*.6666;
        const float spaceX = ImGui::GetStyle().ItemSpacing.x;
        {
            ImGui::PushStyleColor(ImGuiCol_Text, titleCol);
            const bool isOpen = ImGui::TreeNodeEx("##EmitSM",ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoTreePushOnOpen,ICON_FA_SHOWER  " Emitter   " ICON_FA_COMMENT_O);
            ImGui::PopStyleColor();
            ShowHelpOnTitle(GLAPP_HELP_EMIT_SM);
            if(isOpen) {
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
                ImGui::BeginChild("##EmitSMChld", ImVec2(0,ImGui::GetFrameHeightWithSpacing()*7+ImGui::GetStyle().ItemSpacing.y), true);


                ImGui::TextDisabled(" Emit #");  ImGui::SameLine(w3+spaceX);
                ImGui::TextDisabled(" emitVel"); ImGui::SameLine(w6+spaceX);
                ImGui::TextDisabled(" airFriction");
                
                ImGui::PushItemWidth(w3-spaceX*.3333);
                    {
                        int i = cPit.getTransformedEmission();
                        if(ImGui::DragInt("##Emission", &i, .1f, 1, cPit.getMaxTransformedEmission(), "%d")) cPit.setTransformedEmission(i);
                    }
                    ImGui::SameLine();
                    {
                        float f = cPit.getInitialSpeed();
                        if(ImGui::DragFloat("##InitSpeed", &f, .01f, .0f, FLT_MAX, "%.3f")) cPit.setInitialSpeed(f);
                    }
                    ImGui::SameLine();
                    {
                        float f = cPit.getAirFriction();
                        if(ImGui::DragFloat("##airFriction", &f, .01f, .0f, FLT_MAX, "%.2f")) cPit.setAirFriction(f);
                    }
                ImGui::PopItemWidth();

                ImGui::TextDisabled(" pointSize");     ImGui::SameLine(w3+spaceX);
                ImGui::TextDisabled(" lifeTime");      ImGui::SameLine(w6+spaceX);
                ImGui::TextDisabled(" lifeTimeAtten"); 
                ImGui::PushItemWidth(w3-spaceX*.3333);
                    {
                        float f = particles->getSizeTF();
                        if(ImGui::DragFloat("##pointSize", &f, .01f, 0.01f, FLT_MAX, "%.3f")) particles->setSizeTF(f);
                    }
                    ImGui::SameLine();
                    {
                        float f = cPit.getLifeTime();
                        if(ImGui::DragFloat("##lifeTime", &f, .05f, 1.0f, FLT_MAX, "%.2f")) cPit.setLifeTime(f);
                    }
                    ImGui::SameLine();
                    {
                        float f = cPit.getLifeTimeAtten();
                        if(ImGui::DragFloat("##lifeTimeAtten", &f, .001f, 0.0f, 1.0, "%.3f")) cPit.setLifeTimeAtten(f);
                    }
                ImGui::PopItemWidth();

                ImGui::PushItemWidth(w);
                    ImGui::TextDisabled(" Wind x y z startTime");
                    {
                        float *f = value_ptr(cPit.getUdata().wind);
                        if(ImGui::DragFloat4("##wind", f, .01f)) cPit.getUdata().wind = *((vec4 *)f);
                    }
                    ImGui::TextDisabled(" Gravity x y z startTime");
                    {
                        float *f = value_ptr(cPit.getUdata().gravity);
                        if(ImGui::DragFloat4("##gravity", f, .01f)) cPit.getUdata().gravity = *((vec4 *)f);
                    }
                ImGui::PopItemWidth();

                ImGui::EndChild();
                ImGui::PopStyleVar();
            }
        }
        bool cP = attractorsList.getCockpit().cockPit();
        if(colCheckButton(cP , cP ? " CockPit "  ICON_FA_CHECK_SQUARE_O " " : " CockPit " ICON_FA_SQUARE_O " ",ImGui::GetContentRegionAvail().x)) attractorsList.getCockpit().cockPit(cP^1);

        if(cP) {
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
            {
                int DpS = cPit.getSlowMotionDpS();
                ImGui::AlignTextToFramePadding();
                ImGui::TextDisabled("CockPit dots/s "); ImGui::SameLine();
                ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
                if(ImGui::DragInt("##DpSCP", &DpS, 1.0f, 0, INT32_MAX, "%d")) cPit.setSlowMotionDpS(DpS);
                ImGui::PopItemWidth();
            }

            {
                ImGui::PushStyleColor(ImGuiCol_Text, titleCol);
                const bool isOpen = ImGui::TreeNodeEx("##PartCP",ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoTreePushOnOpen,ICON_FA_BRAILLE  " Particles   " ICON_FA_COMMENT_O);
                ImGui::PopStyleColor();
                ShowHelpOnTitle(GLAPP_HELP_COCKPIT_PARTICLES);
                if(isOpen) {
                    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
                    ImGui::BeginChild("##PartCPChld", ImVec2(0,ImGui::GetFrameHeightWithSpacing()*2), true);

                    const float w = ImGui::GetContentRegionAvail().x;
                    const float w2 = w*.5;
                    const float spaceX = ImGui::GetStyle().ItemSpacing.x;

                    ImGui::TextDisabled(" pointSize");      ImGui::SameLine(w3+spaceX);
                    ImGui::TextDisabled(" smoothDist"); ImGui::SameLine(w6+spaceX);
                    ImGui::TextDisabled(" clipDist"); 

                    ImGui::PushItemWidth(w3-spaceX*.3333);
                    {
                        float f = cPit.getPointSize();
                        if(ImGui::DragFloat("##pointSzSM", &f, .001f, .001f, FLT_MAX, "%.3f")) cPit.setPointSize(f);
                    }
                    ImGui::SameLine();
                    {
                        float f = cPit.getSmoothDistance();
                        if(ImGui::DragFloat("##smoothD", &f, .001f, 0.0f, 5.0, "%.3f")) cPit.setSmoothDistance(f);
                    }
                    ImGui::SameLine();
                    {
                        float f = particles->getClippingDist();
                        if(ImGui::DragFloat("##clipDist", &f, .001f, 0.001f, FLT_MAX, "%.3f")) particles->setClippingDist(f);
                    }
                    ImGui::PopItemWidth();

                    ImGui::EndChild();
                    ImGui::PopStyleVar();
                }
            }
            {
                ImGui::PushStyleColor(ImGuiCol_Text, titleCol);
                const bool isOpen = ImGui::TreeNodeEx("##PiPSM",ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoTreePushOnOpen,ICON_FA_OBJECT_GROUP  " PiP   " ICON_FA_COMMENT_O);
                ImGui::PopStyleColor();
                ShowHelpOnTitle(GLAPP_HELP_COCKPIT_PIP);
                if(isOpen) {
                    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
                    ImGui::BeginChild("##PiPSMChld", ImVec2(0,ImGui::GetFrameHeightWithSpacing()*3+ImGui::GetStyle().ItemSpacing.y), true);
                    {
                        int pos = cPit.getPIPposition();
                        float a = ImGui::GetCursorPosX();
                        if(ImGui::RadioButton("##lTop", pos == cPit.pip::lTop)) { cPit.setPIPposition(cPit.pip::lTop); } 
                        ImGui::SameLine(); 
                        float b = ImGui::GetCursorPosX();
                        float size = b - a; 
                        ImGui::SetCursorPosX(b+size);
                        if(ImGui::RadioButton("##rTop", pos == cPit.pip::rTop)) { cPit.setPIPposition(cPit.pip::rTop); }
                        ImGui::SameLine();
                        a = ImGui::GetCursorPosX();
                        {
                            bool b = cPit.invertPIP();
                            if(ImGui::Checkbox(" Invert PIP", &b)) cPit.invertPIP(b);
                        }

                        ImGui::SetCursorPosX(ImGui::GetCursorPosX()+size);
                        if(ImGui::RadioButton("##noPIP", pos == cPit.pip::noPIP)) { cPit.setPIPposition(cPit.pip::noPIP); } 
                        ImGui::SameLine();
                        ImGui::SetCursorPosX(a);
                        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
                        {
                            float f = cPit.getPIPzoom();
                            if(ImGui::SliderFloat("##zoomPIP", &f, .25, 2.0,"size PiP %.3f")) cPit.setPIPzoom(f);
                        }
                        ImGui::PopItemWidth();

                        if(ImGui::RadioButton("##lBottom", pos == cPit.pip::lBottom)) { cPit.setPIPposition(cPit.pip::lBottom); } 
                        ImGui::SameLine(); 
                        ImGui::SetCursorPosX(b+size);
                        if(ImGui::RadioButton("##rBottom", pos == cPit.pip::rBottom)) { cPit.setPIPposition(cPit.pip::rBottom); } 

                    }
                    ImGui::EndChild();
                    ImGui::PopStyleVar();
                }
            }
            {
                ImGui::PushStyleColor(ImGuiCol_Text, titleCol);
                const bool isOpen = ImGui::TreeNodeEx("##viewSM",ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_NoTreePushOnOpen,ICON_FA_SPACE_SHUTTLE  " View settings   " ICON_FA_COMMENT_O);
                ImGui::PopStyleColor();
                ShowHelpOnTitle(GLAPP_HELP_COCKPIT_VIEW);
                if(isOpen) {
                    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 5.0f);
                    ImGui::BeginChild("##viewSMChld", ImVec2(0,ImGui::GetFrameHeightWithSpacing()*5+ImGui::GetStyle().ItemSpacing.y*2), true);
                    const float gizmoDim = ImGui::GetFrameHeightWithSpacing()*4;
                    const float w = ImGui::GetContentRegionAvail().x;
                    const float spaceX = ImGui::GetStyle().ItemSpacing.x;
                    const float posY = ImGui::GetCursorPosY();
                    const float w6 = (w-gizmoDim-spaceX*2);
                    const float w3 = w6*.5;

                    ImGui::TextDisabled(" fov angle");  ImGui::SameLine(w3+spaceX);
                    ImGui::TextDisabled(" tail POV"); 

                    ImGui::PushItemWidth(w3-spaceX*.3333);
                    {
                        float f = cPit.getPerspAngle();
                        if(ImGui::DragFloat("##fov", &f, .01f, 5.f, 170.f, "%.2f")) cPit.setPerspAngle(f);
                    }
                        ImGui::SameLine();
                    {
                        float f = cPit.getTailPosition();
                        if(ImGui::DragFloat("##tailPos", &f, .001f, 0.0f, 1.0, "%.3f")) cPit.setTailPosition(f);
                    }
                    ImGui::PopItemWidth();

                    ImGui::PushItemWidth(w6);
                    ImGui::TextDisabled(" move Target / POV");
                    {
                        float f[2] = { cPit.getMovePositionHead(), cPit.getMovePositionTail() };
                        if(ImGui::DragFloat2("##TgtPOV", f, .001f, -FLT_MAX, FLT_MAX, "%.3f")) {
                            cPit.setMovePositionHead(f[0]);
                            cPit.setMovePositionTail(f[1]);
                        }
                    }
                    ImGui::PopItemWidth();

                    ImGui::SetCursorPos(ImVec2(w6+spaceX*3, posY));
                    {
                        quat q(cPit.getRotation());
                        //imguiGizmo::resizeSolidOf(.75); // sphere resize
                        if(ImGui::gizmo3D("##RotB1", q, gizmoDim, imguiGizmo::sphereAtOrigin))  { cPit.setRotation(q); } 
                    }
                    {
                        bool b = cPit.invertView();
                        if(ImGui::Checkbox("InvertView", &b)) cPit.invertView(b);
                    }
                    ImGui::SameLine();
                    {
                        ImGui::SetCursorPosX(w6+spaceX*3);
                        if(ImGui::Button("Reset",ImVec2(gizmoDim, 0.0))) cPit.setRotation(quat(1.f, 0.f, 0.f, 0.f));
                    }

                    ImGui::EndChild();
                    ImGui::PopStyleVar();
                }
            }
        }

/*
        {
            float f = cPit.getPerspNear();
            if(ImGui::DragFloat("near", &f, .001f, .001f, FLT_MAX, "%.3f")) cPit.setPerspNear(f);
        }
        {
            bool b = cPit.pipTransparentBckgrnd();
            if(ImGui::Checkbox("TranspBack", &b)) cPit.pipTransparentBckgrnd(b);
        }
        ImGui::SameLine();
        {
            vec3 v(cPit.getPanDollyPos());
            if(ImGui::DragFloat3("Pan/Dolly",value_ptr(v),.01,0.0,0.0)) cPit.setPanDollyPos(v);

        }
*/
    }
    ImGui::End();
}

void viewSettingDlgClass::view()
{

    if(!isVisible) return;
#if !defined(GLCHAOSP_LIGHTVER)
    const int posH = 0;
    const int szH = 365+14*ImGui::GetFrameHeightWithSpacing();
    const int posW = 190;
#else
    const int posH = 385;
    const int szH = 146+ImGui::GetFrameHeightWithSpacing()*12;
    const int posW = 0;
#endif
    const int szW = 300;
    ImGui::SetNextWindowPos(ImVec2(theApp->GetWidth()-szW-posW,posH ), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(szW, szH), ImGuiCond_FirstUseEver);

    //bool wndVisible;
    if(ImGui::Begin(getTitle(), &isVisible, ImGuiWindowFlags_NoScrollbar)) { 
        
        particlesSystemClass *pSys = theWnd->getParticlesSystem();
        vg::vGizmo3D &tBall = pSys->getTMat()->getTrackball();

        const float w = ImGui::GetContentRegionAvail().x;
        const float wHalf = w*.5;
        const float wButt2 = (wHalf-DLG_BORDER_SIZE*1.5);
        //const float wButt3 = (w*.3333-DLG_BORDER_SIZE*.5);

#if !defined(GLCHAOSP_LIGHTVER)
        comboWindowRes(w);
        ImGui::NewLine();
#endif
        //  Camera param
        ///////////////////////////////////////////////////////////////////////
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
        bool b = theApp->idleRotation();
        if(ImGui::Checkbox(" vGizmo idle rotation", &b)) { theApp->idleRotation(b); }
        ImGui::SameLine();
        ShowHelpMarker(GLAPP_HELP_TRACK_IDLE);
        //  Gizmo
        ///////////////////////////////////////////////////////////////////////
        ImGui::NewLine();
        ImGui::Text(" Model/Light Transformations");
        {
            //pSys->shaderPointClass::getPtr() whichRenderMode==RENDER_USE_BILLBOARD

            quat q(tBall.getRotation());
#if !defined(GLCHAOSP_LIGHTVER)
            vec3 l(pSys->getWhitchRenderMode()==RENDER_USE_BILLBOARD ? 
                   -pSys->shaderBillboardClass::getPtr()->getLightDir() :
                   -pSys->shaderPointClass::getPtr()->getLightDir());
#else
            vec3 l(-pSys->shaderPointClass::getPtr()->getLightDir());
#endif
            //imguiGizmo::resizeSolidOf(.75); // sphere resize
            if(ImGui::gizmo3D("##RotB1", q, wButt2, imguiGizmo::sphereAtOrigin))  { tBall.setRotation(q); } 
            //imguiGizmo::restoreSolidSize(); // restore at default
            ImGui::SameLine();
            imguiGizmo::resizeAxesOf(vec3(.75));
            if( ImGui::gizmo3D("##RotA", l,wButt2,0)) { 
#if !defined(GLCHAOSP_LIGHTVER)
                if(pSys->getWhitchRenderMode()==RENDER_USE_BILLBOARD)
                     pSys->shaderBillboardClass::getPtr()->setLightDir(-l);
                else
#endif
                     pSys->shaderPointClass::getPtr()->setLightDir(-l);
            }   
            imguiGizmo::restoreAxesSize();


            ImGui::TextDisabled(" Model");
            ImGui::SameLine(wButt2+DLG_BORDER_SIZE);
            ImGui::TextDisabled(" Light");

            ImGui::PushItemWidth(wButt2);
            ImGui::DragFloat4("##Rot",value_ptr(q),.01,0.0,0.0);
            ImGui::SameLine();
            if(ImGui::DragFloat3("##Lux",value_ptr(l),.01,0.0,0.0)) {
#if !defined(GLCHAOSP_LIGHTVER)
                if(pSys->getWhitchRenderMode()==RENDER_USE_BILLBOARD)
                     pSys->shaderBillboardClass::getPtr()->setLightDir(-l);
                else
#endif
                     pSys->shaderPointClass::getPtr()->setLightDir(-l);
            }
            ImGui::PopItemWidth();
            //if(ImGui::DragFloat4("Rot",value_ptr(q),.01,0.0,0.0)) tBall.setRotation(q);
        }
        ImGui::NewLine(); 
        if(ImGui::Button("Clipping planes", ImVec2(-1,0)))
            theDlg.clippingDlg.visible(theDlg.clippingDlg.visible()^1);

#if !defined(GLCHAOSP_LIGHTVER)

        //  Axes
        ///////////////////////////////////////////////////////////////////////
        ImGui::NewLine(); ImGui::Text(" Center of Rotation");
        ImGui::SameLine();
        ShowHelpMarker(GLAPP_HELP_AXES_COR);

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
                if(pSys->showAxes() == renderBaseClass::showAxesToSetCoR ) {
                    pSys->showAxes(renderBaseClass::noShowAxes);                    
                    tBall.setPosition(tBall.getRotationCenter());
                } else pSys->showAxes(renderBaseClass::showAxesToSetCoR);
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
#endif
    }
    ImGui::End();
}

void imGuIZMODlgClass::view()
{
    if(!isVisible) return;
    ImGuiStyle& style = ImGui::GetStyle();

    //quaternionf qt = theWnd->getTrackball().getRotation();
    quat qt = theWnd->getParticlesSystem()->getTMat()->getTrackball().getRotation();

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

#if !defined(GLCHAOSP_LIGHTVER)
        vec3 &ligh = theWnd->getParticlesSystem()->getRenderMode()==RENDER_USE_POINTS ? theWnd->getParticlesSystem()->shaderPointClass::getLightDir() : theWnd->getParticlesSystem()->shaderBillboardClass::getLightDir();
#else
        vec3 &ligh = theWnd->getParticlesSystem()->shaderPointClass::getLightDir();
#endif
        vec3 lL(-ligh);
        if(ImGui::gizmo3D("##aaa", qt, lL, sz))  { 
#if !defined(GLCHAOSP_LIGHTVER)
            if(theWnd->getParticlesSystem()->getRenderMode()==RENDER_USE_BOTH) {
                theWnd->getParticlesSystem()->shaderPointClass::setLightDir(-lL);
                theWnd->getParticlesSystem()->shaderBillboardClass::setLightDir(-lL);

            } else 
#endif
                ligh = -lL;

            theWnd->getParticlesSystem()->getTMat()->getTrackball().setRotation(qt);
            theWnd->getParticlesSystem()->setFlagUpdate();
        }
        ImGui::PushItemWidth(sz);
        style.Colors[ImGuiCol_Text].x = style.Colors[ImGuiCol_Text].y = 1.0, style.Colors[ImGuiCol_Text].z =0.f;
        ImGui::DragFloat3("##uL3",value_ptr(ligh),0.01f);
        style.Colors[ImGuiCol_Text] = oldTex;
        ImGui::PopItemWidth();


    }
    ImGui::End();

    style.WindowBorderSize = prevWindowBorderSize;
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();


}

void aboutDlgClass::view()
{

    if(!isVisible) return;
    ImGui::SetNextWindowPos(ImVec2(320, 190), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(390, 560), ImGuiCond_FirstUseEver);

    if(ImGui::Begin(getTitle(), &isVisible)) {
    
        //ImGui::SetCursorPosX(4*theDlg.getFontSize()*theDlg.getFontZoom()*.5);
        ImGui::TextUnformatted(GLAPP_HELP_ABOUT);
        //showHelpMarkdown(GLAPP_HELP_ABOUT);
        

        ImGui::NewLine();

        ImGui::Text("Mouse controls and shortcut keys");
        
        //ImGui::SetCursorPosX(4*theDlg.getFontSize()*theDlg.getFontZoom()*.5);
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

//void loadActualSelection()


void infoDlgClass::view()
{

    if(!isVisible) return;
#if !defined(GLCHAOSP_LIGHTVER)
    const int hSz = 15;
#else
    const int hSz = 8;
#endif
    ImGui::SetNextWindowPos(ImVec2(270, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(400, ImGui::GetFrameHeightWithSpacing()*hSz), ImGuiCond_FirstUseEver);

    //bool visible = isVisible && theDlg.baseDlgClass::visible();
    if(ImGui::Begin(getTitle(), &isVisible)) { 

        //isVisible = visible;
    

        const GLubyte *vendor = glGetString(GL_VENDOR);
        const GLubyte *render = glGetString(GL_RENDERER);
        const GLubyte *glVer  = glGetString(GL_VERSION);
        const GLubyte *glslVer= glGetString(GL_SHADING_LANGUAGE_VERSION);

#if !defined(GLCHAOSP_LIGHTVER)
        ImGui::TextDisabled("Timings");
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Inst %6.1f fps, Avg %.1f fps  ", theApp->getTimer().fps(), theApp->getTimer().fpsAVG()); ImGui::SameLine(); 
        if(ImGui::Button("Reset AVG")) theApp->getTimer().resetAVG();
#endif
        ImGui::Text("ImGui Avg %.3f ms/f (%.1f fps)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

        ImGui::NewLine();

        ImGui::TextDisabled("Vendor : "); ImGui::SameLine();
        ImGui::TextUnformatted((const char *)vendor);

        ImGui::TextDisabled("Render : "); ImGui::SameLine();
        ImGui::TextUnformatted((const char *)render);

        ImGui::TextDisabled("glVer  : "); ImGui::SameLine();
        ImGui::TextUnformatted((const char *)glVer);

        ImGui::TextDisabled("glslVer: "); ImGui::SameLine();
        ImGui::TextUnformatted((const char *)glslVer);

#if !defined(GLCHAOSP_LIGHTVER)

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

        ImGui::TextDisabled("PointSprite size");
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
#endif
        ImGui::NewLine();
        if(ImGui::Button(" - show metrics - ")) metricW ^=1;   ImGui::SameLine();

        if(metricW) ImGui::ShowMetricsWindow(&metricW);

#if !defined(GLCHAOSP_LIGHTVER)
        if(ImGui::Button(" restart ")) {
            theApp->needRestart(true);
        }
#endif
    }
    ImGui::End();

}

void clippingDlgClass::view()
{
    if(!isVisible) return;

    ImGui::SetNextWindowPos(ImVec2(270, 0), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(IMGUIZMO_DEF_SIZE*1.8, IMGUIZMO_DEF_SIZE*(1.8*3)+8.5*ImGui::GetFrameHeightWithSpacing()), ImGuiCond_FirstUseEver);

    if(ImGui::Begin(getTitle(), &isVisible,ImGuiWindowFlags_NoScrollbar)) { 
        particlesSystemClass *pSys = theWnd->getParticlesSystem();

        const float w = ImGui::GetContentRegionAvail().x;

        char planes[] = "Plane0"; 
        char names[]  = "##zxmesA"; 
        char boundi[] = "Bound##0";
        char reset[]  = " Reset ##A";
        mat3 m(mat3_cast(pSys->getTMat()->getTrackball().getRotation()));

        for(int i=0; i<3; i++) {
            vec3 plane(normalize(pSys->getClippingPlane(i)));

            plane = m * plane;

            float dist = theWnd->getParticlesSystem()->getClippingPlane(i).w;
        
            bool isChanged = false;
            

            planes[5] = 'X' + i;
            bool b = pSys->getUPlanes().planeActive[i];
            if(ImGui::Checkbox(planes, &b)) pSys->getUPlanes().planeActive[i] = b;

            if(pSys->getUPlanes().planeActive[i]) {

                ImGui::SameLine(w*.5);

                boundi[7] = 'M' + i;
                bool b = pSys->getUPlanes().colorActive[i];
                if(ImGui::Checkbox(boundi, &b)) pSys->getUPlanes().colorActive[i] = b;

                ImGui::SameLine(w-(ImGui::GetTextLineHeightWithSpacing()));
                names[7] = 'J' + i;
                const uint32_t flags  = (ImGuiColorEditFlags_AlphaBar   |
                                         ImGuiColorEditFlags_NoInputs   |
                                         ImGuiColorEditFlags_Float      |
                                         ImGuiColorEditFlags_AlphaPreviewHalf);
                
                isChanged |= ImGui::ColorEdit4(names, (float *)value_ptr(pSys->getUPlanes().boundaryColor[i]),flags);
                       
                vec3 pt = plane;

                // Gizmo
                ///////////////////////////////////////////
                names[7] = 'A' + i;
                imguiGizmo::resizeSolidOf(1.5f);
                imguiGizmo::setDirectionColor(ImGui::ColorConvertU32ToFloat4(0xff000000 + 0xff << (i<<3) ), ImGui::ColorConvertU32ToFloat4(0x809f9f9f));
                isChanged |= ImGui::gizmo3D(names, pt, w, imguiGizmo::modeDirPlane);
                imguiGizmo::restoreSolidSize();
                imguiGizmo::restoreDirectionColor();

                ImGui::PushItemWidth(w-57);

                names[7] = 'G' + i;
                isChanged |= ImGui::DragFloat(names, &dist, .001f, 0.0f, 0.0f, "dist %.3f");
                ImGui::PopItemWidth();

                reset[9] = 'A'+i;
                ImGui::SameLine(w-52);
                if(ImGui::Button(reset))
                    pSys->setClippingPlane(vec4(i==0 ? 1.f : 0.f, i==1 ? 1.f : 0.f, i==2 ? 1.f : 0.f, 0.f), i);

                if(isChanged) {
                    pt = inverse(m) * pt;
                    pSys->setClippingPlane(vec4(normalize(pt), dist), i);
                }
            }

           //ImGui::NewLine();
        }
        ImGui::PushItemWidth(w);
        float f = pSys->getUPlanes().thickness;
        if(ImGui::DragFloat("##thickness", &f, .0001f, 0.0f, FLT_MAX, "boundThick. %.4f")) pSys->getUPlanes().thickness = f;
        ImGui::PopItemWidth();
        bool b = pSys->getUPlanes().additiveSpace;
        if(ImGui::Checkbox("Additive Space", &b)) pSys->getUPlanes().additiveSpace = b;

    }
    ImGui::End();

}

void mainImGuiDlgClass::view()
{

    const float wndSizeX = fontSize * fontZoom * 12.f; // 26 char * .5 (fontsize/2);
    const int posH = 0;
#if !defined(GLCHAOSP_LIGHTVER)
    const int numItems = 9;
#else
    const int numItems = 7;
#endif


    ImGui::SetNextWindowSize(ImVec2(wndSizeX, ImGui::GetFrameHeightWithSpacing()*numItems), ImGuiCond_Always);
    ImGui::SetNextWindowPos(ImVec2(theApp->GetWidth()-190-wndSizeX, posH), ImGuiCond_FirstUseEver);


    if(ImGui::Begin(getTitle(),  NULL ,ImGuiWindowFlags_NoResize)) {

        const float w = ImGui::GetContentRegionAvail().x;
        const float wButt = w;

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
#if !defined(GLCHAOSP_LIGHTVER)

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
#endif
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
    paletteDlg.rePosWndByMode(x, y);
    infoDlg.rePosWndByMode(x, y);
    viewSettingDlg.rePosWndByMode(x, y);
    fastViewDlg.rePosWndByMode(x, y);
    clippingDlg.rePosWndByMode(x, y);
    particleEditDlg.rePosWndByMode(x, y);
#if !defined(GLCHAOSP_LIGHTVER)
    progSettingDlg.rePosWndByMode(x, y);
    dataDlg.rePosWndByMode(x, y);
    rePosWndByMode(x, y);
#endif

}

#ifdef __EMSCRIPTEN__

extern bool g_JustTouched[5];
extern float g_touch_x, g_touch_y;

#endif
void mainImGuiDlgClass::postRenderImGui()
{
    if(visible()) {
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


#ifdef GLAPP_IMGUI_VIEWPORT
    // Update and Render additional Platform Windows
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
#endif
    }
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
    if(visible()) {

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
#ifdef __EMSCRIPTEN__
        if(theApp->isTabletMode()) theApp->getEmsDevice().imGuiUpdateTouch();
#endif
        ImGui::NewFrame();

        aboutDlg.view();
        attractorDlg.view();
        particlesDlg.view();
        paletteDlg.view();
        imGuIZMODlg.view();
        viewSettingDlg.view();
        infoDlg.view();
        fastViewDlg.view();
        particleEditDlg.view();
        clippingDlg.view();
        if(attractorsList.get()->dtType() && attractorsList.slowMotion()) cockpitDlg.view();
#if !defined(GLCHAOSP_LIGHTVER)
        progSettingDlg.view();
        dataDlg.view();
#endif
        view();

        ImGui::Render();
    }

}
