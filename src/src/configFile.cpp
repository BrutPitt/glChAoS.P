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
#include <chrono>
#include <array>
#include <vector>
#include <ostream>
                
#include "glApp.h"
#include "glWindow.h"

#define CONFIGURU_IMPLEMENTATION 
#include "libs/configuru/configuru.hpp"

#include "libs/tinyFileDialog/tinyfiledialogs.h"
#include "libs/lodePNG/lodepng.h"

void toggleFullscreenOnOff(GLFWwindow* window);

bool fileExist(const char *filename)
{
 	// Load the file
	ifstream input(filename);
	// Check to see that the file is open
	if (!input.is_open()) return false;
	// Close the input file
	input.close();
    return true;
}



void writeAPalette(const char *filename, int idx)
{
        std::ostringstream sout;

        cmContainerClass cm;

        sout.setf(sout.fixed);
        sout.precision(5);
        const int size = cm.getRGB_CMap3(idx).size()-1;
        std::string indent("    ");
        //sout << "{" << endl;
        //sout << "\"Palette\": \""<< cm.getName(idx) <<"\"," << endl;
        //sout << "\"rgbData\": [ ";
        sout << indent << "\""<< &cm.getName(idx)[3] <<"\": [";
        for(int i=0; i<size; i++) {
            if(!(i%3)) sout << endl << indent << indent;
            sout << cm.getRGB_CMap3(idx).at(i) << ", ";
        }
        sout << cm.getRGB_CMap3(idx).at(size) << " ]";
}

void writePalette(const char *filename, int idx)
{
        std::ostringstream sout;
        cmContainerClass &cm = theWnd->getParticlesSystem()->getColorMapContainer();
        std::string indent("    ");

        sout.setf(sout.fixed);
        sout.precision(5); 

        auto writeSinglePalette = [&] (int idx) {
            const int size = cm.getRGB_CMap3(idx).size()-1;
            sout << "{" << endl;
            sout << indent << "\"Type\"   : \"" << cm.getName(idx) << "\"," << endl;
            sout << indent << "\"Name\"   : \"" << cm.getName(idx) << "\"," << endl;
            sout << indent << "\"rgbData\": [";
            for(int i=0; i<size; i++) {
                if(!(i%3)) sout << endl << indent << indent;
                sout << cm.getRGB_CMap3(idx).at(i) << ", ";
            }
            sout << cm.getRGB_CMap3(idx).at(size) << " ]" << endl;
            sout << "}";
        };

        if(idx<0) {
            const int numPal = cm.elements()-1;
            sout << "[" << endl;
            for(int i=0; i<numPal; i++) {
                writeSinglePalette(i);
                sout << "," <<endl;
            }
            writeSinglePalette(numPal);
            sout << endl;

            sout << "]" << endl;
        } else writeSinglePalette(idx);

        //sout << "}" ;
        std::ofstream ofs_palettes(filename, std::ofstream::out);
        ofs_palettes << sout.str();
        ofs_palettes.close();
}

void loadPalette(Config &cfg, particlesBaseClass *ptr)
{
    if(cfg.has_key("Palette")) {
        auto& c = cfg["Palette"];
        int sel = ptr->getColorMapContainer().addNewPal(c);
        ptr->selectColorMap(sel);
    }
}

void savePalette(Config &cfg, particlesBaseClass *ptr)
{
    auto &c = cfg["Palette"] = Config::object();
    c["Type"    ] = ptr->getColorMap_name();
    c["Name"    ] = ptr->getColorMap_name();
    c["rgbData" ] = Config::array(ptr->getSelectedColorMap_CMap3());
}

