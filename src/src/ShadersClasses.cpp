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
#include <fastRandom.h>

#include "glWindow.h"

//#include "attractorsBase.h"
//#include "ParticlesUtils.h"


//
//  imgTuning
//
////////////////////////////////////////////////////////////////////////////////
imgTuningClass::imgTuningClass(dataBlurClass *ptrGlow) {
    glow = ptrGlow;
    videoControls.x = 2.3; //setGamma(2.5);
    videoControls.z = 0.0; //setBright(0.0);
    videoControls.w = 0.0; //setContrast(0.0);
    videoControls.y = 1.2; //setExposure(1.0);
    texControls.x = 1.0; //blurred image component
    texControls.y = 1.0; //Original image component
    texControls.z = 1.0; //Bilateral image component
    texControls.w = 0.3; // mix(Blur,bilateral,dot(Blur,bilateral)+w
    useDynEQ = false;
}

dataBlurClass::dataBlurClass() 
{ 
    //renderEngine = ptrRE;
    imageTuning = new imgTuningClass(this);
    mixTexture =.0f;        
}

//
// PointSprite 
//
////////////////////////////////////////////////////////////////////////////////
shaderPointClass::shaderPointClass() 
{ 
    setSize(6.f);

    srcBlendAttrib = GL_SRC_ALPHA;
    dstBlendAttrib = GL_ONE;
    srcIdxBlendAttrib = 6;
    dstIdxBlendAttrib = 1;


    setAlphaAtten(0.0);
    setPointSizeFactor(1.0);
    setClippingDist(.5);
    setAlphaKFactor(1.0);

    initShader();
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
    const float lightReduction = theApp->useDetailedShadows() ? .3333 : .25f;

    const bool isTFRender = attractorsList.get()->dtType() && tfSettinsClass::tfMode(); // transformFeedback Render
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
    getUData().pass = (isShadow ? 4:0) | (postRenderingActive() && isFullScreenPiP ? 2:0) | (useAO() ? 1:0);

    transformsClass *currentTMat = cpitView ? getCockPitTMat() : getTMat();
    
    getUPlanes().buildInvMV_forPlanes(this);    // checkPlanes and eventually build invMat
    
    if(checkFlagUpdate()) {
        getUData().scrnRes = vec2(getRenderFBO().getSizeX(), getRenderFBO().getSizeY());
        //getUData().scrnRes = vec2(vp.w, vp.h);
        getUData().invScrnRes = 1.f/getUData().scrnRes;
        getUData().ySizeRatio = theApp->isParticlesSizeConstant() ? 1.0 : float(getUData().scrnRes.y/1024.0);
        getUData().ptSizeRatio = 1.0/(length(getUData().scrnRes) / getUData().scrnRes.x);
        getUData().velocity = getCMSettings()->getVelIntensity();        
        getUData().shadowDetail = float(theApp->useDetailedShadows() ? 2.0f : 1.f);
        getUData().rotCenter = currentTMat->getTrackball().getRotationCenter();
        getUData().lightDir = normalize(vec3(currentTMat->tM.vMatrix * vec4(getLightDir(), 1.f)));
    }

    getUData().slowMotion = isTFRender;

    float distAtt = getUData().pointDistAtten;      // FIXME: use external setting so don't save and restore below
    if(isTFRender) getUData().pointDistAtten = 0.f; // no distance attenuation on cpitView

    tfSettinsClass &cPit = getTFSettings();
    getUData().elapsedTime   = cPit.getUdata().elapsedTime;
    getUData().lifeTime      = tfSettinsClass::getPIPposition() ? std::max(cPit.getLifeTimeCP(),cPit.getLifeTime()) :
            cpitView ? cPit.getLifeTimeCP() : cPit.getLifeTime();           // if PiP get max for both to sync view
    getUData().lifeTimeAtten = tfSettinsClass::getPIPposition() ? std::max(cPit.getLifeTimeAttenCP(), cPit.getLifeTimeAtten()) :
            cpitView ? cPit.getLifeTimeAttenCP() : cPit.getLifeTimeAtten(); // if PiP get max for both to sync view
    getUData().smoothDistance= cPit.getSmoothDistance();
    getUData().vpReSize      = isFullScreenPiP ? 1.0 : cPit.getPIPzoom()*.5;

    getUData().zNear = currentTMat->getPerspNear();
    getUData().zFar  = currentTMat->getPerspFar();
   
    getUData().pointSize = cpitView ? cPit.getPointSize() : ( isTFRender ? cPit.getSizeTF() : getSize());

// Shadow pass
/////////////////////////////////////////////
#if !defined(GLCHAOSP_LIGHTVER) || defined(GLCHAOSP_LIGHTVER_EXPERIMENTAL) 
    if(computeShadow && !blendActive ) {
        if(autoLightDist() ) {
            vec3 vL(normalize(getLightDir()));
            
            currentTMat->setPOV(currentTMat->getPOV()-vec3(0.f, 0.f, currentTMat->getTrackball().getDollyPosition().z));
            currentTMat->getTrackball().setDollyPosition(0.f);
            currentTMat->applyTransforms();

            const float dist = currentTMat->getPOV().z<FLT_EPSILON ?  FLT_EPSILON : currentTMat->getPOV().z;            
            setLightDir(vL * (dist*(theApp->useDetailedShadows() ? 5.5f : 4.f) + dist*.1f));
        }
        //if(shadowDetail>1) glViewport(0,0, getUData().scrnRes.x*shadowDetail, getUData().scrnRes.y*shadowDetail);
        ivec4 vp;
        if(isTFRender) glGetIntegerv(GL_VIEWPORT, (GLint *) value_ptr(vp));      // partial reverted: https://github.com/BrutPitt/glChAoS.P/commit/4bcfc6dd577255ac9460fcc656bcf6796e917c46#
        else vp = {0, 0, int(getUData().scrnRes.x), int(getUData().scrnRes.y) };

        glViewport(0,0, getUData().scrnRes.x*shadowDetail, getUData().scrnRes.y*shadowDetail);

        getShadow()->bindRender();

        currentTMat->setLightView(getLightDir()*lightReduction);
        currentTMat->tM.mvLightM = currentTMat->tM.mvLightM * currentTMat->tM.mMatrix;

        currentTMat->updateBufferData();
        getPlanesUBlock().updateBufferData();
        updateBufferData();

        // render shadow
        emitter->renderEvents();

        getShadow()->releaseRender();
        glViewport(vp.x,vp.y, vp.z, vp.w);

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
    if(!showAxes()) glClearBufferfv(GL_COLOR,  0, value_ptr(backgroundColor()));

    if(blendActive || showAxes()) {
        glEnable(GL_BLEND);
        glBlendFunc(getSrcBlend(), getDstBlend());
    }

// Normal Render
/////////////////////////////////////////////
    bindPipeline();

    getPlanesUBlock().updateBufferData();
    currentTMat->updateBufferData();
    updateBufferData();

     
#ifdef GLAPP_REQUIRE_OGL45
    glBindTextureUnit(0, colorMap->getModfTex());
    glBindTextureUnit(1, dotTex.getTexID());
#else
    glActiveTexture(GL_TEXTURE0+colorMap->getModfTex());
    glBindTexture(GL_TEXTURE_2D,colorMap->getModfTex());

    glActiveTexture(GL_TEXTURE0+dotTex.getTexID());
    glBindTexture(GL_TEXTURE_2D,dotTex.getTexID());

    USE_PROGRAM    

    setUniform1i(locDotsTex, dotTex.getTexID());
    updatePalTex();
#endif
    selectSubroutine();


    // render particles
    emitter->renderEvents();


// AO & Shadows process
/////////////////////////////////////////////
#if !defined(GLCHAOSP_LIGHTVER) || defined(GLCHAOSP_LIGHTVER_EXPERIMENTAL) 
    if(!blendActive && isAO_RD_SHDW)  {

        if(!cpitView) {
            currentTMat->setLightView(getLightDir()*lightReduction);
            mat4 m(1.f);
            m = translate(m,currentTMat->getPOV());
            currentTMat->tM.mvLightM = currentTMat->tM.mvLightM * m;
            currentTMat->tM.mvpLightM = currentTMat->tM.pMatrix * currentTMat->tM.mvLightM; 
            getUData().halfTanFOV = tanf(currentTMat->getPerspAngleRad()*.5f);
        }

// AO frag
/////////////////////////////////////////////
        if(useAO()) {
            currentTMat->updateBufferData();
            getAO()->bindRender(this, fbIdx);
            getAO()->render();
            getAO()->releaseRender();
            //returnedTex = getAO()->getFBO().getTex(0);
        }

// PostRendering frag
/////////////////////////////////////////////

        currentTMat->updateBufferData();
        getPostRendering()->bindRender(this, fbIdx);
        getPostRendering()->render();
        getPostRendering()->releaseRender();

        returnedTex = getPostRendering()->getFBO().getTex(0);
    }
#endif

    //restore GL state
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    //glDisable(GL_MULTISAMPLE);

    if(isTFRender) getUData().pointDistAtten = distAtt; // FIXME: look up

    return returnedTex;
}

#if !defined(GLCHAOSP_LIGHTVER)

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

    initShader();
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

//
//  mergedRendering
//
////////////////////////////////////////////////////////////////////////////////
GLuint mergedRenderingClass::render(GLuint texA, GLuint texB) 
{
    bindPipeline();

    const GLuint fbo = renderEngine->getMotionBlur()->Active() ? mergedFBO.getFB(0) : 0;
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    //glInvalidateBufferData(fbo);
    //glClear(GL_COLOR_BUFFER_BIT);
    CHECK_GL_ERROR()
#ifdef GLAPP_REQUIRE_OGL45
    glBindTextureUnit(0, texA);
    glBindTextureUnit(1, texB);
#else
    glActiveTexture(GL_TEXTURE0 + texA);
    glBindTexture(GL_TEXTURE_2D,  texA);

    glActiveTexture(GL_TEXTURE0 + texB);
    glBindTexture(GL_TEXTURE_2D,  texB);
    USE_PROGRAM
#endif

//leggo dal rendered buffer

    if(renderEngine->checkFlagUpdate()) {
        updatemixingVal();

#ifndef GLAPP_REQUIRE_OGL45
        updateBillboardTex(texA);  
        updatePointsTex(texB);
#endif
    }

    theWnd->getVAO()->draw();

    return mergedFBO.getTex(0);
}

void mergedRenderingClass::create() 
{
#ifdef GLAPP_NO_GLSL_PIPELINE
    useVertex(renderEngine->getCommonVShader());
    addShader(vertObj);
#else
    glUseProgramStages(getPipeline(), GL_VERTEX_SHADER_BIT, renderEngine->getSeparableVertex());
#endif

    useFragment();
    fragObj->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 1, SHADER_PATH "MergedRenderingFrag.glsl");
    addShader(fragObj);

    link();

    removeAllShaders(true);
 
#if !defined(GLAPP_REQUIRE_OGL45)
    LOCbillboardTex = getUniformLocation("billboardTex");
    LOCpointsTex =    getUniformLocation("pointTex");
#endif
    LOCmixingVal =    getUniformLocation("mixingVal");       

}

