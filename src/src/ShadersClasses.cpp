////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2018 Michele Morrone
//  All rights reserved.
//
//  mailto:me@michelemorrone.eu
//  mailto:brutpitt@gmail.com
//  
//  https://github.com/BrutPitt
//
//  https://michelemorrone.eu
//  https://BrutPitt.com
//
//  This software is distributed under the terms of the BSD 2-Clause license:
//  
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are met:
//      * Redistributions of source code must retain the above copyright
//        notice, this list of conditions and the following disclaimer.
//      * Redistributions in binary form must reproduce the above copyright
//        notice, this list of conditions and the following disclaimer in the
//        documentation and/or other materials provided with the distribution.
//   
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
//  ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
//  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
//  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
////////////////////////////////////////////////////////////////////////////////
#include <glm/glm.hpp>

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
    selectColorMap(0);
    useVertex(); useFragment();

	getVertex  ()->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 2, SHADER_PATH "ParticlesVert.glsl", SHADER_PATH "PointSpriteVert.glsl");
	getFragment()->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 2, SHADER_PATH "ParticlesFrag.glsl", SHADER_PATH "PointSpriteFragLight.glsl");
	// The vertex and fragment are added to the program object
    addVertex();
    addFragment();

	link();
    deleteAll();


#ifdef GLAPP_REQUIRE_OGL45
        uniformBlocksClass::create(GLuint(sizeof(uParticlesData)), (void *) &uData);
#else
    bindPipeline();
    useProgram();

    uniformBlocksClass::create(GLuint(sizeof(uParticlesData)), (void *) &uData, getProgram(), "_particlesData");

    getTMat()->blockBinding(getProgram());

    getCommonLocals();

    #if !defined(GLCHAOSP_LIGHTVER)
        idxSubLightOn = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "pixelColorLight");
        idxSubLightOff = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "pixelColorOnly");
        idxSubOBJ = glGetSubroutineIndex(getProgram(),GL_VERTEX_SHADER, "objColor"); 
        idxSubVEL = glGetSubroutineIndex(getProgram(),GL_VERTEX_SHADER, "velColor"); 
    #endif

    ProgramObject::reset();
#endif

}

//
//  particlesBase 
//
////////////////////////////////////////////////////////////////////////////////
void particlesBaseClass::render(GLuint fbOut, emitterBaseClass *emitter) {

    bindPipeline();

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbOut);
    //glBindFramebuffer(GL_FRAMEBUFFER, fbOut);

    //glEnable(GL_MULTISAMPLE);

    if(depthBuffActive) {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc( GL_LESS );
        glDepthMask(GL_TRUE);
#if !defined(GLCHAOSP_LIGHTVER)
        glDepthRange(-1.0, 1.0);
#endif
        //glClearDepth(1.0f);
        GLfloat f=1.0f;
        glClearBufferfv(GL_DEPTH, 0, &f);
    }

    const GLuint texID = dotTex.getTexID();
#if !defined(GLCHAOSP_LIGHTVER)
    if(showAxes() == noShowAxes) glClearBufferfv(GL_COLOR, 0, glm::value_ptr(glm::vec4(0.0f)));
    if(blendActive || showAxes()) {
#else
    getUData().lightActive = lightStateIDX; 
    glClearBufferfv(GL_COLOR, 0, glm::value_ptr(glm::vec4(0.0f)));
    if(blendActive) {
#endif
        glEnable(GL_BLEND);
        //glBlendFuncSeparate(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA,GL_ZERO,GL_ONE_MINUS_SRC_ALPHA);
        glBlendFunc(getSrcBlend(), getDstBlend());
    }

    //getUData().velocity = getCMSettings()->getVelIntensity();
    if(checkFlagUpdate()) updateCommonUniforms();
    //const float dist = getTMat()->getPerspFar()-getTMat()->getPerspNear();
    //const float newFar = getTMat()->getPerspFar() + (getTMat()->getPOV().z - getTMat()->getTrackball().getDollyPosition().z) * 2.0;
    //getUData().zNear = -(newFar+getTMat()->getPerspNear())/dist;
    //getUData().zFar = -2.f * newFar * getTMat()->getPerspNear() / dist;

    getUData().zNear = getTMat()->getPerspNear();
    getUData().zFar = (getTMat()->getPerspFar() + (getTMat()->getPOV().z - getTMat()->getTrackball().getDollyPosition().z)) * 2.0;

    //getUData().zFar = .5f/(getTMat()->getPOV().z-getTMat()->getTrackball().getDollyPosition().z); //1((/POV.z-Dolly.z)*2)

    //getUData().zFar = getTMat()->getPerspFar()+getTMat()->getPerspNear()
    tMat.updateBufferData();
    updateBufferData();
       
#ifdef GLAPP_REQUIRE_OGL45
    glBindTextureUnit(0, colorMap->getModfTex());
    glBindTextureUnit(1, texID);
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, GLsizei(1), &lightStateIDX);
    glUniformSubroutinesuiv(GL_VERTEX_SHADER, GLsizei(1), &idxViewOBJ);
