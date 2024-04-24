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
#include "glslProgramObject.h"



/////////////////////////////////////////////////
void ProgramObject::createProgram()
{
    program = glCreateProgram();

#ifdef GLAPP_USES_GLSL_PIPELINE
    #ifdef GLAPP_REQUIRE_OGL45
        glCreateProgramPipelines(1, &pipeline);
    #else
        glGenProgramPipelines(1, &pipeline);
    #endif
#endif
}

/////////////////////////////////////////////////
void ProgramObject::addShader(ShaderObject* shader)
{
    if(!program) createProgram();
    glAttachShader(program, shader->getShader());
    shader->statusAttached();
}

/////////////////////////////////////////////////
void ProgramObject::removeShader(ShaderObject* shader, bool wantDelete)
{
    if(shader!=nullptr)  shader->detachShader(program, wantDelete);
}

void ProgramObject::deleteShader(ShaderObject* shader)
{
    if(shader!=nullptr) shader->deleteShader();
}

void checkProgram(GLuint program);
/////////////////////////////////////////////////
void ProgramObject::link()
{
    if(!program) createProgram();
#ifdef GLAPP_USES_GLSL_PIPELINE
    glProgramParameteri(program, GL_PROGRAM_SEPARABLE, GL_TRUE);
#endif
    glLinkProgram(program);

    checkProgram(program);

#ifdef GLAPP_USES_GLSL_PIPELINE
    glUseProgramStages(pipeline, (vertObj == nullptr ? 0 : GL_VERTEX_SHADER_BIT  ) | 
                                 (geomObj == nullptr ? 0 : GL_GEOMETRY_SHADER_BIT) | 
                                 (fragObj == nullptr ? 0 : GL_FRAGMENT_SHADER_BIT), 
                       program);
#endif
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
