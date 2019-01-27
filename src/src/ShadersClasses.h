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
//#define GLM_FORCE_SWIZZLE 
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>

#if !defined(GLCHAOSP_LIGHTVER)
#include "tools/oglAxes.h"
#else
#include <transforms.h>
#endif

#include "palettes.h"

#define SHADER_PATH "Shaders/"

#define PURE_VIRTUAL 0

#include "mmFBO.h"

using namespace glm;

template<class T>
inline T trim(T x, T inc, T min, T max) {
    T tmp = x+inc;

    if(tmp<x) return (tmp>=min) ? tmp : x;
    else      return (tmp<=max) ? tmp : x;

}

enum particlsRenderMethod {
    RENDER_USE_BILLBOARD,
    RENDER_USE_POINTS,
    RENDER_USE_BOTH
};

enum {
    BLUR_NONE,    
    BLUR_SINGLE,
    BLUR_DUAL
};


enum RADIAL_BLUR {
    RB_PASS_1,
    RB_PASS_2,
    RB_BYPASS
};

enum particlesSysyemType {
    PS_SINGLE_EMITTER,
    PS_MULTIPLE_EMITTER
};

enum PS_FRAME_BUFFERS {
    FB_BILLBOARD,
    FB_POINTSPRITE
};

class renderBaseClass;
class dataBlurClass ;
class imgTuningClass;
class BlurBaseClass;

/*

struct uTuningData {
    GLfloat gamma;
    GLfloat bright;
    GLfloat contrast;
    GLfloat exposure;
    GLfloat blurIntensity;
    GLfloat renderIntensity;
    GLfloat sigma;

};
*/


//
//  imgTuning
//
////////////////////////////////////////////////////////////////////////////////
class imgTuningClass
{

public:
    imgTuningClass(dataBlurClass *ptrGlow);

////////////////////////////
// imgTuningClass
    GLfloat getGamma()    { return videoControls.x; }
    GLfloat getBright()   { return videoControls.z; }
    GLfloat getContrast() { return videoControls.w; }
    GLfloat getExposure() { return videoControls.y; }

    void  setGamma(GLfloat v)   ;
    void  setBright(GLfloat v)  ;
    void  setContrast(GLfloat v);
    void  setExposure(GLfloat v);
    

    GLfloat getTextComponent() { return texControls.y; }    
    GLfloat getBlurComponent() { return texControls.x; }
    GLfloat getBlatComponent() { return texControls.z; }

    void  setBlurComponent(GLfloat v); 
    void  setBlatComponent(GLfloat v); 
    void  setTextComponent(GLfloat v); 

    void  setMixBilateral(GLfloat v); 
    GLfloat getMixBilateral() { return texControls.w; }

    void  setDynEq(bool b);
    bool  getDynEq() { return useDynEQ; }

    void setToneMap(bool b);
    bool getToneMap() { return toneMapping; }

    void setToneMap_A(float v);
    void setToneMap_G(float v);
    float getToneMap_A() { return toneMapValsAG.x; } 
    float getToneMap_G() { return toneMapValsAG.y; } 

protected:
vec4 videoControls;
vec4 texControls;
bool useDynEQ;
bool toneMapping = false;
vec2 toneMapValsAG = vec2(1.0, 1.0); // tonemap -> col = A * pow(col, G); -> x = A and y = G
dataBlurClass *glow;

friend BlurBaseClass;

};


//
//  dataBlur
//
////////////////////////////////////////////////////////////////////////////////
class dataBlurClass 
{
public:
    enum { glowType_ByPass, glowType_Blur, glowType_Threshold, glowType_Bilateral };
    enum { idxSubroutine_ByPass, 
           idxSubroutine_BlurCommonPass1, 
           idxSubroutine_BlurGaussPass2, 
           idxSubroutine_BlurThresholdPass2, 
           idxSubroutine_Bilateral,
           idxSubroutine_End};

