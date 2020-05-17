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

#if !defined(__EMSCRIPTEN__)

    //pointsprite initialization
    glEnable( GL_PROGRAM_POINT_SIZE );
    //glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_UPPER_LEFT);
    GLfloat retVal[4];
    glGetFloatv(GL_POINT_SIZE_RANGE, &retVal[0]);
    particlesSystem->shaderPointClass::getUData().pointspriteMinSize = retVal[0];
#endif


    vg::vGizmo3D &vgizmo = theWnd->getParticlesSystem()->getTMat()->getTrackball();

    particlesSystem->getTMat()->setPerspective(30.f, float(theApp->GetWidth())/float(theApp->GetHeight()), 0.f, 100.f);
    particlesSystem->getTMat()->setView(attractorsList.get()->getPOV(), attractorsList.get()->getTGT());

    vgizmo.setGizmoRotControl( (vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) 0 /* evNoModifier */ );

    vgizmo.setGizmoRotXControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_SHIFT);
    vgizmo.setGizmoRotYControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_CONTROL);
    vgizmo.setGizmoRotZControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_ALT | GLFW_MOD_SUPER);

    vgizmo.setDollyControl((vgButtons) GLFW_MOUSE_BUTTON_RIGHT, (vgModifiers) 0);
    vgizmo.setPanControl(  (vgButtons) GLFW_MOUSE_BUTTON_RIGHT, (vgModifiers) GLFW_MOD_CONTROL|GLFW_MOD_SUPER);


    vgizmo.setRotationCenter(attractorsList.get()->getTGT());
    vgizmo.viewportSize(theApp->GetWidth(), theApp->GetHeight());

    vgizmo.setIdleRotSpeed(.3f);

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

////////////////////////////////////////////////////////////////////////////
void glWindow::onExit()
{
    delete particlesSystem;
    delete vao;
}

////////////////////////////////////////////////////////////////////////////
GLint glWindow::onRender()
{
    particlesSystem->renderPalette();

#if !defined(GLCHAOSP_LIGHTVER)

    particlesSystem->renderAxes();

    // main render event
    GLuint texRendered = particlesSystem->render();

    //  Motion Blur
    if(particlesSystem->getMotionBlur()->Active()) {

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
    getParticlesSystem()->getTMat()->applyTransforms();
    GLuint texRendered = particlesSystem->render();
#endif
    //glBindFramebuffer(GL_FRAMEBUFFER, 0); // bind both FRAMEBUFFERS to default

    particlesSystem->clearFlagUpdate();
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
    const float z = particlesSystem->getTMat()->getOverallDistance()*particlesSystem->getTMat()->getTrackball().getDistScale();
    particlesSystem->getTMat()->getTrackball().motion(x, y, z);
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
    const float z = particlesSystem->getTMat()->getOverallDistance()*particlesSystem->getTMat()->getTrackball().getDistScale();
    particlesSystem->getTMat()->getTrackball().wheel(x, y, z);
}

