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

#if !defined(GLAPP_DISABLE_DLA)

using tPrec = VG_T_TYPE;

//#define GLAPP_USE_BOOST_LIBRARY

// number of dimensions (must be 2 or 3)
#define DLA_DIM 3

#define DLA_USE_FAST_RANDOM
#ifdef DLA_USE_FAST_RANDOM
    #define DLA_RANDOM_NORM fastPrng64.xorShift_VNI<float>()
    #define DLA_RANDOM_01   fastPrng64.xorShift_UNI<float>()
#else
    #define DLA_RANDOM_NORM stdRandom(-1.f, 1.f)
    #define DLA_RANDOM_01   stdRandom( 0.f, 1.f)
#endif

// boost is used for its spatial index

#ifdef GLAPP_USE_BOOST_LIBRARY
    #include <boost/function_output_iterator.hpp>
    #include <boost/geometry/geometry.hpp>

    using BoostPoint = boost::geometry::model::point<float, DLA_DIM, boost::geometry::cs::cartesian>;
    using boostIndexValue = std::pair<BoostPoint, uint32_t>;
    using boostIndex = boost::geometry::index::rtree<boostIndexValue, boost::geometry::index::linear<4>>;

    #define parentPOINT(PARENT) m_Points[PARENT]
    #define thisPOINT m_Points
#else
    #include <nanoflann/nanoflann.hpp>

    #define parentPOINT(PARENT) m_Points.pts[PARENT]
    #define thisPOINT m_Points.pts


TEMPLATE_TYPENAME_T struct pointCloud
{

    std::vector<vec3> pts;

    // Must return the number of data points
    inline size_t kdtree_get_point_count() const { return pts.size(); }

    // Returns the dim'th component of the idx'th point in the class:
    // Since this is inlined and the "dim" argument is typically an immediate value, the
    //  "if/else's" are actually solved at compile time.
    inline tPrec kdtree_get_pt(const size_t idx, const size_t dim) const
    { return pts[idx][dim]; }

    // Optional bounding-box computation: return false to default to a standard bbox computation loop.
    //   Return true if the BBOX was already computed by the class and returned in "bb" so it can be avoided to redo it again.
    //   Look at bb.size() to find out the expected dimensionality (e.g. 2 or 3 for point clouds)
    template <class BBOX>
    bool kdtree_get_bbox(BBOX& /* bb */) const { return false; }

};

using tPointCloud =  pointCloud<tPrec>;

using tKDTreeDistanceFunc = nanoflann::L2_Simple_Adaptor<tPrec, tPointCloud>;
using tKDTree = nanoflann::KDTreeSingleIndexDynamicAdaptor<tKDTreeDistanceFunc, tPointCloud, 3>;
#endif

//  DLA3D base class
////////////////////////////////////////////////////////////////////////////
// Adaptation and optimization from original project of Michael Fogleman
// https://github.com/fogleman/dlaf
//
// Original Parameters   ==>    substitution
// -------------------------------------------------------------------------
// ParticleSpacing       ==>    kVal[0]
// AttractionDistance    ==>    kVal[1]
// MinMoveDistance       ==>    kVal[2]
//
// Description from author
// -------------------------------------------------------------------------
// ParticleSpacing     The distance between particles when they become joined together.
// AttractionDistance  How close together particles must be in order to join together.
// MinMoveDistance     The minimum distance that a particle will move in an iteration during its random walk.
// Stubbornness        How many join attempts must occur before a particle will allow another particle to join to it.
// Stickiness          The probability that a particle will allow another particle to join to it.
class dla3D : public attractorScalarK {
public:
    dla3D() {
        stepFn = (stepPtrFn) &dla3D::Step;

        vMin = -1.0; vMax = 1.0; kMin = -1.0; kMax = 1.0;

        m_POV = vec3( 0.f, 0, 12.f);
        inputKMin = 0.0001, inputKMax = 10000.0;
        setDLAType();
    }
#if !defined(GLAPP_USE_BOOST_LIBRARY)
    ~dla3D() { delete m_Index; }
#endif

    void buildIndex();

    inline void addLoadedPoint(const vec3 &p) {
        thisPOINT.push_back(p);
        boundingRadius = std::max(boundingRadius, length(p) + kVal[1]);
    }


