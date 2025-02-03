//------------------------------------------------------------------------------
//  Copyright (c) 2018-2025 Michele Morrone
//  All rights reserved.
//
//  https://michelemorrone.eu - https://glchaosp.com - https://brutpitt.com
//
//  X: https://x.com/BrutPitt - GitHub: https://github.com/BrutPitt
//
//  direct mail: brutpitt(at)gmail.com - me(at)michelemorrone.eu
//
//  This software is distributed under the terms of the BSD 2-Clause license
//------------------------------------------------------------------------------
#pragma once
//#define GLM_FORCE_SWIZZLE 

#if !defined(GLCHAOSP_NO_AX)
#include "tools/oglAxes.h"
#else
#include <transforms.h>
#endif

#include "emitter.h"
#include "palettes.h"

#define SHADER_PATH "Shaders/"

#define PURE_VIRTUAL 0
#define NO_TEXTURE -1

#include "mmFBO.h"

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

enum particlesViewColor {
    paletteIndex,
    packedRGB
};

enum PS_FRAME_BUFFERS {
    FB_BILLBOARD,
    FB_POINTSPRITE
};

enum filterType { glowType_ByPass, glowType_Blur, glowType_Threshold, glowType_Bilateral };
enum subIdx {  idxSubroutine_imgTuning,
               idxSubroutine_BlurCommonPass1,
               idxSubroutine_BlurGaussPass2,
               idxSubroutine_BlurThresholdPass2,
               idxSubroutine_Bilateral,
               idxSubroutine_FXAA,
               idxSubroutine_Blit,
               idxSubroutine_MixTwoTex,
               idxSubroutine_pipRender,
               idxSubroutine_End};


class renderBaseClass;
class filtersBaseClass;
class ColorMapSettingsClass;
class imgTuningRenderClass;

struct lockedFBO {
    lockedFBO() = default;
    lockedFBO(GLuint fb_, GLuint tex_, bool locked_) : fb(fb_), tex(tex_), locked(locked_) {}
    lockedFBO& operator = (const lockedFBO& other) = default;
    GLuint fb = -1;
    GLuint tex = -1;
    bool locked = false;
};

class fboContainerClass {
public:

    fboContainerClass() = default;

    void insertItems(mmFBO &fbo, int numItems) {
        //for(int i=numItems-1; i>=0;i--)
        for(int i=0; i<numItems; i++)
            lkdFBO.emplace_back( fbo.getFB(i), fbo.getTex(i), false );
    }

    //void addMainFBO(mmFBO &fbo) { mainFBO.fb = fbo.getFB(0); mainFBO.tex = fbo.getTex(0); }
    void addMainFBO(mmFBO &fbo) { lkdFBO.emplace_back( fbo.getFB(0), fbo.getTex(0), false ); }

    GLuint getUnlockedFB() {
        for(auto &i : lkdFBO) if(!i.locked) return i.fb;
        return -1;
    }
    GLuint getUnlockedTex() {
        for(auto &i : lkdFBO) if(!i.locked) return i.tex;
        return -1;
    }
    GLuint getLockedFB() {
        for(auto &i : lkdFBO) if(i.locked) return i.fb;
        return mainFBO.fb;
    }
    GLuint getLockedTex() {
        for(auto &i : lkdFBO) if(i.locked) return i.tex;
        return mainFBO.tex;
    }

    GLuint lockItemTex(int i)   { lkdFBO[i].locked = true; return lkdFBO[i].tex; }
    GLuint lockItemFB (int i)   { lkdFBO[i].locked = true; return lkdFBO[i].fb;  }
    lockedFBO& lockItem(int i)  { lkdFBO[i].locked = true; return lkdFBO[i];  }

    lockedFBO& getItem(int i)  { return lkdFBO[i];  }

    void unlockItem(int i) { lkdFBO[i].locked = false; }

    void unlockTex(GLuint tex) { for(auto &i : lkdFBO) if(tex == i.tex) { i.locked = false;  return; } }
    void unlockFB (GLuint fb ) { for(auto &i : lkdFBO) if(fb  == i.fb ) { i.locked = false;  return; } }
    void lockTex(GLuint tex) { for(auto &i : lkdFBO) if(tex == i.tex) { i.locked = true;  return; } }
    void lockFB (GLuint fb ) { for(auto &i : lkdFBO) if(fb  == i.fb ) { i.locked = true;  return; } }

    void unlockAll() { for(auto &i : lkdFBO) i.locked = false; }

    lockedFBO& selectFBO() {
        for(auto &i : lkdFBO) if(!i.locked) { i.locked = true; return i; }
        return mainFBO;
    }
    GLuint selectFB() {
        for(auto &i : lkdFBO) if(!i.locked) { i.locked = true; return i.fb; }
        return -1;
    }
    GLuint selectTex() {
        for(auto &i : lkdFBO) if(i.locked) { i.locked = false; return i.tex; }
        return mainFBO.tex;
    }

private:
    vector<lockedFBO> lkdFBO;
    lockedFBO mainFBO;
};

//
//  blurBase
//
////////////////////////////////////////////////////////////////////////////////
class filtersBaseClass : public mainProgramObj, public uniformBlocksClass
{

struct uBlurData {
    vec4 texControls;       // align 32
    vec4 videoControls;     // align 48
    vec4 fxaaData;
    vec4 pipViewport;
    vec2 invScreenSize;
    vec2 toneMapVals;       // align 16+2N 24

    GLfloat sigmaSize;
    GLfloat sigmaRange;
    GLfloat threshold;

    GLfloat mixTexture;
    GLfloat mixBrurGlow;

    GLint filterCallType;
    GLuint toneMapping;     //bool, but need to align 4 byte

} uData;

public:

    filtersBaseClass(renderBaseClass *rndE) : renderEngine(rndE) {}
    ~filtersBaseClass() {}

    renderBaseClass* getRenderEngine() { return renderEngine; }

    void create();

    void bindingShader() { }

    void bindData(GLuint subIndex, GLuint dstFBO, GLuint srcTex, GLuint auxTex=NO_TEXTURE) {
        getUData().filterCallType = subIndex;

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dstFBO);

        bindShaderProg();
    #ifdef GLAPP_REQUIRE_OGL45
        glBindTextureUnit(0, srcTex);
        if(auxTex != NO_TEXTURE) glBindTextureUnit(1, auxTex);

        glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, GLsizei(1), &subIndex);
    #else
        glActiveTexture(GL_TEXTURE0 + srcTex);
        glBindTexture(GL_TEXTURE_2D,  srcTex);
        setUniform1i(LOCorigTexture,  srcTex);

        #if !defined(GLCHAOSP_NO_BLUR)
        if(auxTex != NO_TEXTURE) {
            glActiveTexture(GL_TEXTURE0 + auxTex);
            glBindTexture(GL_TEXTURE_2D,  auxTex);
            setUniform1i(LOCauxTexture,   auxTex);
        }
        #endif

        #if !defined(GLCHAOSP_LIGHTVER) && !defined(GLCHAOSP_NO_USES_GLSL_SUBS)
            glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, GLsizei(1), &idxSubGlowType[subIndex]);
        #endif
    #endif

    }

    void updateDataAndDraw();

    uBlurData& getUData() { return uData; }

private:
    renderBaseClass *renderEngine;

    bool actualPass;
    GLuint LOCauxTexture, LOCorigTexture;
#if !defined(GLAPP_REQUIRE_OGL45)
    GLuint idxSubGlowType[idxSubroutine_End];
#endif

private:
};


class filtersCommonsClass
{
public:
    filtersCommonsClass(filtersBaseClass *filter) : filterShader(filter) {}
    virtual ~filtersCommonsClass() {}
    virtual void bindData(GLuint subIndex, GLuint dstFBO, GLuint srcTex, GLuint auxTex=NO_TEXTURE) {
        filterShader->bindData(subIndex, dstFBO, srcTex, auxTex);
        filterShader->updateDataAndDraw(); // transfer uniform buffer to shader and draw
    }

protected:
    filtersBaseClass *filterShader;
};

//  pipRender
////////////////////////////////////////////////////////////////////////////////
class pipWndClass : public filtersCommonsClass
{
public:
    pipWndClass(filtersBaseClass *filter) :  filtersCommonsClass(filter) {}

    void pipRender(GLuint auxTex, const vec4& viewport, const vec2& transp_intens, bool border, const vec4& borderColor);

private:
};


//  blitRender
////////////////////////////////////////////////////////////////////////////////
class blitRenderClass : public filtersCommonsClass
{

public:
    blitRenderClass(filtersBaseClass *filter) : filtersCommonsClass(filter) {}

    GLuint renderOnFB(); // return locked texture associated FB
    void renderOnDB();


};


//  imgTuning
////////////////////////////////////////////////////////////////////////////////
class imgTuningDataClass
{

public:
    imgTuningDataClass() = default;

////////////////////////////
// imgTuningDataClass
    GLfloat getGamma()    { return videoControls.x; }
    GLfloat getBright()   { return videoControls.z; }
    GLfloat getContrast() { return videoControls.w; }
    GLfloat getExposure() { return videoControls.y; }

    void setGamma(GLfloat v)    { videoControls.x = v; }
    void setBright(GLfloat v)   { videoControls.z = v; }
    void setContrast(GLfloat v) { videoControls.w = v; }
    void setExposure(GLfloat v) { videoControls.y = v; }

    void setBlatComponent(GLfloat v) { texControls.z = v; }
    void setBlurComponent(GLfloat v) { texControls.x = v; }
    void setTextComponent(GLfloat v) { texControls.y = v; }

    GLfloat getTextComponent() { return texControls.y; }
    GLfloat getBlurComponent() { return texControls.x; }
    GLfloat getBlatComponent() { return texControls.z; }

    void setMixBilateral(GLfloat v) { texControls.w = v;  }
    GLfloat getMixBilateral() { return texControls.w; }

    void setDynEq(bool b)       { useDynEQ = b;        }
    bool  getDynEq() { return useDynEQ; }

    void setToneMap(bool b)     { toneMapping = b;     }
    bool getToneMap() { return toneMapping; }

    void setToneMap_A(float v)  { toneMapValsAG.x = v; }
    void setToneMap_G(float v)  { toneMapValsAG.y = v; }
    float getToneMap_A() { return toneMapValsAG.x; }
    float getToneMap_G() { return toneMapValsAG.y; }
    vec2& getToneMap_AG() { return toneMapValsAG; }


private:
    vec4 videoControls = vec4(2.3, 1.2, 0.0, 0.0); // .x = setGamma.y = setExposure, .z = setBright, .w = setContrast
    vec4 texControls = vec4(1.0, 1.0, 1.0, 0.3);   //.x = blurredTex component,  .y = OrigTex comp., z = BilateralTex comp., .w =  mix(Blur,bilateral,dot(Blur,bilateral)+w
    bool useDynEQ = false;
    bool toneMapping = false;
    vec2 toneMapValsAG = vec2(1.0, 1.0); // tonemap -> col = A * pow(col, G); -> x = A and y = G

    friend imgTuningRenderClass;
};

class imgTuningRenderClass : public filtersCommonsClass
{
public:

    imgTuningRenderClass(filtersBaseClass *filter) : filtersCommonsClass(filter) {}
    void render(imgTuningDataClass *imgT);
    void bindData(imgTuningDataClass *imgT, GLuint subIndex, GLuint dstFBO, GLuint srcTex, GLuint auxTex=NO_TEXTURE) {
        filterShader->getUData().toneMapping  = imgT->getToneMap();
        filterShader->getUData().toneMapVals  = imgT->getToneMap_AG();

        const float gamma = 1.f/imgT->getGamma();
        const float exposure =  imgT->getExposure();
        const float bright   =  imgT->getBright();
        const float contrast =  imgT->getContrast();
        filterShader->getUData().videoControls = vec4(gamma , exposure, bright, contrast);
        filterShader->getUData().texControls = vec4(vec3(imgT->texControls), (imgT->texControls.w+1.f)*.5f);

        filtersCommonsClass::bindData(subIndex, dstFBO, srcTex, auxTex);
    }

};

//  fxaa
////////////////////////////////////////////////////////////////////////////////
class fxaaDataClass
{
public:
    fxaaDataClass() = default;

    bool isOn() { return bIsOn; }
    void activate(bool b) {
        if(b==bIsOn) return;
        bIsOn=b;
    }

    void setThreshold(float f) { threshold = f; }  // fxxaData.x
    void setReductMul(float f) { reduceMul = f; }  // 1/fxxaData.y
    void setReductMin(float f) { reduceMin = f; }  // 1/fxxaData.z
    void setSpan     (float f) { span      = f; }  // fxxaData.w

