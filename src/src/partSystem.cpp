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
#include "partSystem.h"


void particlesSystemClass::buildEmitter(enumEmitterEngine ee)
{

    if(ee != emitterEngine_noEmitterSelected) {
#if !defined(GLCHAOSP_NO_TF)
        if(ee == enumEmitterEngine::emitterEngine_staticParticles) {
            emitter = (emitterBaseClass*) new singleEmitterClass;
            renderEmitter = &particlesSystemClass::renderSingle;
        } else {  // enumEmitterEngine::emitterEngine_transformFeedback
            emitter = (emitterBaseClass*) new transformedEmitterClass;
            renderEmitter = &particlesSystemClass::renderTF;
        }
#else
        emitter = (emitterBaseClass*) new singleEmitterClass;
        renderEmitter = &particlesSystemClass::renderSingle;
#endif
        emitter->buildEmitter(); // post build for WebGL texture ID outRange
    }

//start new thread (if aux thread enabled)
#if !defined(GLCHAOSP_NO_TH)
//lock aux thread until initialization is complete
    std::lock_guard<std::mutex> l( attractorsList.getStepMutex() );
#endif
    attractorsList.newStepThread(emitter);
}


void particlesSystemClass::deleteEmitter()
{
    attractorsList.deleteStepThread();
    delete emitter;
}


void particlesSystemClass::changeEmitter(enumEmitterEngine ee)
{

    GLuint circBuffer = emitter->getSizeCircularBuffer();
    bool fStop = emitter->stopFull();
    bool restart = emitter->restartCircBuff();
    deleteEmitter();

#ifdef GLCHAOSP_NO_TF
    ee = enumEmitterEngine::emitterEngine_staticParticles;
#endif
    theApp->setEmitterEngineType(ee);
    buildEmitter(ee);

    emitter->setSizeCircularBuffer(circBuffer);
    emitter->stopFull(fStop);
    emitter->restartCircBuff(restart);

}


void particlesSystemClass::onReshape(int w, int h)
{
    if(w==0 || h==0 || (w == theApp->GetWidth() && h == theApp->GetHeight())) return; //Is in iconic (GLFW do not intercept on Window)

    theApp->SetWidth(w); theApp->SetHeight(h);

    getTMat()->getTrackball().viewportSize(w, h);

    getTMat()->setPerspective(float(w)/float(h));

    getRenderFBO().reSizeFBO(w, h);
    getAuxFBO().reSizeFBO(w, h);

#if !defined(GLCHAOSP_NO_AO_SHDW)
    if(theApp->canUseShadow()) getShadow()->resize(w, h);
#endif
#if !defined(GLCHAOSP_NO_MB)
     getMotionBlur()->getFBO().reSizeFBO(w, h);
#endif
    setFlagUpdate();
}


void particlesSystemClass::renderAxes()
{

#if !defined(GLCHAOSP_NO_AX)

    transformsClass *model = getTMat();
    transformsClass *axes = getAxes()->getTransforms();

    auto syncAxes = [&] () {
        axes->setView(model->getPOV(), getTMat()->getTGT());
        axes->setPerspective(model->getPerspAngle(), float(theApp->GetWidth())/float(theApp->GetHeight()), model->getPerspNear(), model->getPerspFar() );
        axes->getTrackball().setRotation(model->getTrackball().getRotation());
    };

    if(showAxes() == renderBaseClass::showAxesToSetCoR) {
    //  Set center of rotation
    //////////////////////////////////////////////////////////////////
        syncAxes();

        // no dolly & pan: axes are to center
        axes->getTrackball().setPanPosition(vec3(0.0));
        axes->getTrackball().setDollyPosition(vec3(0.0));

        // get rotation & translation of model w/o pan & dolly
        quat q = model->getTrackball().getRotation() ;
        model->tM.mMatrix = mat4_cast(q) * translate(mat4(1.f), model->getTrackball().getRotationCenter());
        model->build_MV_MVP();

        // apply rotation to matrix... then subtract prevous model translation
        axes->tM.mMatrix = mat4(1.f);
        axes->getTrackball().applyRotation(axes->tM.mMatrix);
        axes->tM.mMatrix = translate(axes->tM.mMatrix , -model->getTrackball().getRotationCenter());
        axes->build_MV_MVP();


    } else  {
    //  Show center of rotation
    //////////////////////////////////////////////////////////////////
        if(showAxes() == renderBaseClass::showAxesToViewCoR) {
            syncAxes();

            // add RotCent component to dolly & pan... to translate axes with transforms
            vec3 v(model->getTrackball().getRotationCenter());
            axes->getTrackball().setPanPosition(model->getTrackball().getPanPosition()-vec3(v.x, v.y, 0.0));
            axes->getTrackball().setDollyPosition(model->getTrackball().getDollyPosition()-vec3(0.0, 0.0, v.z));

            axes->applyTransforms();
        }

        model->applyTransforms();
    }
#endif
}


