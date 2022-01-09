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

#include <stdint.h>
#include <chrono>
#include <type_traits>
#include <cfloat>
#include <functional>

namespace fastPRNG {
#define UNI_32BIT_INV 2.3283064365386962890625e-10
#define VNI_32BIT_INV 4.6566128730773925781250e-10   // UNI_32BIT_INV * 2

#define UNI_64BIT_INV 5.42101086242752217003726400434970e-20
#define VNI_64BIT_INV 1.08420217248550443400745280086994e-19 // UNI_64BIT_INV * 2

#define FPRNG_SEED_INIT64 std::chrono::system_clock::now().time_since_epoch().count()
#define FPRNG_SEED_INIT32 FPRNG_SEED_INIT64

inline static uint32_t splitMix32(const uint32_t val) {
    uint32_t z = val + UINT32_C(0x9e3779b9);
    z ^= z >> 15; // 16 for murmur3
    z *= UINT32_C(0x85ebca6b);
    z ^= z >> 13;
    z *= UINT32_C(0xc2b2ae35);
    return z ^ (z >> 16);
}

inline static uint64_t splitMix64(const uint64_t val) {
    uint64_t z = val    + UINT64_C(0x9e3779b97f4a7c15);
    z = (z ^ (z >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94d049bb133111eb);
    return z ^ (z >> 31);
}


// 32/64 bit rotation func
template <typename T> inline static T rotl(const T x, const int k) { return (x << k) | (x >> (sizeof(T)*8 - k)); } // sizeof*8 is resolved to compile-time

/*--------------------------------------------------------------------------
 32bit PRNG Algorithms: xoshiro / xoroshiro

 xoshiro256+ / xoshiro256++ / xoshiro256**
 xoroshiro128+ / xoroshiro128++ / xoroshiro128**

 Algorithms by David Blackman and Sebastiano Vigna
 http://prng.di.unimi.it/

 To the extent possible under law, the author has dedicated all copyright
 and related and neighboring rights to this software to the public domain
 worldwide. This software is distributed without any warranty.

 See <http://creativecommons.org/publicdomain/zero/1.0/>.
-------------------------------------------------------------------------- */
#define XOSHIRO128\
    const uint32_t t = s1 << 9;\
    s2 ^= s0;\
    s3 ^= s1;\
    s1 ^= s2;\
    s0 ^= s3;\
    s2 ^= t;\
    s3 = rotl<uint32_t>(s3, 11);\
    return result;

#define XOROSHIRO64\
    s1 ^= s0;\
    s0 = rotl<uint32_t>(s0, 26) ^ s1 ^ (s1 << 9);\
    s1 = rotl<uint32_t>(s1, 13);\
    return result;

#define XORSHIFT32\
    s0 ^= s0 << 13;\
    s0 ^= s0 >> 17;\
    s0 ^= s0 << 5;\
    return s0;

#define XOSHIRO128_STATIC(FUNC)\
    static const uint32_t seed = uint32_t(FPRNG_SEED_INIT32);\
    static uint32_t s0 = splitMix32(seed), s1 = splitMix32(s0), s2 = splitMix32(s1), s3 = splitMix32(s2);\
    FUNC; XOSHIRO128

#define XOROSHIRO64_STATIC(FUNC)\
    static const uint32_t seed = uint32_t(FPRNG_SEED_INIT32);\
    static uint32_t s0 = splitMix32(seed), s1 = splitMix32(s0);\
    FUNC; XOROSHIRO64

#define XORSHIFT32_STATIC\
    static uint32_t s0 = uint32_t(FPRNG_SEED_INIT32);\
    XORSHIFT32

// fastXS32
//
// 32bit pseudo-random generator
// All integer values are returned in interval [0, UINT32_MAX]
// to get values between [INT32_MIN, INT32_MAX] just cast result to int32_t
///////////////////////////////////////////////////////////////////////////////
class fastXS32
{
public:
    fastXS32(const uint32_t seedVal = uint32_t(FPRNG_SEED_INIT32)) { seed(seedVal); }

    inline uint32_t xoshiro128p()  { return xoshiro128(s0 + s3); }
    inline uint32_t xoshiro128pp() { return xoshiro128(rotl<uint32_t>(s0 + s3, 7) + s0); }
    inline uint32_t xoshiro128xx() { return xoshiro128(rotl<uint32_t>(s1 * 5, 7) * 9); }

    template <typename T> inline T xoshiro128p_UNI() { return T(        xoshiro128p() ) * UNI_32BIT_INV; } // _UNI<T>   returns value in [ 0, 1] with T ==> float/double
    template <typename T> inline T xoshiro128p_VNI() { return T(int32_t(xoshiro128p())) * VNI_32BIT_INV; } // _VNI<T>   returns value in [-1, 1] with T ==> float/double
    template <typename T> inline T xoshiro128p_Range(T min, T max)                                         // _Range<T> returns value in [min, max] with T ==> float/double
            { return min + (max-min) * xoshiro128p_UNI<T>(); }

    inline uint32_t xoroshiro64x()  { return xoroshiro64(               s0 * UINT32_C(0x9E3779BB)); }
    inline uint32_t xoroshiro64xx() { return xoroshiro64(rotl<uint32_t>(s0 * UINT32_C(0x9E3779BB), 5) * 5); }

    template <typename T> inline T xoroshiro64x_UNI() { return T(         xoroshiro64x() ) * UNI_32BIT_INV; } // _UNI<T>   returns value in [ 0, 1] with T ==> float/double
    template <typename T> inline T xoroshiro64x_VNI() { return T(int32_t(xoroshiro64x()))  * VNI_32BIT_INV; } // _VNI<T>   returns value in [-1, 1] with T ==> float/double
    template <typename T> inline T xoroshiro64x_Range(T min, T max)                                           // _Range<T> returns value in [min, max] with T ==> float/double
            { return min + (max-min) * xoroshiro64x_UNI<T>(); }

    inline uint32_t xorShift() { XORSHIFT32 } // Marsaglia xorShift: period 2^32-1

    template <typename T> inline T xorShift_UNI() { return         xorShift()  * UNI_32BIT_INV; } // _UNI<T>   returns value in [ 0, 1] with T ==> float/double
    template <typename T> inline T xorShift_VNI() { return int32_t(xorShift()) * VNI_32BIT_INV; } // _VNI<T>   returns value in [-1, 1] with T ==> float/double
    template <typename T> inline T xorShift_Range(T min, T max)                                   // _Range<T> returns value in [min, max] with T ==> float/double
            { return min + (max-min) * xorShift_UNI<T>(); }

    void seed(const uint32_t seedVal = uint32_t(FPRNG_SEED_INIT32)) {
        s0 = splitMix32(seedVal);
        s1 = splitMix32(s0);
        s2 = splitMix32(s1);
        s3 = splitMix32(s2);
    }
    
private:
    inline uint32_t xoroshiro64(const uint32_t result)  { XOROSHIRO64 }
    inline uint32_t xoshiro128(const uint32_t result)   { XOSHIRO128  }

    uint32_t s0, s1, s2, s3;
};

// fastXS32s - static members
//      you can call directly w/o declaration, but..
//      N.B. all members/functions share same seed, and subsequents xor & shift
//           operations on it, if you need different seeds declare more
//           fastXS32 (non static) objects
//
// 32bit pseudo-random generator
// All integer values are returned in interval [0, UINT32_MAX]
// to get values between [INT32_MIN, INT32_MAX] just cast result to int32_t
///////////////////////////////////////////////////////////////////////////////
class fastXS32s
{
public:
    fastXS32s() = default;

    inline static uint32_t xoshiro128p()  { XOSHIRO128_STATIC(const uint32_t result = s0 + s3)               }
    inline static uint32_t xoshiro128pp() { XOSHIRO128_STATIC(const uint32_t result = rotl<uint32_t>(s0 + s3, 7) + s0) }
    inline static uint32_t xoshiro128xx() { XOSHIRO128_STATIC(const uint32_t result = rotl<uint32_t>(s1 * 5, 7) * 9)   }

    template <typename T> inline static T xoshiro128p_UNI() { return T(        xoshiro128p() ) * UNI_32BIT_INV; } // _UNI<T>   returns value in [ 0, 1] with T ==> float/double
    template <typename T> inline static T xoshiro128p_VNI() { return T(int32_t(xoshiro128p())) * VNI_32BIT_INV; } // _VNI<T>   returns value in [-1, 1] with T ==> float/double
    template <typename T> inline static T xoshiro128p_Range(T min, T max)                                         // _Range<T> returns value in [min, max] with T ==> float/double
            { return min + (max-min) * xoshiro128p_UNI<T>(); }

    inline static uint32_t xoroshiro64x()  { XOROSHIRO64_STATIC(const uint32_t result =                s0 * UINT32_C(0x9E3779BB))         }
    inline static uint32_t xoroshiro64xx() { XOROSHIRO64_STATIC(const uint32_t result = rotl<uint32_t>(s0 * UINT32_C(0x9E3779BB), 5) * 5) }

    template <typename T> inline static T xoroshiro64x_UNI() { return T(         xoroshiro64x() ) * UNI_32BIT_INV; } // _UNI<T>   returns value in [ 0, 1] with T ==> float/double
    template <typename T> inline static T xoroshiro64x_VNI() { return T(int32_t(xoroshiro64x()))  * VNI_32BIT_INV; } // _VNI<T>   returns value in [-1, 1] with T ==> float/double
    template <typename T> inline static T xoroshiro64x_Range(T min, T max)                                           // _Range<T> returns value in [min, max] with T ==> float/double
            { return min + (max-min) * xoroshiro64x_UNI<T>(); }

    inline static uint32_t xorShift() { XORSHIFT32_STATIC } //Marsaglia xorShift: period 2^32-1

    template <typename T> inline static T xorShift_UNI() { return         xorShift()  * UNI_32BIT_INV; } // _UNI<T>   returns value in [ 0, 1] with T ==> float/double
    template <typename T> inline static T xorShift_VNI() { return int32_t(xorShift()) * VNI_32BIT_INV; } // _VNI<T>   returns value in [-1, 1] with T ==> float/double
    template <typename T> inline static T xorShift_Range(T min, T max)                                   // _Range<T> returns value in [min, max] with T ==> float/double
            { return min + (max-min) * xorShift_UNI<T>(); }
};

#undef XOSHIRO128
#undef XOROSHIRO64
#undef XORSHIFT32
#undef XOSHIRO128_STATIC
#undef XOROSHIRO64_STATIC
#undef XORSHIFT32_STATIC

/*--------------------------------------------------------------------------
 64bit PRNG Algorithms: xoshiro / xoroshiro

 xoshiro256+ / xoshiro256++ / xoshiro256**
 xoroshiro128+ / xoroshiro128++ / xoroshiro128**

 Algorithms by David Blackman and Sebastiano Vigna
 http://prng.di.unimi.it/

 To the extent possible under law, the author has dedicated all copyright
 and related and neighboring rights to this software to the public domain
 worldwide. This software is distributed without any warranty.

 See <http://creativecommons.org/publicdomain/zero/1.0/>.
-------------------------------------------------------------------------- */
#define XOSHIRO256\
    const uint64_t t = s1 << 17;\
    s2 ^= s0;\
    s3 ^= s1;\
    s1 ^= s2;\
    s0 ^= s3;\
    s2 ^= t;\
    s3 = rotl<uint64_t>(s3, 45);\
    return result;

#define XOROSHIRO128(A,B,C)\
    s1 ^= s0;\
    s0 = rotl<uint64_t>(s0, A) ^ s1 ^ (s1 << B);\
    s1 = rotl<uint64_t>(s1, C);\
    return result;

#define XORSHIFT64\
    s0 ^= s0 << 13;\
    s0 ^= s0 >> 7;\
    s0 ^= s0 << 17;\
    return s0;

#define XOSHIRO256_STATIC(FUNC)\
    static const uint64_t seed = uint64_t(FPRNG_SEED_INIT64);\
    static uint64_t s0 = splitMix64(seed), s1 = splitMix64(s0), s2 = splitMix64(s1), s3 = splitMix64(s2);\
    FUNC; XOSHIRO256

#define XOROSHIRO128_STATIC(FUNC, A, B, C)\
    static const uint64_t seed = uint64_t(FPRNG_SEED_INIT64);\
    static uint64_t s0 = splitMix64(seed), s1 = splitMix64(s0);\
    FUNC; XOROSHIRO128(A,B,C)

#define XORSHIFT64_STATIC\
    static uint64_t s0 = uint64_t(FPRNG_SEED_INIT64);\
    XORSHIFT64

// fastXS64
//
// 64bit pseudo-random generator
// All integer values are returned in interval [0, UINT64_MAX]
// to get values between [INT64_MIN, INT64_MAX] just cast result to int64_t
///////////////////////////////////////////////////////////////////////////////
class fastXS64
{
public:
    fastXS64(const uint64_t seedVal = uint64_t(FPRNG_SEED_INIT64)) { seed(seedVal); }

    inline uint64_t xoshiro256p()  { return xoshiro256(s0 + s3); }
    inline uint64_t xoshiro256pp() { return xoshiro256(rotl<uint64_t>(s0 + s3, 23) + s0); }
    inline uint64_t xoshiro256xx() { return xoshiro256(rotl<uint64_t>(s1 * 5, 7) * 9); }

    template <typename T> inline T xoshiro256p_UNI() { return T(        xoshiro256p())  * UNI_64BIT_INV; } // _UNI<T>   returns value in [ 0, 1] with T ==> float/double
    template <typename T> inline T xoshiro256p_VNI() { return T(int64_t(xoshiro256p())) * VNI_64BIT_INV; } // _VNI<T>   returns value in [-1, 1] with T ==> float/double
    template <typename T> inline T xoshiro256p_Range(T min, T max)                                         // _Range<T> returns value in [min, max] with T ==> float/double
            { return min + (max-min) * xoshiro256p_UNI<T>(); }

    inline uint64_t xoroshiro128p()  { return xoroshiro128(     s0 + s1); }
    inline uint64_t xoroshiro128pp() { return xoroshiro128(rotl<uint64_t>(s0 + s1, 17) + s0, 49, 21, 28); }
    inline uint64_t xoroshiro128xx() { return xoroshiro128(rotl<uint64_t>(s0 * 5, 7) * 9); }

    template <typename T> inline T xoroshiro128p_UNI() { return T(        xoshiro256p())  * UNI_64BIT_INV; } // _UNI<T>   returns value in [ 0, 1] with T ==> float/double
    template <typename T> inline T xoroshiro128p_VNI() { return T(int64_t(xoshiro256p())) * VNI_64BIT_INV; } // _VNI<T>   returns value in [-1, 1] with T ==> float/double
    template <typename T> inline T xoroshiro128p_Range(T min, T max)                                         // _Range<T> returns value in [min, max] with T ==> float/double
            { return min + (max-min) * xoroshiro128p_UNI<T>(); }

    inline uint64_t xorShift() { XORSHIFT64 } // Marsaglia xorShift: period 2^64-1

    template <typename T> inline T xorShift_UNI() { return         xorShift()  * UNI_64BIT_INV; } // _UNI<T>   returns value in [ 0, 1] with T ==> float/double
    template <typename T> inline T xorShift_VNI() { return int64_t(xorShift()) * VNI_64BIT_INV; } // _VNI<T>   returns value in [-1, 1] with T ==> float/double
    template <typename T> inline T xorShift_Range(T min, T max)                                   // _Range<T> returns value in [min, max] with T ==> float/double
            { return min + (max-min) * xorShift_UNI<T>(); }

    void seed(const uint64_t seedVal = uint64_t(FPRNG_SEED_INIT64)) {
        s0 = splitMix64(seedVal);
        s1 = splitMix64(s0);
        s2 = splitMix64(s1);
        s3 = splitMix64(s2);
    }

private:
    inline uint64_t xoshiro256(const uint64_t result)   { XOSHIRO256 }
    inline uint64_t xoroshiro128(const uint64_t result, const int A = 24, const int B = 16, const int C = 37) { XOROSHIRO128(A,B,C) }

protected:
    union {
        uint64_t s[4];
        struct { uint64_t s0, s1, s2, s3; };
    };

};
#ifdef SHOW_BETA_FEATURES
// fastXS64_mt - multi-threads - to generate non-overlapping subsequences
//
// contains jump and long_jump functions for fastXS64 xoshiro256 and
// xoroshiro128 generators, to avoid overlapping sequence for
// parallel / multi-threads computations
//
// For more informations: http://prng.di.unimi.it/
///////////////////////////////////////////////////////////////////////////////

class fastXS64_mt : public fastXS64
{
public:
    fastXS64_mt(const uint64_t seedVal = uint64_t(FPRNG_SEED_INIT64)) : fastXS64(seedVal) {}

/* This is the jump function for the generator. It is equivalent
   to 2^128 calls to next(); it can be used to generate 2^128
   non-overlapping subsequences for parallel computations. */

    void jump_xoshiro256p()  { jump256(&fastXS64::xoshiro256p , JUMP256); }
    void jump_xoshiro256pp() { jump256(&fastXS64::xoshiro256pp, JUMP256); }
    void jump_xoshiro256xx() { jump256(&fastXS64::xoshiro256xx, JUMP256); }

    void jump_xoroshiro128p()  { jump128(&fastXS64::xoroshiro128p , JUMP128); }
    void jump_xoroshiro128pp() { jump128(&fastXS64::xoroshiro128pp, JUMP128); }
    void jump_xoroshiro128xx() { jump128(&fastXS64::xoroshiro128xx, JUMP128); }

/* This is the long-jump function for the generator. It is equivalent to
   2^192 calls to next(); it can be used to generate 2^64 starting points,
   from each of which jump() will generate 2^64 non-overlapping
   subsequences for parallel distributed computations. */

    void long_jump_xoshiro256p()  { jump256(&fastXS64::xoshiro256p , LONG_JUMP256); }
    void long_jump_xoshiro256pp() { jump256(&fastXS64::xoshiro256pp, LONG_JUMP256); }
    void long_jump_xoshiro256xx() { jump256(&fastXS64::xoshiro256xx, LONG_JUMP256); }

    void long_jump_xoroshiro128p()  { jump128(&fastXS64::xoroshiro128p , LONG_JUMP128); }
    void long_jump_xoroshiro128pp() { jump128(&fastXS64::xoroshiro128pp, LONG_JUMP128); }
    void long_jump_xoroshiro128xx() { jump128(&fastXS64::xoroshiro128xx, LONG_JUMP128); }

private:

    void jump256(uint64_t (fastXS64::*func) (), const uint64_t *JUMP) {

        uint64_t c0 = 0, c1 = 0, c2 = 0, c3 = 0;

        for(int i = 0; i < sizeof JUMP256 / sizeof *JUMP256; i++)
            for(int b = 0; b < 64; b++) {
                if (JUMP256[i] & UINT64_C(1) << b) c0 ^= s0; c1 ^= s1; c2 ^= s2; c3 ^= s3;
                ((fastXS64 *)this->*func)();
            }
        s0 = c0, s1 = c1, s2 = c2, s3 = c3;
    }

    void jump128(uint64_t (fastXS64::*func) (), const uint64_t *JUMP) {

        uint64_t c0 = 0, c1 = 0;
        for(int i = 0; i < sizeof JUMP / sizeof *JUMP; i++)
            for(int b = 0; b < 64; b++) {
                if (JUMP[i] & UINT64_C(1) << b) c0 ^= s0, c1 ^= s1;
                ((fastXS64 *)this->*func)();
            }
        s0 = c0, s1 = c1;
    }

    const uint64_t      JUMP128[2] = { UINT64_C(0xdf900294d8f554a5), UINT64_C(0x170865df4b3201fc) };
    const uint64_t LONG_JUMP128[2] = { UINT64_C(0xd2a98b26625eee7b), UINT64_C(0xdddf9b1090aa7ac1) };
    const uint64_t      JUMP256[4] = { UINT64_C(0x180ec6d33cfd0aba), UINT64_C(0xd5a61266f0c9392c),
                                       UINT64_C(0xa9582618e03fc9aa), UINT64_C(0x39abdc4529b1661c) };
    const uint64_t LONG_JUMP256[4] = { UINT64_C(0x76e15d3efefdcbbf), UINT64_C(0xc5004e441c522fb3),
                                       UINT64_C(0x77710069854ee241), UINT64_C(0x39109bb02acbe635) };
};
#endif
// fastXS64s - static members
//      you can call directly w/o declaration, but..
//      N.B. all members/functions share same seed, and subsequents xor & shift
//           operations on it, if you need different seeds declare more
//           fastXS32 (non static) objects
//
// 64bit pseudo-random generator
// All integer values are returned in interval [0, UINT64_MAX]
// to get values between [INT64_MIN, INT64_MAX] just cast result to int64_t
///////////////////////////////////////////////////////////////////////////////
class fastXS64s
{
public:
    fastXS64s() = default;

    inline static uint64_t xoshiro256p()  { XOSHIRO256_STATIC(const uint64_t result = s0 + s3) }
    inline static uint64_t xoshiro256pp() { XOSHIRO256_STATIC(const uint64_t result = rotl<uint64_t>(s0 + s3, 23) + s0) }
    inline static uint64_t xoshiro256xx() { XOSHIRO256_STATIC(const uint64_t result = rotl<uint64_t>(s1 * 5, 7) * 9) }

    template <typename T> inline static T xoshiro256p_UNI() { return T(        xoshiro256p())  * UNI_64BIT_INV; } // _UNI<T>   returns value in [ 0, 1] with T ==> float/double
    template <typename T> inline static T xoshiro256p_VNI() { return T(int64_t(xoshiro256p())) * VNI_64BIT_INV; } // _VNI<T>   returns value in [-1, 1] with T ==> float/double
    template <typename T> inline static T xoshiro256p_Range(T min, T max)                                         // _Range<T> returns value in [min, max] with T ==> float/double
            { return min + (max-min) * xoshiro256p_UNI<T>(); }

    inline static uint64_t xoroshiro128p()  { XOROSHIRO128_STATIC(const uint64_t result =      s0 + s1,           24, 13, 27) }
    inline static uint64_t xoroshiro128pp() { XOROSHIRO128_STATIC(const uint64_t result = rotl<uint64_t>(s0 + s1, 17) + s0, 49, 21, 28) }
    inline static uint64_t xoroshiro128xx() { XOROSHIRO128_STATIC(const uint64_t result = rotl<uint64_t>(s0 * 5, 7) * 9,    24, 13, 27) }

    template <typename T> inline static T xoroshiro128p_UNI() { return T(        xoshiro256p())  * UNI_64BIT_INV; } // _UNI<T>   returns value in [ 0, 1] with T ==> float/double
    template <typename T> inline static T xoroshiro128p_VNI() { return T(int64_t(xoshiro256p())) * VNI_64BIT_INV; } // _VNI<T>   returns value in [-1, 1] with T ==> float/double
    template <typename T> inline static T xoroshiro128p_Range(T min, T max)                                         // _Range<T> returns value in [min, max] with T ==> float/double
            { return min + (max-min) * xoroshiro128p_UNI<T>(); }

    inline static uint64_t xorShift() { XORSHIFT64_STATIC } // Marsaglia xorShift: period 2^64-1

    template <typename T> inline static T xorShift_UNI() { return         xorShift()  * UNI_64BIT_INV; } // _UNI<T>   returns value in [ 0, 1] with T ==> float/double
    template <typename T> inline static T xorShift_VNI() { return int64_t(xorShift()) * VNI_64BIT_INV; } // _VNI<T>   returns value in [-1, 1] with T ==> float/double
    template <typename T> inline static T xorShift_Range(T min, T max)                                   // _Range<T> returns value in [min, max] with T ==> float/double
            { return min + (max-min) * xorShift_UNI<T>(); }
};

#undef XOSHIRO256
#undef XOROSHIRO128
#undef XORSHIFT64
#undef XOSHIRO256_STATIC
#undef XOROSHIRO128_STATIC
#undef XORSHIFT64_STATIC

/*--------------------------------------------------------------------------
 32bit PRNG Algorithms:

    znew / wnew / MWC / CNG / FIB / XSH / KISS

    LFIB4 / SWB (uncomment: #define FSTRND_USES_BUILT_TABLE below)

 Originally written from George Marsaglia
-------------------------------------------------------------------------- */
//#define FSTRND_USES_BUILT_TABLE   // uncomment to use 32Bit algorithms: LFI84 & SWB

// fastRandom32Class
//
// 32bit pseudo-random generator
// All integer values are returned in interval [0, UINT32_MAX]
// to get values between [INT32_MIN, INT32_MAX] just cast result to int32_t
///////////////////////////////////////////////////////////////////////////////
class fastRandom32Class
{
public:

    // no vaule, seed from system clock, or same seed for same sequence of numbers
    fastRandom32Class(const uint32_t seedVal = uint32_t(FPRNG_SEED_INIT32))
        { reset(); seed(seedVal); }

    // re-seed the current state/values with a new random values
    void seed(const uint32_t seed = uint32_t(FPRNG_SEED_INIT32)) {
        uint32_t s[6];
        s[0] = splitMix32(seed);
        for(int i=1; i<6; i++) s[i] = splitMix32(s[i-1]);
        initialize(s);
    }

    // reset to initial state
    void reset() {
        z   = UINT32_C(362436069); w     = UINT32_C(521288629);
        jsr = UINT32_C(123456789); jcong = UINT32_C(380116160);
        a   = UINT32_C(224466889); b     = UINT32_C(7584631);
    }

    inline uint32_t znew() { return z=36969*(z&65535)+(z>>16); }
    inline uint32_t wnew() { return w=18000*(w&65535)+(w>>16); }
    inline uint32_t MWC()  { return (znew()<<16)+wnew()      ; }
    inline uint32_t CNG()  { return jcong=69069*jcong+1234567; }
    inline uint32_t FIB()  { return (b=a+b),(a=b-a)          ; }
    inline uint32_t XSH()  { return jsr^=(jsr<<17), jsr^=(jsr>>13), jsr^=(jsr<<5); }

    inline uint32_t KISS() { return (MWC()^CNG())+XSH(); } // period 2^123

    template <typename T> inline T KISS_UNI() { return         KISS()  * UNI_32BIT_INV; } // _UNI<T>   returns value in [ 0, 1] with T ==> float/double
    template <typename T> inline T KISS_VNI() { return int32_t(KISS()) * VNI_32BIT_INV; } // _VNI<T>   returns value in [-1, 1] with T ==> float/double
    template <typename T> inline T KISS_Range(T min, T max)                               // _Range<T> returns value in [min, max] with T ==> float/double
            { return min + (max-min) * KISS_UNI<T>(); }

#ifdef FSTRND_USES_BUILT_TABLE
    inline uint32_t LFIB4() { return c++,t[c]=t[c]+t[uint8_t(c+58)]+t[uint8_t(c+119)]+t[uint8_t(c+178)];}
    inline uint32_t SWB()   { uint32_t bro; return c++,bro=(x<y),t[c]=(x=t[uint8_t(c+34)])-(y=t[uint8_t(c+19)]+bro); }
#endif
private:
    void initialize(const uint32_t *i) { z+=i[0]; w+=i[1]; jsr+=i[2]; jcong+=i[3]; a=+i[4]; b=+i[5]; }

    uint32_t z, w, jsr, jcong;
    uint32_t a, b;

#ifdef FSTRND_USES_BUILT_TABLE
    uint32_t t[256];
    unsigned char c=0;
#endif
};

#undef FSTRND_USES_BUILT_TABLE

/*--------------------------------------------------------------------------
 64bit PRNG Algorithms:

    znew / wnew / MWC / CNG / FIB / XSH / KISS

 Originally written from George Marsaglia
-------------------------------------------------------------------------- */


// fastRandom64Class
//
// 64bit pseudo-random generator
// All values are returned in interval [0, UINT64_MAX]
// to get values between [INT64_MIN, INT64_MAX] just cast result to int64_t
///////////////////////////////////////////////////////////////////////////////
class fastRandom64Class
{
public:
    // no vaule, seed from system clock, or same seed for same sequence of numbers
    fastRandom64Class(const uint64_t seedVal = uint64_t(FPRNG_SEED_INIT64)) { reset(); seed(seedVal);  }

    // re-seed the current state/values with a new random values
    void seed(const uint64_t seed = uint64_t(FPRNG_SEED_INIT64)) {
        uint64_t s[6];
        s[0] = splitMix64(seed);
        for(int i=1; i<6; i++) s[i] = splitMix64(s[i-1]);
        initialize(s);
    }
    // reset to initial state
    void reset() {
        x=uint64_t(UINT64_C(1234567890987654321)); c=uint64_t(UINT64_C(123456123456123456));
        y=uint64_t(UINT64_C(362436362436362436 )); z=uint64_t(UINT64_C(1066149217761810  ));
        a=uint64_t(UINT64_C(224466889));           b=uint64_t(UINT64_C(7584631));
    }

    inline uint64_t MWC() { uint64_t t; return t=(x<<58)+c, c=(x>>6), x+=t, c+=(x<t), x; }
    inline uint64_t CNG() { return z=UINT64_C(6906969069)*z+1234567;            }
    inline uint64_t XSH() { return y^=(y<<13), y^=(y>>17), y^=(y<<43);  }
    inline uint64_t FIB() { return (b=a+b),(a=b-a);                     }

    inline uint64_t KISS () { return MWC()+XSH()+CNG(); } //period 2^250

    template <typename T> inline T KISS_UNI() { return         KISS()  * UNI_64BIT_INV; } // _UNI<T>   returns value in [ 0, 1] with T ==> float/double
    template <typename T> inline T KISS_VNI() { return int64_t(KISS()) * VNI_64BIT_INV; } // _VNI<T>   returns value in [-1, 1] with T ==> float/double
    template <typename T> inline T KISS_Range(T min, T max)                               // _Range<T> returns value in [min, max] with T ==> float/double
            { return min + (max-min) * KISS_UNI<T>(); }

private:
    void initialize(const uint64_t *i){ x+=i[0]; y+=i[1]; z+=i[2]; c+=i[3]; a=+i[4]; b=+i[5]; }

    uint64_t x, c, y, z;
    uint64_t a, b;
};

using fastRand32 = fastRandom32Class;
using fastRand64 = fastRandom64Class;

} // end of namespace FstRnd

#undef UNI_32BIT_INV
#undef VNI_32BIT_INV
#undef UNI_64BIT_INV
#undef VNI_64BIT_INV
#undef FPRNG_SEED_INIT32
#undef FPRNG_SEED_INIT64

/*-----------------------------------------------------
    32bit Marsaglia algorithms description
-------------------------------------------------------
Write your own calling program and try one or more of
the above, singly or in combination, when you run a
simulation. You may want to change the simple 1-letter
names, to avoid conflict with your own choices. */

/* All that follows is comment, mostly from the initial
   post. You may want to remove it */


/* Any one of KISS, MWC, FIB, LFIB4, SWB, SHR3, or CONG
   can be used in an expression to provide a random 32-bit
   integer.


   The KISS generator, (Keep It Simple Stupid), is
   designed to combine the two multiply-with-carry
   generators in MWC with the 3-shift register SHR3 and
   the congruential generator CONG, using addition and
   exclusive-or. Period about 2^123.
   It is one of my favorite generators.


   The MWC generator concatenates two 16-bit multiply-
   with-carry generators, x(n)=36969x(n-1)+carry,
   y(n)=18000y(n-1)+carry mod 2^16, has period about
   2^60 and seems to pass all tests of randomness. A
   favorite stand-alone generator---faster than KISS,
   which contains it.


   FIB is the classical Fibonacci sequence
   x(n)=x(n-1)+x(n-2),but taken modulo 2^32.
   Its period is 3*2^31 if one of its two seeds is odd
   and not 1 mod 8. It has little worth as a RNG by
   itself, but provides a simple and fast component for
   use in combination generators.


   SHR3 is a 3-shift-register generator with period
   2^32-1. It uses y(n)=y(n-1)(I+L^17)(I+R^13)(I+L^5),
   with the y's viewed as binary vectors, L the 32x32
   binary matrix that shifts a vector left 1, and R its
   transpose. SHR3 seems to pass all except those
   related to the binary rank test, since 32 successive
   values, as binary vectors, must be linearly
   independent, while 32 successive truly random 32-bit
   integers, viewed as binary vectors, will be linearly
   independent only about 29% of the time.


   CONG is a congruential generator with the widely used 69069
   multiplier: x(n)=69069x(n-1)+1234567. It has period
   2^32. The leading half of its 32 bits seem to pass
   tests, but bits in the last half are too regular.


   LFIB4 is an extension of what I have previously
   defined as a lagged Fibonacci generator:
   x(n)=x(n-r) op x(n-s), with the x's in a finite
   set over which there is a binary operation op, such
   as +,- on integers mod 2^32, * on odd such integers,
   exclusive-or(xor) on binary vectors. Except for
   those using multiplication, lagged Fibonacci
   generators fail various tests of randomness, unless
   the lags are very long. (See SWB below).
   To see if more than two lags would serve to overcome
   the problems of 2-lag generators using +,- or xor, I
   have developed the 4-lag generator LFIB4 using
   addition: x(n)=x(n-256)+x(n-179)+x(n-119)+x(n-55)
   mod 2^32. Its period is 2^31*(2^256-1), about 2^287,
   and it seems to pass all tests---in particular,
   those of the kind for which 2-lag generators using
   +,-,xor seem to fail. For even more confidence in
   its suitability, LFIB4 can be combined with KISS,
   with a resulting period of about 2^410: just use
   (KISS+LFIB4) in any C expression.


   SWB is a subtract-with-borrow generator that I
   developed to give a simple method for producing
   extremely long periods:
   x(n)=x(n-222)-x(n-237)- borrow mod 2^32.
   The 'borrow' is 0, or set to 1 if computing x(n-1)
   caused overflow in 32-bit integer arithmetic. This
   generator has a very long period, 2^7098(2^480-1),
   about 2^7578. It seems to pass all tests of
   randomness, except for the Birthday Spacings test,
   which it fails badly, as do all lagged Fibonacci
   generators using +,- or xor. I would suggest
   combining SWB with KISS, MWC, SHR3, or CONG.
   KISS+SWB has period >2^7700 and is highly
   recommended.
   Subtract-with-borrow has the same local behaviour
   as lagged Fibonacci using +,-,xor---the borrow
   merely provides a much longer period.
   SWB fails the birthday spacings test, as do all
   lagged Fibonacci and other generators that merely
   combine two previous values by means of =,- or xor.
   Those failures are for a particular case: m=512
   birthdays in a year of n=2^24 days. There are
   choices of m and n for which lags >1000 will also
   fail the test. A reasonable precaution is to always
   combine a 2-lag Fibonacci or SWB generator with
   another kind of generator, unless the generator uses
   *, for which a very satisfactory sequence of odd
   32-bit integers results.


   The classical Fibonacci sequence mod 2^32 from FIB
   fails several tests. It is not suitable for use by
   itself, but is quite suitable for combining with
   other generators.


   The last half of the bits of CONG are too regular,
   and it fails tests for which those bits play a
   significant role. CONG+FIB will also have too much
   regularity in trailing bits, as each does. But keep
   in mind that it is a rare application for which
   the trailing bits play a significant role. CONG
   is one of the most widely used generators of the
   last 30 years, as it was the system generator for
   VAX and was incorporated in several popular
   software packages, all seemingly without complaint.


   Finally, because many simulations call for uniform
   random variables in 0<x<1 or -1<x<1, I use #define
   statements that permit inclusion of such variates
   directly in expressions: using UNI will provide a
   uniform random real (float) in (0,1), while VNI will
   provide one in (-1,1).


   All of these: MWC, SHR3, CONG, KISS, LFIB4, SWB, FIB
   UNI and VNI, permit direct insertion of the desired
   random quantity into an expression, avoiding the
   time and space costs of a function call. I call
   these in-line-define functions. To use them, static
   variables z,w,jsr,jcong,a and b should be assigned
   seed values other than their initial values. If
   LFIB4 or SWB are used, the static table t[256] must
   be initialized.


   A note on timing: It is difficult to provide exact
   time costs for inclusion of one of these in-line-
   define functions in an expression. Times may differ
   widely for different compilers, as the C operations
   may be deeply nested and tricky. I suggest these
   rough comparisons, based on averaging ten runs of a
   routine that is essentially a long loop:
   for(i=1;i<10000000;i++) L=KISS; then with KISS
   replaced with SHR3, CONG,... or KISS+SWB, etc. The
   times on my home PC, a Pentium 300MHz, in nanoseconds:
   FIB 49;LFIB4 77;SWB 80;CONG 80;SHR3 84;MWC 93;KISS 157;
   VNI 417;UNI 450;
*/


/*-----------------------------------------------------
    64bit Marsaglia algorithms description
-------------------------------------------------------

64-bit KISS RNGs
https://www.thecodingforums.com/threads/64-bit-kiss-rngs.673657/

Consistent with the Keep It Simple Stupid (KISS) principle,
I have previously suggested 32-bit KISS Random Number
Generators (RNGs) that seem to have been frequently adopted.

Having had requests for 64-bit KISSes, and now that
64-bit integers are becoming more available, I will
describe here a 64-bit KISS RNG, with comments on
implementation for various languages, speed, periods
and performance after extensive tests of randomness.

This 64-bit KISS RNG has three components, each nearly
good enough to serve alone. The components are:
Multiply-With-Carry (MWC), period (2^121+2^63-1)
Xorshift (XSH), period 2^64-1
Congruential (CNG), period 2^64

Compact C and Fortran listings are given below. They
can be cut, pasted, compiled and run to see if, after
100 million calls, results agree with that provided
by theory, assuming the default seeds.

Users may want to put the content in other forms, and,
for general use, provide means to set the 250 seed bits
required in the variables x,y,z (64 bits) and c (58 bits)
that have been given default values in the test versions.

The C version uses #define macros to enumerate the few
instructions that MWC, XSH and CNG require. The KISS
macro adds MWC+XSH+CNG mod 2^64, so that KISS can be
inserted at any place in a C program where a random 64-bit
integer is required.
Fortran's requirement that integers be signed makes the
necessary code more complicated, hence a function KISS().


64-bit xorShift
https://en.wikipedia.org/wiki/Xorshift

*/