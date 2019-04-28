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
#include "glWindow.h"

#include "attractorsBase.h"

#if !defined(GLCHAOSP_LIGHTVER)
#define TINYPLY_IMPLEMENTATION
#include <tinyPLY/tinyply.h>

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

#if !defined(NDEBUG)
        if(!(i%100000))
            cout << i << endl;
#endif
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

bool importPLY(bool wantColors, bool isDLA)
{
    attractorsList.getThreadStep()->stopThread();

    char const * patterns[] = { "*.ply" };           
    char const * fileName = theApp->openFile(nullptr, patterns, 1);

    if(fileName==nullptr) return false;

    try {
        std::ifstream ss(fileName, std::ios::binary);
        if (ss.fail()) throw std::runtime_error(fileName);
        
        PlyFile ply;
        ply.parse_header(ss);

        std::shared_ptr<PlyData> vertices, colors;

        try { vertices = ply.request_properties_from_element("vertex", { "x", "y", "z" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        try { colors = ply.request_properties_from_element("vertex", { "red", "green", "blue" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        ply.read(ss);

        const uint nVtx = vertices->count;
        emitterBaseClass *e = theWnd->getParticlesSystem()->getEmitter();
        e->resetVBOindexes();
        
        if(attractorsList.continueDLA()) e->setSizeCircularBuffer(e->getSizeAllocatedBuffer());
        else                             e->setSizeCircularBuffer(nVtx);

        GLfloat *vtx, *fClr;
        uint8 *bClr;
        if (vertices->t == tinyply::Type::FLOAT32) {
            vtx = (GLfloat *)vertices->buffer.get();
        } else {
            cout << "Unsupported vertex format (only float)" << endl;
            return false;
        }

        if (colors->t == tinyply::Type::FLOAT32 || colors->t == tinyply::Type::UINT8) {
            bClr = (uint8 *)   colors->buffer.get();
            fClr = (GLfloat *) colors->buffer.get();
        } else {
            cout << "Unsupported color format (only byte or float)" << endl;
            return false;
        }

        
        glm::vec4 *mappedBuffer;
        if(e->useMappedMem())   // USE_MAPPED_BUFFER
            mappedBuffer = (glm::vec4 *) e->getVBO()->getBuffer();
        else
            mappedBuffer = new glm::vec4[nVtx];

        glm::vec4 *ptr = mappedBuffer;

        uint iCol;
        for(int i=0; i<nVtx; i++, ptr++) {

            ptr->x = *vtx++; // for portability;
            ptr->y = *vtx++;
            ptr->z = *vtx++;

            if(wantColors) {
                if (colors->t == tinyply::Type::FLOAT32) {
                    const float r = *fClr++;
                    const float g = *fClr++;
                    const float b = *fClr++;
                    iCol = 0xff000000 | (uint(b*255.f) << 16) | (uint(g*255.f) << 8) | uint(r*255.f);
                } else {
                    const uint r = *bClr++;
                    const uint g = *bClr++;
                    const uint b = *bClr++;
                    iCol = 0xff000000 | (b << 16) | (g << 8) | r;
                }         
                ptr->w = glm::uintBitsToFloat( iCol );            
            } else {
                ptr->w = isDLA ? glm::length(vec3(*ptr)) : ((i>0) ? glm::distance(vec3(*ptr), vec3(*(ptr-1))) : 0.f);
                if(attractorsList.continueDLA()) ((dla3D *)attractorsList.get())->addLoadedPoint(vec3(*ptr));
            }

#if !defined(NDEBUG)
            if(!(i%100000))
                cout << i << endl;
#endif
        }

        e->getVBO()->setVertexCount(nVtx);
        if(attractorsList.continueDLA()) ((dla3D *)attractorsList.get())->buildIndex();

        if(!e->useMappedMem()) {   // !defined(USE_MAPPED_BUFFER)
#ifdef GLAPP_REQUIRE_OGL45
            glNamedBufferSubData(e->getVBO()->getVBO(), 0, nVtx * e->getVBO()->getBytesPerVertex(), (void *) mappedBuffer); 
#else
            glBindBuffer(GL_ARRAY_BUFFER, e->getVBO()->getVBO());
            glBufferSubData(GL_ARRAY_BUFFER, 0, nVtx * e->getVBO()->getBytesPerVertex(), (void *) mappedBuffer);
#endif
        }
        delete [] mappedBuffer;

    } catch (const std::exception & e) {
        std::cerr << "Caught tinyply exception: " << e.what() << std::endl;
    }

    return true;

}


uint8_t *getColorBuffer(glm::vec4 *map, const uint32_t sizeBuff)
{
    particlesBaseClass *pSys = theWnd->getParticlesSystem()->getWhitchRenderMode()==RENDER_USE_BILLBOARD ? 
        (particlesBaseClass *) theWnd->getParticlesSystem()->shaderBillboardClass::getPtr() : 
        (particlesBaseClass *) theWnd->getParticlesSystem()->shaderPointClass::getPtr();

    uint8_t *clrBuff = new uint8_t[sizeBuff*3];
    uint32_t *palBuff = new uint32_t[256]; // RGBA*256 -> 256*4
    uint8_t *clr = clrBuff;

    //glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, pSys->getCMSettings()->getModfTex());
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, palBuff);
    //glPixelStorei(GL_PACK_ALIGNMENT, 4);
//        glGetTextureImage(theWnd->getParticlesSystem()->shaderPointClass::getCMSettings()->getModfTex(),
//                          0, GL_RGB, GL_UNSIGNED_BYTE, 256, palBuff);

    const float vel = pSys->getCMSettings()->getVelIntensity();

    for(unsigned i=sizeBuff; i>0; i--, map++) {
        const int32_t offset = int(map->w*vel*255.f+.5);
        uint8_t *p = (uint8_t *)(palBuff + (offset>255 ? 255 : (offset <= 0 ? 0 : offset)));
        *clr++ =  *p++;
        *clr++ =  *p++;
        *clr++ =  *p;

/*
        // to import old OBJ saved -> above func loadOBJFile()

        uint packCol = glm::floatBitsToUint(map->w);
        glm::vec4 col = glm::unpackUnorm4x8(packCol) * 255.f;
        //emitterWasOn, glm::vec4(0.0), glm::vec4(1.0))*255.f;
        *clr++ = col.x;
        *clr++ = col.y;
        *clr++ = col.z;
*/

    }

    delete [] palBuff;

    return clrBuff;
}

glm::vec3 *getVertexBuffer(glm::vec4 *map, const uint32_t sizeBuff, bool bCoR)
{
    glm::vec3 *vtxBuff = new glm::vec3[sizeBuff];
    glm::vec3 *vtx = vtxBuff;

    const glm::vec3 CoR = theWnd->getParticlesSystem()->getTMat()->getTrackball().getRotationCenter();
    for(unsigned i=sizeBuff; i>0; i--, map++) *vtx++ = bCoR ? vec3(*map) + CoR : vec3(*map);

    return vtxBuff;
}

glm::vec3 *getNormalBuffer(glm::vec4 *map, const uint32_t sizeBuff, const bool isNormalized, normalType type)
{
    glm::vec3 *nrmBuff = new glm::vec3[sizeBuff];
    glm::vec3 *nrm = nrmBuff;

    glm::vec3 CoR = theWnd->getParticlesSystem()->getTMat()->getTrackball().getRotationCenter();

    if(type == normalType::ptCoR) {
        for(unsigned i=sizeBuff; i>0; i--) {
            const glm::vec3 v = CoR+glm::vec3(*map++);
            *nrm++ = isNormalized ? glm::normalize(v) : v;
        }
    } else if(type == normalType::ptPt1) {
        for(unsigned i=sizeBuff-1; i>0; i--,map++) {
            const glm::vec3 v = glm::vec3(*(map+1) + *map);
            *nrm++ = isNormalized ? glm::normalize(v) : v;
        }
        *nrm = isNormalized ? glm::normalize(*map) : *map;
    } else { // normalType::ptPt1CoR
        for(unsigned i=sizeBuff-1; i>0; i--,map++) {
            const glm::vec3 v = CoR+glm::vec3(*(map+1) + *map);
            *nrm++ = isNormalized ? glm::normalize(v) : v;
        }
        *nrm = isNormalized ? glm::normalize(CoR+glm::vec3(*map)) : CoR+glm::vec3(*map);
    }

    return nrmBuff;
}

void exportPLY(bool wantBinary, bool wantColors, bool wantNormals, bool bCoR, bool wantNormalized, normalType nType)
{
    
    emitterBaseClass *e = theWnd->getParticlesSystem()->getEmitter();
    
    bool emitterWasOn = e->isEmitterOn();
    attractorsList.getThreadStep()->stopThread();

    char const * patterns[] = { "*.ply" };        
    const char *name = theApp->saveFile(theApp->getCapturePath().c_str(), patterns, 1);
    std::string filename = name != nullptr ? name : "";    

    if(filename.length()>0) {


        std::filebuf fbOut;
	    fbOut.open(filename, wantBinary ? std::ios::out | std::ios::binary : std::ios::out);
        std::ostream os(&fbOut);
	    if (os.fail()) throw std::runtime_error("failed to open " + filename);

        const uint32_t sizeBuff = e->getSizeCircularBuffer()>e->getVBO()->getVertexUploaded() ? 
                                  e->getVBO()->getVertexUploaded() : e->getSizeCircularBuffer();

        glm::vec4 *mappedBuffer = nullptr;
        if(e->useMappedMem())   // USE_MAPPED_BUFFER
            mappedBuffer = (glm::vec4 *) e->getVBO()->getBuffer();
        else {
            mappedBuffer = new glm::vec4[sizeBuff];
#ifdef GLAPP_REQUIRE_OGL45
            glGetNamedBufferSubData(e->getVBO()->getVBO(), 0, sizeBuff * e->getVBO()->getBytesPerVertex(), (void *)mappedBuffer);
#else
            glBindBuffer(GL_ARRAY_BUFFER, e->getVBO()->getVBO());
            glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeBuff * e->getVBO()->getBytesPerVertex(), (void *)mappedBuffer);
#endif
        }

        glm::vec3 *vtxBuff = getVertexBuffer(mappedBuffer, sizeBuff, bCoR);
        uint8_t *clrBuff = nullptr;
        glm::vec3 *nrmBuff = nullptr;

        PlyFile ply;
        ply.add_properties_to_element("vertex", { "x", "y", "z" }, 
        Type::FLOAT32, sizeBuff, reinterpret_cast<uint8_t*>(vtxBuff), Type::INVALID, 0);

        if(wantColors) {
            clrBuff  = getColorBuffer(mappedBuffer, sizeBuff);
            ply.add_properties_to_element("vertex", { "red", "green", "blue" }, 
            Type::UINT8, sizeBuff, reinterpret_cast<uint8_t*>(clrBuff), Type::INVALID, 0);
        }

        if(wantNormals) {
            nrmBuff = getNormalBuffer(mappedBuffer, sizeBuff, wantNormalized, nType);
            ply.add_properties_to_element("vertex", { "nx", "ny", "nz" },
            Type::FLOAT32, sizeBuff, reinterpret_cast<uint8_t*>(nrmBuff), Type::INVALID, 0);

        }

        ply.get_comments().push_back("generated by glChAoS.P");

        ply.write(os, wantBinary);

        if(!e->useMappedMem())  // ! USE_MAPPED_BUFFER
            delete [] mappedBuffer;

        delete [] vtxBuff;
        delete [] clrBuff;
        delete [] nrmBuff;
    }


    if(emitterWasOn) attractorsList.getThreadStep()->startThread();
}

#endif


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

}


void attractorDtType::saveAdditionalData(Config &cfg)
{
        cfg["dtInc"] = dtStepInc;
}
void attractorDtType::loadAdditionalData(Config &cfg) 
{
    dtStepInc = cfg.get_or("dtInc",dtStepInc);


}

#if !defined(GLAPP_DISABLE_DLA)
//  DLA 3D
////////////////////////////////////////////////////////////////////////////
void dla3D::saveAdditionalData(Config &cfg) 
{
        cfg["Stubbornness"] = m_Stubbornness;
        cfg["attachChance"] = m_Stickiness;
}
void dla3D::loadAdditionalData(Config &cfg) 
{
        m_Stubbornness = cfg.get_or("Stubbornness",0);
        m_Stickiness   = cfg.get_or("attachChance",1);
}
#endif

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