    float getThreshold() { return threshold; }
    float getReductMul() { return reduceMul; }
    float getReductMin() { return reduceMin; }
    float getSpan     () { return span;      }

    float getSpanMax() { return spanMax; }
    float getMulMax()  { return mulMax;  }
    float getMinMax()  { return minMax;  }

private:
    bool bIsOn = false;

    const float spanMax = 8.f, mulMax = 64.f, minMax = 512.f;
    GLfloat span = 4.f, reduceMul = 8.f, reduceMin = 128.f, threshold = .5f;
};

class fxaaRenderClass : public filtersCommonsClass
{
public:
    fxaaRenderClass(filtersBaseClass *filter) : filtersCommonsClass(filter) {}

    void render(fxaaDataClass *fxaaData);
};

//
//  radialBlur
//
////////////////////////////////////////////////////////////////////////////////
class glowDataClass
{
public:

    glowDataClass() = default;

////////////////////////////
// dataBlurClass
    GLfloat getSigma()       { return sigmaSize; }
    void setSigma(GLfloat s)    { sigmaSize = s; }

    GLfloat getSigmaRadX()     { return sigmaRange; }
    void setSigmaRadX(float f) { sigmaRange = f;    }
    void setSigmaRad2X()       { sigmaRange = 2.0;  }
    void setSigmaRad3X()       { sigmaRange = 3.0;  }

    GLfloat getMixTexture()       { return mixTexture; }
    void setMixTexture(GLfloat s)    { mixTexture = s; }

    void setFlagUpdate();
    void clearFlagUpdate();
    void flagUpdate(bool b);

    int getGlowState() { return glowState; }
    void setGlowState(int b) { glowState = b; }

    bool isGlowOn() { return glowActive; }
    void setGlowOn(bool b) { glowActive = b; }

    float getThreshold() { return threshold; }
    void setThreshold(float f) { threshold = f; }

    float  getMixBrurGlow() { return mixBrurGlow; }
    void setMixBrurGlow(float f) {   mixBrurGlow = f; }



private:
    float sigmaSize, sigmaRange;
    int glowState = glowType_Threshold;
    bool glowActive = false;
    float threshold = .1;
    float mixBrurGlow = .5;
    GLfloat mixTexture = 0.0;
};

class glowRenderClass : public filtersCommonsClass
{
public:
    glowRenderClass(filtersBaseClass *filter) : filtersCommonsClass(filter) {}

    void render(glowDataClass *glowData);
    void bindData(glowDataClass *glowData, GLuint subIndex, GLuint dstFBO, GLuint srcTex, GLuint auxTex=NO_TEXTURE) {
        filterShader->getUData().sigmaRange   = glowData->getSigmaRadX();
        filterShader->getUData().sigmaSize    = glowData->getSigma();
        filterShader->getUData().threshold    = glowData->getThreshold();
        filterShader->getUData().mixBrurGlow  = glowData->getMixBrurGlow()*glowData->getMixBrurGlow()*glowData->getMixBrurGlow();
        filterShader->getUData().mixTexture   = (1.f + glowData->getMixTexture())*.5;
        //filterShader->getUData().filterCallType = subIndex;
        filtersCommonsClass::bindData(subIndex, dstFBO, srcTex, auxTex);
    }

};


#if !defined(GLCHAOSP_NO_MB)
class motionBlurClass;
#endif

#if !defined(GLCHAOSP_NO_BB)
class mergedRenderingClass;
#endif

//
//  tfSettinsClass
//
////////////////////////////////////////////////////////////////////////////////
class tfSettinsClass
{
protected:
struct uTFData {
    vec4 wind = vec4(vec3(0.0), 60.0);
    vec4 gravity = vec4(vec3(0.0), 60.0);
    GLfloat airFriction = 10.0;
    GLfloat diffTime = 0;
    GLfloat elapsedTime = 0;
} uData;

public:

    enum pip { noPIP, lTop, rTop, lBottom, rBottom, endValue};

    uTFData& getUdata() { return uData; }

    void setViewport(int w, int h);

    static bool cockPit() { return tfCommons.isCockPit; }
    static void cockPit(bool b) {  tfCommons.isCockPit = b; }

    static int  getPIPposition() { return tfCommons.pipPosition; }
    static void setPIPposition(int f)   { tfCommons.pipPosition = f; }

    static float getPerspAngle() { return tfCommons.perspAngle; }
    static void  setPerspAngle(float f) { tfCommons.perspAngle = f; }

    static bool invertPIP() { return tfCommons.invertPip; }
    static void invertPIP(bool b)  { tfCommons.invertPip = b; }

    static bool tfMode() { return tfCommons.isTFMode; }
    static void tfMode(bool b) {  tfCommons.isTFMode = b; }

    static float getPIPzoom() { return tfCommons.pipZoom; }
    static void setPIPzoom(float f) { tfCommons.pipZoom = f; }

    bool invertView() { return isInvertView; }
    void invertView(bool b)  { isInvertView = b; }

    void setSizeTF(float sz) { emitterPointSize=sz;}
    float getSizeTF() { return emitterPointSize; }


    quat& getRotation() { return qRot; }
    void  setRotation(const quat& q) { qRot = q; }

    float getPointSize() { return cpPointSize; }
    void  setPointSize(float f) { cpPointSize = f; }
    float getTailPosition() { return tailPosition; }
    void  setTailPosition(float f) { tailPosition = f; }
    float getMovePositionTail() { return movePositionTail; }
    void  setMovePositionTail(float f) { movePositionTail = f; }

    float getLifeTime() { return lifeTime; }
    void  setLifeTime(float f) { lifeTime = f; }
    float getLifeTimeAtten() { return lifeTimeAtten; }
    void  setLifeTimeAtten(float f) { lifeTimeAtten = f; }
    float getLifeTimeCP() { return lifeTimeCP; }
    void  setLifeTimeCP(float f) { lifeTimeCP = f; }
    float getLifeTimeAttenCP() { return lifeTimeAttenCP; }
    void  setLifeTimeAttenCP(float f) { lifeTimeAttenCP = f; }

    float getSmoothDistance() { return smoothDistance; }
    void  setSmoothDistance(float f) { smoothDistance = f; }

    void  setAirFriction(float f) { uData.airFriction =  f; }
    float getAirFriction() { return uData.airFriction; }

