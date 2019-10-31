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

#include <iomanip>
#include <chrono>
#include <vector>
#include <cstdint>

#include "glslProgramObject.h"
#include "glslShaderObject.h"
#include "appDefines.h"

#define COMPONENTS_PER_ATTRIBUTE 4

//#define GLAPP_REQUIRE_OGL45
//#define PRINT_TIMING

// Create three query objects
// Start the first query

class vertexBufferBaseClass 
{
public:
    vertexBufferBaseClass(GLenum primitive, uint32_t numVertex, int attributesPerVertex) : 
        attributesPerVertex(attributesPerVertex), primitive(primitive), nVtxStepBuffer(numVertex)
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
    void     setVertexCount(GLuint i) { uploadedVtx = i; }
    GLuint   getVBO()            { return vbo; };
    GLenum   getPrimitive() { return primitive; }


    virtual void initBufferStorage(GLsizeiptr numElements) = 0;
    virtual void buildVertexAttrib() {
        const int locID = 0;
#ifdef GLAPP_REQUIRE_OGL45
        glVertexArrayAttribBinding(vao,locID, 0);
        glVertexArrayAttribFormat(vao, locID, COMPONENTS_PER_ATTRIBUTE, GL_FLOAT, GL_FALSE, 0);
        glEnableVertexArrayAttrib(vao, locID);        

        glVertexArrayVertexBuffer(vao, 0, vbo, 0, bytesPerVertex);
#else
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(locID, COMPONENTS_PER_ATTRIBUTE, GL_FLOAT, GL_FALSE, bytesPerVertex, 0L);
        glEnableVertexAttribArray(locID);
#endif
        CHECK_GL_ERROR();
    }


    virtual bool uploadSubBuffer(GLuint nVtx, GLuint szCircularBuff) 
    {
        const GLuint64 offset = uploadedVtx % szCircularBuff;
        const GLuint64 offByte = offset * bytesPerVertex;

        bool retVal = (offset+nVtx >= szCircularBuff) ? true : false;

        if(offset+nVtx > szCircularBuff) {            
            const GLuint64 szPart1 = (szCircularBuff - offset) * bytesPerVertex;
            const GLuint64 szPart2 = ((offset+nVtx) - szCircularBuff) * bytesPerVertex;
           
            //cout << szPart1 << " - " << szPart2 << endl;
#ifdef GLAPP_REQUIRE_OGL45
            glNamedBufferSubData(vbo, offByte, szPart1, vtxBuffer);
            glNamedBufferSubData(vbo, 0      , szPart2, (GLubyte *) vtxBuffer + szPart1); 
        } else {
            //cout << offByte << " - " << nVtx << endl;
            //glBindBuffer(GL_ARRAY_BUFFER,vbo);
            //glBufferSubData(GL_ARRAY_BUFFER, offByte, nVtx * bytesPerVertex, vtxBuffer); 

            glNamedBufferSubData(vbo, offByte, nVtx * bytesPerVertex, vtxBuffer); 
        }
#else
            glBindBuffer(GL_ARRAY_BUFFER,vbo);
            glBufferSubData(GL_ARRAY_BUFFER, offByte, szPart1, vtxBuffer);
            glBufferSubData(GL_ARRAY_BUFFER, 0      , szPart2, (GLubyte *) vtxBuffer + szPart1); 
        } else {
            glBindBuffer(GL_ARRAY_BUFFER,vbo);
            glBufferSubData(GL_ARRAY_BUFFER, offByte, nVtx * bytesPerVertex, vtxBuffer); 
        }
        glBindBuffer(GL_ARRAY_BUFFER,0);
#endif
        uploadedVtx+=nVtx;