//
//  motionBlur
//
////////////////////////////////////////////////////////////////////////////////
void motionBlurClass::create() 
{
#ifdef GLAPP_NO_GLSL_PIPELINE
    useVertex(renderEngine->getCommonVShader());
    addShader(vertObj);
#else
    glUseProgramStages(getPipeline(), GL_VERTEX_SHADER_BIT, renderEngine->getSeparableVertex());
#endif
    useFragment();
    fragObj->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 1, SHADER_PATH "MotionBlurFS.glsl");
    addShader(fragObj);        

    link();

    removeAllShaders(true);

#ifndef GLAPP_REQUIRE_OGL45
    bindPipeline();
    USE_PROGRAM
    LOCsourceRendered = getUniformLocation("sourceRendered");
    LOCaccumMotion =    getUniformLocation("accumMotion");
#endif
    LOCblurIntensity =    getUniformLocation("blurIntensity");

    updateBlurIntensity();
}

GLuint motionBlurClass::render(GLuint renderedTex)
{
    //if(!isActive) return renderedTex;

    bindPipeline();

    glDisable(GL_BLEND);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mBlurFBO.getFB(rotationBuff^1));
    //glInvalidateBufferData(rotationBuff^1);
    glClear(GL_COLOR_BUFFER_BIT);