    dataBlurClass();

    ~dataBlurClass() { delete imageTuning; }


    mmFBO &getFBO() { return glowFBO; };

    ////////////////////////////
// dataBlurClass
    GLfloat getSigma()       { return sigma.x; }
    void setSigma(GLfloat s)    { sigma.x = s; setFlagUpdate(); }

    GLfloat getSigmaRadX()     { return sigma.y; }
    void setSigmaRadX(float f) { sigma.y = f; setFlagUpdate(); }
    void setSigmaRad2X()       { sigma.y = 2.0; setFlagUpdate(); }
    void setSigmaRad3X()       { sigma.y = 3.0; setFlagUpdate(); }

    GLfloat getMixTexture()       { return mixTexture; }
    void setMixTexture(GLfloat s)    { mixTexture = s; setFlagUpdate(); }

    void setFlagUpdate();
    void clearFlagUpdate();
    void flagUpdate(bool b);

    imgTuningClass *getImgTuning() { return imageTuning; }

    int getGlowState() { return glowState; }
    void setGlowState(int b) { glowState = b; setFlagUpdate(); }

    bool isGlowOn() { return glowActive; }
    void setGlowOn(bool b) { glowActive = b; }

    float getThreshold() { return threshold; }
    void setThreshold(float f) { threshold = f; setFlagUpdate(); }


protected:
    vec4 sigma = vec4(3.0); 
    int glowState = glowType_Threshold;
    bool glowActive = false;
    float threshold = .1;
    GLfloat mixTexture;
    mmFBO glowFBO;
    renderBaseClass *renderEngine;
    imgTuningClass *imageTuning;
};

inline void imgTuningClass::setGamma(GLfloat v)    { videoControls.x = v; glow->setFlagUpdate(); }
inline void imgTuningClass::setBright(GLfloat v)   { videoControls.z = v; glow->setFlagUpdate(); }
inline void imgTuningClass::setContrast(GLfloat v) { videoControls.w = v; glow->setFlagUpdate(); }
inline void imgTuningClass::setExposure(GLfloat v) { videoControls.y = v; glow->setFlagUpdate(); }
inline void imgTuningClass::setDynEq(bool b)       { useDynEQ = b;        glow->setFlagUpdate(); }
inline void imgTuningClass::setToneMap(bool b)     { toneMapping = b;     glow->setFlagUpdate(); }
inline void imgTuningClass::setToneMap_A(float v)  { toneMapValsAG.x = v; glow->setFlagUpdate(); } 
inline void imgTuningClass::setToneMap_G(float v)  { toneMapValsAG.y = v; glow->setFlagUpdate(); } 

inline void imgTuningClass::setBlatComponent(GLfloat v) { texControls.z = v; glow->setFlagUpdate(); }
inline void imgTuningClass::setBlurComponent(GLfloat v) { texControls.x = v; glow->setFlagUpdate(); }
inline void imgTuningClass::setTextComponent(GLfloat v) { texControls.y = v; glow->setFlagUpdate(); }

inline void imgTuningClass::setMixBilateral(GLfloat v) { texControls.w = v; glow->setFlagUpdate(); }


//
//  blurBase
//
////////////////////////////////////////////////////////////////////////////////
class BlurBaseClass : public mainProgramObj, public uniformBlocksClass, public virtual dataBlurClass
{
struct uBlurData {
    vec4 sigma;             // align 0
    GLfloat threshold;
    GLuint toneMapping;     //bool, but need to align 4 byte
    vec2 toneMapVals;       // align 16+2N 24

    vec4 texControls;       // align 32
    vec4 videoControls;     // align 48

    GLfloat mixTexture;

    GLint blurCallType;
} uData;

public:

    BlurBaseClass() {
    }

    void create();

    void glowPass(GLuint sourceTex, GLuint fbo, GLuint subIndex);

