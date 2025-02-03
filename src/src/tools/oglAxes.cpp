//------------------------------------------------------------------------------
//  Copyright (c) 2018-2024 Michele Morrone
//  All rights reserved.
//
//  https://michelemorrone.eu - https://brutpitt.com
//
//  X: https://x.com/BrutPitt - GitHub: https://github.com/BrutPitt
//
//  direct mail: brutpitt(at)gmail.com - me(at)michelemorrone.eu
//
//  This software is distributed under the terms of the BSD 2-Clause license
//------------------------------------------------------------------------------
#include "oglAxes.h"

#include "../glApp.h"
#include "../glWindow.h"


#define SHADER_PATH "Shaders/"

#ifdef GLAPP_MINIMIZE_VERTEX
//
//  Cone
//
////////////////////////////////////////////////////////////////////////////
void buildConeFan(std::vector<vec3> &coneVtx, const float x0, const float x1, const float radius, const int slices)
{
    const float height = x1-x0 ;

    // Scaling factors for vertex normals 

    const float sq = sqrtf( height * height + radius * radius );
    const float cosn =  height / sq;
    const float sinn =  radius / sq;


    const float incAngle = 2*T_PI/(float)( slices );
    float angle = 0;

    const float xt0 = x0 * cosn; 

#   define V(x,y,z) coneVtx.push_back(vec3(x, y, z))
#   define N(x,y,z) V(x,y,z)

    V(0.f, 0.f, x1); N(0.f, 0.f, xt0);
    for (int j=0; j<=slices; j++, angle-=incAngle)
    {   
        float yt0 = cosf(angle);
        const float y0  = yt0*radius;   yt0*=sinn;
        float zt0 = sinf(angle);
        const float z0  = zt0*radius;   zt0*=sinn;   

        V( y0,  z0, x0); N(yt0, zt0, xt0);
    }

#undef V
#undef N
}


void buildCapFan(std::vector<vec3> &coneVtx, const float x0, const float radius, const int slices)
{


    const float incAngle = 2*T_PI/(float)( slices );
    float angle = 0;


#   define V(x,y,z) coneVtx.push_back(vec3(x, y, z))
#   define N(x,y,z) V(x,y,z)

    V(0.f, 0.f, x0); N(0.f, 0.f, -1.f);
    for (int j=0; j<=slices; j++, angle+=incAngle)
    {   
        const float y0  = cosf(angle)*radius;
        const float z0  = sinf(angle)*radius;
        V( y0, z0, x0); N(0.f, 0.f, -1.f);
    }
    
#undef V
#undef N
}


//
//  Cylinder
//      Draw w/o up/dw caps
////////////////////////////////////////////////////////////////////////////
void buildCylStrip(std::vector<vec3> &cylVtx, const float z0, const float z1, const float radius, const int slices)
{

    float x1 = 1.0f, xr1 = radius;
    float y1 = 0.0f, yr1 = 0.0f; // * radius

    
    const float incAngle = 2.0f*T_PI/(float)( slices );
    float angle = incAngle;


#   define V(x,y,z) cylVtx.push_back(vec3(x, y, z))
#   define N(x,y,z) V(x,y,z)

    //V(xr0, yr0, z0); N( x0,  y0,  0.f);
    for (int j=0; j<=slices; j++, angle+=incAngle) {
        const float x0  = x1;   x1  = cosf(angle);
        const float y0  = y1;   y1  = sinf(angle);
        const float xr0 = xr1;  xr1 = x1 * radius;
        const float yr0 = yr1;  yr1 = y1 * radius;

    // Surface
        V(xr0, yr0, z0); N( x0,  y0,  0.f);
        V(xr1, yr1, z1); N( x1,  y1,  0.f);

    }
#undef V
#undef N
}
#else
//
//  Cone
//
////////////////////////////////////////////////////////////////////////////
void buildCone(std::vector<vec3> &coneVtx, const float z0, const float z1, const float radius, const int slices)
{
    const float height = z1-z0 ;

    // Scaling factors for vertex normals 

    const float sq = sqrtf( height * height + radius * radius );
    const float cosn =  height / sq;
    const float sinn =  radius / sq;


    const float incAngle = 2*T_PI/(float)( slices );
    float angle = incAngle;

    float xt1 = sinn,  x1 = radius;// cos(0) * sinn ... cos(0) * radius 
    float yt1 = 0.0f,  y1 = 0.0f;  // sin(0) * sinn ... sin(0) * radius 

    const float zt0 = z0 * cosn, zt1 = z1 * cosn; 

#   define V(x,y,z) coneVtx.push_back(vec3(x, y, z))
#   define N(x,y,z) V(x,y,z)

    for (int j=0; j<slices; j++, angle+=incAngle)
    {   
        const float xt0 = xt1;  xt1 = cosf(angle);
        const float x0  = x1;   x1  = xt1*radius;   xt1*=sinn;
        const float yt0 = yt1;  yt1 = sinf(angle);
        const float y0  = y1;   y1  = yt1*radius;   yt1*=sinn;   

    // base
        V(0.f, 0.f, z0); N(0.f, 0.f,-1.f);
        V( x0,  y0, z0); N(0.f, 0.f,-1.f);
        V( x1,  y1, z0); N(0.f, 0.f,-1.f);
    //surf
        V( x0,  y0, z0); N(xt0, yt0, zt0);
        V(0.f, 0.f, z1); N(0.f, 0.f, zt1);
        V( x1,  y1, z0); N(xt1, yt1, zt0);

    }
#undef V
#undef N
}