#else
    glActiveTexture(GL_TEXTURE0+colorMap->getModfTex());
    glBindTexture(GL_TEXTURE_2D,colorMap->getModfTex());

    glActiveTexture(GL_TEXTURE0+texID);
    glBindTexture(GL_TEXTURE_2D,texID);

    useProgram();
    setUniform1i(locDotsTex,texID);

#if !defined(GLCHAOSP_LIGHTVER)
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, GLsizei(1), lightStateIDX==1 ? &idxSubLightOn : &idxSubLightOff);
    glUniformSubroutinesuiv(GL_VERTEX_SHADER, GLsizei(1), idxViewOBJ==1 ? &idxSubOBJ : &idxSubVEL);
#endif
    updatePalTex();
#endif


    //transformInterlieve->renderFeedbackData();
    emitter->renderEvents();

    CHECK_GL_ERROR();

#if !defined(GLAPP_REQUIRE_OGL45)
    ProgramObject::reset();
#endif

    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    
    //glDisable(GL_MULTISAMPLE);

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

    selectColorMap(1); //pal_magma_data
    useAll(); 

	getVertex  ()->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 2, SHADER_PATH "ParticlesVert.glsl", SHADER_PATH "BillboardVert.glsl");
    getGeometry()->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 1, SHADER_PATH "BillboardGeom.glsl");
	getFragment()->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 2, SHADER_PATH "ParticlesFrag.glsl", SHADER_PATH "BillboardFrag.glsl");

	// The vertex and fragment are added to the program object
	addVertex();
    addGeometry();
  	addFragment();

	link();

#ifdef GLAPP_REQUIRE_OGL45
        uniformBlocksClass::create(GLuint(sizeof(uParticlesData)), (void *) &uData);
#else
    bindPipeline();
    useProgram();

    uniformBlocksClass::create(GLuint(sizeof(uParticlesData)), (void *) &uData, getProgram(), "_particlesData");
    getTMat()->blockBinding(getProgram());


    getCommonLocals();

    idxSubLightOn = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "pixelColorLight");
    idxSubLightOff = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "pixelColorOnly");
    idxSubOBJ = glGetSubroutineIndex(getProgram(),GL_VERTEX_SHADER, "objColor"); 
    idxSubVEL = glGetSubroutineIndex(getProgram(),GL_VERTEX_SHADER, "velColor"); 

    ProgramObject::reset();
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
    glClear(GL_COLOR_BUFFER_BIT);
    CHECK_GL_ERROR();
#ifdef GLAPP_REQUIRE_OGL45
    glBindTextureUnit(0, texA);
    glBindTextureUnit(1, texB);
#else
    glActiveTexture(GL_TEXTURE0 + texA);
    glBindTexture(GL_TEXTURE_2D,  texA);

    glActiveTexture(GL_TEXTURE0 + texB);
    glBindTexture(GL_TEXTURE_2D,  texB);
    useProgram();
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
    useVertex();

    vertObj->Load( theApp->get_glslVer().c_str()                         , 1, SHADER_PATH "mmFBO_all_vert.glsl");
    fragObj->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 1, SHADER_PATH "MergedRenderingFrag.glsl");

    addShader(vertObj);
    addShader(fragObj);

	link();
 
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
    useVertex();
    useFragment();

    vertObj->Load( theApp->get_glslVer().c_str()                         , 1, SHADER_PATH "mmFBO_all_vert.glsl");
    fragObj->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 1, SHADER_PATH "MotionBlurFS.glsl");

    addShader(vertObj);
    addShader(fragObj);        

	link();


#ifndef GLAPP_REQUIRE_OGL45
    bindPipeline();
    useProgram();
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
    useProgram();
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
    useProgram();

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



    renderFBO.buildFBO(1, theApp->GetWidth(), theApp->GetHeight(), theApp->getFBOInternalPrecision(), true);

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

}

renderBaseClass::~renderBaseClass()
{
#if !defined(GLCHAOSP_LIGHTVER)
     delete motionBlur; delete mergedRendering;
     delete axes;
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


        useVertex();   commonVShader = vertObj;
        useFragment();

        vertObj->Load( theApp->get_glslVer().c_str()                         , 1, SHADER_PATH "mmFBO_all_vert.glsl");
        fragObj->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 2, SHADER_PATH "colorSpaces.glsl", SHADER_PATH "RadialBlur2PassFrag.glsl");

        addShader(vertObj);
        addShader(fragObj);

        link();
        //deleteAll();
#ifdef GLAPP_REQUIRE_OGL45
        uniformBlocksClass::create(GLuint(sizeof(uBlurData)), (void *) &uData);
