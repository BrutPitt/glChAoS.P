//------------------------------------------------------------------------------
//  Copyright (c) 2018-2024 Michele Morrone
//  All rights reserved.
//
//  https://michelemorrone.eu - https://glchaosp.com - https://brutpitt.com
//
//  X: https://x.com/BrutPitt - GitHub: https://github.com/BrutPitt
//
//  direct mail: brutpitt(at)gmail.com - me(at)michelemorrone.eu
//
//  This software is distributed under the terms of the BSD 2-Clause license
//------------------------------------------------------------------------------
#pragma once

#include <vector>
#include <iostream>
#include <thread>


#include "glApp.h"
#include "attractorsBase.h"
#include "partSystem.h"


// Includes all the files for the library


using namespace std;

class glApp;

class shaderPointClass;
class shaderBillboardClass;


class vaoClass {
public:
    vaoClass() {
        float vtx[] = {-1.0f,-1.0f,
                        1.0f,-1.0f,
                        1.0f, 1.0f,
                       -1.0f, 1.0f };


        size = sizeof(vtx);
#ifdef GLAPP_REQUIRE_OGL45
        glCreateVertexArrays(1, &vao);
        glCreateBuffers(1, &vaoBuffer);
        glNamedBufferStorage(vaoBuffer, size, vtx, 0); 

        glVertexArrayAttribBinding(vao,vPosition, 0);
        glVertexArrayAttribFormat(vao, vPosition, 2, GL_FLOAT, GL_FALSE, 0);
        glEnableVertexArrayAttrib(vao, vPosition);        

        glVertexArrayVertexBuffer(vao, 0, vaoBuffer, 0, 8);

#else
        glGenVertexArrays(1, &vao); 
        glGenBuffers(1, &vaoBuffer);
        glBindBuffer(GL_ARRAY_BUFFER,vaoBuffer);
        glBufferData(GL_ARRAY_BUFFER,size, vtx, GL_STATIC_DRAW);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vaoBuffer);
        glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, 0L);
        glEnableVertexAttribArray(vPosition);
        CHECK_GL_ERROR();
#endif
    }

    ~vaoClass() {
        glDeleteBuffers(1, &vaoBuffer);
        glDeleteVertexArrays(1, &vao);
    }

    void draw() {
        glBindVertexArray(vao);        
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        CHECK_GL_ERROR();
    }
private:
    GLuint vao, vaoBuffer;
    enum Attrib_IDs { vPosition = 0 };
    int size;
};


class glWindow 
{
public:
    glWindow() {}
    virtual ~glWindow() {}

    void Create(const char* wTitle="glApp", int w=1024, int h=1024, int xPos=100, int yPos=50) {}

    // Called when start Window engine
    virtual void onInit();
    // Called when Window engine is closed.
    virtual void onExit();

    virtual void onIdle();
    virtual GLint onRender();
    virtual void onReshape(GLint w, GLint h);

    virtual void onMouseButton(int button, int upOrDown, int x, int y);
    virtual void onMouseWheel(int wheel, int direction, int x, int y);
    virtual void onMotion(int x, int y);
    virtual void onPassiveMotion(int x, int y);

    virtual void onKeyDown(unsigned char key, int x, int y);
    virtual void onKeyUp(unsigned char key, int x, int y);
    virtual void onSpecialKeyUp(int key, int x, int y);
    virtual void onSpecialKeyDown(int key, int x, int y);

    int GetWidth()  { return theApp->GetWidth();  }
    int GetHeight() { return theApp->GetHeight(); }

    particlesSystemClass *getParticlesSystem() { return particlesSystem; }

    vaoClass *getVAO() { return vao; }
    
private:
    vaoClass *vao;
    
    friend class particlesSystemClass;
    particlesSystemClass *particlesSystem = nullptr;
};

