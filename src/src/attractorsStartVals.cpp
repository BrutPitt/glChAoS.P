//------------------------------------------------------------------------------
//  Copyright (c) 2018-2019 Michele Morrone
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
#include "attractorsBase.h"


bool loadAttractorFile(bool fileImport = false, const char *inFileName = nullptr);

void Magnetic::startData()
{
    kVal.push_back(vec3(0.005833899, 0.0253584, 0.01462666  ));
    vVal.push_back(vec3(0.5850307, -0.1643422, 0.6242561    ));
    kVal.push_back(vec3(0.02646308, 0.01038993, 0.02675405  ));
    vVal.push_back(vec3(0.1452986, -0.1467635, 0.09537029   ));
    kVal.push_back(vec3(0.02040754, 0.01961615, 0.01763512  ));
    vVal.push_back(vec3(-0.001556456, 0.7105014, 0.3481246  ));
    kVal.push_back(vec3(0.02710425, 0.01345676, 0.01689266  ));
    vVal.push_back(vec3(0.5499741, -0.399823, 0.01724291    ));
    kVal.push_back(vec3(0.01120706, 0.004694033, 0.007236416));
    vVal.push_back(vec3(-0.8842128, -0.1210669, 0.9827876   ));
    kVal.push_back(vec3(0.01336062, 0.01125256, 0.02044445  ));
    vVal.push_back(vec3(0.5339824, -0.6103396, 0.5201269    ));
    kVal.push_back(vec3(0.01157271, 0.01199158, 0.004106933 ));
    vVal.push_back(vec3(-0.8356273, 0.6260262, -0.8093203   ));
    kVal.push_back(vec3(0.02272246, 0.02665277, 0.01643174  ));
    vVal.push_back(vec3(-0.5333109, 0.8077945, 0.2831202    ));
    kVal.push_back(vec3(0.02304262, 0.02674718, 0.01947968  ));
    vVal.push_back(vec3(0.950621, -0.2300791, -0.6631978    ));
    kVal.push_back(vec3(0.0203423, 0.004762699, 0.003132725 ));
    vVal.push_back(vec3(0.6411633, -0.8827479, -0.9014252   ));
    kVal.push_back(vec3(0.02126244, 0.022392, 0.0284587     ));
    vVal.push_back(vec3(0.642262, 0.6176641, 0.4200873      ));
    kVal.push_back(vec3(0.02781581, 0.003200533, 0.01317694 ));
    vVal.push_back(vec3(0.1664785, 0.5431989, -0.6400037    ));
    kVal.push_back(vec3(0.01140105, 0.02528802, 0.00820719  ));
    vVal.push_back(vec3(0.7803278, 0.877926, -0.6476943     ));
    kVal.push_back(vec3(0.02326321, 0.02735059, 0.003216842 ));
    vVal.push_back(vec3(0.8328806, -0.7654958, 0.9913938    ));
    kVal.push_back(vec3(0.01615364, 0.005050241, 0.01552534 ));
    vVal.push_back(vec3(0.3567919, 0.2183599, -0.9772942    ));
    kVal.push_back(vec3(0.009130756, 0.01596566, 0.02162379 ));
    vVal.push_back(vec3(-0.6134526, -0.9107639, 0.7554246   ));
    kVal.push_back(vec3(0.006016724, 0.01849431, 0.01112552 ));
    vVal.push_back(vec3(0.896237, -0.8448439, 0.5842769     ));
    kVal.push_back(vec3(0.01126457, 0.0184368, 0.03103713   ));
    vVal.push_back(vec3(0.6530045, 0.6302987, -0.1214331    ));
    kVal.push_back(vec3(0.02056461, 0.008661249, 0.0238958  ));
    vVal.push_back(vec3(0.4791712, -0.03793448, 0.4012879   ));
    kVal.push_back(vec3(0.01190231, 0.02844754, 0.0142035   ));
    vVal.push_back(vec3(0.9444563, 0.1188695, 0.9297464     ));
    kVal.push_back(vec3(0.01225166, 0.02446488, 0.02494726  ));
    vVal.push_back(vec3(0.6443373, -0.7141026, -0.9063692   ));
    kVal.push_back(vec3(0.03016507, 0.02160062, 0.02120064  ));
    vVal.push_back(vec3(0.1907102, -0.9048433, -0.1100192   ));
    kVal.push_back(vec3(0.02713429, 0.009274098, 0.02035175 ));
    vVal.push_back(vec3(0.1067232, -0.8424024, -0.8830531   ));
    kVal.push_back(vec3(0.0151614, 0.02738149, 0.0270888    ));
    vVal.push_back(vec3(-0.7700125, -0.4693441, 0.3517869   ));
    kVal.push_back(vec3(0.02900203, 0.00837199, 0.01363529  ));
    vVal.push_back(vec3(0.2304453, 0.08438373, 0.3148595    ));
    kVal.push_back(vec3(0.0184368, 0.02362285, 0.01748148   ));
    vVal.push_back(vec3(-0.3026521, 0.782464, -0.2156743    ));
    kVal.push_back(vec3(0.009456923, 0.02795915, 0.009407998));
    vVal.push_back(vec3(0.1923581, 0.9119235, -0.1184424    ));
    kVal.push_back(vec3(0.02962432, 0.01824368, 0.03043887  ));
    vVal.push_back(vec3(0.9481186, -0.4649495, 0.8029726    ));
    kVal.push_back(vec3(0.02793512, 0.03001743, 0.01859903  ));
    vVal.push_back(vec3(0.7746513, -0.856685, 0.1725211     ));
    kVal.push_back(vec3(0.02638497, 0.02205382, 0.009672365 ));
    vVal.push_back(vec3(-0.5459456, 0.9186987, -0.6205939   ));
    kVal.push_back(vec3(0.008548806, 0.02698408, 0.01166112 ));
    vVal.push_back(vec3(0.7243568, 0.403119, 0.7996765      ));
    kVal.push_back(vec3(0.01628153, 0.02425201, 0.01310827  ));
    vVal.push_back(vec3(-0.8897672, -0.009918511, 0.9320658 ));

    Insert(vec3(0.f));
}

