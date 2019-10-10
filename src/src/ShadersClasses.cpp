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
#include <fastRandom.h>

#include "glWindow.h"

#include "attractorsBase.h"
#include "ParticlesUtils.h"


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
    setSize(6.f, 0.001);

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
    //palette.buildTex(colorMaps.getRGB_pf3(1), 256);
    //selectColorMap(0);
    useVertex(); useFragment();

    getVertex  ()->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 2, SHADER_PATH "ParticlesVert.glsl", SHADER_PATH "PointSpriteVert.glsl");
    getFragment()->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 3, SHADER_PATH "lightModelsFrag.glsl", SHADER_PATH "ParticlesFrag.glsl", SHADER_PATH "PointSpriteFragLight.glsl");
    //getVertex()->Load("", 1, SHADER_PATH "allParticles.vert");
    //getFragment()->Load("", 1, SHADER_PATH "allParticles.frag");
    // The vertex and fragment are added to the program object
    addVertex();
    addFragment();

    link();

    removeAllShaders(true);

#ifdef GLAPP_REQUIRE_OGL45
    uniformBlocksClass::create(GLuint(sizeof(uParticlesData)), (void *) &uData);
    getCommonLocals();
#else
    USE_PROGRAM

    uniformBlocksClass::create(GLuint(sizeof(uParticlesData)), (void *) &uData, getProgram(), "_particlesData");

    getTMat()->blockBinding(getProgram());

    getCommonLocals();

    ProgramObject::reset();
#endif

}

//
//  particlesBase 
//
////////////////////////////////////////////////////////////////////////////////
void particlesBaseClass::clearScreenBuffers()
{

    if(depthBuffActive) {
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
        const GLfloat f=1.0f;
        glClearBufferfv(GL_DEPTH , 0, &f);
        //glClearBufferfi(GL_DEPTH_STENCIL , 0, 1.f, 0);
    }

#if !defined(GLCHAOSP_LIGHTVER)
    if(showAxes() == noShowAxes) glClear(GL_COLOR_BUFFER_BIT); //glClearBufferfv(GL_COLOR,  GL_DRAW_BUFFER, value_ptr(vec4(0.0f)));
    if(blendActive || showAxes()) {
#else
    glClearBufferfv(GL_COLOR, 0, value_ptr(vec4(0.0f)));
    if(blendActive) {
#endif
        glEnable(GL_BLEND);
        //glBlendFuncSeparate(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ZERO,GL_ONE_MINUS_SRC_ALPHA);
        glBlendFunc(getSrcBlend(), getDstBlend());
    }

    //glEnable(GL_CLIP_DISTANCE0);
    //glEnable(GL_CLIP_PLANE0);
}

void particlesBaseClass::restoreGLstate()
{
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    //glDisable(GL_CLIP_DISTANCE0);
    //glDisable(GL_CLIP_PLANE0);

    //glDisable(GL_MULTISAMPLE);
}

GLuint particlesBaseClass::render(GLuint fbIdx, emitterBaseClass *emitter) 
{

    auto updateCommons = [&] () {
        getUData().scrnRes = vec2(getRenderFBO().getSizeX(), getRenderFBO().getSizeY());
        getUData().invScrnRes = 1.f/getUData().scrnRes;
        getUData().ySizeRatio = theApp->isParticlesSizeConstant() ? 1.0 : float(getRenderFBO().getSizeY()/1024.0);
        getUData().ptSizeRatio = 1.0/(length(getUData().scrnRes) / getUData().scrnRes.x);
        getUData().velocity = getCMSettings()->getVelIntensity();
        getUData().lightDir = vec3(getTMat()->tM.vMatrix * vec4(getLightDir(), 1.0));
    };

    const bool isAO_RD_SHDW = ( useAO() ||  postRenderingActive() || useShadow());
    const bool isAO_SHDW = ( useAO() || useShadow());
    const bool isSolid = ( getDepthState() ||  getLightState());
    const bool isShadow = ( useShadow() && isSolid);

    auto selectSubroutines = [&]() {
        GLuint subIDX[2];        
        const int idxPixelColor  = (getBlendState() && !isSolid)       ? pixColIDX::pixBlendig :
                                   getBlendState() || !isAO_RD_SHDW    ? pixColIDX::pixDirect  :
                                   isAO_SHDW && !postRenderingActive() ? pixColIDX::pixAO      :
                                                                         pixColIDX::pixDR      ;
#ifdef GLAPP_REQUIRE_OGL45
        subIDX[locSubPixelColor] = idxPixelColor;
        subIDX[locSubLightModel] = uData.lightModel + lightMDL::modelOffset;

        GLuint subVtxIdx = idxViewOBJ && plyObjGetColor ? particlesViewColor::packedRGB : particlesViewColor::paletteIndex;

        glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, GLsizei(2), subIDX);
        glUniformSubroutinesuiv(GL_VERTEX_SHADER, GLsizei(1), &subVtxIdx);
#else
    uData.renderType = idxPixelColor;
    #if !defined(GLCHAOSP_LIGHTVER)
        #if !defined(__APPLE__)
            subIDX[locSubPixelColor] = idxSubPixelColor[idxPixelColor-pixColIDX::pixOffset];
            subIDX[locSubLightModel] = idxSubLightModel[uData.lightModel];

            glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, GLsizei(2), subIDX);
        #endif
        glUniformSubroutinesuiv(GL_VERTEX_SHADER, GLsizei(1), idxViewOBJ ? &idxSubOBJ : &idxSubVEL);
    #endif
#endif
    };

    if(checkFlagUpdate()) updateCommons();

    getUData().zNear = getTMat()->getPerspNear();
    getUData().zFar  = getTMat()->getPerspFar();

