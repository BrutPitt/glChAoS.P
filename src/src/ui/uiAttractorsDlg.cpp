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
#include <limits>

#include "../glApp.h"
#include "../glWindow.h"
#include "../attractorsBase.h"

#include "../ShadersClasses.h"

#include <IconsFontAwesome/IconsFontAwesome.h>

#include <imguiControls.h>
#include <imGuIZMOquat.h>

#include "uiMainDlg.h"

bool colCheckButton(bool b, const char *s, const float sz=0);

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

    const float w = ImGui::GetContentRegionAvail().x;
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

    ImGui::DragInt("##indici0", &idx0, .02, 0, 7, str[idx0]); ImGui::SameLine();
    ImGui::DragInt("##indici1", &idx1, .02, 0, 7, str[idx1]); ImGui::SameLine();
    ImGui::DragInt("##indici2", &idx2, .02, 0, 7, str[idx2]); ImGui::SameLine();
    ImGui::DragInt("##indici3", &idx3, .02, 0, 7, str[idx3]); 
    ImGui::PopItemWidth();

    //ImGui::NewLine();

    fractalIIMBase::additionalDataCtrls();
}

#define GLCHAOSP_FEATURE_WIP
void ftTools()
{
    const float halfW = (ImGui::GetContentRegionAvail().x)*.5-ImGui::GetStyle().ItemSpacing.x;
    bool b = tfSettinsClass::tfMode();
    if(colCheckButton(b , b ? " multiDot emitter "  ICON_FA_CHECK_SQUARE_O " ": " multiDot emitter " ICON_FA_SQUARE_O " ",halfW)) {
        attractorsList.getThreadStep()->stopThread();
        tfSettinsClass::tfMode(b ^ 1);
        tfSettinsClass::cockPit(false);
        const enumEmitterEngine ee = tfSettinsClass::tfMode() ? enumEmitterEngine::emitterEngine_transformFeedback : enumEmitterEngine::emitterEngine_staticParticles;

        theWnd->getParticlesSystem()->changeEmitter(ee);

        attractorsList.getThreadStep()->restartEmitter();
        attractorsList.get()->initStep();
        attractorsList.getThreadStep()->startThread();
    }

    if(tfSettinsClass::tfMode()) {
        //ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetStyle().ItemSpacing.x);
/*
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x*.5);

        int DpS = attractorsList.getSlowMotionDpS();
        if(ImGui::DragInt("##DpS_", &DpS, 1.0f, 1, INT32_MAX, "dots/s %d")) attractorsList.setSlowMotionDpS(DpS);
        ImGui::PopItemWidth();

        ImGui::SameLine();
*/
        //ImGui::SetCursorPosX()
        bool s = theDlg.cockpitDlg.visible();
        if(colCheckButton(s , s ? " settings "  ICON_FA_CHECK_SQUARE_O " " : " settings " ICON_FA_SQUARE_O " ",halfW)) theDlg.cockpitDlg.visible(s^1);
        ImGui::SameLine();
        bool cP = tfSettinsClass::cockPit();
        if(colCheckButton(cP , cP ? " cockPit "  ICON_FA_CHECK_SQUARE_O " " : " cockPit " ICON_FA_SQUARE_O " ", halfW)) tfSettinsClass::cockPit(cP ^ 1);
/*
        int maxDots = pSys->getSlowMotionMaxDots();
        if(ImGui::DragInt("##maxDots", &maxDots, 10.0f, 0, INT32_MAX, "dots# %d")) pSys->setSlowMotionMaxDots(maxDots);
*/
        
#ifdef GLCHAOSP_FEATURE_WIP_
        bool cP = pSys->cockPit();
        if(colCheckButton(cP , cP ? " cockPit "  ICON_FA_CHECK_SQUARE_O " " : " cockPit " ICON_FA_SQUARE_O " ")) pSys->cockPit(cP^1);
        if(pSys->cockPit()) {
            ImGui::SameLine();
            bool s = theDlg.cockpitDlg.visible();
            if(colCheckButton(s , s ? " settings "  ICON_FA_CHECK_SQUARE_O " " : " settings " ICON_FA_SQUARE_O " ")) theDlg.cockpitDlg.visible(s^1);
        }
#else
        //ImGui::AlignTextToFramePadding();
        //ImGui::NewLine();
#endif
    } else {
        ImGui::AlignTextToFramePadding();
        ImGui::NewLine();
    }
}
/*
void PopCorn4DType::additionalDataCtrls()
{
    ftTools();

}
*/
void attractorDtType::additionalDataCtrls()
{
        
    //ImGui::NewLine();
    ImGui::SameLine();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY()-ImGui::GetFrameHeightWithSpacing()); //repos previous line
    particlesSystemClass *pSys = theWnd->getParticlesSystem();

    ftTools();

    ImGui::NewLine();
    headerAdditionalDataCtrls();

    float f = dtStepInc;
    if(ImGui::DragFloat("##dtI", &f, .000001f, 0.0, 1.0, "dt: %.8f",1.0f)) { 
        dtStepInc = f;
        if(!pSys->getEmitter()->isEmitterOn()) pSys->getEmitter()->setEmitterOn();
    }
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