void Hopalong::startData()
{
    vVal.push_back(vec3(0.f));

    kVal.push_back(0.4249092);
    kVal.push_back(8.0294190);
    kVal.push_back(2.4463632);
    kVal.push_back(0.5752739);
    kVal.push_back(4.9726858);
    kVal.push_back(1.6537983);

    kVal.push_back(.00); // TIME!!! 

    //kVal.push_back(0.66805);
    //kVal.push_back(2.277291);
    //kVal.push_back(7.2335);
    //kVal.push_back(1.75268);
    //kVal.push_back(4.56892);
    //kVal.push_back(2.2096);

    //.66805 2.277291 7.2335
    //.52266017 8.237556458 2.13812685

    Insert(vVal[0]);
}

void PowerN3D::startData()
{
    vVal.push_back(vec3(0.f));

    // order = 2
    kVal.push_back(vec3(-0.511313351336867  ,-0.6241808347404   ,0.538843904156238   ));
    kVal.push_back(vec3(-0.737248965539038  ,-0.713932853657752 ,-0.779133586212993  ));
    kVal.push_back(vec3(-0.357718114275485  ,0.323694566264749  ,0.332013862673193   ));
    kVal.push_back(vec3(0.40526582300663    ,0.776832655537874  ,0.715065854601562   ));
    kVal.push_back(vec3(0.269049969501793   ,0.377229408826679  ,0.262125037610531   ));
    kVal.push_back(vec3(-0.948385126423091  ,-0.450779830105603 ,-0.0864844848401844 ));
    kVal.push_back(vec3(0.514199461787939   ,0.506631922442466  ,-0.761063695885241  ));
    kVal.push_back(vec3(-0.7562220338732    ,0.576225875411183  ,0.830178850330412   ));
    kVal.push_back(vec3(-0.517461677547544  ,-0.28781412076205  ,-0.11858548829332   ));
    kVal.push_back(vec3(-0.0975346867926419 ,-0.708178671076894 ,-0.143647947814316  ));

    //kVal.push_back(vec3(-1.2,  .7, 1.2));
    //kVal.push_back(vec3(-0.9,  .4, 0.6));
    //kVal.push_back(vec3(-0.2,-0.4,-0.3));
    //kVal.push_back(vec3(-0.6,-0.9,-0.7));
    //kVal.push_back(vec3( 0.6,-0.3, 0.7));
    //kVal.push_back(vec3(-0.9,-0.4, 0.1));
    //kVal.push_back(vec3(-0.3,-0.5,-0.8));
    //kVal.push_back(vec3( 0.5,-0.3,-0.4));
    //kVal.push_back(vec3( 1.1,-0.3, 1.1));
    //kVal.push_back(vec3(-1.1,-1.0, 0.8));

    Insert(vVal[0]);
}

