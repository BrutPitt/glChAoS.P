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
#pragma once
//#define GLM_FORCE_SWIZZLE 

#if !defined(GLCHAOSP_LIGHTVER)
#include "tools/oglAxes.h"
#else
#include <transforms.h>
#endif

#include "emitter.h"
#include "palettes.h"

#define SHADER_PATH "Shaders/"

#define PURE_VIRTUAL 0

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
    GLfloat getSigma()       { return sigmaSize; }
    void setSigma(GLfloat s)    { sigmaSize = s; setFlagUpdate(); }

    GLfloat getSigmaRadX()     { return sigmaRange; }
    void setSigmaRadX(float f) { sigmaRange = f; setFlagUpdate(); }
    void setSigmaRad2X()       { sigmaRange = 2.0; setFlagUpdate(); }
    void setSigmaRad3X()       { sigmaRange = 3.0; setFlagUpdate(); }

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

    float  getMixBrurGlow() { return mixBrurGlow; }
    void setMixBrurGlow(float f) {   mixBrurGlow = f; setFlagUpdate(); }

protected:
    float sigmaSize, sigmaRange;
    int glowState = glowType_Threshold;
    bool glowActive = false;
    float threshold = .1;
    float mixBrurGlow = .5;
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
    vec4 texControls;       // align 32
    vec4 videoControls;     // align 48
    vec2 invScreenSize;
    vec2 toneMapVals;       // align 16+2N 24

    GLfloat sigmaSize;
    GLfloat sigmaRange;
    GLfloat threshold;
    GLuint toneMapping;     //bool, but need to align 4 byte


    GLfloat mixTexture;
    GLfloat mixBrurGlow;
    
    GLint blurCallType;

} uData;

public:

    BlurBaseClass() {}
    ~BlurBaseClass() {}

    void create();

    void glowPass(GLuint sourceTex, GLuint fbo, GLuint subIndex);

    void updateData(GLuint subIndex);

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

    enum pip { noPIP, lTop, rTop, lBottom, rBottom, splitView };

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

//const static
    static float getPerspNear() { return perspNear; }
    static int getMaxTransformedEmission()      { return maxEmission; }
    static int getMaxTransformedEmissionFrame() { return maxEmissionFrame; }

//Feedback Funcs
    void setTransformedEmission(int i)  {  transformedEmission =  i; }    //getEmittedParticles
    int  getTransformedEmission() { return transformedEmission; }
    //void setMaxEmissionFrame(int i)  {  maxEmissionFrame =  i; }    //getEmittedParticles
    //static int getMaxEmissionFrame() { return maxEmissionFrame; }

    ivec4 &getViewportSize() { return viewportSize; }
    void setViewportSize(const ivec4 &v) { viewportSize = v; }

    vec4 &getPipBkgrndColor() { return pipBkgrndColor; }
    void setPipBkgrndColor(const vec4 &v) { pipBkgrndColor = v; }
private:

    struct tfCommonsStruct {
        bool isTFMode = false;      // TransformFeedback Mode
        bool isCockPit = false;
        int pipPosition = noPIP;
        float perspAngle = 60.f;
        bool invertPip = false;
        float pipZoom = .5; // 1.0 -> 1/4 Window
    } static tfCommons;

    ivec4 viewportSize = ivec4(0, 0, 100, 100);
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
    vec4 pipBkgrndColor = vec4(.003f, .003f, .007f, 1.f);

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

    mmFBO &getFBO() { return fbo; }

#if !defined(GLAPP_REQUIRE_OGL45)
    GLuint getLocPrevData() { return locPrevData; }
#endif

private:
    mmFBO fbo;
    renderBaseClass *renderEngine;
    std::vector<vec3> ssaoKernel;
    GLuint ssaoKernelTex;
    GLuint noiseTexture;
    const int kernelSize = 64;
#if !defined(GLAPP_REQUIRE_OGL45)
    GLuint locNoiseTexture, locPrevData, locKernelTexture, locZTex;
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

    void bindRender(particlesBaseClass *particle, GLuint fbIdx, const vec4 &bkgColor);
    void render();
    void releaseRender();


    mmFBO &getFBO() { return fbo; }

