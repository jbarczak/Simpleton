
#include <vector>
#include "Tessellate.h"
#include "Matrix.h"
#include "Types.h"

#include "NewellTeasetData.h"

namespace Simpleton
{
    namespace _INTERNAL
    {
        static void GenerateCylinderConnectivity( uint nRadialSegs, uint nAxialSegs, uint* pOutput )
        {
            for( uint i=0; i<nAxialSegs; i++ )
            {            
                // Cheatsheet, for axial==2 (3 rows of verts, 2 segments)
                //  x   2
                //  |  
                //  x   1
                //  |
                //  x   0
                //
                uint r0 = i     * nRadialSegs;
                uint r1 = (i+1) * nRadialSegs;
                for( uint j=0; j<nRadialSegs; j++ )
                {
                    // Cheatsheet: vertex ring connectivity for radial==4
                    //     3
                    //    / \
                    //   0   2
                    //    \ /
                    //     1
                    //
                   
                    // Quad vertex numbering: (clockwise winding)
                    // 2   3    r1   ^
                    //               |  cylinder axis 
                    // 0   1    r0   |
                    //
                    uint v0 = r0 + j;
                    uint v1 = r0 + (j+1)%nRadialSegs;
                    uint v2 = r1 + j;
                    uint v3 = r1 + (j+1)%nRadialSegs;
                    pOutput[0] = v0;
                    pOutput[1] = v2;
                    pOutput[2] = v3;
                    pOutput[3] = v0;
                    pOutput[4] = v3;
                    pOutput[5] = v1;
                    pOutput += 6;
                }
            }
        }

    }

        
    void CreateCylinder( uint nRadialSplits, uint nAxialSegments, std::vector<TessVertex>& vb, std::vector<uint>& ib )
    {
        const float PI = 3.1415926f;
        uint nRadialSegments = (4*nRadialSplits+1);
        uint nTotalVerts   = (nAxialSegments+1)*nRadialSegments;
        uint nTotalIndices = nAxialSegments*6*nRadialSegments;
        
        vb.resize( nTotalVerts );
        ib.resize( nTotalIndices );

        // generate verts
        float v=0;
        float du = 1.0f / nRadialSegments;
        float dv = 1.0f / nAxialSegments;
        uint nVerts=0;
        for( uint nu =0; nu <= nAxialSegments; nu++ )
        {
            float u=0;
            for( uint nv = 0; nv < nRadialSegments; nv++ )
            {
                float fCos = cosf(u*2.0f*PI);
                float fSin = sinf(u*2.0f*PI);
                float x = fCos;
                float y = fSin;
                float z = v;

                vb[nVerts].vPos    = Vec3f(x,y,z);
                vb[nVerts].vNormal = Vec3f(x,y,0);
                vb[nVerts].vUV     = Vec2f(u,v);
                nVerts++;
                u += du;
            }
            v += dv;
        }

        // generate indices
        _INTERNAL::GenerateCylinderConnectivity( nRadialSegments, nAxialSegments, ib.data() );
    }

    void CreateCylinder( const Vec3f& p0, const Vec3f& p1, float radius, uint nRadialSplits, uint nAxialSegments, std::vector<TessVertex>& vb, std::vector<uint>& ib  )
    {
        Vec3f N = (p1-p0);
        Vec3f T,B;
        BuildTangentFrame(N,T,B);
        Vec3f Tn = Normalize3(T);
        Vec3f Bn = Normalize3(B);
        T = Tn*radius;
        B = Bn*radius;

        CreateCylinder( nRadialSplits, nAxialSegments, vb, ib );

        for( uint i=0; i<vb.size(); i++ )
        {
            Vec3f P = vb[i].vPos;
            vb[i].vPos    = (B  * P.x) + (T  * P.y) + (N * P.z) + p0;
            vb[i].vNormal = Bn * vb[i].vNormal.x + Tn * vb[i].vNormal.y;
        }
    }


