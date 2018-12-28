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
#include "glWindow.h"

#include "attractorsBase.h"

bool loadObjFile() 
{  
    //string line;         
    attractorsList.getThreadStep()->stopThread();
    char const * patterns[] = { "*.obj", "*.sca" };           
    char const * fileName = theApp->openFile(nullptr, patterns, 2);

    if(fileName==nullptr) return false;

    ifstream ifs(fileName);
    ifs.precision(5);
        
    GLuint nVtx;
    ifs >> nVtx;

    theWnd->getParticlesSystem()->getEmitter()->resetVBOindexes();
    GLfloat *ptr = theWnd->getParticlesSystem()->getEmitter()->getVBO()->getBuffer();
        //theWnd->getParticlesSystem()->getEmitter()->getVBO()->mapBuffer(nVtx);

    vec3 lum(0.299, 0.587, 0.114), col(1.f), pt;
    uint buffSize = theWnd->getParticlesSystem()->getEmitter()->getSizeStepBuffer();
    theWnd->getParticlesSystem()->getEmitter()->setSizeCircularBuffer(nVtx);
       
    int j=0;
    for(int i=0; i<nVtx; i++, j++) {

        if(j == buffSize) {
            theWnd->getParticlesSystem()->getEmitter()->getVBO()->uploadSubBuffer(buffSize,theWnd->getParticlesSystem()->getEmitter()->getSizeCircularBuffer());
            ptr = theWnd->getParticlesSystem()->getEmitter()->getVBO()->getBuffer();
            j=0;
        }

        ifs >> pt.x >> pt.y >> pt.z >>  col.r >> col.g >> col.b;

        if(!(i%100000))
            cout << i << endl;

        *ptr++ = pt.x;
        *ptr++ = pt.y;
        *ptr++ = pt.z;

        const uint iCol = 0xff000000 | (uint(col.b*255.f) << 16) | (uint(col.g*255.f) << 8) | uint(col.r*255.f);
        *ptr++ = glm::uintBitsToFloat( iCol );
        //*ptr++ = glm::dot(lum, col);

    }  
    theWnd->getParticlesSystem()->getEmitter()->getVBO()->uploadSubBuffer(j,theWnd->getParticlesSystem()->getEmitter()->getSizeCircularBuffer());

    
    //theWnd->getParticlesSystem()->getEmitter()->getVBO()->unmapBuffer(nVtx);

    return true;
}



bool loadAttractorFile(bool fileImport, const char *file)  
{

    //theWnd->getParticlesSystem()->getEmitter()->setEmitterOff();
    attractorsList.getThreadStep()->stopThread();

    char const * patterns[] = { "*.chatt", "*.sca" };           
    char const * fileName = (file == nullptr) ? 
                            theApp->openFile(theApp->getLastFile().size() ? theApp->getLastFile().c_str() : STRATT_PATH, patterns, 2) :
                            file;

    if(fileName==nullptr) {
        theWnd->getParticlesSystem()->getEmitter()->setEmitterOn();
        return false;
    }

    theApp->setLastFile(fileName);

    if(fileImport) {
        attractorsList.loadFile(fileName); 
    }
    else  {
        if(theApp->loadAttractor(fileName))
            attractorsList.setFileName(fileName);

                
    }
    attractorsList.getThreadStep()->restartEmitter();
    attractorsList.get()->initStep();
    attractorsList.getThreadStep()->startThread();

    //theWnd->getParticlesSystem()->getEmitter()->setEmitterOn();

    return true;
}

void saveAttractorFile(bool fileExport)  
{

    attractorsList.getThreadStep()->stopThread();

    char const * patterns[] = { "*.chatt", "*.sca" };        
    char const * fileName = theApp->saveFile(theApp->getLastFile().size() ? theApp->getLastFile().c_str() : STRATT_PATH, patterns, 2);

    if(fileName!=nullptr) {
    
        theApp->setLastFile(fileName);

        if(fileExport) { 
            attractorsList.saveFile(fileName); 
            //attractorsList.get()->saveVals(fileName);
        }
        else {
            theApp->saveAttractor(fileName);
        }
    }

    attractorsList.getThreadStep()->restartEmitter();
    attractorsList.get()->initStep();
    attractorsList.getThreadStep()->startThread();
}


