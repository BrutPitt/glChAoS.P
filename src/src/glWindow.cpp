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
#include <vgMath.h>

#include <fastRandom.h>

#include "glWindow.h"
#include "ParticlesUtils.h"

//Random numbers of particle velocity of fragmentation
RandomTexture rndTexture;
HLSTexture hlsTexture;

// 
////////////////////////////////////////////////////////////////////////////
void glWindow::onInit()
{

    glViewport(0,0,theApp->GetWidth(), theApp->GetHeight());
    vao = new vaoClass;
    
    //rndTexture.buildTex(1024);
    //hlsTexture.buildTex(1024);

    particlesSystem = new particlesSystemClass;

/*
    if(theApp->getEmitterEngineType()==enumEmitterEngine::emitterEngine_transformFeedback) {
        particlesSystem->shaderPointClass::getGlowRender()->setGlowOn(false);
        particlesSystem->shaderPointClass::getFXAA()->activate(false);
#if !defined(__EMSCRIPTEN__)
        particlesSystem->shaderBillboardClass::getGlowRender()->setGlowOn(false);
        particlesSystem->shaderPointClass::getFXAA()->activate(false);
#endif
    }
*/
#if !defined(__EMSCRIPTEN__)

    //pointsprite initialization
    glEnable( GL_PROGRAM_POINT_SIZE );
    //glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_UPPER_LEFT);
    GLfloat retVal[4];
    glGetFloatv(GL_POINT_SIZE_RANGE, &retVal[0]);
    particlesSystem->shaderPointClass::getUData().pointspriteMinSize = retVal[0];
#endif


    vg::vGizmo3D &T = theWnd->getParticlesSystem()->getTMat()->getTrackball();

    particlesSystem->getTMat()->setPerspective(30.f, float(theApp->GetWidth())/float(theApp->GetHeight()), 0.f, 100.f);
    particlesSystem->getTMat()->setView(attractorsList.get()->getPOV(), attractorsList.get()->getTGT());

    T.setGizmoRotControl( (vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) 0 /* evNoModifier */ );

    T.setGizmoRotXControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_SHIFT);
    T.setGizmoRotYControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_CONTROL);
    T.setGizmoRotZControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_ALT | GLFW_MOD_SUPER);

    T.setDollyControl((vgButtons) GLFW_MOUSE_BUTTON_RIGHT, (vgModifiers) 0);
    T.setPanControl(  (vgButtons) GLFW_MOUSE_BUTTON_RIGHT, (vgModifiers) GLFW_MOD_CONTROL|GLFW_MOD_SUPER);


    T.setRotationCenter(attractorsList.get()->getTGT());

    T.viewportSize(theApp->GetWidth(), theApp->GetHeight());

    //load attractor file (if exist) and (if exist) override default parameters
    //attractorsList.setSelection(attractorsList.getSelection()); 

    int listSize = attractorsList.getList().size()-1;

    auto getRandomIDX = [&]() {
        fstRnd::fFastRand32 fastRandom;
        return int(fastRandom.UNI() * float(listSize));
    };

    if(theApp->getStartWithAttractorName()=="random") {
        attractorsList.setSelection(getRandomIDX()); 
    } else if(theApp->getStartWithAttractorName().find(".sca")==std::string::npos) {
        int index = attractorsList.getSelectionByName(theApp->getStartWithAttractorName());
        attractorsList.setSelection((index<0 || index>listSize) ? getRandomIDX() : index);       
    } else {
        //attractorsList.getThreadStep()->stopThread();
        if(theApp->loadAttractor(theApp->getStartWithAttractorName().c_str())) {
            attractorsList.setFileName(theApp->getStartWithAttractorName());
            theApp->setLastFile(theApp->getStartWithAttractorName().c_str());
            attractorsList.checkCorrectEmitter();
            attractorsList.getThreadStep()->restartEmitter();
            attractorsList.get()->initStep();
            attractorsList.getThreadStep()->startThread();
        }
        else attractorsList.setSelection(getRandomIDX());
    }
}

// 
////////////////////////////////////////////////////////////////////////////
void glWindow::onExit()
{
    delete particlesSystem;
    delete vao;
}

