////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2018-2020 Michele Morrone
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

template <class T> bool getVec_asArray(Config& c,const char* name,T& outV);

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
    GLfloat *ptr = theWnd->getParticlesSystem()->getEmitter()->getVertexBase()->getBuffer();
        //theWnd->getParticlesSystem()->getEmitter()->getVBO()->mapBuffer(nVtx);

    vec3 lum(0.299, 0.587, 0.114), col(1.f), pt;
    uint buffSize = theWnd->getParticlesSystem()->getEmitter()->getSizeStepBuffer();
    theWnd->getParticlesSystem()->getEmitter()->setSizeCircularBuffer(nVtx);
       
    int j=0;
    for(int i=0; i<nVtx; i++, j++) {

        if(j == buffSize) {
            theWnd->getParticlesSystem()->getEmitter()->getVertexBase()->uploadSubBuffer(buffSize,theWnd->getParticlesSystem()->getEmitter()->getSizeCircularBuffer());
            ptr = theWnd->getParticlesSystem()->getEmitter()->getVertexBase()->getBuffer();
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
        *ptr++ = uintBitsToFloat( iCol );
        //*ptr++ = dot(lum, col);

    }  
    theWnd->getParticlesSystem()->getEmitter()->getVertexBase()->uploadSubBuffer(j,theWnd->getParticlesSystem()->getEmitter()->getSizeCircularBuffer());

    
    //theWnd->getParticlesSystem()->getEmitter()->getVBO()->unmapBuffer(nVtx);

    return true;
}

bool importPLY(bool wantColors, int velType)
{
    attractorsList.getThreadStep()->stopThread();

    char const * patterns[] = { "*.ply" };           
    char const * fileName = theApp->openFile(theApp->getCapturePath().c_str(), patterns, 1);
    bool haveAlpha = true;

    if(fileName==nullptr) return false;

    try {
        std::ifstream ss(fileName, std::ios::binary);
        if (ss.fail()) throw std::runtime_error(fileName);
        
        PlyFile ply;
        ply.parse_header(ss);

        std::shared_ptr<PlyData> vertices, colors, alpha;

        try { vertices = ply.request_properties_from_element("vertex", { "x", "y", "z" }); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        try { colors = ply.request_properties_from_element("vertex", { "red", "green", "blue"}); }
        catch (const std::exception & e) { std::cerr << "tinyply exception: " << e.what() << std::endl; }

        try { alpha = ply.request_properties_from_element("vertex", { "alpha" }); }
        catch (const std::exception & e) { haveAlpha = false; }

        ply.read(ss);

        const uint nVtx = vertices->count;
        emitterBaseClass *e = theWnd->getParticlesSystem()->getEmitter();
        e->resetVBOindexes();
        
        if(attractorsList.continueDLA()) e->setSizeCircularBuffer(e->getSizeAllocatedBuffer());
        else                             e->setSizeCircularBuffer(nVtx);

        GLfloat *vtx, *fClr, *fAlpha = nullptr;
        uint8 *bClr, *bAlpha = nullptr;
        if (vertices->t == tinyply::Type::FLOAT32) {
            vtx = (GLfloat *)vertices->buffer.get();
        } else {
            cout << "Unsupported vertex format (only float)" << endl;
            return false;
        }

        if (colors->t == tinyply::Type::FLOAT32 || colors->t == tinyply::Type::UINT8) {
            bClr   = (uint8 *)   colors->buffer.get();
            fClr   = (GLfloat *) colors->buffer.get();
            if(haveAlpha) {
                bAlpha = (uint8 *)    alpha->buffer.get();
                fAlpha = (GLfloat *)  alpha->buffer.get();
            }
        } else {
            cout << "Unsupported color format (only byte or float)" << endl;
            return false;
        }

        
        vec4 *mappedBuffer;
        if(e->useMappedMem())   // USE_MAPPED_BUFFER
            mappedBuffer = (vec4 *) e->getVertexBase()->getBuffer();
        else
            mappedBuffer = new vec4[nVtx];

        vec4 *ptr = mappedBuffer;

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
                ptr->w = uintBitsToFloat( iCol );            
            } else {
                ptr->w = velType ? ((velType == 1 || velType == 2 && !haveAlpha) ? 
                                        length(vec3(*ptr)) : 
                                        (colors->t == tinyply::Type::FLOAT32 ? *fAlpha++ : float(*bAlpha++)/255.f)
                                    ) : ((i>0) ? distance(vec3(*ptr), vec3(*(ptr-1))) : 0.f);
                if(attractorsList.continueDLA()) ((dla3D *)attractorsList.get())->addLoadedPoint(vec3(*ptr));
            }

#if !defined(NDEBUG)
            if(!(i%100000))
                cout << i << endl;
#endif
        }

        e->getVertexBase()->setVertexCount(nVtx);
        if(attractorsList.continueDLA()) ((dla3D *)attractorsList.get())->buildIndex();

        if(!e->useMappedMem()) {   // !defined(USE_MAPPED_BUFFER)
#ifdef GLAPP_REQUIRE_OGL45
            glNamedBufferSubData(e->getVBO(), 0, nVtx * e->getVertexBase()->getBytesPerVertex(), (void *) mappedBuffer); 
#else
            glBindBuffer(GL_ARRAY_BUFFER, e->getVBO());
            glBufferSubData(GL_ARRAY_BUFFER, 0, nVtx * e->getVertexBase()->getBytesPerVertex(), (void *) mappedBuffer);
#endif
        }
        delete [] mappedBuffer;

    } catch (const std::exception & e) {
        std::cerr << "Caught tinyply exception: " << e.what() << std::endl;
    }

    return true;

}


