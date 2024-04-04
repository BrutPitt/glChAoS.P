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
#include "../glApp.h"
#include "../glWindow.h"

#include <imguiControls.h>

void pushColorButton();
void popColorButton();

bool colCheckButton(bool b, const char *s, const float sz=0);

void drawMinMaxButtons()
{
    const float wDn = ImGui::GetContentRegionAvail().x;

    ImGui::PushItemWidth((wDn-DLG_BORDER_SIZE*3)*.5+.5); //wDn*.5-border
    ImGui::SetCursorPosX(DLG_BORDER_SIZE);
    ImGui::DragFloatRange2("##vR", &attractorsList.get()->vMin, &attractorsList.get()->vMax,
                                    0.0025f, -10.0f, 10.0f,
                                    "Min: %.4f", "Max: %.4f");
    ImGui::SameLine();
    float min = attractorsList.get()->getKMin(), max = attractorsList.get()->getKMax();
    if(ImGui::DragFloatRange2("##kR", &min, &max, 0.0025f, -10.0f, 10.0f, "Min: %.4f", "Max: %.4f")) {
        if(attractorsList.get()->fractalType()) {
            if(min != attractorsList.get()->getKMin())
                attractorsList.get()->kMin = attractorsList.get()->kMax = min;
            else
                attractorsList.get()->kMin = attractorsList.get()->kMax = max;
        } else {
            attractorsList.get()->kMin = min;
            attractorsList.get()->kMax = max;
        }
    }

    ImGui::PopItemWidth();
}

void drawGenerateButton()
{
    const float wDn = ImGui::GetContentRegionAvail().x;
    const int buttW = wDn*.2;

    ImGui::SetCursorPosX(wDn-DLG_BORDER_SIZE-buttW);
    if(ImGui::Button(ICON_FA_RANDOM " Generate",ImVec2(buttW,0)))  attractorsList.generateNewRandom();
}



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
    const float wButt = (w) *.5 / float(numControls) - border; // dim/5 * 2

    ImGui::SameLine();

    ImGui::PushItemWidth(wButt);
    //ImGui::SetCursorPosX(INDENT(border));
    ImGui::AlignTextToFramePadding();

    ImGui::SetCursorPosX(DLG_BORDER_SIZE);        
}

void fractalIIMBase::drawDepthSkipButtons()
{
    ImGui::NewLine();

    headerAdditionalDataCtrls(2);

    {
        int i = maxDepth;
        if(ImGui::DragInt("##dpt", &i, 1, skipTop, 35000, "Depth: %03d")) {
            if(i<skipTop) skipTop = i-1;
            maxDepth = i;
        }
    }

    ImGui::SameLine();
    {
        int i = skipTop;
        if(ImGui::DragInt("##skp", &i, .25, 0, maxDepth, "Skip: %03d")) skipTop = i>=maxDepth ? maxDepth-1 : i;
    }
    ImGui::SameLine();
    ImGui::PopItemWidth();
}

void fractalIIMBase::additionalDataCtrls()
{
    drawMinMaxButtons();
    ImGui::AlignTextToFramePadding();  ImGui::NewLine();

#ifdef SHOW_BETA_FEATURES
    ImGui::SameLine(ImGui::GetContentRegionAvail().x*.5 + ImGui::GetStyle().FramePadding.x);
    {
        bool b = ifsActive();
        if(ImGui::Checkbox("IFS ", &b)) ifsParam.active(b), ifsPoint.active(b);
    }
    if(ifsActive()) {
        ImGui::SameLine();
        const float w = (ImGui::GetContentRegionAvail().x - DLG_BORDER_SIZE) *.5 ;
        {
        bool b = ifsParam.dlgActive();
        if(colCheckButton(b , b ? " Param " ICON_FA_CHECK_SQUARE_O " " : " Param " ICON_FA_SQUARE_O " ",w)) ifsParam.dlgActive(b^1);
        } ImGui::SameLine(); {
        bool b = ifsPoint.dlgActive();
        if(colCheckButton(b , b ? " Point " ICON_FA_CHECK_SQUARE_O " " : " Point " ICON_FA_SQUARE_O " ",w)) ifsPoint.dlgActive(b^1);
        }
    }
    theDlg.getIFSDlgParam().visible(ifsActive() && ifsParam.dlgActive());
    theDlg.getIFSDlgPoint().visible(ifsActive() && ifsPoint.dlgActive());
#endif
    drawDepthSkipButtons();

}