//  Attractor with scalarK general files
////////////////////////////////////////////////////////////////////////////

void AttractorBase::saveVals(Config &cfg) 
{
    vector<float> v((vVal.size()*3));

    memcpy(v.data(), vVal.data(), vVal.size()*sizeof(glm::vec3));

    cfg["Name"] = getNameID();
    saveAdditionalData(cfg);
    cfg["kMax" ] = kMax;
    cfg["kMin" ] = kMin;
    cfg["vMax" ] = vMax;
    cfg["vMin" ] = vMin;
    cfg["vData"] = Config::array(v);
    saveKVals(cfg);
}

void AttractorBase::loadVals(Config &cfg) 
{
    kMax = cfg.get_or("kMax", kMax);
    kMin = cfg.get_or("kMin", kMin);
    vMax = cfg.get_or("vMax", vMax);
    vMin = cfg.get_or("vMin", vMin);

    loadAdditionalData(cfg);

    vector<float> v;
    for (const Config& e : cfg["vData"].as_array()) v.push_back(e.as_float());

    const int vSize = v.size()/3;
    vVal.resize(vSize);

    memcpy(vVal.data(), v.data(), vSize*sizeof(glm::vec3));

    loadKVals(cfg);

    initStep();

}

//  Attractor with scalarK general files
////////////////////////////////////////////////////////////////////////////
void attractorScalarK::loadKVals(Config &cfg) 
{
    kVal.clear();
    for (const Config& e : cfg["kData"].as_array()) kVal.push_back(e.as_float());
    std::cout << kVal.size() << endl;
}

void attractorScalarK::saveKVals(Config &cfg) 
{
    cfg["kData"] = Config::array(kVal); 
}
//  Attractor with vectorK general files
////////////////////////////////////////////////////////////////////////////
void attractorVectorK::loadKVals(Config &cfg) 
{
    vector<float> k;

    for (const Config& e : cfg["kData"].as_array()) k.push_back(e.as_float());

    const int kSize = k.size()/3;

    kVal.resize(kSize);

    memcpy(kVal.data(), k.data(), kSize*sizeof(glm::vec3));

}

void attractorVectorK::saveKVals(Config &cfg) 
{
vector<float>  k((kVal.size()*3));

    memcpy(k.data(), kVal.data(), kVal.size()*sizeof(glm::vec3));

    cfg["kData"] = Config::array(k); 
}

//  FractalIMMBase Attractor
////////////////////////////////////////////////////////////////////////////
void fractalIIMBase::saveAdditionalData(Config &cfg) 
{
    cfg["maxDepth" ] = maxDepth;
    cfg["dim4D" ] = dim4D;
    cfg["fractDegreeN" ] = degreeN;
}

void fractalIIMBase::loadAdditionalData(Config &cfg) 
{
    maxDepth = cfg.get_or("maxDepth",    maxDepth);
    dim4D    = cfg.get_or("dim4D",       dim4D   );
    degreeN  = cfg.get_or("fractDegreeN",degreeN );
}


//  PowerN3D Attractor
////////////////////////////////////////////////////////////////////////////
void PowerN3D::saveAdditionalData(Config &cfg) 
{
    cfg["Order" ] = order;
}

void PowerN3D::loadAdditionalData(Config &cfg) 
{
    tmpOrder = order= cfg.get_or("Order",order);
/*
    nCoeff = getNumCoeff();
    const int kSize = k.size()/3;

    if(nCoeff!=kSize || v.size()!=3) assert("mismatch loaded size!!");

    resizeBuffers();
    ResetCurrent();

    memcpy(vVal.data(), v.data(), sizeof(glm::vec3));
    memcpy(kVal.data(), k.data(), kSize*sizeof(glm::vec3));
*/
}