uint8_t *getColorBuffer(vec4 *map, const uint32_t sizeBuff, bool alphaDist)
{
    particlesBaseClass *pSys = theWnd->getParticlesSystem()->getWhitchRenderMode()==RENDER_USE_BILLBOARD ? 
        (particlesBaseClass *) theWnd->getParticlesSystem()->shaderBillboardClass::getPtr() : 
        (particlesBaseClass *) theWnd->getParticlesSystem()->shaderPointClass::getPtr();

    uint8_t *clrBuff = new uint8_t[sizeBuff * (alphaDist ? 4:3)];
    uint32_t *palBuff = new uint32_t[256]; // RGBA*256 -> 256*4
    uint8_t *clr = clrBuff;

    //glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glBindTexture(GL_TEXTURE_2D, pSys->getCMSettings()->getModfTex());
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, palBuff);
    //glPixelStorei(GL_PACK_ALIGNMENT, 4);
//        glGetTextureImage(theWnd->getParticlesSystem()->shaderPointClass::getCMSettings()->getModfTex(),
//                          0, GL_RGB, GL_UNSIGNED_BYTE, 256, palBuff);

    if(pSys->wantPlyObjColor() && pSys->viewingObj()) { // packed color data: is loaded PLY
        for(unsigned i=sizeBuff; i>0; i--, map++) {
            const uint32_t c = floatBitsToUint(map->w);
            uint8_t *p = (uint8_t *) &c;
            *clr++ =  *p++;
            *clr++ =  *p++;
            *clr++ =  *p;
            if(alphaDist) *clr++ = *(++p);
        }
    } else { // color  = speed + palette
        const float vel = pSys->getCMSettings()->getVelIntensity();

        for(unsigned i=sizeBuff; i>0; i--, map++) {
            const int32_t offset = int(map->w*vel*255.f+.5);
            uint8_t *p = (uint8_t *)(palBuff + (offset>=255 ? 255 : (offset <= 0 ? 0 : offset)));
            *clr++ =  *p++;
            *clr++ =  *p++;
            *clr++ =  *p;
            if(alphaDist) *clr++ = map->w*255.f+.5;
        }
    }

    delete [] palBuff;

    return clrBuff;
}

vec3 *getVertexBuffer(vec4 *map, const uint32_t sizeBuff, bool bCoR)
{
    vec3 *vtxBuff = new vec3[sizeBuff];
    vec3 *vtx = vtxBuff;

    const vec3 CoR = theWnd->getParticlesSystem()->getTMat()->getTrackball().getRotationCenter();
    for(unsigned i=sizeBuff; i>0; i--, map++) *vtx++ = bCoR ? vec3(*map) + CoR : vec3(*map);

    return vtxBuff;
}

