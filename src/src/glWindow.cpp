//------------------------------------------------------------------------------
//  Copyright (c) 2018-2019 Michele Morrone
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
#include <vGizmoMath.h>

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

    particlesSystem = new particlesSystemClass(new singleEmitterClass);
#if !defined(__EMSCRIPTEN__)
    //new particlesSystemClass(new transformedEmitterClass(1,1/*mVB_COLOR*/)) :

    //pointsprite initialization
    glEnable( GL_PROGRAM_POINT_SIZE );
    //glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_UPPER_LEFT);
    GLfloat retVal[4];
    glGetFloatv(GL_POINT_SIZE_RANGE, &retVal[0]);
    particlesSystem->shaderPointClass::getUData().pointspriteMinSize = retVal[0];

    #if !defined(GLCHAOSP_LIGHTVER)
        //lock aux thread until initialization is complete
        std::lock_guard<std::mutex> l( attractorsList.getStepMutex() );
    #endif
#endif

    //start new thread (if aux thread enabled)
    attractorsList.newStepThread(particlesSystem->getEmitter());    

    vg::vGizmo3D &T = theWnd->getParticlesSystem()->getTMat()->getTrackball();

    particlesSystem->getTMat()->setPerspective(30.f, float(theApp->GetWidth())/float(theApp->GetHeight()), 0.f, 100.f);
    particlesSystem->getTMat()->setView(attractorsList.get()->getPOV(), attractorsList.get()->getTGT());

    T.setGizmoRotControl( (vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) 0 /* evNoModifier */ );

    T.setGizmoRotXControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_SHIFT);
    T.setGizmoRotYControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_CONTROL);
    T.setGizmoRotZControl((vgButtons) GLFW_MOUSE_BUTTON_LEFT, (vgModifiers) GLFW_MOD_ALT | GLFW_MOD_SUPER);

    T.setDollyControl((vgButtons) GLFW_MOUSE_BUTTON_RIGHT, (vgModifiers) 0);
    T.setPanControl(  (vgButtons) GLFW_MOUSE_BUTTON_RIGHT, (vgModifiers) GLFW_MOD_CONTROL|GLFW_MOD_SHIFT);


    T.setRotationCenter(attractorsList.get()->getTGT());

    T.viewportSize(theApp->GetWidth(), theApp->GetHeight());

    mmFBO::Init(theApp->GetWidth(), theApp->GetHeight()); 

    //load attractor file (if exist) and (if exist) override default parameters
    //attractorsList.setSelection(attractorsList.getSelection()); 

    int listSize = attractorsList.getList().size()-1;

    if(theApp->getStartWithAttractorIdx()<0 || theApp->getStartWithAttractorIdx()>listSize) {
        fstRnd::fFastRand32 fastRandom; 
        
        int index = int(fastRandom.UNI() * float(listSize));
        attractorsList.setSelection(index); 
        //attractorsList.setSelection(30); 
    }
    else attractorsList.setSelection(theApp->getStartWithAttractorIdx()); 
    
}

// 
////////////////////////////////////////////////////////////////////////////
void glWindow::onExit()
{
    attractorsList.deleteStepThread();

    delete particlesSystem;
    delete vao;
}

// 
////////////////////////////////////////////////////////////////////////////
void glWindow::onRender()
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
}