// Shadow pass
/////////////////////////////////////////////
#if !defined(GLCHAOSP_LIGHTVER) || defined(GLCHAOSP_LIGHTVER_EXPERIMENTAL) 
    if(isShadow && !getBlendState()) {
        if(autoLightDist()) {
            vec3 vL(normalize(getLightDir()));
            float dist = getTMat()->getPOV().z - getTMat()->getTrackball().getDollyPosition().z;
            setLightDir(vL * (dist*4.f + dist*.1f));
        }

        //tMat.setLightOrtho();
        getShadow()->bindRender();

        tMat.setLightView(getLightDir());
        tMat.tM.mvLightM = tMat.tM.mvLightM * tMat.tM.mMatrix;


        tMat.updateBufferData();
        updateBufferData();

        //render
        emitter->renderEvents();

        getShadow()->releaseRender();
        //tMat.setPerspective();
    }
#endif

// Normal Render
/////////////////////////////////////////////
    bindPipeline();

    GLuint returnedTex = getRenderFBO().getTex(fbIdx);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, getRenderFBO().getFB(fbIdx));
    clearScreenBuffers();

    if(checkFlagUpdate()) updateCommons();

    getUData().zNear = getTMat()->getPerspNear();
    getUData().zFar  = getTMat()->getPerspFar();
//    getUData().zFar = (getTMat()->getPerspFar() + (getTMat()->getPOV().z - getTMat()->getTrackball().getDollyPosition().z)) * 2.0;

    tMat.updateBufferData();
    getUData().pass = (isShadow ? 4:0) | (postRenderingActive() ? 2:0) | (useAO() ? 1:0);
    getUData().POV = vec4(getTMat()->getTrackball().getPosition(), 0.0);

    updateBufferData();
       
    const GLuint texID = dotTex.getTexID();

#ifdef GLAPP_REQUIRE_OGL45
    glBindTextureUnit(0, colorMap->getModfTex());
    glBindTextureUnit(1, texID);
#else
    glActiveTexture(GL_TEXTURE0+colorMap->getModfTex());
    glBindTexture(GL_TEXTURE_2D,colorMap->getModfTex());

    glActiveTexture(GL_TEXTURE0+texID);
    glBindTexture(GL_TEXTURE_2D,texID);

    USE_PROGRAM
    setUniform1i(locDotsTex,texID);

    updatePalTex();
#endif
    selectSubroutines();

    //transformInterlieve->renderFeedbackData();
    emitter->renderEvents();

#if !defined(GLAPP_REQUIRE_OGL45)
    //ProgramObject::reset();
#endif