        return retVal;
    }

    void draw(GLsizei maxSize) {
        glBindVertexArray(vao);
        glDrawArrays(primitive,0,uploadedVtx<GLuint64(maxSize) ? uploadedVtx : maxSize);
        CHECK_GL_ERROR();
    }
    void draw(uint32_t start, GLsizei maxElements, GLsizei maxSize) {
        glBindVertexArray(vao);
        const GLsizei limit = uploadedVtx<GLuint64(maxSize) ? uploadedVtx : maxSize;
        glDrawArrays(primitive,start,start+maxElements<limit ? maxElements : limit-start);
        CHECK_GL_ERROR();
    }

#if !defined(GLCHAOSP_LIGHTVER)
//  Feedback functions
////////////////////////////////////////////////////////////////////////////
    void BindToFeedback(int index)
    {
        glTransformFeedbackBufferRange(0,index,vbo,0,uploadedVtx*bytesPerVertex);
    }
#endif
    void uploadData(int numVtx) 
    {
        int bufferSize = numVtx * bytesPerVertex;
        uploadedVtx = numVtx;
   
        glBindBuffer(GL_ARRAY_BUFFER,vbo);
        glBufferData(GL_ARRAY_BUFFER,bufferSize,&vtxBuffer[0],GL_STATIC_DRAW);        
        //glBufferSubData(GL_ARRAY_BUFFER,0,bufferSize,&vtxBuffer[0]);        
        //glInvalidateBufferData(vbo);
        glBindBuffer(GL_ARRAY_BUFFER,0);
    }
    void drawRange(GLuint start, GLuint size) 
    {
        glBindVertexArray(vao);
        glDrawArrays(primitive,start,size);
    }

protected:
     GLfloat *vtxBuffer = nullptr;
    int attributesPerVertex;
    GLuint vbo,vao;
    GLuint nVtxStepBuffer;
    GLuint bytesPerVertex;           //Total bytes per Vertex: all attributes!
    //GLuint64 uploadedDataSize;
    GLuint64 uploadedVtx;
    GLenum primitive;
};

class mappedVertexBuffer : public vertexBufferBaseClass
{
public:

    mappedVertexBuffer(GLenum primitive, uint32_t numVertex, int attributesPerVertex) : 
        vertexBufferBaseClass(primitive, numVertex, attributesPerVertex) { }

    ~mappedVertexBuffer() {
#ifdef GLAPP_REQUIRE_OGL45
        glUnmapNamedBuffer(vbo);
#else
        //glBindBuffer(GL_ARRAY_BUFFER,vbo);        
        //glUnmapBuffer(GL_ARRAY_BUFFER);
#endif
    }

    void initBufferStorage(GLsizeiptr nVtx) {
        GLsizeiptr storageSize = nVtx * bytesPerVertex;

#ifdef GLAPP_REQUIRE_OGL45
        glNamedBufferStorage(vbo, storageSize, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT ); // ); //  
        vtxBuffer = (GLfloat *) glMapNamedBufferRange(vbo, 0, storageSize, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT  | GL_MAP_COHERENT_BIT );   // | GL_MAP_FLUSH_EXPLICIT_BIT
        if(vtxBuffer==nullptr) { cout << "glMapNamedBufferRange: memory allocation failure!" << endl; exit(-1); }
#else
        //glBindBuffer(GL_ARRAY_BUFFER,vbo); 
        //glBufferData(GL_ARRAY_BUFFER, storageSize, nullptr, GL_DYNAMIC_DRAW ); // ); //
        //vtxBuffer = (GLfloat *) glMapBuffer(GL_ARRAY_BUFFER, GL_READ_WRITE  );   //| GL_MAP_COHERENT_BIT
#endif
        CHECK_GL_ERROR();
        buildVertexAttrib();
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

#ifdef GLAPP_REQUIRE_OGL45
        vtxBuffer = new GLfloat[nVtxStepBuffer * attributesPerVertex * COMPONENTS_PER_ATTRIBUTE];
        glNamedBufferStorage(vbo, storageSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
        //glNamedBufferData(vbo, storageSize,nullptr,GL_DYNAMIC_DRAW);
#else
        vtxBuffer = new GLfloat[nVtxStepBuffer * attributesPerVertex * COMPONENTS_PER_ATTRIBUTE];
        glBindBuffer(GL_ARRAY_BUFFER,vbo);        
        glBufferData(GL_ARRAY_BUFFER,storageSize,nullptr,GL_DYNAMIC_DRAW);
#endif
        CHECK_GL_ERROR();

        buildVertexAttrib();
    }
};

#if !defined(GLCHAOSP_LIGHTVER)

class transformFeedbackInterleaved {
  private:
    GLuint query;
    vertexBuffer *vbo;
    static bool FeedbackActive;
    bool bDiscard;