    void updateData(GLuint subIndex) {
        const float invSigma = 1.f/sigma.x;
        sigma.z = .5 * invSigma * invSigma;
        sigma.w = glm::one_over_pi<float>() * sigma.z;
        getUData().sigma        = sigma;
        getUData().threshold    = threshold;

        getUData().toneMapping  = imageTuning->toneMapping;
        getUData().toneMapVals  = imageTuning->toneMapValsAG;

        const float gamma = 1.f/imageTuning->videoControls.x;
        const float exposure = imageTuning->videoControls.y;
        const float bright   = imageTuning->videoControls.z; 
        const float contrast = imageTuning->videoControls.w;        
        getUData().videoControls = vec4(gamma , exposure, bright, contrast); 
        getUData().texControls = imageTuning->texControls;

        getUData().mixTexture   = (1.f + mixTexture)*.5;

        getUData().blurCallType = subIndex;

        updateBufferData();

    }

    uBlurData &getUData() { return uData; }


protected:

    bool actualPass;

private:
#if !defined(GLAPP_REQUIRE_OGL45)
    GLuint LOCpass1Texture, LOCorigTexture;
    GLuint idxSubGlowType[idxSubroutine_End];
#endif
};

class radialBlurClass;
#if !defined(GLCHAOSP_LIGHTVER)
class motionBlurClass;
class mergedRenderingClass;
#endif

//
//  renderBase
//
////////////////////////////////////////////////////////////////////////////////
class renderBaseClass
{
public:
    renderBaseClass();

    enum { noShowAxes, showAxesToViewCoR, showAxesToSetCoR };

    virtual ~renderBaseClass();

    void setRenderMode(int which);
    int getRenderMode() { return whichRenderMode; }


    mmFBO &getRenderFBO() { return renderFBO; }; 
    //mmFBO &getMSAAFBO() { return msaaFBO; };
    int getWidth()  { return getRenderFBO().getSizeX(); }
    int getHeight() { return getRenderFBO().getSizeY(); }

 
    cmContainerClass &getColorMapContainer() { return colorMapContainer; }


    bool checkFlagUpdate() { return flagUpdate; }
    void setFlagUpdate() { flagUpdate = true; }
    void clearFlagUpdate() { flagUpdate = false; }

    transformsClass *getTMat() { return &tMat; }

#if !defined(GLCHAOSP_LIGHTVER)
    mergedRenderingClass *getMergedRendering() { return mergedRendering; }
    motionBlurClass *getMotionBlur() { return motionBlur; }
    oglAxes *getAxes() { return axes; }

    void showAxes(int b) { axesShow = b; }
    int showAxes() { return axesShow; }

#endif

    int getWhitchRenderMode() { return whichRenderMode; }

    
    int getBlendArrayElements() { return blendArray.size(); }
    std::vector<GLuint> &getBlendArray() { return blendArray; }
    std::vector<const char *> &getBlendArrayStrings() { return blendingStrings; }


protected:
    int whichRenderMode;    

#if !defined(GLCHAOSP_LIGHTVER)
    motionBlurClass *motionBlur;
    mergedRenderingClass *mergedRendering;

    oglAxes *axes;
    int axesShow = noShowAxes;
#endif

    cmContainerClass colorMapContainer;

    bool flagUpdate;

    mmFBO renderFBO;
//        , msaaFBO;

    transformsClass tMat;

    std::vector<GLuint> blendArray;
    std::vector<const char *> blendingStrings;
    

private:
};

//
//  radialBlur
//
////////////////////////////////////////////////////////////////////////////////
class radialBlurClass : public BlurBaseClass
{
public:
    
    radialBlurClass(renderBaseClass *ptrRE) {
        renderEngine = ptrRE;
#if !defined(GLCHAOSP_LIGHTVER)
        glowFBO.buildFBO(2, renderEngine->getWidth(), renderEngine->getHeight(), false);
#else
        glowFBO.buildFBO(1, renderEngine->getWidth(), renderEngine->getHeight(), false);
#endif
        BlurBaseClass::create();
    }