#else
        useProgram();
        LOCorigTexture = getUniformLocation("origTexture");
        LOCpass1Texture = getUniformLocation("pass1Texture");


        uniformBlocksClass::create(GLuint(sizeof(uBlurData)), (void *) &uData, getProgram(), "_blurData");
    #if !defined(GLCHAOSP_LIGHTVER)
        idxSubGlowType[idxSubroutine_ByPass            ]  = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "byPass"                  );
        idxSubGlowType[idxSubroutine_BlurCommonPass1   ]  = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "radialPass1"             );
        idxSubGlowType[idxSubroutine_BlurGaussPass2    ]  = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "radialPass2"             );
        idxSubGlowType[idxSubroutine_BlurThresholdPass2]  = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "radialPass2withBilateral");
        idxSubGlowType[idxSubroutine_Bilateral         ]  = glGetSubroutineIndex(getProgram(),GL_FRAGMENT_SHADER, "bilateralSmooth"         );
    #endif
        ProgramObject::reset();
#endif
}

void BlurBaseClass::glowPass(GLuint sourceTex, GLuint fbo, GLuint subIndex) 
{

    bindPipeline();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);

    //glInvalidateBufferData(fbo);
    glClear(GL_COLOR_BUFFER_BIT);	
    
#ifdef GLAPP_REQUIRE_OGL45
    glBindTextureUnit(0, sourceTex);
    glBindTextureUnit(1, glowFBO.getTex(RB_PASS_1));
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, GLsizei(1), &subIndex);
    updateData(subIndex);

#else
    useProgram();
    glActiveTexture(GL_TEXTURE0+sourceTex);
    glBindTexture(GL_TEXTURE_2D,sourceTex);
    glActiveTexture(GL_TEXTURE0+glowFBO.getTex(RB_PASS_1));
    glBindTexture(GL_TEXTURE_2D,  glowFBO.getTex(RB_PASS_1));
    glUniform1i(LOCorigTexture, sourceTex);
    glUniform1i(LOCpass1Texture, glowFBO.getTex(RB_PASS_1));

    updateData(subIndex);

#if !defined(GLCHAOSP_LIGHTVER)
    glUniformSubroutinesuiv(GL_FRAGMENT_SHADER, GLsizei(1), &idxSubGlowType[subIndex]);
#endif
#endif

    theWnd->getVAO()->draw();
#if !defined(GLAPP_REQUIRE_OGL45)
    ProgramObject::reset();
#endif

}


//
//  colorMapTextured
//
////////////////////////////////////////////////////////////////////////////////
void colorMapTexturedClass::create()
{
    useVertex(commonVShader);
    useFragment();

    //vertObj->Load( theApp->get_glslVer().c_str()                         , 1, SHADER_PATH "mmFBO_all_vert.glsl");
    fragObj->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 2, SHADER_PATH "colorSpaces.glsl", SHADER_PATH "cmTexturedFrag.glsl");


    addShader(vertObj);
    addShader(fragObj);

    link();
    deleteAll();

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
    useProgram();
    glActiveTexture(GL_TEXTURE0 + particles->getPaletteTexID());
    glBindTexture(GL_TEXTURE_2D,  particles->getPaletteTexID());

    glUniform1i(LOCpaletteTex, particles->getPaletteTexID());
#endif
    updateBufferData();

    clearFlagUpdate();

    theWnd->getVAO()->draw();

#if !defined(GLAPP_REQUIRE_OGL45)
    ProgramObject::reset();
#endif
}

#if !defined(GLCHAOSP_NO_FXAA)
//
//  fxaa
//
////////////////////////////////////////////////////////////////////////////////
void fxaaClass::create() {
    useVertex();
    useFragment();

    vertObj->Load( theApp->get_glslVer().c_str()                         , 1, SHADER_PATH "mmFBO_all_vert.glsl");
    fragObj->Load((theApp->get_glslVer() + theApp->get_glslDef()).c_str(), 1, SHADER_PATH "fxaaFrag.glsl");
    addShader(vertObj);
    addShader(fragObj);

    link();

    _fxaaData    = getUniformLocation("fxaaData");
    _invScrnSize = getUniformLocation("invScrnSize");
    _u_colorTexture = getUniformLocation("u_colorTexture");

}

GLuint fxaaClass::render(GLuint texIn)
{
    bindPipeline();

    //glNamedFramebufferDrawBuffer(fbo.getFB(0), GL_NONE);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo.getFB(0));

#ifdef GLAPP_REQUIRE_OGL45
    glBindTextureUnit(0, texIn);
#else
    useProgram();
    glActiveTexture(GL_TEXTURE0 + texIn);
    glBindTexture(GL_TEXTURE_2D,  texIn);

    glUniform1i(_u_colorTexture, texIn);
#endif

    if(renderEngine->checkFlagUpdate()) {
        setUniform2f(_invScrnSize, 1.f/fbo.getSizeX(), 1.f/fbo.getSizeY());    
        updateSettings();
    }

    theWnd->getVAO()->draw();

    return fbo.getTex(0);
}

#endif