#if !defined(GLAPP_REQUIRE_OGL45)
    GLuint getLocPrevData() { return locPrevData; }
    GLuint getLocAOTex() { return locAOTex; }
    GLuint getLocShadowTex() { return locShadowTex; }
    GLuint getSubIdx(int i) { return idxSubLightModel[i]; }
#endif

private:
    mmFBO fbo;
    renderBaseClass *renderEngine;
#if !defined(GLAPP_REQUIRE_OGL45)
    GLuint locSubLightModel;
    GLuint locAOTex, locPrevData, locShadowTex, locTexBaseColor, locZTex;
    GLuint idxSubLightModel[3];
#endif
    friend particlesBaseClass;
};

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
    //mmFBO &getMSAAFBO() { return msaaFBO; };
    int getWidth()  { return getRenderFBO().getSizeX(); }
    int getHeight() { return getRenderFBO().getSizeY(); }

 
    cmContainerClass &getColorMapContainer() { return colorMapContainer; }


    bool checkFlagUpdate() { return flagUpdate; }
    void setFlagUpdate() { flagUpdate = true; }
    void clearFlagUpdate() { flagUpdate = false; }

    transformsClass *getTMat() { return &tMat; }
    transformsClass *getCockPitTMat() { return &cpTMat; }

    void showAxes(int b) { axesShow = b; }
    int showAxes() { return axesShow; }

#if !defined(GLCHAOSP_LIGHTVER)
    mergedRenderingClass *getMergedRendering() { return mergedRendering; }
    motionBlurClass *getMotionBlur() { return motionBlur; }
    oglAxes *getAxes() { return axes; }

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

    
    int getBlendArrayElements() { return blendArray.size(); }
    std::vector<GLuint> &getBlendArray() { return blendArray; }
    std::vector<const char *> &getBlendArrayStrings() { return blendingStrings; }

    vec4 &getClippingPlane(int i) { return clippingPlane[i]; }
    void setClippingPlane(const vec4 &v, int i) { clippingPlane[i] = v; }

    uClippingPlanes &getUPlanes() { return uPlanes; }


protected:
    int whichRenderMode;    

#if !defined(GLCHAOSP_LIGHTVER)
    motionBlurClass *motionBlur = nullptr;
    mergedRenderingClass *mergedRendering = nullptr;

    oglAxes *axes;

    GLuint idxViewOBJ = 0;
    bool plyObjGetColor = true;
#endif
    int axesShow = noShowAxes;

    cmContainerClass colorMapContainer;

    vec4 clippingPlane[3] = { vec4(1.f, 0.f, 0.f, 0.f), vec4(0.f, 1.f, 0.f, 0.f), vec4(0.f, 0.f, 1.f, 0.f) };

    bool flagUpdate;

    mmFBO renderFBO;
    mmFBO drawFBO;
//         msaaFBO;

    transformsClass tMat, cpTMat;

    VertexShader commonVShader;
    GLuint separableVertex;
    postRenderingClass *postRendering = nullptr;
    ambientOcclusionClass *ambientOcclusion = nullptr;
    shadowClass* shadow = nullptr;

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
#if !defined(GLCHAOSP_LIGHTVER_BLUR)
        glowFBO.buildFBO(2, renderEngine->getWidth(), renderEngine->getHeight(), theApp->getFBOInternalPrecision());
#else
        glowFBO.buildFBO(1, renderEngine->getWidth(), renderEngine->getHeight(), theApp->getFBOInternalPrecision());
#endif
    }

    void render(GLuint sourceTex, GLuint fbOut) {

#if !defined(GLCHAOSP_LIGHTVER_BLUR)
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
        fbo.declareFBO(1, renderEngine->getWidth(), renderEngine->getHeight(), theApp->getFBOInternalPrecision());
        create();
    }

    void create();
    GLuint render(GLuint texIn, bool useFB=false);

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
                                   1.f/reduceMul, 
                                   1.f/reduceMin, 
                                   span);
        setUniform4fv(_fxaaData, 1, value_ptr(fxaaData));    
    }

    mmFBO &getFBO() { return fbo; }

    float getSpanMax() { return spanMax; }
    float getMulMax() { return mulMax; }
    float getMinMax() { return minMax; }

