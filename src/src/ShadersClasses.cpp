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
#include <fastPRNG.h>

#include "glWindow.h"

//#include "attractorsBase.h"
//#include "ParticlesUtils.h"

//
// PointSprite 
//
////////////////////////////////////////////////////////////////////////////////
shaderPointClass::shaderPointClass() 
{ 
    setSize(6.f);

    srcBlendAttrib = GL_SRC_ALPHA;
    dstBlendAttrib = GL_ONE;
    srcBlendAttribA = GL_SRC_ALPHA;
    dstBlendAttribA = GL_ONE;
    srcIdxBlendAttrib  = 6;
    dstIdxBlendAttrib  = 1;
    srcIdxBlendAttribA = 6;
    dstIdxBlendAttribA = 1;

    setAlphaAtten(0.0);
    setPointSizeFactor(1.0);
    setClippingDist(.5);
    setAlphaKFactor(1.0);

}

void shaderPointClass::initShader()
{
    useVertex(); useFragment();

    getVertex  ()->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 2, SHADER_PATH "ParticlesVert.glsl", SHADER_PATH "PointSpriteVert.glsl");
    getFragment()->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 3, SHADER_PATH "lightModelsFrag.glsl", SHADER_PATH "ParticlesFrag.glsl", SHADER_PATH "PointSpriteFragLight.glsl");

    addVertex();
    addFragment();

    link();

    removeAllShaders(true);

    setCommonData();
}


//
//  particlesBase 
//
////////////////////////////////////////////////////////////////////////////////
GLuint particlesBaseClass::render(GLuint fbIdx, emitterBaseClass *emitter, bool isFullScreenPiP, bool cpitView)
{
    setFlagUpdate();
    const GLsizei shadowDetail = theApp->useDetailedShadows() ? GLsizei(2) : GLsizei(1);
    const float lightReduction = theApp->useDetailedShadows() ? 1.f : 1.f /*1.f : .667f*/;

#if !defined(GLCHAOSP_NO_TF)
    const bool isTFRender = attractorsList.get()->dtType() && tfSettinsClass::tfMode(); // transformFeedback Render
#else
    const bool isTFRender = false;
#endif
    const bool computeShadow = useShadow() && !cpitView; // FIXME: cockPit Shadow: no shadow on tfSettings
    const bool blendActive = getBlendState() || showAxes();
    const bool isAO_SHDW = ( useAO() || computeShadow);
    const bool isAO_RD_SHDW = isAO_SHDW || postRenderingActive();
    const bool isSolid = ( getDepthState() ||  getLightState());
    const bool isShadow = ( computeShadow && isSolid);

    getUData().renderType = (blendActive && !isSolid)            ? pixColIDX::pixBlendig :
                             blendActive || !isAO_RD_SHDW        ? pixColIDX::pixDirect  :
                             isAO_SHDW && !postRenderingActive() ? pixColIDX::pixAO      :
                             isFullScreenPiP                     ? pixColIDX::pixDR : pixColIDX::pixDirect; //!isFullScreenPiP: no dual pass on tfSettings
    
    //isFullScreenPiP: no dual pass on PiP
    getUData().pass = (scatteredShadow ? 8:0) | (isShadow ? 4:0) | (postRenderingActive() && isFullScreenPiP ? 2:0) | (useAO() ? 1:0);

    transformsClass *currentTMat = cpitView ? getCockPitTMat() : getTMat();
    
    getUPlanes().buildInvMV_forPlanes(this);    // checkPlanes and eventually build invMat
    
    tfSettinsClass &cPit = getTFSettings();

    //if(checkFlagUpdate()) {
        //getUData().scrnRes = vec2((isTFRender && cPit.getPIPposition() == cPit.pip::splitView) ? getRenderFBO().getSizeX()<<1 : getRenderFBO().getSizeX(), (isTFRender && cPit.getPIPposition() == cPit.pip::splitView) ? getRenderFBO().getSizeY()<<1 : getRenderFBO().getSizeY());
        getUData().scrnRes = vec2(getRenderFBO().getSizeX(), getRenderFBO().getSizeY());
        getUData().invScrnRes = 1.f/getUData().scrnRes;
        getUData().ySizeRatio = theApp->isParticlesSizeConstant() ? 1.0 : float(getUData().scrnRes.y/1024.0);
        getUData().ptSizeRatio = 1.0/(length(getUData().scrnRes) / getUData().scrnRes.x);
        getUData().velocity = getCMSettings()->getVelIntensity();        
        getUData().shadowDetail = float(theApp->useDetailedShadows() ? 2.0f : 1.f);
        getUData().rotCenter = currentTMat->getTrackball().getRotationCenter();
        getUData().lightDir = normalize(getLightDir());
    //}

    getUData().slowMotion = isTFRender;

    float distAtt = getUData().pointDistAtten;      // FIXME: use external setting so don't save and restore below
    if(isTFRender) getUData().pointDistAtten = 0.f; // no distance attenuation on cpitView

    getUData().elapsedTime   = cPit.getUdata().elapsedTime;

    getUData().lifeTime      = tfSettinsClass::getPIPposition() && cPit.cockPit() ? std::max(cPit.getLifeTimeCP(),cPit.getLifeTime()) :
            cpitView ? cPit.getLifeTimeCP() : cPit.getLifeTime();           // if PiP get max for both to sync view
    getUData().lifeTimeAtten = tfSettinsClass::getPIPposition() && cPit.cockPit() ? std::max(cPit.getLifeTimeAttenCP(), cPit.getLifeTimeAtten()) :
            cpitView ? cPit.getLifeTimeAttenCP() : cPit.getLifeTimeAtten(); // if PiP get max for both to sync view

    getUData().smoothDistance= cPit.getSmoothDistance();
    getUData().vpReSize      = isFullScreenPiP ? 1.0 : cPit.getPIPzoom()*.5;

    getUData().zNear = currentTMat->getPerspNear();
    getUData().zFar  = currentTMat->getPerspFar();
   
    //getUData().pointSize = cpitView ? cPit.getPointSize() : ( isTFRender ? cPit.getSizeTF() : getSize());

    if(cpitView) {          // TransformFedback cockpit view
        getUData().pointSize = cPit.getPointSize();
        getUData().magnitude = cPit.getCpMagnitude();
        getUData().magnitudeInt = cPit.getCpMagnitudeInt();
        getUData().invMagnitudeAtten = 1.f/cPit.getCpMagnitudeAtten();
    } else if(isTFRender) { // TransformFedback full screen
        getUData().pointSize = cPit.getSizeTF();
        getUData().magnitude = cPit.getMagnitude();
        getUData().magnitudeInt = cPit.getMagnitudeInt();
        getUData().invMagnitudeAtten = 1.f/cPit.getMagnitudeAtten();
    } else {                // Single point emitter
        getUData().pointSize = getSize();
        getUData().magnitude = 1.0;
        getUData().magnitudeInt = 0.0;
        getUData().invMagnitudeAtten = 1.0;
    }

// Shadow pass
/////////////////////////////////////////////
#if !defined(GLCHAOSP_NO_AO_SHDW)

    const vec3 light(getLightDir()*lightReduction);
    const vec3 lightTGT(currentTMat->getTGT());

    if(computeShadow && !blendActive ) {
        //FIXME: POV.z+Dolly.z (shadow) and Light Distance
        const vec3 tmpPov = currentTMat->getPOV();
        const float tmpDolly = currentTMat->getTrackball().getDollyPosition().z;
        currentTMat->setPOV(vec3(vec2(currentTMat->getPOV()), currentTMat->getOverallDistance()));
        currentTMat->getTrackball().setDollyPosition(0.f);
        currentTMat->applyTransforms();

        if(autoLightDist() ) {
            const float dist = currentTMat->getOverallDistance()<FLT_EPSILON ?  FLT_EPSILON : currentTMat->getOverallDistance();
            setLightDir(normalize(getLightDir()) * (dist*(theApp->useDetailedShadows() ? 1.5f : 1.5f /* 2/2.5*/) + dist*.1f));
        }
        glViewport(0,0, getUData().scrnRes.x*shadowDetail, getUData().scrnRes.y*shadowDetail);

        getShadow()->bindRender();

        currentTMat->setLightView(light,lightTGT);
        mat4 m(1.f);

        currentTMat->tM.mvLightM = currentTMat->tM.mvLightM*currentTMat->tM.mMatrix;

        currentTMat->updateBufferData();
        getPlanesUBlock().updateBufferData();
        updateBufferData();

        // render shadow
        emitter->renderEvents();

        getShadow()->releaseRender();

        //FIXME: POV.z+Dolly.z (shadow) ==> look UP
        currentTMat->setPOV(tmpPov);
        currentTMat->getTrackball().setDollyPosition(tmpDolly);
        currentTMat->applyTransforms();
        glViewport(0,0, getUData().scrnRes.x, getUData().scrnRes.y);
    }
#endif


// Select and clear main FBO to draw 
/////////////////////////////////////////////
    GLuint returnedTex = getRenderFBO().getTex(fbIdx);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, getRenderFBO().getFB(fbIdx));

    // Clear Depth buffer
    if(depthBuffActive) {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
    }
    const GLfloat f=1.0f;
    glClearBufferfv(GL_DEPTH , 0, &f);

    // clear Color buffer
    const vec4 bkgColor(backgroundColor());
    if(!showAxes()) glClearBufferfv(GL_COLOR,  0, value_ptr(bkgColor));

    if(blendActive || showAxes()) {
        glEnable(GL_BLEND);
#ifdef BLEND_EQ_SEPARATE
        glBlendEquationSeparate(getBlendEqRGB(), getBlendEqAlpha());
        glBlendFuncSeparate(getSrcBlend(), getDstBlend(), getSrcBlendA(), getDstBlendA());
#else
        glBlendEquation(getBlendEqRGB());
        glBlendFunc(getSrcBlend(), getDstBlend());
#endif
    }