void SinCos::startData()
{
    vVal.push_back(vec3(0.f));

    kVal.push_back( 0.8068027);
    kVal.push_back(-0.1956844);
    kVal.push_back(-1.5730495);
    kVal.push_back( 2.2741647);
    kVal.push_back( 0.3097776);
    kVal.push_back(-2.7527168);
    kVal.push_back( 2.2514729);
    kVal.push_back( 0.4606878);
    kVal.push_back(-1.2902131);

    Insert(vVal[0]);
}

void Pickover::startData()
{

    vVal.push_back(vec3(0.f));

    kVal.push_back(-1.80355f);
    kVal.push_back(-1.30641f);
    kVal.push_back(-1.2392f);
    kVal.push_back(-2.07657f);

    //kVal.push_back(1.50398734177215f);
    //kVal.push_back(2.46335443037974f);
    //kVal.push_back(1.46798734177215f);
    //kVal.push_back(1.51198734177215f);
    Insert(vVal[0]);
}

void PolynomialA::startData()
{
    vVal.push_back(vec3(0.0f));

    kVal.push_back(vec3(1.586, 1.124, 0.281));

    //kVal[0]=1.626;
    //kVal[1]=1.1;
    //kVal[2]=0.252;
    Insert(vVal[0]);
}

void PolynomialB::startData()
{
    vVal.push_back(vec3(0.0f));

    kVal.push_back(vec3(1.586, 1.124, 0.281));
    kVal.push_back(vec3(1.586, 1.124, 0.281));

    Insert(vVal[0]);
}

void PolynomialC::startData()
{
    vVal.push_back(vec3(0.0f));

    kVal.push_back(vec3(-0.5113, -0.6241,  0.5388));
    kVal.push_back(vec3(-0.7372, -0.7139, -0.7791));
    kVal.push_back(vec3( 0.4052,  0.7768,  0.7150));
    kVal.push_back(vec3( 0.2690,  0.3772,  0.2621));
    kVal.push_back(vec3(-0.3577,  0.3236,  0.3320));
    kVal.push_back(vec3(-0.9483, -0.4507, -0.0864));

    Insert(vVal[0]);
}

void PolynomialABS::startData()
{
    vVal.push_back(vec3(0.0f));

    kVal.push_back(vec3( 0.079177737,  0.91686106 ,  0.40953994));
    kVal.push_back(vec3(-0.14236259 , -0.43664807 , -0.01417297));
    kVal.push_back(vec3( 0.72135115 ,  0.48274362 ,  0.17537272));
    kVal.push_back(vec3( 0.45905161 ,  0.6852051  , -0.35317159));
    kVal.push_back(vec3( 0.55965161 , -0.61271751 ,  0.44879115));
    kVal.push_back(vec3( 0.15926731 ,  0.901067   , -0.75434935));
    kVal.push_back(vec3(-0.038277864,  0.47096038 ,  0.69792914));

    Insert(vVal[0]);
}