    void  setInitialSpeed(float f) { initialSpeed =  f; }
    float getInitialSpeed() { return initialSpeed; }

    int getSlowMotionDpS() { return slowMotionDpS; }
    void  setSlowMotionDpS(int v) { slowMotionDpS = v; }

    int getSlowMotionFSDpS() { return slowMotionFSDpS; }
    void  setSlowMotionFSDpS(int v) { slowMotionFSDpS = v; }

    float getMagnitude() { return magnitude; }
    void  setMagnitude(float f) { magnitude=f; }

    float getCpMagnitude() { return cpMagnitude; }
    void  setCpMagnitude(float f) { cpMagnitude=f; }

    float getMagnitudeInt() { return magnitudeInt; }
    void  setMagnitudeInt(float f) { magnitudeInt=f; }

    float getCpMagnitudeInt() { return cpMagnitudeInt; }
    void  setCpMagnitudeInt(float f) { cpMagnitudeInt=f; }

    float getMagnitudeAtten() { return magnitudeAtten; }
    void  setMagnitudeAtten(float f) { magnitudeAtten=f; }

    float getCpMagnitudeAtten() { return cpMagnitudeAtten; }
    void  setCpMagnitudeAtten(float f) { cpMagnitudeAtten=f; }

    bool fixedDistance() { return cpFixDistance; }
    void fixedDistance(bool b)  { cpFixDistance=b; }

    float getPipTransparence()  { return pipTransparence; };
    float getPipIntensity()     { return pipIntensity;    };
    void  setPipTransparence(float f) { pipTransparence = f; };
    void  setPipIntensity(float f)    { pipIntensity    = f; };

//const static
    static float getPerspNear() { return perspNear; }
    static int getMaxTransformedEmission()      { return maxEmission; }
    static int getMaxTransformedEmissionFrame() { return maxEmissionFrame; }

//Feedback Funcs
    void setTransformedEmission(int i)  {  transformedEmission =  i; }    //getEmittedParticles
    int  getTransformedEmission() { return transformedEmission; }
    //void setMaxEmissionFrame(int i)  {  maxEmissionFrame =  i; }    //getEmittedParticles
    //static int getMaxEmissionFrame() { return maxEmissionFrame; }

    vec2 &getViewportSize() { return viewportSize; }
    void setViewportSize(const vec2 &v) { viewportSize = v; }

    vec4 &getViewportLimits() { return viewportLimits; }
    void setViewportLimits(const vec4 &v) { viewportLimits = v; }

    vec4 &getPipBorderColor() { return pipBorderColor; }
    void setPipBorderColor(const vec4 &v) { pipBorderColor = v; }

    bool borderActive() { return border; }
    void borderActive(bool b) { border = b; }
private:

    struct tfCommonsStruct {
        bool isTFMode = false;      // TransformFeedback Mode
        bool isCockPit = false;
        int pipPosition = noPIP;
        float perspAngle = 60.f;
        bool invertPip = false;
        float pipZoom = .4; // 1.0 -> 1/4 Window
    } static tfCommons;

    float pipTransparence = .5f;
    float pipIntensity    = 2.f;

    vec2 viewportSize;
    vec4 viewportLimits;
    float smoothDistance = .250;
    float lifeTime = 75.0;
    float lifeTimeAtten = .3;
    float lifeTimeCP = 75.0;
    float lifeTimeAttenCP = .3;
    float magnitude = 1.5;
    float magnitudeInt = .25;
    float magnitudeAtten = .15;
    float movePositionTail = 0;
    bool isInvertView = false;
    float tailPosition = .25;
    float  emitterPointSize = 1;
    float cpPointSize = 7.f;
    float cpMagnitude = 1.0;
    float cpMagnitudeInt = .2;
    float cpMagnitudeAtten = .25;
    bool cpFixDistance = true;
    quat qRot = quat(1.0f, 0.0f, 0.0f, 0.0f);
    int slowMotionDpS = 100; //tfSettings DotPerSec
    int slowMotionFSDpS = 5000; //FullScreen DotPerSec
    vec4 pipBorderColor = vec4(.1f, .1f, .5f, 1.f);
    bool border = true;

    static constexpr float perspNear = .001f;

//Feedback Data
    int transformedEmission = 4;
    static const int maxEmission = 50;
    static const int maxEmissionFrame = 1000*maxEmission;
    float initialSpeed = 1.0;

    friend transformedEmitterClass;
};


//
//  shadowClass
//
////////////////////////////////////////////////////////////////////////////////
class shadowClass: public mainProgramObj
{
public:
    shadowClass(renderBaseClass* ptrRE);

    void create();

    void bindRender();
    void render();
    void releaseRender();
    void resize(int w, int h) { const int detail = theApp->useDetailedShadows() ? 2 : 1; fbo.reSizeFBO(w*detail, h*detail); }

    mmFBO &getFBO() { return fbo; }

private:
    mmFBO fbo;
    renderBaseClass *renderEngine;
};

//
//  ambientOcclusionClass
//
////////////////////////////////////////////////////////////////////////////////
class ambientOcclusionClass : public mainProgramObj
{
public:
    ambientOcclusionClass(renderBaseClass *ptrRE);

    void create();

    void bindRender(particlesBaseClass *particle, GLuint fbIdx, const vec4 &bkgColor);
    void render();
    void releaseRender();

    //mmFBO &getFBO() { return fbo; }

#if !defined(GLAPP_REQUIRE_OGL45)
    //GLuint getLocPrevData() { return locPrevData; }
#endif

private:
    //mmFBO fbo;
    renderBaseClass *renderEngine;
    std::vector<vec3> ssaoKernel;
    GLuint ssaoKernelTex;
    GLuint noiseTexture;
    const int kernelSize = 64;
#if !defined(GLAPP_REQUIRE_OGL45)
    GLuint locNoiseTexture, locKernelTexture, locZTex; //locPrevData,
    GLuint bindIDX;
#endif
    friend particlesBaseClass;
};

//
//  postRenderingClass
//
////////////////////////////////////////////////////////////////////////////////
class postRenderingClass : public mainProgramObj
{
public:
    postRenderingClass(renderBaseClass *ptrRE);

    void create();

    GLuint bindRender(particlesBaseClass *particle, GLuint fbIdx, const vec4 &bkgColor);
    void render();
    void releaseRender();