void saveParticlesSettings(Config &c, particlesBaseClass *ptr)
{
//Rendering
    c["dstBlendAttrib"  ] = ptr->getDstBlend();
    c["srcBlendAttrib"  ] = ptr->getSrcBlend(); 
    c["DepthState"      ] = ptr->getDepthState();
    c["BlendState"      ] = ptr->getBlendState();
    c["LightState"      ] = ptr->getLightState();
    c["pointSize"       ] = ptr->getSize();
    c["pointSizeFactor" ] = ptr->getPointSizeFactor(); 
    c["clippingDist"    ] = ptr->getClippingDist();
    c["alphaKFactor"    ] = ptr->getAlphaKFactor();
    c["alphaAttenFactor"] = ptr->getAlphaAtten();
    c["alphaSkip"       ] = ptr->getAlphaSkip();   
    c["dotsType"        ] = ptr->getDotType();   


//Colors
    c["ColorVel"        ] = ptr->getCMSettings()->getVelIntensity();
    c["PalInvert"       ] = ptr->getCMSettings()->getReverse();
    c["PalClamp"        ] = ptr->getCMSettings()->getClamp();
    c["PalOffset"       ] = ptr->getCMSettings()->getOffsetPoint();
    c["PalRange"        ] = ptr->getCMSettings()->getRange();
    c["PalH"            ] = ptr->getCMSettings()->getH();
    c["PalS"            ] = ptr->getCMSettings()->getS();
    c["PalL"            ] = ptr->getCMSettings()->getL();

//light
    c["lightShinExp"    ] = ptr->getUData().lightShinExp;   
    c["lightDiffInt"    ] = ptr->getUData().lightDiffInt; 
    c["lightSpecInt"    ] = ptr->getUData().lightSpecInt; 
    c["lightAmbInt"     ] = ptr->getUData().lightAmbInt ;
    c["lightStepMin"    ] = ptr->getUData().sstepColorMin;
    c["lightStepMax"    ] = ptr->getUData().sstepColorMax;

    vector<float> v(3); 
    *((vec3 *)v.data()) = ptr->getUData().lightDir;
    c["lightDir"        ] = Config::array(v);

//glow
    radialBlurClass *glow = ptr->getGlowRender();
    c["glowOn"          ] = glow->isGlowOn();
    c["glowSelect"      ] = glow->getGlowState();
    c["sigma"           ] = glow->getSigma();
    c["sigmaRadX"       ] = glow->getSigmaRadX();
    c["renderInt"       ] = glow->getImgTuning()->getTextComponent();
    c["blurInt"         ] = glow->getImgTuning()->getBlurComponent();
    c["bilatInt"        ] = glow->getImgTuning()->getBlatComponent();
    c["bilatMix"        ] = glow->getImgTuning()->getMixBilateral() ;
    c["mixTexture"      ] = glow->getMixTexture();
    c["glowThreshold"   ] = glow->getThreshold();

//FXAA
    c["fxaaOn"          ] = ptr->getFXAA()->isOn();
    c["fxaaThreshold"   ] = ptr->getFXAA()->getThreshold();
    c["ReductMul"       ] = ptr->getFXAA()->getReductMul();
    c["ReductMin"       ] = ptr->getFXAA()->getReductMin();
    c["Span"            ] = ptr->getFXAA()->getSpan();
//DisplayAdjust
    c["Gamma"           ] = glow->getImgTuning()->getGamma();
    c["Bright"          ] = glow->getImgTuning()->getBright();
    c["Contrast"        ] = glow->getImgTuning()->getContrast();
    c["Exposure"        ] = glow->getImgTuning()->getExposure();
    c["ToneMap"         ] = glow->getImgTuning()->getToneMap();  
    c["ToneMapVal"      ] = glow->getImgTuning()->getToneMap_A();
    c["ToneMapExp"      ] = glow->getImgTuning()->getToneMap_G();
                        
    savePalette(c, ptr);
}


