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
#include <glm/glm.hpp>
#include <vector>
#include <array>
#include <string>
#include "ParticlesUtils.h"
#include "libs/configuru/configuru.hpp"

using namespace std;
using namespace glm;
using namespace configuru;




typedef vector<float> CMap3;

/*
extern unsigned char palAsteria[];
extern unsigned char palNbvroy[];
extern unsigned char palMax__[];
extern unsigned char palCmyw[];
extern unsigned char palAzzurro[];
*/


extern CMap3 cm_viridis;
extern CMap3 cm_plasma ;
extern CMap3 cm_inferno;
extern CMap3 cm_magma  ;
extern CMap3 cm_parula ;
extern CMap3 cm_thermal;
/*


extern CMap3 cm_Algae  ;
extern CMap3 cm_Phase  ;
extern CMap3 cm_dense  ;
extern CMap3 cm_haline ;
extern CMap3 cm_ice    ;
extern CMap3 cm_jeans  ;
extern CMap3 cm_matter ;
extern CMap3 cm_solar  ;
extern CMap3 cm_speed  ;
extern CMap3 cm_tempo  ;
extern CMap3 cm_turbid ;
extern CMap3 cm_amp    ;
extern CMap3 cm_balance;
extern CMap3 cm_curl   ;
extern CMap3 cm_deep   ;
extern CMap3 cm_delta  ;
*/
#define BUILT_IN_STRING "BuiltIn"
class cmContainerClass 
{
public:

#define PB(ELEM) rgb.push_back(ELEM);\
                 name.push_back(#ELEM);\
                 type.push_back(BUILT_IN_STRING);

    cmContainerClass() {
        //rgb.resize(50);
        if(!loadColorMaps("colorMaps/palettes.json")) {
            PB(cm_viridis);
            PB(cm_parula );
            PB(cm_plasma );
            PB(cm_thermal);
            PB(cm_inferno);
            PB(cm_magma  );
        }
#undef PB
/*
        PUSH_BACK(cm_Algae  );
        PUSH_BACK(cm_Phase  );
        PUSH_BACK(cm_dense  );
        PUSH_BACK(cm_haline );
        PUSH_BACK(cm_ice    );
        PUSH_BACK(cm_jeans  );
        PUSH_BACK(cm_matter );
        PUSH_BACK(cm_solar  );
        PUSH_BACK(cm_speed  );
        PUSH_BACK(cm_tempo  );
        PUSH_BACK(cm_turbid );
        PUSH_BACK(cm_amp    );
        PUSH_BACK(cm_balance);
        PUSH_BACK(cm_curl   );
        PUSH_BACK(cm_deep   );
        PUSH_BACK(cm_delta  );
*/
        
    }
    ~cmContainerClass() {

    }

    const char *getName(int i) { return name.at(i).c_str(); }
    CMap3& getRGB_CMap3(int i) { return rgb.at(i); }
    float *getRGB_pf3(int i)   { return rgb.at(i).data(); }
    vec3 *getRGB_pv3(int i)    { return (vec3 *)rgb.at(i).data(); }

    int elements()             { return rgb.size(); }

    int checkExistingName(const std::string &s)
    {
        int j = 0;
        for(auto &i : name) {
            if(i == s) return j;
            j++;
        }
        return -1;
    }

    int addNewPal(Config& c)  {
        if(c.has_key("Type") && c.has_key("Name") && c.has_key("rgbData")) {
            std::string s = c.get_or("Name", "noName" );
            const int idxIfExist = checkExistingName(s);
            if( idxIfExist >= 0) return idxIfExist; // if exist return idx

            type.push_back(c.get_or("Type", "noName" ));
            name.push_back(s);
            rgb.emplace_back(CMap3());
            for (const Config& i : c["rgbData"].as_array())
                rgb.back().push_back(i.as_float());        
            return rgb.size()-1;    // if loaded return last idx
        }
        return 0; // if no palette in file... select first.
        
    }

    void deleteCMap(int i)
    {
        if(rgb.size()>1) {
            rgb.erase(rgb.begin()+i);
            name.erase(name.begin()+i);
            type.erase(type.begin()+i);
        } 
    }


    void importPalette(const char *filename)
    {
        Config cfg = configuru::parse_file(filename, JSON);
        if(cfg["RenderMode0"].has_key("Palette")) {
            addNewPal(cfg["RenderMode0"]["Palette"]);
        }
        if(cfg["RenderMode1"].has_key("Palette")) {
            addNewPal(cfg["RenderMode1"]["Palette"]);
        }
    }

    bool loadColorMaps(const char *filename)  {
        FILE* fp = fopen(filename, "rb");
        if(fp==nullptr) return false;
        else fclose(fp);
        Config cfg = configuru::parse_file(filename, JSON);
        //int vSize  = rgb.size();
        if(cfg.is_array()) {
            for (const Config& element : cfg.as_array()) {            
                if(element.has_key("Type") && element.has_key("Name") && element.has_key("rgbData")) {
                    for (auto& p : element.as_object()) {
                        if((std::string)p.key() == "Type") type.push_back((std::string)p.value());
                        else if((std::string)p.key() == "Name") name.push_back((std::string)p.value());
                        else if((std::string)p.key() == "rgbData" && p.value().is_array()) {
                            rgb.emplace_back(CMap3());
	                        for (const Config& element : p.value().as_array()) {
                                rgb.back().push_back(element.as_float());
                            }
                        }
                    }  
                }
            }
        } else {
            addNewPal(cfg);
        }
        return true;


    }

public:
    vector<CMap3> rgb;
    vector<string> name;
    vector<string> type;
};