    void render(GLuint sourceTex, GLuint fbOut) {

#if !defined(GLCHAOSP_LIGHTVER)
        if(isGlowOn() && (getGlowState()==glowType_Blur || getGlowState()==glowType_Threshold)) {
            glowPass(sourceTex, glowFBO.getFB(RB_PASS_1), idxSubroutine_BlurCommonPass1);
            glowPass(sourceTex, fbOut, getGlowState()==glowType_Blur ? 
                                                       idxSubroutine_BlurGaussPass2 : 
                                                       idxSubroutine_BlurThresholdPass2);
        } else
            glowPass(sourceTex, fbOut, isGlowOn() && getGlowState()==glowType_Bilateral ? idxSubroutine_Bilateral : idxSubroutine_ByPass);
#else
            glowPass(sourceTex, fbOut, isGlowOn() ? idxSubroutine_Bilateral : idxSubroutine_ByPass);
#endif
            
    }
};

#if !defined(GLCHAOSP_NO_FXAA)

//
//  fxaa
//
////////////////////////////////////////////////////////////////////////////////
class fxaaClass : public mainProgramObj
{
public:
    fxaaClass(renderBaseClass *ptrRE) { 
        renderEngine = ptrRE;
        on();
        create();
    }

    void create();
    GLuint render(GLuint texIn);

    bool isOn() { return bIsOn; }
    void activate(bool b) {
        if(b==bIsOn) return;
        if(b) on();
        else off();
        bIsOn=b; 
        renderEngine->setFlagUpdate();
    }

    void setThreshold(float f) { threshold = f; renderEngine->setFlagUpdate(); }  // fxxaData.x
    void setReductMul(float f) { reduceMul = f; renderEngine->setFlagUpdate(); }  // 1/fxxaData.y
    void setReductMin(float f) { reduceMin = f; renderEngine->setFlagUpdate(); }  // 1/fxxaData.z
    void setSpan     (float f) { span      = f; renderEngine->setFlagUpdate(); }  // fxxaData.w

    float getThreshold() { return threshold; }
    float getReductMul() { return reduceMul; }
    float getReductMin() { return reduceMin; }
    float getSpan     () { return span;      }

    void updateSettings() {
        const vec4 fxaaData = vec4(threshold, 
                                   reduceMul>512 ? 0 : 1.f/reduceMul, 
                                   reduceMin>512 ? 0 : 1.f/reduceMin, 
                                   span);
        setUniform4fv(_fxaaData, 1, glm::value_ptr(fxaaData));    
    }

    mmFBO &getFBO() { return fbo; }

private:
    void on()  { fbo.reBuildFBO(1, renderEngine->getWidth(), renderEngine->getHeight(), false); }
    void off() { fbo.deleteFBO(); }

    renderBaseClass *renderEngine;
    mmFBO fbo;
    bool bIsOn = false;

    GLfloat span = 8.f, reduceMul = 8.f, reduceMin = 128.f, threshold = .5f;
    //Locations
    GLuint _fxaaData, _invScrnSize, _u_colorTexture;
};

#endif

#if !defined(GLCHAOSP_LIGHTVER)

//
//  motionBlur
//
////////////////////////////////////////////////////////////////////////////////
class motionBlurClass : public mainProgramObj
{
public:

    motionBlurClass(renderBaseClass *ptrRE) {
        renderEngine = ptrRE;
        rotationBuff = 0;  
        blurIntensity = .5;
        isActive = false;
        
        create();
    }

    GLuint render(GLuint renderedTex);

    void create(); 

    void rotateBuffer() { rotationBuff ^= 1; }