// Normal Render
/////////////////////////////////////////////

    getPlanesUBlock().updateBufferData();
    currentTMat->updateBufferData();
    updateBufferData();

    bindShaderProg();
#ifdef GLAPP_REQUIRE_OGL45
    glBindTextureUnit(0, colorMap->getModfTex());
    glBindTextureUnit(1, dotTex.getTexID());
#else
    glActiveTexture(GL_TEXTURE0+colorMap->getModfTex());
    glBindTexture(GL_TEXTURE_2D,colorMap->getModfTex());

    glActiveTexture(GL_TEXTURE0+dotTex.getTexID());
    glBindTexture(GL_TEXTURE_2D,dotTex.getTexID());

    setUniform1i(locDotsTex, dotTex.getTexID());
    updatePalTex();
#endif
    selectSubroutine();


    // render particles
    emitter->renderEvents();


//    glBindTexture(GL_TEXTURE_2D, 0);
//    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
//    resetShaderProg();


// AO & Shadows process
/////////////////////////////////////////////
#if !defined(GLCHAOSP_NO_AO_SHDW)
    if(!blendActive && isAO_RD_SHDW)  {

        if(!cpitView) {
            currentTMat->setLightView(light,lightTGT);
            mat4 m(1.f);

            m = translate(m,vec3(vec2(0.f), currentTMat->getOverallDistance()));
            currentTMat->tM.mvLightM = currentTMat->tM.mvLightM * m;
            //currentTMat->tM.mvLightM = (currentTMat->tM.mvLightM * currentTMat->tM.mMatrix) ;
            currentTMat->tM.mvpLightM = currentTMat->tM.pMatrix * currentTMat->tM.mvLightM;
            getUData().halfTanFOV = tanf(currentTMat->getPerspAngleRad()*.5f);
        }

// AO frag
/////////////////////////////////////////////
        if(useAO()) {
            currentTMat->updateBufferData();
            getAO()->bindRender(this, fbIdx, bkgColor);
            getAO()->render();
            getAO()->releaseRender();
        }

// PostRendering frag
/////////////////////////////////////////////

        currentTMat->updateBufferData();
        returnedTex = getPostRendering()->bindRender(this, fbIdx, bkgColor);
        getPostRendering()->render();
        getPostRendering()->releaseRender();
    }
