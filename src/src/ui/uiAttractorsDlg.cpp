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

#include <IconsFontAwesome/IconsFontAwesome.h>

#include <imguiControls.h>
#include <imGuIZMO.h>

#include "uiMainDlg.h"

int AttractorBase::additionalDataDlg()
{
    int retVal = 0;
    if(!dlgAdditionalDataVisible()) return 0;

    const float border = DLG_BORDER_SIZE;

    bool isVisible = dlgAdditionalDataVisible();    
    ImGui::SetNextWindowSize(ImVec2(200, ImGui::GetFrameHeightWithSpacing()*5), ImGuiCond_Once);
    if (ImGui::Begin(displayName.c_str(), &isVisible)) {
        ImVec2 startPos(ImGui::GetCursorPos());

        additionalDataCtrls();

        const ImVec2 sz = ImGui::GetContentRegionAvail();
        const float wHalf = sz.x*.5f;
        const float posB = wHalf+border;
        const float wButt2 = (wHalf-border*2);

        ImGui::SetCursorPos(ImGui::GetCursorStartPos()+ImVec2(border, startPos.y + sz.y));
        if(ImGui::Button("OK",ImVec2(wButt2,0.0))) { dlgAdditionalDataVisible(false); retVal = 1; }
        ImGui::SameLine(posB);
        if(ImGui::Button("Cancel",ImVec2(wButt2,0.0))) { dlgAdditionalDataVisible(false); retVal = -1; }

    } ImGui::End();

    return retVal;
}


inline void headerAdditionalDataCtrls(int numControls = 1)
{

    const float border = DLG_BORDER_SIZE;

    const float w = ImGui::GetContentRegionAvailWidth();
    const float wButt = (w - (border*6)) *.4 / float(numControls); // dim/5 * 2

    ImGui::SameLine();

    ImGui::PushItemWidth(wButt);
    ImGui::SetCursorPosX(INDENT(border));     
    ImGui::AlignTextToFramePadding();

    ImGui::SetCursorPosX(DLG_BORDER_SIZE);        
}

void fractalIIMBase::additionalDataCtrls()
{

    ImGui::NewLine();

    headerAdditionalDataCtrls();

    int i = maxDepth;
    if(ImGui::DragInt("##or", &i, 1, 1, 2000, "Depth: %03d")) maxDepth = i;
    ImGui::SameLine();
    ImGui::PopItemWidth();

}

void AttractorBase::additionalDataCtrls() 
{
    ImGui::NewLine();
}
/*
void fractalIIM_4D::additionalDataCtrls()
{

    headerAdditionalDataCtrls(2);

    float f = dim4D;
    if(ImGui::DragFloat("##4d", &f, .0001, 0.0, 0.0, "4D: %.7f")) dim4D = f;
    ImGui::SameLine();

    int i = maxDepth;
    if(ImGui::DragInt("##or", &i, 1, 1, 2000, "Depth: %03d")) maxDepth = i;
    ImGui::SameLine();
    ImGui::PopItemWidth();

}
*/
void fractalIIM_Nth::additionalDataCtrls()
{

    ImGui::NewLine();

    headerAdditionalDataCtrls(2);

    {
        static int oldN = degreeN;
        int i = degreeN;
        if(ImGui::DragInt("##dgr", &i, .1, -100, 100, "Dgree: %03d")) {
            //degreeN = i>-2 && i<3 ? (degreeN = oldN > i ? -2 : 3) : i; 
            degreeN = i>-2 && i<2 ? (degreeN = oldN > i ? -2 : 2) : i;
            oldN = i;
        }
        ImGui::SameLine();
    }
    {
        int i = maxDepth;
        if(ImGui::DragInt("##or", &i, 1, 1, 2000, "Depth: %03d")) maxDepth = i;
        ImGui::SameLine();
    }
    ImGui::PopItemWidth();

}