    void resetIndexData() {
        if(thisPOINT.size()) thisPOINT.clear();
#ifdef GLAPP_USE_BOOST_LIBRARY
        if(m_Index.size()) m_Index.clear();
#else
        delete m_Index;
        m_Index = new tKDTree(DLA_DIM, m_Points, nanoflann::KDTreeSingleIndexAdaptorParams(10 /* max leaf */) );
#endif
        m_JoinAttempts.clear();
        boundingRadius = 0.f;
    }
protected:
    void initStep() {
        resetIndexData();
        resetQueue();
        Insert(vec4(0.f));
        Add(vec3(0.0));
        //addedPoints.clear();
        //startThreads(4);
    }

    void additionalDataCtrls();
    void saveAdditionalData(Config &cfg);
    void loadAdditionalData(Config &cfg);

#ifdef GLAPP_USE_BOOST_LIBRARY
    // Add adds a new particle with the specified parent particle
    void Add(const vec3 &p) {
        const uint32_t id = m_Points.size();
        m_Index.insert(std::make_pair(BoostPoint(p.x, p.y, p.z), id));
        m_Points.push_back(p);
        m_JoinAttempts.push_back(0);
        boundingRadius = max(boundingRadius, length(p) + kVal[1]);
    }
    // Nearest returns the index of the particle nearest the specified point
    uint32_t Nearest(const vec3 &point) const {
        uint32_t result = -1;
        m_Index.query(
            boost::geometry::index::nearest(BoostPoint(point.x,point.y,point.z), 1),
            boost::make_function_output_iterator([&result](const auto &value) {
                result = value.second;
            }));
        return result;
    }
#else
/*
// Multipoints
    std::vector<vec3> tmp;
    void Add(const vec3 &p) {
        //my_kd_tree_t index(3 , m_Points, KDTreeSingleIndexAdaptorParams(10 max leaf ) );
        //static uint32_t count = 0;
        //const int nPT = 16;

        size_t id = m_Points.pts.size();
        m_Points.pts.push_back(p);
        m_JoinAttempts.push_back(0);

        static int count =0;
        static int next = 1;
        static int limit = 1024;
        static int block = 0xff;
        if(next<limit) {
            if(!(id%next)) { m_Index->addPoints(id-count, id); count = 0; next = (1+(id>>4)); }
            else count++;
            //m_Index->addPoints(id, id);
        } else {
            if(!(id&block)) {
                m_Index->addPoints(id-count, id); count = 0; int blk = 512 / (1+(id>>16)); next = blk<1 ? 1 : blk;
            }
            else count++;
        }
        //m_Index->addPoints(id,id);

        boundingRadius = std::max(boundingRadius, length(p) + kVal[1]);
    }
*/
    void Add(const vec3 &p) {
        size_t id = m_Points.pts.size();
        m_Points.pts.push_back(p);
        m_JoinAttempts.push_back(0);

        m_Index->addPoints(id, id);
        boundingRadius = std::max(boundingRadius, length(p) + kVal[1]);
    }

    uint32_t Nearest(const vec3 &point) const {
        size_t ret_index;
        tPrec out_dist_sqr = kVal[1]*.5;
        nanoflann::KNNResultSet<tPrec> resultSet(1);
        resultSet.init(&ret_index, &out_dist_sqr );
        m_Index->findNeighbors(resultSet, (const tPrec *) &point, nanoflann::SearchParams(1, boundingRadius /*kVal[1]*/));
        return ret_index;
    }

#endif
        //std::cout << id << "," << p.x << "," << p.y << "," << p.z << std::endl;

    // PlaceParticle computes the final placement of the particle.
    vec3 PlaceParticle(const vec3 &p, const uint32_t parent) const {
        return lerp(parentPOINT(parent), p, kVal[0]);
    }

    // RandomStartingPosition returns a random point to start a new particle
    vec3 RandomStartingPosition() const {
        return normalizedRandomVector() * boundingRadius;
    }

    // ShouldReset returns true if the particle has gone too far away and
    // should be reset to a new random starting position
    bool ShouldReset(const vec3 &p) const {
        return length(p) > boundingRadius * 2;
    }