//  render Attractor
//////////////////////////////////////////////////////////////////
GLuint glWindow::renderAttractor()
{

    auto saveTSettings = [&]() {


    };

    const int w =particlesSystem->getWidth(), h =particlesSystem->getHeight();

    GLuint texRendered;


    if(attractorsList.get()->dtType() && particlesSystem->slowMotion() && particlesSystem->cockPit()) {

        cockpitClass &cPit = particlesSystem->getCockpit();

        auto renderProcedure = [&]() {
            particlesSystem->setFlagUpdate();
            texRendered = particlesSystem->render();
        };

        if(cPit.invertPIP() && cPit.getPIPposition() != cPit.pip::noPIP) {
            glViewport(0,0, w, h);
            renderProcedure();
        }

        const int sizeBuffer = 100;
        const GLsizei startPoint = GLsizei(particlesSystem->getEmitter()->getStartPointSlowMotion()+
                                           particlesSystem->getSlowMotionMaxDots()-sizeBuffer);
        vec4 buffer[sizeBuffer];
        vec4 *mappedBuffer = nullptr;
        if(particlesSystem->getEmitter()->useMappedMem())   // USE_MAPPED_BUFFER
            mappedBuffer = (vec4 *) particlesSystem->getEmitter()->getVBO()->getBuffer() + startPoint;
        else {
            mappedBuffer = buffer;
#ifdef GLAPP_REQUIRE_OGL45
            glGetNamedBufferSubData(particlesSystem->getEmitter()->getVBO()->getVBO(), 
                                    startPoint * particlesSystem->getEmitter()->getVBO()->getBytesPerVertex(),
                                    sizeBuffer * particlesSystem->getEmitter()->getVBO()->getBytesPerVertex(),
                                    (void *)mappedBuffer);
#else
            glBindBuffer(GL_ARRAY_BUFFER, particlesSystem->getEmitter()->getVBO()->getVBO());
    #ifdef __EMSCRIPTEN__
            int start = startPoint * particlesSystem->getEmitter()->getVBO()->getBytesPerVertex();
            int size = sizeBuffer * particlesSystem->getEmitter()->getVBO()->getBytesPerVertex();
            void *data = (void *)mappedBuffer;
            EM_ASM_(
            {
              Module.ctx.getBufferSubData(Module.ctx.ARRAY_BUFFER, $0, HEAPU8.subarray($2, $2 + $1));
            }, size, start, data);
    #else
            glGetBufferSubData(GL_ARRAY_BUFFER, 
                                startPoint * particlesSystem->getEmitter()->getVBO()->getBytesPerVertex(),                                    
                                sizeBuffer * particlesSystem->getEmitter()->getVBO()->getBytesPerVertex(),
                                (void *)mappedBuffer);
    #endif

#endif
        }
/*
        vec4 *p1 = mappedBuffer+(sizeBuffer-1);
        vec4 *p2 = mappedBuffer+(sizeBuffer-50);
        vec3 vP1(*p1--), vP2(*p2--);
        for(int i = 0; i<49; i++) {
            vP1+=*p1--;
            vP2+=*p2--;
        }
        vP1*=.02f; vP2*=.02f;

        const vec3 vecA(vP1);
        const vec3 vecB(vP2);
        vec3 cpPOV = vecB+(vecA-vecB);
        vec3 cpTGT = vecA+(vecA-vecB);
*/
        //vec3 head = mappedBuffer[sizeBuffer-1];
        const vec3 vecA(mappedBuffer[sizeBuffer-1]);
        const vec3 vecB(mappedBuffer[sizeBuffer-50]);
        vec3 cpPOV = vecB+(vecA-vecB);
        vec3 cpTGT = vecA+(vecA-vecB)*5.f;
        //vec3 c = cross(cpPOV, cpTGT);


        // Saving transform status
        transformsClass *objT = getParticlesSystem()->getTMat();

        //mat4 mModel(objT->getModelMatrix()), mView(objT->getProjMatrix()), mProj(objT->getProjMatrix());
        const float perspAngle = objT->getPerspAngle();
        vec3 vPOV(objT->getPOV()), vTGT(objT->getTGT());
        vec3 vPos(objT->getTrackball().getPosition()), vCoR(objT->getTrackball().getRotationCenter());
        quat qRot = objT->getTrackball().getRotation();
        float pNear = objT->getPerspNear();

//        mat4 m = mat4(1.f) * vec4(cpTGT, 1.0);
        mat4 m = translate(mat4(1.f), cpPOV);
        m = m * mat4_cast(cPit.getRotation());
        //m = translate(m, cpPOV);
        cpTGT = m * cpTGT;

        particlesSystem->getTMat()->setPerspective(particlesSystem->getCockpit().getPerspAngle(), float(w)/float(h), particlesSystem->getCockpit().getPerspNear(), objT->getPerspFar());
        particlesSystem->getTMat()->setView(cpPOV, cpTGT);

        // New view cockpit
        particlesSystem->getTMat()->getTrackball().setRotation(quat(1.0f,0.0f, 0.0f, 0.0f));
        particlesSystem->getTMat()->getTrackball().setRotationCenter(vec3(0.f));

        particlesSystem->getTMat()->getTrackball().setPosition(vec3(0.f));
        particlesSystem->getTMat()->getTrackball().viewportSize(w, h);
        particlesSystem->getTMat()->applyTransforms();


        if(cPit.invertPIP()) cPit.setViewport(w,h);
        else                 glViewport(0,0, w, h);

        renderProcedure();

        // Restore transform status

        particlesSystem->getTMat()->setView(vPOV, vTGT);
        particlesSystem->getTMat()->setPerspective(perspAngle, float(w)/float(h), pNear, objT->getPerspFar());

        //objT->setModelMatrix(mModel);
        //objT->setProjMatrix(mView);
        //objT->setProjMatrix(mProj);        

        particlesSystem->getTMat()->getTrackball().viewportSize(w, h);

        particlesSystem->getTMat()->getTrackball().setRotation(qRot);
        particlesSystem->getTMat()->getTrackball().setPosition(vPos);
        particlesSystem->getTMat()->getTrackball().setRotationCenter(vCoR);
        particlesSystem->getTMat()->applyTransforms();

        if(!cPit.invertPIP() && cPit.getPIPposition() != cPit.pip::noPIP){
            cPit.setViewport(w,h);
            renderProcedure();
        }

    } else {
        glViewport(0,0, w, h);
        texRendered = particlesSystem->render();
    }

    return texRendered;

}


////////////////////////////////////////////////////////////////////////////
void glWindow::onIdle()
{
    particlesSystem->getTMat()->getTrackball().idle();
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

