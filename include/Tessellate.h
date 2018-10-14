//=====================================================================================================================
//
//   Tessellate.h
//
//   Various triangulators
//
//   The lazy man's utility library
//   Joshua Barczak
//   Copyright 2014 Joshua Barczak
//
//   LICENSE:  See Doc\License.txt for terms and conditions
//
//=====================================================================================================================

#ifndef _TESSELLATE_H_
#define _TESSELLATE_H_

#include "VectorMath.h"
#include "Matrix.h"

#include <vector>

typedef unsigned int uint;

namespace Simpleton
{
    
    struct TessVertex
    {
        Vec3f vPos;
        Vec3f vNormal;
        Vec2f vUV;
    };

    void CreateXYQuad( );

    void CreateCylinder( uint nRadialSplits, uint nAxialSegments, std::vector<TessVertex>& vb, std::vector<uint>& ib );
    void CreateCylinder( const Vec3f& p0, const Vec3f& p1, float radius, uint nRadialSplits, uint nAxialSegments, std::vector<TessVertex>& vb, std::vector<uint>& ib  );
    
    void CreateEllipsoid( float a, float b, float c, uint nRadialSplits, uint nAxialSegments, std::vector<TessVertex>& vb, std::vector<uint>& ib );

    void TessellateBezier( uint nLevel, const Vec3f* pControlVertices, const int* pPatchIndices, 
                       std::vector<TessVertex>& rVerts, std::vector<uint>& rIndicesOut );

    void AffineTransformMesh( std::vector<TessVertex>& verts, const Matrix4f& M, const Matrix4f& MInv );

    void GenerateCatmullRohmTangents( Vec2f* pTangents, const Vec2f* pCVs, uint nCVs );    

    void SampleCatmullRohmCurve( float* pX, float* pY, uint nSamples, const Vec2f* pCVs, const Vec2f* pTangents, uint nCVs );   
  
    void SampleCatmullRohmCurve( Vec2f* pPositions, 
                                 Vec2f* pNormals, 
                                 uint nSamples, 
                                 const Vec2f* pCVs, 
                                 const Vec2f* pTangents,
                                 uint nCVs );


    /// Revolves a piecewise linear curve around the Z axis
    void TessellateRevolvedCurve( const Vec2f* pProfileVerts, 
                           const Vec2f* pProfileNormals,
                          uint nProfileVerts,
                          uint nRadialSplits,
                          std::vector<TessVertex>& vb,
                          std::vector<uint>& ib );

    /// Produce a Utah teapot
    void TessellateTeapot( uint nLevel,
                           std::vector<TessVertex>& vb,
                           std::vector<uint>& ib );
}


#endif