void AttractorBase::additionalDataCtrls() 
{
    drawMinMaxButtons();
    drawGenerateButton();

    ImGui::NewLine();
}

void fractalIIM_Nth::additionalDataCtrls()
{
    drawMinMaxButtons();
    ImGui::AlignTextToFramePadding();  ImGui::NewLine();


    //ImGui::NewLine();

    headerAdditionalDataCtrls(2);

    {
        static int oldN = degreeN;
        int i = degreeN;
        if(ImGui::DragInt("##dgr", &i, .1, -100, 100, "Dgree: %03d")) {
            //degreeN = i>-2 && i<3 ? (degreeN = oldN > i ? -2 : 3) : i; 
            degreeN = i>-2 && i<2 ? (degreeN = oldN > i ? -2 : 2) : i;
            oldN = i;
        }
        //ImGui::SameLine();
    }
    ImGui::PopItemWidth();

    drawDepthSkipButtons();
}

void BicomplexJExplorer::additionalDataCtrls()
{
    drawMinMaxButtons();
    ImGui::AlignTextToFramePadding();  ImGui::NewLine();

    headerAdditionalDataCtrls(4);
    ImGui::SameLine(DLG_BORDER_SIZE);

    ImGui::DragInt("##indici0", &idx0, .02, 0, 7, str[idx0]); ImGui::SameLine();
    ImGui::DragInt("##indici1", &idx1, .02, 0, 7, str[idx1]); ImGui::SameLine();
    ImGui::DragInt("##indici2", &idx2, .02, 0, 7, str[idx2]); ImGui::SameLine();
    ImGui::DragInt("##indici3", &idx3, .02, 0, 7, str[idx3]); 
    ImGui::PopItemWidth();

    drawDepthSkipButtons();
}

