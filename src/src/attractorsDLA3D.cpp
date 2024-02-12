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
#include "glWindow.h"


#if !defined(GLAPP_DISABLE_DLA)
////////////////////////////////////////////////////////////////////////////
void dla3D::Step(vec4 &v, vec4 &vp)
{
    v = vec4(theWnd->getParticlesSystem()->getTMat()->getTrackball().getRotationCenter(), 0.f);
    vp = vec4(AddParticle(), 0.f);
}

void dla3D::buildIndex() // for loaded data
{
    uint32_t id = thisPOINT.size();

    m_JoinAttempts.resize(id);
    memset(m_JoinAttempts.data(), 0, id*sizeof(int));

#ifdef GLAPP_USE_BOOST_LIBRARY
    vec3 *p = thisPOINT.data();
    for(int i=0; i<id; i++, p++)
        m_Index.insert(std::make_pair(BoostPoint(p->x, p->y, p->z), i));
#else
    m_Index->addPoints(0, id-1);
#endif

    attractorsList.continueDLA(false); // reset the flag
    attractorsList.getThreadStep()->startThread();
}


// https://github.com/BrutPitt/DLAf-optimized
void dla3D::startData()
{
    //staring point: Center
    vVal.push_back(vec4(0.f));

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