#endif
    //restore GL state
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    //glDisable(GL_MULTISAMPLE);

    // FIXME: use external setting so don't save and restore below ==> look UP
    if(isTFRender) getUData().pointDistAtten = distAtt;

    //glViewport(0,0, getUData().scrnRes.x, getUData().scrnRes.y);
    getFboContainer().lockTex(returnedTex);
    return returnedTex;
}

#if !defined(GLCHAOSP_NO_BB)

//
//  Billboard
//
////////////////////////////////////////////////////////////////////////////////
shaderBillboardClass::shaderBillboardClass()
{
    setSize(4.);

    srcBlendAttrib = GL_SRC_ALPHA;
    dstBlendAttrib = GL_ONE_MINUS_SRC_ALPHA;
    srcIdxBlendAttrib = 6;
    dstIdxBlendAttrib = 7;

    setAlphaAtten(0.0);
    setPointSizeFactor(1.0);
    setClippingDist(.5);
    setAlphaKFactor(1.0);
}

void shaderBillboardClass::initShader()
{
    useAll();

    getVertex  ()->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 2, SHADER_PATH "ParticlesVert.glsl", SHADER_PATH "BillboardVert.glsl");
    getGeometry()->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 1, SHADER_PATH "BillboardGeom.glsl");
    getFragment()->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 3, SHADER_PATH "lightModelsFrag.glsl", SHADER_PATH "ParticlesFrag.glsl", SHADER_PATH "BillboardFrag.glsl");

    // The vertex and fragment are added to the program object
    addVertex();
    addGeometry();
    addFragment();

    link();

    removeAllShaders(true);

    setCommonData();
}
#endif //GLCHAOSP_NO_BB


#if !defined(GLCHAOSP_NO_MB)
//
//  motionBlur
//
////////////////////////////////////////////////////////////////////////////////
void motionBlurClass::create()
{
#ifdef GLAPP_USES_GLSL_PIPELINE
    glUseProgramStages(getPipeline(), GL_VERTEX_SHADER_BIT, renderEngine->getSeparableVertex());
#else
    useVertex(renderEngine->getCommonVShader());
    addShader(vertObj);
#endif
    useFragment();
    fragObj->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 1, SHADER_PATH "MotionBlurFS.glsl");
    addShader(fragObj);

    link();

    removeAllShaders(true);

    bindShaderProg();
#ifndef GLAPP_REQUIRE_OGL45
    //bindPipeline();
    //USE_PROGRAM
    LOCsourceRendered = getUniformLocation("sourceRendered");
    LOCaccumMotion =    getUniformLocation("accumMotion");
#endif
    LOCblurIntensity =    getUniformLocation("blurIntensity");

    updateBlurIntensity();
}

GLuint motionBlurClass::render(GLuint renderedTex)
{
    //if(!isActive) return renderedTex;



    glDisable(GL_BLEND);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mBlurFBO.getFB(rotationBuff^1));
    //glInvalidateBufferData(rotationBuff^1);
    glClear(GL_COLOR_BUFFER_BIT);

//leggo dal rendered buffer
    bindShaderProg();
#ifdef GLAPP_REQUIRE_OGL45
    glBindTextureUnit(0, renderedTex);
    glBindTextureUnit(1, mBlurFBO.getTex(rotationBuff));
#else
    glActiveTexture(GL_TEXTURE0 + renderedTex);
    glBindTexture(GL_TEXTURE_2D,  renderedTex);

    glActiveTexture(GL_TEXTURE0 + mBlurFBO.getTex(rotationBuff));
    glBindTexture(GL_TEXTURE_2D,  mBlurFBO.getTex(rotationBuff));

    updateSourceRenderedTex(renderedTex);
    updateAccumMotionTex(mBlurFBO.getTex(rotationBuff));
#endif
    if(renderEngine->checkFlagUpdate()) updateBlurIntensity();

    theWnd->getVAO()->draw();

    rotateBuffer();

    // return texture associated to FB rendered
    return mBlurFBO.getFB(rotationBuff);

}
#endif // GLCHAOSP_NO_MB

//
// RenderBase
//
////////////////////////////////////////////////////////////////////////////////
renderBaseClass::renderBaseClass()
{

#define PB(ID,NAME) blendArray.push_back(ID); blendingStrings.push_back(NAME);
    PB(GL_ZERO                    ,"Zero"                 )
    PB(GL_ONE                     ,"One"                  )
    PB(GL_SRC_COLOR               ,"SrcColor"             )
    PB(GL_ONE_MINUS_SRC_COLOR     ,"OneMinusSrcColor"     )
    PB(GL_DST_COLOR               ,"DstColor"             )
    PB(GL_ONE_MINUS_DST_COLOR     ,"OneMinusDstColor "    )
    PB(GL_SRC_ALPHA               ,"SrcAlpha"             )
    PB(GL_ONE_MINUS_SRC_ALPHA     ,"OneMinusSrcAlpha"     )
    PB(GL_DST_ALPHA               ,"DstAlpha"             )
    PB(GL_ONE_MINUS_DST_ALPHA     ,"OneMinusDstAlpha"     )
    PB(GL_CONSTANT_COLOR          ,"ConstantColor	"      )
    PB(GL_ONE_MINUS_CONSTANT_COLOR,"OneMinusConstantColor")
    PB(GL_CONSTANT_ALPHA          ,"ConstantAlpha"        )
    PB(GL_ONE_MINUS_CONSTANT_ALPHA,"OneMinusConstantAlpha")
    PB(GL_SRC_ALPHA_SATURATE      ,"SrcAlphaSaturate"     )
#if !defined(GLCHAOSP_LIGHTVER) && !defined(GLAPP_USES_ES3)
    PB(GL_SRC1_COLOR              ,"Src1Color"            )
    PB(GL_ONE_MINUS_SRC1_COLOR    ,"OneMinusSrc1Color"    )
    PB(GL_SRC1_ALPHA              ,"Src1Alpha"            )
    PB(GL_ONE_MINUS_SRC1_ALPHA    ,"OneMinusSrc1Alpha"    )
#endif
#undef PB

#define PB(ID,NAME) blendEqFunc.push_back(ID); blendEqStrings.push_back(NAME);
    PB(GL_FUNC_ADD             , "Add" )
    PB(GL_FUNC_SUBTRACT        , "Sub" )
    PB(GL_FUNC_REVERSE_SUBTRACT, "RSub")
    PB(GL_MIN                  , "Min" )
    PB(GL_MAX                  , "Max" )
#undef PB

#if !defined(GLCHAOSP_NO_AO_SHDW)
    ambientOcclusion = new ambientOcclusionClass(this);
    shadow  = new shadowClass(this);
    postRendering = new postRenderingClass(this);
#endif
}

