//------------------------------------------------------------------------------
//  Copyright (c) 2018-2024 Michele Morrone
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

#include "emitter.h"

#if !defined(GLCHAOSP_NO_BB)
class particlesSystemClass : public shaderPointClass, public shaderBillboardClass
#else
class particlesSystemClass : public shaderPointClass
#endif
{
public:
    particlesSystemClass() { setRenderMode(RENDER_USE_POINTS); }
    ~particlesSystemClass() { deleteEmitter(); }


    /////////////////////////////////////////////
    particlesBaseClass *getParticleRenderPtr() {
#if !defined(GLCHAOSP_NO_BB)
        return getRenderMode() == RENDER_USE_BILLBOARD ?
               (particlesBaseClass *) shaderBillboardClass::getPtr() :
               (particlesBaseClass *) shaderPointClass::getPtr();
#else
        return (particlesBaseClass *) shaderPointClass::getPtr();
#endif
    }

    /////////////////////////////////////////////
    GLuint renderParticles(bool isFullScreenPiP = true, bool cpitView = false) {
#if !defined(GLCHAOSP_NO_AX)
        if(showAxes()) {
            getAxes()->getTransforms()->applyTransforms();
            getAxes()->renderOnFB(getRenderFBO().getFB(0));
            float zoomK = getTMat()->getOverallDistance();
            getAxes()->setZoomFactor(vec3(vec2(zoomK/10.f), zoomK/7.f) * getTMat()->getPerspAngle()/30.f);
        }
#endif
        return getParticleRenderPtr()->render(0, getEmitter(), isFullScreenPiP, cpitView);
    }


#if !defined(GLCHAOSP_NO_BB)
    void mixedRendering(GLuint auxTex) {
        getMergedRendering()->mixRender(auxTex); }
#endif
    void pipRendering(GLuint auxTex, const vec4& viewport, const vec2& transp_intens, bool border, const vec4& borderColor) {
        getPipWnd()->pipRender(auxTex, viewport, transp_intens, border, borderColor); }


    GLuint renderSingle();  // render single emitter engine
    GLuint renderTF();      // render TransformFeedback emitter engine

    /////////////////////////////////////////////
    GLuint render() { // main render renderSingle() or renderTF() ==> this->*renderEmitter()
        glViewport(0,0, getWidth(), getHeight());
        getFboContainer().unlockAll();
        return (this->*renderEmitter)();
    }

    void renderFilters() {
        particlesBaseClass *ptr = getParticleRenderPtr();
        bindFilterShader();
        if(ptr->getGlowData()->isGlowOn()) getGlowRender()->render(ptr->getGlowData());
        getImgTuningRender()->render(ptr->getImgTuningData());
        if(ptr->getFXAAData()->isOn()) getFXAARender()->render(ptr->getFXAAData());
    }

    void blitOnDrawBuffer() { getBlitRender()->renderOnDB(); }
    GLuint blitOnFrameBuffer() { return getBlitRender()->renderOnFB(); }

    void bindFilterShader() {
        getFilter()->bindingShader();

    }
    /////////////////////////////////////////////
    void renderPalette() {      // ColorMaps: rebuilds texture only if settings are changed
        shaderPointClass::getCMSettings()->render();

#if !defined(GLCHAOSP_NO_BB)
        shaderBillboardClass::getCMSettings()->render();
#endif
    }

    void renderAxes();

    emitterBaseClass *getEmitter() { return emitter; }

    //emitterBaseClass *getTransformInterlieve() { return emitter; }
    bool clearScreen() { return canClearScreen; }
    void clearScreen(bool b) {  canClearScreen=b; }
    
    void changeEmitter(enumEmitterEngine ee);

private:
    void onReshape(int w, int h);

    void buildEmitter(enumEmitterEngine ee);
    void deleteEmitter();
    
    //particlesBaseClass *currentRenderEngine;

    typedef GLuint (particlesSystemClass::*renderPtrFn)();
    renderPtrFn renderEmitter;

    friend class glWindow;
    emitterBaseClass* emitter;
    bool canClearScreen = true;
};