// DualRendering
/////////////////////////////////////////////
#if !defined(GLCHAOSP_LIGHTVER) || defined(GLCHAOSP_LIGHTVER_EXPERIMENTAL) 
#if defined(GLCHAOSP_LIGHTVER_EXPERIMENTAL) 
    if(!getBlendState() && isAO_RD_SHDW)  {
#else
    if(!getBlendState() && isAO_RD_SHDW && !showAxes())  {
#endif
        tMat.setLightView(getLightDir());
        mat4 m(1.f);
        m = translate(m,tMat.getTrackball().getPosition());
        m = translate(m,tMat.getPOV());
        tMat.tM.mvLightM = tMat.tM.mvLightM * m;

        getUData().halfTanFOV = tanf(getTMat()->getPerspAngleRad()*.5);

// AO frag
/////////////////////////////////////////////
        if(useAO()) {
            getAO()->bindRender();

            tMat.updateBufferData();
            updateBufferData();

#ifdef GLAPP_REQUIRE_OGL45
            glBindTextureUnit(5, getRenderFBO().getTex(fbIdx));
#else
            glActiveTexture(GL_TEXTURE0 + getRenderFBO().getTex(fbIdx));
            glBindTexture(GL_TEXTURE_2D,  getRenderFBO().getTex(fbIdx));
            getAO()->setUniform1i(getAO()->getLocPrevData(), getRenderFBO().getTex(fbIdx));
#endif

            getAO()->render();
            getAO()->releaseRender();
            //returnedTex = getAO()->getFBO().getTex(0);
        }

// PostRendering frag
/////////////////////////////////////////////
            getPostRendering()->bindRender();

            tMat.updateBufferData();
            updateBufferData();

#ifdef GLAPP_REQUIRE_OGL45

            GLuint subIDX = uData.lightModel + lightMDL::modelOffset; 

            glBindTextureUnit(5, getRenderFBO().getTex(fbIdx));
#else
            GLuint subIDX = getPostRendering()->getSubIdx(uData.lightModel);

            glActiveTexture(GL_TEXTURE0 + getRenderFBO().getTex(fbIdx));
            glBindTexture(GL_TEXTURE_2D,  getRenderFBO().getTex(fbIdx));
            getPostRendering()->setUniform1i(getPostRendering()->getLocPrevData(), getRenderFBO().getTex(fbIdx));

#endif
#if !defined(__APPLE__) && !defined(GLCHAOSP_LIGHTVER_EXPERIMENTAL)
            glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, GLsizei(1), &subIDX);
#endif

            getPostRendering()->render();
            getPostRendering()->releaseRender();
            returnedTex = getPostRendering()->getFBO().getTex(0);
        //}
    }
#endif

    restoreGLstate();
    return returnedTex;
}

#if !defined(GLCHAOSP_LIGHTVER)

//
//  Billboard
//
////////////////////////////////////////////////////////////////////////////////
shaderBillboardClass::shaderBillboardClass() 
{ 
    setSize(4., 0.0001);

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

    //selectColorMap(1); //pal_magma_data
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


#ifdef GLAPP_REQUIRE_OGL45
    uniformBlocksClass::create(GLuint(sizeof(uParticlesData)), (void *) &uData);
    getCommonLocals();
#else
    bindPipeline();
    USE_PROGRAM

    uniformBlocksClass::create(GLuint(sizeof(uParticlesData)), (void *) &uData, getProgram(), "_particlesData");
    getTMat()->blockBinding(getProgram());


    getCommonLocals();

    //ProgramObject::reset();
#endif

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
    CHECK_GL_ERROR();
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

    useFragment();
    useVertex(renderEngine->getCommonVShader());

    //vertObj->Load( theApp->get_glslVer().c_str()                         , 1, SHADER_PATH "mmFBO_all_vert.glsl");
    fragObj->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 1, SHADER_PATH "MergedRenderingFrag.glsl");

    addShader(vertObj);
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
    useVertex(renderEngine->getCommonVShader());
    useFragment();

    //vertObj->Load( theApp->get_glslVer().c_str()                         , 1, SHADER_PATH "mmFBO_all_vert.glsl");
    fragObj->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 1, SHADER_PATH "MotionBlurFS.glsl");

    addShader(vertObj);
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


//
//  transformedEmitter
//
////////////////////////////////////////////////////////////////////////////////
bool transformFeedbackInterleaved::FeedbackActive = false;