    //mmFBO &getFBO() { return fbo; }

#if !defined(GLAPP_REQUIRE_OGL45)
    GLuint getLocPrevData() { return locPrevData; }
    GLuint getLocAOTex() { return locAOTex; }
    GLuint getLocShadowTex() { return locShadowTex; }
    GLuint getSubIdx(int i) { return idxSubLightModel[i]; }
#endif

private:
    //mmFBO fbo;
    renderBaseClass *renderEngine;
#if !defined(GLAPP_REQUIRE_OGL45)
    GLuint locSubLightModel;
    GLuint locAOTex, locPrevData, locShadowTex, locTexBaseColor, locZTex;
    GLuint idxSubLightModel[3];
#endif
    friend particlesBaseClass;
};



class fxaaDataClass;
//
//  renderBase
//
////////////////////////////////////////////////////////////////////////////////
class renderBaseClass
{
protected:

struct uClippingPlanes {
    vec4    clipPlane[3]     = { {1.f, 0.f, 0.f, 0.0f}, {0.f, 1.f, 0.f, 0.0f}, {0.f, 0.f, 1.f, 0.0f} };
    vec4    boundaryColor[3] = { {1.f, 0.f, 0.f, 0.5f}, {0.f, 1.f, 0.f, 0.5f}, {0.f, 0.f, 1.f, 0.5f} };
    uint32_t    planeActive[4]   = { false, false, false, 0 }; // 4: block rounded
    uint32_t    colorActive[4]   = { true , true , true , 0 };
    GLfloat     thickness = .005;
    uint32_t    additiveSpace = true;
    uint32_t    atLeastOneActive = false;

    void buildInvMV_forPlanes(particlesBaseClass *particles);

} uPlanes;
public:
    enum bind { planesIDX = 9 };

    renderBaseClass();

    void create();
    void buildFBO();
    void resizeShadow(int w, int h) {}

    enum { noShowAxes, showAxesToViewCoR, showAxesToSetCoR };

    virtual ~renderBaseClass();

    void setRenderMode(int which);
    int getRenderMode() { return whichRenderMode; }

    mmFBO &getRenderFBO() { return renderFBO; };
    mmFBO &getAuxFBO() { return auxFBO; };
    //mmFBO &getMSAAFBO() { return msaaFBO; };

    int getWidth()  { return getRenderFBO().getSizeX(); }
    int getHeight() { return getRenderFBO().getSizeY(); }

    cmContainerClass &getColorMapContainer() { return colorMapContainer; }
    fboContainerClass &getFboContainer() { return fboContainer; }


    bool checkFlagUpdate() { return flagUpdate; }
    void setFlagUpdate() { flagUpdate = true; }
    void clearFlagUpdate() { flagUpdate = false; }

    transformsClass *getTMat() { return &tMat; }
    transformsClass *getCockPitTMat() { return &cpTMat; }

    void showAxes(int b) { axesShow = b; }
    int showAxes() { return axesShow; }

    pipWndClass *getPipWnd() { return pipWnd; }
#if !defined(GLCHAOSP_NO_BB)
    mergedRenderingClass *getMergedRendering() { return mergedRendering; }
#endif

#if !defined(GLCHAOSP_NO_MB)
    motionBlurClass *getMotionBlur() { return motionBlur; }
#endif

#if !defined(GLCHAOSP_NO_AX)
    oglAxes *getAxes() { return axes; }
#endif

#if !defined(GLCHAOSP_LIGHTVER)
    void viewObjON()  { idxViewOBJ = GLuint(particlesViewColor::packedRGB); }
    void viewObjOFF() { idxViewOBJ = GLuint(particlesViewColor::paletteIndex); }
    GLuint viewingObj() { return idxViewOBJ; }

    bool wantPlyObjColor() { return plyObjGetColor; }
    void wantPlyObjColor(bool b) { plyObjGetColor = b; }
#endif

    int getWhitchRenderMode() { return whichRenderMode; }

    VertexShader* getCommonVShader() { return &commonVShader; }
    GLuint getSeparableVertex() { return separableVertex; }
    postRenderingClass* getPostRendering() { return postRendering; }
    ambientOcclusionClass* getAO() { return ambientOcclusion; }
    shadowClass* getShadow() { return shadow; }
    filtersBaseClass* getFilter()  { return filterBase; }
    blitRenderClass* getBlitRender() { return blitRender; }
    imgTuningRenderClass* getImgTuningRender() { return imgTuningRender; }
    fxaaRenderClass* getFXAARender() { return fxaaRender; }
    glowRenderClass* getGlowRender() { return glowRender; }


    int getBlendArrayElements() { return blendArray.size(); }
    std::vector<GLuint> &getBlendArray() { return blendArray; }
    std::vector<const char *> &getBlendArrayStrings() { return blendingStrings; }
    std::vector<GLuint> &getBlendEqFunc() { return blendEqFunc; }
    std::vector<const char *> &getBlendEqStrings() { return blendEqStrings; }

    vec4 &getClippingPlane(int i) { return clippingPlane[i]; }
    void setClippingPlane(const vec4 &v, int i) { clippingPlane[i] = v; }

    uClippingPlanes &getUPlanes() { return uPlanes; }

    int  getNumAuxFBO() { return numAuxFBO; }

protected:
    int whichRenderMode;

    pipWndClass *pipWnd = nullptr;
    blitRenderClass* blitRender = nullptr;
    imgTuningRenderClass* imgTuningRender = nullptr;
    fxaaRenderClass* fxaaRender = nullptr;
    glowRenderClass* glowRender = nullptr;


#if !defined(GLCHAOSP_NO_MB)
    motionBlurClass *motionBlur = nullptr;
#endif
#if !defined(GLCHAOSP_NO_BB)
    mergedRenderingClass *mergedRendering = nullptr;
#endif
#if !defined(GLCHAOSP_NO_AX)
    oglAxes *axes;
#endif

#if !defined(GLCHAOSP_LIGHTVER)
    GLuint idxViewOBJ = 0;
    bool plyObjGetColor = true;
#endif
    int axesShow = noShowAxes;

    cmContainerClass colorMapContainer;
    fboContainerClass fboContainer;

    vec4 clippingPlane[3] = { vec4(1.f, 0.f, 0.f, 0.f), vec4(0.f, 1.f, 0.f, 0.f), vec4(0.f, 0.f, 1.f, 0.f) };

    bool flagUpdate;