vec3 *getNormalBuffer(vec4 *map, const uint32_t sizeBuff, const bool isNormalized, normalType type)
{
    vec3 *nrmBuff = new vec3[sizeBuff];
    vec3 *nrm = nrmBuff;

    vec3 CoR = theWnd->getParticlesSystem()->getTMat()->getTrackball().getRotationCenter();

    if(type == normalType::ptCoR) {
        for(unsigned i=sizeBuff; i>0; i--) {
            const vec3 v = CoR+vec3(*map++);
            *nrm++ = isNormalized ? normalize(v) : v;
        }
    } else if(type == normalType::ptPt1) {
        for(unsigned i=sizeBuff-1; i>0; i--,map++) {
            const vec3 v = vec3(*(map+1) + *map);
            *nrm++ = isNormalized ? normalize(v) : v;
        }
        *nrm = isNormalized ? normalize(*map) : *map;
    } else { // normalType::ptPt1CoR
        for(unsigned i=sizeBuff-1; i>0; i--,map++) {
            const vec3 v = CoR+vec3(*(map+1) + *map);
            *nrm++ = isNormalized ? normalize(v) : v;
        }
        *nrm = isNormalized ? normalize(CoR+vec3(*map)) : CoR+vec3(*map);
    }

    return nrmBuff;
}