    mmFBO &getFBO() { return mBlurFBO; };
#if !defined(GLAPP_REQUIRE_OGL45)
    void updateAccumMotionTex(GLuint tex)    { glUniform1i(LOCaccumMotion, tex); }
    void updateSourceRenderedTex(GLuint tex) { glUniform1i(LOCsourceRendered, tex); }
#endif

    void updateBlurIntensity() { setUniform1f(LOCblurIntensity, .5 + blurIntensity*.5); }

    bool Active() { return isActive; }
    void Active(bool b) { 
        if(b==isActive) return;
        if(b) {
            mBlurFBO.reBuildFBO(2, renderEngine->getWidth(), renderEngine->getHeight(), false);            
        } else {
            mBlurFBO.deleteFBO();
        }
        isActive=b; 
        setFlagUpdate();
    }

    void setBlurIntensity(GLfloat f) { blurIntensity = f; setFlagUpdate(); }
    GLfloat getBlurIntensity() { return blurIntensity; }

    void setFlagUpdate() { renderEngine->setFlagUpdate(); }
    void clearFlagUpdate() { renderEngine->clearFlagUpdate(); }
    void flagUpdate(bool b) { b ? renderEngine->setFlagUpdate() : renderEngine->clearFlagUpdate(); }

private:
    renderBaseClass *renderEngine;
    bool isActive;
    GLfloat blurIntensity;

    mmFBO mBlurFBO;

    int rotationBuff;
#if !defined(GLAPP_REQUIRE_OGL45)
    GLuint LOCsourceRendered, LOCaccumMotion;
#endif
    GLuint LOCblurIntensity;
};


//
// mergedRendering
//
////////////////////////////////////////////////////////////////////////////////
class mergedRenderingClass : public mainProgramObj
{
public:

    mergedRenderingClass(renderBaseClass *ptrRE) { 
        renderEngine = ptrRE;
        mixingVal=0.0;
        create();
    }

    GLuint render(GLuint texA, GLuint texB);
    void create();

    void Activate()   { 
        renderEngine->getRenderFBO().reBuildFBO(2,renderEngine->getWidth(),renderEngine->getHeight(), true);
        mergedFBO.reBuildFBO(1,renderEngine->getWidth(),renderEngine->getHeight()); 
        renderEngine->setFlagUpdate();
    }
    void Deactivate() { 
        renderEngine->getRenderFBO().reBuildFBO(1,renderEngine->getWidth(),renderEngine->getHeight(), true);
        mergedFBO.deleteFBO(); 
        renderEngine->setFlagUpdate();
    }

#ifndef GLAPP_REQUIRE_OGL45
    void updateBillboardTex(GLuint tex)    { glUniform1i(LOCbillboardTex, tex); }
    void updatePointsTex(GLuint tex) { glUniform1i(LOCpointsTex, tex); }
#endif
    void updatemixingVal()  { setUniform1f(LOCmixingVal, (mixingVal+1.0)*.5); }

    void setMixingVal(GLfloat f) { mixingVal = f; setFlagUpdate(); }
    GLfloat getMixingVal() { return mixingVal; }

    void setFlagUpdate() { renderEngine->setFlagUpdate(); }
    void clearFlagUpdate() { renderEngine->clearFlagUpdate(); }
    void flagUpdate(bool b) { b ? renderEngine->setFlagUpdate() : renderEngine->clearFlagUpdate(); }

    mmFBO &getFBO() { return mergedFBO; }

private:
    GLfloat mixingVal;
#ifndef GLAPP_REQUIRE_OGL45
    GLuint LOCbillboardTex, LOCpointsTex;
#endif
    GLuint LOCmixingVal;
    renderBaseClass *renderEngine;

    mmFBO mergedFBO;
};
#endif

class particlesDlgClass;

