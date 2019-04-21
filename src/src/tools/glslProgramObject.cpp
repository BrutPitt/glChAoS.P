////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2018-2019 Michele Morrone
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
//  This software is distributed under the terms of the BSD 2-Clause license
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
	if(shader!=nullptr) glDetachShader(program, shader->getShader());
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

#if !defined(NDEBUG)
    if(location==-1)  cout << "Uniform variable \"" << name << "\" not found" << endl;
#endif
    return location;
}
