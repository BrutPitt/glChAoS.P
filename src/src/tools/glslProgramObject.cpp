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
#include "glslProgramObject.h"

ProgramObject::ProgramObject()
{
	program = 0;
}

ProgramObject::~ProgramObject()
{
    deleteProgram();	
}

/////////////////////////////////////////////////
void ProgramObject::createProgram()
{
    program = glCreateProgram();

#if !defined(__EMSCRIPTEN__) && !defined(GLAPP_NO_GLSL_PIPELINE)
    #ifdef GLAPP_REQUIRE_OGL45
        glCreateProgramPipelines(1, &pipeline);
    #else
        glGenProgramPipelines(1, &pipeline);
    #endif
#endif
}

/////////////////////////////////////////////////
void ProgramObject::deleteProgram()
{
    if(program) glDeleteProgram(program);
}


/////////////////////////////////////////////////
void ProgramObject::addShader(ShaderObject* shader)
{
    if(!program) createProgram();
	glAttachShader(program, shader->getShader());
}

/////////////////////////////////////////////////
void ProgramObject::removeShader(ShaderObject* shader)
{
	glDetachShader(program, shader->getShader());
}

void checkProgram(GLuint program);
/////////////////////////////////////////////////
void ProgramObject::link()
{
    if(!program) createProgram();
#if !defined(__EMSCRIPTEN__) && !defined(GLAPP_NO_GLSL_PIPELINE)
    glProgramParameteri(program, GL_PROGRAM_SEPARABLE, GL_TRUE);
#endif
	glLinkProgram(program);

    checkProgram(program);

#if !defined(__EMSCRIPTEN__) && !defined(GLAPP_NO_GLSL_PIPELINE)
    glUseProgramStages(pipeline, (vertObj == nullptr ? 0 : GL_VERTEX_SHADER_BIT  ) | 
                                 (geomObj == nullptr ? 0 : GL_GEOMETRY_SHADER_BIT) | 
                                 (fragObj == nullptr ? 0 : GL_FRAGMENT_SHADER_BIT), 
                       program);
#endif
}

/////////////////////////////////////////////////
void ProgramObject::useProgram()
{
	glUseProgram(program);
}
/////////////////////////////////////////////////
void ProgramObject::bindPipeline()
{
#if !defined(__EMSCRIPTEN__) && !defined(GLAPP_NO_GLSL_PIPELINE)
	glBindProgramPipeline(pipeline);
#endif
}

/////////////////////////////////////////////////
void ProgramObject::reset()
{
	glUseProgram(0);
}


/////////////////////////////////////////////////
GLint ProgramObject::getUniformLocation(const GLchar* name)
{
	GLint location = glGetUniformLocation(program, name);

    if(location==-1)  cout << "Uniform variable \"" << name << "\" not found" << endl;
	return location;
}
