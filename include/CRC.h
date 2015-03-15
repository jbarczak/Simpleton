//=====================================================================================================================
//
//   CRC.h
//
//   CRC code
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2014 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#ifndef _CRC_H_
#define _CRC_H_

typedef unsigned int uint32;

namespace Simpleton
{
    uint32 crc32(uint32 crc, const void *buf, size_t size);
    uint32 crcOfString( const char* str );

    inline uint32 crc32( const void *buf, size_t size) { return crc32(0,buf,size); }

    template< class T >
    uint32 crcOf( const T& t )
    {
        return crc32(0,&t,sizeof(t));
    }
}

#endif