void saveSettings(Config &cfg, particlesSystemClass *pSys)
{
    {
        auto &c = cfg["Render"] = Config::object();

        c["RenderMode"   ] = pSys->getRenderMode();
        c["motionBlur"   ] = pSys->getMotionBlur()->Active();
        c["blurIntensity"] = pSys->getMotionBlur()->getBlurIntensity();
        c["mixingVal"    ] = pSys->getMergedRendering()->getMixingVal();
        c["circBuff"     ] = pSys->getEmitter()->getSizeCircularBuffer();
        c["rstrtCircBuff"] = pSys->getEmitter()->restartCircBuff();
        c["stopCircBuff" ] = pSys->getEmitter()->stopFull();

        vec3 persp(pSys->getTMat()->getPerspAngle(),
                   pSys->getTMat()->getPerspNear() ,
                   pSys->getTMat()->getPerspFar() ); 


        vector<float> v(3); 
        *((vec3 *)v.data())= pSys->getTMat()->getPOV();
        c["camPOV"       ] = Config::array(v);
        *((vec3 *)v.data())= pSys->getTMat()->getTGT();
        c["camTGT"       ] = Config::array(v);
        *((vec3 *)v.data())= vec3(pSys->getTMat()->getPerspAngle(),
                                  pSys->getTMat()->getPerspNear() ,
                                  pSys->getTMat()->getPerspFar() ); 
        c["camPerspective"]= Config::array(v);

        *((vec3 *)v.data())= pSys->getTMat()->getTrackball().getDollyPosition();
        c["camDolly"     ] = Config::array(v);
        *((vec3 *)v.data())= pSys->getTMat()->getTrackball().getPanPosition();
        c["camPan"       ] = Config::array(v);
        *((vec3 *)v.data())= pSys->getTMat()->getTrackball().getRotationCenter();
        c["camRotCent"   ] = Config::array(v);

        vector<float> q(4); 
        *((quat *)q.data())= pSys->getTMat()->getTrackball().getRotation();
        c["camRot"       ] = Config::array(q);



    }
    {
        auto &c = cfg["RenderMode0"] = Config::object();
        saveParticlesSettings(c,pSys->shaderBillboardClass::getPtr());
    }
    {
        auto &c = cfg["RenderMode1"] = Config::object();
        saveParticlesSettings(c,pSys->shaderPointClass::getPtr());
    }
}

void mainGLApp::saveSettings(const char *name) 
{
    configuru::Config cfg = Config::object(); 
    ::saveSettings(cfg, theWnd->getParticlesSystem());

    dump_file(name, cfg, JSON);  

}

void mainGLApp::saveAttractor(const char *name) 
{
    configuru::Config cfg = Config::object(); 
    attractorsList.saveVals(cfg);

    ::saveSettings(cfg, theWnd->getParticlesSystem());

    dump_file(name, cfg, JSON);  
}

void mainGLApp::selectCaptureFolder() {        
    const char *path = tinyfd_selectFolderDialog("Select folder...", getCapturePath().c_str());
    if(path != NULL) { setCapturePath(path); capturePath += "/"; }
}



