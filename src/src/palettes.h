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
#include <array>
#include <string>

#include <vGizmoMath.h>

#include "ParticlesUtils.h"
#include "libs/configuru/configuru.hpp"

using namespace std;
using namespace configuru;

typedef vector<float> CMap3;

extern CMap3 cm_viridis;
extern CMap3 cm_plasma ;
extern CMap3 cm_inferno;
extern CMap3 cm_magma  ;
extern CMap3 cm_parula ;
extern CMap3 cm_thermal;

#define BUILT_IN_STRING "BuiltIn"
class cmContainerClass 
{
public:

#define PB(ELEM) rgb.push_back(ELEM);\
                 name.push_back(#ELEM);\
                 type.push_back(BUILT_IN_STRING);

    cmContainerClass() {
        if(!loadColorMaps("colorMaps/palettes.json")) {
            PB(cm_viridis);
            PB(cm_parula );
            PB(cm_plasma );
            PB(cm_thermal);
            PB(cm_inferno);
            PB(cm_magma  );
        }
#undef PB
        
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

        ifstream input(filename,std::ifstream::binary);
        if (!input.is_open()) return false;
        else input.close();

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