//#define SHOW_FULL_CYLINDER
//
//  Cylinder
//      Draw w/o up/dw caps
////////////////////////////////////////////////////////////////////////////
void buildCyl(std::vector<vec3> &cylVtx, const float z0, const float z1, const float radius, const int slices)
{

    float x1 = 1.0f, xr1 = radius;
    float y1 = 0.0f, yr1 = 0.0f; // * radius

    
    const float incAngle = 2.0f*T_PI/(float)( slices );
    float angle = incAngle;


#   define V(x,y,z) cylVtx.push_back(vec3(x, y, z))
#   define N(x,y,z) V(x,y,z)


    for (int j=0; j<slices; j++, angle+=incAngle) {
        const float x0  = x1;   x1  = cosf(angle);
        const float y0  = y1;   y1  = sinf(angle);
        const float xr0 = xr1;  xr1 = x1 * radius;
        const float yr0 = yr1;  yr1 = y1 * radius;

    // Surface
        V(xr0, yr0, z0); N( x0,  y0,  0.f);
        V(xr0, yr0, z1); N( x0,  y0,  0.f);
        V(xr1, yr1, z0); N( x1,  y1,  0.f);
        V(xr1, yr1, z0); N( x1,  y1,  0.f);
        V(xr0, yr0, z1); N( x0,  y0,  0.f);
        V(xr1, yr1, z1); N( x1,  y1,  0.f);

#ifdef SHOW_FULL_CYLINDER 
    // Cover the base  ..in the axes this cap is hidden from cube
        V(0.f, 0.f, z0); N(0.f, 0.f, -1.f);
        V(xr0, yr0, z0); N(0.f, 0.f, -1.f);
        V(xr1, yr1, z0); N(0.f, 0.f, -1.f);

    // Cover the top ..in the arrow this cap is covered from cone/pyramid
        V(xr0, yr0, z1); N(0.f, 0.f, 1.f);
        V(0.f, 0.f, z1); N(0.f, 0.f, 1.f);
        V(xr1, yr1, z1); N(0.f, 0.f, 1.f);
#endif
    }
#undef V
#undef N
}

