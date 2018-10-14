//=====================================================================================================================
//
//   Texture.h
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

#ifndef _TEXTURE_H_
#define _TEXTURE_H_

    typedef unsigned int uint;
typedef char int8;
namespace Simpleton
{

    uint CountTextureMips( uint nTextureWidth, uint nTextureHeight, uint nTextureDepth );

    uint CountTexturePixels( uint nTextureWidth, uint nTextureHeight, uint nTextureDepth, uint nMips );

    /// Given a single RGB image, expand top level to RGBA and generate a mip chain
    void GenerateMips_RGB_To_RGBA( void* pOut, const void* pIn, uint nTopWidth, uint nTopHeight, uint nMipsToGenerate );

    /// Given a full mip chain generate from top down in place
    void GenerateMips_RGBA_InPlace( void* pInOut, uint nTopWidth, uint nTopHeight, uint nMipsToGenerate );

    /// Generate a random rotation texture.  Output is a two channel 8-bit SNORM map containing cos(t),sin(t) in each pixel
    void CreateRandomRotations( int8* pOut, uint nWidth, uint nHeight );


    void Sobel3x3( float* pGradOut, const float* pHeightIn, float fScaleFactor, size_t width, size_t height );
}

#endif