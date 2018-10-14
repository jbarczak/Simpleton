//=====================================================================================================================
//
//   MiscMath.h
//
//   Assorted maths
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2010 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#ifndef _MISC_MATH_H_
#define _MISC_MATH_H_

#ifndef MAX
#define MAX(x,y) (((x)>(y)) ? (x) : (y))
#endif

#ifndef MIN
#define MIN(x,y) (((x)<(y)) ? (x) : (y))
#endif


#ifndef SWAP
#define SWAP(x,y) { auto tmp = x; x = y; y = tmp; }
#endif

namespace Simpleton
{
    inline unsigned int RoundUpPow2( unsigned int  n, unsigned int p ) {  return (n + (p-1)) & ~(p-1); }
    
    inline float Radians( float degrees ) { return (3.1415926f/180.0f)*degrees; }


}

#endif
