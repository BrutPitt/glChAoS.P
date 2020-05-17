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

#include "emitter.h"

#if !defined(GLCHAOSP_LIGHTVER)
class particlesSystemClass : public shaderPointClass, public shaderBillboardClass
#else
class particlesSystemClass : public shaderPointClass
#endif
{
public:
    particlesSystemClass() {
        setRenderMode(RENDER_USE_POINTS);
        buildEmitter((enumEmitterEngine) theApp->getEmitterEngineType());
    }
    ~particlesSystemClass() { deleteEmitter(); }


    /////////////////////////////////////////////
    particlesBaseClass *getParticleRenderPtr() {
#if !defined(GLCHAOSP_LIGHTVER)
        return getRenderMode() == RENDER_USE_BILLBOARD ?
               (particlesBaseClass *) shaderBillboardClass::getPtr() :
               (particlesBaseClass *) shaderPointClass::getPtr();
#else
        return (particlesBaseClass *) shaderPointClass::getPtr();
#endif
    }

    /////////////////////////////////////////////
    GLuint renderParticles(bool isFullScreenPiP = true, bool cpitView = false) {
#if !defined(GLCHAOSP_LIGHTVER)
        if(showAxes()) {
            getAxes()->getTransforms()->applyTransforms();
            getAxes()->renderOnFB(getRenderFBO().getFB(0));
            float zoomK = getTMat()->getOverallDistance();
            getAxes()->setZoomFactor(vec3(vec2(zoomK/10.f), zoomK/7.f) * getTMat()->getPerspAngle()/30.f);
        }
#endif
        return getParticleRenderPtr()->render(0, getEmitter(), isFullScreenPiP, cpitView);
    }

    /////////////////////////////////////////////
    GLuint renderGlowEffect(GLuint texRendered) {
#if !defined(GLCHAOSP_LIGHTVER)
        particlesBaseClass *particles = getParticleRenderPtr();

        const GLuint fbo = (getMotionBlur()->Active() || particles->getFXAA()->isOn()) ? particles->getGlowRender()->getFBO().getFB(1) : 0;
        particles->getGlowRender()->render(texRendered, fbo);
        return particles->getGlowRender()->getFBO().getTex(1);  // used only if FXAA and/or Motionblur
#else
        shaderPointClass::getPtr()->getGlowRender()->render(texRendered, 0);
        return 0;
#endif
    }

#if !defined(GLCHAOSP_NO_FXAA)
    GLuint renderFXAA(GLuint texRendered, bool useFB=false) {
        particlesBaseClass *particles = getParticleRenderPtr();

        return particles->getFXAA()->isOn() ? particles->getFXAA()->render(texRendered, useFB) : texRendered;
    }
#endif


    GLuint renderSingle();  // render single emitter engine
    GLuint renderTF();      // render TransformFeedback emitter engine

    /////////////////////////////////////////////
    GLuint render() {
        return attractorsList.get()->dtType() && tfSettinsClass::tfMode() && tfSettinsClass::cockPit() ?
                renderTF() :
                renderSingle();
    }
    /////////////////////////////////////////////
    void renderPalette() {      // ColorMaps: rebuilds texture only if settings are changed
        shaderPointClass::getCMSettings()->render();

#if !defined(GLCHAOSP_LIGHTVER)
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

    friend class glWindow;
    emitterBaseClass* emitter;
    bool canClearScreen = true;
};