void BicomplexJExplorer::additionalDataCtrls()
{


    headerAdditionalDataCtrls(4);
    ImGui::SameLine(DLG_BORDER_SIZE);

    ImGui::DragInt("##indici0", &idx0, .1, 0, 7, str[idx0]); ImGui::SameLine();
    ImGui::DragInt("##indici1", &idx1, .1, 0, 7, str[idx1]); ImGui::SameLine();
    ImGui::DragInt("##indici2", &idx2, .1, 0, 7, str[idx2]); ImGui::SameLine();
    ImGui::DragInt("##indici3", &idx3, .1, 0, 7, str[idx3]); 
    ImGui::PopItemWidth();

    ImGui::NewLine();

    fractalIIMBase::additionalDataCtrls();
}



void attractorDtType::additionalDataCtrls()
{
        
    ImGui::NewLine();
    headerAdditionalDataCtrls();

    float f = dtStepInc;

    if(ImGui::DragFloat("##dtI", &f, .000001f, 0.0, 1.0, "dt: %.8f",1.0f)) dtStepInc = f;
    ImGui::PopItemWidth();
}

void PowerN3D::additionalDataCtrls()
{
    ImGui::NewLine();
    headerAdditionalDataCtrls();        

    ImGui::DragInt("##or", &tmpOrder, .1, 1, 20, "Order: %03d");
    ImGui::SameLine();
    if(ImGui::Button(" Set "/*, ImVec2(wButt,0.0)*/)) {
        attractorsList.getThreadStep()->stopThread();
        
        setOrder(tmpOrder);
        attractorsList.getThreadStep()->restartEmitter();
        attractorsList.getThreadStep()->startThread();

    }
    ImGui::PopItemWidth();
}


void Magnetic::additionalDataCtrls()
{
    ImGui::NewLine();
    headerAdditionalDataCtrls();
        
    ImGui::DragInt("##el", &tmpElements, .1, 2, 999, "Elem: %03d");
    ImGui::SameLine();
    if(ImGui::Button(" Set " /*, ImVec2(wButt,0.0)*/)) {
        attractorsList.getThreadStep()->stopThread();
        
        setElements(tmpElements);
        attractorsList.getThreadStep()->restartEmitter();
        attractorsList.getThreadStep()->startThread();

    }
    ImGui::PopItemWidth();
}


int Magnetic::additionalDataDlg()
{
    if(!dlgAdditionalDataVisible()) return 0;
    
    //ImGui::BeginPopupModal("aaa")
    //getThreadStep()->stopStepThread();

    if(AttractorBase::additionalDataDlg() == 1) {
        attractorsList.getThreadStep()->stopThread();
        
        setElements(tmpElements);
        attractorsList.getThreadStep()->restartEmitter();
        attractorsList.getThreadStep()->startThread();
    }

    return 0;

}