    mmFBO renderFBO; // multi FB con depth
    mmFBO auxFBO;
//         msaaFBO;

    transformsClass tMat, cpTMat;

    VertexShader commonVShader;
    GLuint separableVertex;
    postRenderingClass *postRendering = nullptr;
    ambientOcclusionClass *ambientOcclusion = nullptr;
    shadowClass* shadow = nullptr;
    filtersBaseClass *filterBase = nullptr;

#ifdef GLCHAOSP_LIGHTVER
    int numAuxFBO = 4;
#else
    int numAuxFBO = 4;
#endif


    std::vector<GLuint> blendArray;
    std::vector<const char *> blendingStrings;
    std::vector<GLuint> blendEqFunc;
    std::vector<const char *> blendEqStrings;


private:
};

struct overlapWindow {
    vec4 viewport;
    vec4 color;
    float transparence;
    float intensity;
    bool border;
};




#if !defined(GLCHAOSP_NO_BB)
// mergedRendering
////////////////////////////////////////////////////////////////////////////////
class mergedRenderingClass : public filtersCommonsClass
{
public:
    mergedRenderingClass(filtersBaseClass *filter) : filtersCommonsClass(filter), mixingVal(0.0) {}

    void mixRender(GLuint auxTex);

    void setMixingVal(GLfloat f) { mixingVal = f; }
    GLfloat getMixingVal() { return mixingVal; }

private:
    GLfloat mixingVal;
};

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

        //mBlurFBO.declareFBO(2, renderEngine->getWidth(), renderEngine->getHeight(), theApp->getFBOInternalPrecision());
        mBlurFBO.buildFBO(2, renderEngine->getWidth(), renderEngine->getHeight(), theApp->getFBOInternalPrecision());
    }

    GLuint render(GLuint renderedTex);

    void create(); 

    void rotateBuffer() { rotationBuff ^= 1; }

    mmFBO &getFBO() { return mBlurFBO; };
#if !defined(GLAPP_REQUIRE_OGL45)
    void updateAccumMotionTex(GLuint tex)    { setUniform1i(LOCaccumMotion, tex); }
    void updateSourceRenderedTex(GLuint tex) { setUniform1i(LOCsourceRendered, tex); }
#endif

    void updateBlurIntensity() { setUniform1f(LOCblurIntensity, .5 + blurIntensity*.5); }

    bool Active() { return isActive; }
    void Active(bool b) { 
        if(b==isActive) return;
        if(b) {
            mBlurFBO.reBuildFBO(2, renderEngine->getWidth(), renderEngine->getHeight(), theApp->getFBOInternalPrecision());            
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
        ORIG_TEXTURE,
        MODF_TEXTURE
    };

    colorMapTexturedClass(particlesBaseClass *p) {
        particles = p;
        flagUpdate = true;

        //cmTex.buildMultiDrawFBO(1,256,1);
        cmTex.buildFBO(1,256,1, theApp->getFBOInternalPrecision());        
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

    //mmFBO &getFBO() { return cmTex; }

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
    ColorMapSettingsClass(particlesBaseClass *p) : colorMapTexturedClass(p), velIntensity(.3f) { setFlagUpdate(); }

    void selected(int i)       { selection = i;    }
    int  selected()            { return selection; }

    float getVelIntensity() { return velIntensity; }    
    void setVelIntensity(float f);

protected:
    int selection;
    float velIntensity;

private:
};


//
// ParticlesBase
//
////////////////////////////////////////////////////////////////////////////////
class particlesBaseClass : public mainProgramObj, public uniformBlocksClass, public virtual renderBaseClass 
{
protected:

struct uParticlesData {
    vec3    lightDir; // align 0
    GLfloat lightDiffInt = 1.f;
    vec3    lightColor = vec3(1.f);           // align 16
    GLfloat lightSpecInt = .75f;
    vec2    scrnRes;
    vec2    invScrnRes;
    vec3    rotCenter = { 0.f, 0.f, 0.f }; 
    GLfloat lightAmbInt = .1f;
    GLfloat lightShinExp = 10.f;
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
    GLfloat halfTanFOV;
    GLfloat velocity;
    GLfloat ySizeRatio = 1.0;
    GLfloat ptSizeRatio = 1.0;
    GLfloat pointspriteMinSize = 1.0;
    GLfloat ggxRoughness = .75f;
    GLfloat ggxFresnel = .5f;
    GLfloat shadowSmoothRadius = 2.f;
    GLfloat shadowGranularity = 1.f;
    GLfloat shadowBias = 0.0;
    GLfloat shadowDarkness = 0.0;
    GLfloat shadowDetail = 1.0;
    GLfloat aoRadius =  .5;
    GLfloat aoBias = .025;
    GLfloat aoDarkness = .25;
    GLfloat aoMul = 1.0;
    GLfloat aoModulate = 1.0;
    GLfloat aoStrong = 0.0;
    GLfloat dpAdjConvex = .250;
    GLfloat dpNormalTune = .025;
    GLfloat elapsedTime = 0.0;
    GLfloat lifeTime = 10.0;
    GLfloat lifeTimeAtten = .1;
    GLfloat smoothDistance = 0.0;
    GLfloat vpReSize = 1.0;
    GLfloat magnitude = 5.0;
    GLfloat magnitudeInt = .1;
    GLfloat invMagnitudeAtten = 1.0/.2;
    GLuint  slowMotion = 0;
// __APPLE__ & GL_ES
    GLuint lightModel = modelBlinnPhong - modelOffset;
    GLuint lightActive = GLuint(on);
    GLuint pass = 0;
    GLuint renderType = 0;
    GLint colorizingMethod = 0;
} uData;

public:
    enum lightIDX { off, on };
    enum lightMDL { modelOffset = 5, modelPhong=modelOffset, modelBlinnPhong, modelGGX };
    enum pixColIDX { pixOffset, pixBlendig=pixOffset, pixDirect, pixAO, pixDR };
    enum subsLoc { lightModel, pixelColor };

    particlesBaseClass ()  {

        colorMap        = new ColorMapSettingsClass(this);

        dotTex.build(DOT_TEXT_SHFT, vec4(.7f, 0.f, .3f, 0.f), dotsTextureClass::dotsAlpha);
        selectColorMap(0);

    }

    ~particlesBaseClass ()  {
        delete colorMap;
    }

    void clearFB(GLuint fbID) {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbID);
        glViewport(0,0, getWidth(), getHeight());
        glClearBufferfv(GL_COLOR,  0, value_ptr(backgroundColor()));
    }


    void setCommonData() {
#ifdef GLAPP_REQUIRE_OGL45
    uniformBlocksClass::create(GLuint(sizeof(uParticlesData)), (void *) &uData);
    getPlanesUBlock().create(sizeof(uClippingPlanes), &uPlanes, GLuint(renderBaseClass::bind::planesIDX));

#else
    USE_PROGRAM

    uniformBlocksClass::create(GLuint(sizeof(uParticlesData)), (void *) &uData, getProgram(), "_particlesData");
    getPlanesUBlock().create(sizeof(uClippingPlanes), &uPlanes, getProgram(), "_clippingPlanes", GLuint(renderBaseClass::bind::planesIDX));
    getTMat()->blockBinding(getProgram());

    locPaletteTex = getUniformLocation("paletteTex" );
    locDotsTex    = getUniformLocation("tex"); 

    #if !defined(GLCHAOSP_LIGHTVER) && !defined(GLCHAOSP_NO_USES_GLSL_SUBS)
        locSubPixelColor = glGetSubroutineUniformLocation(getProgram(), GL_FRAGMENT_SHADER, "pixelColor");
        locSubLightModel = glGetSubroutineUniformLocation(getProgram(), GL_FRAGMENT_SHADER, "lightModel");
        const int numSub = 2;
        if(locSubPixelColor>=numSub || locSubPixelColor == -1) locSubPixelColor = 0;
        if(locSubLightModel>=numSub || locSubLightModel == -1) locSubLightModel = 0;

        idxSubPixelColor[pixBlendig] = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "pixelColorBlending");
        idxSubPixelColor[pixDirect ] = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "pixelColorDirect");
        idxSubPixelColor[pixAO     ] = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "pixelColorAO");
        idxSubPixelColor[pixDR     ] = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "pixelColorDR");

        idxSubLightModel[0] = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "specularPhong");
        idxSubLightModel[1] = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "specularBlinnPhong");
        idxSubLightModel[2] = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "specularGGX");

        idxSubOBJ = glGetSubroutineIndex(getProgram(),GL_VERTEX_SHADER, "objColor"); 
        idxSubVEL = glGetSubroutineIndex(getProgram(),GL_VERTEX_SHADER, "velColor"); 
    #endif