void PowerN3D::saveVals(const char *name) 
{
    ofstream ofs(name);
    ofs.precision(15);

    const int nMagnets = kVal.size();

    ofs << order << endl;
    ofs << nMagnets << endl;

    ofs <<  vVal[0].x << " " << vVal[0].y << " " << vVal[0].z << endl;
        
    for(int i=0; i<nMagnets; i++) {
        ofs <<  kVal[i].x << " " << kVal[i].y << " " << kVal[i].z << endl;
    }
    cout << endl;
}
void PowerN3D::loadVals(const char *name) 
{
    ifstream ifs(name);
    ifs.precision(15);
        
    ifs >> order;
    //nCoeff = getNumCoeff();
    ifs >> nCoeff;

    resetData();
    resetQueue();


    ifs >>  vVal[0].x >> vVal[0].y >> vVal[0].z;
        
    for(int i=0; i<nCoeff; i++) {
        ifs >>  kVal[i].x >> kVal[i].y >> kVal[i].z;
    }  

    
}


void attractorDtType::saveAdditionalData(Config &cfg)
{
        cfg["dtInc"] = dtStepInc;
}
void attractorDtType::loadAdditionalData(Config &cfg) 
{
    dtStepInc = cfg.get_or("dtInc",dtStepInc);


}






//  Magnetic Attractor
////////////////////////////////////////////////////////////////////////////
void Magnetic::saveAdditionalData(Config &cfg) 
{
        cfg["nMagnets"] = vVal.size();    
}
void Magnetic::loadAdditionalData(Config &cfg) 
{
        tmpElements = nElements = cfg.get_or("nMagnets",2);
}

void loadAdditionalData(Config &cfg) {};

void Magnetic::saveVals(const char *name) 
{
    ofstream ofs(name);
    ofs.precision(15);

    const int nMagnets = vVal.size();

    ofs << nMagnets << endl;
        
    for(int i=0; i<nMagnets; i++) {
        ofs <<  kVal[i].x << " " << kVal[i].y << " " << kVal[i].z << endl;
        ofs <<  vVal[i].x << " " << vVal[i].y << " " << vVal[i].z << endl;
    }
    cout << endl;
}


void Magnetic::loadVals(const char *name) 
{  
    //string line;         

    ifstream ifs(name);
    ifs.precision(15);
        
    int nMagnets;;
    ifs >> nMagnets;

    resetQueue();
    //ResizeVectors();        
    kVal.resize(nMagnets);
    vVal.resize(nMagnets);       

        
    for(int i=0; i<nMagnets; i++) {
        ifs >>  kVal[i].x >> kVal[i].y >> kVal[i].z;
        ifs >>  vVal[i].x >> vVal[i].y >> vVal[i].z;
    }  

    
}

//  Attractor Continer Class
////////////////////////////////////////////////////////////////////////////
bool AttractorsClass::loadVals(Config &cfg)
{
    auto& c = cfg["Attractor"];
    if(c.has_key("Name")) {
        int i = getSelectionByName((std::string)c.get_or("Name",""));
        if(i>=0) {
            selected = i;
            get()->loadVals(c);            
            theWnd->getParticlesSystem()->getTMat()->setView(get()->getPOV(),get()->getTGT());
            restart();
            return true;
            // NED INIT
        }
        return true;
    } 
    return false;
}

void AttractorsClass::saveFile(const char *name) 
{
    Config cfg = Config::object();

    saveVals(cfg);

    configuru::dump_file(name, cfg, configuru::JSON);
}
     
bool AttractorsClass::loadFile(const char *name) 
{
    Config cfg = configuru::parse_file(name, JSON);
    return loadVals(cfg);
}

void AttractorsClass::saveVals(Config &cfg)
{
    auto &a = cfg["Attractor"] = Config::object();
    get()->saveVals(a);

}