//leggo dal rendered buffer

#ifdef GLAPP_REQUIRE_OGL45
    glBindTextureUnit(0, renderedTex); 
    glBindTextureUnit(1, mBlurFBO.getTex(rotationBuff)); 
#else
    USE_PROGRAM
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
#endif

//
// RenderBase
//
////////////////////////////////////////////////////////////////////////////////
renderBaseClass::renderBaseClass()
{

#define PB(ID,NAME) blendArray.push_back(ID); blendingStrings.push_back(NAME);
    PB(GL_ZERO                     ,"Zero"                    )
    PB(GL_ONE                      ,"One"                     )
    PB(GL_SRC_COLOR                ,"Src_Color"               )
    PB(GL_ONE_MINUS_SRC_COLOR      ,"One_Minus_Src_Color"     )
    PB(GL_DST_COLOR                ,"Dst_Color"               )
    PB(GL_ONE_MINUS_DST_COLOR      ,"One_Minus_Dst_Color "    )
    PB(GL_SRC_ALPHA                ,"Src_Alpha"               )
    PB(GL_ONE_MINUS_SRC_ALPHA      ,"One_Minus_Src_Alpha"     )
    PB(GL_DST_ALPHA                ,"Dst_Alpha"               )
    PB(GL_ONE_MINUS_DST_ALPHA      ,"One_Minus_Dst_Alpha"     )
    PB(GL_CONSTANT_COLOR           ,"Constant_Color	"         )
    PB(GL_ONE_MINUS_CONSTANT_COLOR ,"One_Minus_Constant_Color")
    PB(GL_CONSTANT_ALPHA           ,"Constant_Alpha"          )
    PB(GL_ONE_MINUS_CONSTANT_ALPHA ,"One_Minus_Constant_Alpha")
    PB(GL_SRC_ALPHA_SATURATE       ,"Src_Alpha_Saturate"      )
#if !defined(GLCHAOSP_LIGHTVER)
    PB(GL_SRC1_COLOR               ,"Src1_Color"              )
    PB(GL_ONE_MINUS_SRC1_COLOR     ,"One_Minus_Src1_Color"    )
    PB(GL_SRC1_ALPHA               ,"Src1_Alpha"              )
    PB(GL_ONE_MINUS_SRC1_ALPHA     ,"One_Minus_Src1_Alpha"    )
#endif
#undef PB

    {
#ifdef GLAPP_NO_GLSL_PIPELINE
        commonVShader.Load( (theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 1, SHADER_PATH "mmFBO_all_vert.glsl");
#else
        string str(theApp->get_glslVer() + theApp->get_glslDef());
        getFileContents(SHADER_PATH "mmFBO_all_vert.glsl", str);
        const char *s[] = { str.c_str(), "\0" };
        separableVertex = glCreateShaderProgramv(GL_VERTEX_SHADER, 1, s);
#endif
    }
    renderBaseClass::buildFBO();

#if !defined(GLCHAOSP_LIGHTVER)
    //msaaFBO.buildFBO(1, theApp->GetWidth(), theApp->GetHeight(), theApp->getFBOInternalPrecision(), true, 4);

    axes = new oglAxes;

    std::string s = theApp->get_glslVer();
    #if !defined(GLAPP_NO_GLSL_PIPELINE)
        s += "#define GLAPP_USE_PIPELINE\n";
    #endif
    axes->initShaders(s.c_str(), theApp->get_glslVer().c_str());

    motionBlur = new motionBlurClass(this);
    mergedRendering = new mergedRenderingClass(this);
#endif

#if !defined(GLCHAOSP_LIGHTVER) || defined(GLCHAOSP_LIGHTVER_EXPERIMENTAL) 
    ambientOcclusion = new ambientOcclusionClass(this);
    shadow  = new shadowClass(this);
    postRendering = new postRenderingClass(this);
#endif
}

void renderBaseClass::buildFBO()
{
#if defined(GLCHAOSP_LIGHTVER) && !defined(GLCHAOSP_LIGHTVER_EXPERIMENTAL)
    GLuint precision = GL_RGBA16F;
#else
    GLuint precision = theApp->getFBOInternalPrecision();
#endif
    renderFBO.buildFBO(1, theApp->GetWidth(), theApp->GetHeight(),  precision); 
    renderFBO.attachMultiFB(1);

#if defined(GLCHAOSP_LIGHTVER)
    renderFBO.attachDB(mmFBO::depthTexture,GL_NEAREST,GL_CLAMP_TO_EDGE);  
#else
    renderFBO.attachDB(mmFBO::depthTexture,GL_LINEAR,GL_CLAMP_TO_BORDER);
#endif
}

void renderBaseClass::create()
{
    ambientOcclusion->create();
    shadow->create();
    postRendering->create();
}


renderBaseClass::~renderBaseClass()
{
#if !defined(GLCHAOSP_LIGHTVER)
     delete motionBlur; delete mergedRendering; 
     delete axes;
#endif

#if !defined(GLCHAOSP_LIGHTVER) || defined(GLCHAOSP_LIGHTVER_EXPERIMENTAL) 
     delete ambientOcclusion;
     delete shadow;
     delete postRendering; 
#endif
}

void renderBaseClass::setRenderMode(int which) 
{ 
#if !defined(GLCHAOSP_LIGHTVER)
    if(which == RENDER_USE_BOTH && whichRenderMode!=RENDER_USE_BOTH) getMergedRendering()->Activate();
    else 
        if(which!=RENDER_USE_BOTH && whichRenderMode==RENDER_USE_BOTH) getMergedRendering()->Deactivate();
#endif
    whichRenderMode=which;
    setFlagUpdate();
}

VertexShader* commonVShader = nullptr;

//
//  BlurBase (Glow)
//
////////////////////////////////////////////////////////////////////////////////
void BlurBaseClass::create()    
{
#ifdef GLAPP_NO_GLSL_PIPELINE
        useVertex(renderEngine->getCommonVShader());
        addShader(vertObj);
#else
        glUseProgramStages(getPipeline(), GL_VERTEX_SHADER_BIT, renderEngine->getSeparableVertex());
#endif
        useFragment();
        fragObj->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 2, SHADER_PATH "colorSpaces.glsl", SHADER_PATH "RadialBlur2PassFrag.glsl");
        addShader(fragObj);

        link();

        removeAllShaders(true);

#ifdef GLAPP_REQUIRE_OGL45
        uniformBlocksClass::create(GLuint(sizeof(uBlurData)), (void *) &uData);
#else
        USE_PROGRAM
        LOCorigTexture = getUniformLocation("origTexture");


        uniformBlocksClass::create(GLuint(sizeof(uBlurData)), (void *) &uData, getProgram(), "_blurData");
    #if !defined(GLCHAOSP_LIGHTVER) 
        LOCpass1Texture = getUniformLocation("pass1Texture");
        #if !defined(GLCHAOSP_NO_USES_GLSL_SUBS)
            idxSubGlowType[idxSubroutine_ByPass            ]  = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "byPass"                  );
            idxSubGlowType[idxSubroutine_BlurCommonPass1   ]  = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "radialPass1"             );
            idxSubGlowType[idxSubroutine_BlurGaussPass2    ]  = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "radialPass2"             );
            idxSubGlowType[idxSubroutine_BlurThresholdPass2]  = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "radialPass2withBilateral");
            idxSubGlowType[idxSubroutine_Bilateral         ]  = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "bilateralSmooth"         );
        #endif
    #endif
        //ProgramObject::reset();
