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
#include <fastRandom.h>
#include <vgMath.h>


extern const char *tranformsText;
struct ifsDataStruct
{
    vec4 variations = vec4(0.f);
    float weight = 1.f;
    float startWeight = 0; // [0, 1]
    int transfType = 0;
    bool active = true;
};

extern fstRnd::fFastRand32 fastRandom;

class ifsBaseClass
{
public:
    ifsBaseClass() { setSize(); }
    void setSize() { ifsTransforms.resize(numTransf); activeIDX.resize(numTransf); }

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

    int getCurrentTransform() { return getCurrentTransform(fastRandom.UNI()); }

    int getCurrentTransform(float rndValue) {
        for(int i=0; i<totalActive; i++)
            if(ifsTransforms[activeIDX[i]].startWeight > rndValue)
                return activeIDX[i];
        return activeIDX[totalActive ? totalActive-1 : 0];
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
private:
    std::vector<int> activeIDX;
    int totalActive = 1;

    std::vector<ifsDataStruct> ifsTransforms;
    int numTransf = 1, tmpTransf = 1;
    bool isActive = false;
    bool isDlgActive = false;

    float totalWeight = 1.0;

    friend class ifsDlgClass;
};