void PolynomialPow::startData()
{
    vVal.push_back(vec3(0.0f));

    kVal.push_back(vec3(-0.5113, -0.6241,  0.5388));
    kVal.push_back(vec3(-0.7372, -0.7139, -0.7791));
    kVal.push_back(vec3( 0.4052,  0.7768,  0.7150));
    kVal.push_back(vec3( 0.2690,  0.3772,  0.2621));
    kVal.push_back(vec3(-0.3577,  0.3236,  0.3320));
    kVal.push_back(vec3(-0.9483, -0.4507, -0.0864));
    kVal.push_back(vec3( 0.5141,  0.5066, -0.7610));
    kVal.push_back(vec3(-0.7562,  0.5762,  0.8301));

    Insert(vVal[0]);
}

void PolynomialSin::startData()
{
    vVal.push_back(vec3(0.0f));

    kVal.push_back(vec3(-0.5113, -0.6241,  0.5388));
    kVal.push_back(vec3(-0.7372, -0.7139, -0.7791));
    kVal.push_back(vec3( 0.4052,  0.7768,  0.7150));
    kVal.push_back(vec3( 0.2690,  0.3772,  0.2621));
    kVal.push_back(vec3(-0.3577,  0.3236,  0.3320));
    kVal.push_back(vec3(-0.9483, -0.4507, -0.0864));
    kVal.push_back(vec3(-0.7562,  0.5762,  0.8301));
    kVal.push_back(vec3(-0.3577,  0.3236,  0.3320));
    kVal.push_back(vec3( 0.5141,  0.5066, -0.7610));
    kVal.push_back(vec3(-0.7562,  0.5762,  0.8301));

    Insert(vVal[0]);
}

void Rampe01::startData()
{
    vVal.push_back(vec3(0.0f));

    kVal.push_back(vec3(1.586, 1.124, 0.281));
    kVal.push_back(vec3(1.586, 1.124, 0.281));

    Insert(vVal[0]);
}
void Rampe02::startData()
{
    vVal.push_back(vec3(0.0f));

    kVal.push_back(vec3(1.586, 1.124, 0.281));
    kVal.push_back(vec3(1.586, 1.124, 0.281));

    Insert(vVal[0]);
}
void Rampe03::startData()
{
    vVal.push_back(vec3(0.0f));

    kVal.push_back(vec3(1.586, 1.124, 0.281));
    kVal.push_back(vec3(1.586, 1.124, 0.281));

    Insert(vVal[0]);
}
void Rampe03A::startData()
{
    vVal.push_back(vec3(0.0f));

    kVal.push_back(vec3(1.586, 1.124, 0.281));
    kVal.push_back(vec3(1.586, 1.124, 0.281));

    Insert(vVal[0]);
}
void Rampe04::startData()
{
    vVal.push_back(vec3(0.0f));

    kVal.push_back(vec3(1.586, 1.124, 0.281));
    kVal.push_back(vec3(1.586, 1.124, 0.281));

    Insert(vVal[0]);
}
void Rampe05::startData()
{
    vVal.push_back(vec3(0.0f));

    kVal.push_back(vec3(1.586, 1.124, 0.281));
    kVal.push_back(vec3(1.586, 1.124, 0.281));
    kVal.push_back(vec3(1.586, 1.124, 0.281));

    Insert(vVal[0]);
}
void Rampe06::startData()
{
    vVal.push_back(vec3(0.0f));

    kVal.push_back(vec3(1.586, 1.124, 0.281));
    kVal.push_back(vec3(1.586, 1.124, 0.281));

    Insert(vVal[0]);
}
void Rampe07::startData()
{
    vVal.push_back(vec3(0.0f));

    kVal.push_back(vec3(1.586, 1.124, 0.281));
    kVal.push_back(vec3(1.586, 1.124, 0.281));

    Insert(vVal[0]);
}
void Rampe08::startData()
{
    vVal.push_back(vec3(0.0f));

    kVal.push_back(vec3(1.586, 1.124, 0.281));

    Insert(vVal[0]);
}
void Rampe09::startData()
{
    vVal.push_back(vec3(0.0f));

    kVal.push_back(vec3(1.586, 1.124, 0.281));
    kVal.push_back(vec3(1.586, 1.124, 0.281));
    kVal.push_back(vec3(1.586, 1.124, 0.281));

    Insert(vVal[0]);
}
void Rampe10::startData()
{
    vVal.push_back(vec3(0.0f));

    kVal.push_back(vec3(1.586, 1.124, 0.281));
    kVal.push_back(vec3(1.586, 1.124, 0.281));
    kVal.push_back(vec3(1.586, 1.124, 0.281));

    Insert(vVal[0]);
}
void KingsDream::startData()
{
    vVal.push_back(vec3(0.0f));

    kVal.push_back( 0.8068027);
    kVal.push_back(-0.1956844);
    kVal.push_back(-1.5730495);
    kVal.push_back( 2.2741647);
    kVal.push_back( 0.3097776);
    kVal.push_back(-2.7527168);

    Insert(vVal[0]);
}
void Mira3D::startData()
{
    vVal.push_back(vec3(0.f, 12.f, 0.0f));

    kVal.push_back( 0.2);
    kVal.push_back( 1.0);
    kVal.push_back( 0.2);
    kVal.push_back(-1.0);
    kVal.push_back( 0.2);
    kVal.push_back( 1.0);
    kVal.push_back( 0.2);
    kVal.push_back(-1.0);

    Insert(vVal[0]);
}