#endif
}

void BlurBaseClass::glowPass(GLuint sourceTex, GLuint fbo, GLuint subIndex) 
{
    bindPipeline();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

    //glInvalidateBufferData(fbo);
    //glClear(GL_COLOR_BUFFER_BIT);	
    
#ifdef GLAPP_REQUIRE_OGL45
    glBindTextureUnit(0, sourceTex);
    glBindTextureUnit(1, glowFBO.getTex(RB_PASS_1));

    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, GLsizei(1), &subIndex);
    updateData(subIndex);
#else
    USE_PROGRAM
    glActiveTexture(GL_TEXTURE0+sourceTex);
    glBindTexture(GL_TEXTURE_2D,sourceTex);
    setUniform1i(LOCorigTexture, sourceTex);

    #if !defined(GLCHAOSP_LIGHTVER)
        glActiveTexture(GL_TEXTURE0+glowFBO.getTex(RB_PASS_1));
        glBindTexture(GL_TEXTURE_2D,  glowFBO.getTex(RB_PASS_1));
        setUniform1i(LOCpass1Texture, glowFBO.getTex(RB_PASS_1));

        #if !defined(GLCHAOSP_NO_USES_GLSL_SUBS)
            glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, GLsizei(1), &idxSubGlowType[subIndex]);
        #endif
    #endif
    updateData(subIndex);
