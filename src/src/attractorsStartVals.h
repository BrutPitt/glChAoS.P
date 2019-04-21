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