void renderBaseClass::buildFBO()
{
    //msaaFBO.buildFBO(1, theApp->GetWidth(), theApp->GetHeight(), theApp->getFBOInternalPrecision(), true, 4);
    renderFBO.buildFBO(1, theApp->GetWidth(), theApp->GetHeight(),  theApp->getFBOInternalPrecision());
    renderFBO.attachMultiFB(1);

    renderFBO.attachDB(mmFBO::depthTexture,theApp->getDBInterpolation(),theApp->getClampToBorder());

    auxFBO.buildFBO(numAuxFBO, theApp->GetWidth(), theApp->GetHeight(),  theApp->getFBOInternalPrecision());

    fboContainer.insertItems(auxFBO,numAuxFBO);   // add to locked list
    fboContainer.addMainFBO(renderFBO);
}

void renderBaseClass::create()
{
    renderBaseClass::buildFBO();

    filterBase = new filtersBaseClass(this);
    pipWnd = new pipWndClass(filterBase);

    blitRender = new blitRenderClass(filterBase);
    imgTuningRender  = new imgTuningRenderClass(this->getFilter());
    fxaaRender = new fxaaRenderClass(this->getFilter());
    glowRender = new glowRenderClass(this->getFilter());

#if !defined(GLCHAOSP_NO_AX)
    axes = new oglAxes;
#endif
#if !defined(GLCHAOSP_NO_MB)
    motionBlur = new motionBlurClass(this);
#endif
#if !defined(GLCHAOSP_NO_BB)
    mergedRendering = new mergedRenderingClass(filterBase);
#endif

    {
#ifdef GLAPP_USES_GLSL_PIPELINE
        string str(theApp->get_glslVer() + theApp->get_glslDef());
        getFileContents(SHADER_PATH "mmFBO_all_vert.glsl", str);
        const char *s[] = { str.c_str(), "\0" };
        separableVertex = glCreateShaderProgramv(GL_VERTEX_SHADER, 1, s);
#else
        commonVShader.Load( (theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 1, SHADER_PATH "mmFBO_all_vert.glsl");
#endif
    }
    std::string verS = theApp->get_glslVer() + theApp->get_glslDef();
#ifdef GLAPP_USES_GLSL_PIPELINE
    verS += "#define GLAPP_USE_PIPELINE\n";
#endif

#if !defined(GLCHAOSP_NO_AO_SHDW)
    ambientOcclusion->create();
    shadow->create();
    postRendering->create();
#endif
    filterBase->create();

#if !defined(GLCHAOSP_NO_AX)
    axes->initShaders(verS.c_str(), verS.c_str());
#endif

#if !defined(GLCHAOSP_NO_MB)
    motionBlur->create();
#endif
}

renderBaseClass::~renderBaseClass()
{
    delete filterBase;

#if !defined(GLCHAOSP_NO_MB)
    delete motionBlur;
#endif
#if !defined(GLCHAOSP_NO_BB)
    delete mergedRendering;
#endif
#if !defined(GLCHAOSP_NO_AX)
     delete axes;
#endif

#if !defined(GLCHAOSP_NO_AO_SHDW)
    delete ambientOcclusion;
    delete shadow;
    delete postRendering;
#endif
    delete pipWnd;
    delete imgTuningRender;
    delete fxaaRender;
    delete glowRender;
}

void renderBaseClass::setRenderMode(int which)
{
/*#if !defined(GLCHAOSP_NO_BB)
    if(which == RENDER_USE_BOTH && whichRenderMode!=RENDER_USE_BOTH) getMergedRendering()->Activate();
    else
        if(which!=RENDER_USE_BOTH && whichRenderMode==RENDER_USE_BOTH) getMergedRendering()->Deactivate();
#endif*/
    whichRenderMode=which;
    setFlagUpdate();
}

VertexShader* commonVShader = nullptr;

//
//  BlurBase (Glow)
//
////////////////////////////////////////////////////////////////////////////////
void filtersBaseClass::create()
{
#ifdef GLAPP_USES_GLSL_PIPELINE
        glUseProgramStages(getPipeline(), GL_VERTEX_SHADER_BIT, renderEngine->getSeparableVertex());
#else
        useVertex(renderEngine->getCommonVShader());
        addShader(vertObj);
#endif
        useFragment();
        fragObj->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 2, SHADER_PATH "colorSpaces.glsl", SHADER_PATH "filtersFrag.glsl");
        addShader(fragObj);

        link();

        removeAllShaders(true);

#ifdef GLAPP_REQUIRE_OGL45
        uniformBlocksClass::create(GLuint(sizeof(uBlurData)), (void *) &uData);
#else
        USE_PROGRAM
        LOCorigTexture = getUniformLocation("origTexture");

        uniformBlocksClass::create(GLuint(sizeof(uBlurData)), (void *) &uData, getProgram(), "_filterData");
    #if !defined(GLCHAOSP_NO_BLUR)
        LOCauxTexture = getUniformLocation("auxTexture");
    #endif
    #if !defined(GLCHAOSP_LIGHTVER) && !defined(GLCHAOSP_NO_USES_GLSL_SUBS)
        idxSubGlowType[idxSubroutine_imgTuning         ]  = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "imgTuning"               );
        idxSubGlowType[idxSubroutine_BlurCommonPass1   ]  = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "radialPass1"             );
        idxSubGlowType[idxSubroutine_BlurGaussPass2    ]  = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "radialPass2"             );
        idxSubGlowType[idxSubroutine_BlurThresholdPass2]  = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "radialPass2withBilateral");
        idxSubGlowType[idxSubroutine_Bilateral         ]  = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "bilateralSmooth"         );
        idxSubGlowType[idxSubroutine_FXAA              ]  = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "fxaaData"              );
        idxSubGlowType[idxSubroutine_Blit              ]  = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "blitFilter"              );
        idxSubGlowType[idxSubroutine_MixTwoTex         ]  = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "mixTwoTex"               );
        idxSubGlowType[idxSubroutine_pipRender         ]  = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "pipRender"               );
    #endif
        //ProgramObject::reset();