#endif

    theWnd->getVAO()->draw();
    if(theApp->useSyncOGL()) glFinish(); // stuttering with frequent calls on slow GPU & APPLE
}

void BlurBaseClass::updateData(GLuint subIndex) 
{
    const float invSigma = 1.f/sigma.x;
    sigma.z = .5 * invSigma * invSigma;
    sigma.w = T_INV_PI * sigma.z;
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
    particlesSystemClass *pSys = theWnd->getParticlesSystem();
    getUData().invScreenSize = 1.f/vec2(float(pSys->getWidth()), float(pSys->getHeight()));

    updateBufferData();
}

//
//  colorMapTextured
//
////////////////////////////////////////////////////////////////////////////////
void colorMapTexturedClass::create()
{
#ifdef GLAPP_NO_GLSL_PIPELINE
    useVertex(particles->getCommonVShader());
    addShader(vertObj);
#else
    glUseProgramStages(getPipeline(), GL_VERTEX_SHADER_BIT, particles->getSeparableVertex());
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

    bindPipeline();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, cmTex.getFB(0));
    glViewport(0,0,cmTex.getSizeX(),cmTex.getSizeY());
    //glClear(GL_COLOR_BUFFER_BIT);


#ifdef GLAPP_REQUIRE_OGL45
    glBindTextureUnit(0, particles->getPaletteTexID());