  public:
    transformFeedbackInterleaved(vertexBuffer *vbo)
    {
      this->vbo = vbo;
      glGenQueries(1,&query);
      bDiscard = true;
    }

    void Begin() {
        if (FeedbackActive) return;

        FeedbackActive = true;
        vbo->BindToFeedback(0);

        glBeginTransformFeedback(vbo->getPrimitive());

        if (bDiscard) glEnable(GL_RASTERIZER_DISCARD);

        glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);
    }

    int End() {
        int iPrimitivesWritten;
        if(!FeedbackActive)  return -1;
        FeedbackActive = false;

        if(bDiscard)  glDisable(GL_RASTERIZER_DISCARD);
        glEndTransformFeedback();

        glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
        glGetQueryObjectiv(query,GL_QUERY_RESULT,&iPrimitivesWritten);
  
        return iPrimitivesWritten;

    }
    void SetDiscard(bool value){ bDiscard = true; }
};
#endif

#ifdef PRINT_TIMING
        //glBeginQuery(GL_TIME_ELAPSED, queries[0]);
        cout <<  "n.vtx:"<< std::setfill( ' ' ) << std::setw(7) << nVtx << " - ";
        auto before = std::chrono::system_clock::now();
#endif
#ifdef PRINT_TIMING
        //glEndQuery(GL_TIME_ELAPSED);
        auto now = std::chrono::system_clock::now();
        double diff_ms = std::chrono::duration <double, std::nano> (now - before).count();
        cout << std::setw(3) << std::fixed << std::setprecision(3) << std::setfill( '0' ) << "buff ms: " << float(diff_ms)/1000000.f;
#endif


#ifdef NON_MI_SERVE
    GLfloat *mapBuffer(GLuint nVtx) {
        uploadedVtx = 0;
        return (GLfloat *) glMapNamedBufferRange(vbo, 0, nVtx * bytesPerVertex, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
    }

    void unmapBuffer(GLuint nVtx) {
        glUnmapNamedBuffer(vbo);
        uploadedVtx+=nVtx;
    }



    GLfloat* mapBuffer(GLuint nVtx, GLuint szCircularBuff)
    {
        //int bufferSize = nVtx * bytesPerVertex;
        //GLuint bytesCircularBuffer = szCircularBuff * bytesPerVertex;
        const GLuint offset = uploadedVtx % szCircularBuff;


        return (GLfloat *) ((GLbyte *) vtxBuffer + offset * bytesPerVertex); //glMapNamedBufferRange(vbo, offset, bufferSize, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT  | GL_MAP_COHERENT_BIT);
    }


    void viewElapsed() {
#ifdef PRINT_TIMING
        glGetQueryObjectuiv(queries[0], GL_QUERY_RESULT, &buffFill);
        glGetQueryObjectuiv(queries[1], GL_QUERY_RESULT, &drawArray);
        
        cout << std::fixed << std::setprecision(3) << std::setfill( '0' ) << "Buff: " << float(buffFill)/1000000.f << " - Array: " << float(drawArray)/1000000.f << endl;
#endif
    }
        

    void drawVtx() 
    {
        ActivateClientStates();
        glDrawArrays(primitive,0,numVertex);
        DeactivateClientStates();

    }


#endif
