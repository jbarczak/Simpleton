//=====================================================================================================================
//
//   OBJLoader.h
//
//   OBJ file loader.  
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2014 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================
#ifndef _OBJLOADER_H_
#define _OBJLOADER_H_

namespace Simpleton
{

    namespace OBJData
    {
        struct Material
        {
            // key/value pairs
        };

        struct VertexRef
        {
            unsigned int nPos;
            unsigned int nNormal;
            unsigned int nTexCoord;
        };

        struct OBJFace
        {
            unsigned int nMaterial;
            unsigned int nVertices;
            const VertexRef* pVertices;
        };


    }
}


#endif