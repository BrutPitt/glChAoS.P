//------------------------------------------------------------------------------
//  Copyright (c) 2018-2020 Michele Morrone
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

#include <vector>
#include <functional>
#include <float.h>

#include <fastPRNG.h>
#include <vgMath.h>

extern const char *tranformsText;
extern std::vector<std::function<vec4(const vec4 &)>> variationFuncsArray;

class variationFuncsClass
{
public:
    static vec4 linear(const vec4& v) { return v; }

    static vec4 spherical(const vec4& v)
    {
        const float r = 1.f / (dot(v,v) + FLT_EPSILON);
        return v*r; //vec4(vec3(v)/r, 1.f);
    }

    static vec4 swirl2D(const vec4& v) {
        const float r = length(v);
        const float theta = atan2(v.y, v.x);
        return vec4(r * cos(theta + r),
                    r * sin(theta + r),
                    v.z,
                    v.w);
    }
    static vec4 swirl4D(const vec4& v) {
        const float r = length(v);
        const float theta1 = atan2(v.y, v.x);
        const float theta2 = atan2(v.w, v.z);
        return vec4(r * cos(theta1 + r),
                    r * sin(theta1 + r),
                    r * cos(theta2 + r),
                    r * sin(theta2 + r));
    }

    static vec4 polar3D(const vec4& v) {
        const float r = length(vec3(v));
        const float theta = atan2(v.y, v.x);
        const float phi = acos(v.z/r);
        return vec4(r, theta, phi, 1.0);
/*        return vec4(r*sin(theta)*cos(phi),
                    r*sin(theta)*sin(phi),
                    r*cos(theta),
                    1.0);*/
    }
};


struct ifsDataStruct
{
    ifsDataStruct(float f) : variations(vec4(f)) {}
    vec4 variations = vec4(0.f);
    float variationFactor = 1.0;
    float weight = 1.f;
    float startWeight = 0; // [0, 1]
    int transfType = 0;
    bool active = true;
    //std::function<vec4(variationFuncsClass&, const vec4 &)> variationFunc = &variationFuncsClass::linear;
    std::function<vec4(const vec4 &)> variationFunc = &variationFuncsClass::linear;
};

extern fastPRNG::fastRand32 fastRandom;
extern fastPRNG::fastXS64 fastPrng64;

class ifsBaseClass
{
public:
    ifsBaseClass(float f) : initValue(f) { setSize(); }
    void setSize() { ifsTransforms.resize(numTransf, ifsDataStruct(initValue)); activeIDX.resize(numTransf); }

    void set() {
        setSize();
        rebuildWeight();
    }

    void rebuildWeight() {
        totalWeight = 0;
        for(int i=0; i<numTransf; i++)
            if(ifsTransforms[i].active)
                totalWeight += ifsTransforms[i].weight;

        float increment = 0;
        int j = 0;
        for(int i=0; i<numTransf; i++) {
            if(ifsTransforms[i].active) {
                activeIDX[j++] = i;
                increment += ifsTransforms[i].weight/totalWeight;
                ifsTransforms[i].startWeight = increment;
            }
        }
        totalActive = j;
    }

    int getCurrentTransform() { return getCurrentTransform(fastPrng64.xoroshiro128p_UNI<float>()); }

    int getCurrentTransform(float rndValue) {
        for(int i=0; i<totalActive; i++)
            if(ifsTransforms[activeIDX[i]].startWeight > rndValue)
                return activeIDX[i];
        return totalActive ? activeIDX[totalActive-1] : -1; // no active transforms, return -1
    }

    ifsDataStruct *getTransfStruct(int idx) { return &ifsTransforms[idx]; }

    bool active() { return isActive; }
    void active(bool b) { isActive = b; }

    bool dlgActive() { return isDlgActive; }
    void dlgActive(bool b)  { isDlgActive = b; }

    int getNumTransf() { return numTransf; }
    void setNumTransf(int i) {  numTransf = i; }

    int getTmpTransf() { return tmpTransf; }
    void setTmpTransf(int i) {  tmpTransf = i; }

    float getInitValue() { return initValue; }
private:
    std::vector<int> activeIDX;
    int totalActive = 1;

    std::vector<ifsDataStruct> ifsTransforms;
    int numTransf = 1, tmpTransf = 1;
    bool isActive = false;
    bool isDlgActive = false;

    float totalWeight = 1.0;
    const float initValue;

    friend class ifsDlgClass;
};