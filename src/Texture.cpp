
//=====================================================================================================================
//
//   Texture.cpp
//
//   Assorted texture and image related functions
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2014 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#include "Texture.h"
#include "MiscMath.h"
#include "Types.h"
#include "Rand.h"
#include <math.h>

namespace Simpleton
{
    uint CountTextureMips( uint nTextureWidth, uint nTextureHeight, uint nTextureDepth )
    {
        uint nMips=0;
        nTextureWidth  = MAX(nTextureWidth,1);
        nTextureHeight = MAX(nTextureHeight,1);
        nTextureDepth  = MAX(nTextureDepth,1);
        while( nTextureWidth > 1 || nTextureHeight > 1 || nTextureDepth > 1 )
        {
            nTextureWidth >>= 1;
            nTextureHeight >>= 1;
            nTextureDepth >>= 1;
            nMips++;
            nTextureWidth  = MAX(nTextureWidth,1);
            nTextureHeight = MAX(nTextureHeight,1);
            nTextureDepth  = MAX(nTextureDepth,1);
        }
        return nMips;
    }

    uint CountTexturePixels( uint nTextureWidth, uint nTextureHeight, uint nTextureDepth, uint nMips )
    {
        if( !nMips )
            nMips = CountTextureMips(nTextureWidth,nTextureHeight,nTextureDepth);

        uint nTexels=0;
        nTextureWidth  = MAX(nTextureWidth,1);
        nTextureHeight = MAX(nTextureHeight,1);
        nTextureDepth  = MAX(nTextureDepth,1);
        while( nTextureWidth > 1 || nTextureHeight > 1 || nTextureDepth > 1 )
        {
            nTexels += nTextureWidth*nTextureHeight*nTextureDepth;
            nTextureWidth >>= 1;
            nTextureHeight >>= 1;
            nTextureDepth >>= 1;            
            nTextureWidth  = MAX(nTextureWidth,1);
            nTextureHeight = MAX(nTextureHeight,1);
            nTextureDepth  = MAX(nTextureDepth,1);
        }
        return nTexels;
    }

    /// Given a single RGB image, expand top level to RGBA and generate a mip chain
    void GenerateMips_RGB_To_RGBA( void* pOut, const void* pIn, uint nTopWidth, uint nTopHeight, uint nMipsToGenerate )
    {
        if( !nMipsToGenerate )
            nMipsToGenerate = CountTextureMips(nTopWidth,nTopHeight,1);
        
        const uint8* pInBytes = (const uint8*)pIn;
        uint8* pCurrentMip = (uint8*) pOut;
        for( uint y=0; y<nTopHeight; y++ )
        {
            for( uint x=0; x<nTopWidth; x++ )
            {
                pCurrentMip[0] = pInBytes[0];
                pCurrentMip[1] = pInBytes[1];
                pCurrentMip[2] = pInBytes[2];
                pCurrentMip[3] = 255;
                pCurrentMip += 4;
                pInBytes += 3;
            }
        }

        const uint8* pTopMip = (const uint8*)pOut;
        for( uint m=1; m<nMipsToGenerate; m++ )
        {
            uint nCurrentWidth  = MAX(1,nTopWidth>>1);
            uint nCurrentHeight = MAX(1,nTopHeight>>1);
            for( uint y=0; y<nCurrentHeight; y++ )
            {
                const uint8* pL0 = pTopMip;
                const uint8* pL1 = pTopMip+8*nCurrentWidth;
                for( uint x=0; x<nCurrentWidth; x++ )
                {
                    for( uint c=0; c<4; c++ )
                    {
                        uint v = pL0[8*x+c] + pL0[8*x+c+4] +
                                 pL1[8*x+c] + pL1[8*x+c+4];
                        *(pCurrentMip++) = v>>2;
                    }
                }
                
                pTopMip += 16*nCurrentWidth;
            }
            nTopWidth = nCurrentWidth;
            nTopHeight = nCurrentHeight;
        }
    }

 
    void GenerateMips_RGBA_InPlace( void* pInOut, uint nTopWidth, uint nTopHeight, uint nMipsToGenerate )
    {
        if( !nMipsToGenerate )
            nMipsToGenerate = CountTextureMips(nTopWidth,nTopHeight,1);
        
        uint8* pCurrentMip = ((uint8*)pInOut) + 4*nTopWidth*nTopHeight;
        const uint8* pTopMip = (const uint8*)pInOut;
        for( uint m=1; m<nMipsToGenerate; m++ )
        {
            uint nCurrentWidth  = MAX(1,nTopWidth>>1);
            uint nCurrentHeight = MAX(1,nTopHeight>>1);
            for( uint y=0; y<nCurrentHeight; y++ )
            {
                const uint8* pL0 = pTopMip;
                const uint8* pL1 = pTopMip+8*nCurrentWidth;
                for( uint x=0; x<nCurrentWidth; x++ )
                {
                    for( uint c=0; c<4; c++ )
                    {
                        uint v = pL0[8*x+c] + pL0[8*x+c+4] +
                                 pL1[8*x+c] + pL1[8*x+c+4];
                        *(pCurrentMip++) = v>>2;
                    }
                }
                
                pTopMip += 16*nCurrentWidth;
            }
            nTopWidth = nCurrentWidth;
            nTopHeight = nCurrentHeight;
        }
    }


    void CreateRandomRotations( int8* pOut, uint nWidth, uint nHeight )
    {
        uint nPix = nWidth*nHeight;
        for( uint i=0; i<nPix; i++ )
        {
            float t= Rand(-3.1415926,3.1415926);
            float cosT = cos(t);
            float sinT = sin(t);
            pOut[2*i]   = cosT*127;
            pOut[2*i+1] = sinT*127;
        }
    }
}