#if !defined(GLAPP_DISABLE_DLA)
// https://github.com/fogleman/dlaf
void dla3D::startData()
{
    //staring point: Center
    vVal.push_back(vec3(0.f));

    // m_ParticleSpacing defines the distance between particles that are
    // joined together
    kVal.push_back( 1.0/100.0); // kVal[0] -> m_ParticleSpacing;    

    // m_AttractionDistance defines how close together particles must be in
    // order to join together
    kVal.push_back( 3.0/100.0); // kVal[1] -> m_AttractionDistance;

    // m_MinMoveDistance defines the minimum distance that a particle will move
    // during its random walk
    kVal.push_back( 1.0/100.0); // kVal[2] -> m_MinMoveDistance;

}
#endif

void ChenLee::startData()
{
    vVal.push_back(vec3(1.f, 0.0f, 4.5f));

    kVal.push_back( 5.0);
    kVal.push_back(-10);
    kVal.push_back(-.38);

    Insert(vVal[0]);
}

void TSUCS::startData()
{
    vVal.push_back(vec3(.1f, 1.0f, -.1f));

    kVal.push_back( 40);
    kVal.push_back( 0.f);
    kVal.push_back(.833f);
    kVal.push_back(.5);
    kVal.push_back(.65f);
    kVal.push_back( 20);

    Insert(vVal[0]);
}

void Aizawa::startData()
{
    vVal.push_back(vec3(0.1f, 0.0f, 0.0f));

    kVal.push_back( .95f);
    kVal.push_back( .7f);
    kVal.push_back( .6f);
    kVal.push_back(3.5f);
    kVal.push_back( .25f);
    kVal.push_back( .1f);

    Insert(vVal[0]);
}

void YuWang::startData()
{
    vVal.push_back(vec3(2.2f, 2.4f, 28.0f));

    kVal.push_back( 10.f );
    kVal.push_back( 40.f );
    kVal.push_back(  2.f );
    kVal.push_back(  2.5f);

    Insert(vVal[0]);
}

void FourWing::startData()
{
    vVal.push_back(vec3(.1f, .1f, .1f));

    kVal.push_back(  4.f);
    kVal.push_back(  6.f);
    kVal.push_back( 10.f);
    kVal.push_back(  1.f);
    kVal.push_back(  5.f);

    Insert(vVal[0]);
}

void FourWing2::startData()
{
    vVal.push_back(vec3(4.0f, 1.0f, 1.0f));

    kVal.push_back(-14.f);
    kVal.push_back(  5.f);
    kVal.push_back(  1.f);
    kVal.push_back( 16.f);
    kVal.push_back(-43.f);
    kVal.push_back(  1.f);

    Insert(vVal[0]);
}

void FourWing3::startData()
{
    vVal.push_back(vec3(1.0f, -2.0f, 1.0f));

    kVal.push_back(  1.f);
    kVal.push_back(  1.f);
    kVal.push_back(  1.f);
    kVal.push_back(  1.f);
    kVal.push_back(  1.f);
    kVal.push_back(  1.f);
    kVal.push_back(  1.f);

    Insert(vVal[0]);
}