    void CreateEllipsoid( float a, float b, float c, uint nRadialSplits, uint nAxialSegments, std::vector<TessVertex>& vb, std::vector<uint>& ib )
    {
        const float PI = 3.1415926f;
        uint nRadialSegments = (4*nRadialSplits+1);
        uint nTotalVerts   = (nAxialSegments+1)*nRadialSegments;
        uint nTotalIndices = nAxialSegments*6*nRadialSegments;
        
        vb.resize( nTotalVerts );
        ib.resize( nTotalIndices );

        // generate verts
        float u=0;
        float du = 1.0f / nAxialSegments;
        float dv = 1.0f / nRadialSegments;
        uint nVerts=0;
        for( uint nu =0; nu <= nAxialSegments; nu++ )
        {
            float v=0;
            for( uint nv = 0; nv < nRadialSegments; nv++ )
            {
                float theta = u*PI;
                float phi = v*2*PI;

                float cosT = cosf(theta);
                float sinT = sinf(theta);
                float cosP = cosf(phi);
                float sinP = sinf(phi);
                 
                float x = a*sinT*cosP;
                float y = b*sinT*sinP;
                float z = c*cosT;

                float nx = x / a;
                float ny = y / b;
                float nz = z / c;
                float nrm = 1.0f / sqrt(nx*nx + ny*ny +nz*nz);
                nx /= nrm;
                ny /= nrm;
                nz /= nrm;

                vb[nVerts].vPos    = Vec3f(x,y,z);
                vb[nVerts].vNormal = Vec3f(nx,ny,nz);
                vb[nVerts].vUV     = Vec2f(u,v);
                nVerts++;
                v += dv;
            }
            u += du;
        }

        // generate indices
        _INTERNAL::GenerateCylinderConnectivity( nRadialSegments, nAxialSegments, ib.data() );
    }

#if 0
    float BezierBasis(float u, int i)
    {
        switch(i) 
        {
        case 0:  return pow(1.0f-u,3.0f);      // 
        case 1:  return 3*u*pow(1.0f-u,2.0f);  // 3U( 1 - 2U + U^2 ) --> 3U - 6U^2 + 3U^3
        case 2:  return 3*pow(u,2.0f)*(1.0f-u);
        case 3:  return pow(u,3.0f);
        }
    }

    float BezierBasisDeriv( float u, int i )
    {
        switch(i)
        {
        case 0:  return -3.0f*pow(u,2.0f) + 6.0f*u - 3;
        case 1:  return 3.0f - 12.0f*u + 9.0f*pow(u,2.0f);
        case 2:  return 6*u - 9*pow(u,2.0f);
        case 3:  return 3*pow(u,2.0f);
        }
    }
#endif

    static void BezierBasis( float* pBasis, float* pDerivative, float u )
    {
        float inv_u      = 1.f-u;
        float inv_u_sq   = inv_u*inv_u;
        float three_u    = 3.f*u;
        float three_u_u  = 3.f*u*u;
        pBasis[0] = inv_u_sq*inv_u;
        pBasis[1] = three_u*inv_u_sq;
        pBasis[2] = three_u_u*inv_u;
        pBasis[3] = u*u*u;

        float six_u = 6.f*u;
        float nine_u_u = 9.f*u*u;
        pDerivative[0] = six_u - 3.f - three_u_u;
        pDerivative[1] = 3.f   - 12.f*u + nine_u_u;
        pDerivative[2] = six_u - nine_u_u;
        pDerivative[3] = three_u_u;

    }

    void TessellateBezier( uint nLevel, const Vec3f* pControlVertices, const int* pPatchIndices, 
                           std::vector<TessVertex>& rVerts, std::vector<uint>& rIndicesOut )
    {
        uint nFirstVertex = rVerts.size();

        uint nVertices = 2+nLevel;
        float fDelta = 1.0f / (nVertices-1);
        float fV=0;

        float BasisV[4];
        float BasisVDeriv[4];
        float BasisU[4];
        float BasisUDeriv[4];

        for( uint i=0; i<nVertices; i++ )
        {
            float fU=0;
            BezierBasis( BasisV, BasisVDeriv, fV );

            for( uint j=0; j<nVertices; j++ )
            {
                BezierBasis( BasisU, BasisUDeriv, fU );
                Vec3f vPos(0,0,0);
                Vec3f vDU(0,0,0);
                Vec3f vDV(0,0,0);

                for(int i=0; i<4; i++)
                {
	                for(int j=0; j<4; j++)
                    {
	                    vPos += pControlVertices[ pPatchIndices[ (4*j)+i ] ]*(BasisU[i] * BasisV[j]);
	                    vDU  += pControlVertices[ pPatchIndices[ (4*j)+i ] ]*(BasisUDeriv[i] * BasisV[j]);
	                    vDV  += pControlVertices[ pPatchIndices[ (4*j)+i ] ]*(BasisU[i] *BasisVDeriv[j]);
                    }
                }
            
                TessVertex v;
                v.vPos = vPos;
                v.vNormal = Normalize3(Cross3(vDU,vDV));
                v.vUV = Vec2f(fU,fV);
                rVerts.push_back(v);
    
                fU += fDelta;
            }
            fV += fDelta;
        }

        for( uint i=0; i<nVertices-1; i++ )
        {
            for( uint j=0; j<nVertices-1; j++ )
            {
                uint nV0 = nFirstVertex + (nVertices*j) + i;
                uint nV1 = nV0 + 1;
                uint nV2 = nV0 + nVertices;
                uint nV3 = nV2 + 1;

                rIndicesOut.push_back( nV2 );
                rIndicesOut.push_back( nV1 );
                rIndicesOut.push_back( nV0 );
            
                rIndicesOut.push_back( nV2 );
                rIndicesOut.push_back( nV3 );
                rIndicesOut.push_back( nV1 );
            }
        }
    }