void attractorDlgClass::view() 
{
    if(!visible()) return;

    //ImGuiStyle& style = ImGui::GetStyle();
        
    const float border = DLG_BORDER_SIZE;
    //const float oldWindowPadding = style.WindowPadding.x;
    const int szX = 600, szY = 270;    
    ImGui::SetNextWindowSize(ImVec2(szX, szY), ImGuiCond_FirstUseEver);
    int w,h;
    glfwGetWindowSize(theApp->getGLFWWnd(), &w, &h);
    ImGui::SetNextWindowPos(ImVec2(w-szX, h-szY), ImGuiCond_FirstUseEver);
    if(ImGui::Begin(getTitle(), &isVisible)) {

        ImGui::Columns(2);

        static bool firstTime = true;
        if(firstTime) { ImGui::SetColumnWidth(0, ImGui::GetWindowContentRegionWidth()*.225); firstTime = false; }

        // left
        ImGui::BeginGroup(); 
            const float wGrp = ImGui::GetContentRegionAvailWidth();
#if !defined(GLCHAOSP_LIGHTVER)
    const int sizeLeft = (ImGui::GetFrameHeightWithSpacing()*4+border*5);
#else
    const int sizeLeft = 0;
#endif
            ImGui::BeginChild("List", ImVec2(wGrp,-sizeLeft));            

                for (int i = 0; i < attractorsList.getList().size(); i++)   {
                    //ImGui::SetCursorPosX(border);
                    if (ImGui::Selectable(attractorsList.getDisplayName(i).c_str(), attractorsList.getSelection() == i)) {
                        attractorsList.setSelection(i);
                        //onRestart();
                    }
                }

            ImGui::EndChild();           

#if !defined(GLCHAOSP_LIGHTVER)
            ImGui::BeginChild("load_save",ImVec2(wGrp,0)); {
                const float w = ImGui::GetContentRegionAvailWidth();
                //ImGui::PushItemWidth(w);
                ImGui::SetCursorPosY(ImGui::GetCursorPosY()+border);
                if(ImGui::Button(ICON_FA_FOLDER_OPEN_O " Import",ImVec2(w,0))) { loadAttractorFile(true); }
                if(ImGui::Button(ICON_FA_FLOPPY_O " Export" ,ImVec2(w,0))) { saveAttractorFile(true); }
                
                ImGui::SetCursorPosY(ImGui::GetCursorPosY()+border*3);

                if(ImGui::Button(ICON_FA_FOLDER_OPEN_O "  Load ",ImVec2(w,0))) { loadAttractorFile(); }
                if(ImGui::Button(ICON_FA_FLOPPY_O "  Save " ,ImVec2(w,0))) { saveAttractorFile(); }
                //ImGui::PopItemWidth();

            } ImGui::EndChild();
#endif

            //style.WindowPadding.x = oldWindowPadding;

        ImGui::EndGroup();
        ImGui::NextColumn();

        ImGui::SameLine();

        // right        
        ImGui::BeginGroup();
            ImGui::BeginChild("Parameters", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()*3-border)); // Leave room for 1 line below us            
                
                const float wCl = ImGui::GetContentRegionAvailWidth();
                ImGui::Columns(2);               
                
                
                //ImGui::Separator();
                //ImGui::Text("X"); ImGui::NextColumn(); ImGui::Text("Y"); ImGui::NextColumn(); ImGui::Text("Z"); ImGui::NextColumn();
                //ImGui::Text("Ka"); ImGui::NextColumn(); ImGui::Text("Kb"); ImGui::NextColumn(); ImGui::Text("Kc"); ImGui::NextColumn();

                //ImGuiWindow* w = ImGui::GetCurrentWindow();
                //float sz = w->InnerRect.GetSize().x/2;
                fillAttractorData();

                
                ImGui::Columns(1);
                ImGui::Separator();
            ImGui::EndChild();
            ImGui::BeginChild("buttons"); {
                const float wDn = ImGui::GetContentRegionAvailWidth();
                const int buttW = wDn*.2;
                if(!attractorsList.get()->dtType()) {
                    ImGui::PushItemWidth((wDn-border*3)*.5+.5); //wDn*.5-border
                    ImGui::SetCursorPosX(border);
                    ImGui::DragFloatRange2("##vR", &attractorsList.get()->vMin, &attractorsList.get()->vMax, 
                                                    0.01f, -10.0f, 10.0f, 
                                                    "Min: %.3f", "Max: %.3f");
                    ImGui::SameLine();
                    ImGui::DragFloatRange2("##kR", &attractorsList.get()->kMin, &attractorsList.get()->kMax, 
                                                    0.01f, -10.0f, 10.0f, 
                                                    "Min: %.3f", "Max: %.3f");
                    if(!attractorsList.get()->fractalType()) {
                        ImGui::PopItemWidth();

                        ImGui::SetCursorPosX(wDn-border-buttW);
                        if(ImGui::Button(ICON_FA_RANDOM " Generate",ImVec2(buttW,0)))  attractorsList.generateNewRandom();
                    } else { ImGui::AlignTextToFramePadding();  ImGui::NewLine(); }
                } else {
                    ImGui::AlignTextToFramePadding(); ImGui::NewLine();
                    ImGui::AlignTextToFramePadding(); ImGui::NewLine();
                }

/*
                {
                    if(ImGui::Button("AdditionalData")) attractorsList.get()->dlgAdditionalDataVisible(true);
                    attractorsList.get()->additionalDataDlg();

                }
*/
                //ImGui::NewLine();
                attractorsList.get()->additionalDataCtrls();
                ImGui::SameLine();

                
                ImGui::SetCursorPosX(wDn-(border+buttW)*2);
                if(ImGui::Button(ICON_FA_SHARE_SQUARE_O " ReLoad",ImVec2(buttW,0)))  { 
                    if(theApp->getLastFile().size()>0) loadAttractorFile(false, theApp->getLastFile().c_str());
                    else                               loadAttractorFile();
                }

                ImGui::SameLine(wDn-border-buttW);
                if(ImGui::Button(ICON_FA_REFRESH " Restart",ImVec2(buttW,0))) { 
                    //theWnd->getParticlesSystem()->getTMat()->setView();

                    attractorsList.restart(); //theWnd->getParticlesSystem()->restartEmitter();    

                    //onRestart(); 
                }

            } ImGui::EndChild(); 
        ImGui::EndGroup();
    } ImGui::End();
}
 