void transformedEmitterClass::renderOfflineFeedback(AttractorBase *att)
{
    bindPipeline();
    USE_PROGRAM

    tfbs[activeBuffer]->Begin();

    //const GLint index = program->getAttribLocation(p->name);


    int emiss = getSizeStepBuffer();
    while(isEmitterOn() && emiss-- && (getParticlesCount()<getSizeCircularBuffer())) {
        attractorsList.get()->Step();

        vec3 oldPosAttractor(attractorsList.get()->getPrevious());
        vec3 newPosAttractor(attractorsList.get()->getCurrent());

        //glGetUniformfv(progParticles->getHandle(),locPosProgress, posProgress._array);
        //while(fStep >= EmissionTime

        InsertVbo->getBuffer()[0] = oldPosAttractor.x;
        InsertVbo->getBuffer()[1] = oldPosAttractor.y;
        InsertVbo->getBuffer()[2] = oldPosAttractor.z;
        InsertVbo->getBuffer()[3] = 0.0;
        //(*InsertVbo)[3] =

        InsertVbo->getBuffer()[4] = newPosAttractor.x;
        InsertVbo->getBuffer()[5] = newPosAttractor.y;
        InsertVbo->getBuffer()[6] = newPosAttractor.z;
        InsertVbo->getBuffer()[7] = 0.0; //color control

        
        InsertVbo->uploadData(1);

        // OCCHIO!!!
        // Controllare, era :
        // InsertVbo->drawVtx();  -->  glDrawArrays(primitive,0,numVertex);
        // ma forse c'e' un errore di ridondanza "numVertex"
        InsertVbo->drawRange(0,InsertVbo->getAttribPerVtx());
        

    }
    vbos[activeBuffer^1]->drawRange(0, Sizes[activeBuffer^1]<getSizeCircularBuffer() ? Sizes[activeBuffer^1] : getSizeCircularBuffer());

    Sizes[activeBuffer] = tfbs[activeBuffer]->End();
//    setParticlesCount(Sizes[activeBuffer]);


    ProgramObject::reset();


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
    PB(GL_SRC_COLOR 	           ,"Src_Color"               )
    PB(GL_ONE_MINUS_SRC_COLOR 	   ,"One_Minus_Src_Color"     )
    PB(GL_DST_COLOR 	           ,"Dst_Color"               )
    PB(GL_ONE_MINUS_DST_COLOR      ,"One_Minus_Dst_Color "    )
    PB(GL_SRC_ALPHA                ,"Src_Alpha"               )
    PB(GL_ONE_MINUS_SRC_ALPHA      ,"One_Minus_Src_Alpha"     )
    PB(GL_DST_ALPHA                ,"Dst_Alpha"               )
    PB(GL_ONE_MINUS_DST_ALPHA      ,"One_Minus_Dst_Alpha"     )
    PB(GL_CONSTANT_COLOR 	       ,"Constant_Color	"         )
    PB(GL_ONE_MINUS_CONSTANT_COLOR ,"One_Minus_Constant_Color")
    PB(GL_CONSTANT_ALPHA 	       ,"Constant_Alpha"          )
    PB(GL_ONE_MINUS_CONSTANT_ALPHA ,"One_Minus_Constant_Alpha")
    PB(GL_SRC_ALPHA_SATURATE       ,"Src_Alpha_Saturate"      )
#if !defined(GLCHAOSP_LIGHTVER)
    PB(GL_SRC1_COLOR               ,"Src1_Color"              )
    PB(GL_ONE_MINUS_SRC1_COLOR     ,"One_Minus_Src1_Color"    )
    PB(GL_SRC1_ALPHA               ,"Src1_Alpha"              )
    PB(GL_ONE_MINUS_SRC1_ALPHA     ,"One_Minus_Src1_Alpha"    )
#endif
#undef PB

    setRenderMode(RENDER_USE_POINTS);

    commonVShader.Load( (theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 1, SHADER_PATH "mmFBO_all_vert.glsl");

#if defined(GLCHAOSP_LIGHTVER) && !defined(GLCHAOSP_LIGHTVER_EXPERIMENTAL)
    renderFBO.buildFBO(1, theApp->GetWidth(), theApp->GetHeight(), GL_RGBA32F); 
#else
    renderFBO.buildFBO(1, theApp->GetWidth(), theApp->GetHeight(), GL_RGBA32F);
#endif
    renderFBO.attachDB(mmFBO::depthBuiltIn);


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

    // create(); // Now called from particlesBaseClass for Texture WebGL issue
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
    whichRenderMode=which; setFlagUpdate(); 
}

VertexShader* commonVShader = nullptr;

//
//  BlurBase (Glow)
//
////////////////////////////////////////////////////////////////////////////////
void BlurBaseClass::create()    {


        useVertex(renderEngine->getCommonVShader());
        useFragment();

        //vertObj->Load( theApp->get_glslVer().c_str()                         , 1, SHADER_PATH "mmFBO_all_vert.glsl");
        fragObj->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 2, SHADER_PATH "colorSpaces.glsl", SHADER_PATH "RadialBlur2PassFrag.glsl");

        addShader(vertObj);
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
        idxSubGlowType[idxSubroutine_ByPass            ]  = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "byPass"                  );
        idxSubGlowType[idxSubroutine_BlurCommonPass1   ]  = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "radialPass1"             );
        idxSubGlowType[idxSubroutine_BlurGaussPass2    ]  = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "radialPass2"             );
        idxSubGlowType[idxSubroutine_BlurThresholdPass2]  = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "radialPass2withBilateral");
        idxSubGlowType[idxSubroutine_Bilateral         ]  = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "bilateralSmooth"         );
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

    //glBindTextureUnit(3, theWnd->getParticlesSystem()->getRenderFBO().getDepth(0));
    //glBindTextureUnit(4, theWnd->getParticlesSystem()->getRenderFBO().getTexMultiFB(0)); //vtx
    //glBindTextureUnit(5, theWnd->getParticlesSystem()->getRenderFBO().getTexMultiFB(1)); //Norm

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

    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, GLsizei(1), &idxSubGlowType[subIndex]);
