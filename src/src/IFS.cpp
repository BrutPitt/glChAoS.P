//------------------------------------------------------------------------------
//  Copyright (c) 2018-2024 Michele Morrone
//  All rights reserved.
//
//  https://michelemorrone.eu - https://glchaosp.com - https://brutpitt.com
//
//  X: https://x.com/BrutPitt - GitHub: https://github.com/BrutPitt
//
//  direct mail: brutpitt(at)gmail.com - me(at)michelemorrone.eu
//
//  This software is distributed under the terms of the BSD 2-Clause license
//------------------------------------------------------------------------------
#include "IFS.h"

const char *tranformsText = \
"Linear\0"\
"Swirl2D\0"\
"Swirl4D\0"\
"Sphere\0"\
"Polar3D\0"\
;
    

std::vector<std::function<vec4(const vec4 &)>> variationFuncsArray = {
    &variationFuncsClass::linear,
    &variationFuncsClass::swirl2D,
    &variationFuncsClass::swirl4D,
    &variationFuncsClass::spherical,
    &variationFuncsClass::polar3D
};