#else
    USE_PROGRAM
    glActiveTexture(GL_TEXTURE0 + particles->getPaletteTexID());
    glBindTexture(GL_TEXTURE_2D,  particles->getPaletteTexID());

    setUniform1i(LOCpaletteTex, particles->getPaletteTexID());
#endif
    updateBufferData();

    clearFlagUpdate();

    theWnd->getVAO()->draw();

#if !defined(GLAPP_REQUIRE_OGL45)
    //ProgramObject::reset();
#endif

    glViewport(0,0, particles->getWidth(), particles->getHeight());
}

#if !defined(GLCHAOSP_NO_FXAA)
//
//  fxaa
//
////////////////////////////////////////////////////////////////////////////////
void fxaaClass::create() {
#ifdef GLAPP_NO_GLSL_PIPELINE
    useVertex(renderEngine->getCommonVShader());
    addShader(vertObj);
#else
    glUseProgramStages(getPipeline(), GL_VERTEX_SHADER_BIT, renderEngine->getSeparableVertex());
#endif
    useFragment();
    fragObj->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 1, SHADER_PATH "fxaaFrag.glsl");
    addShader(fragObj);

    link();

    removeAllShaders(true);

    _fxaaData    = getUniformLocation("fxaaData");
    _invScrnSize = getUniformLocation("invScrnSize");
    _u_colorTexture = getUniformLocation("u_colorTexture");

}

GLuint fxaaClass::render(GLuint texIn, bool useFB)
{
    bindPipeline();

#if !defined(GLCHAOSP_LIGHTVER)
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,  useFB || renderEngine->getMotionBlur()->Active() ? fbo.getFB(0) : 0);
#else
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,  0);
#endif

#ifdef GLAPP_REQUIRE_OGL45
    glBindTextureUnit(0, texIn);
#else
    USE_PROGRAM
    glActiveTexture(GL_TEXTURE0 + texIn);
    glBindTexture(GL_TEXTURE_2D,  texIn);

    setUniform1i(_u_colorTexture, texIn);
#endif

    if(renderEngine->checkFlagUpdate()) {
        setUniform2f(_invScrnSize, 1.f/fbo.getSizeX(), 1.f/fbo.getSizeY());    
        updateSettings();
    }

    theWnd->getVAO()->draw();