#endif
}

void blitRenderClass::renderOnDB()
{
    fboContainerClass &fboContainer = filterShader->getRenderEngine()->getFboContainer();
    const GLuint srcTex = fboContainer.selectTex(); //select and unlock FBO texture

    bindData(idxSubroutine_Blit, 0, srcTex);

}
GLuint blitRenderClass::renderOnFB() //return locked texture associated FB
{
    fboContainerClass &fboContainer = filterShader->getRenderEngine()->getFboContainer();
    const GLuint srcTex = fboContainer.getLockedTex(); // get locked texture... unlock after...
    lockedFBO &fbo = fboContainer.getItem(filterShader->getRenderEngine()->getNumAuxFBO()-1); // Temporary on tail "UNLOCKED!"

    bindData(idxSubroutine_Blit, fbo.fb, srcTex);

    //fboContainer.unlockTex(srcTex);
    return fbo.tex;
}

void imgTuningRenderClass::render(imgTuningDataClass *imgT)
{
    fboContainerClass &fboContainer = filterShader->getRenderEngine()->getFboContainer();
    const GLuint srcTex = fboContainer.getLockedTex(); // get locked texture... unlock after...
    const GLuint dstFB  = fboContainer.selectFB();  //select and lock free FBO FB

    bindData(imgT, idxSubroutine_imgTuning, dstFB, srcTex);
    fboContainer.unlockTex(srcTex); // ... unlock now
}

void fxaaRenderClass::render(fxaaDataClass *fxaaData)
{

    fboContainerClass &fboContainer = filterShader->getRenderEngine()->getFboContainer();
    const GLuint srcTex = fboContainer.getLockedTex(); // get locked texture... unlock after...
    const GLuint dstFB  = fboContainer.selectFB();  //select and lock free FBO FB
    //const GLuint srcTex = fboContainer.selectTex(); //select and unlock FBO texture
    filterShader->getUData().fxaaData = vec4(fxaaData->getThreshold(), 1.f/fxaaData->getReductMul(), 1.f/fxaaData->getReductMin(), fxaaData->getSpan());

    bindData(idxSubroutine_FXAA, dstFB, srcTex);
    fboContainer.unlockTex(srcTex); // ... unlock now
}

#if !defined(GLCHAOSP_NO_BB)
void mergedRenderingClass::mixRender(GLuint auxTex)
{
    fboContainerClass &fboContainer = filterShader->getRenderEngine()->getFboContainer();
    const GLuint srcTex = fboContainer.getLockedTex(); // get locked texture... unlock after...
    lockedFBO &fbo = fboContainer.selectFBO(); // select and lock FBO

    filterShader->getUData().mixTexture = (mixingVal+1.f)*.5f;

    bindData(idxSubroutine_MixTwoTex, fbo.fb, srcTex, auxTex);

    fboContainer.unlockTex(srcTex); // ... unlock now
    fboContainer.unlockTex(auxTex); // ... unlock
}
#endif

void pipWndClass::pipRender(GLuint auxTex, const vec4& viewport, const vec2& transp_intens, bool border, const vec4& borderColor)
{
    fboContainerClass &fboContainer = filterShader->getRenderEngine()->getFboContainer();
    const GLuint srcTex = fboContainer.getLockedTex(); // get locked texture... unlock after...
    lockedFBO &fbo = fboContainer.selectFBO(); // select and lock FBO

    filterShader->getUData().pipViewport = viewport; // viewport limits
    filterShader->getUData().toneMapVals = 1.f/vec2(viewport.z-viewport.x, viewport.w-viewport.y); // size;
    filterShader->getUData().mixTexture = transp_intens.x;
    filterShader->getUData().threshold  = transp_intens.y;
    filterShader->getUData().toneMapping = border; //bool value
    filterShader->getUData().fxaaData = borderColor;

    bindData(idxSubroutine_pipRender, fbo.fb, srcTex, auxTex);

    fboContainer.unlockTex(srcTex); // ... unlock now
    fboContainer.unlockTex(auxTex); // ... unlock
}