#endif
    }

    void selectSubroutine() {
        GLuint subIDX[2];        
#if defined(GLCHAOSP_LIGHTVER)
        const GLuint subVtxIdx = particlesViewColor::paletteIndex;
#else
        const GLuint subVtxIdx = idxViewOBJ && plyObjGetColor ? particlesViewColor::packedRGB : particlesViewColor::paletteIndex;
#endif
        uData.colorizingMethod = subVtxIdx;
#ifdef GLAPP_REQUIRE_OGL45 
        glUniformSubroutinesuiv(GL_VERTEX_SHADER, GLsizei(1), &subVtxIdx);

        subIDX[subsLoc::pixelColor] = uData.renderType;
        subIDX[subsLoc::lightModel] = uData.lightModel + lightMDL::modelOffset;
        glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, GLsizei(2), subIDX);
#else
    #if !defined(GLCHAOSP_LIGHTVER) && !defined(GLCHAOSP_NO_USES_GLSL_SUBS)
        subIDX[locSubPixelColor] = idxSubPixelColor[uData.renderType-pixColIDX::pixOffset];
        subIDX[locSubLightModel] = idxSubLightModel[uData.lightModel];

        glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, GLsizei(2), subIDX);
        glUniformSubroutinesuiv(GL_VERTEX_SHADER, GLsizei(1), subVtxIdx == particlesViewColor::packedRGB ? &idxSubOBJ : &idxSubVEL);
    #endif
#endif
    }

    void updateCommons() {

    }

#ifndef GLAPP_REQUIRE_OGL45
    void updatePalTex()  { setUniform1i(locPaletteTex, getCMSettings()->getModfTex()); }