//
// colorMapTextured
//
////////////////////////////////////////////////////////////////////////////////
class colorMapTexturedClass : public mainProgramObj, public uniformBlocksClass
{
struct uCMapData {
    vec3 hslData;
    GLfloat offsetPoint = 0.f;
    GLfloat palRange = 1.f;
    GLuint clamp = false;     //bool, but need to align 4 byte
    GLuint reverse = false;   //bool
} uData;

public:
    enum {
        ORIG_TEXTURE ,
        MODF_TEXTURE
    };

    colorMapTexturedClass(particlesBaseClass *p) {
        particles = p;
        flagUpdate = true;

        //cmTex.buildMultiDrawFBO(1,256,1);
        cmTex.buildFBO(1,256,2);

        create();
    }

    void create();

    uCMapData &getUData() { return uData; }


    void render(int tex=0);

    GLuint getOrigTex();
    GLuint getModfTex() { return cmTex.getTex(0); }

    float getOffsetPoint() { return getUData().offsetPoint; }
    float getRange() { return getUData().palRange; }
    void setOffsetPoint(float f)  { getUData().offsetPoint = f;  setFlagUpdate(); }
    void setRange(float f)   { getUData().palRange = f;   setFlagUpdate(); }


    bool getReverse() { return getUData().reverse; }
    void setReverse(bool b) { getUData().reverse = b; setFlagUpdate(); }

    bool getClamp() { return getUData().clamp; }
    void setClamp(bool b) { getUData().clamp = b;  setFlagUpdate(); }

    float getH() { return getUData().hslData.x; }
    float getS() { return getUData().hslData.y; }
    float getL() { return getUData().hslData.z; }
    void setH(float f) { getUData().hslData.x = f;   setFlagUpdate(); }
    void setS(float f) { getUData().hslData.y = f;   setFlagUpdate(); }
    void setL(float f) { getUData().hslData.z = f;   setFlagUpdate(); }

    bool checkFlagUpdate() { return flagUpdate; }
    void setFlagUpdate()   { flagUpdate = true; }
    void clearFlagUpdate() { flagUpdate = false; }

    mmFBO &getFBO() { return cmTex; }

private:
    
    GLuint LOCpaletteTex;
    particlesBaseClass *particles;
    mmFBO cmTex;
    bool flagUpdate;

};

//
// ColorMapSettings
//
////////////////////////////////////////////////////////////////////////////////
class ColorMapSettingsClass : public paletteTexClass, public colorMapTexturedClass
{
public:
    ColorMapSettingsClass(particlesBaseClass *p) : colorMapTexturedClass(p) {
        particles = p;
        velIntensity = .3f;
    }

    void selected(int i)       { selection = i;    }
    int  selected()            { return selection; } 

    float getVelIntensity() { return velIntensity; }    
    void setVelIntensity(float f);


protected:
    int selection;
    float velIntensity;

private:
    particlesBaseClass *particles;

};


//
// ParticlesBase
//
////////////////////////////////////////////////////////////////////////////////
class particlesBaseClass : public mainProgramObj, public uniformBlocksClass, public virtual renderBaseClass 
{
protected:

struct uParticlesData {
    vec3    lightDir = vec3(50.f, 0.f, 15.f); // align 0
    GLfloat lightDiffInt = 3.f;
    vec3    lightColor = vec3(1.f);           // align 16
    GLfloat lightSpecInt = 1.f;
    vec2    scrnRes;
    GLfloat lightAmbInt = .1f;
    GLfloat lightShinExp = 50.f;
    GLfloat sstepColorMin = .1;
    GLfloat sstepColorMax = 1.1;
    GLfloat pointSize;
    GLfloat pointDistAtten;
    GLfloat alphaDistAtten;
    GLfloat alphaSkip = .15f;
    GLfloat alphaK;
    GLfloat colIntensity = 1.0;
    GLfloat clippingDist;
    GLfloat zNear;
    GLfloat zFar;
    GLfloat velocity;
    GLfloat ySizeRatio = 1.0;
    GLfloat ptSizeRatio = 1.0;
    GLfloat pointspriteMinSize = 1.0;
    GLuint lightActive;
} uData;


public:
    particlesBaseClass ()  { 

        glowRender = new radialBlurClass(this);
        CHECK_GL_ERROR();

        colorMap = new ColorMapSettingsClass(this);
#if !defined(GLCHAOSP_NO_FXAA)
        fxaaFilter = new fxaaClass(this);
#endif
        dotTex.build(DOT_TEXT_SHFT, vec4(.7f, 0.f, .3f, 0.f), dotsTextureClass::dotsAlpha);
    }

