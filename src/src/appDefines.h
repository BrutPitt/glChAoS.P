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
#pragma once

#define APP_USE_FBO
//#define __APPLE__

//#define GLAPP_NO_GLSL_PIPELINE


//#define APP_DEBUG_GUI_INTERFACE


#define GLAPP_IMGUI_VIEWPORT

#ifdef __EMSCRIPTEN__
    #define DOT_TEXT_SHFT 1
#else
    #define DOT_TEXT_SHFT 2
#endif


#ifdef GLCHAOSP_LIGHTVER
//FF !:! pixel -> layout.css.devPixelsPerPx = 1
    #define GLCHAOSP_LIGHTVER_EXPERIMENTAL
    #define GLCHAOSP_NO_FXAA
    //#define GLCHAOSP_NO_DeL
    //#define GLCHAOSP_USE_LOWPRECISION
#endif

//Attractor, Fractal, OpenGL, 3D, C++, Particles, ImGui, Source, Code, Algorithm, Chaos, caos, hypercomplex, Julia

#define DRAGFLOAT_POW_2 (1.0f)
#define DRAGFLOAT_POW_3 (1.0f)


#if defined(USE_MAPPED_BUFFER) && !defined(GLAPP_REQUIRE_OGL45)
    #error OpenGL 4.5 required: compile with -D GLAPP_REQUIRE_OGL45 
#endif

//#define GLAPP_REQUIRE_OGL45 ->in Compiler option
//#define SIMPLEX_PROC
//#define BYPASS_FBO

#ifndef theApp
    #define theApp mainGLApp::theMainApp
#endif
#ifndef theWnd
    #define theWnd theApp->getEngineWnd()
#endif
#ifndef theDlg
    #define theDlg theApp->getMainDlg()
#endif

#if !defined(GLCHAOSP_LIGHTVER)
    #define INIT_WINDOW_W 1280
    #define INIT_WINDOW_H 1024
#else
    #define INIT_WINDOW_W 1024
    #define INIT_WINDOW_H 1024
#endif

#define getBillboard(H) H->shaderBillboardClass
#define getPointsprite(H) H->shaderPointClass

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
    #include <emscripten/html5.h>
    #include <GLES3/gl3.h>
#else
    #include <glad/glad.h>
#endif


#ifdef APP_USE_GLUT
    #include <GL/freeglut.h>

#define APP_NO_MOD      0
#define APP_MOD_SHIFT   GLUT_ACTIVE_SHIFT 
#define APP_MOD_CONTROL GLUT_ACTIVE_CTRL
#define APP_MOD_ALT     GLUT_ACTIVE_ALT
#define APP_MOD_SUPER   8

#define APP_MOUSE_BUTTON_LEFT   GLUT_LEFT_BUTTON  
#define APP_MOUSE_BUTTON_RIGHT  GLUT_RIGHT_BUTTON 
#define APP_MOUSE_BUTTON_MIDDLE GLUT_MIDDLE_BUTTON

#define APP_MOUSE_BUTTON_DOWN  GLUT_DOWN
#define APP_MOUSE_BUTTON_UP    GLUT_UP  
   
#define APP_MOUSE_BUTTON_1 GLUT_LEFT_BUTTON  
#define APP_MOUSE_BUTTON_2 GLUT_LEFT_BUTTON + 1
#define APP_MOUSE_BUTTON_3 GLUT_LEFT_BUTTON + 2
#define APP_MOUSE_BUTTON_4 GLUT_LEFT_BUTTON + 3
#define APP_MOUSE_BUTTON_5 GLUT_LEFT_BUTTON + 4
#define APP_MOUSE_BUTTON_6 GLUT_LEFT_BUTTON + 5
#define APP_MOUSE_BUTTON_7 GLUT_LEFT_BUTTON + 6
#define APP_MOUSE_BUTTON_8 GLUT_LEFT_BUTTON + 7

#define APP_KEY_F1  GLUT_KEY_F1 
#define APP_KEY_F2  GLUT_KEY_F2 
#define APP_KEY_F3  GLUT_KEY_F3 
#define APP_KEY_F4  GLUT_KEY_F4 
#define APP_KEY_F5  GLUT_KEY_F5 
#define APP_KEY_F6  GLUT_KEY_F6 
#define APP_KEY_F7  GLUT_KEY_F7 
#define APP_KEY_F8  GLUT_KEY_F8 
#define APP_KEY_F9  GLUT_KEY_F9 
#define APP_KEY_F10 GLUT_KEY_F10
#define APP_KEY_F11 GLUT_KEY_F11
#define APP_KEY_F12 GLUT_KEY_F12
#else // Application use GLFW

#define APP_NO_MOD      0
#define APP_MOD_SHIFT   GLFW_MOD_SHIFT
#define APP_MOD_CONTROL GLFW_MOD_CONTROL
#define APP_MOD_ALT     GLFW_MOD_ALT
#define APP_MOD_SUPER   GLFW_MOD_SUPER

#define APP_MOUSE_BUTTON_LEFT   GLFW_MOUSE_BUTTON_LEFT
#define APP_MOUSE_BUTTON_RIGHT  GLFW_MOUSE_BUTTON_RIGHT
#define APP_MOUSE_BUTTON_MIDDLE GLFW_MOUSE_BUTTON_MIDDLE

#define APP_MOUSE_BUTTON_DOWN  GLFW_PRESS
#define APP_MOUSE_BUTTON_UP    GLFW_RELEASE  

#define APP_MOUSE_BUTTON_1 GLFW_MOUSE_BUTTON_1
#define APP_MOUSE_BUTTON_2 GLFW_MOUSE_BUTTON_2
#define APP_MOUSE_BUTTON_3 GLFW_MOUSE_BUTTON_3
#define APP_MOUSE_BUTTON_4 GLFW_MOUSE_BUTTON_4
#define APP_MOUSE_BUTTON_5 GLFW_MOUSE_BUTTON_5
#define APP_MOUSE_BUTTON_6 GLFW_MOUSE_BUTTON_6
#define APP_MOUSE_BUTTON_7 GLFW_MOUSE_BUTTON_7
#define APP_MOUSE_BUTTON_8 GLFW_MOUSE_BUTTON_8

#define APP_KEY_F1  GLFW_KEY_F1 
#define APP_KEY_F2  GLFW_KEY_F2 
#define APP_KEY_F3  GLFW_KEY_F3 
#define APP_KEY_F4  GLFW_KEY_F4 
#define APP_KEY_F5  GLFW_KEY_F5 
#define APP_KEY_F6  GLFW_KEY_F6 
#define APP_KEY_F7  GLFW_KEY_F7 
#define APP_KEY_F8  GLFW_KEY_F8 
#define APP_KEY_F9  GLFW_KEY_F9 
#define APP_KEY_F10 GLFW_KEY_F10
#define APP_KEY_F11 GLFW_KEY_F11
#define APP_KEY_F12 GLFW_KEY_F12

#endif