#endif

    uParticlesData &getUData() { return uData; }

    GLuint render(GLuint fbOut, emitterBaseClass *em, bool isFullScreen = true, bool cpitView = false);

    GLuint getDstBlend() { return dstBlendAttrib; }
    GLuint getSrcBlend() { return srcBlendAttrib; }
    GLuint getDstBlendA() { return dstBlendAttribA; }
    GLuint getSrcBlendA() { return srcBlendAttribA; }
    GLuint getBlendEqRGB() { return blendEqRGB; }
    GLuint getBlendEqAlpha() { return blendEqAlpha; }

    void setDstBlend(GLuint v) {  dstBlendAttrib = v; }
    void setSrcBlend(GLuint v) {  srcBlendAttrib = v; }
    void setDstBlendA(GLuint v) {  dstBlendAttribA = v; }
    void setSrcBlendA(GLuint v) {  srcBlendAttribA = v; }
    void setBlendEqRGB(GLuint v)   { blendEqRGB   = v; }
    void setBlendEqAlpha(GLuint v) { blendEqAlpha = v; }

    void setSize(float sz) { ptSize=sz;}
    float getSize() { return ptSize; }

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
    vec3 *getSelectedColorMap_vec3() { return colorMapContainer.getRGB_pv3(colorMap->selected()); }
    float *getColorMap_pf3(int i) { return colorMapContainer.getRGB_pf3(i); }
    const char *getColorMap_name()      { return colorMapContainer.getName(colorMap->selected()); }
    const char *getColorMap_name(int i) { return colorMapContainer.getName(i); }

    bool getDepthState() { return depthBuffActive; }
    bool getBlendState() { return blendActive; }
    bool getLightState() { return bool(getUData().lightActive); }
    
    void setDepthState(bool b) { depthBuffActive = b; }
    void setBlendState(bool b) { blendActive = b; }
    void setLightState(bool b) { uData.lightActive = b ? GLuint(on) : GLuint(off); }

    void dstBlendIdx(int i) { dstIdxBlendAttrib = i; }
    int  dstBlendIdx() { return dstIdxBlendAttrib; }
    void dstBlendIdxA(int i) { dstIdxBlendAttribA = i; }
    int  dstBlendIdxA() { return dstIdxBlendAttribA; }
    int rgbEqIdx() { return rgbIdxEq; }
    void rgbEqIdx(int i) { rgbIdxEq = i; }

    void srcBlendIdx(int i) { srcIdxBlendAttrib = i; }
    int  srcBlendIdx() { return srcIdxBlendAttrib; }
    void srcBlendIdxA(int i) { srcIdxBlendAttribA = i; }
    int  srcBlendIdxA() { return srcIdxBlendAttribA; }

    int aEqIdx() { return aIdxEq; }
    void aEqIdx(int i) { aIdxEq = i; }


    void setHermiteVals(const vec4 &v) { dotTex.setHermiteVals(v); }
    vec4& getHermiteVals() { return dotTex.getHermiteVals(); }

    void backgroundColor(const vec4 &v) { backgrndColor = v; }
    vec4& backgroundColor() { return backgrndColor; }

    int getLightModel() { return uData.lightModel; }
    void setLightModel(int l) { uData.lightModel = l; }

    bool postRenderingActive() { return isPostRenderingActive; }
    void postRenderingActive(bool b) { isPostRenderingActive = b; }

    dotsTextureClass& getDotTex() { return dotTex; }

    vec3& getLightDir() { return lightVec; }
    void setLightDir(const vec3& v) { lightVec = v; setFlagUpdate(); }

    // Ambient Ocllusion
    /////////////////////////////////////////////
    bool useAO() { return usingAO; }
    void useAO(bool b) { usingAO = b; }

    float getAOStrong() { return uData.aoStrong; }
    void setAOStrong(float f)  { uData.aoStrong = f; }

    float getAORadius() { return uData.aoRadius; }
    void  setAORadius(float f) { uData.aoRadius = f;  }

    float getAOBias() { return uData.aoBias; }
    void  setAOBias(float f) { uData.aoBias = f; }

    float getAODarkness() { return uData.aoDarkness; }
    void  setAODarkness(float f) { uData.aoDarkness = f;  }

    float getAOMul() { return uData.aoMul; }
    void  setAOMul(float f) { uData.aoMul = f; }

    float getAOModulate() { return uData.aoModulate; }
    void  setAOModulate(float f) { uData.aoModulate = f; }
    // Shadow
    /////////////////////////////////////////////
    bool useShadow() { return usingShadow; }
    void useShadow(bool b) { usingShadow = b; }

    float getShadowRadius() { return uData.shadowSmoothRadius; }
    void  setShadowRadius(float f) { uData.shadowSmoothRadius = f; }

    float getShadowGranularity() { return uData.shadowGranularity; }
    void  setShadowGranularity(float f) { uData.shadowGranularity = f; }

    float getShadowBias() { return uData.shadowBias; }
    void  setShadowBias(float f) { uData.shadowBias = f; }

    float getShadowDarkness() { return uData.shadowDarkness; }
    void  setShadowDarkness(float f) { uData.shadowDarkness = f; }

    bool autoLightDist() { return autoLight; }
    void autoLightDist(bool b) { autoLight = b; }

    bool useScatterdShadow() { return scatteredShadow; }
    void useScatterdShadow(bool b) {  scatteredShadow = b; }
    // Depth Rendering
    /////////////////////////////////////////////
    float dpAdjConvex() { return uData.dpAdjConvex; }
    void dpAdjConvex(float f) { uData.dpAdjConvex = f; }

    float dpNormalTune() { return uData.dpNormalTune; }
    void dpNormalTune(float f) { uData.dpNormalTune = f; }

    uniformBlocksClass &getPlanesUBlock() { return planesUBlock; }

    tfSettinsClass& getTFSettings() { return tfSettings; }

    glowDataClass* getGlowData() { return &glowData; }
    fxaaDataClass* getFXAAData() { return &fxaaData; }
    imgTuningDataClass* getImgTuningData() { return &imgTuning; }

protected:
    uniformBlocksClass planesUBlock;

    GLuint dstBlendAttrib = GL_ONE, srcBlendAttrib;
    GLuint dstBlendAttribA, srcBlendAttribA;
    GLuint blendEqRGB = GL_FUNC_ADD, blendEqAlpha = GL_FUNC_ADD;
    int dstIdxBlendAttrib = 0, srcIdxBlendAttrib = 0;
    int dstIdxBlendAttribA = 0, srcIdxBlendAttribA = 0;
    int rgbIdxEq = 0, aIdxEq = 0;

    ColorMapSettingsClass *colorMap;
    dotsTextureClass dotTex;

    glowDataClass glowData;
    fxaaDataClass fxaaData;
    imgTuningDataClass imgTuning;

#if !defined(GLAPP_REQUIRE_OGL45)
    GLuint locSubLightModel, locSubPixelColor;
    GLuint locDotsTex, locPaletteTex;
    GLuint idxSubOBJ, idxSubVEL;
    GLuint idxSubLightModel[3] = { 0, 0, 0 };
    GLuint idxSubPixelColor[4]  = { 0, 0, 0, 0 };
#endif
    vec3 lightVec = vec3(50.f, 15.f, 25.f);
    vec4 backgrndColor = vec4(0.f);

    bool depthBuffActive = true;
    bool blendActive = true;
    bool isPostRenderingActive = false;
    bool usingAO = false;
    bool usingShadow = false;
    bool autoLight = true;
    bool scatteredShadow = false;

    float ptSize;
    tfSettinsClass tfSettings;


private:


friend class particlesDlgClass;
};


inline void renderBaseClass::uClippingPlanes::buildInvMV_forPlanes(particlesBaseClass *particles) {
    if(planeActive[0] || planeActive[1] || planeActive[2]) {
        particles->getTMat()->buid_invMV(); 
        atLeastOneActive = true;
    } else atLeastOneActive = false;
    clipPlane[0] = particles->getClippingPlane(0);
    clipPlane[1] = particles->getClippingPlane(1);
    clipPlane[2] = particles->getClippingPlane(2);
}

inline GLuint colorMapTexturedClass::getOrigTex() { return particles->getPaletteTexID(); }
inline void ColorMapSettingsClass::setVelIntensity(float f) { velIntensity = f; setFlagUpdate(); }



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
    void initShader();

private:
};

#if !defined(GLCHAOSP_NO_BB)
//
// Billboard
//
////////////////////////////////////////////////////////////////////////////////
class shaderBillboardClass : public particlesBaseClass 
{
public:
    shaderBillboardClass();

    shaderBillboardClass *getPtr() { return this; }
    void initShader();

private:
};
#endif