    ~particlesBaseClass ()  {  delete glowRender; delete colorMap;
#if !defined(GLCHAOSP_NO_FXAA)
    delete fxaaFilter; 
#endif
    }

    void getCommonLocals() {
#ifndef GLAPP_REQUIRE_OGL45
        locPaletteTex = getUniformLocation("paletteTex" );
        locDotsTex    = getUniformLocation("tex"); 
#endif

    }

    void updateCommonUniforms() {
        getUData().scrnRes.x = getRenderFBO().getSizeX();
        getUData().scrnRes.y = getRenderFBO().getSizeY();
        getUData().ySizeRatio = theApp->isParticlesSizeConstant() ? 1.0 : float(getRenderFBO().getSizeY()/1024.0);
        getUData().ptSizeRatio = 1.0/(length(getUData().scrnRes) / getUData().scrnRes.x);
        getUData().velocity = getCMSettings()->getVelIntensity();
    }

#ifndef GLAPP_REQUIRE_OGL45
    void updatePalTex()  { setUniform1i(locPaletteTex, getCMSettings()->getModfTex()); }
#endif

    uParticlesData &getUData() { return uData; }

    virtual void render(GLuint fbOut, emitterBaseClass *em);

    GLuint getDstBlend() { return dstBlendAttrib; }
    GLuint getSrcBlend() { return srcBlendAttrib; }

    void setDstBlend(GLuint v) {  dstBlendAttrib = v; }
    void setSrcBlend(GLuint v) {  srcBlendAttrib = v; }

    void setSize(float sz, float step) { getUData().pointSize=sz; stepInc=step; setFlagUpdate(); }
    void setSize(float sz) { getUData().pointSize=sz; setFlagUpdate(); }
    float getSize() { return getUData().pointSize; }
    float getInc() { return stepInc; }

    void setAlphaAtten(float v) { getUData().alphaDistAtten=v; setFlagUpdate(); }
    float getAlphaAtten() { return getUData().alphaDistAtten; }
    void setAlphaKFactor(float v) { getUData().alphaK=v; setFlagUpdate(); }
    float getAlphaKFactor() { return getUData().alphaK; }
    void setPointSizeFactor(float v) { getUData().pointDistAtten=v; setFlagUpdate(); }
    float getPointSizeFactor() { return getUData().pointDistAtten; }
    void setClippingDist(float v) { getUData().clippingDist=v; setFlagUpdate(); }
    float getClippingDist() { return getUData().clippingDist; }
    void setAlphaSkip(float f) { getUData().alphaSkip = f;  setFlagUpdate();}
    float getAlphaSkip() { return getUData().alphaSkip; }
    void setColIntensity(float f) { getUData().colIntensity = f;  setFlagUpdate();}
    float getColIntensity() { return getUData().colIntensity; }
    void setLightColor(const vec3 &v) { getUData().lightColor = v;  setFlagUpdate();}
    vec3& getLightColor() { return getUData().lightColor; }

    virtual particlesBaseClass *getPtr() = 0;

    GLuint getPaletteTexID() { return colorMap->getTexID(); }

/////////////////////////////
// colorMaps
    ColorMapSettingsClass *getCMSettings() { return colorMap; }

