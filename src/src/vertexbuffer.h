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

#include <iomanip>
#include <chrono>
#include <vector>
#include <cstdint>

#include "glslProgramObject.h"
#include "glslShaderObject.h"
#include "appDefines.h"

#define COMPONENTS_PER_ATTRIBUTE 4

class vertexBufferBaseClass
{
public:
    vertexBufferBaseClass(GLenum primitive, uint32_t numVertex, int attributesPerVertex) : 
        primitive(primitive), nVtxStepBuffer(numVertex), attributesPerVertex(attributesPerVertex) 
    {
#ifdef GLAPP_REQUIRE_OGL45 
        glCreateVertexArrays(1, &vao);
        glCreateBuffers(1,&vbo);
#else
        glGenVertexArrays(1, &vao); 
        glGenBuffers(1,&vbo);
#endif
        bytesPerVertex = attributesPerVertex * COMPONENTS_PER_ATTRIBUTE * sizeof(float); 
        uploadedVtx = 0;
    }

    virtual ~vertexBufferBaseClass() 
    {
        glDeleteVertexArrays(1, &vao);
        glDeleteBuffers(1,&vbo);
    }

    GLfloat* getBuffer()  { return vtxBuffer; }
    int      getBytesPerVertex() { return bytesPerVertex; }
    int      getNumComponents()  { return COMPONENTS_PER_ATTRIBUTE * attributesPerVertex; }
    int      getAttribPerVtx()  { return attributesPerVertex; }
    GLuint64 getVertexUploaded() { return uploadedVtx; }
    GLuint64 *getPtrVertexUploaded() { return &uploadedVtx; }
    void     incVertexCount() { uploadedVtx++;  }
    void     resetVertexCount()  { uploadedVtx = 0; }
    void     setVertexCount(GLuint64 i) { uploadedVtx = i; }
    void     addVertexCount(GLuint64 i) { uploadedVtx+= i; }
    GLuint   getVAO()            { return vao; };
    GLuint   getVBO()            { return vbo; };
    GLenum   getPrimitive() { return primitive; }
    GLuint   getNumVtxStepBuffer() { return nVtxStepBuffer; }


    virtual void initBufferStorage(GLsizeiptr numElements) {}
    virtual void initBufferStorage(GLsizeiptr nVtx, GLenum target, GLenum usage, bool allocMemBuffer=false) {}
    virtual void buildVertexAttrib() {
        const int locID = 0;
#ifdef GLAPP_REQUIRE_OGL45
        glVertexArrayAttribBinding(vao,locID, 0);
        glVertexArrayAttribFormat(vao, locID, COMPONENTS_PER_ATTRIBUTE, GL_FLOAT, GL_FALSE, 0L);
        glEnableVertexArrayAttrib(vao, locID);        

        glVertexArrayVertexBuffer(vao, 0, vbo, 0, bytesPerVertex);
#else
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(locID, COMPONENTS_PER_ATTRIBUTE, GL_FLOAT, GL_FALSE, bytesPerVertex, 0L);
        glEnableVertexAttribArray(locID);
#endif
        CHECK_GL_ERROR()
    }
    virtual void buildTransformVertexAttrib() {
        enum { locPos, locVelT };
#ifdef GLAPP_REQUIRE_OGL45
        glVertexArrayAttribBinding(vao,locPos, 0);
        glVertexArrayAttribFormat (vao,locPos, COMPONENTS_PER_ATTRIBUTE, GL_FLOAT, GL_FALSE, 0L);
        glEnableVertexArrayAttrib (vao,locPos);

        glVertexArrayAttribBinding(vao,locVelT, 0);
        glVertexArrayAttribFormat (vao,locVelT, COMPONENTS_PER_ATTRIBUTE, GL_FLOAT, GL_FALSE, sizeof(vec4));
        glEnableVertexArrayAttrib (vao,locVelT);

        glVertexArrayVertexBuffer(vao, 0, vbo, 0, bytesPerVertex);
#else
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(locPos , 4, GL_FLOAT, GL_FALSE, bytesPerVertex, 0L);
        glVertexAttribPointer(locVelT, 4, GL_FLOAT, GL_FALSE, bytesPerVertex, (GLvoid *)sizeof(vec4));
        glEnableVertexAttribArray(locPos );
        glEnableVertexAttribArray(locVelT);
#endif
        CHECK_GL_ERROR()
    }