void Thomas::startData()
{
    vVal.push_back(vec3(.1f, .0f, .0f));

    kVal.push_back(0.19f);
    kVal.push_back(0.19f);
    kVal.push_back(0.19f);

    Insert(vVal[0]);
}

void Halvorsen::startData()
{
    vVal.push_back(vec3(1.0f, .0f, .0f));

    kVal.push_back(1.4f);
    kVal.push_back(1.4f);
    kVal.push_back(1.4f);

    Insert(vVal[0]);
}

void Arneodo::startData()
{
    vVal.push_back(vec3(.1f, .0f, .0f));

    kVal.push_back(-5.5f);
    kVal.push_back(3.5f);
    kVal.push_back(-1.0f);

    Insert(vVal[0]);
}

void Bouali::startData()
{
    vVal.push_back(vec3(1.f, .1f, .1f));

    kVal.push_back(4.0f);
    kVal.push_back(1.0f);
    kVal.push_back(0.3f);
    kVal.push_back(.05f);
    kVal.push_back(1.5f);
    kVal.push_back(1.0f);

    Insert(vVal[0]);
}

void Hadley::startData()
{
    vVal.push_back(vec3(.1f, .0f, .0f));

    kVal.push_back(0.2f);
    kVal.push_back(4.0f);
    kVal.push_back(8.0f);
    kVal.push_back(1.0f);

    Insert(vVal[0]);
}

void LiuChen::startData()
{
    vVal.push_back(vec3(1.f, 3.f, 5.f));

    kVal.push_back(2.4f);
    kVal.push_back(-3.78f);
    kVal.push_back(14.f);
    kVal.push_back(-11.f);
    kVal.push_back(4.f);
    kVal.push_back(5.58f);
    kVal.push_back(-1.f);

    Insert(vVal[0]);
}

void GenesioTesi::startData()
{
    vVal.push_back(vec3(.1f, .1f, .1f));

    kVal.push_back(.44f);
    kVal.push_back(1.1f);
    kVal.push_back(1.0f);

    Insert(vVal[0]);
}

void NewtonLeipnik::startData()
{
    vVal.push_back(vec3(.349f, 0.f, -.16f));

    kVal.push_back(.4f);
    kVal.push_back(.175f);

    Insert(vVal[0]);
}

void NoseHoover::startData()
{
    vVal.push_back(vec3(.1f, .0f, .0f));

    kVal.push_back(1.5f);

    Insert(vVal[0]);
}

void RayleighBenard::startData()
{
    vVal.push_back(vec3(.1f, 0.f, 0.f));

    kVal.push_back(9.f);
    kVal.push_back(5.f);
    kVal.push_back(12.f);

    Insert(vVal[0]);
}


void Sakarya::startData()
{
    vVal.push_back(vec3(1.f, -1.f, 1.f));

    kVal.push_back(.4f);
    kVal.push_back(.3f);

    Insert(vVal[0]);
}

void Robinson::startData()
{
    vVal.push_back(vec3(.1f, .1f, .1f));

    kVal.push_back(0.71f);
    kVal.push_back(1.8587f);
    kVal.push_back(0.7061f);
    kVal.push_back(0.1f);
    kVal.push_back(1.f);

    Insert(vVal[0]);
}

void Rossler::startData()
{
    vVal.push_back(vec3(.1f, 0.f, 0.f));

    kVal.push_back(.2f);
    kVal.push_back(.2f);
    kVal.push_back(5.7f);

    Insert(vVal[0]);
}

void Rucklidge::startData()
{
    vVal.push_back(vec3(1.f, 0.f, 0.f));

    kVal.push_back(6.7f);
    kVal.push_back(2.f);

    Insert(vVal[0]);
}