#endif
    updateData(subIndex);

#endif

    theWnd->getVAO()->draw();
#if !defined(GLAPP_REQUIRE_OGL45)
    //ProgramObject::reset();
#endif

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
    useVertex(particles->getCommonVShader());
    useFragment();

    //vertObj->Load( theApp->get_glslVer().c_str()                         , 1, SHADER_PATH "mmFBO_all_vert.glsl");
    fragObj->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 2, SHADER_PATH "colorSpaces.glsl", SHADER_PATH "cmTexturedFrag.glsl");


    addShader(vertObj);
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
    useVertex(renderEngine->getCommonVShader());
    useFragment();

    //vertObj->Load( theApp->get_glslVer().c_str()                         , 1, SHADER_PATH "mmFBO_all_vert.glsl");
    fragObj->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 1, SHADER_PATH "fxaaFrag.glsl");
    addShader(vertObj);
    addShader(fragObj);

    link();

    removeAllShaders(true);

    _fxaaData    = getUniformLocation("fxaaData");
    _invScrnSize = getUniformLocation("invScrnSize");
    _u_colorTexture = getUniformLocation("u_colorTexture");

}

GLuint fxaaClass::render(GLuint texIn)
{
    bindPipeline();

#if !defined(GLCHAOSP_LIGHTVER)
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,  renderEngine->getMotionBlur()->Active() ? fbo.getFB(0) : 0);
#else
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,  0);
#endif
    //glClearBufferfv(GL_COLOR,  0, value_ptr(vec4(0.0f)));
    //glClear(GL_COLOR_BUFFER_BIT);

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


    //create();
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

#if !defined(GLCHAOSP_LIGHTVER)
    locSubLightModel = glGetSubroutineUniformLocation(getProgram(), GL_FRAGMENT_SHADER, "lightModel");
#endif

#if !defined(GLAPP_REQUIRE_OGL45)
    USE_PROGRAM
    locAOTex = getUniformLocation("aoTex");
    locShadowTex = getUniformLocation("shadowTex");
    locPrevData = getUniformLocation("prevData");
    uniformBlocksClass::bindIndex(getProgram(), "_particlesData");
    renderEngine->getTMat()->blockBinding(getProgram());

    setUniform1i(getLocAOTex(), renderEngine->getAO()->getFBO().getTex(0));

// for Chrome76 message: "texture is not renderable" if only zBuffer... need ColorBuffer 
// FireFox68 Works fine also only with zBuffer w/o ColorBuffer
////////////////////////////////////////////////////////////////////////////////
#if !defined(GLCHAOSP_LIGHTVER)
    setUniform1i(getLocShadowTex(), renderEngine->getShadow()->getFBO().getDepth(0));
#else
    setUniform1i(getLocShadowTex(), renderEngine->getShadow()->getFBO().getTex(0));
#endif



#if !defined(GLCHAOSP_LIGHTVER)
    idxSubLightModel[0] = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "specularPhong");
    idxSubLightModel[1] = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "specularBlinnPhong");
    idxSubLightModel[2] = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "specularGGX");
#endif