void exportPLY(bool wantBinary, bool wantColors, bool alphaDist, bool wantNormals, bool bCoR, bool wantNormalized, normalType nType)
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

        const uint32_t sizeBuff = e->getSizeCircularBuffer()>e->getVertexBase()->getVertexUploaded() ? 
                                  e->getVertexBase()->getVertexUploaded() : e->getSizeCircularBuffer();

        vec4 *mappedBuffer = nullptr;
        if(e->useMappedMem())   // USE_MAPPED_BUFFER
            mappedBuffer = (vec4 *) e->getVertexBase()->getBuffer();
        else {
            mappedBuffer = new vec4[sizeBuff];
#ifdef GLAPP_REQUIRE_OGL45
            glGetNamedBufferSubData(e->getVBO(), 0, sizeBuff * e->getVertexBase()->getBytesPerVertex(), (void *)mappedBuffer);
#else
            glBindBuffer(GL_ARRAY_BUFFER, e->getVBO());
            glGetBufferSubData(GL_ARRAY_BUFFER, 0, sizeBuff * e->getVertexBase()->getBytesPerVertex(), (void *)mappedBuffer);
#endif
        }

        vec3 *vtxBuff = getVertexBuffer(mappedBuffer, sizeBuff, bCoR);
        uint8_t *clrBuff = nullptr;
        vec3 *nrmBuff = nullptr;

        PlyFile ply;
        ply.add_properties_to_element("vertex", { "x", "y", "z" }, 
            Type::FLOAT32, sizeBuff, reinterpret_cast<uint8_t*>(vtxBuff), Type::INVALID, 0);

        if(wantColors) {
            clrBuff  = getColorBuffer(mappedBuffer, sizeBuff, alphaDist);
            ply.add_properties_to_element("vertex", alphaDist ? std::initializer_list<std::string> { "red", "green", "blue", "alpha" } : std::initializer_list<std::string> { "red", "green", "blue" }, 
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
    attractorsList.checkCorrectEmitter();
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
    vector<float> v((vVal.size()*4));

    memcpy(v.data(), vVal.data(), vVal.size()*sizeof(vec4));

    cfg["Name"] = getNameID();
    saveAdditionalData(cfg);
    cfg["kMax" ] = kMax;
    cfg["kMin" ] = kMin;
    cfg["vMax" ] = vMax;
    cfg["vMin" ] = vMin;
    cfg["vData4"] = Config::array(v);
    saveKVals(cfg);
}

void AttractorBase::loadVals(Config &cfg) 
{
    kMax = cfg.get_or("kMax", kMax);
    kMin = cfg.get_or("kMin", kMin);
    vMax = cfg.get_or("vMax", vMax);
    vMin = cfg.get_or("vMin", vMin);


    vector<float> v;
    if(cfg.has_key("vData")) {

        for (const Config& e : cfg["vData"].as_array()) v.push_back(e.as_float());

        const int vSize = v.size()/3;
        vVal.resize(vSize);

        for(int i=0, j=0; i<vSize; i++, j+=3) {
            vVal[i] = vec4(v[j], v[j+1], v[j+2], 0.f);
        }

    } else { //vdata4
        for (const Config& e : cfg["vData4"].as_array()) v.push_back(e.as_float());

        const int vSize = v.size()/4;
        vVal.resize(vSize);

        memcpy(vVal.data(), v.data(), vSize*sizeof(vec4));
    }

    loadAdditionalData(cfg);
    loadKVals(cfg);


    initStep();

}

//  Attractor with scalarK general files
////////////////////////////////////////////////////////////////////////////
void attractorScalarK::loadKVals(Config &cfg) 
{
    kVal.clear();
    const char *str = cfg.has_key("kData") ? "kData" : "kData4";
    for (const Config& e : cfg[str].as_array()) kVal.push_back(e.as_float());
}

void attractorScalarK::saveKVals(Config &cfg) 
{
    cfg["kData4"] = Config::array(kVal); 
}
//  Attractor with vectorK general files
////////////////////////////////////////////////////////////////////////////
void attractorVectorK::loadKVals(Config &cfg) 
{
    vector<float> k;


    if(cfg.has_key("kData")) {
        for (const Config& e : cfg["kData"].as_array()) k.push_back(e.as_float());

        const int kSize = k.size()/3;
        kVal.resize(kSize);

        for(int i=0, j=0; i<kSize; i++, j+=3) {
            kVal[i] = vec4(k[j], k[j+1], k[j+2], 0.f);
        }
    } else {
        for (const Config& e : cfg["kData4"].as_array()) k.push_back(e.as_float());

        const int kSize = k.size()/4;
        kVal.resize(kSize);
        memcpy(kVal.data(), k.data(), kSize*sizeof(vec4));
    }
}

void attractorVectorK::saveKVals(Config &cfg) 
{
vector<float>  k((kVal.size()*4));

    memcpy(k.data(), kVal.data(), kVal.size()*sizeof(vec4));

    cfg["kData4"] = Config::array(k); 
}

//  FractalIMMBase Attractor
////////////////////////////////////////////////////////////////////////////
void fractalIIMBase::saveAdditionalData(Config &cfg) 
{
    cfg["maxDepth" ] = maxDepth;
    cfg["fractDegreeN" ] = degreeN;
}

void fractalIIMBase::loadAdditionalData(Config &cfg) 
{
    maxDepth = cfg.get_or("maxDepth",    maxDepth);
    if(kVal.size()==4) kVal[3] = cfg.get_or("dim4D", kVal[3] );
    degreeN  = cfg.get_or("fractDegreeN",degreeN );
}

void BicomplexJExplorer::saveAdditionalData(Config &cfg) 
{
/*
    cfg["bjeIDX0" ] = idx0;
    cfg["bjeIDX1" ] = idx1;
    cfg["bjeIDX2" ] = idx2;
    cfg["bjeIDX3" ] = idx3;
*/
    fractalIIMBase::saveAdditionalData(cfg);
    std::vector<int> v{idx0, idx1, idx2, idx3};
    cfg["bjeIDX"        ] = Config::array(v);
}

void BicomplexJExplorer::loadAdditionalData(Config &cfg) 
{
/*
    idx0 = cfg.get_or("bjeIDX0", 4);
    idx1 = cfg.get_or("bjeIDX1", 5);
    idx2 = cfg.get_or("bjeIDX2", 6);
    idx3 = cfg.get_or("bjeIDX3", 7);
    //getVec_asArray
*/
    fractalIIMBase::loadAdditionalData(cfg);
    const char *name = "bjeIDX";
    idx0 = 4, idx1 = 5, idx2 = 6, idx3 = 7 ;
    //std::vector<int> v{ 4, 5, 6, 7};
    if(cfg.has_key(name)) {
        std::vector<int> v;
        for (const Config& e : cfg[name].as_array()) v.push_back(e.as_integer<int>());
        idx0 = v[0]; idx1 = v[1]; idx2 = v[2]; idx3 = v[3];
    }
    
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
        m_Stickiness   = cfg.get_or("attachChance",1.0);
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