void getRenderMode(Config &c, particlesBaseClass *ptr)
{

    auto getBlendIdx = [&] (GLuint blendCode) -> int
    {
        for(int i=0; i<ptr->getBlendArrayElements(); i++)
            if(ptr->getBlendArray()[i] == blendCode) return i;
        return 0;
    };
    
//Rendering
    ptr->setDstBlend(       c.get_or("dstBlendAttrib"  , ptr->getDstBlend()       ));
    ptr->setSrcBlend(       c.get_or("srcBlendAttrib"  , ptr->getSrcBlend()       ));
    ptr->setDepthState(     c.get_or("DepthState"      , ptr->getDepthState()     ));
    ptr->setBlendState(     c.get_or("BlendState"      , ptr->getBlendState()     ));
    ptr->setLightState(     c.get_or("LightState"      , ptr->getLightState()     ));
    ptr->setSize(           c.get_or("pointSize"       , ptr->getSize()           ));
    ptr->setPointSizeFactor(c.get_or("pointSizeFactor" , ptr->getPointSizeFactor())); 
    ptr->setClippingDist(   c.get_or("clippingDist"    , ptr->getClippingDist()   ));
    ptr->setAlphaKFactor(   c.get_or("alphaKFactor"    , ptr->getAlphaKFactor()   ));
    ptr->setAlphaAtten(     c.get_or("alphaAttenFactor", ptr->getAlphaAtten()     ));
    ptr->setAlphaSkip(      c.get_or("alphaSkip"       , ptr->getAlphaSkip()      ));
    ptr->setDotType(        c.get_or("dotsType"        , ptr->getDotType()        ));

    ptr->dstBlendIdx(getBlendIdx(ptr->getDstBlend()));
    ptr->srcBlendIdx(getBlendIdx(ptr->getSrcBlend()));

//Colors
    ptr->getCMSettings()->setVelIntensity(c.get_or("ColorVel"  , ptr->getCMSettings()->getVelIntensity()));
    ptr->getCMSettings()->setReverse(     c.get_or("PalInvert" , ptr->getCMSettings()->getReverse()     ));
    ptr->getCMSettings()->setClamp(       c.get_or("PalClamp"  , ptr->getCMSettings()->getClamp()       ));
    ptr->getCMSettings()->setOffsetPoint( c.get_or("PalOffset" , ptr->getCMSettings()->getOffsetPoint() ));
    ptr->getCMSettings()->setRange(       c.get_or("PalRange"  , ptr->getCMSettings()->getRange()       ));
    ptr->getCMSettings()->setH(           c.get_or("PalH"      , ptr->getCMSettings()->getH()           ));
    ptr->getCMSettings()->setS(           c.get_or("PalS"      , ptr->getCMSettings()->getS()           ));
    ptr->getCMSettings()->setL(           c.get_or("PalL"      , ptr->getCMSettings()->getL()           ));

//light
    ptr->getUData().lightShinExp  = c.get_or("lightShinExp"    , ptr->getUData().lightShinExp );
    ptr->getUData().lightDiffInt  = c.get_or("lightDiffInt"    , ptr->getUData().lightDiffInt );
    ptr->getUData().lightSpecInt  = c.get_or("lightSpecInt"    , ptr->getUData().lightSpecInt );
    ptr->getUData().lightAmbInt   = c.get_or("lightAmbInt"     , ptr->getUData().lightAmbInt  );
    ptr->getUData().sstepColorMin = c.get_or("lightStepMin"    , ptr->getUData().sstepColorMin);
    ptr->getUData().sstepColorMax = c.get_or("lightStepMax"    , ptr->getUData().sstepColorMax);

//glow    
    radialBlurClass *glow = ptr->getGlowRender();
    if(c.has_key("glowOn")) { //last version
        glow->setGlowOn(c.get_or("glowOn"  , glow->isGlowOn() ));
        glow->setGlowState( c.get_or("glowSelect"  , glow->getGlowState() ));
    } else {
        if(c.has_key("glowState")) { //first version
            glow->setGlowOn(c.get_or("glowState",false));
            glow->setGlowState(glow->glowType_Blur);
        } else {                  //second version
            int gSel = c.get_or("glowSelect" , glow->getGlowState() );
            glow->setGlowOn(gSel>0);
            glow->setGlowState( gSel>0 ? gSel : glow->glowType_Threshold);
        }
    }
    glow->setSigma(     c.get_or("sigma"       , glow->getSigma()     ));
    glow->setSigmaRadX( c.get_or("sigmaRadX"   , glow->getSigmaRadX() ));
    glow->setMixTexture(c.get_or("mixTexture"  , glow->getMixTexture()));
    glow->setThreshold(c.get_or("glowThreshold", glow->getThreshold()));

    glow->getImgTuning()->setTextComponent(c.get_or("renderInt", glow->getImgTuning()->getTextComponent()));
    glow->getImgTuning()->setBlurComponent(c.get_or("blurInt"  , glow->getImgTuning()->getBlurComponent()));
    glow->getImgTuning()->setBlatComponent(c.get_or("bilatInt" , glow->getImgTuning()->getBlatComponent()));
    glow->getImgTuning()->setMixBilateral (c.get_or("bilatMix" , glow->getImgTuning()->getMixBilateral() ));
//FXAA
    ptr->getFXAA()->activate(    c.get_or("fxaaOn"       , ptr->getFXAA()->isOn()));
    ptr->getFXAA()->setThreshold(c.get_or("fxaaThreshold", ptr->getFXAA()->getThreshold()));
    ptr->getFXAA()->setReductMul(c.get_or("ReductMul"    , ptr->getFXAA()->getReductMul()));
    ptr->getFXAA()->setReductMin(c.get_or("ReductMin"    , ptr->getFXAA()->getReductMin()));
    ptr->getFXAA()->setSpan     (c.get_or("Span"         , ptr->getFXAA()->getSpan     ()));

//DisplayAdjoust
    glow->getImgTuning()->setGamma(    c.get_or("Gamma"     , glow->getImgTuning()->getGamma()    ));
    glow->getImgTuning()->setBright(   c.get_or("Bright"    , glow->getImgTuning()->getBright()   ));
    glow->getImgTuning()->setContrast( c.get_or("Contrast"  , glow->getImgTuning()->getContrast() ));
    glow->getImgTuning()->setExposure( c.get_or("Exposure"  , glow->getImgTuning()->getExposure() ));
    glow->getImgTuning()->setToneMap(  c.get_or("ToneMap"   , glow->getImgTuning()->getToneMap()  ));
    glow->getImgTuning()->setToneMap_A(c.get_or("ToneMapVal", glow->getImgTuning()->getToneMap_A()));
    glow->getImgTuning()->setToneMap_G(c.get_or("ToneMapExp", glow->getImgTuning()->getToneMap_G()));


    loadPalette(c, ptr);
}