#endif

}

void postRenderingClass::bindRender()
{
    bindPipeline();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.getFB(0));
    glClearBufferfv(GL_COLOR,  0, value_ptr(vec4(0.0f)));

    USE_PROGRAM

#ifdef GLAPP_REQUIRE_OGL45
        glBindTextureUnit(6, renderEngine->getAO()->getFBO().getTex(0));
        glBindTextureUnit(7, renderEngine->getShadow()->getFBO().getDepth(0));
#else
        glActiveTexture(GL_TEXTURE0 + renderEngine->getAO()->getFBO().getTex(0));
        glBindTexture(GL_TEXTURE_2D,  renderEngine->getAO()->getFBO().getTex(0));

// for Chrome76 message: "texture is not renderable" if only zBuffer... need ColorBuffer 
// FireFox68 Works fine also only with zBuffer w/o ColorBuffer
////////////////////////////////////////////////////////////////////////////////
    #if !defined(GLCHAOSP_LIGHTVER)
        glActiveTexture(GL_TEXTURE0 + renderEngine->getShadow()->getFBO().getDepth(0));
        glBindTexture(GL_TEXTURE_2D,  renderEngine->getShadow()->getFBO().getDepth(0));
    #else 
        glActiveTexture(GL_TEXTURE0 + renderEngine->getShadow()->getFBO().getTex(0));
        glBindTexture(GL_TEXTURE_2D,  renderEngine->getShadow()->getFBO().getTex(0));
    #endif
#endif

}

void postRenderingClass::render()
{

    theWnd->getVAO()->draw();
    CHECK_GL_ERROR();
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
    for(unsigned int i = 0; i < kernelSize; ++i)  {
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
    glGenTextures(1, &ssaoKernelTex);
    glBindTexture(GL_TEXTURE_2D, ssaoKernelTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, kernelSize, 1, 0, GL_RGB, GL_FLOAT, ssaoKernel.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, 4, 4, 0, GL_RGB, GL_FLOAT, ssaoNoise.data());
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
    locKernelTexture = getUniformLocation("ssaoSample");
    bindIDX = uniformBlocksClass::bindIndex(getProgram(), "_particlesData");
    renderEngine->getTMat()->blockBinding(getProgram());

    setUniform1i(locKernelTexture, ssaoKernelTex);
    setUniform1i(locNoiseTexture, noiseTexture);
#endif
    //setUniform3fv(getUniformLocation("ssaoSamples"), kernelSize, (const GLfloat*)ssaoKernel.data());
}

void ambientOcclusionClass::bindRender()
{
    bindPipeline();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.getFB(0));
    glClearBufferfv(GL_COLOR,  0, value_ptr(vec4(0.0f)));

#ifdef GLAPP_REQUIRE_OGL45
    glBindTextureUnit(6, noiseTexture);
    glBindTextureUnit(7, ssaoKernelTex);

#else
    USE_PROGRAM
    glActiveTexture(GL_TEXTURE0 + noiseTexture);
    glBindTexture(GL_TEXTURE_2D,  noiseTexture);

    glActiveTexture(GL_TEXTURE0 + ssaoKernelTex);
    glBindTexture(GL_TEXTURE_2D,  ssaoKernelTex);
    
#endif
}

void ambientOcclusionClass::render()
{
    theWnd->getVAO()->draw();
    CHECK_GL_ERROR();
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
// for Chrome76 message: "texture is not renderable" if only zBuffer... need ColorBuffer 
// FireFox68 Works fine also only with zBuffer w/o ColorBuffer
////////////////////////////////////////////////////////////////////////////////
#if !defined(GLCHAOSP_LIGHTVER)
    fbo.buildOnlyFBO(1, theApp->GetWidth(), theApp->GetHeight(), GL_RGBA32F); //
    fbo.attachDB(mmFBO::depthTexture,GL_LINEAR,GL_CLAMP_TO_BORDER);
#else
    fbo.buildFBO(1, theApp->GetWidth(), theApp->GetHeight(), GL_RGBA32F);
    fbo.attachDB(mmFBO::depthBuiltIn, GL_NEAREST, GL_CLAMP_TO_EDGE);
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
    uniformBlocksClass::bindIndex(getProgram(), "_particlesData");
    renderEngine->getTMat()->blockBinding(getProgram());
#endif
}

void shadowClass::render()
{
}

void shadowClass::releaseRender()
{
}