    virtual bool uploadSubBuffer(GLuint nVtx, GLuint szCircularBuff, bool stopFull = false) 
    {
        const GLuint64 offset = uploadedVtx % szCircularBuff;
        const GLuint64 offByte = offset * bytesPerVertex;

        bool retVal = offset+nVtx >= szCircularBuff;

        GLuint addVtx = nVtx;

        if(offset+nVtx > szCircularBuff) {
            const GLuint64 remainVtx = szCircularBuff - offset;
            const GLuint64 szPart1 = remainVtx * bytesPerVertex;
            const GLuint64 szPart2 = ((offset+nVtx) - szCircularBuff) * bytesPerVertex;
           
#ifdef GLAPP_REQUIRE_OGL45
            glNamedBufferSubData(vbo, offByte, szPart1, vtxBuffer);
            if(!stopFull) glNamedBufferSubData(vbo, 0      , szPart2, (GLubyte *) vtxBuffer + szPart1); 
            else          addVtx = remainVtx;
                
        } else {
            glNamedBufferSubData(vbo, offByte, nVtx * bytesPerVertex, vtxBuffer); 
        }
#else
            glBindBuffer(GL_ARRAY_BUFFER,vbo);
            glBufferSubData(GL_ARRAY_BUFFER, offByte, szPart1, vtxBuffer);
            if(!stopFull) glBufferSubData(GL_ARRAY_BUFFER, 0      , szPart2, (GLubyte *) vtxBuffer + szPart1); 
            else          addVtx = remainVtx;
        } else {
            glBindBuffer(GL_ARRAY_BUFFER,vbo);
            glBufferSubData(GL_ARRAY_BUFFER, offByte, nVtx * bytesPerVertex, vtxBuffer); 
        }
        glBindBuffer(GL_ARRAY_BUFFER,0);
#endif
        uploadedVtx+=addVtx;
        return retVal;
    }

    void draw(GLsizei maxSize) {
        glBindVertexArray(vao);
        glDrawArrays(primitive,0,uploadedVtx<GLuint64(maxSize) ? uploadedVtx : maxSize);
        if(theApp->useSyncOGL()) glFinish(); // stuttering with frequent calls on slow GPU & APPLE

        CHECK_GL_ERROR()
    }

    void draw(uint32_t start, GLsizei maxElements, GLsizei maxSize) {
        const GLsizei limit = uploadedVtx<GLuint64(maxSize) ? uploadedVtx : maxSize;
        glBindVertexArray(vao);
        glDrawArrays(primitive,start,start+maxElements<limit ? maxElements : limit-start);
        CHECK_GL_ERROR()
    }

#if !defined(GLCHAOSP_NO_TF)
//  Feedback functions
////////////////////////////////////////////////////////////////////////////
    void BindToFeedback(int index, GLsizeiptr sz)
    {
#ifdef GLAPP_REQUIRE_OGL45
        glTransformFeedbackBufferRange(0,index,vbo,0,sz*bytesPerVertex);
#else
        //WebGL need *Base (otherwise circular don't works)
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, index, vbo);
        //glBindBufferRange(GL_TRANSFORM_FEEDBACK_BUFFER, index, vbo, 0, sz*bytesPerVertex);
#endif
    }
    
    void endBindToFeedback(int index)
    {
        // Need this in WebGL
        // https://bugs.chromium.org/p/chromium/issues/detail?id=853978
#ifdef GLAPP_REQUIRE_OGL45
        glTransformFeedbackBufferBase(0,index,0);
#else
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER,index,0);
#endif
    }
#endif
    void uploadData(int numVtx) 
    {
        int bufferSize = numVtx * bytesPerVertex;
   
        glBindBuffer(GL_ARRAY_BUFFER,vbo);
        glBufferSubData(GL_ARRAY_BUFFER,0,bufferSize,vtxBuffer);
    }
    void drawRange(GLenum type, GLuint start, GLuint size)
    {
        glBindVertexArray(vao);
        glBindBuffer(type, vbo);

        glDrawArrays(primitive,start,size);
        CHECK_GL_ERROR()
        if(theApp->useSyncOGL()) glFinish(); // stuttering with frequent calls on slow GPU & APPLE
    }