    void AffineTransformMesh( std::vector<TessVertex>& verts, const Simpleton::Matrix4f& M, const Simpleton::Matrix4f& MInv )
    {
        for( uint i=0; i<verts.size(); i++ )
        {
            verts[i].vPos = AffineTransformPoint(M,verts[i].vPos);
            verts[i].vNormal = AffineTransformNormal(MInv,verts[i].vNormal);
            verts[i].vNormal = Normalize3(verts[i].vNormal);
        }
    }

    void GenerateCatmullRohmTangents( Vec2f* pTangents, const Vec2f* pCVs,  uint nCVs )
    {
        if( nCVs < 2 )
            return;

        pTangents[0] = pCVs[1] - pCVs[0];
        pTangents[nCVs-1] = pCVs[nCVs-1] - pCVs[nCVs-2];
        for( uint i=1; i<nCVs-1; i++ )
            pTangents[i] = (pCVs[i+1]-pCVs[i-1])*0.5f;
    
    }


    void SampleCatmullRohmCurve( float* pX, float* pY, uint nSamples, const Vec2f* pCVs, const Vec2f* pTangents, uint nCVs )    
    {
        if( nCVs < 3 || nSamples <= 1 )
            return;

        uint nSegs = nSamples-1;
        float T=0;
        float TMax = (float)(nCVs-1);
        float dt = TMax / (float)nSegs;
        uint s=0;
        while( s < nSamples && T < TMax )
        {
            float lower = floorf(T);
            float upper = lower+1.0f;
            
            // increment curve parameter until we reach the end of the current segment
            uint s0  = s;
            float t0 = T;
            do
            {
                T += dt;
                s++;
            } while( s < nSamples && T <= upper );

            // get the control points of current segment
            uint i = (uint)lower;
            Vec2f v0 = pCVs[i];
            Vec2f v1 = pCVs[i+1];
            Vec2f m0 = pTangents[i];
            Vec2f m1 = pTangents[i+1];

            // tessellate
            while( s0 < s )
            {
                float t = t0 - lower;
                float t2 = t*t;
                float t3 = t2*t;

                float a = 2.0f*t3 - 3.0f*t2 + 1;
                float b = t3 - 2.0f*t2 + t;
                float c = 3.0f*t2 - 2.0f*t3;
                float d = t3-t2;
                Vec2f v = v0*a + m0*b + v1*c + m1*d;
                pX[s0] = v.x;
                pY[s0] = v.y;
                s0++;
                t0 += dt;
            }
        }

        // numerics being what they are, sometimes we'll run a little off the end
        while( s < nSamples )
        {
            pX[s] = pCVs[nCVs-1].x;
            pY[s] = pCVs[nCVs-1].y;
            s++;
        }
    }