// 
////////////////////////////////////////////////////////////////////////////
GLint glWindow::onRender()
{
    transformsClass *model = getParticlesSystem()->getTMat();

    //  render ColorMaps: rebuild texture only if settings are changed
    //////////////////////////////////////////////////////////////////
    particlesSystem->shaderPointClass::getCMSettings()->render();

#if !defined(GLCHAOSP_LIGHTVER)

    particlesSystem->shaderBillboardClass::getCMSettings()->render();

    transformsClass *axes = getParticlesSystem()->getAxes()->getTransforms();

    auto syncAxes = [&] () {
        axes->setView(model->getPOV(), getParticlesSystem()->getTMat()->getTGT());    
        axes->setPerspective(model->getPerspAngle(), float(theApp->GetWidth())/float(theApp->GetHeight()), model->getPerspNear(), model->getPerspFar() );
        axes->getTrackball().setRotation(model->getTrackball().getRotation());
    };

    if(particlesSystem->showAxes() == renderBaseClass::showAxesToSetCoR) {
    //  Set center of rotation
    //////////////////////////////////////////////////////////////////
        syncAxes();

        // no dolly & pan: axes are to center
        axes->getTrackball().setPanPosition(vec3(0.0));
        axes->getTrackball().setDollyPosition(vec3(0.0));

        // get rotation & translation of model w/o pan & dolly
        quat q =   model->getTrackball().getRotation() ;
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
        if(particlesSystem->showAxes() == renderBaseClass::showAxesToViewCoR) {
            syncAxes();

            // add RotCent component to dolly & pan... to translate axes with transforms
            vec3 v(model->getTrackball().getRotationCenter());
            axes->getTrackball().setPanPosition(model->getTrackball().getPanPosition()-vec3(v.x, v.y, 0.0));
            axes->getTrackball().setDollyPosition(model->getTrackball().getDollyPosition()-vec3(0.0, 0.0, v.z));

            axes->applyTransforms();
        }

        model->applyTransforms();
    }

    GLuint texRendered = renderAttractor();

    //  Motion Blur
    //////////////////////////////////////////////////////////////////
    if(particlesSystem->getMotionBlur()->Active()) {

    //glDisable(GL_BLEND);
    #ifdef GLAPP_REQUIRE_OGL45
        glBlitNamedFramebuffer(particlesSystem->getMotionBlur()->render(texRendered),
                               0,
                               0,0,particlesSystem->getWidth(), particlesSystem->getHeight(),
                               0,0,theApp->GetWidth(), theApp->GetHeight(),
                               GL_COLOR_BUFFER_BIT, GL_NEAREST );
    #else
        glBindFramebuffer(GL_READ_FRAMEBUFFER, particlesSystem->getMotionBlur()->render(texRendered));
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

        glBlitFramebuffer(0,0,particlesSystem->getWidth(), particlesSystem->getHeight(),
                          0,0,theApp->GetWidth(), theApp->GetHeight(),
                          GL_COLOR_BUFFER_BIT, GL_NEAREST );
    #endif
    }

#else 

    model->applyTransforms();
    GLuint texRendered = renderAttractor();

#endif
    //glBindFramebuffer(GL_FRAMEBUFFER, 0); // bind both FRAMEBUFFERS to default

    particlesSystem->clearFlagUpdate();
    return texRendered;
}