protected:
     GLfloat *vtxBuffer = nullptr;
    int attributesPerVertex;
    GLuint vbo,vao;
    GLuint nVtxStepBuffer;
    GLuint bytesPerVertex;           //Total bytes per Vertex: all attributes!
    GLuint64 uploadedVtx;
    GLenum primitive;
};

// Only in v.4.5
class mappedVertexBuffer : public vertexBufferBaseClass
{
public:

    mappedVertexBuffer(GLenum primitive, uint32_t numVertex, int attributesPerVertex) : 
        vertexBufferBaseClass(primitive, numVertex, attributesPerVertex) { }

    ~mappedVertexBuffer() {
#ifdef GLAPP_REQUIRE_OGL45
        glUnmapNamedBuffer(vbo);
#endif
    }

    void initBufferStorage(GLsizeiptr nVtx) {
        GLsizeiptr storageSize = nVtx * bytesPerVertex;

#ifdef GLAPP_REQUIRE_OGL45
        glNamedBufferStorage(vbo, storageSize, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT ); // ); //  
        vtxBuffer = (GLfloat *) glMapNamedBufferRange(vbo, 0, storageSize, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT  | GL_MAP_COHERENT_BIT );   // | GL_MAP_FLUSH_EXPLICIT_BIT
        if(vtxBuffer==nullptr) { cout << "glMapNamedBufferRange: memory allocation failure!" << endl; exit(-1); }
#endif
        CHECK_GL_ERROR()
    }
};

class vertexBuffer : public vertexBufferBaseClass 
{
public:
    vertexBuffer(GLenum primitive, uint32_t numVertex, int attributesPerVertex) : 
        vertexBufferBaseClass(primitive, numVertex, attributesPerVertex) { }

    ~vertexBuffer() { delete [] vtxBuffer; }

    void initBufferStorage(GLsizeiptr nVtx) {
        GLsizeiptr storageSize = nVtx * bytesPerVertex;

        vtxBuffer = new GLfloat[nVtxStepBuffer * attributesPerVertex * COMPONENTS_PER_ATTRIBUTE];
#ifdef GLAPP_REQUIRE_OGL45
        glNamedBufferStorage(vbo, storageSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
#else
        glBindBuffer(GL_ARRAY_BUFFER,vbo);        
        glBufferData(GL_ARRAY_BUFFER,storageSize,nullptr,GL_DYNAMIC_DRAW);
#endif
        CHECK_GL_ERROR()
    }
};

class transformVertexBuffer : public vertexBufferBaseClass 
{
public:
    transformVertexBuffer(GLenum primitive, uint32_t numVertex, int attributesPerVertex) : 
        vertexBufferBaseClass(primitive, numVertex, attributesPerVertex) { }

    ~transformVertexBuffer() { delete [] vtxBuffer; }

    void initBufferStorage(GLsizeiptr nVtx, GLenum target, GLenum usage, bool allocMemBuffer=false) {
        GLsizeiptr storageSize = nVtx * bytesPerVertex;

        if(allocMemBuffer) vtxBuffer = new GLfloat[nVtxStepBuffer * attributesPerVertex * COMPONENTS_PER_ATTRIBUTE];
#ifdef GLAPP_REQUIRE_OGL45
        glNamedBufferStorage(vbo, storageSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
        //glNamedBufferData(vbo, storageSize,nullptr,GL_DYNAMIC_DRAW);
#else
        glBindBuffer(target,vbo);        
        glBufferData(target,storageSize,nullptr,usage);
#endif
        CHECK_GL_ERROR()
    }
};

//#ifdef PRINT_TIMING
//        //glBeginQuery(GL_TIME_ELAPSED, queries[0]);
//        cout <<  "n.vtx:"<< std::setfill( ' ' ) << std::setw(7) << nVtx << " - ";
//        auto before = std::chrono::system_clock::now();
//#endif
//#ifdef PRINT_TIMING
//        //glEndQuery(GL_TIME_ELAPSED);
//        auto now = std::chrono::system_clock::now();
//        double diff_ms = std::chrono::duration <double, std::nano> (now - before).count();
//        cout << std::setw(3) << std::fixed << std::setprecision(3) << std::setfill( '0' ) << "buff ms: " << float(diff_ms)/1000000.f;
//#endif


