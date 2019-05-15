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
//
//  From George Marsaglia Algorithms
//
////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <stdint.h>
#include <chrono>
#include <type_traits>
#include <random>

namespace fstRnd {

//#define FSTRND_USES_BUILT_TABLE   // uncomment to use 32Bit algorithms: LFI84 & SWB 

//
// fastRandom32Class 
//
// 32bit pseudo-random generator
// All values are returned in interval [0, UINT32_MAX] 
// to get values between [INT32_MIN, INT32_MAX] just cast result to int32_t
///////////////////////////////////////////////////////////////////////////////
class fastRandom32Class 
{
public:
    fastRandom32Class()
    {
 // obtain a seed from the system clock:
        rnd32.seed(uint32_t(std::chrono::system_clock::now().time_since_epoch().count()));

        initialize(rnd32(), rnd32(), rnd32(),
                   rnd32(), rnd32(), rnd32());
    }

    inline uint32_t znew() { return z=36969*(z&65535)+(z>>16); }
    inline uint32_t wnew() { return w=18000*(w&65535)+(w>>16); }
    inline uint32_t MWC()  { return (znew()<<16)+wnew()      ; }
    inline uint32_t CNG()  { return jcong=69069*jcong+1234567; }
    inline uint32_t FIB()  { return (b=a+b),(a=b-a)          ; }
    inline uint32_t XSH()  { return jsr^=(jsr<<17), jsr^=(jsr>>13), jsr^=(jsr<<5); }

    inline uint32_t KISS() { return (MWC()^CNG())+XSH(); } // period 2^123

#ifdef FSTRND_USES_BUILT_TABLE
    inline uint32_t LFIB4() { return c++,t[c]=t[c]+t[uint8_t(c+58)]+t[uint8_t(c+119)]+t[uint8_t(c+178)];}
    inline uint32_t SWB()   { uint32_t bro; return c++,bro=(x<y),t[c]=(x=t[uint8_t(c+34)])-(y=t[uint8_t(c+19)]+bro); }
#endif

    inline static uint32_t xorShift() //period 2^32-1
    {
        // Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" 
        static uint32_t state = uint32_t(std::chrono::system_clock::now().time_since_epoch().count());
        uint32_t x = state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;        
        return state = x;
    }

    int sizeOfGenerator() { return sizeof(uint32_t); }

private:

    void initialize(uint32_t i1,uint32_t i2,uint32_t i3,uint32_t i4,uint32_t i5, uint32_t i6)
    { 
        z+=i1; w+=i2; jsr+=i3; jcong+=i4; a=+i5; b=+i6;
#ifdef FSTRND_USES_BUILT_TABLE
        for(int i=0;i<256;i=i+1) t[i]=rnd32();
#endif
    }

    std::mt19937 rnd32;

    uint32_t z=362436069, w=521288629, jsr=123456789, jcong=380116160;
    uint32_t a=224466889, b=7584631;
    uint32_t x=0,y=0; 

#ifdef FSTRND_USES_BUILT_TABLE
    uint32_t t[256];
    unsigned char c=0;
#endif
};

//
// fastRandom64Class 
//
// 64bit pseudo-random generator
// All values are returned in interval [0, UINT64_MAX] 
// to get values between [INT64_MIN, INT64_MAX] just cast result to int64_t
///////////////////////////////////////////////////////////////////////////////
class fastRandom64Class 
{

public:
    fastRandom64Class()
    {
 // obtain a seed from the system clock:
        rnd64.seed(uint64_t(std::chrono::system_clock::now().time_since_epoch().count()));

        initialize(rnd64(), rnd64(), rnd64(), 
                   rnd64(), rnd64(), rnd64());
    }

    inline uint64_t MWC() { uint64_t t; return t=(x<<58)+c, c=(x>>6), x+=t, c+=(x<t), x; }
    inline uint64_t CNG() { return z=6906969069LL*z+1234567;            }
    inline uint64_t XSH() { return y^=(y<<13), y^=(y>>17), y^=(y<<43);  }
    inline uint64_t FIB() { return (b=a+b),(a=b-a);                     }

    inline uint64_t KISS () { return MWC()+XSH()+CNG(); } //period 2^250

    inline static uint64_t xorShift() //period 2^64-1
    {
        // Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" 
        static uint64_t state = uint64_t(std::chrono::system_clock::now().time_since_epoch().count());
        uint64_t x = state;
        x ^= x << 13;
        x ^= x >> 7;
        x ^= x << 17;
        return state = x;
    }

    int sizeOfGenerator() { return sizeof(uint64_t); }
    
private:

    void initialize(uint64_t i1, uint64_t i2, uint64_t i3, uint64_t i4, uint64_t i5, uint64_t i6)
    { x+=i1; y+=i2; z+=i3; c+=i4; a=+i5; b=+i6; }

    std::mt19937_64 rnd64;

    uint64_t x=uint64_t(1234567890987654321ULL),c=uint64_t(123456123456123456ULL),
             y=uint64_t(362436362436362436ULL ),z=uint64_t(1066149217761810ULL  );
    uint64_t a=224466889,b=7584631;
};

using fastRand32 = fastRandom32Class;
using fastRand64 = fastRandom64Class;


template <typename fT, typename classT> class floatfastRandomClass
{
public:
    floatfastRandomClass() : inv_2uiT_max(inv_uiT_max * fT(2.0)),
                             inv_uiT_max (std::is_same<fastRand32, classT>::value ? 
                                          fT(2.3283064365386962890625e-10) : 
                                          fT(5.4210108624275221700372640043497e-20))
    {}
    
    // period 2^123 / 2^250 (32/64 bit)
    inline fT UNI() { return fT(          fastRandom.KISS())  * inv_uiT_max;  } // return [ 0.0, 1.0]
    inline fT VNI() { return fT(((signed) fastRandom.KISS())) * inv_2uiT_max; } // return [-1.0, 1.0]
    
    // fastest but with period 2^32-1 / 2^64-1 (32/64 bit generator)
    inline fT xshUNI() { return fT(          classT::xorShift() ) * inv_uiT_max;  }
    inline fT xshVNI() { return fT(((signed) classT::xorShift())) * inv_2uiT_max; }

    inline fT range(fT min, fT max) { return min + (max-min) * UNI(); }

    inline auto KISS()     { return fastRandom.KISS();  }
    inline auto xorShift() { return classT::xorShift(); }

    int sizeOfGenerator() { return fastRandom.sizeOfInBits(); }
    int sizeOfPrecision() { return sizeof(fT);                }

private:
    const fT inv_uiT_max;
    const fT inv_2uiT_max;
    classT fastRandom;
};

// single precision interface for 32 bit generator
using fFastRand32 = floatfastRandomClass<float,  fastRand32>;
// double precision interface for 32 bit generator
using dFastRand32 = floatfastRandomClass<double, fastRand32>;

// single precision interface for 64 bit generator
using fFastRand64 = floatfastRandomClass<float,  fastRand64>;
// double precision interface for 64 bit generator
using dFastRand64 = floatfastRandomClass<double, fastRand64>;

}; // end of namespace FstRnd 




/*-----------------------------------------------------
    32bit algorithms
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
    64bit algorithms
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