//  render Attractor
//////////////////////////////////////////////////////////////////
GLuint glWindow::renderAttractor()
{

    auto saveTSettings = [&]() {


    };

    const int w =particlesSystem->getWidth(), h =particlesSystem->getHeight();

    GLuint texRendered;


    if(attractorsList.get()->dtType() && attractorsList.slowMotion() && attractorsList.getCockpit().cockPit()) {

        cockpitClass &cPit = attractorsList.getCockpit();

        particlesSystem->getEmitter()->preRenderEvents();

        auto renderProcedure = [&]() {
            particlesSystem->setFlagUpdate();
            texRendered = particlesSystem->renderParticles();
            texRendered = particlesSystem->renderGlowEffect(texRendered);
        };

        //Render standard View in FullScreen (CockPitView is in PiP)
        if(cPit.invertPIP() && cPit.getPIPposition() != cPit.pip::noPIP) {
            glViewport(0,0, w, h);
            renderProcedure();
        }

        const vec3 head(attractorsList.get()->getCurrent());
        const vec3 vecA(vec3(attractorsList.get()->getCurrent() ));

        const int buffSize = attractorsList.get()->getQueueSize()-1;
        int idx = cPit.getTailPosition()*buffSize+.5; 

        const vec3 vecB(vec3(attractorsList.get()->getAt(idx<1 ? 1 : (idx>buffSize ? buffSize : idx))));

        vec3 cpPOV((cPit.invertView() ? vecA : vecB));
        vec3 cpTGT((cPit.invertView() ? vecB : vecA));

        const vec3 vecDirH = (cpPOV-cpTGT)*cPit.getMovePositionHead();
        const vec3 vecDirT = (cpPOV-cpTGT)*cPit.getMovePositionTail();

        cpPOV+=vecDirT;
        cpTGT+=vecDirH;

        // Saving transform status
        transformsClass *objT = getParticlesSystem()->getTMat();

        const float perspAngle = objT->getPerspAngle();
        vec3 vPOV(objT->getPOV()), vTGT(objT->getTGT());
        vec3 vPos(objT->getTrackball().getPosition()), vCoR(objT->getTrackball().getRotationCenter());
        quat qRot = objT->getTrackball().getRotation();
        float pNear = objT->getPerspNear();

        mat4 m = translate(mat4(1.f), cpTGT);
        m = m * mat4_cast(cPit.getRotation());
        cpPOV = mat4(m) * vec4(cpPOV-cpTGT, 1.0);

        particlesSystem->getTMat()->setPerspective(attractorsList.getCockpit().getPerspAngle(), float(w)/float(h), attractorsList.getCockpit().getPerspNear(), objT->getPerspFar());
        particlesSystem->getTMat()->setView(cpPOV, cpTGT);

        // New settings for cockpit
        particlesSystem->getTMat()->getTrackball().setRotation(quat(1.0f,0.0f, 0.0f, 0.0f));
        particlesSystem->getTMat()->getTrackball().setRotationCenter(vec3(0.f));

        particlesSystem->getTMat()->getTrackball().setPosition(vec3(0.f));
        particlesSystem->getTMat()->applyTransforms();

        shaderPointClass *ps = getParticlesSystem()->shaderPointClass::getPtr();
        const float ptSizeP = ps->getSize();
        //ps->setSize(cPit.invertPIP() && cPit.getPIPposition()!=cockpitClass::pip::noPIP ? cPit.getPointSize()*.5*cPit.getPIPzoom() : cPit.getPointSize());
        ps->setSize(cPit.getPointSize());

#if !defined(GLCHAOSP_LIGHTVER)
        shaderBillboardClass *bb = getParticlesSystem()->shaderBillboardClass::getPtr();
        const float ptSizeB = bb->getSize();
        bb->setSize(ps->getSize());
#endif

        //Render CockPit view 
        if(cPit.invertPIP()) { 
            cPit.setViewport(w,h); 
            texRendered = particlesSystem->renderParticles(false, particlesSystem->shaderPointClass::getPtr()->getGlowRender()->getFBO().getFB(1)); 
            texRendered = particlesSystem->renderGlowEffect(texRendered);
        }  
        else  { glViewport(0,0, w, h);  renderProcedure(); }

        

        // Restore transform status
        particlesSystem->getTMat()->setPerspective(perspAngle, float(w)/float(h), pNear, objT->getPerspFar());
        particlesSystem->getTMat()->setView(vPOV, vTGT);

        particlesSystem->getTMat()->getTrackball().setRotation(qRot);
        particlesSystem->getTMat()->getTrackball().setPosition(vPos);
        particlesSystem->getTMat()->getTrackball().setRotationCenter(vCoR);
        particlesSystem->getTMat()->applyTransforms();

        //If StandardView in PiP
        if(!cPit.invertPIP() && cPit.getPIPposition() != cPit.pip::noPIP) {
            ps->setSize(ptSizeP); //*.5*cPit.getPIPzoom()
#if !defined(GLCHAOSP_LIGHTVER)
            bb->setSize(ptSizeB); //
#endif                                                           
            cPit.setViewport(w,h);
            texRendered = particlesSystem->renderParticles(false, particlesSystem->shaderPointClass::getPtr()->getGlowRender()->getFBO().getFB(1)); 
            texRendered = particlesSystem->renderGlowEffect(texRendered);
        }

        glViewport(0,0, w, h);
#if !defined(GLCHAOSP_NO_FXAA)
            texRendered = particlesSystem->renderFXAA(texRendered);
#endif                                                           

        ps->setSize(ptSizeP);
#if !defined(GLCHAOSP_LIGHTVER)
        bb->setSize(ptSizeB);
#endif
        particlesSystem->getEmitter()->postRenderEvents();
    } else {
        glViewport(0,0, w, h);
        texRendered = particlesSystem->render();
    }

    return texRendered;
}


////////////////////////////////////////////////////////////////////////////
void glWindow::onIdle()
{
    if(theApp->idleRotation()) particlesSystem->getTMat()->getTrackball().idle();
}


////////////////////////////////////////////////////////////////////////////
void glWindow::onReshape(GLint w, GLint h)
{
    glViewport(0,0,w,h);

    theApp->SetWidth(w); theApp->SetHeight(h);
    particlesSystem->getTMat()->getTrackball().viewportSize(w, h);

    if(particlesSystem) particlesSystem->onReshape(w,h);

}


////////////////////////////////////////////////////////////////////////////
void glWindow::onMouseButton(int button, int upOrDown, int x, int y)
{
    particlesSystem->getTMat()->getTrackball().mouse((vgButtons) (button),
                                                     (vgModifiers) theApp->getModifier(),
                                                      upOrDown==APP_MOUSE_BUTTON_DOWN, x, y );
}

////////////////////////////////////////////////////////////////////////////
void glWindow::onMotion(int x, int y)
{
    particlesSystem->getTMat()->getTrackball().motion(x, y);
}

////////////////////////////////////////////////////////////////////////////
void glWindow::onPassiveMotion(int x, int y) {}

////////////////////////////////////////////////////////////////////////////
void glWindow::onKeyUp(unsigned char key, int x, int y) {}

////////////////////////////////////////////////////////////////////////////
void glWindow::onSpecialKeyDown(int key, int x, int y) {}

////////////////////////////////////////////////////////////////////////////
void glWindow::onKeyDown(unsigned char key, int x, int y) {}

////////////////////////////////////////////////////////////////////////////
void glWindow::onSpecialKeyUp(int key, int x, int y) {}

////////////////////////////////////////////////////////////////////////////
void glWindow::onMouseWheel(int wheel, int direction, int x, int y) 
{
    particlesSystem->getTMat()->getTrackball().wheel(x, y);
}