private:
    void on()  { fbo.reBuildFBO(1, renderEngine->getWidth(), renderEngine->getHeight(), theApp->getFBOInternalPrecision()); }
    void off() { fbo.deleteFBO(); }

    renderBaseClass *renderEngine;
    mmFBO fbo;
    bool bIsOn = false;

    const float spanMax = 8.f, mulMax = 64.f, minMax = 512.f;
    GLfloat span = 4.f, reduceMul = 8.f, reduceMin = 128.f, threshold = .5f;
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
        
        mBlurFBO.declareFBO(2, renderEngine->getWidth(), renderEngine->getHeight(), theApp->getFBOInternalPrecision());        
        create();
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
        mergedFBO.declareFBO(1,renderEngine->getWidth(),renderEngine->getHeight(), theApp->getFBOInternalPrecision()); 
        create();
    }

    GLuint render(GLuint texA, GLuint texB);
    void create();

    void Activate()   { 
        renderEngine->getRenderFBO().reBuildFBO(2,renderEngine->getWidth(),renderEngine->getHeight(), GL_RGBA32F);
        mergedFBO.reBuildFBO(1,renderEngine->getWidth(),renderEngine->getHeight(), theApp->getFBOInternalPrecision()); 
        renderEngine->setFlagUpdate();
    }
    void Deactivate() { 
        renderEngine->getRenderFBO().reBuildFBO(1,renderEngine->getWidth(),renderEngine->getHeight(), GL_RGBA32F);
        mergedFBO.deleteFBO(); 
        renderEngine->setFlagUpdate();
    }

#ifndef GLAPP_REQUIRE_OGL45
    void updateBillboardTex(GLuint tex)    { setUniform1i(LOCbillboardTex, tex); }
    void updatePointsTex(GLuint tex)       { setUniform1i(LOCpointsTex, tex); }
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
        glowRender = new radialBlurClass(this);

        colorMap = new ColorMapSettingsClass(this);
#if !defined(GLCHAOSP_NO_FXAA)
        fxaaFilter = new fxaaClass(this);
#endif
        dotTex.build(DOT_TEXT_SHFT, vec4(.7f, 0.f, .3f, 0.f), dotsTextureClass::dotsAlpha);
        selectColorMap(0);

        glowRender->create();
        colorMap->create();
    }

    ~particlesBaseClass ()  {  delete glowRender; delete colorMap;
#if !defined(GLCHAOSP_NO_FXAA)
    delete fxaaFilter; 
#endif
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
#ifdef GLCHAOSP_LIGHTVER
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

    void setDstBlend(GLuint v) {  dstBlendAttrib = v; }
    void setSrcBlend(GLuint v) {  srcBlendAttrib = v; }

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

protected:
    uniformBlocksClass planesUBlock;

    GLuint dstBlendAttrib, srcBlendAttrib;
    int dstIdxBlendAttrib, srcIdxBlendAttrib;

    ColorMapSettingsClass *colorMap;

    dotsTextureClass dotTex;

    radialBlurClass *glowRender;

#if !defined(GLCHAOSP_NO_FXAA)
    fxaaClass *fxaaFilter;
#endif

    
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

inline void ColorMapSettingsClass::setVelIntensity(float f) { velIntensity = f; particles->setFlagUpdate(); }
inline GLuint colorMapTexturedClass::getOrigTex() { return particles->getPaletteTexID(); }


inline void dataBlurClass::setFlagUpdate() { renderEngine->setFlagUpdate(); }
inline void dataBlurClass::clearFlagUpdate() { renderEngine->clearFlagUpdate(); }
inline void dataBlurClass::flagUpdate(bool b) { b ? renderEngine->setFlagUpdate() : renderEngine->clearFlagUpdate(); }

inline void renderBaseClass::uClippingPlanes::buildInvMV_forPlanes(particlesBaseClass *particles) {
    if(planeActive[0] || planeActive[1] || planeActive[2]) {
        particles->getTMat()->buid_invMV(); 
        atLeastOneActive = true;
    } else atLeastOneActive = false;
    clipPlane[0] = particles->getClippingPlane(0);
    clipPlane[1] = particles->getClippingPlane(1);
    clipPlane[2] = particles->getClippingPlane(2);
}


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