#if !defined(GLAPP_REQUIRE_OGL45)
    //ProgramObject::reset();
#endif


    return fbo.getTex(0);
}

#endif

//
//  postRenderingClass
//
////////////////////////////////////////////////////////////////////////////////
postRenderingClass::postRenderingClass(renderBaseClass *ptrRE) : renderEngine(ptrRE) 
{
    fbo.buildFBO(1, theApp->GetWidth(), theApp->GetHeight(), theApp->getFBOInternalPrecision());
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

void postRenderingClass::bindRender(particlesBaseClass *particle, GLuint fbIdx)
{
    particle->updateBufferData();
    mmFBO &renderFBO = particle->getRenderFBO();

    bindPipeline();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.getFB(0));

    const vec4 bkg(particle->backgroundColor());
    glClearBufferfv(GL_COLOR,  0, value_ptr(bkg));
   

#ifdef GLAPP_REQUIRE_OGL45
        glBindTextureUnit(6, renderEngine->getAO()->getFBO().getTex(0));
        glBindTextureUnit(7, renderEngine->getShadow()->getFBO().getDepth(0));

        GLuint subIDX = particle->getUData().lightModel + particlesBaseClass::lightMDL::modelOffset; 

        glBindTextureUnit( 5, renderFBO.getTex(fbIdx));
        glBindTextureUnit( 8, renderFBO.getTexMultiFB(fbIdx, 0));
        glBindTextureUnit(10, renderFBO.getDepth(fbIdx));

#else
        USE_PROGRAM
        glActiveTexture(GL_TEXTURE0 + renderEngine->getAO()->getFBO().getTex(0));
        glBindTexture(GL_TEXTURE_2D,  renderEngine->getAO()->getFBO().getTex(0));
        setUniform1i(getLocAOTex() ,  renderEngine->getAO()->getFBO().getTex(0));

        glActiveTexture(GL_TEXTURE0 +   renderEngine->getShadow()->getFBO().getDepth(0));
        glBindTexture(GL_TEXTURE_2D,    renderEngine->getShadow()->getFBO().getDepth(0));
        setUniform1i(getLocShadowTex(), renderEngine->getShadow()->getFBO().getDepth(0));

        GLuint subIDX = getSubIdx(particle->getUData().lightModel);

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

}

void postRenderingClass::render()
{

    theWnd->getVAO()->draw();
    CHECK_GL_ERROR()
}

void postRenderingClass::releaseRender()
{
#if !defined(GLAPP_REQUIRE_OGL45)
    //ProgramObject::reset();
#endif
}

//
//  ambientOcclusionClass
//
////////////////////////////////////////////////////////////////////////////////
ambientOcclusionClass::ambientOcclusionClass(renderBaseClass *ptrRE) : renderEngine(ptrRE) 
{
    fbo.buildFBO(1, theApp->GetWidth(), theApp->GetHeight(), theApp->getFBOInternalPrecision());

    // generate sample kernel
    // ----------------------
    std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); // generates random floats between 0.0 and 1.0
    std::default_random_engine generator;
    for(unsigned int i = 0; i < kernelSize; i++)  {
        vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
        sample = normalize(sample);
        sample *= randomFloats(generator);
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
        vec3 noise(randomFloats(generator), randomFloats(generator), 0.0f); // rotate around z-axis (in tangent space)
        ssaoNoise.push_back(noise);
    }