GLuint particlesSystemClass::renderSingle()
{
    GLuint texRendered = 0;

    emitter->preRenderEvents();

#if !defined(GLCHAOSP_NO_BB)
    if(getRenderMode() != RENDER_USE_BOTH) {
        renderParticles();
        renderFilters();    //render gausBlur-glow, imgTuning, FXAA
    } else {
        shaderBillboardClass::render(0, getEmitter());
        if(shaderBillboardClass::getGlowData()->isGlowOn()) getGlowRender()->render(shaderBillboardClass::getGlowData());
        getImgTuningRender()->render(shaderBillboardClass::getImgTuningData());
        if(shaderBillboardClass::getFXAAData()->isOn())     getFXAARender()->render(shaderBillboardClass::getFXAAData());

        const GLuint tex1 = blitOnFrameBuffer();
        
        getFboContainer().unlockAll();

        shaderPointClass::render(0, getEmitter());
        if(shaderPointClass::getGlowData()->isGlowOn()) getGlowRender()->render(shaderPointClass::getGlowData());
        getImgTuningRender()->render(shaderPointClass::getImgTuningData());
        if(shaderPointClass::getFXAAData()->isOn())     getFXAARender()->render(shaderPointClass::getFXAAData());

        mixedRendering(tex1);

        //emitter->bufferRendered();

    }
#else
    glBindTexture(GL_TEXTURE_2D,  0);
    renderParticles();

    bindFilterShader();

    //glBindTexture(GL_TEXTURE_2D,  0);
    renderFilters();
#endif

    emitter->postRenderEvents();
    return texRendered;
}

void blitFrameBuffer(const GLuint srcFB, const GLuint dstFB, const ivec4 &srcRect, const ivec4 &dstRect, const GLuint filter=GL_LINEAR);


GLuint particlesSystemClass::renderTF()
{
    particlesBaseClass *particles = getParticleRenderPtr();
    tfSettinsClass &cPit = particles->getTFSettings();

    const int w = getWidth(), h = getHeight();

    const bool isDualView = cPit.cockPit() && cPit.getPIPposition() != cPit.pip::noPIP;

    getEmitter()->preRenderEvents();

    const vec3 vecA(vec3(attractorsList.get()->getCurrent() ));

    const int buffSize = attractorsList.get()->getQueueSize()-1;
    const int idx = cPit.getTailPosition()*buffSize+.5;

    const vec3 vecB(vec3(attractorsList.get()->getAt(idx<1 || cPit.fixedDistance() ? 1 : (idx>buffSize ? buffSize : idx))));

    vec3 cpPOV((cPit.invertView() ? vecA : vecB));
    vec3 cpTGT((cPit.invertView() ? vecB : vecA));

    transformsClass *cpTM = getCockPitTMat();

    cpTM->setPerspective(tfSettinsClass::getPerspAngle(),  w / float(h),
                         tfSettinsClass::getPerspNear(),
                         getTMat()->getPerspFar());


    const vec3 dirVec = cPit.fixedDistance() ? normalize(cpPOV-cpTGT) : cpPOV-cpTGT;
    const vec3 vecDirT = dirVec * cPit.getMovePositionTail();

    mat4 m = translate(mat4(1.f), cpTGT);
    m = m * mat4_cast(cPit.getRotation());
    cpPOV = mat4(m) * vec4(vecDirT, 1.0);

    cpTM->setView(cpPOV, cpTGT);


    // New settings for tfSettings
    cpTM->getTrackball().setRotation(quat(1.0f,0.0f, 0.0f, 0.0f));
    cpTM->getTrackball().setRotationCenter(vec3(0.f));

    cpTM->getTrackball().setPosition(vec3(0.f));
    cpTM->applyTransforms();

    setFlagUpdate();
    
    //Render TF full screen view
    renderParticles(true, cPit.cockPit() && !cPit.invertPIP());
    renderFilters();    //render gausBlur-glow, imgTuning, FXAA

    GLuint tex1 = 0;

    //Render PiP view
    if(isDualView) {
        cPit.setViewport(w,h);
        vec4 normViewport(cPit.getViewportLimits()/vec4(w,h,w,h)); //normalized viewport Limits

        tex1 = blitOnFrameBuffer();
        getFboContainer().unlockAll();

        renderParticles(false, cPit.invertPIP());
        renderFilters();    //render gausBlur-glow, imgTuning, FXAA
        pipRendering(tex1, normViewport, vec2(cPit.getPipTransparence(), cPit.getPipIntensity()), cPit.borderActive(), cPit.getPipBorderColor());

    }
    getEmitter()->postRenderEvents();

    return tex1;
}