void fillAttractorData()
{
const float start = 35;
const float border = 5;
                
//Display headers

    auto columnsHeader = [=] (const float w, auto h1, auto h2, auto h3)
    {
        const float step = (w-border-start)/3.f;
        const float fontW = ImGui::GetFontSize()*.5+.5;
        const float center = step*.5f+fontW;

        ImGui::SetCursorPosX(ImGui::GetCursorPosX()+w-step*2-center); ImGui::TextDisabled(h1); 
        ImGui::SameLine(w-step-center);   ImGui::TextDisabled(h2); 
        ImGui::SameLine(w-center);        ImGui::TextDisabled(h3); 
    };
    auto columnHeader = [=] (const float w, auto h1)
    {
        const float step = (w-border-start);
        const float fontW = ImGui::GetFontSize()*.5+.5;
        const float center = step*.5f+fontW*8;

        ImGui::SetCursorPosX(ImGui::GetCursorPosX()+w-center);        ImGui::TextDisabled(h1); 
    };

    ImGuiStyle& style = ImGui::GetStyle();
                
    const ImVec4 wBG(style.Colors[ImGuiCol_WindowBg]);
    const ImVec4 fBG(style.Colors[ImGuiCol_FrameBg]);
    const ImVec4 rowD(fBG.x*.8f,fBG.y*.8f,fBG.z*.8f,fBG.w*.25f);
    const ImVec4 rowP(fBG.x    ,fBG.y    ,fBG.z    ,fBG.w*1.f);

    static bool sIdx=0; 
                

    char s[10];
    static int selIdx=-1;
    int valIdx = selIdx;   

    int idx = 0;
    static const char *idCell = "XYZW";
    AttractorBase *att = attractorsList.get();

    auto populateData = [&] (auto colWidth, int elem, int pos, int nCol = 1) {
        const int typeVal = ImGui::GetColumnIndex();
        for(int i=0; i<elem ; i++) {
            float szItem;
            if(nCol != 1) {
                sprintf(s,"##c_%03d",i+1);
                szItem = colWidth/3.f - (border + style.ItemInnerSpacing.x * 4.f) ;
            } else {
                sprintf(s,"##c_%c", (typeVal==AttractorBase::attLoadKtVal ? 'A'+i : idCell[i]) );
                szItem = colWidth - (border*2 + style.ItemInnerSpacing.x) ;
            }
                    
            ImGui::PushStyleColor(ImGuiCol_FrameBg,i&1 ? rowD : rowP); 

            ImGui::SetCursorPosX(pos);

            ImGui::AlignTextToFramePadding();
            ImGui::TextDisabled(&s[4]); ImGui::SameLine();                       
            ImGui::SetCursorPosX(ImGui::GetCursorPosX()+border);

            ImGui::PushItemWidth(szItem);

            auto innerLoop3 = [&] () {                        
                for(int j = 0; j < 3; j++, idx++)
                {
                    s[3] = (typeVal ? '1' : '5') + j; //name different for single column
                    if(idx==selIdx) {
                        ImGui::PushStyleColor(ImGuiCol_FrameBg,style.Colors[ImGuiCol_PlotHistogram]); 
                        ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,style.Colors[ImGuiCol_PlotHistogramHovered]); 
                        ImGui::PushStyleColor(ImGuiCol_FrameBgActive,style.Colors[ImGuiCol_CheckMark]); 
                    }
                        
                    float f = att->getValue(i,j,typeVal);
                    if(ImGui::DragFloatEx(s, (float *) &f, .0001, 0.0, 0.0, "%.7f",1.0f,ImVec2(.93,0.5))) 
                        { att->setValue(i, j, typeVal, f); valIdx=idx; }
                    ImGui::SameLine(0, style.ItemInnerSpacing.x);

                    if(idx==selIdx) { 
                        ImGui::PopStyleColor();
                        ImGui::PopStyleColor();
                        ImGui::PopStyleColor();
                    }
                }
                ImGui::NewLine();
            };
            auto innerLoop = [&] () {                        
                  s[3] = typeVal ? '1' : '5'; //name different for single column
                if(idx==selIdx) {
                    ImGui::PushStyleColor(ImGuiCol_FrameBg,style.Colors[ImGuiCol_PlotHistogram]); 
                }
                
                const bool test4D = !typeVal && att->getPtSize()==AttractorBase::attPt4D && i==3;
                float f =  test4D ? att->getDim4D() : att->getValue(i,typeVal);
                if(ImGui::DragFloatEx(s, (float *) &f, .0001, 0.0, 0.0, "%.7f",1.0f,ImVec2(.93,0.5))) {
                    if(test4D) att->setDim4D(f);
                    else       att->setValue(i, typeVal, f); 
                    valIdx=idx; 
                }
                if(idx==selIdx) ImGui::PopStyleColor();
                idx++;
            };

            if(nCol == 1) innerLoop();
            else          innerLoop3();
            ImGui::PopItemWidth();
            ImGui::PopStyleColor();               
        }
    };

    const float posY = ImGui::GetCursorPosY();
    {
        const float wCl = ImGui::GetContentRegionAvailWidth();

        const int nElem = att->getNumElements(AttractorBase::attLoadPtVal);
        if(nElem > 1) { columnsHeader(wCl, "X", "Y", "Z"); populateData(wCl, nElem, border, 3); }
        else          { columnHeader(wCl, "Start Pt"); 
                        populateData(wCl, att->getPtSize(), border, 1); }
    }

    ImGui::SetCursorPosY(posY);

    ImGui::NextColumn();

    {
        const float wCl = ImGui::GetContentRegionAvailWidth();

        const int nElem = att->getNumElements(AttractorBase::attLoadKtVal);
        const int nCol = att->getKType() == AttractorBase::attHaveKVect ? 3 : 1;
        if(nCol > 1) columnsHeader(wCl, "Kx", "Ky", "Kz");
        else         columnHeader(wCl, " K Vals ");

        populateData(wCl, nElem, ImGui::GetCursorPosX()+border, nCol);
    }
/*
    if(attractorsList.get()->getKType() == attractorsList.get()->attHaveKVect )
        populateVect(wCl2, ImGui::GetCursorPosX()+border);
    else
        //populateScalar(wCl2, "##c_%03d", ImGui::GetCursorPosX()+border);
        populateScalar(wCl2, ImGui::GetCursorPosX()+border);
*/
                
    selIdx = valIdx;

}