void glowRenderClass::render(glowDataClass *glowData)
{
#if !defined(GLCHAOSP_NO_BLUR)
    fboContainerClass &fboContainer = filterShader->getRenderEngine()->getFboContainer();

    GLuint srcTex = fboContainer.getLockedTex(); // get locked texture... unlock after...

    if((glowData->getGlowState()==glowType_Blur || glowData->getGlowState()==glowType_Threshold)) {
        lockedFBO &fbo1 = fboContainer.selectFBO(), fbo2 = fboContainer.selectFBO(); //select and lock 2 FBO

        bindData(glowData, idxSubroutine_BlurCommonPass1, fbo1.fb, srcTex);
        bindData(glowData, glowData->getGlowState()==glowType_Blur ? idxSubroutine_BlurGaussPass2 : idxSubroutine_BlurThresholdPass2,
                                                fbo2.fb, srcTex, fbo1.tex);
        fboContainer.unlockFB(fbo1.fb); // unlock fbo1 not more used
    } else {
        lockedFBO &fbo = fboContainer.selectFBO();
        bindData(glowData, idxSubroutine_Bilateral, fbo.fb, srcTex);
    }

    fboContainer.unlockTex(srcTex); // ... unlock now
#endif
}

void filtersBaseClass::updateDataAndDraw()
{
    getUData().invScreenSize = 1.f/vec2(float(renderEngine->getAuxFBO().getSizeX()), float(renderEngine->getAuxFBO().getSizeY()));
    updateBufferData();
    theWnd->getVAO()->draw();
#if !defined(GLAPP_USES_GLSL_PIPELINE) // Necessary! : WebGL GL_INVALID_OPERATION: Feedback loop formed between Framebuffer and active Texture.
    glBindTexture(GL_TEXTURE_2D, 0); // Perhaps necessary also on secondary texture for 2 pass filters (???): now works
#endif
}

//
//  colorMapTextured
//
////////////////////////////////////////////////////////////////////////////////
void colorMapTexturedClass::create()
{
#ifdef GLAPP_USES_GLSL_PIPELINE
    glUseProgramStages(getPipeline(), GL_VERTEX_SHADER_BIT, particles->getSeparableVertex());
#else
    useVertex(particles->getCommonVShader());
    addShader(vertObj);
#endif

    useFragment();
    fragObj->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 2, SHADER_PATH "colorSpaces.glsl", SHADER_PATH "cmTexturedFrag.glsl");
    addShader(fragObj);

    link();

    removeAllShaders(true);

#ifdef GLAPP_REQUIRE_OGL45
    uniformBlocksClass::create(GLuint(sizeof(uCMapData)), (void *) &uData);
#else
    uniformBlocksClass::create(GLuint(sizeof(uCMapData)), (void *) &uData, getProgram(), "_cmData");
#endif
    LOCpaletteTex = getUniformLocation("paletteTex");
}

void colorMapTexturedClass::render(int tex)
{
    if(!flagUpdate) return;

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, cmTex.getFB(0));
    glViewport(0,0,cmTex.getSizeX(),cmTex.getSizeY());
    //glClear(GL_COLOR_BUFFER_BIT);

    bindShaderProg();
#ifdef GLAPP_REQUIRE_OGL45
    glBindTextureUnit(0, particles->getPaletteTexID());
#else
    glActiveTexture(GL_TEXTURE0 + particles->getPaletteTexID());
    glBindTexture(GL_TEXTURE_2D,  particles->getPaletteTexID());

    setUniform1i(LOCpaletteTex, particles->getPaletteTexID());
#endif
    updateBufferData();

    clearFlagUpdate();

    theWnd->getVAO()->draw();
}

//
//  postRenderingClass
//
////////////////////////////////////////////////////////////////////////////////
postRenderingClass::postRenderingClass(renderBaseClass *ptrRE) : renderEngine(ptrRE)
{
    //fbo.buildFBO(1, theApp->GetWidth(), theApp->GetHeight(), theApp->getFBOInternalPrecision());
}

void postRenderingClass::create() {

    useVertex();
    useFragment();

    vertObj->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 1, SHADER_PATH "postRenderingVert.glsl");
    fragObj->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 2, SHADER_PATH "lightModelsFrag.glsl", SHADER_PATH "postRenderingFrag.glsl");
    addShader(vertObj);
    addShader(fragObj);

    link();

    removeAllShaders(true);

#if !defined(GLAPP_REQUIRE_OGL45)
        USE_PROGRAM
        locAOTex = getUniformLocation("aoTex");
        locShadowTex = getUniformLocation("shadowTex");
        locPrevData = getUniformLocation("prevData");
        locZTex = getUniformLocation("zTex");
        locTexBaseColor = getUniformLocation("texBaseColor");

        uniformBlocksClass::bindIndex(getProgram(), "_particlesData", uniformBlocksClass::bindIdx);
        renderEngine->getTMat()->blockBinding(getProgram());
        //if present in the shader, WebGL want to bindIndex also if not used
        uniformBlocksClass::bindIndex(getProgram(), "_clippingPlanes", GLuint(renderBaseClass::bind::planesIDX));

    #if !defined(GLCHAOSP_LIGHTVER) && !defined(GLCHAOSP_NO_USES_GLSL_SUBS)
        locSubLightModel = glGetSubroutineUniformLocation(getProgram(), GL_FRAGMENT_SHADER, "lightModel");

        idxSubLightModel[0] = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "specularPhong");
        idxSubLightModel[1] = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "specularBlinnPhong");
        idxSubLightModel[2] = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "specularGGX");
    #endif

#endif
}

GLuint postRenderingClass::bindRender(particlesBaseClass *particle, GLuint fbIdx, const vec4 &bkgColor)
{
    mmFBO &renderFBO = renderEngine->getRenderFBO();


    const int idxFBO = 2;
    const GLuint fb = renderEngine->getAuxFBO().getFB(idxFBO);
    bindShaderProg();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb);
    //particle->getFboContainer().lockItem(1);

    //const vec4 bkg(particle->backgroundColor());
    glClearBufferfv(GL_COLOR,  0, value_ptr(bkgColor));

    particle->updateBufferData();