#ifdef GLAPP_REQUIRE_OGL45
    glCreateTextures(GL_TEXTURE_2D, 1, &ssaoKernelTex);
    glTextureStorage2D(ssaoKernelTex, 1, GL_RGB32F, kernelSize, 1);
    glTextureSubImage2D(ssaoKernelTex, 0, 0, 0, kernelSize, 1, GL_RGB, GL_FLOAT, ssaoKernel.data());
    glTextureParameteri(ssaoKernelTex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(ssaoKernelTex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(ssaoKernelTex, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(ssaoKernelTex, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glCreateTextures(GL_TEXTURE_2D, 1, &noiseTexture);
    glTextureStorage2D(noiseTexture, 1, GL_RGB32F, 4, 4);
    glTextureSubImage2D(noiseTexture, 0, 0, 0, 4, 4, GL_RGB, GL_FLOAT, ssaoNoise.data());
    glTextureParameteri(noiseTexture, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(noiseTexture, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(noiseTexture, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(noiseTexture, GL_TEXTURE_WRAP_T, GL_REPEAT);
#else
    //GLuint precision = theApp->useLowPrecision() ? GL_RGB16F : GL_RGB32F;
    GLuint precision = theApp->useLowPrecision() ? GL_RGB16F : GL_RGB32F;
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
    locPrevData = getUniformLocation("prevData");
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

void ambientOcclusionClass::bindRender(particlesBaseClass *particle, GLuint fbIdx)
{
    particle->updateBufferData();
    mmFBO &renderFBO = particle->getRenderFBO();

    bindPipeline();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.getFB(0));

    const vec4 bkg(particle->backgroundColor());
    glClearBufferfv(GL_COLOR,  0, value_ptr(bkg));

#ifdef GLAPP_REQUIRE_OGL45
    glBindTextureUnit( 6, noiseTexture);
    glBindTextureUnit( 7, ssaoKernelTex);

    glBindTextureUnit( 5, renderFBO.getTex(fbIdx));
    glBindTextureUnit(10, renderFBO.getDepth(fbIdx));


#else
    USE_PROGRAM
    glActiveTexture(GL_TEXTURE0 + noiseTexture);
    glBindTexture(GL_TEXTURE_2D,  noiseTexture);

    glActiveTexture(GL_TEXTURE0 + ssaoKernelTex);
    glBindTexture(GL_TEXTURE_2D,  ssaoKernelTex);

    glActiveTexture(GL_TEXTURE0 + renderFBO.getTex(fbIdx));
    glBindTexture(GL_TEXTURE_2D,  renderFBO.getTex(fbIdx));
    setUniform1i(locPrevData,     renderFBO.getTex(fbIdx));

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
    //ProgramObject::reset();
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
    fbo.buildOnlyFBO(1, theApp->GetWidth()*detail, theApp->GetHeight()*detail, GL_RGBA32F); 
    fbo.attachDB(mmFBO::depthTexture,GL_LINEAR,GL_CLAMP_TO_BORDER);
#else
    fbo.buildOnlyFBO(1, theApp->GetWidth(), theApp->GetHeight(), GL_RGBA32F); 
    fbo.attachDB(mmFBO::depthTexture,GL_NEAREST,GL_CLAMP_TO_EDGE);  
#endif
    //create();
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
    bindPipeline();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.getFB(0));

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);

    const GLfloat f=1.0f;
    glClearBufferfv(GL_DEPTH , 0, &f);


#if !defined(GLAPP_REQUIRE_OGL45)
    USE_PROGRAM
    uniformBlocksClass::bindIndex(getProgram(), "_particlesData", uniformBlocksClass::bindIdx);
    uniformBlocksClass::bindIndex(getProgram(), "_clippingPlanes", GLuint(renderBaseClass::bind::planesIDX));
    renderEngine->getTMat()->blockBinding(getProgram());
#endif
}

void shadowClass::render()
{
}

void shadowClass::releaseRender()
{
}

tfSettinsClass::tfCommonsStruct tfSettinsClass::tfCommons;

void tfSettinsClass::setViewport(int w, int h) {
    float szX = float(w)*getPIPzoom()*.5+.5, szY = float(h)*getPIPzoom()*.5+.5;
    w++; h++;
    switch(getPIPposition()) {
        case pip::lTop:
            setViewportSize(ivec4(0, h-szY, szX, szY));
            break;
        case pip::lBottom:
            setViewportSize(ivec4(0,  0, szX, szY));
            break;
        case pip::rTop:
            setViewportSize(ivec4(w-szX,h-szY, szX, szY));
            break;
        case pip::rBottom:
            setViewportSize(ivec4(w-szX,0 , szX, szY));
            break;
        default:
        case pip::noPIP:
            setViewportSize(ivec4(0,0, w, h));
            break;
    }
    glViewport(getViewportSize().x, getViewportSize().y, getViewportSize().z, getViewportSize().w);
}