void loadSettings(Config &cfg, particlesSystemClass *pSys) 
{
    vec3 retVal;

    auto getVec3_asArray = [&] (Config& c, const char *name) -> bool
    {
        if(c.has_key(name)) {
            vector<float> v;
            for (const Config& e : c[name].as_array()) v.push_back(e.as_float());
            retVal = *((vec3 *)v.data());
            return true;
        }
        return false;
    };

    {
        auto& c = cfg["Render"];

        pSys->setRenderMode(                      c.get_or("RenderMode"   , pSys->getRenderMode()                      ));
        pSys->getMotionBlur()->Active(            c.get_or("motionBlur"   , pSys->getMotionBlur()->Active()            ));
        pSys->getMotionBlur()->setBlurIntensity(  c.get_or("blurIntensity", pSys->getMotionBlur()->getBlurIntensity()  ));
        pSys->getMergedRendering()->setMixingVal( c.get_or("mixingVal"    , pSys->getMergedRendering()->getMixingVal() ));
        pSys->getEmitter()->setSizeCircularBuffer(c.get_or("circBuff"     , pSys->getEmitter()->getSizeCircularBuffer()));
        if(pSys->getEmitter()->getSizeCircularBuffer()>pSys->getEmitter()->getSizeAllocatedBuffer())
            pSys->getEmitter()->setSizeCircularBuffer(pSys->getEmitter()->getSizeAllocatedBuffer());

        pSys->getEmitter()->restartCircBuff(      c.get_or("rstrtCircBuff", pSys->getEmitter()->restartCircBuff()      ));        
        pSys->getEmitter()->stopFull(             c.get_or("stopCircBuff" , pSys->getEmitter()->stopFull()             ));

        if(getVec3_asArray(c, "camPOV"))         pSys->getTMat()->setPOV(retVal);
        if(getVec3_asArray(c, "camTGT"))         pSys->getTMat()->setTGT(retVal);
        if(getVec3_asArray(c, "camPerspective")) pSys->getTMat()->setPerspective(retVal.x, retVal.y, retVal.z);
        if(getVec3_asArray(c, "camDolly"))       pSys->getTMat()->getTrackball().setDollyPosition(retVal);
        if(getVec3_asArray(c, "camPan"))         pSys->getTMat()->getTrackball().setPanPosition(retVal);
        if(getVec3_asArray(c, "camRotCent"))     pSys->getTMat()->getTrackball().setRotationCenter(retVal);

        if(c.has_key("camRot")) {
            vector<float> v;
            for (const Config& e : c["camRot"].as_array()) v.push_back(e.as_float());
            pSys->getTMat()->getTrackball().setRotation(*((quat *)v.data()));
        }
    }
    {
        auto &c = cfg["RenderMode0"];
        particlesBaseClass *ptr = pSys->shaderBillboardClass::getPtr();
        getRenderMode(c, ptr);

        if(getVec3_asArray(c, "lightDir")) ptr->getUData().lightDir = vec4(retVal, 0.f);
    }
    {
        auto &c = cfg["RenderMode1"];
        particlesBaseClass *ptr = pSys->shaderPointClass::getPtr();
        getRenderMode(c,ptr);

        if(getVec3_asArray(c, "lightDir")) ptr->getUData().lightDir = vec4(retVal, 0.f);
    }

}