#ifdef GLAPP_REQUIRE_OGL45
        GLuint subIDX = particle->getUData().lightModel + particlesBaseClass::lightMDL::modelOffset;

        glBindTextureUnit(6, particle->getAuxFBO().getTex(0));
        glBindTextureUnit(7, renderEngine->getShadow()->getFBO().getDepth(0));
        glBindTextureUnit( 5, renderFBO.getTex(fbIdx));
        glBindTextureUnit( 8, renderFBO.getTexMultiFB(fbIdx, 0));
        glBindTextureUnit(10, renderFBO.getDepth(fbIdx));
#else
        GLuint subIDX = getSubIdx(particle->getUData().lightModel);

        if(particle->useAO()) {
            glActiveTexture(GL_TEXTURE0 + renderEngine->getAuxFBO().getTex(1));
            glBindTexture(GL_TEXTURE_2D,  renderEngine->getAuxFBO().getTex(1));
            setUniform1i(getLocAOTex() ,  renderEngine->getAuxFBO().getTex(1));
        }

        glActiveTexture(GL_TEXTURE0 +   renderEngine->getShadow()->getFBO().getDepth(0));
        glBindTexture(GL_TEXTURE_2D,    renderEngine->getShadow()->getFBO().getDepth(0));
        setUniform1i(getLocShadowTex(), renderEngine->getShadow()->getFBO().getDepth(0));

        glActiveTexture(GL_TEXTURE0 + renderFBO.getTex(fbIdx));
        glBindTexture(GL_TEXTURE_2D,  renderFBO.getTex(fbIdx));
        setUniform1i(locPrevData,     renderFBO.getTex(fbIdx));

        glActiveTexture(GL_TEXTURE0 + renderFBO.getTexMultiFB(fbIdx, 0));
        glBindTexture(GL_TEXTURE_2D,  renderFBO.getTexMultiFB(fbIdx, 0));
        setUniform1i(locTexBaseColor, renderFBO.getTexMultiFB(fbIdx, 0));

        glActiveTexture(GL_TEXTURE0 + renderFBO.getDepth(fbIdx));
        glBindTexture(GL_TEXTURE_2D,  renderFBO.getDepth(fbIdx));
        setUniform1i(locZTex,         renderFBO.getDepth(fbIdx));
#endif

#if !defined(GLCHAOSP_LIGHTVER) && !defined(GLCHAOSP_NO_USES_GLSL_SUBS)
        glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, GLsizei(1), &subIDX);
#endif
        return renderEngine->getAuxFBO().getTex(idxFBO);
}

void postRenderingClass::render()
{

    theWnd->getVAO()->draw();
    CHECK_GL_ERROR()
}

void postRenderingClass::releaseRender()
{
#if !defined(GLAPP_REQUIRE_OGL45)
//    ProgramObject::resetShaderProg();
//    glBindTexture(GL_TEXTURE_2D,0);
//    glBindFramebuffer(GL_FRAMEBUFFER,0);
#endif
}

