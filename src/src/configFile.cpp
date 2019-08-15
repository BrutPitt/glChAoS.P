////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2018-2019 Michele Morrone
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
//  This software is distributed under the terms of the BSD 2-Clause license
//  
////////////////////////////////////////////////////////////////////////////////
#include <chrono>
#include <array>
#include <algorithm>
#include <vector>
#include <ostream>

#ifdef _WIN32
    #include <dirent/dirent.h>
#else
    #include <dirent.h>
#endif
                
#include "glApp.h"
#include "glWindow.h"

#define CONFIGURU_IMPLEMENTATION 
#include <configuru/configuru.hpp>

#if !defined(GLCHAOSP_LIGHTVER)
    #include "libs/tinyFileDialog/tinyfiledialogs.h"
    #include "libs/lodePNG/lodepng.h"
#endif

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
    c["dotsSize"        ] = ptr->getDotTex().getIndex();   
    {
        vector<float> v(4); 
        *((vec4 *)v.data()) = ptr->getDotTex().getHermiteVals();
        c["HermiteVals"     ] = Config::array(v);
    }
    c["dotsType"        ] = ptr->getDotTex().getDotType();

    c["ShadowEnabled"      ] = ptr->useShadow();
    c["ShadowBias"         ] = ptr->getShadowBias();
    c["ShadowDarkness"     ] = ptr->getShadowDarkness();
    c["ShadowRadius"       ] = ptr->getShadowRadius();
    c["ShadowGranularity"  ] = ptr->getShadowGranularity();
    c["ShadowAutoLightDist"] = ptr->autoLightDist();


    c["AOenabled"       ] = ptr->useAO();
    c["AOstrong"        ] = ptr->getAOStrong();
    c["AObias"          ] = ptr->getAOBias();
    c["AOradius"        ] = ptr->getAORadius();
    c["AOdarkness"      ] = ptr->getAODarkness();
    c["AOmul"           ] = ptr->getAOMul();
    c["AOmodulate"      ] = ptr->getAOModulate();

    
    c["depthRender"     ] = ptr->postRenderingActive();
    c["dpAdjConvex"     ] = ptr->dpAdjConvex();
    c["dpNormalTune"    ] = ptr->dpNormalTune();


//Colors
    c["ColorInt"        ] = ptr                 ->getColIntensity();
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
    c["lightModel"      ] = ptr->getUData().lightModel;
    c["ggxRoughness"    ] = ptr->getUData().ggxRoughness;
    c["ggxFresnel"      ] = ptr->getUData().ggxFresnel;
    {
        vector<float> v(3); 
        *((vec3 *)v.data()) = ptr->getLightDir();
        c["lightDir"        ] = Config::array(v);
        *((vec3 *)v.data()) = ptr->getUData().lightColor;
        c["lightColor"      ] = Config::array(v);
    }

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

#if !defined(GLCHAOSP_NO_FXAA)
//FXAA
    c["fxaaOn"          ] = ptr->getFXAA()->isOn();
    c["fxaaThreshold"   ] = ptr->getFXAA()->getThreshold();
    c["ReductMul"       ] = ptr->getFXAA()->getReductMul();
    c["ReductMin"       ] = ptr->getFXAA()->getReductMin();
    c["Span"            ] = ptr->getFXAA()->getSpan();
#endif
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
#if !defined(GLCHAOSP_LIGHTVER)
        c["motionBlur"   ] = pSys->getMotionBlur()->Active();
        c["blurIntensity"] = pSys->getMotionBlur()->getBlurIntensity();
        c["mixingVal"    ] = pSys->getMergedRendering()->getMixingVal();
#endif
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
#if !defined(GLCHAOSP_LIGHTVER)
    {
        auto &c = cfg["RenderMode0"] = Config::object();
        saveParticlesSettings(c,pSys->shaderBillboardClass::getPtr());
    }
#endif
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


// QuickView Dir List
void mainGLApp::getQuickViewDirList()
{
/*

    std::string path = "ChaoticAttractors";
    for (const auto & entry : std::filesystem::directory_iterator(path))
        std::cout << entry.path().filename() << std::endl;
*/

    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir ("ChaoticAttractors")) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            const int len = strlen(ent->d_name);
            if(len>4 && !strcmp(&ent->d_name[len-4], ".sca") 
#ifdef __EMSCRIPTEN__
               && strncmp(ent->d_name, "test", 4)
#endif
            ) {
                getListQuickView().push_back(ent->d_name);
                //printf ("%s\n", ent->d_name);
            }
        }
        closedir (dir);
    } 

    std::sort(getListQuickView().begin(), getListQuickView().end());
}