bool mainGLApp::loadSettings(const char *name) 
{
    if(!fileExist(name)) return false;
    Config cfg = configuru::parse_file(name, JSON);

    ::loadSettings(cfg, theWnd->getParticlesSystem());

    return true;
}

bool mainGLApp::loadAttractor(const char *name) 
{
    if(!fileExist(name)) return false;
    Config cfg = configuru::parse_file(name, JSON);
    if(!attractorsList.loadVals(cfg)) return false;

    ::loadSettings(cfg, theWnd->getParticlesSystem());

    return true;
}

std::ostringstream &buildDatatedFilename(std::ostringstream &out)
{
    out.fill('0'); out.width(2);
    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );        
        out << now->tm_year + 1900 
            << now->tm_mon  + 1 
            << now->tm_mday << "_"
            << now->tm_hour
            << now->tm_min
            << now->tm_sec;                

    return out;
}


void mainGLApp::saveScreenShot(unsigned char *data, int w, int h)  
{        

    std::string filename;
    std::ostringstream out;

    if(screenShotRequest == ScreeShotReq::ScrnSht_FILE_NAME) {    
        char const * patterns[] = { "*.png" };        
        const char *name = saveFile(capturePath.c_str(), patterns, 1);
        filename = name != nullptr ? name : "";
    } else {
        out << "sShot_";    
        buildDatatedFilename(out) << ".png";
        filename  = capturePath + out.str();
    }

    if(filename.length()>0) {
        unsigned error = lodepng_encode24_file(filename.c_str(), data, w, h);
        if(error) std::cerr << "error " << error << ": " << lodepng_error_text(error) << std::endl;
        tinyfd_beep();
    }

    screenShotRequest = ScreeShotReq::ScrnSht_NO_REQUEST;
}

char const *mainGLApp::openFile(const char *startDir, char const * patterns[], int numPattern)  
{
    if(glfwGetWindowMonitor(getGLFWWnd())) toggleFullscreenOnOff(getGLFWWnd());

    //char const * patterns[2] = { "*.chatt", "*.txt" };
    return tinyfd_openFileDialog("Load File...", startDir, numPattern, patterns, NULL, 0);
}


char const *mainGLApp::saveFile(const char *startDir, char const * patterns[], int numPattern)  
{
    if(glfwGetWindowMonitor(getGLFWWnd())) toggleFullscreenOnOff(getGLFWWnd());

    //char const * patterns[2] = { "*.chatt", "*.txt" };
    return tinyfd_saveFileDialog("Save File...", startDir, numPattern, patterns, NULL);
}