    void selectColorMap(int i) {
        if(i>=colorMapContainer.elements()) i = 0;
        colorMap->selected(i);
        colorMap->buildTex(colorMapContainer.getRGB_pf3(i), colorMapContainer.getRGB_CMap3(i).size()/3); 
        setFlagUpdate(); colorMap->setFlagUpdate();
    }
    int getSelectedColorMap() { return colorMap->selected(); }

    float *getSelectedColorMap_pf3()   { return colorMapContainer.getRGB_pf3  (colorMap->selected()); }
    CMap3 &getSelectedColorMap_CMap3() { return colorMapContainer.getRGB_CMap3(colorMap->selected()); }
    float *getColorMap_pf3(int i) { return colorMapContainer.getRGB_pf3(i); }
    const char *getColorMap_name()      { return colorMapContainer.getName(colorMap->selected()); }
    const char *getColorMap_name(int i) { return colorMapContainer.getName(i); }

    bool getDepthState() { return depthBuffActive; }
    bool getBlendState() { return blendActive; }
    bool getLightState() { return bool(lightStateIDX); }
    
    void setDepthState(bool b) { depthBuffActive = b; }
    void setBlendState(bool b) { blendActive = b; }
    void setLightState(bool b) { lightStateIDX = b ? GLuint(on) : GLuint(off); }

    radialBlurClass *getGlowRender()  { return glowRender; }
#if !defined(GLCHAOSP_NO_FXAA)
    fxaaClass *getFXAA() { return fxaaFilter; } 
#endif
    void dstBlendIdx(int i) { dstIdxBlendAttrib = i; }
    int  dstBlendIdx() { return dstIdxBlendAttrib; }

    void srcBlendIdx(int i) { srcIdxBlendAttrib = i; }
    int  srcBlendIdx() { return srcIdxBlendAttrib; }

    void setHermiteVals(const vec4 &v) { dotTex.setHermiteVals(v); }
    vec4& getHermiteVals() { return dotTex.getHermiteVals(); }

    dotsTextureClass& getDotTex() { return dotTex; }

protected:
    GLuint dstBlendAttrib, srcBlendAttrib;
    int dstIdxBlendAttrib, srcIdxBlendAttrib;

    ColorMapSettingsClass *colorMap;

    dotsTextureClass dotTex;

    radialBlurClass *glowRender;

#if !defined(GLCHAOSP_NO_FXAA)
    fxaaClass *fxaaFilter;
#endif

    
    GLfloat stepInc;  

#if !defined(GLAPP_REQUIRE_OGL45)
    GLuint locDotsTex, locPaletteTex;
#endif

    enum lightIDX { off, on };
    GLuint lightStateIDX = GLuint(on);
    GLuint idxSubLightOn, idxSubLightOff;
    bool depthBuffActive = true;
    bool blendActive = true;

private:


friend class particlesDlgClass;
};

inline void ColorMapSettingsClass::setVelIntensity(float f) { velIntensity = f; particles->setFlagUpdate(); }
inline GLuint colorMapTexturedClass::getOrigTex() { return particles->getPaletteTexID(); }


inline void dataBlurClass::setFlagUpdate() { renderEngine->setFlagUpdate(); }
inline void dataBlurClass::clearFlagUpdate() { renderEngine->clearFlagUpdate(); }
inline void dataBlurClass::flagUpdate(bool b) { b ? renderEngine->setFlagUpdate() : renderEngine->clearFlagUpdate(); }

class emitterBaseClass;

//
// PointSprite
//
////////////////////////////////////////////////////////////////////////////////
class shaderPointClass : public particlesBaseClass 
{
public:
    shaderPointClass();

    shaderPointClass *getPtr() { return this; }

private:
    void initShader();
};

#if !defined(GLCHAOSP_LIGHTVER)

//
// Billboard
//
////////////////////////////////////////////////////////////////////////////////
class shaderBillboardClass : public particlesBaseClass 
{
public:
    shaderBillboardClass();

    shaderBillboardClass *getPtr() { return this; }

private:
    void initShader();
};

#endif