#define GLCHAOSP_FEATURE_WIP
void tfTools()
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

    ImGui::SameLine();
    if(tfSettinsClass::tfMode()) {
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

void volumetricFractals::additionalDataCtrls()
{

    const float halfW = ImGui::GetContentRegionAvail().x*.5 - ImGui::GetStyle().ItemSpacing.x;

    ImGui::PushItemWidth(halfW);
    ImGui::DragFloat3("##minVol", value_ptr(sMin), .01f);
    ImGui::SameLine();
    ImGui::DragFloat3("##maxVol", value_ptr(sMax), .01f);

    //ImGui::AlignTextToFramePadding();   ImGui::NewLine();

    ImGui::DragIntRange2("##range", &plotRange.x, &plotRange.y, .2, 0, maxIter);
    //ImGui::Combo("skip dots", (int *) &whatPlot, "plot all dots\0" "skip convergents\0" "skip divergents\0");

    //ImGui::Checkbox(" skip conv", &skipConvergent);
    ImGui::PopItemWidth();


    ImGui::NewLine();

    headerAdditionalDataCtrls(2);

    {
        int i = maxIter;
        if(ImGui::DragInt("##itr", &i, 1, plotRange.x, 35000, "Iter: %03d")) {
            if(i<plotRange.x) plotRange.x = i-1;
            maxIter = i;
        }
    }

    ImGui::SameLine();
    ImGui::PopItemWidth();
}

void attractorDtType::additionalDataCtrls()
{
    ImGui::AlignTextToFramePadding(); ImGui::NewLine();
    ImGui::AlignTextToFramePadding(); ImGui::NewLine();

    ImGui::SameLine();
    ImGui::SetCursorPosY(ImGui::GetCursorPosY()-ImGui::GetFrameHeightWithSpacing()); //repos previous line
    particlesSystemClass *pSys = theWnd->getParticlesSystem();

#if !defined(GLCHAOSP_NO_TF)
    tfTools();
#endif

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
    drawMinMaxButtons();
    drawGenerateButton();

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
    ImGui::AlignTextToFramePadding(); ImGui::NewLine();
    ImGui::AlignTextToFramePadding(); ImGui::NewLine();
    
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
    drawMinMaxButtons();
    drawGenerateButton();

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
#if defined(GLCHAOSP_LIGHTVER) || defined(GLAPP_USES_ES3)
#if !defined(GLCHAOSP_TEST_RANDOM_DISTRIBUTION)
    const int szX = 520, szY = 200;
#else
    const int szX = 340, szY = 260;
#endif
#else
    const int szX = 600, szY = 270;    
#endif
    ImGui::SetNextWindowSize(ImVec2(szX, szY), ImGuiCond_FirstUseEver);
    {
        int w,h; glfwGetWindowSize(theApp->getGLFWWnd(), &w, &h);
#if defined(GLCHAOSP_LIGHTVER) || defined(GLAPP_USES_ES3)
        ImGui::SetNextWindowCollapsed(isCollapsed, ImGuiCond_FirstUseEver);
        if(theWnd->GetWidth()<=1280) {
            ImGui::SetNextWindowPos(ImVec2(270, ImGui::GetFrameHeightWithSpacing()), ImGuiCond_FirstUseEver);
        } else
#endif
            ImGui::SetNextWindowPos(ImVec2(w-szX, h-szY), ImGuiCond_FirstUseEver);
    }
#ifdef GLCHAOSP_TEST_RANDOM_DISTRIBUTION
    if(ImGui::Begin("PRNG functions", &isVisible)) {
#else
    if(ImGui::Begin(getTitle(), &isVisible)) {
#endif

#if !defined(GLCHAOSP_TEST_RANDOM_DISTRIBUTION)
        ImGui::Columns(2);

    #ifdef GLCHAOSP_LIGHTVER
        const float szReg = .3f;
    #else
        const float szReg = .25f;
    #endif

        if(firstTime) { ImGui::SetColumnWidth(0, (ImGui::GetContentRegionAvail() + ImGui::GetCursorScreenPos() - ImGui::GetWindowPos()).x*szReg); firstTime = false; }
#else
        const float szReg = 1.f;
#endif

        // left
        ImGui::BeginGroup(); 
            const float wGrp = ImGui::GetContentRegionAvail().x;
#if !defined(GLCHAOSP_WEBGL)
    const int sizeLeft = (ImGui::GetFrameHeightWithSpacing()*3+border*6);
#else
    const int sizeLeft = ImGui::GetFrameHeightWithSpacing()+border;
#endif
#if !defined(GLCHAOSP_TEST_RANDOM_DISTRIBUTION)
            ImGui::BeginChild("List", ImVec2(wGrp,-sizeLeft));
#else
            ImGui::BeginChild("List");
#endif
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
               
#if !defined(GLCHAOSP_WEBGL)
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
#if !defined(GLCHAOSP_TEST_RANDOM_DISTRIBUTION)
                const bool b = theDlg.getfastViewDlg().visible();
                if(colCheckButton(b, b ? ICON_FA_SEARCH " MORE " ICON_FA_LONG_ARROW_RIGHT " savedList"  : ICON_FA_SEARCH " MORE " ICON_FA_LONG_ARROW_RIGHT " savedList", wGrp)) 
                    { theDlg.getfastViewDlg().visible(b^1); }
#endif
                //ImGui::PopItemWidth();

            } ImGui::EndChild();

            //style.WindowPadding.x = oldWindowPadding;

        ImGui::EndGroup();
#if !defined(GLCHAOSP_TEST_RANDOM_DISTRIBUTION)
        ImGui::NextColumn();

        ImGui::SameLine();
        // right
        ImGui::BeginGroup();
            ImGui::BeginChild("Parameters", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()*3-border)); // Leave room for 1 line below us            
                
                ImGui::Columns(2);               
                
                fillAttractorData();

                ImGui::Columns(1);
                ImGui::Separator();
            ImGui::EndChild();
            ImGui::BeginChild("buttons"); {
                const float wDn = ImGui::GetContentRegionAvail().x;
                const int buttW = wDn*.2;
/*
                if(!attractorsList.get()->dtType() && !attractorsList.get()->dlaType()) {
                    drawMinMaxButtons();
                    if(!attractorsList.get()->fractalType()) {
                        drawGenerateButton();
                    } else { ImGui::AlignTextToFramePadding();  ImGui::NewLine(); }
                } else {
                    ImGui::AlignTextToFramePadding(); ImGui::NewLine();

                    ImGui::AlignTextToFramePadding(); ImGui::NewLine();
                }
*/
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
#endif
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

            auto resetEmitter = [&] () {
                auto *emitter = theWnd->getParticlesSystem()->getEmitter();
                if(!emitter->isEmitterOn()) emitter->setEmitterOn();
                if(emitter->restartCircBuff()) { attractorsList.restart(); }
            };

            auto innerLoop3 = [&] () {                        
                for(int j = 0; j < 3; j++, idx++)
                {
                    s[3] = (typeVal ? '1' : '5') + j; //name different for single column
                    if(idx==selIdx) pushColorSelectedCell();
                        
                    float f = att->getValue(i,j,typeVal);
                    if(ImGui::DragFloatEx(s, (float *) &f, .0001, minVal, maxVal, "%.7f",1.0f,ImVec2(.93,0.5))) {
                         att->setValue(i, j, typeVal, f); valIdx=idx;
                         resetEmitter();

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
                    resetEmitter();
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


void ifsDlgClass::view(ifsBaseClass *ifs)
{
    if(!visible() || ifs==nullptr) return;

#ifdef GLCHAOSP_LIGHTVER
    const int szX = 300, szY = 200;
#else
    const int szX = 300, szY = 270;
#endif
    ImGui::SetNextWindowSize(ImVec2(szX, szY), ImGuiCond_FirstUseEver);
    {
        int w,h; glfwGetWindowSize(theApp->getGLFWWnd(), &w, &h);
        ImGui::SetNextWindowPos(ImVec2(w-szX, h-szY), ImGuiCond_FirstUseEver);
    }
    if(ImGui::Begin(getTitle(), &isVisible)) {
        const float w = ImGui::GetContentRegionAvail().x-DLG_BORDER_SIZE*2.f;
        const float realButtW = w * .16667 - DLG_BORDER_SIZE;
        const float buttW = realButtW<48 ? 48 : realButtW>64 ? 64 : realButtW;

        ImGui::SetNextWindowContentSize(ImVec2(buttW*6.f+ImGui::GetFrameHeightWithSpacing(),0.0f));
        ImGui::BeginChild("params", ImVec2(0,-ImGui::GetFrameHeightWithSpacing()), true, ImGuiWindowFlags_HorizontalScrollbar);
        char buttName[16] = "rnd##Ax";
        char checkName[16] = "##chkAx";
        char weightName[16] = "##wgtAx";
        char comboName[16] = "##cmbAx";
        char varsName[16] = "##vrsAAx";
        char ampliName[16] = "##ampAAx";

        ImGui::PushItemWidth(buttW);
        for(int i = 0; i<ifs->getNumTransf(); i++) {
            ampliName[5] = buttName[5] = checkName[5] = comboName[5] = varsName[5] = weightName[5] = 'A'+i; //dynamic names
            if(ImGui::Checkbox(checkName,&ifs->ifsTransforms[i].active))         ifs->rebuildWeight(); ImGui::SameLine();
            if(ImGui::DragFloat(weightName, &ifs->ifsTransforms[i].weight,.001,0.0,FLT_MAX)) ifs->rebuildWeight(); ImGui::SameLine();
            if(ImGui::Button(buttName)) {
                ifs->ifsTransforms[i].variations = vec4(fastPrng64.xoroshiro128p_VNI<float>(),fastPrng64.xoroshiro128p_VNI<float>(),fastPrng64.xoroshiro128p_VNI<float>(),fastPrng64.xoroshiro128p_VNI<float>());
                // ifs->ifsTransforms[i].weight = fastRandom.UNI();
            } ImGui::SameLine();
            for(int j = 0; j<4; j++) {
                varsName[6] = 'A'+j;
                ImGui::DragFloat(varsName, &ifs->ifsTransforms[i].variations[j],.001);
                ImGui::SameLine();
            }
            if(ImGui::Combo(comboName, &ifs->ifsTransforms[i].transfType, tranformsText)) {
                ifs->ifsTransforms[i].variationFunc = variationFuncsArray[ifs->ifsTransforms[i].transfType];
            }
            ImGui::SameLine();
            ImGui::DragFloat(ampliName, &ifs->ifsTransforms[i].variationFactor,.01);
        }
        ImGui::PopItemWidth();



        ImGui::EndChild();

        if(ifs->getTmpTransf()!= ifs->getNumTransf()) pushColorButton();
        bool buttonPressed = ImGui::Button(" Set ");
        if(ifs->getTmpTransf()!= ifs->getNumTransf()) popColorButton();
        ImGui::SameLine();

        int n = ifs->getTmpTransf();
        if(ImGui::DragInt("##transf",&n,.05,1,20,"%d transforms")) ifs->setTmpTransf(n);

        if(buttonPressed && ifs->getTmpTransf()!= ifs->getNumTransf()) {
            ifs->setNumTransf(ifs->getTmpTransf());
            ifs->set();
        }

    } ImGui::End();
}