void saveSettingsFile()  
{
    bool isOn = theWnd->getParticlesSystem()->getEmitter()->isEmitterOn();

    if(isOn) attractorsList.getThreadStep()->stopThread();

    char const * patterns[] = { "*.cfg", "*.sca" };        
    char const * fileName = theApp->saveFile(RENDER_CFG_PATH, patterns, 2);

    if(fileName!=nullptr) 
        theApp->saveSettings(fileName);

    if(isOn) attractorsList.getThreadStep()->startThread();
}

void loadSettingsFile()  
{
    bool isOn = theWnd->getParticlesSystem()->getEmitter()->isEmitterOn();

    if(isOn) attractorsList.getThreadStep()->stopThread();

    char const * patterns[] = { "*.cfg", "*.sca" };        
    char const * fileName = theApp->openFile(RENDER_CFG_PATH, patterns, 2);

    if(fileName!=nullptr) 
        theApp->loadSettings(fileName);

    if(isOn) attractorsList.getThreadStep()->startThread();
}


void mainGLApp::saveProgConfig()  
{
    const char *filename = GLAPP_PROG_CONFIG;

    configuru::Config cfg = Config::object();

    vector<float> v(4); 

    *((vec4 *)v.data()) = *((vec4 *) &theDlg.getGuiThemeBaseColor());
    cfg["baseColorTheme"      ] = Config::array(v);   
    cfg["selectedColorTheme"  ] = theDlg.getSelectedGuiTheme();
    cfg["fontSize"            ] = theDlg.getFontSize();
    cfg["fontZoom"            ] = theDlg.getFontZoom();
    
    cfg["windowPosX"  ] = getPosX();
    cfg["windowPosY"  ] = getPosY();
    int w, h;
    glfwGetWindowSize(theApp->getGLFWWnd(), &w, &h);
    cfg["windowSizeW" ] = w;
    cfg["windowSizeH" ] = h;

    cfg["vSync" ] = theApp->getVSync();

    cfg["maxParticles" ] = getMaxAllocatedBuffer();
    cfg["capturePath" ] = capturePath;

    dump_file(filename, cfg, JSON);

}

bool mainGLApp::loadProgConfig()  
{
    const char *filename = GLAPP_PROG_CONFIG;

    FILE* fp = fopen(filename, "rb");
    if(fp==nullptr) return false; // config file not exist
    else fclose(fp);

    Config cfg = configuru::parse_file(filename, JSON);

    vec4 retVal;
    auto getVec4_asArray = [&] (Config& c, const char *name) -> bool
    {
        if(c.has_key(name)) {
            vector<float> v;
            for (const Config& e : c[name].as_array()) v.push_back(e.as_float());
            retVal = *((vec4 *)v.data());
            return true;
        }
        return false;
    };



    if(getVec4_asArray(cfg, "baseColorTheme")) theDlg.setGuiThemeBaseColor((ImVec4 *)&retVal);
    theDlg.setSelectedGuiTheme(cfg.get_or("selectedColorTheme", theDlg.getSelectedGuiTheme()));

    theDlg.setFontSize(cfg.get_or("fontSize", theDlg.getFontSize()));
    theDlg.setFontZoom(cfg.get_or("fontZoom", theDlg.getFontZoom()));

    theApp->setPosX(cfg.get_or("windowPosX" , -1));
    theApp->setPosY(cfg.get_or("windowPosY" , -1));

    //const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    //int window_width = mode->width;
    //int window_height = mode->height;

    //if(x>=0 && y>=0 ) glfwSetWindowPos(theApp->getGLFWWnd(), x, y);
    
    width  = cfg.get_or("windowSizeW", INIT_WINDOW_W);
    height = cfg.get_or("windowSizeH", INIT_WINDOW_H);  

    vSync = cfg.get_or("vSync", vSync);

    setMaxAllocatedBuffer(cfg.get_or("maxParticles", getMaxAllocatedBuffer()));

    capturePath = cfg.get_or("capturePath", capturePath);

    return true; // config file exist
}