//
//  Cap (circle)
//      for full axes need only one Cap
////////////////////////////////////////////////////////////////////////////
void buildCap(std::vector<vec3> &vtx, const float z0, const float z1, const float radius, const int slices)
{

    float xr1 = radius;
    float yr1 = 0.0f; // * radius

    const float incAngle = 2.0f*T_PI/(float)( slices );
    float angle = incAngle;


#   define V(x,y,z) vtx.push_back(vec3(x, y, z))
#   define N(x,y,z) V(x,y,z)

    for (int j=0; j<slices; j++, angle+=incAngle) {
        const float x1  = cosf(angle);
        const float y1  = sinf(angle);
        const float xr0 = xr1;  xr1 = x1 * radius;
        const float yr0 = yr1;  yr1 = y1 * radius;

    // Cover the base  ..in the axes this cap is hidden from cube
        V(0.f, 0.f, z0); N(0.f, 0.f, -1.f);
        V(xr0, yr0, z0); N(0.f, 0.f, -1.f);
        V(xr1, yr1, z0); N(0.f, 0.f, -1.f);
#ifdef SHOW_FULL_CYLINDER 
    // Cover the top ..in the arrow this cap is covered from cone/pyramid
        V(xr0, yr0, z1); N(0.f, 0.f, 1.f);
        V(0.f, 0.f, z1); N(0.f, 0.f, 1.f);
        V(xr1, yr1, z1); N(0.f, 0.f, 1.f);
#endif
    }
#undef V
#undef N
}

#endif

//
//  Cube
//      Draw only 2 squared opposed faces, and then instance them
////////////////////////////////////////////////////////////////////////////
void buildCube(std::vector<vec3> &cubeVtx, const float size)
{
#define V(x,y,z) cubeVtx.push_back(vec3(x size, y size, z size))
#define N(x,y,z) cubeVtx.push_back(vec3(x, y, z))

    V(+,+,+); N( 0.0, 0.0, 1.0); V(-,-,+); N( 0.0, 0.0, 1.0); V(-,+,+); N( 0.0, 0.0, 1.0); 
    V(+,-,+); N( 0.0, 0.0, 1.0); V(-,-,+); N( 0.0, 0.0, 1.0); V(+,+,+); N( 0.0, 0.0, 1.0); 

    V(-,-,-); N( 0.0, 0.0,-1.0); V(+,+,-); N( 0.0, 0.0,-1.0); V(-,+,-); N( 0.0, 0.0,-1.0); 
    V(+,-,-); N( 0.0, 0.0,-1.0); V(+,+,-); N( 0.0, 0.0,-1.0); V(-,-,-); N( 0.0, 0.0,-1.0); 

#undef V
#undef N
}


void oglAxes::initShaders(const char *vtxDefs,  const char *fragDefs) 
{

    useVertex(); useFragment();

	getVertex()->Load(vtxDefs, 1, SHADER_PATH "oglAxesVert.glsl");
	getFragment()->Load(fragDefs, 1 ,SHADER_PATH "oglAxesFrag.glsl");

	// The vertex and fragment are added to the program object
    addVertex();
    addFragment();

	link();

    removeAllShaders(true);

    USE_PROGRAM

    _pMat  = getUniformLocation("pMat");
    _mvMat = getUniformLocation("mvMat");
    _zoomF = getUniformLocation("zoomF");
    _light = getUniformLocation("light_position");
    
}

void oglAxes::render() 
{

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glFrontFace(GL_CW);
#if !defined(__EMSCRIPTEN__)
    glDepthRange(-1.0, 1.0);
#endif

    GLfloat f=1.0f;
    glClearBufferfv(GL_DEPTH, 0, &f);    

    glClearBufferfv(GL_COLOR, 0, value_ptr(bgColor));    


    vec3 zoom(axesZoom*zoomFactor);
    //bindPipeline();
    //USE_PROGRAM
    bindShaderProg();
    setUniform3fv(_zoomF, 1, value_ptr(zoom));
    setUniform3fv(_light, 1, value_ptr(lightPos));
    setUniformMatrix4fv(_pMat , 1, GL_FALSE, value_ptr(getTransforms()->tM.pMatrix) );
    setUniformMatrix4fv(_mvMat, 1, GL_FALSE, value_ptr(getTransforms()->tM.mvMatrix));

    //glFrontFace(GL_CCW); 
#ifdef GLAPP_MINIMIZE_VERTEX
    vaoCone->drawInstanced(GL_TRIANGLE_FAN);
    vaoConeCap->drawInstanced(GL_TRIANGLE_FAN);    
    //vaoCone->drawInstanced();
    vaoCyl->drawInstanced(GL_TRIANGLE_STRIP);
    vaoCube->drawInstanced(GL_TRIANGLES);
#else
    vaoAxes->drawInstanced(GL_TRIANGLES);
#endif

#if !defined(GLAPP_USES_GLSL_PIPELINE)
    //reset();
#endif

    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);


}