//
//  ambientOcclusionClass
//
////////////////////////////////////////////////////////////////////////////////
ambientOcclusionClass::ambientOcclusionClass(renderBaseClass *ptrRE) : renderEngine(ptrRE)
{
    //fbo.buildFBO(1, theApp->GetWidth(), theApp->GetHeight(), theApp->getFBOInternalPrecision());

    // generate sample kernel
    // ----------------------
#ifdef GLCHAOSP_AO_STD_RND
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    #define AO_RND_GEN() randomFloats(generator)
    #define AO_RND_GEN() (randomFloats(generator) * 2.0 - 1.0)
#else
    #define AO_RND_GEN_UNI() fastXS64s::xoshiro256p_UNI<float>()
    #define AO_RND_GEN_VNI() fastXS64s::xoshiro256p_VNI<float>()
#endif

    for(unsigned int i = 0; i < kernelSize; i++)  {
        vec3 sample(AO_RND_GEN_VNI(), AO_RND_GEN_VNI(), AO_RND_GEN_UNI());
        sample = normalize(sample);
        sample *= AO_RND_GEN_UNI();
        float scale = float(i) / float(kernelSize);

        // scale samples s.t. they're more aligned to center of kernel
        scale = mix(0.1f, 1.0f, scale * scale);
        sample *= scale;
        ssaoKernel.push_back(sample);
    }

    // generate noise texture
    // ----------------------
    std::vector<vec3> ssaoNoise;
    for(unsigned int i = 0; i < 16; i++) {
        //vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
        vec3 noise(AO_RND_GEN_UNI(), AO_RND_GEN_UNI(), 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }
    const GLenum precision = theApp->getPalInternalPrecision();   //GL_RGB32F || GL_RGB16F
#ifdef GLAPP_REQUIRE_OGL45
    glCreateTextures(GL_TEXTURE_2D, 1, &ssaoKernelTex);
    glTextureStorage2D(ssaoKernelTex, 1, precision, kernelSize, 1);
    glTextureSubImage2D(ssaoKernelTex, 0, 0, 0, kernelSize, 1, GL_RGB, GL_FLOAT, ssaoKernel.data());
    glTextureParameteri(ssaoKernelTex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(ssaoKernelTex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(ssaoKernelTex, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(ssaoKernelTex, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glCreateTextures(GL_TEXTURE_2D, 1, &noiseTexture);
    glTextureStorage2D(noiseTexture, 1, precision, 4, 4);
    glTextureSubImage2D(noiseTexture, 0, 0, 0, 4, 4, GL_RGB, GL_FLOAT, ssaoNoise.data());
    glTextureParameteri(noiseTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(noiseTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(noiseTexture, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(noiseTexture, GL_TEXTURE_WRAP_T, GL_REPEAT);
#else
    glGenTextures(1, &ssaoKernelTex);
    glBindTexture(GL_TEXTURE_2D, ssaoKernelTex);
    glTexImage2D(GL_TEXTURE_2D, 0, precision, kernelSize, 1, 0, GL_RGB, GL_FLOAT, ssaoKernel.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, precision, 4, 4, 0, GL_RGB, GL_FLOAT, ssaoNoise.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
#endif
    //create();


}

void ambientOcclusionClass::create() {

    //useVertex(renderEngine->getCommonVShader());
    useVertex();
    useFragment();

    vertObj->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 1, SHADER_PATH "postRenderingVert.glsl");
    fragObj->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 2, SHADER_PATH "lightModelsFrag.glsl", SHADER_PATH "ambientOcclusionFrag.glsl");
    addShader(vertObj);
    addShader(fragObj);

    link();

    removeAllShaders(true);

#if !defined(GLAPP_REQUIRE_OGL45)
    USE_PROGRAM
    locNoiseTexture = getUniformLocation("noise");
    //locPrevData = getUniformLocation("prevData");
    locZTex = getUniformLocation("zTex");
    locKernelTexture = getUniformLocation("ssaoSample");
    bindIDX = uniformBlocksClass::bindIndex(getProgram(), "_particlesData", uniformBlocksClass::bindIdx);
    //if present in the shader, WebGL want to bindIndex also if not used
    uniformBlocksClass::bindIndex(getProgram(), "_clippingPlanes", GLuint(renderBaseClass::bind::planesIDX));
    renderEngine->getTMat()->blockBinding(getProgram());

    setUniform1i(locKernelTexture, ssaoKernelTex);
    setUniform1i(locNoiseTexture, noiseTexture);
#endif
    //setUniform3fv(getUniformLocation("ssaoSamples"), kernelSize, (const GLfloat*)ssaoKernel.data());
}

void ambientOcclusionClass::bindRender(particlesBaseClass *particle, GLuint fbIdx, const vec4 &bkgColor)
{
    mmFBO &renderFBO = renderEngine->getRenderFBO();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, renderEngine->getAuxFBO().getFB(1));

    //const vec4 bkg(particle->backgroundColor());
    glClearBufferfv(GL_COLOR,  0, value_ptr(bkgColor));

    bindShaderProg();
    particle->updateBufferData();
#ifdef GLAPP_REQUIRE_OGL45
    glBindTextureUnit( 6, noiseTexture);
    glBindTextureUnit( 7, ssaoKernelTex);
    glBindTextureUnit(10, renderFBO.getDepth(fbIdx));
#else
    glActiveTexture(GL_TEXTURE0 + noiseTexture);
    glBindTexture(GL_TEXTURE_2D,  noiseTexture);

    glActiveTexture(GL_TEXTURE0 + ssaoKernelTex);
    glBindTexture(GL_TEXTURE_2D,  ssaoKernelTex);

    glActiveTexture(GL_TEXTURE0 + renderFBO.getDepth(fbIdx));
    glBindTexture(GL_TEXTURE_2D,  renderFBO.getDepth(fbIdx));
    setUniform1i(locZTex,         renderFBO.getDepth(fbIdx));
#endif
}

void ambientOcclusionClass::render()
{
    theWnd->getVAO()->draw();
    CHECK_GL_ERROR()
}

void ambientOcclusionClass::releaseRender()
{
#if !defined(GLAPP_REQUIRE_OGL45)
//    ProgramObject::resetShaderProg();
//    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
//    glBindTexture(GL_TEXTURE_2D, 0);
#endif
}

//
//  shadowClass
//
////////////////////////////////////////////////////////////////////////////////
shadowClass::shadowClass(renderBaseClass *ptrRE) : renderEngine(ptrRE) 
{
#if !defined(GLCHAOSP_LIGHTVER)
    const int detail = theApp->useDetailedShadows() ? 2 : 1;
    const int width = theApp->GetWidth()*detail, height = theApp->GetHeight()*detail;
#else
    const int width = theApp->GetWidth(), height = theApp->GetHeight();
#endif

    fbo.buildOnlyFBO(1, width, height, GL_RGBA32F);
    fbo.attachDB(mmFBO::depthTexture,theApp->getDBInterpolation(),theApp->getClampToBorder());
}

void shadowClass::create() {

    useVertex();
    useFragment();
    std::string s("#define SHADOW_PASS\n");
    vertObj->Load((theApp->get_glslVer() + theApp->get_glslDef() + s).c_str(), 2, SHADER_PATH "ParticlesVert.glsl", SHADER_PATH "PointSpriteVert.glsl");
    fragObj->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 2, SHADER_PATH "lightModelsFrag.glsl", SHADER_PATH "shadowFrag.glsl");
    addShader(vertObj);
    addShader(fragObj);

    link();

    removeAllShaders(true);

}

void shadowClass::bindRender()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.getFB(0));

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    const GLfloat f=1.0f;
    glClearBufferfv(GL_DEPTH , 0, &f);

    bindShaderProg();
#if !defined(GLAPP_REQUIRE_OGL45)
    uniformBlocksClass::bindIndex(getProgram(), "_particlesData", uniformBlocksClass::bindIdx);
    uniformBlocksClass::bindIndex(getProgram(), "_clippingPlanes", GLuint(renderBaseClass::bind::planesIDX));
    renderEngine->getTMat()->blockBinding(getProgram());
#endif
}

void shadowClass::render()
{
    //FixMe: move here Shadow rendere to make mainRender procedure clearer
}

void shadowClass::releaseRender()
{
}

tfSettinsClass::tfCommonsStruct tfSettinsClass::tfCommons;

void tfSettinsClass::setViewport(int w, int h) {
    const float szX = float(w)*getPIPzoom(), szY = float(h)*getPIPzoom();
    setViewportSize(vec2(szX, szY));

    //w++; h++;
    switch(getPIPposition()) {
        case pip::lTop:
            setViewportLimits(vec4(0, h-szY, szX, h));
            break;
        case pip::lBottom:
            setViewportLimits(vec4(0, 0, szX, szY));
            break;
        case pip::rTop:
            setViewportLimits(vec4(w-szX, h-szY, w, h));
            break;
        case pip::rBottom:
            setViewportLimits(vec4(w-szX, 0, w, szY));
            break;
        case pip::noPIP:
        default:
            setViewportLimits(vec4(0,0, w, h));
            break;
    }
}