void DequanLi::startData()
{
    vVal.push_back(vec3(.349f, 0.f, -.16f));

    kVal.push_back(40.f);
    kVal.push_back(1.833f);
    kVal.push_back(0.16f);
    kVal.push_back(0.65f);
    kVal.push_back(55.f);
    kVal.push_back(20.f);

    Insert(vVal[0]);
}

void MultiChuaII::startData()
{
    vVal.push_back(vec3(.1f, -.2f, .3f));

    kVal.push_back(9.f);
    kVal.push_back(14.286f);
    kVal.push_back(-1.f/7.f);
    kVal.push_back( 2.f/7.f);
    kVal.push_back(-4.f/7.f);
    kVal.push_back( 2.f/7.f);
    kVal.push_back(-4.f/7.f);
    kVal.push_back( 2.f/7.f);
    kVal.push_back( 1.0f);
    kVal.push_back( 2.15);
    kVal.push_back( 3.6f);
    kVal.push_back( 8.2f);
    kVal.push_back(13.0f);

    Insert(vVal[0]);
}

void ZhouChen::startData()
{
    vVal.push_back(vec3(3.f, 1.f, 1.f));

    kVal.push_back(2.97f);
    kVal.push_back(.15f);
    kVal.push_back(-3.f);
    kVal.push_back(1.f);
    kVal.push_back(-8.78f);

    Insert(vVal[0]);
}

void SprottLinzF::startData()
{
    vVal.push_back(vec3(.1f, 0.f, 0.f));

    kVal.push_back(.5f);

    Insert(vVal[0]);
}

void SprottLinzB::startData()
{
    vVal.push_back(vec3(.1f, 0.f, 1.f));

    kVal.push_back(1.0f);
    kVal.push_back(1.0f);
    kVal.push_back(1.0f);
    kVal.push_back(1.0f);

    Insert(vVal[0]);
}

void Coullet::startData()
{
    vVal.push_back(vec3(.1f, .41f, .31f));

    kVal.push_back(.8f);
    kVal.push_back(-1.1f);
    kVal.push_back(-.45f);
    kVal.push_back(-1.f);

    Insert(vVal[0]);
}

void Dadras::startData()
{
    vVal.push_back(vec3(.1f, .1f, .1f));

    kVal.push_back(3.f);
    kVal.push_back(2.7f);
    kVal.push_back(1.7f);
    kVal.push_back(2.f);
    kVal.push_back(9.f);

    Insert(vVal[0]);
}


void Lorenz::startData()
{
    //16.242, 29.4125, 1.16328, infinite loop

    //        Insert(vec3(0.f, 1.f, 0.f));
    vVal.push_back(vec3(.0f, .0f, .1f));

    kVal.push_back(10.0f);
    kVal.push_back(28.f);
    kVal.push_back(8.0f / 3.0f);

    Insert(vVal[0]);
}


void juliaBulb_IIM::startData()
{
    vVal.push_back(vec3(0.0f));

    kVal.push_back(- .2f);
    kVal.push_back(  .8f);
    kVal.push_back( 0.0f);

    Insert(vVal[0]);
}

void BicomplexBase::startData()
{
    vVal.push_back(vec3(0.0f));

    kVal.push_back(- .2f);
    kVal.push_back(  .8f);
    kVal.push_back( 0.0f);
    kVal.push_back( 0.0f);

    Insert(vVal[0]);
}

void quatJulia_IIM::startData()
{
    vVal.push_back(vec3(0.0f));

    kVal.push_back(- .2f);
    kVal.push_back(  .8f);
    kVal.push_back( 0.0f);
    kVal.push_back( 0.0f);

    Insert(vVal[0]);
}

void juliaBulb4th_IIM::startData()
{
    vVal.push_back(vec3(0.0f));

    kVal.push_back(- .2f);
    kVal.push_back(  .8f);
    kVal.push_back( 0.0f);

    Insert(vVal[0]);
}

void glynnJB_IIM::startData()
{
    vVal.push_back(vec3(-.61f, 0.0f, -.41f));
    //vVal.push_back(vec3(0.0f));

    kVal.push_back(- .2f);
    kVal.push_back(  .8f);
    kVal.push_back( 0.0f);

    Insert(vVal[0]);
}
