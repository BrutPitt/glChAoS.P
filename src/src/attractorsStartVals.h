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
#pragma once
#include <vector>

#include <glm/glm.hpp>

using namespace std;
using namespace glm;

void magneticStartVals(vector<vec3> &vVal, vector<vec3> &kVal);
void PowerN3DStartVals(vector<vec3> &vVal, vector<vec3> &kVal);
void SinCosStartVals(vector<vec3> &vVal, vector<float> &kVal);
void PickoverStartVals(vector<vec3> &vVal, vector<float> &kVal);
void PolinomialStartVals(vector<vec3> &vVal, vector<float> &kVal);
void ChenLeeStartVals(vector<vec3> &vVal, vector<float> &kVal);
void TSUCS1StartVals(vector<vec3> &vVal, vector<float> &kVal);
void AizawaStartVals(vector<vec3> &vVal, vector<float> &kVal);
void FourWing2StartVals(vector<vec3> &vVal, vector<float> &kVal);
void ThomasStartVals(vector<vec3> &vVal, vector<float> &kVal);
void LorenzStartVals(vector<vec3> &vVal, vector<float> &kVal);
void HopalongStartVals(vector<vec3> &vVal, vector<float> &kVal);