#if !defined(GLAPP_DISABLE_DLA)
void dla3D::additionalDataCtrls()
{
    ImGui::NewLine();
    headerAdditionalDataCtrls(2);
        
    ImGui::DragInt("##stub", &m_Stubbornness, .05, 0, 99, "Trials %02d");
    ImGui::SameLine();
    int i = int(m_Stickiness * 100.f);
    if(ImGui::DragInt("##perc", &i, .5, 1, 100, "Chance %02d%%")) m_Stickiness = float(i)/100.f;

    ImGui::PopItemWidth();
}
#endif

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
#ifdef GLCHAOSP_LIGHTVER
    const int szX = 520, szY = 200;    
#else
    const int szX = 600, szY = 270;    
#endif
    ImGui::SetNextWindowSize(ImVec2(szX, szY), ImGuiCond_FirstUseEver);
    {
        int w,h; glfwGetWindowSize(theApp->getGLFWWnd(), &w, &h);
        ImGui::SetNextWindowPos(ImVec2(w-szX, h-szY), ImGuiCond_FirstUseEver);
    }
    if(ImGui::Begin(getTitle(), &isVisible)) {

        ImGui::Columns(2);

#ifdef GLCHAOSP_LIGHTVER
        const float szReg = .3f;
#else
        const float szReg = .25f;
#endif
        if(firstTime) { ImGui::SetColumnWidth(0, ImGui::GetWindowContentRegionWidth()*szReg); firstTime = false; }

        // left
        ImGui::BeginGroup(); 
            const float wGrp = ImGui::GetContentRegionAvail().x;
#if !defined(GLCHAOSP_LIGHTVER)
    const int sizeLeft = (ImGui::GetFrameHeightWithSpacing()*3+border*6);
#else
    const int sizeLeft = ImGui::GetFrameHeightWithSpacing()+border;
#endif
            ImGui::BeginChild("List", ImVec2(wGrp,-sizeLeft));            
            
                ImGuiStyle& style = ImGui::GetStyle();
                ImGui::PushStyleColor(ImGuiCol_Header       ,style.Colors[ImGuiCol_PlotHistogram       ]); 
                ImGui::PushStyleColor(ImGuiCol_HeaderHovered,style.Colors[ImGuiCol_PlotHistogramHovered]); 
                ImGui::PushStyleColor(ImGuiCol_HeaderActive ,style.Colors[ImGuiCol_CheckMark           ]); 

                
                const int idx = attractorsList.getSelection();                
                if(!theDlg.selectableScrolled()) { ImGui::SetScrollY(idx * (ImGui::GetFontSize()+style.FramePadding.y)); theDlg.selectableScrolled(true); }
                
                char s[16];
                for (int i = 0; i < attractorsList.getList().size(); i++)   {
                    sprintf(s,"##%d",i);
                    bool selected = idx == i;

                    if (ImGui::Selectable(s, selected))  attractorsList.setSelection(i);
                    
                    ImGui::SameLine(); ImGui::TextColored((const ImVec4 &)attractorsList.getColorGraphChar(i), attractorsList.getGraphChar(i).c_str());
                    ImGui::SameLine(); ImGui::SetCursorPosX(ImGui::GetCursorPosX()+3); ImGui::Text(attractorsList.getDisplayName(i).c_str());
                }
                

                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();

            ImGui::EndChild();

            ImGui::BeginChild("load_save",ImVec2(wGrp,0)); {
               
#if !defined(GLCHAOSP_LIGHTVER)
                const float butt = wGrp * .5 - 1;
                //ImGui::PushItemWidth(w);
                ImGui::SetCursorPosY(ImGui::GetCursorPosY()+border);
                if(ImGui::Button(ICON_FA_LEVEL_DOWN " Imprt",ImVec2(butt,0))) { loadAttractorFile(true); }
                ImGui::SameLine();
                if(ImGui::Button( "Exprt " ICON_FA_LEVEL_UP,ImVec2(butt,0))) { saveAttractorFile(true); }
                
                ImGui::SetCursorPosY(ImGui::GetCursorPosY()+border*2);

                if(ImGui::Button(ICON_FA_FOLDER_OPEN_O " Load ", ImVec2(butt,0))) { loadAttractorFile(); }
                ImGui::SameLine();
                if(ImGui::Button( " Save " ICON_FA_FLOPPY_O, ImVec2(butt,0))) { saveAttractorFile(); }

                ImGui::SetCursorPosY(ImGui::GetCursorPosY()+border*2);
#endif
                const bool b = theDlg.getfastViewDlg().visible();
                if(colCheckButton(b, b ? ICON_FA_SEARCH " MORE " ICON_FA_LONG_ARROW_RIGHT " savedList"  : ICON_FA_SEARCH " MORE " ICON_FA_LONG_ARROW_RIGHT " savedList", wGrp)) 
                    { theDlg.getfastViewDlg().visible(b^1); }

                //ImGui::PopItemWidth();

            } ImGui::EndChild();

            //style.WindowPadding.x = oldWindowPadding;

        ImGui::EndGroup();
        ImGui::NextColumn();

        ImGui::SameLine();

        // right        
        ImGui::BeginGroup();
            ImGui::BeginChild("Parameters", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()*3-border)); // Leave room for 1 line below us            
                
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
                const float wDn = ImGui::GetContentRegionAvail().x;
                const int buttW = wDn*.2;
                if(!attractorsList.get()->dtType() && !attractorsList.get()->dlaType()) {
                    ImGui::PushItemWidth((wDn-border*3)*.5+.5); //wDn*.5-border
                    ImGui::SetCursorPosX(border);
                    ImGui::DragFloatRange2("##vR", &attractorsList.get()->vMin, &attractorsList.get()->vMax, 
                                                    0.0025f, -10.0f, 10.0f, 
                                                    "Min: %.4f", "Max: %.4f");
                    ImGui::SameLine();
                    ImGui::DragFloatRange2("##kR", &attractorsList.get()->kMin, &attractorsList.get()->kMax, 
                                                    0.0025f, -10.0f, 10.0f, 
                                                    "Min: %.4f", "Max: %.4f");
                    ImGui::PopItemWidth();
                    if(!attractorsList.get()->fractalType()) {

                        ImGui::SetCursorPosX(wDn-border-buttW);
                        if(ImGui::Button(ICON_FA_RANDOM " Generate",ImVec2(buttW,0)))  attractorsList.generateNewRandom();
                    } else { ImGui::AlignTextToFramePadding();  ImGui::NewLine(); }
                } else {
                    ImGui::AlignTextToFramePadding(); ImGui::NewLine();

                    ImGui::AlignTextToFramePadding(); ImGui::NewLine();
/*
#ifdef GLCHAOSP_LIGHTVER
                    ImGui::AlignTextToFramePadding(); ImGui::NewLine();
#else
                    ImGui::SetCursorPosX(border);
                    static bool b;
                    if(colCheckButton(b, ICON_FA_RANDOM " Explorer", buttW)) { b^=1; }
#endif
*/
                }

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

    char s[32];
    static int selIdx=-1;
    int valIdx = selIdx;   

    int idx = 0;
    static const char idCell[] = "XYZW";
    AttractorBase *att = attractorsList.get();

    auto populateData = [&] (const float colWidth, int elem, int nCol, float minVal, float maxVal, const int typeVal) {
        const float pos = ImGui::GetCursorPosX();

        for(int i=0; i<elem ; i++) {
                    
            ImGui::PushStyleColor(ImGuiCol_FrameBg,i&1 ? rowD : rowP); 

            float szItem;
            if(nCol != 1) {
                sprintf(s,"##c_%03d",i); // vec3 k vals
            } else {
                if(typeVal==AttractorBase::attLoadKtVal) sprintf(s,"##c_%03d",   i );    // float k vals
                else                                     sprintf(s,"##c_%c", idCell[i]); // fractals XYZW
            }

            ImGui::SetCursorPosX(pos);

            ImGui::AlignTextToFramePadding();
            ImGui::TextDisabled(&s[4]); ImGui::SameLine();
            
            const float curPos = ImGui::GetCursorPosX();
            //ImGui::SetCursorPosX(curPos);

            szItem = nCol != 1 ? (colWidth-(curPos-pos)- style.ItemInnerSpacing.x)*.33333f : 
                                 (colWidth-(curPos-pos));

            ImGui::PushItemWidth(szItem);

            auto pushColorSelectedCell = [&] () {
                ImGui::PushStyleColor(ImGuiCol_FrameBg       ,style.Colors[ImGuiCol_PlotHistogram       ]); 
                ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,style.Colors[ImGuiCol_PlotHistogramHovered]); 
                ImGui::PushStyleColor(ImGuiCol_FrameBgActive ,style.Colors[ImGuiCol_CheckMark           ]); 
            };
            auto popColorSelectedCell = [&] () {
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
                ImGui::PopStyleColor();
            };

            auto innerLoop3 = [&] () {                        
                for(int j = 0; j < 3; j++, idx++)
                {
                    s[3] = (typeVal ? '1' : '5') + j; //name different for single column
                    if(idx==selIdx) pushColorSelectedCell();
                        
                    float f = att->getValue(i,j,typeVal);
                    if(ImGui::DragFloatEx(s, (float *) &f, .0001, minVal, maxVal, "%.7f",1.0f,ImVec2(.93,0.5))) {
                         att->setValue(i, j, typeVal, f); valIdx=idx; 
                         if(!theWnd->getParticlesSystem()->getEmitter()->isEmitterOn()) 
                             theWnd->getParticlesSystem()->getEmitter()->setEmitterOn(); 
                    }
                    ImGui::SameLine(0, style.ItemInnerSpacing.x);

                    if(idx==selIdx) popColorSelectedCell();
                }
                ImGui::NewLine();
            };
            auto innerLoop = [&] () {                        
                 s[3] = typeVal ? '1' : '5'; //name different for single column
                if(idx==selIdx) pushColorSelectedCell();
                
                //const bool test4D = !typeVal && att->getPtSize()==AttractorBase::attPt4D && i==3;
                float f =  att->getValue(i,typeVal);
                if(ImGui::DragFloatEx(s, (float *) &f, .0001, minVal, maxVal, "%.7f",1.0f,ImVec2(.93,0.5))) {
                    att->setValue(i, typeVal, f); 
                    valIdx=idx; 
                    if(!theWnd->getParticlesSystem()->getEmitter()->isEmitterOn()) 
                        theWnd->getParticlesSystem()->getEmitter()->setEmitterOn(); 
                }
                if(idx==selIdx) popColorSelectedCell();
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
        const float wCl = ImGui::GetContentRegionAvail().x;

        const int nElem = att->getNumElements(AttractorBase::attLoadPtVal);
        if(nElem > 1) { columnsHeader(wCl, "X", "Y", "Z"); populateData(wCl, nElem, 3, att->getInputVMin(), att->getInputVMax(), 0); }
        else          { columnHeader(wCl, "Start Pt"); 
                        populateData(wCl, att->getPtSize(), 1, att->getInputVMin(), att->getInputVMax(), 0); }
    }

    ImGui::SetCursorPosY(posY);

    ImGui::NextColumn();

    {
        const float wCl = ImGui::GetContentRegionAvail().x;

        const int nElem = att->getNumElements(AttractorBase::attLoadKtVal);
        const int nCol = att->getKType() == AttractorBase::attHaveKVect ? 3 : 1;
        if(nCol > 1) columnsHeader(wCl, "Kx", "Ky", "Kz");
        else         columnHeader(wCl, " K Vals ");

        populateData(wCl, nElem, nCol, att->getInputKMin(), att->getInputKMax(), 1);
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


void fastViewDlgClass::view()
{
    if(!isVisible) return;

#if !defined(GLCHAOSP_LIGHTVER)
    const int posH = 0;
    const int szH = 365+7*ImGui::GetFrameHeightWithSpacing();
    const int posW = 190;
#else
    const int posH = 385;
    const int szH = 365;
    const int posW = 0;
#endif
    const int szW = 180;
    ImGui::SetNextWindowPos(ImVec2(theApp->GetWidth()-szW-posW,posH ), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(szW, szH), ImGuiCond_FirstUseEver);

    //ImGui::SetNextWindowSize(ImVec2(150, 400), ImGuiCond_FirstUseEver);
    //ImGui::SetNextWindowPos(ImVec2(theApp->GetWidth()-330,0), ImGuiCond_FirstUseEver);

    if(ImGui::Begin(getTitle(), &isVisible)) {
            int idx = theApp->selectedListQuickView();

            ImGui::BeginChild("chaoticA");  //,ImVec2(ImGui::GetContentRegionAvailWidth(), -ImGui::GetFrameHeightWithSpacing())
                for (int i = 0; i < theApp->getListQuickView().size(); i++)   {
                    if (ImGui::Selectable(theApp->getListQuickView().at(i).c_str(), idx == i)) {
                        theApp->loadQuikViewSelection(i);
                        theDlg.needToScrooll();
                    }
                }

            ImGui::EndChild();            

    } ImGui::End();

}