    // ShouldJoin returns true if the point should attach to the specified
    // parent particle. This is only called when the point is already within
    // the required attraction distance.
    bool ShouldJoin(const vec3 &p, const uint32_t parent) {
        return (m_JoinAttempts[parent]++ < m_Stubbornness) ? false : DLA_RANDOM_01 <= m_Stickiness;
    }
/*
    // MotionVector returns a vector specifying the direction that the
    // particle should move for one iteration. The distance that it will move
    // is determined by the algorithm.
    vec3 MotionVector(const vec3 &p) const {
        return RandomInUnitSphere();
    }

    vec3 RandomInUnitSphere() const {
        vec3 p;
        do {
            p = vec3(DLA_RANDOM_NORM, DLA_RANDOM_NORM, DLA_RANDOM_NORM);
        } while(length(p) >= 1.f);
        return p;
    }
*/
    vec3 normalizedRandomVector() const { return normalize(vec3(DLA_RANDOM_NORM, DLA_RANDOM_NORM, DLA_RANDOM_NORM)); }


    // AddParticle diffuses one new particle and adds it to the model
    vec3 &AddParticle_() {
        // compute particle starting location
        vec3 p = RandomStartingPosition();

        // do the random walk
        while (true) {
            // get distance to nearest other particle
            const uint32_t parent = Nearest(p);
            const tPrec d = distance(p, parentPOINT(parent));

            // check if close enough to join
            if (d < kVal[1]) {
                if (!ShouldJoin(p, parent)) {
                    // push particle away a bit
                    //p = lerp(parentPOINT(parent), p, kVal[1]+kVal[2]);
                    p = lerp(parentPOINT(parent), p, kVal[1]);
                    continue;
                }
                // adjust particle pos in relation to its parent and add the point
                Add(PlaceParticle(p, parent));
                return thisPOINT.back();
            }

            // move randomly
            p += normalizedRandomVector() * DLA_RANDOM_01 * std::max(kVal[2], d - kVal[1]);

            // check if particle is too far away, reset if so
            if (ShouldReset(p)) p = RandomStartingPosition();
        }
    }

    vec3 &AddParticle() {
        vec3 p = RandomStartingPosition();

        uint32_t parent;
        tPrec d;
        const tPrec val1 = kVal[1], val2 = kVal[2];
        do {
            parent = Nearest(p);
            d = distance(p, parentPOINT(parent)); // get distance to nearest other particle

            if (ShouldReset(p)) p = RandomStartingPosition(); // check if particle is too far away, reset if so
            else                p += normalizedRandomVector() * DLA_RANDOM_01 * std::max(val2, d - val1); // move randomly

        } while (d > val1); // repeat until is close enough to join

        while(!ShouldJoin(p, parent)) { p = lerp(parentPOINT(parent), p, val1+val2); } // push particle away a bit

        Add(PlaceParticle(p, parent)); // adjust particle pos in relation to its parent and add the point
        return thisPOINT.back();
    }

    void Step(vec4 &v, vec4 &vp);
    void startData();

private:
#if !defined(DLA_USE_FAST_RANDOM)
    tPrec stdRandom(tPrec lo, tPrec hi) const {
        static thread_local std::mt19937 gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        std::uniform_real_distribution<tPrec> dist(lo, hi); return dist(gen);
    }
#endif
    vec3 lerp(const vec3 &a, const vec3 &b, tPrec d) const {
        return a + normalize(b-a) * d;
    }

    //The probability that a particle will allow another particle to join to it.
    tPrec m_Stickiness = 1.0;
    // m_Stubbornness defines how many interactions must occur before a
    // particle will allow another particle to join to it.
    int m_Stubbornness = 0;

    // m_BoundingRadius defines the radius of the bounding sphere that bounds
    // all of the particles
    tPrec boundingRadius = 0;

    // m_JoinAttempts tracks how many times other particles have attempted to
    // join with each finalized particle
    std::vector<uint32_t> m_JoinAttempts;

#ifdef GLAPP_USE_BOOST_LIBRARY
    // m_Index is the spatial index used to accelerate nearest neighbor queries
    boostIndex m_Index;
    // m_Points stores the final particle positions
    std::vector<vec3> m_Points;
#else
    tPointCloud m_Points;
    tKDTree *m_Index = nullptr;
#endif
};
#endif