void mainGLApp::loadQuikViewSelection(int idx)
{
    selectedListQuickView(idx);
    std::string s = STRATT_PATH; s+=getListQuickView().at(idx);
    attractorsList.getThreadStep()->stopThread();
    theDlg.getParticlesDlgClass().resetTreeParticlesFlags();
    loadAttractor(s.c_str());
    setLastFile(s);
    attractorsList.getThreadStep()->restartEmitter();
    attractorsList.get()->initStep();
    attractorsList.getThreadStep()->startThread();
}


void mainGLApp::selectCaptureFolder() {        
#if !defined(GLCHAOSP_LIGHTVER)
    const char *path = tinyfd_selectFolderDialog("Select folder...", getCapturePath().c_str());
    if(path != NULL) { setCapturePath(path); capturePath += "/"; }
#endif
}


template <class T> bool getVec_asArray(Config& c, const char *name, T &outV)
{
        if(c.has_key(name)) {
            vector<float> v;
            for (const Config& e : c[name].as_array()) v.push_back(e.as_float());
            outV = *((vec4 *)v.data());
            return true;
        }
        return false;

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


    vec4 v4;
    ptr->getDotTex().rebuild( c.get_or("dotsSize"        , DOT_TEXT_SHFT),
                              getVec_asArray(c, "HermiteVals", v4) ? v4 : vec4(.7f, 0.f, .3f, 0.f),
                              c.get_or("dotsType"        , 0));

    ptr->useAO(                    c.get_or("AOenabled"   , false               ));
    ptr->setAOStrong(              c.get_or("AOstrong"    , 0.0                 ));
    ptr->setAOBias(                c.get_or("AObias"      , .2                  ));
    ptr->setAORadius(              c.get_or("AOradius"    , ptr->getAORadius()  ));
    ptr->setAODarkness(            c.get_or("AOdarkness"  , .25                 ));
    ptr->setAOMul(                 c.get_or("AOmul"       , 1.0                 ));
    ptr->setAOModulate(            c.get_or("AOmodulate"  , 1.0                 ));


    ptr->postRenderingActive(             c.get_or("depthRender" , false));
    ptr->dpAdjConvex(                     c.get_or("dpAdjConvex" , 0.333));
    ptr->dpNormalTune(                    c.get_or("dpNormalTune", 0.025));


    ptr->useShadow(            c.get_or("ShadowEnabled"      , false ));
    ptr->setShadowBias(        c.get_or("ShadowBias"         , 0.0   ));
    ptr->setShadowDarkness(    c.get_or("ShadowDarkness"     , 0.0   ));
    ptr->setShadowRadius(      c.get_or("ShadowRadius"       , 2.0   ));
    ptr->setShadowGranularity( c.get_or("ShadowGranularity"  , 1.0   ));
    ptr->autoLightDist(        c.get_or("ShadowAutoLightDist", true  ));


    ptr->dstBlendIdx(getBlendIdx(ptr->getDstBlend()));
    ptr->srcBlendIdx(getBlendIdx(ptr->getSrcBlend()));

//Colors
    ptr->                 setColIntensity(c.get_or("ColorInt"  , 1.0));
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
    ptr->getUData().lightModel    = c.get_or("lightModel"      , int(ptr->modelBlinnPhong-ptr->modelOffset));
    ptr->getUData().ggxRoughness  = c.get_or("ggxRoughness"    , ptr->getUData().ggxRoughness);
    ptr->getUData().ggxFresnel    = c.get_or("ggxFresnel"      , ptr->getUData().ggxFresnel);

    vec3 v3;
    ptr->setLightDir( (getVec_asArray(c, "lightDir"  , v3) ? v3 : vec3(50.f, 15.f, 25.f)));
    ptr->getUData().lightColor = (getVec_asArray(c, "lightColor", v3) ? v3 : vec3(1.f));

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
#ifdef GLCHAOSP_LIGHTVER
    if(theApp->startWithGlowOFF()) glow->setGlowOn(false);    
    if(glow->isGlowOn()) glow->setGlowState(glow->glowType_Bilateral);
#endif

    glow->setSigma(     c.get_or("sigma"       , glow->getSigma()     ));
    glow->setSigmaRadX( c.get_or("sigmaRadX"   , glow->getSigmaRadX() ));
    glow->setMixTexture(c.get_or("mixTexture"  , glow->getMixTexture()));
    glow->setThreshold(c.get_or("glowThreshold", glow->getThreshold()));

    glow->getImgTuning()->setTextComponent(c.get_or("renderInt", glow->getImgTuning()->getTextComponent()));
    glow->getImgTuning()->setBlurComponent(c.get_or("blurInt"  , glow->getImgTuning()->getBlurComponent()));
    glow->getImgTuning()->setBlatComponent(c.get_or("bilatInt" , glow->getImgTuning()->getBlatComponent()));
    glow->getImgTuning()->setMixBilateral (c.get_or("bilatMix" , glow->getImgTuning()->getMixBilateral() ));
#if !defined(GLCHAOSP_NO_FXAA)
//FXAA
    fxaaClass *fxaa = ptr->getFXAA();
    fxaa->activate(    c.get_or("fxaaOn"       , fxaa->isOn()));
    fxaa->setThreshold(c.get_or("fxaaThreshold", fxaa->getThreshold()));
    fxaa->setReductMul(c.get_or("ReductMul"    , fxaa->getReductMul()));
    fxaa->setReductMin(c.get_or("ReductMin"    , fxaa->getReductMin()));
    fxaa->setSpan     (c.get_or("Span"         , fxaa->getSpan     ()));
    if(fxaa->getSpan()     >fxaa->getSpanMax()) fxaa->setSpan(     fxaa->getSpanMax());
    if(fxaa->getReductMul()>fxaa->getMulMax() ) fxaa->setReductMul(fxaa->getMulMax() );
    if(fxaa->getReductMin()>fxaa->getMinMax() ) fxaa->setReductMin(fxaa->getMinMax() );
#endif

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

void loadSettings(Config &cfg, particlesSystemClass *pSys, int typeToIgnore = loadSettings::ignoreNone) 
{

    vec3 v3;
    {
        auto& c = cfg["Render"];

        pSys->setRenderMode(                      c.get_or("RenderMode"   , pSys->getRenderMode()                      ));
#if !defined(GLCHAOSP_LIGHTVER)
        pSys->getMotionBlur()->Active(            c.get_or("motionBlur"   , pSys->getMotionBlur()->Active()            ));
        pSys->getMotionBlur()->setBlurIntensity(  c.get_or("blurIntensity", pSys->getMotionBlur()->getBlurIntensity()  ));
        pSys->getMergedRendering()->setMixingVal( c.get_or("mixingVal"    , pSys->getMergedRendering()->getMixingVal() ));
#endif

        if(typeToIgnore != loadSettings::ignoreCircBuffer) {
            pSys->getEmitter()->setSizeCircularBuffer(c.get_or("circBuff"     , pSys->getEmitter()->getSizeCircularBuffer()));
#ifdef GLCHAOSP_LIGHTVER
            pSys->getEmitter()->setSizeCircularBuffer(pSys->getEmitter()->getSizeCircularBuffer()>>1);
#endif
            if(pSys->getEmitter()->getSizeCircularBuffer()>pSys->getEmitter()->getSizeAllocatedBuffer())
                pSys->getEmitter()->setSizeCircularBuffer(pSys->getEmitter()->getSizeAllocatedBuffer());

            pSys->getEmitter()->restartCircBuff(      c.get_or("rstrtCircBuff", pSys->getEmitter()->restartCircBuff()      ));        
            pSys->getEmitter()->stopFull(             c.get_or("stopCircBuff" , pSys->getEmitter()->stopFull()             ));

#ifdef GLCHAOSP_LIGHTVER
            if(theApp->isTabletMode()) pSys->getEmitter()->stopFull(true);
#endif
        }


        if(getVec_asArray(c, "camPOV"        , v3)) pSys->getTMat()->setPOV(v3);
        if(getVec_asArray(c, "camTGT"        , v3)) pSys->getTMat()->setTGT(v3);
        if(getVec_asArray(c, "camPerspective", v3)) pSys->getTMat()->setPerspective(v3.x, v3.y, v3.z);
        if(getVec_asArray(c, "camDolly"      , v3)) pSys->getTMat()->getTrackball().setDollyPosition(v3);
        if(getVec_asArray(c, "camPan"        , v3)) pSys->getTMat()->getTrackball().setPanPosition(v3);
        if(getVec_asArray(c, "camRotCent"    , v3)) pSys->getTMat()->getTrackball().setRotationCenter(v3);

        if(c.has_key("camRot")) {
            vector<float> v;
            for (const Config& e : c["camRot"].as_array()) v.push_back(e.as_float());
            pSys->getTMat()->getTrackball().setRotation(*((quat *)v.data()));
        }
    }

    {
#if !defined(GLCHAOSP_LIGHTVER)
        auto &c0 = cfg["RenderMode0"];
        particlesBaseClass *ptr0 = pSys->shaderBillboardClass::getPtr();
        getRenderMode(c0, ptr0);

        auto &c1 = cfg["RenderMode1"];
#else
        auto &c1 = cfg[(!theDlg.getInvertSettings())^(pSys->getRenderMode()==RENDER_USE_BILLBOARD) ? "RenderMode1" : "RenderMode0"];
#endif
        particlesBaseClass *ptr1 = pSys->shaderPointClass::getPtr();
        getRenderMode(c1,ptr1);

    }

}


void mainGLApp::invertSettings() 
{
    configuru::Config cfg = Config::object(); 
    particlesSystemClass *pSys = theWnd->getParticlesSystem();

    //Save Inverted
#if !defined(GLCHAOSP_LIGHTVER)
    {
        auto &c = cfg["RenderMode1"] = Config::object();
        saveParticlesSettings(c,pSys->shaderBillboardClass::getPtr());
    }
#endif
    {
        auto &c = cfg["RenderMode0"] = Config::object();
        saveParticlesSettings(c,pSys->shaderPointClass::getPtr());
    }

#if !defined(GLCHAOSP_LIGHTVER)
    {
        auto &c = cfg["RenderMode0"];
        particlesBaseClass *ptr = pSys->shaderBillboardClass::getPtr();
        getRenderMode(c, ptr);
    }
#endif
    {
        auto &c = cfg["RenderMode1"];
        particlesBaseClass *ptr = pSys->shaderPointClass::getPtr();
        getRenderMode(c,ptr);
    }
}




bool mainGLApp::loadSettings(const char *name, const int typeToIgnore)
{
    if(!fileExist(name)) return false;
    Config cfg = configuru::parse_file(name, JSON);

    ::loadSettings(cfg, theWnd->getParticlesSystem(), typeToIgnore);

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
#if !defined(GLCHAOSP_LIGHTVER)
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
#endif
    screenShotRequest = ScreeShotReq::ScrnSht_NO_REQUEST;
}

char const *mainGLApp::openFile(const char *startDir, char const * patterns[], int numPattern)  
{
#if !defined(GLCHAOSP_LIGHTVER)
    if(glfwGetWindowMonitor(getGLFWWnd())) toggleFullscreenOnOff(getGLFWWnd());

    //char const * patterns[2] = { "*.chatt", "*.txt" };
    return tinyfd_openFileDialog("Load File...", startDir, numPattern, patterns, NULL, 0);
#else 
    return NULL;
#endif
}


char const *mainGLApp::saveFile(const char *startDir, char const * patterns[], int numPattern)  
{
#if !defined(GLCHAOSP_LIGHTVER)
    if(glfwGetWindowMonitor(getGLFWWnd())) toggleFullscreenOnOff(getGLFWWnd());

    //char const * patterns[2] = { "*.chatt", "*.txt" };
    return tinyfd_saveFileDialog("Save File...", startDir, numPattern, patterns, NULL);
#else
    return NULL;
#endif
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
        theApp->loadSettings(fileName, loadSettings::ignoreCircBuffer);

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
    cfg["partSizeConst"] =  theApp->isParticlesSizeConstant();

    cfg["useLowPrecision"] =  theApp->useLowPrecision();

    cfg["capturePath" ] = capturePath;

    cfg["emitterType" ] = theApp->getEmitterType();
    cfg["auxStepBuffer" ] = theApp->getEmissionStepBuffer();

    cfg["startWithAttractorIdx" ] = theApp->getStartWithAttractorIdx();

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
    if(getMaxAllocatedBuffer()>PARTICLES_MAX) setMaxAllocatedBuffer(PARTICLES_MAX);
    
    theApp->setParticlesSizeConstant(cfg.get_or("partSizeConst", false));

    theApp->useLowPrecision(cfg.get_or("useLowPrecision", false));

    theApp->selectEmitterType(cfg.get_or("emitterType",int(emitter_separateThread_externalBuffer)));
    theApp->setEmissionStepBuffer(cfg.get_or("auxStepBuffer", theApp->getEmissionStepBuffer()));

    theApp->setStartWithAttractorIdx(cfg.get_or("startWithAttractorIdx", -1));  // -1 Random start

    if(theApp->useLowPrecision()) theApp->setLowPrecision();
    else                          theApp->setHighPrecision();

    capturePath = cfg.get_or("capturePath", capturePath);

    return true; // config file exist
}