    void SampleCatmullRohmCurve( Vec2f* pPositions, 
                                 Vec2f* pNormals, 
                                 uint nSamples, 
                                 const Vec2f* pCVs, 
                                 const Vec2f* pTangents,
                                 uint nCVs )
    {
        if( nCVs < 3 )
            return;

        uint nSegs = nCVs-1;
    
        float t=0;
        float dt = nSegs / (float)nSamples;

        // Borrow the output vertex array temporarily and generate
        //  segment index and local T value for each sample vert
        struct SampleInfo
        {
            uint32 nSegment;
            float fLocalT;
        };

        static_assert( sizeof(SampleInfo) >= sizeof(Vec2f), "Insane compiler" );

        SampleInfo* pSampleInfo = reinterpret_cast<SampleInfo*>( pPositions );
        for( uint i=0; i<nSamples; i++ )
        {
            float f = floor(t);
            pSampleInfo[i].fLocalT  = t - f;
            pSampleInfo[i].nSegment = f;
            t += dt;
        }

    
        if( pNormals )
        {
            // evaluate normals
            uint i=0;
            while( i < nSamples )
            {
                // skip run of sample points in same curve segment
                uint i0 = i;
                uint s = pSampleInfo[i].nSegment;
                do
                {
                    i++;
                } while( i < nSamples && pSampleInfo[i].nSegment == s );

                // derive hermite control values for this curve segment
                Vec2f v0 = pCVs[s];
                Vec2f v1 = pCVs[s+1];
                Vec2f m0 = pTangents[s];
                Vec2f m1 = pTangents[s+1];

                // evaluate all the normals for this segment
                while( i0 < i )
                {
                    float t  = pSampleInfo[i0].fLocalT;
                    float t2 = t*t;
                    float da = 6*(t2-t);
                    float db = 3*t2 - 4*t + 1;
                    float dc = 6*(t-t2);
                    float dd = 3*t2 - 2.0f*t;
                    pNormals[i0++] = v0*da + m0*db + v1*dc + m1*dd; 
                }            
            }

            // normalize them
            for( i=0; i<nSamples; i++ )
            {
                float x = pNormals[i].x;
                float y = pNormals[i].y;
                float d = sqrtf( x*x + y*y );
                x /= d;
                y /= d;
                pNormals[i].x = y;
                pNormals[i].y = -x;
                
            }
        }
    
        // evaluate positions
        {
            uint i=0;
            while( i < nSamples )
            {
                // skip run of sample points in same curve segment
                uint i0 = i;
                uint s = pSampleInfo[i].nSegment;
                do
                {
                    i++;
                } while( i < nSamples && pSampleInfo[i].nSegment == s );

                // derive hermite control values for this curve segment
                Vec2f v0 = pCVs[s];
                Vec2f v1 = pCVs[s+1];
                Vec2f m0 = pTangents[s];
                Vec2f m1 = pTangents[s+1];

                // evaluate all the positions for this segment
                while( i0 < i )
                {
                    float t = pSampleInfo[i0].fLocalT;
                    float t2 = t*t;
                    float t3 = t2*t;

                    float a = 2.0f*t3 - 3.0f*t2 + 1;
                    float b = t3 - 2.0f*t2 + t;
                    float c = 3.0f*t2 - 2.0f*t3;
                    float d = t3-t2;
                    pPositions[i0++] = v0*a + m0*b + v1*c + m1*d;
                }            
            }
        }
    }


    void TessellateRevolvedCurve( const Vec2f* pProfileVerts, 
                           const Vec2f* pProfileNormals,
                          uint nProfileVerts,
                          uint nRadialSplits,
                          std::vector<TessVertex>& vb,
                          std::vector<uint>& ib )
    {
        const float PI = 3.1415926f;
        uint nAxialSegments = nProfileVerts-1;
        uint nRadialSegments = (4*nRadialSplits+1);
        uint nTotalVerts   = (nAxialSegments+1)*nRadialSegments;
        uint nTotalIndices = nAxialSegments*6*nRadialSegments;
        
        vb.resize( nTotalVerts );
        ib.resize( nTotalIndices );

        // generate verts
        float v=0;
        float du = 1.0f / nRadialSegments;
        float dv = 1.0f / nAxialSegments;
        uint nVerts=0;
        for( uint i =0; i <= nAxialSegments; i++ )
        {
            float px = pProfileVerts[i].x;
            float py = pProfileVerts[i].y;
            float nx = pProfileNormals[i].x;
            float ny = pProfileNormals[i].y;
            float u=0;
            for( uint j = 0; j < nRadialSegments; j++ )
            {
                float fCos = cosf(u*2.0f*PI);
                float fSin = sinf(u*2.0f*PI);
                float x =  fCos*px;
                float y = fSin*px;
                float z = py;
                float Nx =  fCos*nx;
                float Ny = fSin*nx;
                float Nz = ny;
                vb[nVerts].vPos    = Vec3f(x,y,z);
                vb[nVerts].vNormal = Vec3f(Nx,Ny,Nz);
                vb[nVerts].vUV     = Vec2f(u,v);
                nVerts++;
                u += du;
            }
            v += dv;
        }

        // generate indices
        _INTERNAL::GenerateCylinderConnectivity( nRadialSegments, nAxialSegments, ib.data() );
    }

    void TessellateTeapot( uint nLevel, std::vector<TessVertex>& vb, std::vector<uint>& ib )
    {
        for( uint i=0; i<32; i++ )
        {
            const int* pPatch = newellTeapotPatch[i];
            int patch[16];
            memcpy(patch,pPatch,sizeof(int)*16);
            for( int j=0; j<16; j++ )
                patch[j]--; // fix indices

            const Simpleton::Vec3f* pVec = (const Simpleton::Vec3f*)newellTeapotVertices;
            Simpleton::TessellateBezier( nLevel, pVec, patch,
                                        vb,ib );
        }
    }
}

